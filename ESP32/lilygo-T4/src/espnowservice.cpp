#include <WiFi.h>
#include <esp_now.h>
#include "espnowservice.h"
#include "esp_wifi.h" 
#include "global.h"

#include <esp_now.h>
#include <WiFi.h>
#include "esp_wifi.h"

// Initialize ESP-NOW and set WiFi channel
void initESPNow() {
  Serial.println("Initializing ESP-NOW");

  // Set WiFi mode to Station (STA)
  WiFi.mode(WIFI_MODE_STA);

  // Optionally connect to a WiFi network (if required for your application)
  WiFi.begin((const char*)config.ssid, (const char*)config.password);

  // Wait for the WiFi connection to be established
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print("Connecting to ");
    Serial.println(config.ssid);
  }
  Serial.println("Connected to WiFi");

  // Set the WiFi channel after connection (e.g., channel 6)
  esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register ESP-NOW callbacks
  esp_now_register_send_cb(onDataSent);
  esp_now_register_recv_cb(onDataRecv);

  // Add peer (using the MAC address from config)
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, config.MAC, 6);
  peerInfo.channel = 6;  // Set the channel to match
  peerInfo.encrypt = false;

  // Remove existing peer first, if any, then add the peer
  esp_now_del_peer(peerInfo.peer_addr);

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
  } else {
    Serial.println("Peer added");
  }

  // Optionally, start a FreeRTOS task for ESP-NOW communication
  xTaskCreate(espNowTask, "ESP-NOW Task", 4096, NULL, 1, NULL);
}

// Task to manage ESP-NOW communication
void espNowTask(void *pvParameter) {
  while (1) {
    // ESP-NOW communication can be processed here
    // This loop will keep running for ESP-NOW-related tasks
    vTaskDelay(100 / portTICK_PERIOD_MS);  // Delay to prevent blocking other tasks
  }
}

// ESP-NOW Send and Receive Callback functions
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "ESP-NOW Send Success" : "ESP-NOW Send Fail");
}

void onDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len) {
  Serial.print("Received ESP-NOW data: ");
  for (int i = 0; i < len; i++) {
    Serial.write(data[i]);
  }
  Serial.println();
}
