#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "processData.h"
#include <ArduinoJson.h>

char serialBuffer[BUFFER_SIZE];
int bufferIndex = 0;
unsigned long lastReceiveTime = 0;

void initProcessData() {

  compSerial.begin(115200);
  // mescSerial.begin(115200, SERIAL_8N1, 27, 25);
  mescSerial.begin(9600, SERIAL_8N1, 25, 27);
  Serial.println("Loopback program started");

  compSerial.println("Starting up...");

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

  while (true) { // equivalent of loop()

    while (mescSerial.available()){
      ch = mescSerial.read();
      compSerial.write(ch);
    }

    while (compSerial.available()) {
      ch = compSerial.read();
      compSerial.write(ch);
      if (ch == 'g') {
	mescSerial.write("get vbus\r\n");
      }
    }

    mescSerial.write("get vbus\r\n");

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}


void sendThing() {
  // compSerial.println("send: get");
  // mescSerial.write("get vbus\r\n");
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

  // serialize JSON to string
  char output[256];
  serializeJson(doc, output, sizeof(output));
  
  mescSerial.println(output);
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
