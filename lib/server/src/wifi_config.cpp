#include "wifi_config.h"
#include "device_id.h"
#include "wifi_power_config.h"

Preferences prefs;
WebServer server(80);

String ssid = "";
String pass = "";

void HandleRoot() {
  server.send(200, "text/html", htmlPage);
}

void Set_Wifi_Infor_To_Flash(String ssid, String pass) {
  prefs.begin("wifi", false);
  prefs.putString("ssid", ssid);
  prefs.putString("pass", pass);
  prefs.end();
}

void Get_Wifi_Infor_From_Flash(String& ssid, String& pass) {
  prefs.begin("wifi", true);
  ssid = prefs.getString("ssid", "");
  pass = prefs.getString("pass", "");
  prefs.end();
}

void HandleSave() {
  ssid = server.arg("ssid");
  pass = server.arg("pass");
  Serial.printf("Lưu SSID: %s, Pass: %s\n", ssid.c_str(), pass.c_str());

  Set_Wifi_Infor_To_Flash(ssid, pass);
  
  server.send(200, "text/html", successPage);
  Serial.print("Đã nhận thông tin.\nĐang khởi động lại");
  for(int i=0; i<3; i++) 
  {
    Serial.print(".");
    delay(1000);
  }
  ESP.restart();
  while(true) { delay(1000); }  // Đợi ESP restart  
}

void StartAPMode() {
  // Cấu hình IP tùy chỉnh cho Access Point
  IPAddress local_IP(192, 168, 4, 2);      // IP của ESP32 AP
  IPAddress gateway(192, 168, 4, 2);       // Gateway
  IPAddress subnet(255, 255, 255, 0);      // Subnet mask
  
  // Thiết lập IP tùy chỉnh
  WiFi.softAPConfig(local_IP, gateway, subnet);
  
  // Khởi tạo Access Point
  String ap_ssid = "ESP32_" + GetDeviceId();
  
  // Giảm công suất phát sóng để tiết kiệm năng lượng (0-20.5 dBm)
  // 8.5 dBm: phù hợp cho khoảng cách gần, tiết kiệm năng lượng tốt
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  
  WiFi.softAP(ap_ssid.c_str(), "12345678");

  // Thiết lập mDNS - Truy cập bằng tên miền thay vì IP
  const char* mdns_name = "wificonfig"; // Tên mDNS tùy chỉnh

  if (MDNS.begin(mdns_name)) {
    Serial.println("mDNS đã khởi động!");
    Serial.println("Truy cập qua: http://" + String(mdns_name) + ".local");
    MDNS.addService("http", "tcp", 80);
  } else {
    Serial.println("Lỗi khởi động mDNS");
  }

  server.on("/", HandleRoot);
  server.on("/save", HandleSave);
  server.begin();
}

void ConnectToWiFi() {
  Get_Wifi_Infor_From_Flash(ssid, pass);

  if (ssid != "") {
    Serial.printf("Kết nối WiFi: %s\n", ssid.c_str());
    Serial.printf("Pass: %s\n", pass.c_str());
    WiFi.begin(ssid.c_str(), pass.c_str());
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 30000) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
  }

  if (WiFi.status() == WL_CONNECTED) 
  {
    Serial.println("Kết nối WiFi thành công!");
    Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
    
    // Tắt AP mode để tiết kiệm năng lượng
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA); // Chuyển sang chế độ Station only
    
    Serial.println("AP mode đã tắt - Tiết kiệm năng lượng");
  } 

  if(WiFi.status() != WL_CONNECTED) 
  {
    Serial.println("Không kết nối được WiFi -> Bật AP config mode");
    
    // Chuyển sang chế độ AP only để tiết kiệm năng lượng
    WiFi.mode(WIFI_AP);
    
    StartAPMode();
    
    // Vòng lặp xử lý web server trong AP mode
    Serial.println("AP mode đang chạy - Chờ thiết bị kết nối...");
    
    uint32_t apStartTime = millis();
    const uint32_t apTimeout = 300000; // 5 phút
    while(millis() - apStartTime < apTimeout) 
    {
      int clientCount = WiFi.softAPgetStationNum();
      
      if (clientCount > 0) 
      {
        // Chỉ xử lý web server khi có client
        server.handleClient();
        delay(10);
      } 
      else 
      {
        // Không có client -> delay lâu hơn để tiết kiệm năng lượng
        delay(1000);
      }
    }

    if(millis() - apStartTime >= apTimeout) 
    {
      Serial.println("AP mode hết thời gian chờ. Khởi động lại...");
      ESP.restart();
      while(true) { delay(1000); }  // Đợi ESP restart
    }
  }
}
