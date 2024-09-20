#ifndef UDPSERVICE_H
#define UDPSERVICE_H

#include <Arduino.h>

void initUDPService();
void udpReceiveTask(void *pvParameter);

#endif // UDPSERVICE_H
