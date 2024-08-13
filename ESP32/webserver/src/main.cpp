#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <AsyncTCP.h>
#include "processData.h"
#include "processConfig.h"
#include "global.h"
#include "webservice.h"
#include "blink.h"

// Global variable definitions
int commState = 0;
DynamicJsonDocument jsonDoc(5024);

float globalVar2 = 0.0f;
char globalArray[100] = {0};

// Define global pointers
HardwareSerial* g_compSerial = nullptr;
HardwareSerial* g_mescSerial = nullptr;
AsyncWebSocket* g_webSocket = nullptr;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Pins to define the hardware serial object
#define TX 0
#define RX 1

HardwareSerial mescSerial(1);
#define compSerial Serial

String getLocalIPAddress() {
    return WiFi.localIP().toString();
}

void setup() {
  compSerial.begin(115200);
  mescSerial.begin(115200, SERIAL_8N1, RX, TX); 

  compSerial.println("Starting setup...");

  g_compSerial = &compSerial;
  g_mescSerial = &mescSerial;
  g_webSocket = &ws;

  readConfig();

  initBlinkTask();
  initProcessData(mescSerial, compSerial, server, ws); 
  initWebService(compSerial, mescSerial, server, ws);
}

void loop() {}
