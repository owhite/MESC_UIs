#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <WiFi.h>
#include <LittleFS.h>
#include "processData.h"
#include "blink.h"  // Include your custom header file

// Define the hardware serial object
#define compSerial Serial
#define TX 0
#define RX 1
HardwareSerial mescSerial(1);

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
  mescSerial.begin(115200, SERIAL_8N1, RX, TX); 

  initBlinkTask();
  initProcessData(mescSerial, compSerial);
}

void loop() {
}
