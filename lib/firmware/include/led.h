#ifndef LED_H
#define LED_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class LedControl {
private:
    uint8_t _pin;

    void write(bool on);   // LOW = ON, HIGH = OFF (LED dương chung)

public:
    LedControl(uint8_t pin);

    void begin();

    // Điều khiển cơ bản

    // Điều khiển theo thời gian (blocking)
    void onDelay(uint32_t ms);
    void offDelay(uint32_t ms);

    // Hàm blink CŨ (GIỮ NGUYÊN)
    void blink(unsigned long onMs, unsigned long offMs);

    // Trạng thái WiFi
    void wifiConnected();      // OFF 5s → ON 100ms
    void wifiConnecting();     // blink 500ms
    void wifiDisconnected();   // blink 100ms
};

#endif
