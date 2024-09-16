#ifndef ESPNOWSERVICE_H
#define ESPNOWSERVICE_H

#include <esp_now.h>


void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void onDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len);
void espNowTask(void *pvParameter); // ESP-NOW task
void initESPNow();                  // ESP-NOW initialization


#endif // ESPNOWSERVICE_H
