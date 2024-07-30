// global.h
#ifndef GLOBAL_H
#define GLOBAL_H

#include <ArduinoJson.h>

#define compSerial Serial
#define mescSerial Serial2

extern float globalVar2;
extern char globalArray[100];

extern DynamicJsonDocument jsonDoc;

extern int commState;

#define COMM_IDLE      0
#define COMM_GET       1
#define COMM_SEND      2

// This can make function declarations (if needed)
// void someGlobalFunction();

#endif // GLOBAL_H
