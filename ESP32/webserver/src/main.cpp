#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <AsyncTCP.h>
#include <esp_wifi.h>
#include "processData.h"
#include "processConfig.h"
#include "global.h"
#include "webservice.h"
#include "espnowservice.h"
#include "sd_card.h" // Include the SD card header

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

LoggingState sdLoggingState = {false, "", File(), NULL};

// Pins to define the hardware serial object
#define TX 0
#define RX 1

HardwareSerial mescSerial(1);
#define compSerial Serial

// remove, not used
String getLocalIPAddress() {
    return WiFi.localIP().toString();
}

void readMacAddress(){
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    Serial.println("my mac address");
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                  baseMac[0], baseMac[1], baseMac[2],
                  baseMac[3], baseMac[4], baseMac[5]);
  } else {
    Serial.println("Failed to read MAC address");
  }
}

void setup() {
  compSerial.begin(115200);
  mescSerial.begin(115200, SERIAL_8N1, RX, TX); 

  compSerial.println("Starting setup...");

  g_compSerial = &compSerial;
  g_mescSerial = &mescSerial;
  g_webSocket = &ws;


  readConfig();
  initProcessData(mescSerial, compSerial, server, ws); 
  initWebService(compSerial, mescSerial, server, ws);

  readMacAddress();

  initESPNow();
}

void loop() {}
