#ifndef TIMER0_MSEC_H
#define TIMER0_MSEC_H

#include <stdint.h>

// Class for handling digital output operations
class Timer0_msec
{
public:
    Timer0_msec();
    void init(int ms);
};

#endif // DIGITAL_OUT