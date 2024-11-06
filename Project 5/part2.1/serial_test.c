#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#define SERIAL_PORT "/dev/ttyUSB0"  
#define BAUD_RATE B57600

// Serial port configurations - opens and configurates the serial port for communication with the arduino
int configure_serial(const char *device) {
    int fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror("Unable to open serial device");
        return -1;
    }
    struct termios options;
    tcgetattr(fd, &options);
    options.c_cflag = BAUD_RATE | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &options);
    return fd;
}

// Function to send a single-character command to the Arduino
void send_command(int fd, char command) {
    write(fd, &command, 1); 
    write(fd, "\n", 1);    
    tcflush(fd, TCOFLUSH);    
    printf("Sent command: %c\n", command);
    usleep(200000);           
}

// Function to read the response from the Arduino and display it
void read_response(int fd) {
    char buffer[256];
    int bytes_read = 0;

    // Read until newline is received, or the buffer limit is reached
    do {
        bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    } while (bytes_read == 0);

    // Null-terminate the response and print it
    buffer[bytes_read] = '\0';
    printf("Arduino response: %s\n", buffer);
    tcflush(fd, TCIFLUSH);  // Clear buffer after reading
}

int main() {
    // Open and configure serial connection to Arduino
    int arduinoFd = configure_serial(SERIAL_PORT);
    if (arduinoFd == -1) return 1;

    char choice;

    while (1) {
        // Command options to user
        printf("Enter command for Arduino (p: Pre-Operational, o: Operational, s: Stopped, r: Reset, q: Quit): ");
        choice = getchar();
        getchar(); 

        // Send command to the Arduino and read response
        switch (choice) {
            case 'p':  // Pre-operational
                send_command(arduinoFd, 'p');
                read_response(arduinoFd);
                break;

            case 'o':  // Operational
                send_command(arduinoFd, 'o');
                read_response(arduinoFd);
                break;

            case 's':  // Stopped
                send_command(arduinoFd, 's');
                read_response(arduinoFd);
                break;

            case 'r':  // Reset (Initialization)
                send_command(arduinoFd, 'r');
                read_response(arduinoFd);
                break;

            case 'q':  // Quit the program
                printf("Exiting...\n");
                close(arduinoFd);
                return 0;

            default:
                printf("Invalid command. Please enter p, o, s, r, or q to quit.\n");
                break;
        }
    }

    close(arduinoFd);
    return 0;
}