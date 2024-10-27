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
#include "gui.h"

extern DynamicJsonDocument jsonDoc;

extern TFT_eSPI tft;

#define BUFFER_SIZE 526

extern int commState;
#define COMM_IDLE      0
#define COMM_GET       1
#define COMM_SEND      2
#define COMM_LOG       3
#define COMM_ESP32     4

extern int displayState;
#define DISPLAY_IDLE    0
#define DISPLAY_EHRZ    1
#define DISPLAY_MPH     2
#define DISPLAY_ADC     3
#define DISPLAY_AMP     4
#define DISPLAY_BAT     5
#define DISPLAY_TMOT    6
#define DISPLAY_TMOS    7
#define DISPLAY_ERR     8

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
extern lv_obj_t * coord_label;
extern lv_obj_t * ip_label;
extern lv_obj_t * local_label;
extern lv_obj_t * sdcard_label;
extern lv_obj_t * logname_label;
extern lv_obj_t * brightness_sw;
extern lv_obj_t * brightness_lbl;

extern lv_obj_t * data_label;

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
extern LoggingState sdLoggingState;

// Struct for passing logging requests to the logging task
typedef struct {
  LoggingCommandType commandType;
  char logLine[2000];
} LoggingRequest;

// Declare the queue handle for logging requests
extern QueueHandle_t displayQueue; 
extern QueueHandle_t loggingQueue; 
extern SemaphoreHandle_t sdLoggingStateMutex;

typedef struct {
  float amp;
  float bat;
  int ehz;
  int mph;
} DisplayDataRequest;

typedef struct {
  char displayLine[5000];
} DisplayRequest;

extern LoggingRequest logRequest;
extern DisplayDataRequest displayDataRequest;

extern unsigned long last_udp_receive; 
extern unsigned long last_data_receive; 
extern AsyncWebSocket* g_webSocket;
extern HardwareSerial* g_mescSerial; 
extern HardwareSerial* g_compSerial;


#endif // GLOBAL_H
