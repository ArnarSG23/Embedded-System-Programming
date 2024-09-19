#ifndef DIGITAL_IN
#define DIGITAL_IN

#include <stdint.h>

// Class for handling digital input operations
class Digital_in
{
    public:

    Digital_in(int pinNumber, char port); // Initialize with the given pin and port
    void init(); // Initialize the digital input pin
    bool is_hi(); // Check if the digital input is high
    bool is_lo(); // Check if the digital input is low

    private:

    uint8_t pinMask; // Pin mask used for reading pin state
    char portChar; // Port character ('B' or 'D')
};

#endif