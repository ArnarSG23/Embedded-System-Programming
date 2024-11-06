#include <Arduino.h>

//const uint8_t SLAVE_ID = 0x01;       // Modbus slave ID - arduino 1
const uint8_t SLAVE_ID = 0x01;       // Modbus slave ID - arduino 2
uint16_t register_value = 128;        // Register to store a single value

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

// Send exception response
void sendExceptionResponse(uint8_t function, uint8_t exceptionCode) {
    uint8_t response[5];
    response[0] = SLAVE_ID;
    response[1] = function | 0x80; // Exception indicator
    response[2] = exceptionCode;
    uint16_t crc = ModRTU_CRC(response, 3);
    response[3] = crc & 0xFF;
    response[4] = (crc >> 8) & 0xFF;
    Serial.write(response, 5);
}

// Handle incoming Modbus messages
void handleModbusMessage(uint8_t *frame, uint8_t length) {
    if (length < 8 || frame[0] != SLAVE_ID) return;

    uint8_t function_code = frame[1];
    uint16_t address = (frame[2] << 8) | frame[3];
    uint16_t crc_received = (frame[length - 2] | (frame[length - 1] << 8));
    uint16_t crc_calculated = ModRTU_CRC(frame, length - 2);

    if (crc_received != crc_calculated) return;

    if (function_code == 0x03) {  // Read register
        if (address != 0x0001) {
            sendExceptionResponse(function_code, 0x02); // Illegal data address
            return;
        }
        uint8_t response[7] = {SLAVE_ID, function_code, 0x02, highByte(register_value), lowByte(register_value)};
        uint16_t crc = ModRTU_CRC(response, 5);
        response[5] = crc & 0xFF;
        response[6] = (crc >> 8) & 0xFF;
        Serial.write(response, 7);
    } else if (function_code == 0x06) {  // Write single register
        if (address != 0x0001) {
            sendExceptionResponse(function_code, 0x02); // Illegal data address
            return;
        }
        register_value = (frame[4] << 8) | frame[5];
        uint8_t response[8] = {SLAVE_ID, function_code, frame[2], frame[3], frame[4], frame[5]};
        uint16_t crc = ModRTU_CRC(response, 6);
        response[6] = crc & 0xFF;
        response[7] = (crc >> 8) & 0xFF;
        Serial.write(response, 8);
    } else { // Unsupported function code
        sendExceptionResponse(function_code, 0x01); // Illegal function
    }
}

void setup() {
    Serial.begin(9600);
}

void loop() {
    if (Serial.available() >= 8) {
        uint8_t frame[8];
        for (int i = 0; i < 8; i++) {
            frame[i] = Serial.read();
        }
        handleModbusMessage(frame, 8);
    }
}