#ifndef PROCESSDATA_H
#define PROCESSDATA_H

#include <Arduino.h>
#include <WebSocketsServer.h>

#define BUFFER_SIZE 1024
#define SERIAL_TIMEOUT_MS 500

extern char serialBuffer[BUFFER_SIZE];
extern int bufferIndex;
extern SemaphoreHandle_t mutex;
extern unsigned long lastReceiveTime;

// WebSocket server
extern WebSocketsServer webSocket;

void initProcessData();
void processData(void *parameter);
void processLine(char *line);
void stringToJSON(const char* data);
int countCharOccurrences(const char* str, char ch);

#endif // PROCESSDATA_H
