#ifndef __SERVER__
#define __SERVER__
#include <Arduino.h>
#include <stdint.h>

class Server_c
{
  private:
    const char *scriptURL;

  public:
    void begin(const char *ssid, const char *password, const char *scriptURL);
    void sendData(String payload);

};

extern Server_c Server;

#endif