#ifndef __SERVER__
#define __SERVER__
#include <Arduino.h>
#include <stdint.h>

class Server_c
{
  private:
    const char* scriptURL;

  public:
    void begin(const char* scriptURL);
    void sendData(String payload);
    void MQTTLoop();
    bool CheckUpdate();

    void SaveWifi(char *new_ssid, char *new_password);

    bool IsWiFiConnected();
    void OtaUpdate();
};

extern Server_c Server;

#endif