#ifndef GLOBAL_H
#define GLOBAL_H

#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include <ESPAsyncWebServer.h>
#include <WiFiUdp.h>

extern DynamicJsonDocument jsonDoc;

#define BUFFER_SIZE 526
#define BIG_BUFFER_SIZE 10000

extern int configState;
extern int logState;
extern bool graphingState;

extern WiFiUDP udpReceiver;
extern WiFiUDP udpSender;

#define CONFIG_IDLE      0
#define CONFIG_SEND      1
#define CONFIG_ESP32     2 // changing config of ESP32
#define CONFIG_GRAPH     3 

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

typedef struct {
  char displayLine[5000];
  int displayValue;
  float Vd;
  float Vq;
} DisplayDataRequest;

#endif // GLOBAL_H
