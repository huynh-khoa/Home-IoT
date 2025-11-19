#include "tft_display.h"

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_SDA, TFT_SCL, TFT_RST);

static float lastTemp = -999, lastHumi = -999;
static float lastEnergy = -999, lastPower = -999;
static bool lastRain = false;
static int lastServo = -1;

// ======================= KHỞI TẠO =======================
void initDisplay() {
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(2); // Xoay 90 độ để chữ nằm ngang
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.setCursor(10, 5);
  tft.print("HOME IOT");//Tên nền

  tft.drawFastHLine(0, 25, tft.width(), ST77XX_GREEN);
  tft.setTextSize(1);
  //Chọn vị trí đặt tên dữ liệu Temp,humi,...
  tft.setCursor(5, 30);  tft.print("Temp:");
  tft.setCursor(5, 50);  tft.print("Humi:");
  tft.setCursor(5, 70);  tft.print("Power:");
  tft.setCursor(5, 90);  tft.print("Energy:");
  tft.setCursor(5, 110); tft.print("Rain:");
  tft.setCursor(5, 130); tft.print("Clothes:");
  tft.setCursor(5, 150); tft.printf("Water S:");
}

// ======================= CẬP NHẬT DHT =======================
void displayDHT(float temp, float humi) {
  static unsigned long lastUpdate = 0;
  if( millis()- lastUpdate < 2000) return ; // làm chậm thời gian cập nhật lên màn 
  if (fabs(temp - lastTemp) > 0.2 || fabs(humi - lastHumi) > 0.5) {
    tft.fillRect(70, 30, 80, 30, ST77XX_BLACK);
    tft.setTextColor(ST77XX_CYAN);
    tft.setTextSize(1);
    tft.setCursor(70, 30);
    tft.printf("%.1f C", temp);
    tft.setCursor(70, 50);
    tft.printf("%.1f %%", humi);
    lastTemp = temp;
    lastHumi = humi;
  }
}

// ======================= CẬP NHẬT PZEM =======================
void displayPZEM(float energy, float power) {
  if (fabs(energy - lastEnergy) > 0.1 || fabs(power - lastPower) > 1.0) {
    tft.fillRect(70, 70, 80, 40, ST77XX_BLACK);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextSize(1);
    tft.setCursor(70, 70);
    tft.printf("%.1f W", power);
    tft.setCursor(70, 90);
    tft.printf("%.1f J", energy);
    lastEnergy = energy;
    lastPower = power;
  }
}

// ======================= CẬP NHẬT TRẠNG THÁI =======================
void displayStatus(bool isRaining, int servoPos, bool rainSensorEnabled) {
  static bool lastRainShown = !isRaining;  // ép khác để lần đầu in ra
  static int lastServoShown = -1;
  static bool lastSensorShown = !rainSensorEnabled;
  static int lastServoMode = -1;  // 0: DRYING, 1: RETRACT
  if (isRaining != lastRainShown) {
    tft.fillRect(70, 110, 50, 10, ST77XX_BLACK);
    tft.setCursor(70, 110);
    tft.setTextColor(isRaining ? ST77XX_BLUE : ST77XX_WHITE, ST77XX_BLACK);
    tft.print(isRaining ? "RAIN " : "CLEAR");
    lastRainShown = isRaining;
  }
  int currentMode = lastServoMode;
  if (servoPos == 0)
    currentMode = 0;  // DRYING
  else if (servoPos == 90)
    currentMode = 1;  // RETRACT
  // Chỉ cập nhật khi trạng thái thay đổi thực sự
  if (currentMode != lastServoMode) {
    tft.fillRect(70, 130, 80, 10, ST77XX_BLACK);
    tft.setCursor(70, 130);
    tft.setTextColor(ST77XX_MAGENTA, ST77XX_BLACK);
    tft.print(currentMode == 1 ? "RETRACT" : "DRYING");
    lastServoMode = currentMode;
  }
  //Water sensor on hoac off
  if (rainSensorEnabled != lastSensorShown) {
        tft.fillRect(70, 150, 50, 10, ST77XX_BLACK);
        tft.setCursor(70, 150);
        tft.setTextColor(rainSensorEnabled ? ST77XX_GREEN : ST77XX_RED, ST77XX_BLACK);
        tft.print(rainSensorEnabled ? "ON" : "OFF");
        lastSensorShown = rainSensorEnabled;
    }
}

