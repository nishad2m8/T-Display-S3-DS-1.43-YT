#include <lvgl.h>
#include <Arduino_GFX_Library.h>
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"
#include <ui.h>

// https://www.youtube.com/nishad2m8
// https://buymeacoffee.com/nishad2m8

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */, LCD_SDIO1 /* SDIO1 */,
    LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);

Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST /* RST */,
                                      0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT);

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

void Arduino_IIC_Touch_Interrupt(void);

std::unique_ptr<Arduino_IIC> FT3168(new Arduino_CST816x(IIC_Bus, FT3168_DEVICE_ADDRESS,
                                                        DRIVEBUS_DEFAULT_VALUE, TP_INT, Arduino_IIC_Touch_Interrupt));

void Arduino_IIC_Touch_Interrupt(void)
{
    FT3168->IIC_Interrupt_Flag = true;
}

// LVGL buffer and driver setup
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[LCD_WIDTH * LCD_HEIGHT / 10]; // Larger buffer for better performance

int rpm_value = 0;          // Current RPM value
bool rpm_increasing = true; // Flag for increasing/decreasing RPM

// Function declarations
void simulate_rpm();
void update_rpm_ui(int rpm);

// Display flush function for LVGL
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    int32_t w = (area->x2 - area->x1 + 1);
    int32_t h = (area->y2 - area->y1 + 1);

    // Use draw16bitRGBBitmap() to draw the pixel data
    gfx->startWrite();
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)color_p, w, h);
    gfx->endWrite();

    lv_disp_flush_ready(disp); // Indicate that flushing is done
}

// Touchpad read function for LVGL
void touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    if (FT3168->IIC_Interrupt_Flag == true)
    {
        FT3168->IIC_Interrupt_Flag = false;

        int touch_x = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
        int touch_y = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);

        // Serial.print("Touch Detected at X: ");
        // Serial.print(touch_x);
        // Serial.print(", Y: ");
        // Serial.println(touch_y);

        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch_x;
        data->point.y = touch_y;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}

// RPM Simulation: Simulates the RPM going from 0 to 10,000 and back with smoother transitions
void simulate_rpm()
{
    const int max_rpm = 10000;    // Maximum RPM value (10,000 RPM)
    const int rpm_step = 100;     // Smaller increment step for smoother transition (100 RPM)

    if (rpm_increasing)
    {
        if (rpm_value < max_rpm)
        {
            rpm_value += rpm_step;
        }
        else
        {
            rpm_increasing = false;  // Start decrementing when max RPM is reached
        }
    }
    else
    {
        if (rpm_value > 0)
        {
            rpm_value -= rpm_step;
        }
        else
        {
            rpm_increasing = true;  // Start incrementing when 0 RPM is reached
        }
    }

    update_rpm_ui(rpm_value); // Update the RPM UI elements
}

// Update the LVGL UI elements based on RPM value
void update_rpm_ui(int rpm)
{
    // Update RPM label with the current value
    char rpm_str[10];
    sprintf(rpm_str, "%d", rpm);  // Display the current RPM value (e.g., 100, 200, etc.)
    lv_label_set_text(ui_Label_rpm, rpm_str);

    // Update gauge needle based on RPM value
    // Map 0 RPM to 0 degrees and 10,000 RPM to 2500 degrees
    int needle_angle = map(rpm, 0, 10000, 0, 2500); 
    lv_img_set_angle(ui_Image_gauge_needle, needle_angle);

    // Map RPM value to the range of 0-93 for the arc
    int arc_value = map(rpm, 0, 10000, 0, 93);
    lv_arc_set_value(ui_Arc_rpm, arc_value);  // Update the arc based on RPM value
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting Setup...");

    pinMode(LCD_EN, OUTPUT);
    digitalWrite(LCD_EN, HIGH);

    // Initialize the display
    Serial.println("Initializing Display...");
    gfx->begin();
    gfx->fillScreen(BLACK);
    gfx->Display_Brightness(255);

    // gfx->setRotation(1);  // Try different rotations: 0, 1, 2, 3

    // Initialize LVGL
    Serial.println("Initializing LVGL...");
    lv_init();

    // Initialize display buffer
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, LCD_WIDTH * LCD_HEIGHT / 10);

    // Initialize display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LCD_WIDTH;
    disp_drv.ver_res = LCD_HEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    Serial.println("Display driver registered.");

    // Initialize touch input driver
    FT3168->begin();
    Serial.println("Touch Input Initialized");

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    lv_indev_drv_register(&indev_drv);
    Serial.println("Touch input driver registered.");

    // Initialize the user interface
    ui_init();
}

void loop()
{
    lv_timer_handler(); // Call LVGL processing function
    simulate_rpm();     // Simulate RPM and update the UI
    delay(5);         // Delay for smoother UI update
}
