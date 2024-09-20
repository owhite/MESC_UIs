#include <Arduino.h>
#include <AsyncWebSocket.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "global.h"
#include "processData.h"
#include "processConfig.h"
#include "webservice.h"

// manages serial data from the mesc controller and esp32's 
//  -user connects to serial via ESP32 USB
//  -ESP32 also receives serial data from MESC via UART
//  -if user typed '>' sets state = COMM_ESP32 and updates the config file
//  -if not in COMM_ESP32 state and user types commands, sends those to mesc
//  -when data comes from MESC loads up jsonString, among other thigns
//  -when appropriate data is is pumped to g_webSocket->textAll(jsonString); 
//    and that stuff usually appears in the display of the webserver

int bufferIndex = 0;
unsigned long lastReceiveTime = 0;
char *serialBuffer = nullptr; 
char *localBuffer = nullptr; 

void initProcessData(HardwareSerial& mescSerial, HardwareSerial& compSerial, AsyncWebServer& server, AsyncWebSocket& webSocket) {
  g_mescSerial = &mescSerial;
  g_compSerial = &compSerial;
  g_webSocket = &webSocket;

  serialBuffer = (char *)malloc(BIG_BUFFER_SIZE * sizeof(char));

  xTaskCreate( processData, "Process Data", 4048, NULL, 1, NULL);
}

// top level function for receiving stuff through
//  the ESP32 USB-serial (called g_compSerial)
//  or data from the mesc uart (called g_mescSerial)
void processData(void *parameter) {
  localBuffer = (char *)malloc(BIG_BUFFER_SIZE * sizeof(char));

  int localBufferIndex = 0;
  char ch;

  while (true) {
    if (commState == COMM_ESP32) {
      // if we're in this state we are changing the
      //   config file of the ESP32
      handleESP32Config(localBuffer, localBufferIndex, serialBuffer, bufferIndex);
    } else {

      // this block mostly chowing mesc-uart data
      while (g_mescSerial->available()) {
	char incomingByte = g_mescSerial->read();
	if (bufferIndex < BIG_BUFFER_SIZE - 1) {
	  if (incomingByte == '\r') {
	    processLine(serialBuffer);
	    bufferIndex = 0;
	  }
	  else if (incomingByte == 0) {
	  }
	  else if (incomingByte == '\n') {
	  }
	  else {
	    serialBuffer[bufferIndex] = incomingByte;
	    serialBuffer[bufferIndex+1] = '\0';
	    bufferIndex++;
	  }
	}
	else {
	  g_compSerial->println("odd: broke string length?");
	  g_compSerial->println(bufferIndex);
	}
	lastReceiveTime = millis();
      }

      // we're probably here because input from mesc was just sitting at the jens prompt.
      //   it's basically we've gotten all our results and have timed out.
      //   this section chucks back all the values caught by processLine()
      //   that's been loaded into jsonDoc()
      if (bufferIndex > 0 && (millis() - lastReceiveTime) >= SERIAL_TIMEOUT_MS) {
	String jsonString; 
	serializeJson(jsonDoc, jsonString);
	// if we have data in jsonString, send it to the websocket
	g_webSocket->textAll(jsonString); 

	// One case for being here, we  may have completed receiving "get" results from user
	//   if the user is logging, send the get results to udp
	if (logState == LOG_GET) {
	  udpSend((char*)jsonString.c_str()); 
	  // sent the results so request to start gathering json
	  logState = LOG_REQUEST_JSON; 
	}

	jsonDoc.clear();
	bufferIndex = 0;
      }

      // this block mostly chowing ESP-USB-uart data
      while (g_compSerial->available()) {
	ch = g_compSerial->read();
	g_mescSerial->write(ch);
	if (ch == '>') {
	  bufferIndex = 0;
	  localBufferIndex = 0;
	  commState = COMM_ESP32;
	  initESP32Config();
	}
      }
    }
    vTaskDelay(1 / portTICK_PERIOD_MS); // easy on the resources
  }
}

// usually arrive here when data is coming from the MESC uart
//  or we timed out at Jens' prompt
void processLine(char *line) {
  // mesc has sent a string from the command 'log -fl'
  //  that is used for graphing. Send to the websocket
  if (strncmp(line, "{\"time\":", 8) == 0) { 
    g_webSocket->textAll(line);
  }
  // mesc sent a string from the command 'status json'
  else if (strncmp(line, "{\"adc1\":", 8) == 0) { 
    g_compSerial->printf("%d: LOG: %s\n", logState, line);
    // send to udp if user is logging
    if (logState == LOG_JSON) {
      udpSend(line);
    }
  }
  else { // mesc has PROBABLY sent a string resulting from get

    remove_ansi_escape_sequences(line);
    replace_pipe_with_tab(line);
    // g_compSerial->println(line);

    int count = countCharOccurrences(line, '\t');
    if (count == 4) { // more likely it came from get
      g_compSerial->printf("TERM: %s\n", line);

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
	// collect data into a jsonDoc, later will send to webserver. 
	//  see, processData() g_webSocket->textAll(jsonString); 
	jsonDoc[value1] = value2;
      }
    }
  }
}

// some simple string operations:
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

