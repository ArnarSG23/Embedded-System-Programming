// -------------------------------------------  ----------------------------------
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdint.h>
#include <stdlib.h>

#define USB_PORT "/dev/ttyUSB2"  
#define BAUD_RATE B9600

uint16_t ModRTU_CRC(uint8_t buf[], int len);

int main(int argc, char *argv[]) {
    int file, count;

    if (argc != 5) {
        printf("Usage: %s <slave_id> <function_code> <register_address> <value_to_write>\n", argv[0]);
        return -2;
    }

    if ((file = open(USB_PORT, O_RDWR | O_NOCTTY)) < 0) { 
        perror("Failed to open the USB file.");
        return -1;
    }

    struct termios options;
    tcgetattr(file, &options);
    cfmakeraw(&options);  // USB set port to raw mode for binary comms
    options.c_cflag = BAUD_RATE | CS8 | CREAD | CLOCAL;
    options.c_iflag = IGNPAR | ICRNL;
    tcflush(file, TCIFLUSH);
    tcsetattr(file, TCSANOW, &options);

    // Prepare Modbus message
    uint8_t msg[8];
    msg[0] = atoi(argv[1]);             // Slave ID
    msg[1] = atoi(argv[2]);             // Function Code
    msg[2] = atoi(argv[3]) >> 8;        // Register Address MSB
    msg[3] = atoi(argv[3]) & 0xFF;      // Register Address LSB
    msg[4] = atoi(argv[4]) >> 8;        // Value MSB
    msg[5] = atoi(argv[4]) & 0xFF;      // Value LSB
    uint16_t crc = ModRTU_CRC(msg, 6);
    msg[6] = crc & 0xFF;
    msg[7] = crc >> 8;

    // Write message to the serial port
    if ((count = write(file, msg, 8)) < 0) {
        perror("Failed to write to the output");
        close(file);
        return -1;
    }
    tcflush(file, TCIFLUSH);  

    // Print sent message
    printf("Sent request: ");
    for (int i = 0; i < 8; i++) {
        printf("%02x ", msg[i]);
    }
    printf("\n");

    // Wait for response
    usleep(300000);  

    // Read response
    uint8_t receive[100];
    if ((count = read(file, (void *)receive, 100)) < 0) {
        perror("Failed to read from the input");
        close(file);
        return -1;
    }

    // Check if data is available
    if (count == 0) {
        printf("No data available to read!\n");
    } else {
        printf("Received reply: ");
        for (int i = 0; i < count; i++) {
            printf("%02x ", receive[i]);
        }
        printf("\n");
    }

    close(file);
    return 0;
}

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
