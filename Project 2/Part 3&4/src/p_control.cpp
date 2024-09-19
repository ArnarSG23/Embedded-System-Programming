#include "p_control.h"
#include <avr/io.h>
#include <Arduino.h>

P_Control::P_Control(double gain) : Kp(gain){}

// Update function for the proportional control
double P_Control::update(double setpoint, double actual){
    double update = Kp * (setpoint - actual); // Calculate control based on error
    if ((update) > setpoint){ // Ensure that output does not exceed setpoint
        OCR1B = 30.25;
        update = setpoint; // limit the update to the setpoint
    }
    else if (update < 0){ // Prevent output from going below zero
        OCR1B = 0; 
        update = 0;
    }
    else {
        OCR1B = (update/setpoint) * 30.25; // If within valid range, calculate proportional PWM
    }
    return update;
}

