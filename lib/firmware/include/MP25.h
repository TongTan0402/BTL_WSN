#ifndef MP25_H
#define MP25_H

#include <Arduino.h>

class MP25 {
private:
    int _ledPin;       // chân điều khiển LED
    int _adcPin;       // chân ADC đọc Vo
    float _Vcc;        // điện áp cấp cảm biến
    float _offset;     // offset hiệu chuẩn (tùy người dùng)
    int _samples;      // số mẫu trung bình khi đọc
    float _Voc;        // điện áp khi không khí sạch (zero-dust voltage)

    float _readRawADC();          // đọc ADC trung bình (_samples)

public:
    MP25(int ledPin, int adcPin, float Vcc = 5.0);

    void begin(int samples = 10, float offset = 0.0);

    // tự động hiệu chỉnh Voc từ môi trường sạch
    void autoCalibrate(int sampleCount = 100, int delayMs = 10);

    float readDustDensity();      // trả về µg/m³

    void calibrate(float offset); // set offset thủ công
    void setVoc(float voc);       // set Voc thủ công
    float getVoc();
    float getVo();
    int getRaw();
};

#endif
