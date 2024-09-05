// sd_card.h

#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h>
#include "FS.h"
#include "SD.h"

// Enum for different logging commands
typedef enum {
    LOG_START,
    LOG_STOP,
    LOG_ADD_LINE
} LoggingCommandType;

// Struct for passing logging requests to the logging task
typedef struct {
    LoggingCommandType commandType;  // Type of command (start, stop, add line)
    String logLine;                  // Used for adding lines to the log file
} LoggingRequest;

// Declare the queue handle for logging requests
extern QueueHandle_t loggingQueue; 
extern SemaphoreHandle_t sdLoggingStateMutex;

// Function prototypes
void initSDCard(HardwareSerial& compSerial, HardwareSerial& mescSerial);
void sdLoggingTask(void* pvParameters);
void setLoggingState(bool logging);
void setLogFileName(const char* name);
void openLogFile();
void addLineToLogFile(const char* line);
void closeLogFile();

#endif // SD_CARD_H
