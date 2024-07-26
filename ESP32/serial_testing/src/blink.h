#ifndef BLINK_H
#define BLINK_H

#include <Arduino.h>

extern volatile int blinkSpeed;

void initBlinkTask();
void blinkTask(void *pvParameter);

#endif  // BLINK_H
