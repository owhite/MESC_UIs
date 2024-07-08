#include "blink.h"

#define BLINK_GPIO LED_BUILTIN
volatile int blinkSpeed = 400;  // Initial blink speed, in milliseconds


void initBlinkTask() {
  xTaskCreate(blinkTask, "Blink Task", 1024, NULL, 1, NULL);
}

void blinkTask(void *pvParameter) {
  pinMode(BLINK_GPIO, OUTPUT);

  while (1) {
    digitalWrite(BLINK_GPIO, HIGH);
    vTaskDelay(blinkSpeed / portTICK_PERIOD_MS);
    digitalWrite(BLINK_GPIO, LOW);
    vTaskDelay(blinkSpeed / portTICK_PERIOD_MS);
  }
}
