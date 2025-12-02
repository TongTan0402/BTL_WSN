#include "MP25.h"

// khởi tạo chân và điện áp
MP25::MP25(int ledPin, int adcPin, float Vcc) {
    _ledPin = ledPin;
    _adcPin = adcPin;
    _Vcc = Vcc;
    _offset = 0.0f;
    _samples = 10;
    _Voc = 0.0f;
}

// cấu hình pinmode và số mẫu trung bình
void MP25::begin(int samples, float offset) {
    pinMode(_ledPin, OUTPUT);
    pinMode(_adcPin, INPUT);
    _samples = samples;
    _offset = offset;
    _Voc = 0.9f;  // mặc định ban đầu
}

// đọc ADC trung bình (_samples) theo chu kỳ LED chuẩn
float MP25::_readRawADC() {
    long sum = 0;
    for (int i = 0; i < _samples; i++) {
        // BẬT LED
        digitalWrite(_ledPin, LOW);
        delayMicroseconds(280);

        // ĐỌC ADC
        int adc = analogRead(_adcPin);

        delayMicroseconds(40);

        // TẮT LED
        digitalWrite(_ledPin, HIGH);
        delayMicroseconds(9680);   // tổng chu kỳ ~10ms
        sum += adc;
    }
    return (float)sum / _samples;
}

// đọc và tính nồng độ bụi, trừ Voc
float MP25::readDustDensity() {
    float adc_avg = _readRawADC();
    float Vo = (adc_avg / 4095.0f) * _Vcc;

    float dustDensity = (Vo - _Voc) * 200.0f + _offset; // µg/m³
    // if (dustDensity < 0) dustDensity = 0;
    return dustDensity;
}

// hiệu chỉnh offset thủ công
void MP25::calibrate(float offset) {
    _offset = offset;
}

// set Voc thủ công
void MP25::setVoc(float voc) {
    _Voc = voc;
}

// tự động đo Voc từ môi trường sạch
void MP25::autoCalibrate(int sampleCount, int delayMs) {
    long sum = 0;
    for (int i = 0; i < sampleCount; i++) {
        sum += _readRawADC();
        delay(delayMs);
    }
    // tính Voc trung bình
    float adc_avg = (float)sum / sampleCount;
    _Voc = (adc_avg / 4095.0f) * _Vcc;
}
