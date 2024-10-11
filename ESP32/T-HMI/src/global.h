#ifndef GLOBAL_H
#define GLOBAL_H

#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_LEDBackpack.h>
#include <WiFiUdp.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
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

#define HOR_PIXELS 240
#define VER_PIXELS 320

extern lv_disp_draw_buf_t draw_buf;
extern lv_color_t buf[HOR_PIXELS * 10];

// Declare external objects used across files
extern lv_obj_t * ehrz_btn;
extern lv_obj_t * amp_btn;
extern lv_obj_t * mph_btn;
extern lv_obj_t * bat_btn;
extern lv_obj_t * log_btn;
extern lv_obj_t * temp_btn;
extern lv_obj_t * controls_btn;

extern lv_obj_t * udpstatus_label;
extern lv_obj_t * btn_label;
extern lv_obj_t * coord_label;
extern lv_obj_t * ip_label;
extern lv_obj_t * local_label;
extern lv_obj_t * sdcard_label;
extern lv_obj_t * brightness_sw;
extern lv_obj_t * brightness_lbl;

extern lv_obj_t * lg_digit_lbl[3];
extern lv_obj_t * label_mph;

extern lv_obj_t * led;

extern lv_obj_t * controls_parent;
extern lv_obj_t * data_controls_parent;

extern Adafruit_AlphaNum4 alphaLED;
extern int segmentDisplayInt;
extern int segmentDisplayLetter;

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

extern unsigned long last_udp_receive; 

extern bool refreshDisplay;
extern LoggingState sdLoggingState;
extern LoggingRequest globalRequest;

extern AsyncWebSocket* g_webSocket;
extern HardwareSerial* g_mescSerial; 
extern HardwareSerial* g_compSerial;


#endif // GLOBAL_H
