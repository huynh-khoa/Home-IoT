#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include<pins.h>
#include<WiFi.h>
#include<PubSubClient.h>
#include <WiFiClientSecure.h>
#include"sensors.h"

//Chân LED báo trạng thái PIR(chống trộm)
#define LED_GREEN 22
#define LED_RED 23

extern WiFiClientSecure  espClient;  
extern PubSubClient client;

//============================================================
//Biến trạng thái lấy từ file sensors.cpp(để điều khiển từ MQTT)
extern bool alarmEnabled;       //Chóng trộm ON/OFF
extern bool rainSensorEnabled;  //Cảm biến mưa ON/OFF
extern float servoTarget;       //Góc đến Servo(phơi || rút)
extern const unsigned long blinkDelayTime;
//Hàm nháy LED (gọi từ MQTT Callback)
extern void blinkLED(int ledPin, int times, unsigned long delayTime);
//================================================================
//Hàm kết nối mạng và MQTT
//================================================================
void connectWiFi(const char* ssid, const char* password);                            //Kết nối WiFi
void connectMQTT(const char* broker, int port, const char* user, const char* pass);  //Kết nối MQTT
//Hàm sử lý tin nhắn & gửi dữ liệu 
void mqttCallback(char* topic, byte* payload, unsigned int length); //Callback khi nhận lệnh
void mqttLoop();                                                    //Gọi liên tục trong loop()
void publishSensorData();        //Gửi dữ liệu cảm biến
void publishState();             //Gửi trạng thái thiết bị (Pir, rain, clothers)
void guiCanhBao();               //Gửi cảnh báo: trộm, mất điện, mưa đất đầu/hết
#endif