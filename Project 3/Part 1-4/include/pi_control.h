#ifndef PI_CONTROL_H
#define PI_CONTROL_H

#include "p_control.h"

class PI_Control : public P_Control {
private:
    double integral; // The integral term
    double Ti; // Integration time constant
    double saturation_limit; // The limit for anti-windup
    double T; // Time step for integration
public:
    PI_Control(double Kp, double Ti, double T, double saturation_limit); 
    double update(double setpoint, double actual); 
};

#endif
