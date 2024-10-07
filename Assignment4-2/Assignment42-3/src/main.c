#include <stdio.h>
#include <fcntl.h>   // For open()
#include <unistd.h>  // For write() and close()
#include <string.h>  // For strlen()

#define DEVICE "/dev/mydev1"   // Path to the device node

int main() {
    int fd;
    int counter = 1;
    char buffer[128];  // Buffer to hold the message

    // Open the device file
    fd = open(DEVICE, O_WRONLY);  // Open in write-only mode
    if (fd < 0) {
        perror("Failed to open the device");
        return -1;
    }

    // Write data to the device 10 times, once per second
    while (counter <= 10) {
        // Update the buffer to contain the message
        snprintf(buffer, sizeof(buffer), "Run %d, Writing bytes to driver", counter);

        // Print the message to the terminal
        printf("%s\n", buffer);

        // Write the message to the device
        if (write(fd, buffer, strlen(buffer)) < 0) {
            perror("Failed to write to the device");
            close(fd);
            return -1;
        }

        // Wait for 1 second before writing again
        sleep(1);
        counter++;
    }

    // Close the device file
    close(fd);

    return 0;
}