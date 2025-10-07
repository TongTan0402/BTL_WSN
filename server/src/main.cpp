#include <Arduino.h>
#include <server.h>
#include <payload_format.h>
#include <stdlib.h>

const char *ssid = "FPT Telecom-3A93 2.4G";
const char *password = "20c01d9a";
const char *scriptURL = "https://script.google.com/macros/s/AKfycbwhTF9oBVzyj8QJrQak8oJ33r3Y5sb9E3t3pU96MB9cQt6CT6TCBKiZiFTQa9Oolbc/exec";

void setup() {
  Serial.begin(115200);
  Server.begin(ssid, password, scriptURL);
}

void loop() {
  float temperature = random(200, 400) / 10.0; 
  float humidity = random(400, 600) / 10.0;    
  float pm25 = random(0, 1000) / 10.0;      
  float gas = random(0, 1000) / 10.0;   

  String payload = PayloadFormat(temperature, humidity, pm25, gas);

  Server.SendData(payload);
  
  Serial.println("Payload: " + payload);
}
