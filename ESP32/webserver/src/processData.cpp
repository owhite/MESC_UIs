#include <Arduino.h>
#include <AsyncWebSocket.h> // Ensure this is included
#include "global.h"
#include "processData.h"

#define BUFFER_SIZE 526

char serialBuffer[BUFFER_SIZE];
int bufferIndex = 0;
unsigned long lastReceiveTime = 0;

static AsyncWebSocket* g_webSocket;
static HardwareSerial* g_mescSerial; 
static HardwareSerial* g_compSerial;

void initProcessData(HardwareSerial& mescSerial, HardwareSerial& compSerial, AsyncWebServer& server, AsyncWebSocket& webSocket) {
  g_mescSerial = &mescSerial;
  g_compSerial = &compSerial;
  g_webSocket = &webSocket;

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

void processData(void *parameter) {
  char localBuffer[BUFFER_SIZE];
  int localBufferIndex = 0;
  char ch;

  while (true) { // the equivalent of arduino loop()

    while (g_mescSerial->available()) {
      char incomingByte = g_mescSerial->read();

      if (bufferIndex < BUFFER_SIZE - 1) {
        serialBuffer[bufferIndex++] = incomingByte;
        serialBuffer[bufferIndex] = '\0';
      }
    }

    if (bufferIndex > 0) {
      memcat(localBuffer, localBufferIndex, serialBuffer, bufferIndex);
      localBufferIndex += bufferIndex;
      bufferIndex = 0;
      serialBuffer[bufferIndex] = '\0';
      lastReceiveTime = millis();
    }

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

    // in general this means we have reached the prompt of Jens' term
    if (localBufferIndex > 0 && (millis() - lastReceiveTime) >= SERIAL_TIMEOUT_MS) {
      localBuffer[localBufferIndex] = '\0';
      processLine(localBuffer);
      localBufferIndex = 0;

      // server performed a get, and we have loaded up a bunch of values into
      //  the json string, so send that by websocket
      if (commState == COMM_GET || commState == COMM_IDLE) {
	// I hate arduino strings, but...
	String jsonString; 
	serializeJson(jsonDoc, jsonString);
	Serial.println(jsonString);
	g_webSocket->textAll(jsonString);
	jsonDoc.clear();
      }

      commState = COMM_IDLE;
    }

    while (g_compSerial->available()) {
      ch = g_compSerial->read();
      g_mescSerial->write(ch);
      g_compSerial->write(ch);
    }

    // vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void processLine(char *line) {
  remove_ansi_escape_sequences(line);
  replace_pipe_with_tab(line);

  // use ful debugging
  // g_compSerial->println(line);

  if (commState == COMM_GET || commState == COMM_IDLE) {
    int count = countCharOccurrences(line, '\t');

    // if we're in the middle of a get it should always be count == 4
    if (count == 4) {
      char value1[20];
      char value2[20];

      const char* tab1 = strchr(line, '\t');
      if (tab1 != NULL) {
        size_t length1 = tab1 - line;
        strncpy(value1, line, length1);
        value1[length1] = '\0';

        const char* tab2 = strchr(tab1 + 1, '\t');
        if (tab2 != NULL) {
            size_t length2 = tab2 - (tab1 + 1);
            strncpy(value2, tab1 + 1, length2);
            value2[length2] = '\0';
        } else {
            // should be a second tab,
	    //  but if not take the remaining part of the string
            strcpy(value2, tab1 + 1);
        }
      }

      // avoid running websocket->send on each line
      //   dynamically concatenate to json string
      // Serial.println(value1);
      // Serial.println(value2);
      jsonDoc[value1] = value2;
    }
  }
}

// Function to remove ANSI escape sequences
void remove_ansi_escape_sequences(char *data) {
  char buffer[strlen(data) + 1];
  int bufferIndex = 0;
  int i = 0;

  while (data[i] != '\0') {
    if (data[i] == '\x1B') {
      i++;
      if (data[i] == '[') {
        i++;
        while ((data[i] >= '0' && data[i] <= '?') || (data[i] >= ' ' && data[i] <= '/')) {
          i++;
        }
        if (data[i] >= '@' && data[i] <= '~') {
          i++;
        }
      }
    } else {
      buffer[bufferIndex++] = data[i++];
    }
  }

  buffer[bufferIndex] = '\0';
  strcpy(data, buffer);
}

// Function to replace "| " with "\t"
void replace_pipe_with_tab(char *data) {
  char *pos;
  while ((pos = strstr(data, "| ")) != NULL) {
    *pos = '\t';
    memmove(pos + 1, pos + 2, strlen(pos + 2) + 1);
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
