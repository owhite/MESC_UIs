#ifndef GLOBAL_H
#define GLOBAL_H

#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include <ESPAsyncWebServer.h>
#include <WiFiUdp.h>

extern float globalVar2;
extern char globalArray[100];

extern DynamicJsonDocument jsonDoc;

extern int commState;

#define BUFFER_SIZE 526
#define BIG_BUFFER_SIZE 10000


#define COMM_IDLE      0
#define COMM_GET       1
#define COMM_SEND      2
#define COMM_LOG       3
#define COMM_ESP32     4

String getLocalIPAddress();

// extern WiFiUDP udp; 
extern WiFiServer tcpServer;

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
    String fileName;
    File fileHandle;
    SemaphoreHandle_t mutex;  // Protect access to shared state
};

extern LoggingState sdLoggingState;

extern AsyncWebSocket* g_webSocket;
extern HardwareSerial* g_mescSerial; 
extern HardwareSerial* g_compSerial;

#endif // GLOBAL_H
