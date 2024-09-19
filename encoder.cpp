#include "Encoder.h"
#include <avr/io.h>
#include <timer0_msec.h>

// Encoder constructor initializes pin1 and pin2 for the encoder, and setsup interrupts
Encoder::Encoder(int pin1_num, int pin2_num) : pin1(pin1_num, 'B'), pin2(pin2_num, 'D'){ 
        EICRA |= (1 << ISC00); //Sense interrupt on INT0
        EIMSK |= (1 << INT0);  //Enable interrupt on INT0
        
        sei(); // enable interrupts
    }

// Get the current position of the encoder
int Encoder::getPosition(){
    return count*250; 
}

