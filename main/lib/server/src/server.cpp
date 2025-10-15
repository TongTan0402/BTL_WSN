#include "server.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Update.h>
#include "device_id.h"
#include <iostream>
#include <sstream>

#include "wifi_config.h"


// Private Variables
HTTPClient http;
WiFiClientSecure client_secure;

// Global Server Object
Server_c Server;

const char* mqtt_server = "test.mosquitto.org";
const char* firmware_url = "https://raw.githubusercontent.com/TongTan0402/firmware_esp32_ota/main/firmware.bin";
String device_topic = "ota/";
String update_topic = "";
String wifi_config_topic = "";

bool update_firmware = false;
bool wifi_config = false;
unsigned long lastReconnectAttempt = 0;  // Thời gian lần cuối thử reconnect

WiFiClient espClient;
PubSubClient client(espClient);

void Server_c::OtaUpdate() 
{
  HTTPClient http;
  http.begin(firmware_url);

  unsigned long startTime = millis();
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {  
    int total = http.getSize();
    WiFiClient *stream = http.getStreamPtr();
    if (!Update.begin(total)) {
      Serial.println("Không thể bắt đầu OTA!");
      return;
    }

    uint8_t buff[512];
    int written = 0;
    unsigned long lastPrint = 0;

    Serial.printf("Bắt đầu cập nhật firmware (%d byte)...\n", total);

    while (http.connected() && (written < total || total == -1)) {
      size_t size = stream->available();
      if (size) {
        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
        Update.write(buff, c);
        written += c;

        if (millis() - lastPrint > 1000) {
          float percent = (total > 0) ? (written * 100.0 / total) : 0;
          Serial.printf("Đã nạp: %d/%d byte (%.1f%%)\n", written, total, percent);
          lastPrint = millis();
        }
      }
      delay(1);
    }

    if (Update.end(true)) {
      unsigned long duration = millis() - startTime;
      Serial.printf("Cập nhật thành công! Tổng thời gian: %.2f giây\n", duration / 1000.0);
      
      if (update_firmware) {
        // Xóa retained message bằng cách publish empty message
        client.publish(update_topic.c_str(), "false", true);
        delay(500);
      }
      
      Serial.println("Đang khởi động lại...");
      Serial.flush();  // Đảm bảo message được in ra
      
      // Dọn dẹp trước khi restart
      update_firmware = false;
      client.disconnect();  // Ngắt kết nối MQTT
      WiFi.disconnect(true);  // Ngắt WiFi hoàn toàn
      delay(1000);  // Đợi ngắt kết nối hoàn tất
      
      ESP.restart();
      while(true) { delay(1000); }  // Đợi ESP restart
    } 
    else 
    {
      Serial.print("Lỗi khi cập nhật: ");
      Update.printError(Serial);
    }
  } 
  else 
  {
    Serial.printf("HTTP GET thất bại! Mã lỗi: %d\n", httpCode);
  }
}

void MQTT_Callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String msg = (char*)payload;
  Serial.printf("Nhận từ topic %s: %s\n", topic, msg.c_str());
  
  if (msg == "true") 
  {
    if(String(topic) == update_topic)
    {
      Serial.println("Đã nhận lệnh UPDATE!");
      
      update_firmware = true;
    }

    if(String(topic) == wifi_config_topic)
    {
      Serial.println("Đã nhận lệnh WIFI_CONFIG!");

      wifi_config = true;
    }
  }
}

void MQTT_Reconnect() {
  while (!client.connected()) {
    Serial.println("Đang kết nối MQTT...");
    
    // Tạo Client ID duy nhất dựa trên Device ID
    if (client.connect(GetDeviceId().c_str())) 
    {
      Serial.println("MQTT kết nối thành công!");
      // Subscribe với QoS 1 để đảm bảo nhận được message
      bool subscribed = client.subscribe((device_topic + "/#").c_str(), 1);
      if (subscribed) {
        Serial.printf("Đã subscribe topic: %s với QoS 1\n", device_topic.c_str());
      } else {
        Serial.println("Lỗi subscribe topic!");
      }
    } 
    else 
    {
      Serial.printf("MQTT kết nối thất bại, rc=%d. Thử lại sau 2s...\n", client.state());
      delay(2000);
    }
  }
}

// -------------------------------

void WiFi_MQTT_Init() {
  device_topic += GetDeviceId();
  update_topic = device_topic + "/update_firmware";
  wifi_config_topic = device_topic + "/wifi_config";

  Serial.println("\nDevice ID: " + GetDeviceId() + "\n");

  ConnectToWiFi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(MQTT_Callback);
  client.setKeepAlive(60);  // Keep-alive 60s (giá trị ổn định)
  client.setBufferSize(64);  // Tăng buffer size để nhận message tốt hơn
}


bool Server_c::IsWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void Server_c::MQTTLoop() {
  // Chỉ reconnect nếu chưa kết nối VÀ đã qua 5 giây từ lần thử cuối
  if (!client.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > 5000) {  // 5 giây giữa các lần reconnect
      lastReconnectAttempt = now;
      MQTT_Reconnect();
    }
  } else {
    // Nếu đã kết nối, gọi loop() để xử lý message
    client.loop();

    if(wifi_config)
    {
      wifi_config = false;
      client.publish(wifi_config_topic.c_str(), "false", true);
      delay(500);
      Set_Wifi_Infor_To_Flash("", "");
      ESP.restart();  
      while(true) { delay(1000); }  // Đợi ESP restart
    }
  }
}

void Server_c::begin(const char* scriptURL)
{
  this->scriptURL = scriptURL;
  
  client_secure.setInsecure();

  WiFi_MQTT_Init();
}

void Server_c::sendData(String payload)
{
  if (WiFi.status() == WL_CONNECTED) 
  {
    http.begin(client_secure, this->scriptURL);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(payload);

    Serial.print("HTTP Response: ");
    Serial.println(httpResponseCode);
    http.end();
    if(httpResponseCode == 302) {
      Serial.println("Dữ liệu đã gửi thành công!");
      Serial.println("Payload sent: " + payload + "\n");
    } else {
      Serial.println("Lỗi khi gửi dữ liệu!");
    }
  }
}

bool Server_c::CheckUpdate() 
{
  return update_firmware;
}
