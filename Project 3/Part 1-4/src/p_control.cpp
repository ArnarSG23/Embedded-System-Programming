#include "p_control.h"
#include <avr/io.h>
#include <Arduino.h>

P_Control::P_Control(double gain) : Kp(gain){}

// Update function for the proportional control
double P_Control::update(double setpoint, double actual){
    double update = Kp * (setpoint - actual); // Calculate control based on error
    return update;
}