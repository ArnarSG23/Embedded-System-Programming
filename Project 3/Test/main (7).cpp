#include <Arduino.h>
#include "pi_control.h"

double Kp = 1.0;
double Ti = 1.0; 
double T = 1.0; 
double saturation = 200; 

PI_Control pi_controller(Kp, Ti, T, saturation);

double setpoints[] = {100, 100, 100, 300, 300, 300, 200, 200, 200, 200}; // Array of setpoints
double actuals[] = {0, 50, 120, 180, 240, 255, 240, 210, 200, 190};      // Array of actual values
int num_tests = sizeof(setpoints) / sizeof(setpoints[0]); 
int current_test = 0; 
double control_output = 0; // Output from the controller

void setup() {
  Serial.begin(9600);

}

void loop() {
  if (current_test < num_tests) {
    double setpoint = setpoints[current_test];
    double actual = actuals[current_test];

    control_output = pi_controller.update(setpoint, actual);

    Serial.print("Test Case "); Serial.println(current_test + 1);
    Serial.print("Setpoint: "); Serial.print(setpoint);
    Serial.print(" | Actual: "); Serial.print(actual);
    Serial.print(" | Control Output: "); Serial.println(control_output);
    Serial.println("--------------------");

    current_test++;
  } else {
    while (1); 
  }
}
