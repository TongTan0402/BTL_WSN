#include "all_header.h"

// #define TEST 

#ifndef TEST
MQ2   mq2(PIN_MQ2, 10.0, 5.0);
DHT11 dht(PIN_DHT);
MP25  dust(PIN_PM_LED, PIN_PM_ADC, 3.3);

enum {
  IDLE_PRIORITY = 0,

  SEND_HTTP_PRIORITY ,
  CHECK_UPDATE_PRIORITY,
  READ_SENSORS_PRIORITY,
  LED_PRIORITY ,

  PRIORITY_COUNT
};

TaskHandle_t ReadSensorsHandle; // handle của task đọc cảm biến
TaskHandle_t SendHTTPHandle; // handle của task gửi HTTP
TaskHandle_t CheckUpdateHandle; // handle của task kiểm tra cập nhật
TaskHandle_t LedTaskHandle; // hanlde của task Led Wifi

// Biến lưu dữ liệu cảm biến
struct SensorData {
  float temperature;
  float humidity;
  float dustDensity;
  float ppm;
} sensorData;

SemaphoreHandle_t sensorMutex; // Mutex để bảo vệ dữ liệu cảm biến

unsigned long last_time = 0; // thời gian lần thử reconnect WiFi cuối cùng

void ReadSensorsTask(void *parameter);
void SendHTTPTTask(void *parameter);
void CheckUpdateFirmwareTask(void *parameter);
void LedTask(void *parameter);


LedControl wifiLed(PIN_LED_WIFI);

void setup() {
  wifiLed.begin();
  Serial.begin(115200);
  delay(1000);
  
  xTaskCreatePinnedToCore(LedTask, "LedTask", 2048, NULL, LED_PRIORITY, &LedTaskHandle, 1);
  
  Serial.println("\n\n========================================");
  Serial.println("ESP32 đã khởi động!");
  Serial.printf("Firmware version: %s\n", __DATE__ " " __TIME__);
  Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
  Serial.println("========================================\n");

  // Bật WiFi Modem Sleep để tiết kiệm năng lượng
  WiFi.setSleep(true);
  Serial.println("WiFi Modem Sleep: ENABLED (Power saving mode)\n");
  
  // Kết nối WiFi và khởi tạo server
  Server.begin(scriptURL);
  
  // Khởi tạo cảm biến
  mq2.begin(10.0);         // Ro hiệu chuẩn (kΩ)
  dust.begin(30, 0);   // 10 mẫu, offset -15 µg/m³
  dust.autoCalibrate(120, 20);
  float voc = dust.getVoc();
  dust.setVoc(voc);
  //dust.autoCalibrate(100);
  
  // Tạo mutex
  sensorMutex = xSemaphoreCreateMutex();
  
  // Tạo các task
  xTaskCreatePinnedToCore(ReadSensorsTask, "ReadSensorsTask", 4096, NULL, READ_SENSORS_PRIORITY, &ReadSensorsHandle, 0);
  xTaskCreatePinnedToCore(SendHTTPTTask, "SendHTTPTask", 10000, NULL, SEND_HTTP_PRIORITY, &SendHTTPHandle, 0);
  xTaskCreatePinnedToCore(CheckUpdateFirmwareTask, "CheckMQTTTask", 10000, NULL, CHECK_UPDATE_PRIORITY, &CheckUpdateHandle, 1);
}


void loop() 
{

}


void ReadSensorsTask(void *parameter) {
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = 1000 / portTICK_PERIOD_MS; // 1000 ms = 1 giây

  // Khởi tạo thời điểm ban đầu
  xLastWakeTime = xTaskGetTickCount();
  
  for(;;) {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    // --- Đọc cảm biến khí gas ---
    float ppm = mq2.readPPM();

    // --- Đọc cảm biến bụi ---
    float dustDensity = dust.readDustDensity();

    // --- Đọc cảm biến nhiệt độ & độ ẩm ---
    auto dht11_value = dht.read();

    // Lưu dữ liệu vào biến toàn cục với mutex
    if (xSemaphoreTake(sensorMutex, portMAX_DELAY) == pdTRUE) {
      sensorData.temperature = dht11_value.temperature;
      sensorData.humidity = dht11_value.humidity;
      sensorData.dustDensity = dustDensity;
      sensorData.ppm = ppm;
      xSemaphoreGive(sensorMutex);
    }

    //Serial.printf("Temp: %.1f°C, Humidity: %.1f%%, Dust: %.1f µg/m³, Gas: %.1f ppm\n",
     //             dht11_value.temperature, dht11_value.humidity, dustDensity, ppm);
  }
}

void SendHTTPTTask(void *parameter) {
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = 6000 / portTICK_PERIOD_MS; // 600000 ms = 10 phút

  // Khởi tạo thời điểm ban đầu
  xLastWakeTime = xTaskGetTickCount();
  
  for(;;) {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    // Lấy dữ liệu cảm biến với mutex
    float temp, hum, dust, ppm;
    if (xSemaphoreTake(sensorMutex, portMAX_DELAY) == pdTRUE) {
      temp = sensorData.temperature;
      hum = sensorData.humidity;
      dust = sensorData.dustDensity;
      ppm = sensorData.ppm;
      xSemaphoreGive(sensorMutex);
    }

    // --- Gửi dữ liệu lên server ---
    String payload = PayloadFormat(temp, hum, dust, ppm);
    Server.sendData(payload);
  }
}

void CheckUpdateFirmwareTask(void *parameter)
{
  for (;;)
  {
    Server.MQTTLoop();

    // OTA giữ nguyên
    if (Server.CheckUpdate())
    {
        vTaskSuspend(ReadSensorsHandle);
        vTaskSuspend(SendHTTPHandle);
        Server.OtaUpdate();
    }

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}



void LedTask(void *parameter)
{
  wifiLed.begin();

  for (;;)
  {
    switch(Server.GetState())
    {
      case WIFI_CONNECTED:
        wifiLed.wifiConnected();
        break;

      case WIFI_DISCONNECTED:
        wifiLed.wifiDisconnected();
        break;

      case UPDATE_AVAILABLE:
        wifiLed.UpdateFirmware();
        break;

      default:
        wifiLed.wifiConnecting();
        break;
    }
  }
}



#else
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include "wifi_config_page.h"

void setup() {
  Serial.begin(115200);
  connectToWiFi();
}

void loop() {
  server.handleClient();
}

#endif