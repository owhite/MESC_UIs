#ifndef PROCESSDATA_H
#define PROCESSDATA_H

#include <Arduino.h>
#include <WebSocketsServer.h>

#define BUFFER_SIZE 526
#define SERIAL_TIMEOUT_MS 100
#define compSerial Serial
#define mescSerial Serial2

extern char serialBuffer[BUFFER_SIZE];
extern int bufferIndex;
extern unsigned long lastReceiveTime;

void initProcessData(HardwareSerial& mescSerial, HardwareSerial& compSerial, WebSocketsServer& webSocket);
void processData(void *parameter);
void processLine(char *line);
void stringToJSON(const char* data);
int countCharOccurrences(const char* str, char ch);
void remove_ansi_escape_sequences(char *data);
void replace_pipe_with_tab(char *data);

#endif // PROCESSDATA_H
