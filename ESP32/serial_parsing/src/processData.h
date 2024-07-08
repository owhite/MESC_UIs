#ifndef PROCESS_DATA_H
#define PROCESS_DATA_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

// Buffer size for serial data
#define BUFFER_SIZE 1024
#define SERIAL_TIMEOUT_MS 300

// Function prototypes
void initProcessData();
void processData(void *parameter);
void *memcat(void *dest, size_t dest_len, const void *src, size_t src_len);
void processLine(char *line);

// External declarations of global variables
extern char serialBuffer[];
extern int bufferIndex;
extern SemaphoreHandle_t mutex;
extern unsigned long lastReceiveTime;

#endif // PROCESS_DATA_H
