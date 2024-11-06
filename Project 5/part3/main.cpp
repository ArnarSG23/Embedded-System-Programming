#include <Arduino.h>

const uint8_t SLAVE_ID = 0x01;
//const uint8_t SLAVE_ID = 0x02;

// CRC calculation for Modbus RTU
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

// Handle incoming Modbus messages
void handleModbusMessage(uint8_t *frame, uint8_t length) {
    if (frame[0] != SLAVE_ID) return;

    uint8_t command = frame[5];
    uint8_t response[8] = {SLAVE_ID, 0x06, frame[2], frame[3], frame[4], frame[5]};
    uint16_t crc = ModRTU_CRC(response, 6);
    response[6] = crc & 0xFF;
    response[7] = crc >> 8;
    Serial.write(response, 8);
    Serial.flush();
}

void setup() {
    Serial.begin(9600);
}

void loop() {
    if (Serial.available() >= 8) {
        uint8_t frame[8];
        Serial.readBytes(frame, 8);
        handleModbusMessage(frame, 8);
    }
}