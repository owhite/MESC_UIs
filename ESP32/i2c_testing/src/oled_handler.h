#ifndef OLED_HANDLER_H
#define OLED_HANDLER_H

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 

#define I2C_SDA 6
#define I2C_SCL 7

#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 

void oledTask(void *pvParameter);
void initOLEDTask(void);

#endif // OLED_HANDLER_H
