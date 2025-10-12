#include "server.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Update.h>
#include <Preferences.h>  // Dùng để lưu vào flash
#include "device_id.h"


// Private Variables
HTTPClient http;
WiFiClientSecure client_secure;

// Global Server Object
Server_c Server;
Preferences prefs;

char ssid[50];
char password[50];
const char* mqtt_server = "test.mosquitto.org";
const char* firmware_url = "https://raw.githubusercontent.com/TongTan0402/firmware_esp32_ota/main/firmware.bin";
String topic_ota = "ota/";

bool update_firmware = false;
unsigned long lastReconnectAttempt = 0;  // Thời gian lần cuối thử reconnect

WiFiClient espClient;
PubSubClient client(espClient);

void OtaUpdate() {
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
      delay(3000);
      ESP.restart();
    } else {
      Serial.print("Lỗi khi cập nhật: ");
      Update.printError(Serial);
    }

  } else {
    Serial.printf("HTTP GET thất bại! Mã lỗi: %d\n", httpCode);
  }

  http.end();
}

void MQTT_Callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String msg = (char*)payload;
  Serial.printf("Nhận từ topic %s: %s\n", topic, msg.c_str());
  
  if (msg == "update") 
  {
    Serial.println("Đã nhận lệnh UPDATE!");
    
    update_firmware = true;
  }
}

void MQTT_Reconnect() {
  while (!client.connected()) {
    Serial.println("Đang kết nối MQTT...");
    
    // Tạo Client ID duy nhất dựa trên Device ID
    if (client.connect(GetDeviceId().c_str())) {
      Serial.println("MQTT kết nối thành công!");
      // Subscribe với QoS 1 để đảm bảo nhận được message
      bool subscribed = client.subscribe(topic_ota.c_str(), 1);
      if (subscribed) {
        Serial.printf("Đã subscribe topic: %s với QoS 1\n", topic_ota.c_str());
      } else {
        Serial.println("Lỗi subscribe topic!");
      }
    } else {
      Serial.printf("MQTT kết nối thất bại, rc=%d. Thử lại sau 2s...\n", client.state());
      delay(2000);
    }
  }
}

// ---- Hàm nhập và lưu WiFi ----
void InputWiFiInfo() {
  Serial.println("\nKhông kết nối được WiFi!");
  
  // --- Nhập SSID ---
  Serial.println("Nhập SSID: ");
  String s = "";
  while (s.length() == 0) {
    if (Serial.available()) {
      s = Serial.readStringUntil('\n');
      s.trim();
    }
  }

  // --- Nhập Password ---
  Serial.println("Nhập Password: ");
  String p = "";
  while (p.length() == 0) {
    if (Serial.available()) {
      p = Serial.readStringUntil('\n');
      p.trim();
    }
  }

  // --- Xác nhận ---
  Serial.println("\n--- Xác nhận thông tin ---");
  Serial.printf("SSID: %s\n", s.c_str());
  Serial.printf("Password: %s\n", p.c_str());
  Serial.println("Nhập 'y' để xác nhận, hoặc phím khác để nhập lại:");

  while (!Serial.available()); // chờ người dùng nhập
  char confirm = Serial.read();
  Serial.readStringUntil('\n'); // xóa phần dư trong buffer

  if (confirm != 'y' && confirm != 'Y') {
    Serial.println("Hủy lưu. Nhập lại thông tin...");
    delay(500);
    InputWiFiInfo(); // gọi lại hàm để nhập lại
    return;
  }

  // --- Lưu và kết nối ---
  s.toCharArray(ssid, sizeof(ssid));
  p.toCharArray(password, sizeof(password));

  prefs.putString("ssid", ssid);
  prefs.putString("password", password);

  Serial.println("Đã lưu SSID và Password vào flash!");
  Serial.printf("Đang thử kết nối WiFi: %s...\n", ssid);

  WiFi.begin(ssid, password);
}

// -------------------------------

void WiFi_MQTT_Init() {
  Serial.println("\nDevice ID: " + GetDeviceId());

  prefs.begin("wifi", false); // namespace: "wifi"

  // Đọc SSID và password từ flash
  String ssid_saved = prefs.getString("ssid", "");
  String pass_saved = prefs.getString("password", "");

  if (ssid_saved.length() > 0) {
    ssid_saved.toCharArray(ssid, sizeof(ssid));
    pass_saved.toCharArray(password, sizeof(password));
    Serial.printf("Đọc từ flash: SSID=%s\n", ssid);
  } else {
    Serial.println("Chưa có SSID lưu trong flash.");
    ssid[0] = '\0';
    password[0] = '\0';
  }

  if (strlen(ssid) > 0) {
    WiFi.begin(ssid, password);
    Serial.printf("Kết nối WiFi: %s\n", ssid);
  }

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    InputWiFiInfo();
    unsigned long retryStart = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - retryStart < 10000) {
      delay(500);
      Serial.print(".");
    }
  }

  if (WiFi.status() == WL_CONNECTED)
    Serial.println("\nWiFi connected!");
  else
    Serial.println("\nKhông thể kết nối WiFi!");

  
  topic_ota += GetDeviceId();

  client.setServer(mqtt_server, 1883);
  client.setCallback(MQTT_Callback);
  client.setKeepAlive(60);  // Keep-alive 60s (giá trị ổn định)
  client.setBufferSize(64);  // Tăng buffer size để nhận message tốt hơn
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
  }

  if (update_firmware) {

    delay(3000); // Đợi 3 giây trước khi bắt đầu OTA

    // Xóa retained message bằng cách publish empty message
    client.publish(topic_ota.c_str(), "", true);
    Serial.println("Đã xóa retained message\n");
    
    Serial.println("Bắt đầu cập nhật firmware...");
    OtaUpdate();
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
  if (WiFi.status() == WL_CONNECTED && !update_firmware) 
  {
    http.begin(client_secure, this->scriptURL);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(payload);

    Serial.print("HTTP Response: ");
    Serial.println(httpResponseCode);
    http.end();
  }
}

bool Server_c::CheckUpdate() 
{
  return update_firmware;
}
