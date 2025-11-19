#ifndef SENSORS_H
#define  SENSORS_H

struct SensorData {
  float temperature; //Nhiệt độ (°C)
  float humidity;    //Độ ẩm (%)
  float voltage;     //Điện áp lưới (V)
  float current;     //Dòng điện (A)
  float power;       //Công suất tức thời (W)
  float energy;      //Điện năng tiêu thụ (KWh)
  float frequency;   //Tần số lưới (Hz)
  float pf;          //Hệ số công suất (Power Factor)
};

extern volatile SensorData sensordata;
//=================================================
//Các cờ báo hiệu sự kiện (được set trong sensor.c)
//=================================================
extern volatile bool coTrom;
extern volatile bool batDauMua;
extern volatile bool hetMua;
extern volatile bool matDien;
extern volatile bool coDien;
//=======================
//Hàm đọc và cập nhật biến 
//======================== 
void readDHT22();
void readPZEM();
void updateRain();
void updateServo();
void checkIR();
void updatePIR();
void updateDisplay();
void updateBlinkLED();
//============================================================
//Khởi tạo tất cả các cảm biến và ngoại vi( gọi trong setup())
//============================================================
void initSensor();

#endif
