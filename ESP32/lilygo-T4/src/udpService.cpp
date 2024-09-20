#include <WiFi.h>
#include <WiFiUdp.h>
#include "udpService.h"
#include "global.h"

const uint16_t localPort = 1234;

WiFiUDP udpReceiver; 

void initUDPService() {
  WiFi.begin(config.ssid, config.password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }

  // Set fixed IP address
  IPAddress local_IP(config.local_IP_array[0], config.local_IP_array[1], config.local_IP_array[2], config.local_IP_array[3]);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);

  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  } else {
    Serial.print("Assigned IP: ");
    Serial.println(WiFi.localIP());
  }

  xTaskCreate(udpReceiveTask, "UDP Send Task", 4096, NULL, 1, NULL);
}

void udpReceiveTask(void *pvParameter) {
  udpReceiver.begin(localPort);
  Serial.printf("UDP Receiver listening on port %d\n", localPort);

  while (1) {
    int packetSize = udpReceiver.parsePacket();
    if (packetSize) {
      char incomingPacket[255];
      int len = udpReceiver.read(incomingPacket, 255);
      if (len > 0) {
        incomingPacket[len] = 0;  // Null-terminate packet
        Serial.printf("Received UDP packet: %s\n", incomingPacket);
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

