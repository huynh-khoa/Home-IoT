#ifndef TFT_DISPLAY_H
#define TFT_DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// ===== Cấu hình chân TFT =====
#define TFT_CS   32  
#define TFT_RST  33
#define TFT_DC   25
#define TFT_SDA  27
#define TFT_SCL  14

extern Adafruit_ST7735 tft;

// ===== Các hàm hiển thị =====
void initDisplay();
void displayDHT(float temp, float humi);
void displayPZEM(float voltage, float power);
void displayStatus(bool isRaining, int servoPos, bool rainSensorEnabled);

#endif
