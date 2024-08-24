SET(SOURCES screens/ui_Screen1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_1920837027.c
    images/ui_img_images_car_icon_png.c
    images/ui_img_1251939133.c
    images/ui_img_874541913.c
    fonts/ui_font_t20.c
    fonts/ui_font_t50.c)

add_library(ui ${SOURCES})
