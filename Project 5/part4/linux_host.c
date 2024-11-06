#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#define ARDUINO_1_PORT "/dev/ttyUSB0"  //  potentiometer
#define ARDUINO_2_PORT "/dev/ttyUSB1"  // motor control
#define LOG_FILE "../logs/control_log.csv"
#define CONTROL_RATE_DELAY_US 434      // Delay to achieve 2304 messages per sec

// CRC calculation for Modbus RTU
uint16_t ModRTU_CRC(uint8_t buf[], int len) {
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++) {
        crc ^= (uint16_t)buf[pos];          // XOR byte into least sig. byte of crc
        for (int i = 8; i != 0; i--) {      // Loop over each bit
            if ((crc & 0x0001) != 0) {      // If the LSB is set
                crc >>= 1;                  // Shift right and XOR 0xA001
                crc ^= 0xA001;
            } else {                        // Else LSB is not set
                crc >>= 1;                  // Just shift right
            }
        }
    } 
    // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
    return crc;
}

int open_serial_port(const char* port_name) {
    int file = open(port_name, O_RDWR | O_NOCTTY);
    if (file < 0) {
        perror("Failed to open the serial port");
    }
    return file;
}

void configure_serial_port(int file) {
    struct termios options;
    tcgetattr(file, &options);
    options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(file, TCIFLUSH);
    tcsetattr(file, TCSANOW, &options);
}

// Send Modbus request to Arduino
void send_modbus_request(int file, uint8_t slave_id, uint8_t command, uint16_t value) {
    uint8_t msg[8];
    msg[0] = slave_id;          // Slave ID
    msg[1] = 0x06;              // Function Code (Preset Single Register)
    msg[2] = 0x00;
    msg[3] = 0x01;              // Register Address
    msg[4] = (value >> 8) & 0xFF;  // High byte of value
    msg[5] = value & 0xFF;         // Low byte of value
    uint16_t crc = ModRTU_CRC(msg, 6);
    msg[6] = crc & 0xFF;        // CRC Low byte
    msg[7] = crc >> 8;          // CRC High byte

    write(file, msg, 8);
    tcdrain(file);  // Ensure all data has been sent
}

// Read Modbus response from Arduino
int read_modbus_response(int file, uint8_t *buffer, int buffer_size) {
    int count = read(file, buffer, buffer_size);
    if (count <= 0) {
        printf("No response or timeout.\n");
        return -1;
    }
    return count;
}

// Request data from Arduino 1
int request_data_from_arduino_1(int file) {
    uint8_t response[8];
    send_modbus_request(file, 0x01, 0x00, 0x0000);  // Command Arduino 1 to send sensor data

    int count = read_modbus_response(file, response, sizeof(response));
    if (count > 0 && response[0] == 0x01 && response[1] == 0x06) {
        int raw_value = (response[4] << 8) | response[5];
        printf("Raw value from Arduino 1: %d\n", raw_value);
        return raw_value;
    }
    return -1;
}

// Send scaled value to Arduino 2
void send_data_to_arduino_2(int file, int pwm_value) {
    send_modbus_request(file, 0x02, 0x06, pwm_value);  // Command Arduino 2 to set PWM
    printf("Sending PWM value %d to Arduino 2...\n", pwm_value);
}

// Log data to CSV
void log_data(FILE* log_file, int raw_value, int scaled_value, int pwm_value) {
    if (log_file) {
        fprintf(log_file, "%ld,%d,%d,%d\n", time(NULL), raw_value, scaled_value, pwm_value);
        fflush(log_file);
    }
}

int main() {
    int arduino1_fd = open_serial_port(ARDUINO_1_PORT);
    int arduino2_fd = open_serial_port(ARDUINO_2_PORT);
    if (arduino1_fd < 0 || arduino2_fd < 0) {
        if (arduino1_fd >= 0) close(arduino1_fd);
        if (arduino2_fd >= 0) close(arduino2_fd);
        return -1;
    }

    configure_serial_port(arduino1_fd);
    configure_serial_port(arduino2_fd);

    FILE* log_file = fopen(LOG_FILE, "w");
    if (!log_file) {
        perror("Failed to open log file");
        close(arduino1_fd);
        close(arduino2_fd);
        return -1;
    }
    fprintf(log_file, "Timestamp,Raw Value,Scaled Value,PWM Value\n");

    while (1) {
        int raw_value = request_data_from_arduino_1(arduino1_fd);
        int scaled_value = -1;

        if (raw_value >= 0 && raw_value <= 1023) {
            scaled_value = (raw_value * 255) / 1023;
            send_data_to_arduino_2(arduino2_fd, scaled_value);
        } else {
            printf("Invalid value received from Arduino 1: %d\n", raw_value);
        }

        log_data(log_file, raw_value, scaled_value, scaled_value);
        usleep(CONTROL_RATE_DELAY_US);
    }

    fclose(log_file);
    close(arduino1_fd);
    close(arduino2_fd);
    return 0;
}