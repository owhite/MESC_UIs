#include <cstring>
#include <cstdio>
#include <cstdlib>
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
	      4048,            // Stack size
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
  int count = countCharOccurrences(line, '\t');
  Serial.print(" :: ");
  Serial.println(count);
  if (count == 4) {
    Serial.println("making row");
    stringToTableRow(line);
  }
}

int countCharOccurrences(const char* str, char ch) {
    int count = 0;
    while (*str) {
        if (*str == ch) {
            count++;
        }
        str++;
    }
    return count;
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

void stringToTableRow(const char* data) {
    char row[1024];
    char tableRows[4096] = "";  // Adjust size as needed for your application
    int fieldNum = 1;
    char delimiter = '\t';

    size_t dataLength = strlen(data);
    size_t index = 0;
    size_t rowIndex = 0;

    while (index < dataLength) {
        if (data[index] == delimiter || data[index] == '\n') {
            strcat(row, "<td>");
            if (fieldNum == 1) {
                char field[128];
                snprintf(field, sizeof(field), "<input type='text' name='field1' value='%.*s'>", (int)(index - rowIndex), data + rowIndex);
                strcat(row, field);
            } else {
                strncat(row, data + rowIndex, index - rowIndex);
            }
            strcat(row, "</td>");
            fieldNum++;
            rowIndex = index + 1;  // Move past the delimiter
        }

        if (data[index] == '\n') {
            strcat(row, "</tr>");
            strcat(tableRows, row);
            strcpy(row, "<tr>");
            fieldNum = 1;
            rowIndex = index + 1;  // Move past the newline
        }

        index++;
    }
}
