#ifndef PROCESSDATA_H
#define PROCESSDATA_H

#include <Arduino.h>

#define BUFFER_SIZE 526
#define SERIAL_TIMEOUT_MS 100

#define compSerial  Serial  // computer keyboard to ESP32 USB
#define mescSerial  Serial2 // data from MP2 to ESP32

extern char serialBuffer[BUFFER_SIZE];
extern int bufferIndex;
extern unsigned long lastReceiveTime;

void initProcessData();
void processData(void *parameter);
void processLine(char *line);
void sendThing();
void stringToJSON(const char* data);
int countCharOccurrences(const char* str, char ch);

#endif // PROCESSDATA_H
