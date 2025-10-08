#include <Arduino.h>
#include "MQ2.h"
#include "DHT11.h"
#include "MP25.h"

// --- Khai báo chân ---
#define PIN_MQ2    34
#define PIN_DHT    4
#define PIN_PM_LED 23
#define PIN_PM_ADC 32

// --- Khai báo đối tượng cảm biến ---
MQ2   mq2(PIN_MQ2, 10.0, 5.0);
DHT11 dht(PIN_DHT);
MP25  dust(PIN_PM_LED, PIN_PM_ADC, 5.0);

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("=== ESP32 MULTI SENSOR TEST (UPGRADED LIBS) ===");

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
  if (dht.read() && dht.isValid()) {
    float temp = dht.getTemperature();
    float hum  = dht.getHumidity();

    // --- In dữ liệu ra Serial ---
    Serial.printf("Temp: %.1f°C  Hum: %.1f%%  ", temp, hum);
  } else {
    Serial.print("DHT11 Error  ");
  }

  Serial.printf("MQ2: %.2f ppm  ", ppm);
  Serial.printf("Dust: %.2f µg/m³\n", dustDensity);

  delay(2000);
}
