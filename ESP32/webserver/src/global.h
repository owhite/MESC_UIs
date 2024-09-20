#ifndef GLOBAL_H
#define GLOBAL_H

#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include <ESPAsyncWebServer.h>
#include <WiFiUdp.h>

extern float globalVar2;
extern char globalArray[100];

extern DynamicJsonDocument jsonDoc;

#define BUFFER_SIZE 526
#define BIG_BUFFER_SIZE 10000

extern int commState;
extern int logState;

extern WiFiUDP udpReceiver;
extern WiFiUDP udpSender;

#define COMM_IDLE      0
#define COMM_GET       1
#define COMM_SEND      2
#define COMM_ESP32     3 // changing config of ESP32
#define COMM_GRAPH     4 

#define LOG_IDLE          0 
#define LOG_REQUEST_IDLE  1 // aka "stop logging"
#define LOG_REQUEST_GET   2 // aka "start logging"
#define LOG_GET           3
#define LOG_REQUEST_JSON  4
#define LOG_JSON          5

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

extern AsyncWebSocket* g_webSocket;
extern HardwareSerial* g_mescSerial; 
extern HardwareSerial* g_compSerial;

#endif // GLOBAL_H
