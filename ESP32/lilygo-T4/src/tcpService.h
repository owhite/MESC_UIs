#ifndef TCPSERVICE_H
#define TCPSERVICE_H

#include <Arduino.h>

void initTCPService();
void tcpReceiveTask(void *pvParameter);

#endif // TCPSERVICE_H
