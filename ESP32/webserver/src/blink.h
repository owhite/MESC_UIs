#ifndef BLINK_H
#define BLINK_H

extern volatile int blinkSpeed;

void initBlinkTask();
void setBlinkSpeed(int speed);
int getBlinkSpeed();

#endif
