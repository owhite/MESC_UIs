// sd_card.h

#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h>
#include <SD.h>
#include <SD_MMC.h>
#include "FS.h"

// Enum for different logging commands
typedef enum {
    LOG_START,
    LOG_STOP,
    LOG_ADD_LINE
} LoggingCommandType;

// Struct for passing logging requests to the logging task
typedef struct {
  LoggingCommandType commandType;
  char logLine[2000];
} LoggingRequest;

// Declare the queue handle for logging requests
extern QueueHandle_t loggingQueue; 
extern SemaphoreHandle_t sdLoggingStateMutex;

// Function prototypes
void initSDCard();
bool isSDCardStillMounted();
const char* getLogFileName();
void sdLoggingTask(void* pvParameters);
void setLoggingState(bool logging);
void setLogFileName(char* name);
void clrLogFileName();
void openLogFile();
void addLineToLogFile(const char* line);
void closeLogFile();

#endif // SD_CARD_H
