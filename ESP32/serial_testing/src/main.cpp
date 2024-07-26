#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <WiFi.h>
#include <LittleFS.h>
#include "processData.h"
#include "blink.h"  // Include your custom header file

void setup() {
    initBlinkTask();
    initProcessData();
}

void loop() {
}
