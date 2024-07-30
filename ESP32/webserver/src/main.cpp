#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>  // For ESP32
#include "processData.h"
#include "global.h"
#include "webservice.h"
#include "blink.h"

int commState = 0;
DynamicJsonDocument jsonDoc(5024);

float globalVar2 = 0.0f;
char globalArray[100] = {0};

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void setup() {
    compSerial.begin(115200);
    mescSerial.begin(115200, SERIAL_8N1, 25, 27);

    while (!compSerial) {
        delay(100);  // Wait for serial port to connect
    }

    compSerial.println("Starting setup...");

    initBlinkTask();
    initProcessData(mescSerial, compSerial, server, ws); 
    initWebService(compSerial, mescSerial, server, ws);
}

void loop() {}
