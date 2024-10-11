// gui.h
#ifndef GUI_H
#define GUI_H

#include <lvgl.h>
#include "button_on.h"
#include "button_off.h"
#include "controls_on.h"
#include "controls_off.h"
#include "temp_hi.h"
#include "temp_lo.h"

// Function declarations
void setupGUI();
void btnEventCB(lv_event_t * e);
void logBtnCB(lv_event_t * e);
void displayNetworkInfo(lv_obj_t * parent);
void numberUpdateTask(void * parameter);
void throbLedTask(void *parameter);
void createControlsPanel(lv_obj_t * parent);
void dataControlsPanel(lv_obj_t * parent);
lv_obj_t * createButtonWithImage(int x, int y, int width, int height, 
                                 const lv_img_dsc_t *img_on, const lv_img_dsc_t *img_off,
                                 const char *name,
                                 lv_event_cb_t btnEventCB);

#endif // GUI_H
