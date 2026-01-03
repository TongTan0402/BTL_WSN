#ifndef __SERVER__
#define __SERVER__
#include <Arduino.h>
#include <stdint.h>

typedef enum 
{
    WIFI_DISCONNECTED = 0,
    WIFI_CONNECTED = 1,
    UPDATE_AVAILABLE = 2
}Server_State_e;

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

    Server_State_e GetState();
    void OtaUpdate();
};

extern Server_c Server;

#endif