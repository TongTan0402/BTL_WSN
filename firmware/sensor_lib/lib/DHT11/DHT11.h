#ifndef DHT11_H
#define DHT11_H

#include <Arduino.h>

class DHT11 {
private:
    int _pin;
    float _temperature;
    float _humidity;
    bool _isValid;

    bool _readRawData(uint8_t data[5]);

public:
    DHT11(int pin);
    bool read(float &temperature, float &humidity); 
    bool isValid();
};

#endif
