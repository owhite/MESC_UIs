#include <lvgl.h>
#include "sd_card.h"
#include "gui.h"
#include "udpService.h"
#include "global.h"
#include "IMAGES/button_on.h"
#include "IMAGES/button_off.h"
#include "IMAGES/controls_on.h"
#include "IMAGES/controls_off.h"
// #include "IMAGES/temp_hi.h" // deprecated
// #include "IMAGES/temp_lo.h"
#include "IMAGES/mesc.h"
#include "IMAGES/mosfet.h"
#include "IMAGES/motor.h"

lv_disp_draw_buf_t draw_buf;
lv_color_t buf[HOR_PIXELS * 10];

// user can request display for...
lv_obj_t * ehrz_btn;      // ehrz
lv_obj_t * amp_btn;       // amperage
lv_obj_t * mph_btn;       // miles per hour
lv_obj_t * bat_btn;       // battery
// lv_obj_t * temp_btn;      // temperature, deactivated
lv_obj_t * log_btn;       // logging data
lv_obj_t * controls_btn;  // the controls panel

lv_obj_t * udpstatus_label; // if a pulse came in
lv_obj_t * ip_label;        // display IP addresses and SD card status
lv_obj_t * local_label;     // isplay the local IP
lv_obj_t * sdcard_label;    // display SD card status
lv_obj_t * logname_label;   // file name for log
lv_obj_t * coord_label;     // display touch screen coordinates 
lv_obj_t * brightness_lbl;  // brightness of 14-segment LED
lv_obj_t * brightness_sw;   // switch for brightness
lv_obj_t * data_label;      // shows ehz, mph, amps on big display
lv_obj_t * tmot_label;      // temp of motor
lv_obj_t * tmos_label;      // temp of mosfets

lv_obj_t * led;            // glowing dot on screen

// panel that shows controls
lv_obj_t * controls_parent; 
#define PANEL_X_PIXELS 240
#define PANEL_Y_PIXELS 140

lv_obj_t * data_controls_parent; 
lv_obj_t * mesc_parent; 

lv_obj_t *btn_array[5];

LV_IMG_DECLARE(button_on);
LV_IMG_DECLARE(button_off);

LV_IMG_DECLARE(controls_on);
LV_IMG_DECLARE(controls_off);

// LV_IMG_DECLARE(temp_hi);
// LV_IMG_DECLARE(temp_lo);

LV_IMG_DECLARE(mesc);
LV_IMG_DECLARE(mosfet);

unsigned long guiStartTime = 0;

// supports createButtonWithImage()
typedef struct {
  const lv_img_dsc_t *img_on;
  const lv_img_dsc_t *img_off;
} button_image_t;

// Function to handle button events
void btnEventCB(lv_event_t * e) {
  lv_obj_t *btn = lv_event_get_target(e);

  int i;
  // buttons will be have as mutually exclusive checkboxes
  for(i = 0; i < 5; i++) {
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
      // show the controls widget with logname, 
      //   coordinates and stuff
      lv_obj_add_flag(mesc_parent, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag(controls_parent, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(data_controls_parent, LV_OBJ_FLAG_HIDDEN);
      Serial.println("CLEAR CONTROLS");

    }
    else {
      lv_obj_add_flag(mesc_parent, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(controls_parent, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag(data_controls_parent, LV_OBJ_FLAG_HIDDEN);
      Serial.println("CLEAR DATAT");
    }
  }

  // Handle specific button actions
  if (btn == amp_btn) {
    // Serial.println("amps");
    displayState = DISPLAY_AMP;
  } else if (btn == mph_btn) {
    // Serial.println("mph");
    displayState = DISPLAY_MPH;
  } else if (btn == ehrz_btn) {
    // Serial.println("hrz");
    displayState = DISPLAY_EHRZ;
  } else if (btn == bat_btn) {
    // Serial.println("bat");
    displayState = DISPLAY_BAT;
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


void logBtnCB(lv_event_t * e)
{
  lv_obj_t * btn = lv_event_get_target(e);

  if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
    udpSend((char *) "LOG_START:");
    Serial.println("start logging");
    sdLoggingState.isLogging = true;
  }
  else {
    udpSend((char *) "LOG_STOP:");
    Serial.println("stop logging");
    sdLoggingState.isLogging = false;
  }
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

// Controls panel:
//   displays network and SD card info
//   allows user to change brightness
void createControlsPanel(lv_obj_t * parent) {

  lv_obj_set_pos(parent, 0, 60);
  lv_obj_set_size(parent, PANEL_X_PIXELS, PANEL_Y_PIXELS);
  lv_obj_set_style_pad_all(parent, 0, 0);
  lv_obj_set_style_outline_width(parent, 0, 0);
  lv_obj_set_style_border_width(parent, 2, 2);
  lv_obj_set_style_bg_color(parent, lv_color_hex(0x000066), 0); 

  brightness_sw = lv_switch_create(parent);
  coord_label = lv_label_create(parent);
  ip_label = lv_label_create(parent);
  udpstatus_label = lv_label_create(parent);
  local_label = lv_label_create(parent);
  sdcard_label = lv_label_create(parent);
  logname_label = lv_label_create(parent);
  brightness_lbl = lv_label_create(parent);
  log_btn = lv_btn_create(parent);
  lv_obj_t * label = lv_label_create(log_btn);

  // coordinates, IP addresses, status, SD card
  lv_obj_set_pos(coord_label, 1, 4);
  lv_label_set_text(coord_label, "");
  lv_obj_set_pos(ip_label, 1, 22);
  lv_obj_set_pos(local_label, 1, 40);
  lv_obj_set_pos(udpstatus_label, 1, 58);
  lv_obj_set_pos(sdcard_label, 1, 76);
  lv_obj_set_pos(logname_label, 1, 94);

  lv_obj_set_style_bg_color(brightness_sw, lv_color_hex(0xb3b3cc), LV_PART_KNOB | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(brightness_sw, lv_color_hex(0xb3b3cc), LV_PART_KNOB | LV_STATE_CHECKED);

  // button on controls panel spawns logging
  lv_obj_add_event_cb(log_btn, logBtnCB, LV_EVENT_CLICKED, NULL);
  lv_obj_add_flag(log_btn, LV_OBJ_FLAG_CHECKABLE);
  lv_obj_set_height(log_btn, LV_SIZE_CONTENT);
  lv_obj_set_pos(log_btn, 170, 5);
  lv_label_set_text(label, "LOG");
  lv_obj_center(label);

  // brightness switch and label

  lv_obj_set_pos(brightness_sw, 170, 40);  // Set position
  lv_label_set_text(brightness_lbl, "brightness");
  lv_obj_set_pos(brightness_lbl, 150, 70);
  lv_obj_add_state(brightness_sw, LV_STATE_CHECKED);
  static lv_style_t style_switch; // used to change appearance of switch-dot
  lv_style_init(&style_switch);
  lv_style_set_bg_color(&style_switch, lv_color_hex(0x3d3f4a));  // background default grey, off
  lv_style_set_bg_color(&style_switch, lv_color_hex(0xffd400));  // on
  lv_obj_add_style(brightness_sw, &style_switch, LV_PART_INDICATOR | LV_STATE_CHECKED);
  lv_obj_add_style(brightness_sw, &style_switch, LV_PART_INDICATOR);   
  lv_obj_add_event_cb(brightness_sw, brightnessCB, LV_EVENT_ALL, NULL);

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
  // width = temp_lo.header.w;
  //   height = temp_lo.header.h;
  // temp_btn  = createButtonWithImage(270, 174, width, height, &temp_hi, &temp_lo, "", btnEventCB);

  btn_array[0] = ehrz_btn;
  btn_array[1] = amp_btn;
  btn_array[2] = mph_btn;
  btn_array[3] = bat_btn;
  btn_array[4] = controls_btn;
  // btn_array[5] = temp_btn;

  lv_obj_t *led = lv_led_create(lv_scr_act());
  lv_obj_set_pos(led, 258, 155);
  lv_obj_set_size(led, 40, 8);

  // create controls panel
  controls_parent = lv_obj_create(lv_scr_act());
  createControlsPanel(controls_parent);  

  data_controls_parent = lv_obj_create(lv_scr_act());
  dataControlsPanel(data_controls_parent);  

  mesc_parent = lv_obj_create(lv_scr_act());
  mescControlsPanel(mesc_parent);  

  // turns on one of our buttons
  // lv_event_send(ehrz_btn, LV_EVENT_CLICKED, NULL); 

  LV_FONT_DECLARE(GREAT_LAKES_30px); 
  const lv_font_t *temp_font;
  temp_font = &GREAT_LAKES_30px;

  lv_obj_t * mos_img = lv_img_create(lv_scr_act());
  lv_img_set_src(mos_img, &motor);
  lv_obj_set_pos(mos_img, 10, 204);

  tmos_label = lv_label_create(lv_scr_act());
  lv_obj_set_style_text_font(tmos_label, temp_font, 0);
  lv_label_set_text(tmos_label, "20c");
  lv_obj_set_pos(tmos_label, 46, 204);

  lv_obj_t * mot_img = lv_img_create(lv_scr_act());
  lv_img_set_src(mot_img, &mosfet);
  lv_obj_set_pos(mot_img, 120, 204);

  tmot_label = lv_label_create(lv_scr_act());
  lv_obj_set_style_text_font(tmot_label, temp_font, 0);
  lv_label_set_text(tmot_label, "20c");
  lv_obj_set_pos(tmot_label, 152, 204);

  // Create a queue for 10 requests
  displayQueue = xQueueCreate(20, sizeof(DisplayDataRequest));  
  if (displayQueue == NULL) {
    Serial.println("Failed to create display queue");
    return;
  }

  xTaskCreate(guiTask, "Number GUI Task", 4096, led, 1, NULL);
  xTaskCreate(queueTask, "Handle udp/queue input", 4096, led, 1, NULL);
}

// Data controls panel:
//   displays ehrz, amps, mph, battery
void dataControlsPanel(lv_obj_t * parent) {
  // Panel styling
  lv_obj_set_pos(parent, 0, 60);
  lv_obj_set_size(parent, PANEL_X_PIXELS, PANEL_Y_PIXELS);
  lv_obj_set_style_pad_all(parent, 0, 0);
  lv_obj_set_style_outline_width(parent, 0, 0);
  lv_obj_set_style_border_width(parent, 2, 2);
  lv_obj_set_style_bg_color(parent, lv_color_hex(0x000066), 0);

  LV_FONT_DECLARE(GREAT_LAKES_130px); 
  const lv_font_t *label_font;
  label_font = &GREAT_LAKES_130px;
  data_label = lv_label_create(parent);
  lv_obj_set_style_text_font(data_label, label_font, 0);
  lv_label_set_text(data_label, "OPEN");

  lv_label_set_long_mode(data_label, LV_LABEL_LONG_WRAP);
  lv_obj_align(data_label, LV_ALIGN_TOP_RIGHT, -10, -20);
  lv_obj_set_style_text_color(data_label, lv_color_hex(0xB0C4DE), LV_PART_MAIN | LV_STATE_DEFAULT); 
}

// Splash screen display
void mescControlsPanel(lv_obj_t * parent) {
  lv_obj_set_pos(parent, 0, 60);
  lv_obj_set_size(parent, PANEL_X_PIXELS, PANEL_Y_PIXELS);
  lv_obj_set_style_pad_all(parent, 0, 0);
  lv_obj_set_style_outline_width(parent, 0, 0);
  lv_obj_set_style_border_width(parent, 2, 2);
  lv_obj_set_style_bg_color(parent, lv_color_hex(0x000066), 0);

  lv_obj_t * img = lv_img_create(parent);
  lv_img_set_src(img, &mesc);
  lv_obj_center(img);
}

void guiTask(void *parameter) {
  char buffer[128];

  lv_obj_t *led = (lv_obj_t *)parameter; 
  uint8_t i = 0; // counter for color cycling
  uint32_t colors_off[10] = {
    0x093162, 0x084c7c, 0x0582b0, 0x03a5d2, 0x02b5e1,
    0x00d4ff, 0x02b5e1, 0x03a5d2, 0x0582b0, 0x084c7c};

  uint32_t colors_on[10] = {
    0x625809, 0x7c6b08, 0xb08f05, 0xd2a503, 0xe1b502,
    0xffd400, 0xe1b502, 0xd2a503, 0xb08f05, 0x7c6b08
  };

  bool splashScreenFlag = true;
  guiStartTime = millis();

  while (true) {
    // test if getting json from udp
    if (lv_tick_get() - last_data_receive > 400) {
      // not getting json data
      zeroOutDisplay(); 
      snprintf(buffer, sizeof(buffer), "Udp pulse recvd");
      lv_led_set_color(led, lv_color_hex(colors_off[i]));
    }
    else if (lv_tick_get() - last_udp_receive <= 12000) {
      // getting an occasional pulse, but no json
      snprintf(buffer, sizeof(buffer), "Udp pulse recvd");
      lv_led_set_color(led, lv_color_hex(colors_on[i]));
    }
    else {
      snprintf(buffer, sizeof(buffer), "No udp pulse");
      lv_led_set_color(led, lv_color_hex(colors_off[i]));
    }
    lv_label_set_text(udpstatus_label, buffer);

    i++; if(i > 9) {i = 0;}

    // this section updates other parts of UI
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

    snprintf(buffer, sizeof(buffer), "log name: %s", getLogFileName());
    lv_label_set_text(logname_label, buffer);

    switch (displayState) {
    case DISPLAY_MPH:
      segmentDisplayLetter = 'M';
      break;
    case DISPLAY_AMP:
      segmentDisplayLetter = 'A';
      break;
    case DISPLAY_EHRZ:
      segmentDisplayLetter = 'E';
      break;
    case DISPLAY_BAT:
      segmentDisplayLetter = 'B';
      break;
    default:
      break;
    }

    if ((millis() - guiStartTime) < 2000) {
      // start with the mesc splash screen
      lv_obj_clear_flag(mesc_parent, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(controls_parent, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(data_controls_parent, LV_OBJ_FLAG_HIDDEN);
      Serial.println("CLEAR MESC");
    }
    else { // then stop
      if (splashScreenFlag) {
	lv_event_send(ehrz_btn, LV_EVENT_CLICKED, NULL);
      }
      splashScreenFlag = false;
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void zeroOutDisplay(void) {
  char buffer[12];
  segmentDisplayInt = 0;
  snprintf(buffer, sizeof(buffer), "%d", (int) 0);
  lv_label_set_text(data_label, buffer);
}

void queueTask(void *parameter) {
  char buffer[12];

  while (true) {
    // this is blocked unless a xQueueReceive() is received
    if (xQueueReceive(displayQueue, &displayDataRequest, portMAX_DELAY) == pdPASS) {
      switch(displayState)
	  {
	  case DISPLAY_MPH:
	    snprintf(buffer, sizeof(buffer), "%d", (int) displayDataRequest.mph);
	    segmentDisplayInt = (int) displayDataRequest.mph;
	    break;
	  case DISPLAY_AMP:
	    snprintf(buffer, sizeof(buffer), "%d", (int) displayDataRequest.amp);
	    segmentDisplayInt = (int) displayDataRequest.amp;
	    break;
	  case DISPLAY_EHRZ:
	    snprintf(buffer, sizeof(buffer), "%d", (int) displayDataRequest.ehz);
	    segmentDisplayInt = (int) displayDataRequest.ehz;
	    break;
	  case DISPLAY_BAT:
	    snprintf(buffer, sizeof(buffer), "%d", (int) displayDataRequest.bat);
	    segmentDisplayInt = (int) displayDataRequest.bat;
	    break;
	  default:
	    break;
	  }
      lv_label_set_text(data_label, buffer);
      snprintf(buffer, sizeof(buffer), "%dC", (int) (displayDataRequest.TMOT - 273.15));
      lv_label_set_text(tmot_label, buffer);
      snprintf(buffer, sizeof(buffer), "%dC", (int) (displayDataRequest.TMOS - 273.15));
      lv_label_set_text(tmos_label, buffer);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
