// gui.cpp
#include "gui.h"
#include "global.h"
#include <lvgl.h>
#include "button_on.h"
#include "button_off.h"
#include "controls_on.h"
#include "controls_off.h"
#include "temp_hi.h"
#include "temp_lo.h"

lv_disp_draw_buf_t draw_buf;
lv_color_t buf[HOR_PIXELS * 10];

// user can request display for...
lv_obj_t * ehrz_btn;      // ehrz
lv_obj_t * amp_btn;       // amperage
lv_obj_t * mph_btn;       // miles per hour
lv_obj_t * bat_btn;       // battery
lv_obj_t * temp_btn;      // temperature
lv_obj_t * log_btn;       // logging data
lv_obj_t * controls_btn;  // the controls panel

lv_obj_t * ip_label;       // display IP addresses and SD card status
lv_obj_t * local_label;    // isplay the local IP
lv_obj_t * sdcard_label;   // display SD card status
lv_obj_t * btn_label;      // info about buttons
lv_obj_t * coord_label;    // display touch screen coordinates 
lv_obj_t * brightness_lbl; // brightness of 14-segment LED
lv_obj_t * brightness_sw;  // switch for brightness

lv_obj_t * led;            // throbbing dot on screen

// panel that shows controls
lv_obj_t * controls_parent; 
#define PANEL_X_PIXELS 240
#define PANEL_Y_PIXELS 170

lv_obj_t *btn_array[6];

LV_IMG_DECLARE(button_on);
LV_IMG_DECLARE(button_off);

LV_IMG_DECLARE(controls_on);
LV_IMG_DECLARE(controls_off);

LV_IMG_DECLARE(temp_hi);
LV_IMG_DECLARE(temp_lo);

// supports createButtonWithImage()
typedef struct {
    const lv_img_dsc_t *img_on;
    const lv_img_dsc_t *img_off;
} button_image_t;

// Function to handle button events
void btnEventCB(lv_event_t * e) {
  lv_obj_t *btn = lv_event_get_target(e);

  static uint32_t cnt = 1;
  char buf[10];

  int i;
  // buttons will be have as mutually exclusive checkboxes
  for(i = 0; i < 6; i++) {
    lv_obj_t * imgbtn = btn_array[i];
    lv_obj_t * label = lv_obj_get_child(imgbtn, 0);
    button_image_t *btn_images = (button_image_t *)lv_obj_get_user_data(imgbtn);

    if (btn == imgbtn) {
      lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_RELEASED, NULL, btn_images->img_on, NULL);
      lv_obj_set_style_text_color(label, lv_color_hex(0x50ff7d), LV_PART_MAIN | LV_STATE_DEFAULT); 
      lv_obj_add_state(imgbtn, LV_STATE_CHECKED); 
    }
    else {
      lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_RELEASED, NULL, btn_images->img_off, NULL);
      lv_obj_t * label = lv_obj_get_child(imgbtn, 0);
      lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT); 
      lv_obj_clear_state(imgbtn, LV_STATE_CHECKED);
    }

    if (btn == controls_btn) {
      lv_obj_clear_flag(controls_parent, LV_OBJ_FLAG_HIDDEN);
    }
    else {
      lv_obj_add_flag(controls_parent, LV_OBJ_FLAG_HIDDEN);
    }
  }

  // Handle specific button actions
  if (btn == log_btn) {
    Serial.println("Log button clicked!");
  } else if (btn == amp_btn) {
    segmentDisplayLetter = 'A';
  } else if (btn == mph_btn) {
    segmentDisplayLetter = 'M';
  } else if (btn == ehrz_btn) {
    segmentDisplayLetter = 'E';
  } else if (btn == bat_btn) {
    segmentDisplayLetter = 'B';
  }
  snprintf(buf, sizeof(buf), "BTN: %d", cnt);
  lv_label_set_text(btn_label, buf);
  cnt++;    
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

// Controls panel:
//   displays network and SD card info
//   allows user to change brightness
void displayControlsPanel(lv_obj_t * parent) {
  char buffer[128];

  lv_obj_set_pos(parent, 0, 60);
  lv_obj_set_size(parent, PANEL_X_PIXELS, PANEL_Y_PIXELS);
  lv_obj_set_style_pad_all(parent, 0, 0);
  lv_obj_set_style_outline_width(parent, 0, 0);
  lv_obj_set_style_border_width(parent, 2, 2);
  lv_obj_set_style_bg_color(parent, lv_color_hex(0x000066), 0); 

  brightness_sw = lv_switch_create(parent);
  btn_label = lv_label_create(parent);
  coord_label = lv_label_create(parent);
  ip_label = lv_label_create(parent);
  local_label = lv_label_create(parent);
  sdcard_label = lv_label_create(parent);
  brightness_lbl = lv_label_create(parent);

  lv_obj_set_style_bg_color(brightness_sw, lv_color_hex(0xb3b3cc), LV_PART_KNOB | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(brightness_sw, lv_color_hex(0xb3b3cc), LV_PART_KNOB | LV_STATE_CHECKED);

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

  // brightness switch and label
  lv_obj_set_pos(brightness_sw, 1, 10);  // Set position
  lv_label_set_text(brightness_lbl, "brightness");
  lv_obj_set_pos(brightness_lbl, 58, 12);
  lv_obj_add_state(brightness_sw, LV_STATE_CHECKED);
  static lv_style_t style_switch; // used to change appearance of switch-dot
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

// creates toggle buttons with images
lv_obj_t * createButtonWithImage(int x, int y, int width, int height,
                                 const lv_img_dsc_t *img_on, const lv_img_dsc_t *img_off, 
                                 const char *name, lv_event_cb_t btnCB) {

  lv_obj_t *imgbtn = lv_imgbtn_create(lv_scr_act());
  lv_obj_set_pos(imgbtn, x, y);
  lv_obj_set_size(imgbtn, width, height);

  // Set initial images for the button
  lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_RELEASED, img_off, NULL, NULL);
  lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_PRESSED, img_on, NULL, NULL);
  lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_CHECKED_RELEASED, img_off, NULL, NULL);
  lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_CHECKED_PRESSED, img_on, NULL, NULL);
  lv_obj_clear_state(imgbtn, LV_STATE_CHECKED);  // Clear the checked state

  // Store the on/off images in user data
  button_image_t *btn_images = (button_image_t *)lv_mem_alloc(sizeof(button_image_t));
  btn_images->img_on = img_on;
  btn_images->img_off = img_off;
  lv_obj_set_user_data(imgbtn, btn_images);

  // Label for the button
  lv_obj_t *label = lv_label_create(imgbtn);
  lv_label_set_text(label, name);
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  // Add event callback
  lv_obj_add_event_cb(imgbtn, btnCB, LV_EVENT_CLICKED, NULL);

  return imgbtn;
}

// task changes appearances of LED/dot on the screen
void throbLedTask(void *parameter) {
    lv_obj_t *led = (lv_obj_t *)parameter; 
    uint8_t i = 0;
    uint32_t colors[10] = {
      0x093162, 0x084c7c, 0x0582b0, 0x03a5d2, 0x02b5e1,
      0x00d4ff, 0x02b5e1, 0x03a5d2, 0x0582b0, 0x084c7c};

    while (true) {
      // lv_led_set_brightness(led, i);
      lv_led_set_color(led, lv_color_hex(colors[i]));
      i++;
      if(i > 9) {i = 0;}
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void setupGUI() {
  lv_obj_t * screen = lv_scr_act();  // Get the active screen object
  lv_obj_set_style_bg_color(screen, lv_color_hex(0x3d3f4a), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN); 

  uint16_t width = button_off.header.w;
  uint16_t height = button_off.header.h;
  uint16_t bump = width + 10;

  ehrz_btn = createButtonWithImage(10, 10, width, height,
				   &button_on, &button_off, "EHRZ", btnEventCB);
  amp_btn  = createButtonWithImage(10 + (bump * 1), 10, width, height,
				   &button_on, &button_off, "AMP", btnEventCB);
  mph_btn  = createButtonWithImage(10 + (bump * 2), 10, width, height,
				   &button_on, &button_off, "MPH", btnEventCB);
  bat_btn  = createButtonWithImage(10 + (bump * 3), 10, width, height,
				   &button_on, &button_off, "BAT", btnEventCB);
  // Create controls button
  width = controls_off.header.w;
  height = controls_off.header.h;
  controls_btn  = createButtonWithImage(240, 64, width, height,
					&controls_on, &controls_off, "", btnEventCB);
  width = temp_lo.header.w;
  height = temp_lo.header.h;
  temp_btn  = createButtonWithImage(270, 174, width, height,
				    &temp_hi, &temp_lo, "", btnEventCB);

  btn_array[0] = ehrz_btn;
  btn_array[1] = amp_btn;
  btn_array[2] = mph_btn;
  btn_array[3] = bat_btn;
  btn_array[4] = controls_btn;
  btn_array[5] = temp_btn;

  lv_obj_t *led = lv_led_create(lv_scr_act());
  lv_obj_set_pos(led, 306, 55);
  lv_obj_set_size(led, 8, 8);
  xTaskCreate(throbLedTask, "Number Update Task", 4096, led, 1, NULL);

  // create controls panel
  controls_parent = lv_obj_create(lv_scr_act());
  displayControlsPanel(controls_parent);  

  // turns on one of our buttons
  lv_event_send(ehrz_btn, LV_EVENT_CLICKED, NULL); 
}

