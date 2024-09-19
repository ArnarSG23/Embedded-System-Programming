#include <Arduino.h>
#include "Encoder.h"
#include "digital_out.h"
#include <avr/interrupt.h>
#include <timer_msec.h>
#include <p_control.h>
#include <timer0_msec.h>

//Initialize encoder and digital output objects
Encoder encoder(0, 2); 
Digital_out Aout1(1);

Timer0_msec timer0;
Timer_msec timer;
P_Control control(1); // Proportional control object with gain Kp

double setpoint = 1500.0; // Target speed in PPS
int counter = 0; 
int array1[100]; // Array to store actual speed values
int array2[100]; // Array to store PWM values

void setup() {
    Serial.begin(57600);
    while (!Serial)
    {
      ; // wait for serial port to connect. Needed for Native USB only
    }  
  
}

int main() {

    _delay_ms(2000);
    Aout1.init();
    timer0.init(4); // Timer for update rate
    timer.init(2); // Timer PWM signal
    while(1){
    }
}

// Print function to print output of the collected data to Serial monitor
void print(){
    for (int i = 0; i < 100; i++){
        Serial.print("Reference value is: ");
        Serial.println(setpoint);
        Serial.print("Actual value is: ");
        Serial.println(array1[i]);
        Serial.print("PWM value is: ");
        Serial.println(array2[i]);
    }
}

// ISR for encoder
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
    Aout1.set_hi();
}

ISR(TIMER1_COMPB_vect)
{
    Aout1.set_lo();
}


ISR(TIMER0_COMPA_vect)
{ 
    // This ISR executes every 4ms to update control and store data
    int pwm = control.update(setpoint, encoder.count*250);
    
    if (counter > 0){
        array1[counter] = (encoder.count/0.004);
        array2[counter] = pwm;
    }
    if (counter == 100){
        print();
    }
    counter ++;
    encoder.count = 0;

}