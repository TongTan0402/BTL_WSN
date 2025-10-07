#include "server.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

Server_c Server;


HTTPClient http;
WiFiClientSecure client;

void Server_c::begin(const char *ssid, const char *password, const char *scriptURL)
{
  this->scriptURL = scriptURL;

  WiFi.begin(ssid, password);
  Serial.print("Wifi connecting");

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
}

void Server_c::SendData(String payload)
{
  if (WiFi.status() == WL_CONNECTED) 
  {
    client.setInsecure(); 
    http.begin(client, this->scriptURL);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(payload);

    Serial.print("HTTP Response: ");
    Serial.println(httpResponseCode);
    http.end();
  }
}