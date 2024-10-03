#include "pi_control.h"
#include <Arduino.h>

PI_Control::PI_Control(double gain, double integration_time, double time_step, double saturation) 
    : P_Control(gain), Ti(integration_time), T(time_step), saturation_limit(saturation), integral(0) {}

double PI_Control::update(double setpoint, double actual) {

    double error = setpoint - actual;

    double proportional = P_Control::update(setpoint, actual);
    integral += error * T;

    if (proportional >= saturation_limit || proportional <= 0) {
        integral = 0;
    }

    double integral_term = integral / Ti;
    double output = proportional + integral_term;
    
    if (output > saturation_limit) {
        output = saturation_limit;
    } else if (output < 0) {
        output = 0;
    }

    OCR1B = (output / setpoint) * 30.25;
    return output;
}
