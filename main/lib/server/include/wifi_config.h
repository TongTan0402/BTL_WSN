#ifndef __WIFI_CONFIG__
#define __WIFI_CONFIG__
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include "wifi_config_page.h"

void Set_Wifi_Infor_To_Flash(String ssid, String pass);
void Get_Wifi_Infor_From_Flash(String& ssid, String& pass);
void ConnectToWiFi();

#endif