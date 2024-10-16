#include <WiFi.h>
#include <WiFiUdp.h>
#include "udpService.h"
#include "sd_card.h"
#include "gui.h"
#include "global.h"

const uint16_t localPort = 1234;
const uint16_t remotePort = 1234;

IPAddress remote_IP;
WiFiUDP udpReceiver;
WiFiUDP udpSender;

unsigned long last_udp_receive = 0; 

AsyncWebSocket* g_webSocket = nullptr;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

DisplayRequest displayRequest;

void initUDPService() {
  WiFi.begin(config.ssid, config.password);

  Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tft.print("* ");
  }

  // Set fixed IP address
  IPAddress local_IP(config.local_IP_array[0], config.local_IP_array[1], config.local_IP_array[2], config.local_IP_array[3]);
  IPAddress gateway(192, 168, 1, 1); // dunno how these work
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
//   this function doesnt send any requests to the host
//   the code managing logRequest.commandType are sent
//   to the internal sdcard object
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

	if (strncmp(message, "PULSE:", 6) == 0) {
	  // only reset this if pulse or if log data is received
	  last_udp_receive = lv_tick_get(); 
	}
	else if (strncmp(message, "{\"adc1\":", 8) == 0) { 
	  last_udp_receive = lv_tick_get(); // log data received, reset
	  // logRequest.commandType = LOG_ADD_LINE;

	  strncpy(displayRequest.displayLine, message, sizeof(message) - 1);
	  displayRequest.displayLine[sizeof(displayRequest.displayLine) - 1] = '\0'; 
	  if (xQueueSend(displayQueue, &displayRequest, portMAX_DELAY) != pdPASS) {
	    Serial.println("Failed to send display message.");
	  }
	}
	else if (strncmp(message, "{\"Parameter\":", 13) == 0) { 
	  // logRequest.commandType = LOG_ADD_LINE;
	  strncpy(displayRequest.displayLine, message, sizeof(message) - 1);
	  displayRequest.displayLine[sizeof(displayRequest.displayLine) - 1] = '\0'; 
	  if (xQueueSend(displayQueue, &displayRequest, portMAX_DELAY) != pdPASS) {
	    Serial.println("Failed to send display message.");
	  }
	}
	else if (strncmp(message, "LOG_NAME:", 9) == 0 && spacePtr) {
	  strcpy(my_str, spacePtr + 1); 
	  my_str[sizeof(my_str) - 1] = '\0'; 
	  setLogFileName(my_str);
	}
	else if (strncmp(message, "LOG_START:", 10) == 0) {
	  logRequest.commandType = LOG_START;

	  if (xQueueSend(loggingQueue, &logRequest, portMAX_DELAY) != pdPASS) {
	    Serial.println("Failed to send LOG_START request to queue.");
	  }
	}
	else if (strncmp(message, "LOG_STOP:", 9) == 0) {
	  logRequest.commandType = LOG_STOP;

	  if (xQueueSend(loggingQueue, &logRequest, portMAX_DELAY) != pdPASS) {
	    Serial.println("Failed to send LOG_STOP request to queue.");
	  }
	}
	else {
	  Serial.println("strange websocket message");
	}
      }
    }
  }
}

// ESP32 starts a wifi network and sends messages to host by udp
//  right now it looks like it only manages requests for the
//  host to start logging    
void udpSend(char *message) {
    udpSender.beginPacket(remote_IP, remotePort);
    udpSender.printf("%s", message);
    udpSender.endPacket();
}
