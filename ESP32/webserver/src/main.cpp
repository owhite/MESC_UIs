#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <WiFi.h>
#include <LittleFS.h>
#include "processData.h"
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

    initBlinkTask();
    initProcessData();
    initWebService();
}

void loop() {

}
