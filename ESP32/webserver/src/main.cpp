#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include "webservice.h"
#include "blink.h"  // Include your custom header file

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(1000);
    delay(3000);

    while (!Serial) {
        delay(100);  // Wait for serial port to connect
    }

    Serial.println("Starting setup...");

    // Initialize tasks
    initBlinkTask();          // Initialize the blink task
    initWebService();         // Initialize the web service (HTTP server)

}

void loop() {
    // The loop function is empty because tasks are running in parallel
}
