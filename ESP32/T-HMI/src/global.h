#ifndef GLOBAL_H
#define GLOBAL_H

#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include <ESPAsyncWebServer.h>
#include <WiFiUdp.h>
#include <TFT_eSPI.h>
#include "sd_card.h"

extern DynamicJsonDocument jsonDoc;
extern int commState;

extern TFT_eSPI tft;

#define BUFFER_SIZE 526
#define BIG_BUFFER_SIZE 10000

#define COMM_IDLE      0
#define COMM_GET       1
#define COMM_SEND      2
#define COMM_LOG       3
#define COMM_ESP32     4

String getLocalIPAddress();

struct Config {
  char ssid[32];
  char password[32];
  char device_name[32];
  char remote_IP[32];
  uint8_t remote_IP_array[4];
  char local_IP[32];
  uint8_t local_IP_array[4];
  float sensor1_threshold;
  bool debug_mode;
  bool access_point;
  int log_interval;
};

extern Config config;

struct LoggingState {
    bool isLogging;
    char fileName[32];
    File fileHandle;
    SemaphoreHandle_t mutex;  // Protect access to shared state
};

extern bool refreshDisplay;
extern LoggingState sdLoggingState;
extern LoggingRequest globalRequest;

extern AsyncWebSocket* g_webSocket;
extern HardwareSerial* g_mescSerial; 
extern HardwareSerial* g_compSerial;

#endif // GLOBAL_H
