#include <lvgl.h>

const uint16_t image1_map[] = {

};

const lv_img_dsc_t image1 = {
    .header = {
        .cf = LV_IMG_CF_TRUE_COLOR,
        .w = 191,                  
        .h = 144                   
    },
    .data_size = sizeof(image1_map),
    .data = (const uint8_t *)image1_map
};
