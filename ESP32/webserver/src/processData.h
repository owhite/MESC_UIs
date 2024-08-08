#ifndef PROCESSDATA_H
#define PROCESSDATA_H

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define BUFFER_SIZE 526
#define SERIAL_TIMEOUT_MS 300

extern char serialBuffer[BUFFER_SIZE];
extern int bufferIndex;
extern unsigned long lastReceiveTime;

void initProcessData(HardwareSerial& mescSerial, HardwareSerial& compSerial, AsyncWebServer& webServer, AsyncWebSocket& webSocket);

void *memcat(void *dest, size_t dest_len, const void *src, size_t src_len); 
void processData(void *parameter);
void processLine(char *line);
void processConfig(char *line); 
int countCharOccurrences(const char* str, char ch);
void remove_ansi_escape_sequences(char *data);
void replace_pipe_with_tab(char *data);


#endif // PROCESSDATA_H
