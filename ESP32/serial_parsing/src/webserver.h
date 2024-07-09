#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <Arduino.h>
#include <WebServer.h>  // Include WebServer.h for ESP32 web server functionality
#include <LittleFS.h>

extern WebServer server;  // Declare the server instance

void initWebserver();
void handleRoot();
String fetchTableRows();

#endif // WEBSERVER_H
