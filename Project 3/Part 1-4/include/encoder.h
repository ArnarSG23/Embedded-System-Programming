#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include <digital_in.h>

class Encoder {
private:

public:
    Encoder(int pin1_num, int pin2_num);
    int getPosition();
    float RPM();
    int count;
    Digital_in pin1;
    Digital_in pin2;
};

#endif