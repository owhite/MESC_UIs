// this code was hacked from
//   https://github.com/thorsten-l/ESP32-TTGO-T4-T10-TestCode.git
//   to work with my T4-lillygo V1.3 board
// note this is an example of a platformio project where files are
//   held in /lib

#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <AsyncTCP.h>
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

#define IP5306_ADDR 0X75
#define IP5306_REG_SYS_CTL0 0x00

#define ROW_HEIGHT 30

uint8_t state = 1;
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

void button_handle(uint8_t gpio) {
  switch (gpio) {
  case BUTTON_1: {
    state = 1;
  } break;

  case BUTTON_2: {
    state = 2;
  } break;

  case BUTTON_3: {
    state = 3;
  } break;

  default:
    break;
  }
}

void button_callback(Button2 &b) {
  for (int i = 0; i < sizeof(g_btns) / sizeof(g_btns[0]); ++i) {
    if (pBtns[i] == b) {
      compSerial.printf("btn: %u press\n", pBtns[i].getAttachPin());
      button_handle(pBtns[i].getAttachPin());
    }
  }
}

void button_init() {
  uint8_t args = sizeof(g_btns) / sizeof(g_btns[0]);
  pBtns = new Button2[args];
  for (int i = 0; i < args; ++i) {
    pBtns[i] = Button2(g_btns[i]);
    pBtns[i].setPressedHandler(button_callback);
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
  }
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

  sdSPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);

  if (!SD.begin(SD_CS, sdSPI)) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("SDCard MOUNT FAIL", tft.width() / 2,
		   tft.height() / 2);
  } else {
    uint32_t cardSize = SD.cardSize() / (1024 * 1024);
    String str = "SDCard Size: " + String(cardSize) + "MB";
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString(str, tft.width() / 2, tft.height() / 2);
  }

  button_init();
  readConfig();

  initSDCard(compSerial, mescSerial);
  initUDPService();

  Wire.begin(I2C_SDA, I2C_SCL);
  btnscanT.attach_ms(30, button_loop);
}

void loop() {
  switch (state) {
  case 1:
    state = 0;
    tft.setTextFont(2);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Button 1: status", 0, tft.height() - (3 * ROW_HEIGHT));
    tft.drawString("Button 2: log", 0, tft.height() - (2 * ROW_HEIGHT));
    tft.drawString("Button 3: speedo", 0, tft.height() - (1 * ROW_HEIGHT));

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

      tft.print("LOCAL:  ");
      tft.println(WiFi.localIP());
    }

    break;
  case 2:
    state = 0;
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Button 2", tft.width() / 2, tft.height() / 2);
    break;
  case 3:
    state = 0;
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Button 3", tft.width() / 2, tft.height() / 2);
    break;
  default:
    break;
  }
}
