#ifndef WEBSERVICE_H
#define WEBSERVICE_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

extern WebServer server;

void initWebService();
void handleRoot();
void webServerTask(void *pvParameter);
void healthCheckTask(void *pvParameter);

#endif  // WEBSERVICE_H
