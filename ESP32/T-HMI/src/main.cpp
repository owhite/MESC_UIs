#include "pins.h"
#include <SPI.h>
#include <SD.h>
#include <SD_MMC.h>
#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <xpt2046.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include <lv_conf.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>

#include "button_on.h"
#include "button_off.h"
#include "controls_on.h"
#include "controls_off.h"
#include "temp_hi.h"
#include "temp_lo.h"
#include "data.h"
#include "processConfig.h"
#include "udpService.h"
#include "global.h"
#include "sd_card.h"

TFT_eSPI tft = TFT_eSPI();

#define HOR_PIXELS 240
#define VER_PIXELS 320

#define BTN_WIDTH 50

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[HOR_PIXELS * 10];

XPT2046 touch = XPT2046(SPI, TOUCHSCREEN_CS_PIN, TOUCHSCREEN_IRQ_PIN);
touch_calibration_t calibration_data[4];

LoggingRequest globalRequest;
int commState = 0;
DynamicJsonDocument jsonDoc(5024);

AsyncWebSocket* g_webSocket = nullptr;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

LoggingState sdLoggingState = {false, "", File(), NULL};
bool refreshDisplay = true;

int number = 0;  // Variable to store the current number

lv_obj_t * ip_label;        // Label to display IP addresses and SD card status
lv_obj_t * local_label;     // Label to display the local IP
lv_obj_t * sdcard_label;    // Label to display SD card status
lv_obj_t * btn_label;       // Info about buttons
lv_obj_t * coord_label;     // Display coordinates
lv_obj_t * number_label;  // Label for the number display
lv_obj_t * brightness_lbl;

lv_obj_t *btn_array[4];

lv_obj_t * ehrz_btn;
lv_obj_t * amp_btn;
lv_obj_t * mph_btn;
lv_obj_t * bat_btn;

lv_obj_t * log_btn;

lv_obj_t * brightness_sw;

bool is_image1 = true;
bool rlst = false;

LV_IMG_DECLARE(button_on);
LV_IMG_DECLARE(button_off);

LV_IMG_DECLARE(controls_on);
LV_IMG_DECLARE(controls_off);

LV_IMG_DECLARE(temp_hi);
LV_IMG_DECLARE(temp_lo);

int segmentDisplayInt = 123;
int segmentDisplayLetter = 'E';

Adafruit_AlphaNum4 alphaLED = Adafruit_AlphaNum4();
TwoWire customWire = TwoWire(0);

void segmentTask(void * parameter) {
  customWire.begin(43, 44);
  alphaLED.begin(0x70, &customWire);

  while (true) {
    String str = String(segmentDisplayInt);
    if (str.length() < 4) {
      alphaLED.clear();
      for(int i=0;i < int(str.length()); i++) {
	alphaLED.writeDigitAscii(i+(4-str.length()), str.charAt(i));
      }
      alphaLED.writeDigitAscii(0, segmentDisplayLetter);
      alphaLED.writeDisplay();
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// Task that updates the number every second
void numberUpdateTask(void * parameter) {
  while (true) {
    number++;  // Increment the number
    char buf[10];
    snprintf(buf, sizeof(buf), "%d", number);  // Convert the number to a string

    lv_label_set_text(number_label, buf);  // Update the label with the new number

    // Trigger LVGL to refresh the display with the updated number
    lv_obj_invalidate(number_label);

    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay for 1 second
  }
}

static void btnEventCB(lv_event_t * e) {
  lv_obj_t * btn = lv_event_get_target(e);
  lv_obj_t * label = lv_obj_get_child(btn, 0);

  static uint32_t cnt = 1;
  char buf[10];

  LV_LOG_USER("Clicked");
  Serial.println("event detected!");

  int i;
  // buttons will be have as mutually exclusive checkboxes
  for(i = 0; i < 4; i++) {
    if (btn == btn_array[i]) {
      lv_imgbtn_set_src(btn_array[i], LV_IMGBTN_STATE_RELEASED, NULL, &button_on, NULL);
      lv_obj_set_style_text_color(label, lv_color_hex(0x50ff7d), LV_PART_MAIN | LV_STATE_DEFAULT); 
      lv_obj_add_state(btn_array[i], LV_STATE_CHECKED); 
    }
    else {
      lv_imgbtn_set_src(btn_array[i], LV_IMGBTN_STATE_RELEASED, NULL, &button_off, NULL);
      lv_obj_t * label = lv_obj_get_child(btn_array[i], 0);
      lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT); 
      lv_obj_clear_state(btn_array[i], LV_STATE_CHECKED);
    }
  }

  if (btn == log_btn) {
    Serial.println("Button was clicked!");
  }
  else if (btn == amp_btn) {
    segmentDisplayLetter = 'A';
  }
  else if (btn == mph_btn) {
    segmentDisplayLetter = 'M';
  }
  else if (btn == ehrz_btn) {
    segmentDisplayLetter = 'E';
  }
  else if (btn == bat_btn) {
    segmentDisplayLetter = 'B';
  }
  snprintf(buf, sizeof(buf), "BTN: %d", cnt);
  lv_label_set_text(btn_label, buf);
  cnt++;
}

void touchpadRead(lv_indev_drv_t * indev_driver, lv_indev_data_t * data) {
  if (touch.pressed()) {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = touch.X();
    data->point.y = touch.Y();

    segmentDisplayInt = touch.X();

    char buf[20];
    snprintf(buf, sizeof(buf), "x: %d y: %d", touch.X(), touch.Y());
    lv_label_set_text(coord_label, buf);
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

static void brightnessCB(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = lv_event_get_target(e);
  if(code == LV_EVENT_VALUE_CHANGED) {
    // XXX
    alphaLED.setBrightness(14 * lv_obj_has_state(obj, LV_STATE_CHECKED));
    LV_UNUSED(obj);
    LV_LOG_USER("State: %s\n", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "On" : "Off");
  }
}

void displayNetworkInfo() {
  char buffer[128];

  // Create and set the IP or Remote IP label
  if (config.access_point) {
    snprintf(buffer, sizeof(buffer), "AP IP: %s", WiFi.softAPIP().toString().c_str());
  } else {
    snprintf(buffer, sizeof(buffer), "REMOTE: %d.%d.%d.%d",
             config.remote_IP_array[0], config.remote_IP_array[1], 
             config.remote_IP_array[2], config.remote_IP_array[3]);
  }
  lv_label_set_text(ip_label, buffer);

  snprintf(buffer, sizeof(buffer), "LOCAL: %s", WiFi.localIP().toString().c_str());
  lv_label_set_text(local_label, buffer);

  if (isSDCardStillMounted()) {
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);  // Convert to MB
    snprintf(buffer, sizeof(buffer), "SD Size: %llu MB", cardSize);
  } else {
    snprintf(buffer, sizeof(buffer), "SD card not found");
  }
  lv_label_set_text(sdcard_label, buffer);

  // brightness switch and lable
  lv_obj_set_pos(brightness_sw, 1, 10);  // Set position
  lv_label_set_text(brightness_lbl, "brightness");
  lv_obj_set_pos(brightness_lbl, 58, 12);
  lv_obj_add_state(brightness_sw, LV_STATE_CHECKED);
  static lv_style_t style_switch;
  lv_style_init(&style_switch);
  lv_style_set_bg_color(&style_switch, lv_color_hex(0x3d3f4a));  // background default grey, off
  lv_style_set_bg_color(&style_switch, lv_color_hex(0x50ff7d));  // electric green, on
  lv_obj_add_style(brightness_sw, &style_switch, LV_PART_INDICATOR | LV_STATE_CHECKED);  // Checked state
  lv_obj_add_style(brightness_sw, &style_switch, LV_PART_INDICATOR);   
  lv_obj_add_event_cb(brightness_sw, brightnessCB, LV_EVENT_ALL, NULL);

  // displays last button pressed
  lv_obj_set_pos(btn_label, 1, 44);
  lv_label_set_text(btn_label, "");

  // display coordinates
  lv_obj_set_pos(coord_label, 1, 62);
  lv_label_set_text(coord_label, "");

  // IP addresses and SD card
  lv_obj_set_pos(ip_label, 1, 80);
  lv_obj_set_pos(local_label, 1, 98);
  lv_obj_set_pos(sdcard_label, 1, 116);
}

lv_obj_t * createButtonWithImage(int x, int y, int width, int height, const char *name, lv_event_cb_t btnEventCB) {
  lv_obj_t *imgbtn;

  // Create image button
  imgbtn = lv_imgbtn_create(lv_scr_act());  // Create image button on active screen
  lv_obj_set_pos(imgbtn, x, y);  // Set position
  lv_obj_set_size(imgbtn, width, height);  // Set size

  // Set button images for different states
  lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_RELEASED, &button_off, NULL, NULL);
  lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_PRESSED, &button_on, NULL, NULL);
  lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_CHECKED_RELEASED, &button_off, NULL, NULL);
  lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_CHECKED_PRESSED, &button_on, NULL, NULL);
  lv_obj_clear_state(imgbtn, LV_STATE_CHECKED);  // Clear the checked state

  // Create child label for imgbtn
  lv_obj_t *label = lv_label_create(imgbtn);  
  lv_label_set_text(label, name);  // Set label text to the name
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);  // Set label text color
  lv_obj_set_style_text_font(label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);  // Set font style
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);  // Center the label on the button

  // Add event callback for button state toggle
  lv_obj_add_event_cb(imgbtn, btnEventCB, LV_EVENT_CLICKED, NULL);

  return imgbtn;
}

void setupGUI() {
  lv_obj_t * screen = lv_scr_act();  // Get the active screen object
  lv_obj_set_style_bg_color(screen, lv_color_hex(0x3d3f4a), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN); 

  uint16_t width = button_off.header.w;
  uint16_t height = button_off.header.h;
  uint16_t bump = width + 10;

  ehrz_btn = createButtonWithImage(10, 10, width, height, "EHRZ", btnEventCB);
  amp_btn  = createButtonWithImage(10 + (bump * 1), 10, width, height, "AMP", btnEventCB);
  mph_btn  = createButtonWithImage(10 + (bump * 2), 10, width, height, "MPH", btnEventCB);
  bat_btn  = createButtonWithImage(10 + (bump * 3), 10, width, height, "BAT", btnEventCB);

  width = controls_off.header.w;
  height = controls_off.header.h;

  btn_array[0] = ehrz_btn;
  btn_array[1] = amp_btn;
  btn_array[2] = mph_btn;
  btn_array[3] = bat_btn;

  lv_obj_t *imgbtn;

  // Create image button
  imgbtn = lv_imgbtn_create(lv_scr_act());
  lv_obj_set_pos(imgbtn, 240, 152);
  lv_obj_set_size(imgbtn, width, height);

  // Set button images for different states
  lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_RELEASED, &controls_off, NULL, NULL);
  lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_PRESSED, &controls_on, NULL, NULL);
  lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_CHECKED_RELEASED, &controls_off, NULL, NULL);
  lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_CHECKED_PRESSED, &controls_on, NULL, NULL);
  lv_obj_clear_state(imgbtn, LV_STATE_CHECKED);  // Clear the checked state

  // shows output of numberUpdateTask()
  number_label = lv_label_create(lv_scr_act());
  lv_obj_set_pos(number_label, 280, 138);
  lv_label_set_text(number_label, "0");

  // thermometer
  lv_obj_t * temp_img = lv_img_create(lv_scr_act());
  lv_img_set_src(temp_img, &temp_lo);
  lv_obj_set_pos(temp_img, 205, 174);
  lv_obj_set_size(temp_img, temp_lo.header.w, temp_lo.header.h);

  lv_obj_t *parent = lv_obj_create(lv_scr_act());
  lv_obj_set_pos(parent, 0, 60);
  lv_obj_set_size(parent, 200, 170);   
  lv_obj_set_style_pad_all(parent, 0, 0);
  lv_obj_set_style_outline_width(parent, 0, 0);
  lv_obj_set_style_border_width(parent, 2, 2);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN); 
  lv_obj_set_style_bg_color(parent, lv_color_hex(0x0000FF), 0); 

  brightness_sw = lv_switch_create(parent);
  btn_label = lv_label_create(parent);
  coord_label = lv_label_create(parent);
  ip_label = lv_label_create(parent);
  local_label = lv_label_create(parent);
  sdcard_label = lv_label_create(parent);
  brightness_lbl = lv_label_create(parent);

  // Call the function to update the display
  displayNetworkInfo();  
}

void displayFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  int32_t w = (area->x2 - area->x1 + 1);
  int32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)color_p, w * h, true);  
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

void setup() {
  Serial.begin(115200);

  Serial.println("Starting setup...");

  lv_init();           

  pinMode(PWR_EN_PIN, OUTPUT);
  digitalWrite(PWR_EN_PIN, HIGH);

  data_init();
  data_read(calibration_data);

  SPI.begin(TOUCHSCREEN_SCLK_PIN, TOUCHSCREEN_MISO_PIN, TOUCHSCREEN_MOSI_PIN);
  touch.begin(HOR_PIXELS, VER_PIXELS);
  touch.setCal(calibration_data[0].rawX, calibration_data[2].rawX,
	       calibration_data[0].rawY, calibration_data[2].rawY,
	       HOR_PIXELS, VER_PIXELS);
  touch.setRotation(1);

  tft.begin();         
  tft.setRotation(1);  
  tft.setSwapBytes(true);

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, HOR_PIXELS * 10);
    
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = VER_PIXELS; 
  disp_drv.ver_res = HOR_PIXELS;
  disp_drv.flush_cb = displayFlush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // input driver for touchscreen
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = touchpadRead;
  lv_indev_t * my_indev = lv_indev_drv_register(&indev_drv);

  readConfig();
  // initUDPService();
  initSDCard();

  setupGUI();
  
  lv_event_send(ehrz_btn, LV_EVENT_CLICKED, NULL); // highlights a top row button

  // <3 FreeRTOS tasks
  xTaskCreate(numberUpdateTask, "Number Update Task", 4096, NULL, 1, NULL);
  xTaskCreate(segmentTask, "Segment Task", 4096, NULL, 1, NULL);
}

void loop() {
  lv_timer_handler(); 
  delay(5); 
}
