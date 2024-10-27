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
unsigned long last_data_receive = 0; 
int count = 0;

AsyncWebSocket* g_webSocket = nullptr;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

DisplayDataRequest displayDataRequest;
DynamicJsonDocument jsonDoc(2024);

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

  xTaskCreate(udpReceiveTask, "UDP Receive Task", 8192, NULL, 1, NULL);
}

// ESP32 starts a wifi network, gets messages by udp
//   this function doesnt send any requests to the host
//   the code managing logRequest.commandType are sent
//   to the internal sdcard object
void udpReceiveTask(void *pvParameter) {
  udpReceiver.begin(localPort);
  Serial.printf("UDP Receiver listening on port %d\n", localPort);

  // Variables to handle the log name extraction
  char *spacePtr;
  char message[2001];
  char my_str[100];

  while (1) {
    int packetSize = udpReceiver.parsePacket();
    if (packetSize) {
      if (packetSize > 2000) {
	// Handle large packet, if necessary
	continue;  // Skip this packet
      }

      int len = udpReceiver.read(message, 2000);

      if (len > 0) {

	last_udp_receive = lv_tick_get(); 

        message[len] = '\0';  // Null-terminate packet
	spacePtr = strstr(message, ": ");
	if (spacePtr != NULL) {
	  spacePtr += 2;
	  strncpy(my_str, spacePtr, sizeof(my_str) - 1);
	  my_str[sizeof(my_str) - 1] = '\0'; 
	} 

	if (strncmp(message, "{\"adc1\":", 8) == 0) { 
	  last_data_receive = lv_tick_get(); 

	  count++;
	  if (count > 30) {
	    Serial.printf("\n%s\n", message);
	    count = 0;
	  } else {
	    Serial.print(".");
	  }

	  DeserializationError error = deserializeJson(jsonDoc, message);

	  if (error) {
	    Serial.printf("Failed to parse JSON: %s\n", error.f_str());
	    continue;
	  }

	  float Vd;
	  float Vq;
	  if (jsonDoc.containsKey("ehz")) {
	    displayDataRequest.ehz = jsonDoc["ehz"].as<int>();
	    displayDataRequest.mph = displayDataRequest.ehz / 10;
	    if (displayDataRequest.ehz > 999) {
	      displayDataRequest.ehz = 999;
	    }
	    if (displayDataRequest.ehz < 0) {displayDataRequest.ehz = 0;}
	  }

	  if (jsonDoc.containsKey("vbus")) {
	    displayDataRequest.bat = jsonDoc["vbus"].as<float>();
	  }

	  if (jsonDoc.containsKey("Vd") && jsonDoc.containsKey("Vq")) {
	    Vd = jsonDoc["Vd"].as<float>();
	    Vq = jsonDoc["Vq"].as<float>();
	    displayDataRequest.amp = sqrt((Vd * Vd) + (Vq * Vq));
	  }

	  UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
	  if (stackHighWaterMark < 4000) { // this is not good. 
	    Serial.printf("Stack high water mark: %d bytes remaining\n", stackHighWaterMark);
	  }
        
	  if (xQueueSend(displayQueue, &displayDataRequest, portMAX_DELAY) != pdPASS) {
	    Serial.println("Failed to send display data message.");
	  }
	  // if (xQueueSend(loggingQueue, &logRequest, portMAX_DELAY) != pdPASS) {
	  // Serial.println("Failed to send LOG_START request to queue.");
	  // }
	}
	else if (strncmp(message, "{\"Parameter\":", 13) == 0) { 
	  last_data_receive = lv_tick_get(); 
	  Serial.printf("\n%d :: %s :\n", strlen(message), message);

	  // should send something to log task

	}
	else if (strncmp(message, "LOG_NAME:", 9) == 0 && spacePtr) {
	  strncpy(my_str, spacePtr, sizeof(my_str) - 1); 
	  my_str[sizeof(my_str) - 1] = '\0'; 
	  setLogFileName(my_str);
	}
	else if (strncmp(message, "LOG_START:", 10) == 0) {
	  Serial.printf("log start requested\n");
	}
	else if (strncmp(message, "PULSE:", 10) == 0) {
	  Serial.printf("pulse received\n");
	}
	else if (strncmp(message, "TIME_STAMP:", 11) == 0) { // for now treating this as a pulse from host
	  strncpy(my_str, spacePtr, sizeof(my_str) - 1); 
	  my_str[sizeof(my_str) - 1] = '\0'; 
	  if (lv_tick_get() - last_data_receive > 400) {
	    Serial.printf("timestamp: %s, no json, requesting json\n", my_str);
	    udpSend((char *) "JSON_START:");
	  }
	  else {
	    Serial.printf("timestamp: %s\n", my_str);
	  }
	  setLogFileName(my_str);
	}
	else if (strncmp(message, "LOG_STOP:", 9) == 0) {
	  Serial.printf("log stop requested\n");
	}
	else {
	  Serial.printf("unexpected websocket message: %s\n", message);
	}
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
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
