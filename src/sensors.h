#ifndef SENSORS_H
#define  SENSORS_H

struct SensorData {
  float temperature;
  float humidity;
  float voltage;
  float current;
  float power;
  float energy;
  float frequency;
  float pf;
};

extern volatile SensorData sensordata;
//Cờ theo dõi
extern volatile bool coTrom;
extern volatile bool batDauMua;
extern volatile bool hetMua;
extern volatile bool matDien;
extern volatile bool coDien;

//Cam bien 
void readDHT22();
void readPZEM();
void updateRain();
void updateServo();
void checkIR();
void updatePIR();
void updateDisplay();
void updateBlinkLED();
//Khởi tạo cảm biến ( setupSensor )
void initSensor();

#endif