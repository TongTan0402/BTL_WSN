#include "all_header.h"

MQ2   mq2(PIN_MQ2, 10.0, 5.0);
DHT11 dht(PIN_DHT);
MP25  dust(PIN_PM_LED, PIN_PM_ADC, 5.0);

enum {
  IDLE_PRIORITY = 0,
  SEND_HTTP_PRIORITY ,
  CHECK_UPDATE_PRIORITY
};

void SendHTTPTTask(void *parameter) {
  for(;;) {
    if(Server.CheckUpdate()) 
    {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      continue;
    }
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

    vTaskDelay(5000 / portTICK_PERIOD_MS); // Đọc mỗi 5 giây
  }
}

void CheckUpdateFirmwareTask(void *parameter) {
  for(;;) {
    Server.MQTTLoop();

    // Serial.println("Đang chờ...");

    vTaskDelay(1 / portTICK_PERIOD_MS); 
  }
}

void setup() {
  Serial.begin(115200);

  // Kết nối WiFi và khởi tạo server
  Server.begin(scriptURL);
  
  // Khởi tạo cảm biến
  mq2.begin(10.0);         // Ro hiệu chuẩn (kΩ)
  dust.begin(10, -15.0);   // 10 mẫu, offset -15 µg/m³

  xTaskCreatePinnedToCore(SendHTTPTTask, "SendHTTPTTask", 8192, NULL, SEND_HTTP_PRIORITY, NULL, 0);
  xTaskCreatePinnedToCore(CheckUpdateFirmwareTask, "CheckUpdateFirmwareTask", 8192, NULL, CHECK_UPDATE_PRIORITY, NULL, 1);
}


void loop() {
}
