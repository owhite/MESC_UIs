#include <esp_now.h>
#include "esp_wifi.h" 
#include "espnowservice.h"
#include "global.h"

// Initialize ESP-NOW and add peer
void initESPNow() {
  if (esp_now_init() != ESP_OK) {
    g_compSerial->println("Error initializing ESP-NOW");
    return;
  }

  // Register ESP-NOW Send and Receive callback functions
  esp_now_register_send_cb(onDataSent);
  esp_now_register_recv_cb(onDataRecv);

  // Print the MAC address
  g_compSerial->print("MAC::");
  for (int i = 0; i < 6; ++i) {
    if (i < 5) {
      g_compSerial->printf("%02X:", config.MAC[i]);
    } else {
      g_compSerial->printf("%02X", config.MAC[i]); 
    }
  }
  g_compSerial->println(); 

  // Add peer (using the MAC address from config)
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, config.MAC, 6);
  peerInfo.channel = WiFi.channel();  // Set the channel to match the WiFi channel
  peerInfo.encrypt = false; 

  esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE);  


  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    g_compSerial->println("Failed to add peer");
  } else {
    g_compSerial->println("Peer added");
  }

  // Start a FreeRTOS task to manage ESP-NOW communication
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
  g_compSerial->println(status == ESP_NOW_SEND_SUCCESS ? "ESP-NOW Send Success" : "ESP-NOW Send Fail");
}

void onDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len) {
  g_compSerial->print("Received ESP-NOW data: ");
  for (int i = 0; i < len; i++) {
    g_compSerial->write(data[i]);
  }
  g_compSerial->println();
}
