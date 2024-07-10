#ifndef WEBSERVICE_H
#define WEBSERVICE_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <WebSocketsServer.h>

extern WebServer server;
extern WebSocketsServer webSocket;

void initWebService();
void handleRoot();
void handleUpdate();
void webServerTask(void *pvParameter);
void healthCheckTask(void *pvParameter);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

#endif  // WEBSERVICE_H
