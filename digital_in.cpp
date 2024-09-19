#include "digital_in.h"
#include <avr/io.h>

// Constructor: Initialize the pin mask and port character for the given pin
Digital_in::Digital_in(int pinNumber, char port)
{
    pinMask = 1 << pinNumber; // Create a bit mask for the given pin
    portChar = port; // Store the port character ('B' or 'D')

}

// Initialize the pin as input
void Digital_in::init()
{
    switch(portChar){ // Configure the input pin based on the port character
        case 'B':
            DDRB |= pinMask; // Set the pin as input
            PORTB |= pinMask; // Enable pull-up resistor for the pin
            break;
        case 'D':
            DDRD |= pinMask; // Set the pin as input
            PORTD |= pinMask; // Enable pull-up resistor for the pin
            break;
    }
}

// Check if the digital input is high
bool Digital_in::is_hi()
{
    return PINB & pinMask; // Return true if the corresponding pin bit is high in the PINB register
}

// Check if the digital input is low
bool Digital_in::is_lo()
{
    return !is_hi(); // Return true if the pin is not high
}