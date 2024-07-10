#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "processData.h"

char serialBuffer[BUFFER_SIZE];
char tableRows[4096];
int bufferIndex = 0;
SemaphoreHandle_t mutex;
unsigned long lastReceiveTime = 0;

WebSocketsServer webSocket = WebSocketsServer(81);

void initProcessData() {
  mutex = xSemaphoreCreateMutex();
  if (mutex == NULL) {
    Serial.println("Mutex creation failed!");
    while (true);
  }

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

void processLine(char *line) {
  int count = countCharOccurrences(line, '\t');
  Serial.println(count);
  if (count == 4) {
    stringToTableRow(line);
  }
}

void stringToTableRow(const char* data) {
  char row[1024] = "<tr>"; 
  int fieldNum = 1;
  char delimiter = '\t';

  size_t dataLength = strlen(data);
  size_t index = 0;
  size_t rowIndex = 0;

  while (index <= dataLength) {
    if (data[index] == delimiter || data[index] == '\n' || data[index] == '\0') {
      strcat(row, "<td>");
      if (fieldNum == 1) {
        char field[256]; 
        snprintf(field, sizeof(field), "<input type='text' name='field1' value='%.*s'>", (int)(index - rowIndex), data + rowIndex);
        strcat(row, field);
      } else {
        strncat(row, data + rowIndex, index - rowIndex);
      }
      strcat(row, "</td>");
      fieldNum++;
      rowIndex = index + 1; 
    }

    if (data[index] == '\n' || data[index] == '\0') {
      strcat(row, "</tr>");
      break; 
    }

    index++;
  }

  // Append the new row to the global tableRows buffer
  if (strlen(tableRows) + strlen(row) < sizeof(tableRows)) {
    strcat(tableRows, row);
    // Send the updated row to all connected clients
    webSocket.broadcastTXT(row);
  } else {
    Serial.println("Error: tableRows buffer overflow.");
  }
  Serial.println(":::::");
  Serial.println(tableRows);
  Serial.println(":::::");
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

      if (localBufferIndex > 0 && (millis() - lastReceiveTime) >= SERIAL_TIMEOUT_MS) {
        localBuffer[localBufferIndex] = '\0'; 
        processLine(localBuffer); 
        localBufferIndex = 0; 
      }
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    Serial.printf("[%u] get Text: %s\n", num, payload);
    // Process incoming WebSocket data here
  }
}
