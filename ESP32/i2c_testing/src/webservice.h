#ifndef WEBSERVICE_H
#define WEBSERVICE_H

#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>  // For ESP32
#include <cstring> // For strcmp, strstr

void initWebService(HardwareSerial& compSerial, HardwareSerial& mescSerial, AsyncWebServer& server, AsyncWebSocket& webSocket);

void webServerTask(void *pvParameter);
void handleRoot(AsyncWebServerRequest *request);
void handleButtonPress(AsyncWebServerRequest *request);
void processCommand(const char* command);
void processButtonPress();
void handleWebSocketMessage(AsyncWebSocketClient* client, uint8_t *data, size_t len);

#endif // WEBSERVICE_H
