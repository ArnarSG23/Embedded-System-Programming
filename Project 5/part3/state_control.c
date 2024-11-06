#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define USB_PORT_1 "/dev/ttyUSB1"  
#define USB_PORT_2 "/dev/ttyUSB0"  
#define BAUD_RATE B9600
#define LOG_FILE "../logs/state_control.log"

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

// Logging function
void log_event(const char *message) {
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file) {
        time_t now = time(NULL);
        fprintf(log_file, "[%s] %s\n", ctime(&now), message);
        fclose(log_file);
    } else {
        perror("Failed to open log file");
    }
}

// Configure serial connection
int configure_serial(const char *device) {
    int fd = open(device, O_RDWR | O_NOCTTY);
    if (fd == -1) {
        perror("Failed to open USB file");
        log_event("Failed to open USB file");
        return -1;
    }
    struct termios options;
    tcgetattr(fd, &options);
    cfmakeraw(&options);
    options.c_cflag = BAUD_RATE | CS8 | CREAD | CLOCAL;
    options.c_iflag = IGNPAR | ICRNL;
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &options);
    return fd;
}

// Send command to Arduino
void send_command(int fd, uint8_t slave_id, uint8_t command) {
    uint8_t msg[8] = {slave_id, 0x06, 0x00, 0x01, 0x00, command};
    uint16_t crc = ModRTU_CRC(msg, 6);
    msg[6] = crc & 0xFF;   
    msg[7] = crc >> 8;     

    printf("Sending command to Arduino %d: ", slave_id);
    for (int i = 0; i < 8; i++) {
        printf("%02x ", msg[i]);
    }
    printf("\n");

    char log_msg[100];
    snprintf(log_msg, sizeof(log_msg), "Sending command to Arduino %d: %02x %02x %02x %02x %02x %02x %02x %02x",
             slave_id, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5], msg[6], msg[7]);
    log_event(log_msg);

    write(fd, msg, 8);
    tcflush(fd, TCOFLUSH); 
}

// Read response with error handling
int read_response(int fd, uint8_t *buffer, int buffer_size) {
    int count = read(fd, buffer, buffer_size);
    if (count <= 0) {
        perror("Failed to read response: Timeout or no data");
        log_event("Failed to read response: Timeout or no data");
        return -1;
    }
    return count;
}

// Display the state message based on command
void display_state_message(uint8_t command) {
    switch (command) {
        case 0x01:
            printf("State: Operational\n");
            log_event("State: Operational");
            break;
        case 0x02:
            printf("State: Stop\n");
            log_event("State: Stop");
            break;
        case 0x80:
            printf("State: Pre-Operational\n");
            log_event("State: Pre-Operational");
            break;
        case 0x81:
            printf("State: Reset Node\n");
            log_event("State: Reset Node");
            break;
        case 0x82:
            printf("State: Reset Communications\n");
            log_event("State: Reset Communications");
            break;
        default:
            printf("Unknown state\n");
            log_event("Unknown state");
            break;
    }
}

int main() {
    int arduinoFd1 = configure_serial(USB_PORT_1);
    int arduinoFd2 = configure_serial(USB_PORT_2);
    if (arduinoFd1 == -1 || arduinoFd2 == -1) return 1;

    char choice;
    uint8_t selected_arduino;
    uint8_t command_code;

    while (1) {
        // Select Arduino
        printf("Select Arduino (1 or 2, q to quit): ");
        choice = getchar();
        getchar();  

        if (choice == '1') {
            selected_arduino = 0x01;  // Slave ID for Arduino 1
        } else if (choice == '2') {
            selected_arduino = 0x02;  // Slave ID for Arduino 2
        } else if (choice == 'q') {
            printf("Exiting...\n");
            log_event("Exiting program");
            close(arduinoFd1);
            close(arduinoFd2);
            return 0;
        } else {
            printf("Invalid selection. Please choose 1, 2, or q to quit.\n");
            continue;
        }

        // Select command to send
        printf("Enter command (o: Operational, s: Stop, p: Pre-Operational, n: Reset Node, c: Reset Comms): ");
        choice = getchar();
        getchar();  

        switch (choice) {
            case 'o': command_code = 0x01; break;
            case 's': command_code = 0x02; break;
            case 'p': command_code = 0x80; break;
            case 'n': command_code = 0x81; break;
            case 'c': command_code = 0x82; break;
            default:
                printf("Invalid command.\n");
                continue;
        }

        // Send the command to the selected Arduino
        int fd = (selected_arduino == 0x01) ? arduinoFd1 : arduinoFd2;
        send_command(fd, selected_arduino, command_code);

        // Read the response
        uint8_t response[100];
        int count = read_response(fd, response, sizeof(response));
        if (count > 0) {
            printf("\nReceived response from Arduino %d: ", selected_arduino);
            for (int i = 0; i < count; i++) {
                printf("%02x ", response[i]);
            }
            printf("\n");

            // Log the received response
            char log_msg[100];
            snprintf(log_msg, sizeof(log_msg), "Received response from Arduino %d", selected_arduino);
            log_event(log_msg);

            // Display the state message based on the command sent
            display_state_message(command_code);
        }
    }

    close(arduinoFd1);
    close(arduinoFd2);
    return 0;
}