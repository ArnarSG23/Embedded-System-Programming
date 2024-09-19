// // --------------------------------------------------------- PART 1 -----------------------------------------------------
// #include <Arduino.h>
// #include "Encoder.h"
// #include "digital_out.h"
// #include <avr/interrupt.h>
// #include <timer_msec.h>

// //Initialize encoder and digital output objects
// Encoder encoder(0, 2); 
// //Digital_out Aout1(1);
// //Digital_out Aout2(2);
// Timer_msec timer;

// //Arduino setup function
// void setup() {
//     Serial.begin(57600);
//     while (!Serial)
//     {
//       ; // wait for serial port to connect. Needed for Native USB only
//     }  
// }

// int main() {
    
//     //Aout1.init();
//     //Aout2.init();
//     //Aout1.set_lo();
//     timer.init(1000);
//     //Aout2.set_hi();
//     while(1){
//     }
// }

// ISR(INT0_vect)
// {
//     if (encoder.pin1.is_hi()) 
//     {
//         if (encoder.pin1.is_hi()) 
//         {
//             encoder.count++;
//         }
//         else 
//         {
//             encoder.count--;
//         }
//     }
    
//     else
//     {
//         if (encoder.pin2.is_lo())
//         {
//             encoder.count++;
//         }
//         else
//         {
//             encoder.count--;
//         }
//     }

// }

// ISR(TIMER1_COMPA_vect)
// {
//     Serial.print(" PPS: ");
//     Serial.println((encoder.count));
//     Serial.print("RPM: ");
//     Serial.println(encoder.RPM());
//     Serial.println();
//     encoder.count = 0;
// }


// --------------------------------------------------------- main - Part 2 ---------------------------------------------------------

#include <Arduino.h>
#include "Encoder.h"
#include "digital_out.h"
#include <avr/interrupt.h>
#include <timer_msec.h>

//Initialize encoder and digital output objects
Encoder encoder(0, 2); 
Digital_out Aout1(1);
//Digital_out Aout2(2);
Timer_msec timer;
int ms = 0;


//Arduino setup function
void setup() {
    Serial.begin(57600);
    while (!Serial)
    {
      ; // wait for serial port to connect. Needed for Native USB only
    }  
}

int main() {
    
    Aout1.init();
    //Aout2.init();
    Aout1.set_hi();
    timer.init(10);
    //Aout2.set_hi();
    while(1){
    }
}

ISR(INT0_vect)
{
    if (encoder.pin1.is_hi()) 
    {
        if (encoder.pin1.is_hi()) 
        {
            encoder.count++;
        }
        else 
        {
            encoder.count--;
        }
    }
    
    else
    {
        if (encoder.pin2.is_lo())
        {
            encoder.count++;
        }
        else
        {
            encoder.count--;
        }
    }

}

ISR(TIMER1_COMPA_vect)
{
    ms=ms+10;
    Serial.print(ms);
    Serial.print("ms  ");
    Serial.print(" PPS: ");
    Serial.println((encoder.count)*100);
    // Serial.print("Max RPM: ");
    // Serial.println((encoder.RPM()));
    // Serial.print("63% of PPS: " );
    // Serial.println((encoder.count)*0.63);
    // Serial.println();
     encoder.count = 0;

}



// --------------------------------------------------------- main - Part 3 ---------------------------------------------------------
