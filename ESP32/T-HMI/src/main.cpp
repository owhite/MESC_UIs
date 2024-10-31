#include "pins.h"
#include <SPI.h>
#include <Arduino.h>
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

int commState = COMM_IDLE;
int displayState = DISPLAY_BAT;

LoggingState sdLoggingState = {false, "", File(), NULL};

bool rlst = false;

int segmentDisplayInt;
int segmentDisplayLetter = 'E';

Adafruit_AlphaNum4 alphaLED = Adafruit_AlphaNum4();
TwoWire customWire = TwoWire(0);

LoggingRequest logRequest;
QueueHandle_t loggingQueue;
QueueHandle_t displayQueue;

// run the LED segments
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

  tft.begin();         
  tft.setRotation(1);  
  tft.setSwapBytes(true);
  tft.setTextFont(2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);

  readConfig();

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.print("touch init: ");
  touchInit();

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.print("read SD: ");
  // initSDCard(); 

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.print("start internet: ");
  // initUDPService();

  setupGUI();

  xTaskCreate(segmentTask, "Segment Task", 4096, NULL, 1, NULL);
}

void loop() {
  lv_timer_handler(); 
  delay(5); 
}
