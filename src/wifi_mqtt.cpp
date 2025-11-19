#include"wifi_mqtt.h"
#include<ArduinoJson.h>

WiFiClientSecure espClient;   // Dùng kết nối bảo mật
PubSubClient client(espClient); //Dùng thư viện PubSubClient để kết nối với MQTT qua bảo mật trên
//==================================================
//    Callback khi có tin nhắn tới từ các topic
//==================================================
void mqttCallback(char* topic, byte* payload, unsigned int length){
    Serial.println("📥 Callback đã kích hoạt!");
    //chuyển payload thành string
    String msg;
    for(unsigned int i=0; i< length; i++){
        msg += (char)payload[i];
    }
    Serial.printf("📩 Nhận từ topic[%s]: %s\n", topic, msg.c_str());
    //Dieu khien cam bien chong trom
    if(String(topic)=="home/control/pir"){
        if(msg=="ON"){
            Serial.println(msg);
            alarmEnabled= true;
            blinkLED(LED_GREEN, 2, 150);
            digitalWrite(LED_GREEN, HIGH);
            digitalWrite(LED_RED, LOW);
            Serial.println("✅ Chống trộm: BẬT");
        }else if(msg=="OFF"){
            alarmEnabled= false;
            blinkLED(LED_RED, 2, 150);
            digitalWrite(LED_GREEN, LOW);
            digitalWrite(LED_RED, LOW);
            digitalWrite(BUZZER, HIGH);
            Serial.println("❌Chống trộm: TẮT");
        }
    }
    //dieu khien cam bien mua
    else if(String(topic)== "home/control/rain"){
        if(msg=="ON"){
            rainSensorEnabled= true;
            Serial.println("✅ Rain Sensor: BẬT");
        }
        else if(msg=="OFF"){
            rainSensorEnabled= false;
            Serial.println("❌Rain Sensor: TẮT");
        }
    }
    else if(String(topic)== "home/control/clothes"){
        if(msg=="ON"){
            servoTarget=0; // phơi quần áo
            Serial.println("👕 Phơi quần áo");
        }
        else if(msg=="OFF"){
            servoTarget=90; // rut quan ao
            Serial.println("👕 Rút quần áo");
        }
    }
}
//=======================================
//   Gưi dũ liệu lên MQTT( gửi định kỳ)
//=======================================
void publishSensorData(){
    StaticJsonDocument<256> doc;
    doc["temperature"] = sensordata.temperature;
    doc["humidity"] = sensordata.humidity;
    doc["voltage"] = sensordata.voltage;
    doc["current"] = sensordata.current;
    doc["power"] = sensordata.power;
    doc["energy"] = sensordata.energy;
    doc["frequency"] = sensordata.frequency;
    doc["pf"] = sensordata.pf;
    //gui mqtt
    char buffer[256];
    serializeJson(doc, buffer); //Biến doc thành dạng chuỗi json
    Serial.println("JSON gửi đi: ");
    Serial.println(buffer);
    client.publish("home/sensors/data", buffer);
}

// Kiểm tra trạng thái thực tế của thiết bị (chỉ gửi khi có sự thay đổi)
bool firstRun= true;
bool lastAlarmEnabled = false;
bool lastRainSensorEnabled = false;
float lastServoTarget = -1; // 0 → ON, 90 → OFF
void publishState() {
    if(firstRun || 
        alarmEnabled != lastAlarmEnabled || 
        rainSensorEnabled != lastRainSensorEnabled || 
        servoTarget != lastServoTarget) {
        //Tao json
        StaticJsonDocument<256> doc;
        doc["pir"] = alarmEnabled ? "ON" : "OFF";
        doc["watersensor"] = rainSensorEnabled ? "ON" : "OFF";
        doc["clothes"] = (servoTarget == 0) ? "ON" : "OFF";
        char buffer[256];
        serializeJson(doc, buffer);
        client.publish("home/device/state", buffer);
        //debug 
        Serial.println("📤 Trạng thái thiết bị gửi đi:");
        serializeJsonPretty(doc, Serial);
        Serial.println();
        // cập nhật biến lưu trạng thái
        lastAlarmEnabled = alarmEnabled;
        lastRainSensorEnabled = rainSensorEnabled;
        lastServoTarget = servoTarget;
        firstRun = false;//Đã gửi lần đâu( khởi động thiết bị)
    }
}
//============================================
//  Gửi các cảnh báo(trộm, mất điện, mưa)
//============================================
void guiCanhBao() {
  if (!client.connected()) return;

  if (coTrom) {
    client.publish("home/alert/pir", "INTRUDER DETECTED");
    Serial.println("ALERT: [home/alert/pir] INTRUDER DETECTED");
    coTrom = false;
  }
  if (matDien) {
    client.publish("home/alert/power", "POWER OUTAGE");
    Serial.println("ALERT: [home/alert/power] POWER OUTAGE");
    matDien = false;
  }
  if (coDien) {
    client.publish("home/alert/power", "POWER RESTORED");
    Serial.println("ALERT: [home/alert/power] POWER RESTORED");
    coDien = false;
  }
  if (batDauMua) {
    client.publish("home/alert/rain", "RAIN STARTED");
    Serial.println("ALERT: [home/alert/rain] RAIN STARTED");
    batDauMua = false;
  }
  if (hetMua) {
    client.publish("home/alert/rain", "RAIN STOPPED");
    Serial.println("ALERT: [home/alert/rain] RAIN STOPPED");
    hetMua = false;
  }
}

//=========================================================
//              Kết nối WiFi và MQTT
//=========================================================
void connectWiFi(const char* ssid, const char* password){
    WiFi.begin(ssid, password);
    Serial.print("🔌 Đang kết nối WiFi");
    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    Serial.printf("\n✅ WiFi OK! IP: %s\n", WiFi.localIP().toString().c_str());
}


void connectMQTT(const char* broker, int port, const char* user, const char* pass){
    espClient.setInsecure();
    client.setServer(broker, port);
    client.setCallback(mqttCallback);
    Serial.print("Connecting MQTT...");
    while (!client.connected()){
        if(client.connect("ESP32Client", user, pass)){
            Serial.printf("\n✅ MQTT connected!");
            //dong kiem tra 
            client.subscribe("home/control/#");
            Serial.println("\n📡 Đã đăng ký topic home/control/#");
        }
        else{
            Serial.print(".");
            delay(1000);
        }
    }
}

//============================================================
//Phải gọi liên tục trong loop() để xử lý tin nhắn từ MQTT
//============================================================
void mqttLoop(){
    client.loop();// duy trì kết nối với MQTT
}
