// processConfig.h

#ifndef PROCESSCONFIG_H
#define PROCESSCONFIG_H

#include <Arduino.h>
#include "global.h"
#include "processData.h"

void initESP32Config();
void handleESP32Config(char* localBuffer, int& localBufferIndex, char* serialInput, int& bufferIndex);
void processConfig(char* line);
void consolidateSpaces(char* input);
void getConfig();
void readConfig();
void writeConfig();

#endif // PROCESSCONFIG_H
