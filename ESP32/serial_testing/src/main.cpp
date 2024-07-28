#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <WiFi.h>
#include <LittleFS.h>
#include "processData.h"
#include "blink.h"  // Include your custom header file

#define compSerial Serial
#define mescSerial Serial2

// This runs on a ESP32 module, typically with Serial connected
//   by USB to a computer, and another hardware serial is
//   connected to MESC/F405
// pupose of this is to connect to serial, perform a get,
//  parse escape characters, generate json
// to run, launch on the command line, run a terminal,
//  send a command like "get" and it is sent to
//  mesc term

void setup() {
  compSerial.begin(115200);
  // 25/27 are pins on the ESP32 connected to the F405 UART
  mescSerial.begin(115200, SERIAL_8N1, 25, 27);

  initBlinkTask();
  initProcessData(mescSerial, compSerial);
}

void loop() {
}
