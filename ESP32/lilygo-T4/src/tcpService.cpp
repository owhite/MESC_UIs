#include <WiFi.h>
#include "tcpService.h"
#include "global.h"

const uint16_t localPort = 1234;  // TCP port

WiFiServer tcpServer(localPort);  // Create a TCP server object

void initTCPService() {
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

  // Start the TCP server
  tcpServer.begin();
  Serial.printf("TCP Server started on port %d\n", localPort);

  // Create task to handle incoming TCP connections
  xTaskCreate(tcpReceiveTask, "TCP Receive Task", 4096, NULL, 1, NULL);
}

void tcpReceiveTask(void *pvParameter) {
  while (1) {
    // Check if a client has connected
    WiFiClient client = tcpServer.available();

    if (client) {
      Serial.println("New TCP client connected");

      while (client.connected()) {
        // Check if there is data available to read
        if (client.available()) {
          char incomingPacket[255];
          int len = client.readBytesUntil('\n', incomingPacket, sizeof(incomingPacket) - 1);
          if (len > 0) {
            incomingPacket[len] = '\0';  // Null-terminate packet
            Serial.printf("Received TCP packet: %s\n", incomingPacket);

            // Echo the message back to the client
            client.println(incomingPacket);
          }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
      }

      // Close the connection when client disconnects
      client.stop();
      Serial.println("Client disconnected");
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);  // Delay to avoid busy-looping
  }
}
