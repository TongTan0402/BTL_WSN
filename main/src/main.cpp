#include "all_header.h"

MQ2   mq2(PIN_MQ2, 10.0, 5.0);
DHT11 dht(PIN_DHT);
MP25  dust(PIN_PM_LED, PIN_PM_ADC, 5.0);

enum {
  IDLE_PRIORITY = 0,
  SEND_HTTP_PRIORITY ,
  CHECK_UPDATE_PRIORITY
};

TaskHandle_t SendHTTPHandle; // handle của task gửi HTTP
TaskHandle_t CheckUpdateHandle; // handle của task kiểm tra cập nhật

unsigned long last_time = 0; // thời gian lần thử reconnect WiFi cuối cùng

void SendHTTPTTask(void *parameter);
void CheckUpdateFirmwareTask(void *parameter);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n========================================");
  Serial.println("ESP32 đã khởi động!");
  Serial.printf("Firmware version: %s\n", __DATE__ " " __TIME__);
  Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
  Serial.println("========================================\n");

  // Kết nối WiFi và khởi tạo server
  Server.begin(scriptURL);
  
  // Khởi tạo cảm biến
  mq2.begin(10.0);         // Ro hiệu chuẩn (kΩ)
  dust.begin(10, -15.0);   // 10 mẫu, offset -15 µg/m³

  xTaskCreatePinnedToCore(SendHTTPTTask, "SendHTTPTTask", 10000, NULL, SEND_HTTP_PRIORITY, &SendHTTPHandle, 1);
  xTaskCreatePinnedToCore(CheckUpdateFirmwareTask, "CheckUpdateFirmwareTask", 10000, NULL, CHECK_UPDATE_PRIORITY, &CheckUpdateHandle, 1);
}


void loop() 
{
  if(Serial.available()) 
  {
    String str = Serial.readStringUntil('\n');
    str.trim();
    if(str.equalsIgnoreCase("change wifi")) 
    {
      if (eTaskGetState(SendHTTPHandle) != eSuspended) 
      {
        vTaskSuspend(SendHTTPHandle);
      }
      if(eTaskGetState(CheckUpdateHandle) != eSuspended) 
      {
        vTaskSuspend(CheckUpdateHandle);
      }

      // Thay đổi WiFi
      while(1)
      {
        Server.ChangeWiFiInfo();
        Server.ReconnectWiFi();
        if (Server.IsWiFiConnected()) {
          vTaskResume(SendHTTPHandle);
          vTaskResume(CheckUpdateHandle);
          break;
        }
      }
    }
  }
}


void SendHTTPTTask(void *parameter) {
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = 5000 / portTICK_PERIOD_MS; // 5000 ms

  // Khởi tạo thời điểm ban đầu
  xLastWakeTime = xTaskGetTickCount();
  for(;;) {
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

    vTaskDelayUntil(&xLastWakeTime, xFrequency); // Đọc mỗi 5 giây
  }
}

void CheckUpdateFirmwareTask(void *parameter) {
  for(;;) {
    Server.MQTTLoop();

    if(Server.CheckUpdate()) 
    {
      if (eTaskGetState(SendHTTPHandle) != eSuspended) 
      {
        vTaskSuspend(SendHTTPHandle);
      }
    
      Serial.println("Bắt đầu cập nhật firmware...");
      Server.OtaUpdate();
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);  // Delay 10ms sau mỗi batch
  }
}
