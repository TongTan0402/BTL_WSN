#include "led.h"

LedControl::LedControl(uint8_t pin)
{
    _pin = pin;
}

void LedControl::begin()
{
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, HIGH);   // OFF (LED dương chung)
}

void LedControl::write(bool on)
{
    digitalWrite(_pin, on ? LOW : HIGH);
}


void LedControl::onDelay(uint32_t ms)
{
    write(true);
    vTaskDelay(pdMS_TO_TICKS(ms));
}

void LedControl::offDelay(uint32_t ms)
{
    write(false);
    vTaskDelay(pdMS_TO_TICKS(ms));
}

/* ===== BLINK CŨ – GIỮ NGUYÊN ===== */

void LedControl::blink(unsigned long onMs, unsigned long offMs)
{
    digitalWrite(_pin, HIGH);
    vTaskDelay(pdMS_TO_TICKS(onMs));
    digitalWrite(_pin, LOW);
    vTaskDelay(pdMS_TO_TICKS(offMs));
}


void LedControl::wifiConnected()
{

    blink(3000, 100);
}

void LedControl::wifiConnecting()
{

    blink(500, 500);
}

void LedControl::wifiDisconnected()
{
    blink(80, 100);
}
