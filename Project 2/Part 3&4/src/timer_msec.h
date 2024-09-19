#ifndef TIMER_MSEC_H
#define TIMER_MSEC_H

#include <stdint.h>

// Class for handling digital output operations
class Timer_msec
{
public:
    Timer_msec();
    void init(int ms);
};

#endif // DIGITAL_OUT