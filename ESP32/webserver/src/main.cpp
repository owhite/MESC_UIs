#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include "processData.h"
#include "webservice.h"
#include "blink.h"

#define compSerial Serial
#define mescSerial Serial2

WebSocketsServer webSocket(81);

void setup() {
    compSerial.begin(115200);
    mescSerial.begin(115200, SERIAL_8N1, 25, 27);

    while (!compSerial) {
        delay(100);  // Wait for serial port to connect
    }

    compSerial.println("Starting setup...");

    initBlinkTask();
    initProcessData(mescSerial, compSerial, webSocket);
    initWebService(compSerial, mescSerial, webSocket);
}

void loop() {}
