#ifndef TIMER_MSEC_h
#define TIMER_MSEC_h

#include <stdint.h>

// Class for handling digital output operations
class Timer_msec
{
public:
    Timer_msec();
    void init(int ms);
    void set_duty_cycle(int duty_cycle);

private:
};

#endif // DIGITAL_OUT