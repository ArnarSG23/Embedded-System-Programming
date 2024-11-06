//------------------------------------------ potentiometer ----------------------------------------------------
// #include <Arduino.h>

// const uint8_t SLAVE_ID = 0x01;
// const int analogPin = A0;

// uint16_t ModRTU_CRC(uint8_t *data, uint8_t length) {
//     uint16_t crc = 0xFFFF;
//     for (int pos = 0; pos < length; pos++) {
//         crc ^= (uint16_t)data[pos];
//         for (int i = 8; i != 0; i--) {
//             if ((crc & 0x0001) != 0) {
//                 crc >>= 1;
//                 crc ^= 0xA001;
//             } else {
//                 crc >>= 1;
//             }
//         }
//     }
//     return crc;
// }

// void handleModbusRequest() {
//     if (Serial.available() >= 8) {
//         uint8_t request[8];
//         Serial.readBytes(request, 8);

//         if (request[0] == SLAVE_ID && request[1] == 0x06) {
//             int sensor_value = analogRead(analogPin);
//             uint8_t response[8] = { SLAVE_ID, 0x06, request[2], request[3], (sensor_value >> 8) & 0xFF, sensor_value & 0xFF };
//             uint16_t crc = ModRTU_CRC(response, 6);
//             response[6] = crc & 0xFF;
//             response[7] = crc >> 8;
//             Serial.write(response, 8);
//             Serial.flush();
//         }
//     }
// }

// void setup() {
//     Serial.begin(9600);
// }

// void loop() {
//     handleModbusRequest();
// }

//------------------------------------------ motor ----------------------------------------------------

#include <Arduino.h>

const uint8_t SLAVE_ID = 0x02;
const int PWM_PIN = 9;

uint16_t ModRTU_CRC(uint8_t *data, uint8_t length) {
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < length; pos++) {
        crc ^= (uint16_t)data[pos];
        for (int i = 8; i != 0; i--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

void handleModbusRequest() {
    if (Serial.available() >= 8) {
        uint8_t request[8];
        Serial.readBytes(request, 8);

        if (request[0] == SLAVE_ID && request[1] == 0x06) {
            // Extract PWM value from the request
            int pwm_value = (request[4] << 8) | request[5];
            pwm_value = constrain(pwm_value, 0, 255);  // Ensure PWM value is within range

            // Set the PWM output
            analogWrite(PWM_PIN, pwm_value);

            // Prepare the response message
            uint8_t response[8] = { SLAVE_ID, 0x06, request[2], request[3], request[4], request[5] };
            uint16_t crc = ModRTU_CRC(response, 6);
            response[6] = crc & 0xFF;
            response[7] = crc >> 8;

            // Send the response
            Serial.write(response, 8);
            Serial.flush();
        }
    }
}

void setup() {
    Serial.begin(9600);
    pinMode(PWM_PIN, OUTPUT);
}

void loop() {
    handleModbusRequest();
}
