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
#include "button_on.h"
#include "button_off.h"
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
lv_obj_t * log_label;
lv_obj_t * imgbtn;
lv_obj_t * number_label;  // Label for the number display
bool is_image1 = true;
bool rlst = false;


LV_IMG_DECLARE(button_on);  // Declare the image for the "on" state
LV_IMG_DECLARE(button_off); // Declare the image for the "off" state


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

  if (lv_obj_has_state(btn, LV_STATE_CHECKED) == false) {
    // If the button is checked, set the "off" image
    Serial.println("going to on");
    lv_imgbtn_set_src(btn, LV_IMGBTN_STATE_RELEASED, NULL, &button_on, NULL);
    lv_obj_set_style_text_color(label, lv_color_hex(0x50ff7d), LV_PART_MAIN | LV_STATE_DEFAULT); 
    lv_obj_add_state(btn, LV_STATE_CHECKED); 
  } else { // set the checked state
    Serial.println("going to off");
    lv_imgbtn_set_src(btn, LV_IMGBTN_STATE_RELEASED, NULL, &button_off, NULL);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT); 
    lv_obj_clear_state(btn, LV_STATE_CHECKED);
  }

  if (btn == imgbtn) {
    Serial.println("Button was clicked!");
    snprintf(buf, sizeof(buf), "BTN: %d", cnt);
    lv_label_set_text(btn_label, buf);
  }
  cnt++;
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
    snprintf(buffer, sizeof(buffer), "SDCard Size: %llu MB", cardSize);
  } else {
    snprintf(buffer, sizeof(buffer), "SD card not found");
  }
  lv_label_set_text(sdcard_label, buffer);
}

void setupGUI() {
  lv_obj_t * screen = lv_scr_act();  // Get the active screen object
  lv_obj_set_style_bg_color(screen, lv_color_hex(0x3d3f4a), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN); 

  imgbtn = lv_imgbtn_create(lv_scr_act());  // Create image button on the active screen
  lv_obj_set_pos(imgbtn, 10, 10);  // Set position
  lv_obj_set_size(imgbtn, 66, 44);  

  // Set images for the different states (replace these with your actual image data or symbols)
  lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_RELEASED, &button_off, NULL, NULL);
  lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_PRESSED, &button_on, NULL, NULL);
  lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_CHECKED_RELEASED, &button_off, NULL, NULL);
  lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_CHECKED_PRESSED, &button_on, NULL, NULL);
  lv_obj_clear_state(imgbtn, LV_STATE_CHECKED);

  // Create child label of imgbtn
  log_label = lv_label_create(imgbtn);  
  lv_label_set_text(log_label, "LOG");   
  lv_obj_set_style_text_color(log_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);   
  lv_obj_set_style_text_font(log_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT); 
  lv_obj_align(log_label, LV_ALIGN_CENTER, 0, 0);

  // Add event callback for toggling the button state
  lv_obj_add_event_cb(imgbtn, btnEventCB, LV_EVENT_CLICKED, NULL);

  // info about button
  btn_label = lv_label_create(lv_scr_act());
  lv_obj_set_pos(btn_label, 10, 144);
  lv_label_set_text(btn_label, "");

  // display coordinates
  coord_label = lv_label_create(lv_scr_act());
  lv_obj_set_pos(coord_label, 10, 162);
  lv_label_set_text(coord_label, "");

  // Info about IP addresses and SD card
  ip_label = lv_label_create(lv_scr_act());
  lv_obj_set_pos(ip_label, 10, 180);  // remote IP
  local_label = lv_label_create(lv_scr_act());
  lv_obj_set_pos(local_label, 10, 198);  // local IP
  sdcard_label = lv_label_create(lv_scr_act());
  lv_obj_set_pos(sdcard_label, 10, 216);  // SD card info

  // shows output of numberUpdateTask()
  number_label = lv_label_create(lv_scr_act());
  lv_obj_set_pos(number_label, 280, 218);
  lv_label_set_text(number_label, "0");

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
  initUDPService();
  Serial.println("entering");
  initSDCard();
  Serial.println("exiting");

  setupGUI();

  // Create the FreeRTOS task for updating the number
  xTaskCreate(numberUpdateTask, "Number Update Task", 4096, NULL, 1, NULL);

  if (config.access_point) {
    WiFi.softAP("AP_NAME", "AP_PASSWORD");
    IPAddress ip = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(ip);
  } else {
    WiFi.begin(config.ssid, config.password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
  }
}

void loop() {
  lv_timer_handler(); 
  delay(5); 
}
