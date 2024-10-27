#ifndef UDPSERVICE_H
#define UDPSERVICE_H

#include <Arduino.h>

void initUDPService();
void udpReceiveTask(void *pvParameter);
void udpSend(char *line);
void chowJSON(char *line);

#endif // UDPSERVICE_H
