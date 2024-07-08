#include "processData.h"

char serialBuffer[BUFFER_SIZE];
int bufferIndex = 0;
SemaphoreHandle_t mutex;
unsigned long lastReceiveTime = 0;

// Thread to receive serial data
void initProcessData() {
  mutex = xSemaphoreCreateMutex();
  if (mutex == NULL) {
    Serial.println("Mutex creation failed!");
    while (true);
  }

  // Create the task to process the serial data
  xTaskCreate(
	      processData,     // Task function
	      "Process Data",  // Name of the task
	      2048,            // Stack size
	      NULL,            // Parameter
	      1,               // Priority
	      NULL             // Task handle
	      );
}

void *memcat(void *dest, size_t dest_len, const void *src, size_t src_len) {
  void *concat_position = (char *)dest + dest_len;
  memcpy(concat_position, src, src_len);
  return dest;
}

// Function to process a complete line
void processLine(char *line) {
  Serial.print("Processing data: ");
  Serial.println(line);
}

void processData(void *parameter) {
  char localBuffer[BUFFER_SIZE];
  int localBufferIndex = 0;

  while (true) {
    if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
      if (bufferIndex > 0) {
	memcat(localBuffer, localBufferIndex, serialBuffer, bufferIndex);
	localBufferIndex += bufferIndex;
	bufferIndex = 0; // clear global buffer
	serialBuffer[bufferIndex] = '\0';
	lastReceiveTime = millis();
      }
      xSemaphoreGive(mutex);

      // Check if the local buffer has a complete line (ending with '\n')
      for (int i = 0; i < localBufferIndex; i++) {
	if (localBuffer[i] == '\n') {
	  localBuffer[i] = '\0';
	  processLine(localBuffer);
	  int remainingLength = localBufferIndex - (i + 1);
	  memmove(localBuffer, localBuffer + i + 1, remainingLength);
	  localBufferIndex = remainingLength;
	  i = -1; 
	}
      }

      // Check if timeout occurred and there is data in the buffer.
      //  happens when Jens' term leaves the "usb@MESC>" prompt
      //  with no end of line
      if (localBufferIndex > 0 && (millis() - lastReceiveTime) >= SERIAL_TIMEOUT_MS) {
	localBuffer[localBufferIndex] = '\0'; 
	processLine(localBuffer); 
	localBufferIndex = 0; 
      }
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
