#ifndef ESPNOWSERVICE_H
#define ESPNOWSERVICE_H

#include <Arduino.h>
#include <esp_now.h>

void initESPNow();
void initESPNow2();
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void onDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len);
void espNowTask(void *pvParameter); // ESP-NOW task

#endif // ESPNOWSERVICE_H
