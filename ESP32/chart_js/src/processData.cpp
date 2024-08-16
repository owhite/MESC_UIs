#include <Arduino.h>
#include <AsyncWebSocket.h>
#include "global.h"
#include "processData.h"
#include "processConfig.h"

int bufferIndex = 0;
unsigned long lastReceiveTime = 0;
char *serialBuffer = nullptr; 
char *localBuffer = nullptr; 

void initProcessData(HardwareSerial& mescSerial, HardwareSerial& compSerial, AsyncWebServer& server, AsyncWebSocket& webSocket) {
  g_mescSerial = &mescSerial;
  g_compSerial = &compSerial;
  g_webSocket = &webSocket;

  serialBuffer = (char *)malloc(BIG_BUFFER_SIZE * sizeof(char));

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
  // char localBuffer[BIG_BUFFER_SIZE];
  localBuffer = (char *)malloc(BIG_BUFFER_SIZE * sizeof(char));

  int localBufferIndex = 0;
  char ch;

  while (true) {
    if (commState == COMM_ESP32) {
      handleESP32Config(localBuffer, localBufferIndex, serialBuffer, bufferIndex);
    } else {
      while (g_mescSerial->available()) {
	char incomingByte = g_mescSerial->read();
	if (bufferIndex < BIG_BUFFER_SIZE - 1) {
	  if (incomingByte == '\n') {
	    processLine(serialBuffer);
	    bufferIndex = 0;
	  }
	  else {
	    serialBuffer[bufferIndex] = incomingByte;
	    serialBuffer[bufferIndex+1] = '\0';
	    bufferIndex++;
	  }
	}
	else {
	  g_compSerial->println("broke string length");
	  g_compSerial->println(bufferIndex);
	}
	lastReceiveTime = millis();
      }

      // we're probably here because we're just sitting at the jens prompt
      if (bufferIndex > 0 && (millis() - lastReceiveTime) >= SERIAL_TIMEOUT_MS) {
	if (commState == COMM_GET || commState == COMM_IDLE) {
	  String jsonString; 
	  serializeJson(jsonDoc, jsonString);
	  g_webSocket->textAll(jsonString);
	  jsonDoc.clear();
	  bufferIndex = 0;
	}
	commState = COMM_IDLE;
      }

      while (g_compSerial->available()) {
	ch = g_compSerial->read();
	g_mescSerial->write(ch);
	g_compSerial->write(ch);
	if (ch == '>') {
	  bufferIndex = 0;
	  localBufferIndex = 0;
	  initESP32Config();
	}
      }
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void processLine(char *line) {

  // the user requested a 'log -fl':
  //  avoiding a test for: (commState == COMM_LOG)
  if (strncmp(line, "{\"time\":", 8) == 0) {
    g_webSocket->textAll(line);
    // commState = COMM_IDLE;
  }
  else {
    remove_ansi_escape_sequences(line);
    replace_pipe_with_tab(line);
    // this is also avoid testing the communication state
    if (commState == COMM_GET || commState == COMM_IDLE) {
      int count = countCharOccurrences(line, '\t');

      if (count == 4) {
	g_compSerial->println(line);

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
	    strcpy(value2, tab1 + 1);
	  }
	}

	jsonDoc[value1] = value2;
      }
    }
  }
}

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
