#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "global.h"
#include "oled_handler.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initOLEDTask() {
  xTaskCreate(oledTask, "Oled Task", 1024, NULL, 1, NULL);
}

void oledTask(void *pvParameter) {
  Wire.begin(I2C_SDA, I2C_SCL);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    // Serial.println(F("SSD1306 allocation failed"));
    for(;;); 
  }

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();

  while (1) {
    display.setCursor(0,0);
    IPAddress ip = WiFi.localIP();
    display.println(ip);

    display.setCursor(106,0);
    display.print('*');
    display.display();
    vTaskDelay(500 / portTICK_PERIOD_MS);

    display.setCursor(106,0);
    display.fillRect(106, 0, 10, 10, BLACK);
    display.display();
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

