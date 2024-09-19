#include <avr/interrupt.h>
#include <timer0_msec.h>
#include <avr/io.h>


Timer0_msec::Timer0_msec(){}

void Timer0_msec::init(int period_ms)
{
    // Set Timer0 to normal operation
    TCCR0A = 0;   // Clear the Timer0 Control Register A
    TCCR0B = 0;   // Clear the Timer0 Control Register B
    TCNT0 = 0;    // Initialize Timer0 counter to 0

    // Calculate the compare value for CTC mode
    OCR0A = (16000000 / 1024 / 1000) * period_ms - 1; // Assuming 16MHz clock, prescaler 1024, period in ms 

    // Configure timer: CTC mode, clear on compare match
    TCCR0A |= (1 << WGM01);  // Set WGM01 for CTC mode
    TIMSK0 |= (1 << OCIE0A);  // Enable Timer0 Compare Match A interrupt

    // Set Prescaler to 1024 and start the timer
    TCCR0B |= (1 << CS02) | (1 << CS00);
}

