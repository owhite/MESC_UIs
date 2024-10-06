#ifndef TOUCH_H
#define TOUCH_H

#include <lvgl.h>
#include <xpt2046.h>
#include "data.h"

// Declare the touch calibration data and the touch object
extern XPT2046 touch;
extern touch_calibration_t calibration_data[4];

// Function declarations
void displayFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void touchInit();
void touchpadRead(lv_indev_drv_t * indev_driver, lv_indev_data_t * data);

#endif // TOUCH_H

