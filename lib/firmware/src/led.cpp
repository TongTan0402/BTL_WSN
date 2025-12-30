#include "led.h"

LedControl::LedControl(uint8_t pin)
{
    _pin = pin;
    _state = false;
    _lastMillis = 0;
    _onTime = 0;
    _offTime = 0;
    _blinkEnable = false;
}


void LedControl::begin()
{
    pinMode(_pin, OUTPUT);

    // LED dương chung: HIGH = OFF
    digitalWrite(_pin, HIGH);
    _state = false;
}

void LedControl::apply()
{
    // LED dương chung → đảo logic
    digitalWrite(_pin, _state ? LOW : HIGH);
}

void LedControl::on()
{
    _state = true;
    _blinkEnable = false;
    apply();
}

void LedControl::off()
{
    _state = false;
    _blinkEnable = false;
    apply();
}

void LedControl::toggle()
{
    _state = !_state;
    apply();
}



void LedControl::blink(unsigned long onMs, unsigned long offMs)
{
   digitalWrite(_pin, LOW);
   vTaskDelay(onMs);
   digitalWrite(_pin, HIGH);
   vTaskDelay(offMs);
   //  _onTime = onMs;
   //  _offTime = offMs;
   //  _lastMillis = millis();
   //  _blinkEnable = true;

   //  _state = true;   // bắt đầu bằng ON
   //  apply();
}


void LedControl::stopBlink()
{
    _blinkEnable = false;
}

void LedControl::update()
{
    if (!_blinkEnable) return;

    unsigned long now = millis();
    unsigned long period = _state ? _onTime : _offTime;

    if (now - _lastMillis >= period)
    {
        _lastMillis = now;
        toggle();
    }
}


bool LedControl::getState()
{
    return _state;
}

void LedControl::reset()
{
    _blinkEnable = false;
    _state = false;          // OFF
    _onTime = 0;
    _offTime = 0;
    _lastMillis = millis();  // reset mốc thời gian

    apply();                // LED OFF (HIGH vì dương chung)
}

