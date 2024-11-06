#include <Arduino.h>

void setup() {
    Serial.begin(57600); 
    Serial.println("Arduino ready");
}

void loop() {
    if (Serial.available() > 0) {
        char command = Serial.read();  // Read the incoming command character

        // Process the command
        switch (command) {
            case 'p':  // Pre-operational
                Serial.println("Set to Pre-Operational");
                break;
                
            case 'o':  // Operational
                Serial.println("Set to Operational");
                break;
                
            case 's':  // Stop
                Serial.println("Set to Stopped");
                break;
                
            case 'r':  // Reset
                Serial.println("Reset Node");
                break;
                
            default:
                Serial.println("Unknown command");
                break;
        }
    }
}