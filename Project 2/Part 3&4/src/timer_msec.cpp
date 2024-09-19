#include <avr/interrupt.h>
#include <timer_msec.h>
#include <avr/io.h>


Timer_msec::Timer_msec(){}

void Timer_msec::init(int period_ms)
{
 TCCR1A = 0;                                              // set timer1 to normal operation
 TCCR1B = 0;                                              // ​ (all bits in control registers A and B set to zero)​
 TCNT1 = 0;                                               // initialize timer to 0​

 OCR1A = (16000/1024) * period_ms - 1;        // assign target count to compare register A 
                                               // (must be less than 65536)​
 //OCR1A = 255;
 //OCR1B = OCR1A / 4; // assign target count to compare register B 
 OCR1B = OCR1A;                                                         // (should not exceed value of OCR1A)
 TCCR1B |= (1 << WGM12);                                  // clear the timer on compare match A (Mode 4, CTC on OCR1A)
 TIMSK1 |= (1 << OCIE1A);                                 // set interrupt on compare match A
 TIMSK1 |= (1 << OCIE1B);                                 // set interrupt on compare match B​
 TCCR1B |= (1 << CS12) | (1 << CS10);                     // set prescaler to 1024 and start the timer​
}
