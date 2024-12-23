#include "touch.h"
#include "pins.h"
#include "data.h"
#include "global.h"
#include <SPI.h>
// #include "GREAT_LAKES_130px.c"

// Initialize the touch object
XPT2046 touch = XPT2046(SPI, TOUCHSCREEN_CS_PIN, TOUCHSCREEN_IRQ_PIN);
touch_calibration_t calibration_data[4];

void displayFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  int32_t w = (area->x2 - area->x1 + 1);
  int32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)color_p, w * h, true);  
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

void touchpadRead(lv_indev_drv_t * indev_driver, lv_indev_data_t * data) {
  if (touch.pressed()) {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = touch.X();
    data->point.y = touch.Y();

    char buf[20];
    snprintf(buf, sizeof(buf), "x: %d y: %d", touch.X(), touch.Y());
    lv_label_set_text(coord_label, buf);
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

// New touchInit function
void touchInit() {

  data_init();
  data_read(calibration_data);

  // SPI initialization for touch
  SPI.begin(TOUCHSCREEN_SCLK_PIN, TOUCHSCREEN_MISO_PIN, TOUCHSCREEN_MOSI_PIN);
  touch.begin(HOR_PIXELS, VER_PIXELS);
  touch.setCal(calibration_data[0].rawX, calibration_data[2].rawX,
               calibration_data[0].rawY, calibration_data[2].rawY,
               HOR_PIXELS, VER_PIXELS);
  touch.setRotation(1);

  // tft.begin();         
  // tft.setRotation(1);  
  // tft.setSwapBytes(true);

  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setCursor(0, 0);

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, HOR_PIXELS * 10);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = VER_PIXELS; 
  disp_drv.ver_res = HOR_PIXELS;
  disp_drv.flush_cb = displayFlush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // Input driver for touchscreen
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = touchpadRead;
  lv_indev_t * my_indev = lv_indev_drv_register(&indev_drv);
}

