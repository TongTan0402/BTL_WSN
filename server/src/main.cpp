#include <Arduino.h>
#include "../lib/payload_format.h"
#include "../lib/server.h"

void setup() {
  Serial.begin(115200);
  Server.begin("Your_SSID", "Your_PASSWORD", "Your_WEBAPP_URL");
}

void loop() {
  float temperature = 27.3; 
  float humidity = 55.1;    
  float fine_dust = 12.4;   

  String payload = PayloadFormat(temperature, humidity, fine_dust);
  Serial.println("Payload: " + payload);
  
  Server.SendData(payload);
  
  delay(1000);
}
