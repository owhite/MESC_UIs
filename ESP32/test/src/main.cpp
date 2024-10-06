#include "pins.h"
#include <SPI.h>
#include <SD.h>
#include <SD_MMC.h>
#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <xpt2046.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include <lv_conf.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>

#include "gui.h"
#include "processConfig.h"
#include "udpService.h"
#include "global.h"
#include "sd_card.h"
#include "touch.h"

TFT_eSPI tft = TFT_eSPI();

LoggingRequest globalRequest;
int commState = 0;
DynamicJsonDocument jsonDoc(5024);

AsyncWebSocket* g_webSocket = nullptr;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

LoggingState sdLoggingState = {false, "", File(), NULL};
bool refreshDisplay = true;

int number = 0;  // Variable to store the current number

bool is_image1 = true;
int segmentDisplayInt = 123;
int segmentDisplayLetter = 'E';

bool rlst = false;

Adafruit_AlphaNum4 alphaLED = Adafruit_AlphaNum4();
TwoWire customWire = TwoWire(0);

void segmentTask(void * parameter) {
  customWire.begin(43, 44);
  alphaLED.begin(0x70, &customWire);

  while (true) {
    String str = String(segmentDisplayInt);
    if (str.length() < 4) {
      alphaLED.clear();
      for(int i=0;i < int(str.length()); i++) {
        alphaLED.writeDigitAscii(i+(4-str.length()), str.charAt(i));
      }
      alphaLED.writeDigitAscii(0, segmentDisplayLetter);
      alphaLED.writeDisplay();
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting setup...");

  lv_init();           
  pinMode(PWR_EN_PIN, OUTPUT);
  digitalWrite(PWR_EN_PIN, HIGH);

  touchInit();
  initSDCard();
  setupGUI();
  readConfig();

  // we <3 FreeRTOS tasks
 xTaskCreate(segmentTask, "Segment Task", 4096, NULL, 1, NULL);
}

void loop() {
  lv_timer_handler(); 
  delay(5); 
}
