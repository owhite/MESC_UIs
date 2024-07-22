#ifndef WEBSERVICE_H
#define WEBSERVICE_H

#include <Arduino.h>
#include <WebSocketsServer.h>  // Include WebSocketsServer library

extern WebSocketsServer webSocket;

void initWebService();
void webServerTask(void *pvParameter);
void handleRoot();
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
void handleButtonPress();
void processButtonPress();


#endif // WEBSERVICE_H
