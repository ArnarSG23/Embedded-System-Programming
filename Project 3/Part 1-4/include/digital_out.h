#ifndef DIGITAL_IN_H
#define DIGITAL_IN_H

#include <stdint.h>

// Class for handling digital output operations
class Digital_out
{
public:
    Digital_out(int pin);

    void init(); // Initialize digital ouput pin

    void set_hi(); // Set digital output high

    void set_lo(); // Set digital output low

    void toggle(); // Toggle digital output state

private:
    uint8_t pin_mask; // Pinmask for setting pin state
};

#endif // DIGITAL_OUT