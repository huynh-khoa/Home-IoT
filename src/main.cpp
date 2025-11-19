#include <Arduino.h>
#include"wifi_mqtt.h"
#include"sensors.h"
#include"tft_display.h"
//=============================================
// Cấu hình thông tin đăng nhập WiFi + MQTT
//=============================================
const char* WIFI_SSID= "Kkk";
const char* WIFI_PASS= "88888888";

const char* MQTT_BROKER= "0aaf6db2d3df41e49993c322ef7048d0.s1.eu.hivemq.cloud";
const int MQTT_PORT= 8883 ;
const char* MQTT_USER="Khoa1";
const char* MQTT_PASS= "Hathinguyet@101075";

//Biến tạo delay gửi topic lên MQTT
unsigned long lastPublishTime=0;
const unsigned long publishInterval = 5000;
// =========================**** SETUP **** =======================
void setup() {
  Serial.begin(115200);
  //Hàm kết nối WiFi và MQTT
  connectWiFi(WIFI_SSID, WIFI_PASS);
  connectMQTT(MQTT_BROKER, MQTT_PORT, MQTT_USER, MQTT_PASS);
  //Khởi tạo Sensor và màn hình tft
  initSensor();
  initDisplay();
  Serial.println("\n✅ Hệ thống khởi động hoàn tất!");
}
// ======================= **LOOP** =======================
void loop() {
  readDHT22();      // Hàm đọc DHT22 
  readPZEM();       //Hàm đọc cảm biến đo thông số điện
  updateRain();     //Hàm kiểm tra trạng thái Water Sensor
  updateServo();    //Hàm gọi điều khiển Servo
  checkIR();        //Hàm kiểm tra vs xử lý tín hiệu từ cảm biến thu IR receier
  updatePIR();      //Hàm xử lý cảm biến nhiệt chuyển động
  updateDisplay();  //Hàm in status từ các cảm biến PIR, Water, Servo
  updateBlinkLED(); //Hàm cập nhật trạng thái led đơn 
  //Hàm gửi dữ liệu 5s 1 lần
  if(millis()- lastPublishTime >= publishInterval){
    lastPublishTime = millis();
    publishSensorData();//Gọi hàm gửi topic( dữ liệu Sensor) lên MQTT
  }
  //Hàm kiểm tra trạng thái thực tế
  publishState();
  guiCanhBao();
  mqttLoop(); 
}
