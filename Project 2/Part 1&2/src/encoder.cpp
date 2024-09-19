#include "Encoder.h"
#include <avr/io.h>

Encoder::Encoder(int pin1_num, int pin2_num) : pin1(pin1_num, 'B'), pin2(pin2_num, 'D'){
        //DDRD &= ~(1 << DDD2); // set the PD2 pin as input
        //PORTD |= (1 << PORTD2);     
        EICRA |= (1 << ISC00); //Sense interrupt
        EIMSK |= (1 << INT0);  //Enable interrupt on INT0
        
        sei(); // enable interrupts
    }

// Get the current position of the encoder
int Encoder::getPosition(){
    return count;
}

float Encoder::RPM(){
    return count / 1400.0 * 60;
}

