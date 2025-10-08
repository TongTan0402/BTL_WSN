#include "MP25.h"

// cấu hình chân pinled và adc, điện áp cấp
MP25::MP25(int ledPin, int adcPin, float Vcc) {
    _ledPin = ledPin;
    _adcPin = adcPin;
    _Vcc = Vcc;
}

// thiết lập pinmode và sample
void MP25::begin(int samples, float offset) {
    pinMode(_ledPin, OUTPUT);
    _samples = samples;
    _offset = offset;
}

// Đọc trung bình nhiều mẫu ADC để giảm nhiễu
float MP25::_readRawADC() {
    long sum = 0;
    for (int i = 0; i < _samples; i++) {
        digitalWrite(_ledPin, LOW);
        delayMicroseconds(280);
        int adc = analogRead(_adcPin);
        delayMicroseconds(40);
        digitalWrite(_ledPin, HIGH);
        delayMicroseconds(9680);
        sum += adc;
    }
    return (float)sum / _samples;
}

// chuyển từ ADC sang nồng độ bụi
float MP25::readDustDensity() {
    float adc_avg = _readRawADC();
    float Vo = (adc_avg / 4095.0) * 3.3;
    float dustDensity = (Vo - 0.9) * 1000 / 0.5 + _offset; // µg/m³
    if (dustDensity < 0) dustDensity = 0;
    return dustDensity;
}

// Cho phép hiệu chỉnh offset theo điều kiện môi trường
void MP25::calibrate(float offset) {
    _offset = offset;
}
