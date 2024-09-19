#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include <digital_in.h>

class Encoder {
private:

public:
    Encoder(int pin1_num, int pin2_num);
    int getPosition(); // Function to get current position
    int count; // Varialbe to store encoder count
    Digital_in pin1;
    Digital_in pin2;

};

#endif