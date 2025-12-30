#ifndef LED_H
#define LED_H

#include <Arduino.h>

class LedControl {
private:
    uint8_t _pin;
    bool _state;

    unsigned long _lastMillis;
    unsigned long _onTime;
    unsigned long _offTime;

    bool _blinkEnable;


    void apply();               // áp mức điện áp ra chân GPIO

public:
    LedControl(uint8_t pin);

    void begin();

    // Điều khiển cơ bản
    void on();
    void off();
    void toggle();

    // Nhấp nháy không blocking
    void blink(unsigned long onMs, unsigned long offMs);
    void stopBlink();

    // Gọi trong loop()
    void update();
    void reset();    
    bool getState();
};

#endif
