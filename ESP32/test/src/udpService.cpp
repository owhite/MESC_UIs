#include <WiFi.h>
#include <WiFiUdp.h>
#include "udpService.h"
#include "sd_card.h"
#include "global.h"

const uint16_t localPort = 1234;
const uint16_t remotePort = 1234;

IPAddress remote_IP;
WiFiUDP udpReceiver;
WiFiUDP udpSender;

void initUDPService() {
  WiFi.begin(config.ssid, config.password);

  tft.setTextFont(2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setCursor(0, 0);

  tft.print("joining internet: ");
  Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tft.print("* ");
  }

  // Set fixed IP address
  IPAddress local_IP(config.local_IP_array[0], config.local_IP_array[1], config.local_IP_array[2], config.local_IP_array[3]);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);

  remote_IP = IPAddress(config.remote_IP_array[0], config.remote_IP_array[1],
			config.remote_IP_array[2], config.remote_IP_array[3]);

  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  } else {
    Serial.print("Assigned IP: ");
    Serial.println(WiFi.localIP());
  }

  xTaskCreate(udpReceiveTask, "UDP Receive Task", 4096, NULL, 1, NULL);
}

// ESP32 starts a wifi network, gets messages by udp
void udpReceiveTask(void *pvParameter) {
  udpReceiver.begin(localPort);
  Serial.printf("UDP Receiver listening on port %d\n", localPort);

  // Variables to handle the log name extraction
  char my_str[100];  // Array to hold the extracted log name
  char* spacePtr = NULL;
  int spaceCount = 0;

  char message[2000];

  while (1) {
    int packetSize = udpReceiver.parsePacket();
    if (packetSize) {
      // This chokes on long strings
      int len = udpReceiver.read(message, 2000);
      if (len > 0) {
        message[len] = 0;  // Null-terminate packet
        Serial.printf("Received UDP packet: %s\n", message);

	// Count spaces and find the first space
	for (char* p = (char*)message; *p != '\0'; p++) {
	  if (*p == ' ') {
	    spaceCount++;
	    if (spaceCount == 1) {
	      spacePtr = p;
	    }
	  }
	}

	if (strncmp(message, "LOG_NAME:", 9) == 0 && spacePtr) {
	  strcpy(my_str, spacePtr + 1); 
	  my_str[sizeof(my_str) - 1] = '\0'; 
	  setLogFileName(my_str);
	  refreshDisplay = true;
	}
	else if (strncmp(message, "LOG_START:", 10) == 0) {
	  globalRequest.commandType = LOG_START;

	  if (xQueueSend(loggingQueue, &globalRequest, portMAX_DELAY) != pdPASS) {
	    Serial.println("Failed to send LOG_START request to queue.");
	  }
	  refreshDisplay = true;
	}
	else if (strncmp(message, "LOG_STOP:", 9) == 0) {
	  globalRequest.commandType = LOG_STOP;

	  if (xQueueSend(loggingQueue, &globalRequest, portMAX_DELAY) != pdPASS) {
	    Serial.println("Failed to send LOG_STOP request to queue.");
	  }
	  refreshDisplay = true;
	}
	else if (strncmp(message, "{\"adc1\":", 8) == 0) { 
	  globalRequest.commandType = LOG_ADD_LINE;
	  strncpy(globalRequest.logLine, message, sizeof(message) - 1);
	  if (xQueueSend(loggingQueue, &globalRequest, portMAX_DELAY) != pdPASS) {
	    Serial.println("Failed to send log message.");
	  }
	}
	else if (strncmp(message, "{\"Parameter\":", 13) == 0) { 
	  globalRequest.commandType = LOG_ADD_LINE;
	  strncpy(globalRequest.logLine, message, sizeof(message) - 1);
	  if (xQueueSend(loggingQueue, &globalRequest, portMAX_DELAY) != pdPASS) {
	    Serial.println("Failed to save get message.");
	  }
	}
	else {
	  Serial.println("strange websocket message");
	}

      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// ESP32 starts a wifi network and sends messages by udp
void udpSend(char *message) {
  udpSender.beginPacket(remote_IP, remotePort);
  udpSender.printf("%s", message);
  udpSender.endPacket();
}
