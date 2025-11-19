#include<Arduino.h>
#include<sensors.h>
#include<pins.h>
#include <IRremote.hpp>
#include <ESP32Servo.h>
#include "DHT.h"
#include <PZEM004Tv30.h>
#include "tft_display.h"

//================================================
//Biến toàn cục 
//================================================
volatile SensorData sensordata; //Dữ liệu cảm biến chung
Servo myServo;
DHT dht(DHTPIN, DHTTYPE);
PZEM004Tv30 pzem(Serial2, PZEM_RX, PZEM_TX);

volatile bool alarmEnabled = true;         //chống trộm ON/OFF
bool motionState = false;                  //Trạng thái đang báo động PIR
unsigned long lastMotionTime = 0;
const unsigned long alarmDuration = 3000;  //Thời gian còi báo động (ms)

volatile bool rainSensorEnabled = true;    //Cảm biến mưa ON/OFF
volatile bool isRaining = false;

unsigned long previousServoMillis = 0;    
const unsigned long servoInterval = 15;  // thời gian cập nhật mỗi bước (ms)
float servoCurrent = 0;                  // góc hiện tại
volatile float servoTarget = 0;          // góc đích
float servoSpeed = 1;                    // tốc độ (độ mỗi bước)

const unsigned long blinkDelayTime = 150;
//==================================================
//Cờ cảnh báo sự kiện (dùng để gửi MQTT)
//=================================================
volatile bool coTrom = false;
volatile bool batDauMua = false;
volatile bool hetMua = false;
volatile bool matDien = false;
volatile bool coDien = false;
//=====================================================
// Đọc PZEM-004T (Điện áp , dòng, công suất,...)
//=====================================================
void readPZEM() {
  static unsigned long lastReadPZEM = 0;
  const unsigned long readInterval = 1000;
  if (millis() - lastReadPZEM < readInterval) return;
  lastReadPZEM = millis();

  sensordata.voltage = pzem.voltage();
  sensordata.current = pzem.current();
  sensordata.power   = pzem.power();
  sensordata.energy  = pzem.energy();
  sensordata.frequency = pzem.frequency();
  sensordata.pf      = pzem.pf();

  static float dienApCu = 220.0;
  //Phát hiện mất điện
  if (isnan(sensordata.voltage) || sensordata.voltage < 50.0) {
    if (dienApCu >= 50.0) {
        matDien = true;
        coDien  = false;
        Serial.println("⚡ MẤT ĐIỆN hoặc LỖI ĐỌC PZEM!");
    }
    dienApCu = 0; // cập nhật điện áp cũ
    return;
  }
  // Phát hiện điện có lại
  if (sensordata.voltage >= 100.0 && dienApCu < 50.0) {
      coDien  = true;
      matDien = false;
      Serial.println("⚡ ĐÃ CÓ ĐIỆN!");
  }
    dienApCu = sensordata.voltage;

  Serial.printf("⚡ V: %.1fV | A: %.2fA | W: %.1fW | kWh: %.3f | Hz: %.1f | PF: %.2f\n",
                sensordata.voltage, sensordata.current, sensordata.power, sensordata.energy, sensordata.frequency, sensordata.pf);
  displayPZEM(sensordata.energy, sensordata.power);
}
//=================================================
// Đọc DHT22 (nhiệt độ và độ ẩm)
//=================================================
void readDHT22() {
  static unsigned long lastReadTime = 0;
  const unsigned long readInterval = 5000; // 5 s đọc 1 lần
  if (millis() - lastReadTime < readInterval) return;
  lastReadTime = millis();

  sensordata.humidity = dht.readHumidity();
  sensordata.temperature = dht.readTemperature();
  if (isnan(sensordata.humidity) || isnan(sensordata.temperature)) {
    Serial.println("❌ Lỗi đọc cảm biến DHT22!");
    return;
  }

  Serial.printf("💧 %.1f%%  🌡️ %.1f°C\n", sensordata.humidity, sensordata.temperature);
  displayDHT(sensordata.temperature, sensordata.humidity);
}
//
// Cập nhật cảm biến mưa-> tự động rút/phơi quần áo
void updateRain() {
  static unsigned long lastPrint = 0;
  if (!rainSensorEnabled) return;

  int rainValue = analogRead(WATER_PIN);
  const int threshold = 500;     //Ngưỡng điều chỉnh tùy cảm biến 
  bool rainingNow = (rainValue > threshold);

  if (rainingNow != isRaining) {
    isRaining = rainingNow;
    if (isRaining) {
      Serial.println("🌧️ Trời mưa, rút quần áo");
      servoTarget = 90;
      batDauMua = true; // Cờ theo dõi thời tiết
    } else {
      Serial.println("☀️ Trời khô, phơi quần áo");
      servoTarget = 0;
      hetMua = true; //Cờ theo dõi thời tiết
    }
  }

  if (millis() - lastPrint > 5000) {
    Serial.printf("Rain value: %d\n", rainValue);
    lastPrint = millis();
  }
}
//==============================================
// Di chuyển SERVO mượt 
//==============================================
void updateServo() {
  unsigned long currentMillis = millis();

  // chỉ cập nhật servo mỗi "servoInterval" mili-giây
  if (currentMillis - previousServoMillis >= servoInterval) {
    previousServoMillis = currentMillis;

    if (servoCurrent < servoTarget) {
      servoCurrent += servoSpeed;
      if (servoCurrent > servoTarget) servoCurrent = servoTarget;  // tránh vượt quá
      myServo.write(servoCurrent);
    } 
    else if (servoCurrent > servoTarget) {
      servoCurrent -= servoSpeed;
      if (servoCurrent < servoTarget) servoCurrent = servoTarget;
      myServo.write(servoCurrent);
    }
  }
}
//=======================
// Nháy LED 
//=======================
struct BlinkState {
  int ledPin = 0;           // Chân LED
  int times = 0;            // Số lần nháy (bật/tắt)
  int currentCount = 0;     // Đếm số lần đã nháy
  bool isActive = false;    // Trạng thái nháy (đang hoạt động hay không)
  unsigned long lastToggle = 0; // Thời điểm chuyển đổi cuối
  unsigned long delayTime = blinkDelayTime; // Thời gian chờ mỗi lần nháy
};
BlinkState blinkState;      // Biến lưu trạng thái nháy LED

//=================BLINK LED=======================
// Khởi tạo nháy LED 
void blinkLED(int ledPin, int times, unsigned long delayTime = blinkDelayTime) {
  if (blinkState.isActive) return; // Thoát nếu đang nháy để tránh xung đột
  
  // Khởi tạo trạng thái nháy
  blinkState.ledPin = ledPin;
  blinkState.times = times * 2; // Mỗi nháy gồm bật và tắt
  blinkState.currentCount = 0;
  blinkState.isActive = true;
  blinkState.lastToggle = millis();
  blinkState.delayTime = delayTime;
  
  digitalWrite(ledPin, !digitalRead(ledPin)); // Đảo trạng thái LED ban đầu
}
// Cập nhật trạng thái nháy LED trong vòng lặp chính
void updateBlinkLED() {
  if (!blinkState.isActive) return; // Thoát nếu không nháy
  
  unsigned long currentMillis = millis();
  if (currentMillis - blinkState.lastToggle >= blinkState.delayTime) {
    // Đảo trạng thái LED
    digitalWrite(blinkState.ledPin, !digitalRead(blinkState.ledPin));
    blinkState.currentCount++;
    blinkState.lastToggle = currentMillis;
    
    // Kết thúc nháy khi đủ số lần
    if (blinkState.currentCount >= blinkState.times) {
      blinkState.isActive = false;
    }
  }
}
//===============================================
// Nhận lệnh từ IR receiver hồng ngoại
//===============================================
void checkIR() {
  if (IrReceiver.decode()) {
    unsigned long code = IrReceiver.decodedIRData.decodedRawData;
    Serial.printf("📥 Mã nhận được: 0x%lX\n", code);

    switch (code)
    {
    case 0xF30CFF00: //Toggle servo
        servoTarget = (servoTarget == 0) ? 90 : 0;
        Serial.println("Remote: Toggle servo");
        break;
    case 0xE718FF00: // Toggle rain sensor
        rainSensorEnabled = !rainSensorEnabled;
        Serial.printf("Remote: Cảm biến mưa %s\n", rainSensorEnabled ? "BẬT" : "TẮT");
        break;
    case 0xE916FF00://Toggle PIR
        alarmEnabled = !alarmEnabled;
        Serial.printf("Remote: PIR %s\n", alarmEnabled ? "BẬT" : "TẮT");
        if (alarmEnabled) {
        blinkLED(LED_GREEN, 2);
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_RED, LOW);
        } else {
        blinkLED(LED_RED, 2);
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_RED, LOW);
        digitalWrite(BUZZER, HIGH);
        motionState = false;
        }
        break;

    default:
       Serial.printf("Mã remote không xử lý : 0x%lX\n", code);
       break;
    }
    IrReceiver.resume();// sẵn sàng nhận lệnh tiếp theo
  }
}
//=====================================
// Xử lý cảm biến chuyển động PIR (chống trộm)
//=====================================
void updatePIR() {
  if (!alarmEnabled) return;

  bool motionDetected = digitalRead(PIR_PIN);
  if (motionDetected && !motionState) {
    motionState = true;
    lastMotionTime = millis();
    Serial.println("⚠️ Phát hiện chuyển động!");
    digitalWrite(LED_RED, HIGH);
    digitalWrite(BUZZER, LOW);
    digitalWrite(LED_GREEN, LOW);
    coTrom = true;
  }

  if (motionState && millis() - lastMotionTime > alarmDuration) {
    motionState = false;
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(BUZZER, HIGH);
  }
}
//============================================
// Cập nhật màn hình TFT
//============================================
void updateDisplay() {
  displayStatus(isRaining, servoCurrent, rainSensorEnabled);
}

//=====================================
//Khởi tạo tất cả cảm biến và ngoại vi
//======================================
void initSensor(){
    Serial2.begin(9600, SERIAL_8N1, PZEM_RX, PZEM_TX);
    dht.begin();
    myServo.attach(13);
    myServo.write(servoCurrent);

    pinMode(PIR_PIN, INPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(WATER_PIN, INPUT);
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, HIGH); // Mặt định buzzer k kêu 
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); //Bật nhận IR

    //Trạng thái ban đầu
    alarmEnabled = false;
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);
}
