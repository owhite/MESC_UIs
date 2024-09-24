// this code was hacked from
//   https://github.com/thorsten-l/ESP32-TTGO-T4-T10-TestCode.git
//   to work with my T4-lillygo V1.3 board
// note this is an example of a platformio project where files are
//   held in /lib

#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <Button2.h>
#include <SD.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include "T4_V13.h"
#include "udpService.h"
#include "processConfig.h"
#include "global.h"
#include "sd_card.h"
// used by buttons -- wonder if it's in conflict with other threads
#include <Ticker.h> 

TFT_eSPI tft = TFT_eSPI(); 

SPIClass sdSPI(VSPI);
LoggingRequest globalRequest;

#define IP5306_ADDR 0X75
#define IP5306_REG_SYS_CTL0 0x00

#define ROW_HEIGHT 30

bool refreshDisplay = true;
uint8_t lastButton = 1; 

Button2 *pBtns = nullptr;
uint8_t g_btns[] = BUTTONS_MAP;
char buff[512];
Ticker btnscanT;

int commState = 0;
DynamicJsonDocument jsonDoc(5024);

HardwareSerial* g_compSerial = nullptr;
HardwareSerial* g_mescSerial = nullptr;
AsyncWebSocket* g_webSocket = nullptr;

HardwareSerial mescSerial(1);
#define compSerial Serial

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

LoggingState sdLoggingState = {false, "", File(), NULL};


bool setPowerBoostKeepOn(int en) {
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(IP5306_REG_SYS_CTL0);
  if (en)
    Wire.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
  else
    Wire.write(0x35); // 0x37 is default reg value
  return Wire.endTransmission() == 0;
}

void buttonHandle(uint8_t gpio) {
  switch (gpio) {
  case BUTTON_1: {
    globalRequest.commandType = LOG_START;
    
    if (xQueueSend(loggingQueue, &globalRequest, portMAX_DELAY) != pdPASS) {
      g_compSerial->println("Failed to send LOG_START request to queue.");
    }
    refreshDisplay = true;

    lastButton = 1;
    udpSend((char*) "LOG_START:");
    refreshDisplay = true;
  } break;

  case BUTTON_2: {
    globalRequest.commandType = LOG_STOP;

    if (xQueueSend(loggingQueue, &globalRequest, portMAX_DELAY) != pdPASS) {
      g_compSerial->println("Failed to send LOG_STOP request to queue.");
    }

    lastButton = 2;
    udpSend((char*) "LOG_STOP:");
    refreshDisplay = true;
  } break;

  case BUTTON_3: {
    lastButton = 3;
    udpSend((char*) "3");
    refreshDisplay = true;
  } break;

  default:
    break;
  }
}

void buttonCallback(Button2 &b) {
  for (int i = 0; i < sizeof(g_btns) / sizeof(g_btns[0]); ++i) {
    if (pBtns[i] == b) {
      // compSerial.printf("btn: %u press\n", pBtns[i].getAttachPin());
      buttonHandle(pBtns[i].getAttachPin());
    }
  }
}

void buttonInit() {
  uint8_t args = sizeof(g_btns) / sizeof(g_btns[0]);
  pBtns = new Button2[args];
  for (int i = 0; i < args; ++i) {
    pBtns[i] = Button2(g_btns[i]);
    pBtns[i].setPressedHandler(buttonCallback);
  }

  pBtns[0].setLongClickHandler([](Button2 &b) {
    int x = tft.width() / 2;
    int y = tft.height() / 2 - ROW_HEIGHT;
    int r = digitalRead(TFT_BL);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.fillScreen(TFT_BLACK);
    tft.drawString(r ? "Backlight OFF" : "Backlight ON", x, y);
    tft.drawString("IP5306 KeepOn ", x - 20, y + ROW_HEIGHT);

    bool isOk = setPowerBoostKeepOn(1);
    tft.setTextColor(isOk ? TFT_GREEN : TFT_RED, TFT_BLACK);
    tft.drawString(isOk ? "PASS" : "FAIL", x + 50, y + ROW_HEIGHT);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("Press again to wake up", x - 20, y + ROW_HEIGHT);

    delay(6000);
    digitalWrite(TFT_BL, !r);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_1, LOW);
    esp_deep_sleep_start();
  });

}

void button_loop() {
  for (int i = 0; i < sizeof(g_btns) / sizeof(g_btns[0]); ++i) {
    pBtns[i].loop();
  }
}

void spisd_test() {
  if (SD_CS > 0) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    sdSPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS, sdSPI)) {
      tft.setTextFont(2);
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.drawString("SDCard MOUNT FAIL", tft.width() / 2, tft.height() / 2);
    } else {
      uint32_t cardSize = SD.cardSize() / (1024 * 1024);
      String str = "SDCard Size: " + String(cardSize) + "MB";
      tft.setTextFont(2);
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.drawString(str, tft.width() / 2, tft.height() / 2);
    }
    delay(1000);
  }
}

void updateDisplay() {
  tft.setTextFont(2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);

  if (config.access_point) {
    tft.println(WiFi.softAPIP());
  }
  else {
    tft.setCursor(0, 0);
    tft.print("REMOTE: ");
    tft.print(config.remote_IP_array[0]);
    tft.print(".");
    tft.print(config.remote_IP_array[1]);
    tft.print(".");
    tft.print(config.remote_IP_array[2]);
    tft.print(".");
    tft.println(config.remote_IP_array[3]);

  }
  tft.print("LOCAL:  ");
  tft.println(WiFi.localIP());

  uint32_t cardSize = SD.cardSize() / (1024 * 1024);
  String str = "SDCard Size: " + String(cardSize) + "MB";
  tft.println(str);

  tft.print("Last button press: ");
  tft.println(lastButton);
 
  if (sdLoggingState.isLogging) {
    tft.println("LOGGING");
  }
  else {
    tft.println("NO LOGGING");
  }

  tft.print("SD file name: ");
  tft.println(getLogFileName());

  tft.setCursor(0, 160);
  tft.println("Button 1: log start");
  tft.println("Button 2: log stop");
  tft.println("Button 3: speedo");
}

void setup() {
  compSerial.begin(115200);
  mescSerial.begin(115200);

  g_compSerial = &compSerial;
  g_mescSerial = &mescSerial;

  compSerial.println("Starting setup...");

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 0);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);

  // this will flash up on the screen...
  tft.drawString("SDCard MOUNT FAIL", tft.width() / 2, tft.height() / 2);
  // and then this just locks up....
  sdSPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS, sdSPI)) { 
    tft.drawString("SDCard MOUNT FAIL", tft.width() / 2, tft.height() / 2);
  }
  else {
    tft.drawString("SDCard INIT:", tft.width() / 2, tft.height() / 2);
  }

  buttonInit();
  readConfig();
  initSDCard(compSerial, mescSerial);

  initUDPService();

  Wire.begin(I2C_SDA, I2C_SCL);
  btnscanT.attach_ms(30, button_loop);
}

void loop() {
  if (refreshDisplay) {
    updateDisplay();
    refreshDisplay = false;
  }
}
