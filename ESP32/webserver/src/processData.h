#ifndef PROCESSDATA_H
#define PROCESSDATA_H

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "global.h"

#define SERIAL_TIMEOUT_MS 300

extern int bufferIndex;
extern unsigned long lastReceiveTime;

void initProcessData(HardwareSerial& mescSerial, HardwareSerial& compSerial, AsyncWebServer& webServer, AsyncWebSocket& webSocket);

void processData(void *parameter);
void processLine(char *line);
void processConfig(char *line); 
int countCharOccurrences(const char* str, char ch);
void remove_ansi_escape_sequences(char *data);
void replace_pipe_with_tab(char *data);
void sendESPNowString(const char* message);

#endif // PROCESSDATA_H
