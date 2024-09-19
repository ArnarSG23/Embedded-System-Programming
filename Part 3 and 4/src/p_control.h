#ifndef P_CONTROL_H
#define P_CONTROL_H

class P_Control {
private:
    double Kp; // Proportional gain value
public:
    P_Control(double gain); // Constructor to initialize the proportional gain
    double update(double ref, double actual); // Update function to calculate the control output based on reference and actual values
};

#endif