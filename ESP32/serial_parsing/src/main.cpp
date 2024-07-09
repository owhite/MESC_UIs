#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "processData.h"
#include "blink.h"
#include "webservice.h"

void setup() {
  Serial.begin(115200);

  initProcessData();
  initBlinkTask();
  // initWebService();

}

void loop() {
    while (!Serial) {
        delay(100);
    }

    while (Serial.available()) {
        char incomingByte = Serial.read();
        
        if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
            if (bufferIndex < BUFFER_SIZE - 1) {
                serialBuffer[bufferIndex++] = incomingByte;
                serialBuffer[bufferIndex] = '\0'; 
                lastReceiveTime = millis(); 
            }
            xSemaphoreGive(mutex);
        }
    }
}

