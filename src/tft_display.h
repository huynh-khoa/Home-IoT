#ifndef TFT_DISPLAY_H
#define TFT_DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// ===================================================
// Chân kết nối TFT ST7735 phần cứng cố định
#define TFT_CS   32  
#define TFT_RST  33
#define TFT_DC   25
#define TFT_SDA  27
#define TFT_SCL  14
// TFT toàn cục (khai báo ở .cpp)
extern Adafruit_ST7735 tft;
//========================================================
//  Các hàm hiển thị trên màn hình TFT
//========================================================
void initDisplay();                                       //Khởi tạo màn hình 
void displayDHT(float temp, float humi);                  //Cập nhật nhiệt độ + độ ẩm (khi thay đổi)
void displayPZEM(float voltage, float power);             //Cập nhật trạng thái công suất & điện năng tiêu thụ
void displayStatus(bool isRaining, int servoPos, bool rainSensorEnabled);
//->Hiển thị: mưa/khô, phơi rút, cảm biến mưa ON/OFF

#endif
