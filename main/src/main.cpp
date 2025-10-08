#include "all_header.h"


MQ2   mq2(PIN_MQ2, 10.0, 5.0);
DHT11 dht(PIN_DHT);
MP25  dust(PIN_PM_LED, PIN_PM_ADC, 5.0);


const char *ssid = "FPT Telecom-3A93 2.4G";
const char *password = "20c01d9a";
const char *scriptURL = "https://script.google.com/macros/s/AKfycbwhTF9oBVzyj8QJrQak8oJ33r3Y5sb9E3t3pU96MB9cQt6CT6TCBKiZiFTQa9Oolbc/exec";

void setup() {
  Serial.begin(115200);
  Server.begin(ssid, password, scriptURL);
  
  // Khởi tạo cảm biến
  mq2.begin(10.0);         // Ro hiệu chuẩn (kΩ)
  dust.begin(10, -15.0);   // 10 mẫu, offset -15 µg/m³
}

void loop() {
  // --- Đọc cảm biến khí gas ---
  float ppm = mq2.readPPM();

  // --- Đọc cảm biến bụi ---
  float dustDensity = dust.readDustDensity();

  // --- Đọc cảm biến nhiệt độ & độ ẩm ---
  auto dht11_value = dht.read();


  // --- Gửi dữ liệu lên server ---
  String payload = PayloadFormat(dht11_value.temperature, dht11_value.humidity, dustDensity, ppm);

  Server.sendData(payload);

  Serial.println("Payload: " + payload);
}
