#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "processData.h"
#include <ArduinoJson.h>
#include "webservice.h"

char serialBuffer[BUFFER_SIZE];
int bufferIndex = 0;
SemaphoreHandle_t mutex;
unsigned long lastReceiveTime = 0;

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

void processData(void *parameter) {
    char localBuffer[BUFFER_SIZE];
    int localBufferIndex = 0;

    while (true) {

      while (Serial.available()) {
	char incomingByte = Serial.read();
	if (bufferIndex < BUFFER_SIZE - 1) {
	  serialBuffer[bufferIndex++] = incomingByte;
	  serialBuffer[bufferIndex] = '\0'; 
	}
      }

      if (bufferIndex > 0) {
	memcat(localBuffer, localBufferIndex, serialBuffer, bufferIndex);
	localBufferIndex += bufferIndex;
	bufferIndex = 0; // clear global buffer
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

      if (localBufferIndex > 0 && (millis() - lastReceiveTime) >= SERIAL_TIMEOUT_MS) {
	localBuffer[localBufferIndex] = '\0'; 
	processLine(localBuffer); 
	localBufferIndex = 0; 
      }
      /* 

      */ 

      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}


void processLine(char *line) {
    int count = countCharOccurrences(line, '\t');
    if (count == 4) {
        stringToJSON(line);
    }
}

void stringToJSON(const char* data) {
    // Create a JSON document
    StaticJsonDocument<256> doc;

    // Make a mutable copy of the input string
    char inputCopy[256];
    strncpy(inputCopy, data, sizeof(inputCopy));
    inputCopy[sizeof(inputCopy) - 1] = '\0'; // Ensure null-termination
    
    // Parse the input string
    char* token = strtok(inputCopy, "\t");
    int index = 0;
    const char* field1 = "";
    const char* field2 = "";
    const char* field3 = "";
    const char* field4 = "";
    const char* field5 = "";

    while (token != NULL) {
        if (index == 0) field1 = token;
        else if (index == 1) field2 = token;
        else if (index == 2) field3 = token;
        else if (index == 3) field4 = token;
        else if (index == 4) field5 = token;
        
        token = strtok(NULL, "\t");
        index++;
    }
    
    // Populate the JSON document
    doc["parameter"] = field1;
    doc["value"] = field2;
    doc["min"] = field3;
    doc["max"] = field4;
    doc["description"] = field5;

    // Serialize JSON to string
    char output[256];
    serializeJson(doc, output, sizeof(output));
  
    Serial.println(output);

    webSocket.broadcastTXT(output);
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
