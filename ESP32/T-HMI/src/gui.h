// gui.h
#ifndef GUI_H
#define GUI_H

#include <lvgl.h>
#include <Arduino.h>
#include "IMAGES/button_on.h"
#include "IMAGES/button_off.h"
#include "IMAGES/controls_on.h"
#include "IMAGES/controls_off.h"
#include "IMAGES/temp_hi.h"
#include "IMAGES/temp_lo.h"

// Function declarations
void setupGUI();
void btnEventCB(lv_event_t * e);
void logBtnCB(lv_event_t * e);
void displayNetworkInfo(lv_obj_t * parent);
void numberUpdateTask(void * parameter);
void guiTask(void *parameter);
void createControlsPanel(lv_obj_t * parent);
void zeroOutDisplay();
void dataControlsPanel(lv_obj_t * parent);
void mescControlsPanel(lv_obj_t * parent);
void udpTask(void *parameter);
void queueTask(void *parameter);
lv_obj_t * createButtonWithImage(int x, int y, int width, int height, 
                                 const lv_img_dsc_t *img_on, const lv_img_dsc_t *img_off,
                                 const char *name,
                                 lv_event_cb_t btnEventCB);

#endif // GUI_H
