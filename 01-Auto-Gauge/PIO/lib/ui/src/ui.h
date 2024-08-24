// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.2
// LVGL version: 8.3.11
// Project name: Auto gauge

#ifndef _AUTO_GAUGE_UI_H
#define _AUTO_GAUGE_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined __has_include
  #if __has_include("lvgl.h")
    #include "lvgl.h"
  #elif __has_include("lvgl/lvgl.h")
    #include "lvgl/lvgl.h"
  #else
    #include "lvgl.h"
  #endif
#else
  #include "lvgl.h"
#endif

#include "ui_helpers.h"
#include "ui_events.h"

void needle_Animation( lv_obj_t *TargetObject, int delay);
// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
extern lv_obj_t *ui_Screen1;
extern lv_obj_t *ui_Image_gauge_bg;
extern lv_obj_t *ui_Label1;
extern lv_obj_t *ui_Label2;
extern lv_obj_t *ui_Label_rpm;
extern lv_obj_t *ui_Image_car;
extern lv_obj_t *ui_Arc_rpm;
extern lv_obj_t *ui_Image_gauge_needle;
extern lv_obj_t *ui____initial_actions0;

LV_IMG_DECLARE( ui_img_1920837027);   // assets/images/gauge-bg.png
LV_IMG_DECLARE( ui_img_images_car_icon_png);   // assets/images/car_icon.png
LV_IMG_DECLARE( ui_img_1251939133);   // assets/images/gauge-indicator.png
LV_IMG_DECLARE( ui_img_874541913);   // assets/images/gauge-needle.png


LV_FONT_DECLARE( ui_font_t20);
LV_FONT_DECLARE( ui_font_t50);


void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif