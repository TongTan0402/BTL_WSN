#include "MQ2.h"

// Constructor
MQ2::MQ2(int pin, float RL, float Vcc, float divider) {
    _pin = pin;
    _RL = RL;
    _Vcc = Vcc;
    _divider = divider; // nếu không dùng chia áp: = 1.0
    _slope = -1.44;     // mặc định cho LPG
    _intercept = 2.30;
}

// Gán giá trị Ro đã hiệu chuẩn
void MQ2::begin(float Ro_calib) {
    _Ro = Ro_calib;
}

// Đặt đường cong cho loại khí khác (nếu cần)
void MQ2::setGasCurve(float slope, float intercept) {
    _slope = slope;
    _intercept = intercept;
}

// Đọc trung bình nhiều mẫu ADC để giảm nhiễu
float MQ2::readADC_Avg(int samples) {
    long sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += analogRead(_pin);
        delay(10);
    }
    return (float)sum / samples;
}

// Đọc Rs tại 1 thời điểm
float MQ2::readRsOnce() {
    int adc = analogRead(_pin);
    float Vadc = (adc / 4095.0) * 3.3;
    float Vout = Vadc * _divider; // nếu dùng chia áp thì khôi phục Vout thực

    if (Vout <= 0.0001) return 1e6; // tránh chia 0
    float Rs = _RL * (_Vcc - Vout) / Vout;
    return Rs;
}

// Đọc Rs với trung bình nhiều mẫu
float MQ2::readRs(int samples) {
    float Vadc_avg = readADC_Avg(samples);
    float Vout = (Vadc_avg / 4095.0) * 3.3 * _divider;
    if (Vout <= 0.0001) return 1e6;
    float Rs = _RL * (_Vcc - Vout) / Vout;
    return Rs;
}

// Hiệu chuẩn Ro trong không khí sạch
float MQ2::calibrateRo(int samples, int delayMs) {
    float Rs_sum = 0;
    for (int i = 0; i < samples; i++) {
        Rs_sum += readRs();
        delay(delayMs);
    }
    float Rs_avg = Rs_sum / samples;
    // Theo datasheet, Rs/Ro ≈ 9.8 trong không khí sạch cho LPG
    _Ro = Rs_avg / 9.8;
    return _Ro;
}

// Tỉ lệ Rs/Ro
float MQ2::getRatio() {
    if (_Ro <= 0.001) return 0;
    float Rs = readRs();
    return Rs / _Ro;
}

// Tính nồng độ khí (ppm)
float MQ2::readPPM() {
    float ratio = getRatio();
    if (ratio <= 0) return 0;
    float ppm = pow(10, (_slope * log10(ratio)) + _intercept);
    return ppm;
}
