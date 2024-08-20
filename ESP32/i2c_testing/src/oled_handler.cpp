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
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();

    while (1) {
      getLocalIPAddress();
      testdrawchar();
      vTaskDelay(2000 / portTICK_PERIOD_MS);
      testdrawrect();
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void testdrawchar(void) {
  display.clearDisplay();

  display.setTextSize(1);     
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.cp437(true); // Use full 256 char 'Code Page 437' font

  for(int16_t i=0; i<256; i++) {
    if(i == '\n') display.write(' ');
    else          display.write(i);
  }

  display.display();
}

void testdrawrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
}

