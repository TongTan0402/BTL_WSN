#ifndef MP25_H
#define MP25_H

#include <Arduino.h>

class MP25 {
private:
    int _ledPin;    // chân điều khiển led
    int _adcPin;    // chân ADC đọc Vo
    float _Vcc;     // điện áp cảm biến
    float _offset;  // offset hiệu chuẩn
    int _samples;   // số mẫu lấy trung bình

    float _readRawADC();  // đọc giá trị ADC trung bình

public:
    MP25(int ledPin, int adcPin, float Vcc = 5.0);
    void begin(int samples = 10, float offset = 0.0);
    float readDustDensity();  // µg/m³
    void calibrate(float offset);
};

#endif
