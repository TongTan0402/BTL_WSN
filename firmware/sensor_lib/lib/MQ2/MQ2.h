#ifndef MQ2_H
#define MQ2_H

#include <Arduino.h>

class MQ2 {
private:
    int _pin;         // Chân ADC kết nối cảm biến
    float _RL;        // Điện trở tải RL (10K Ohm)
    float _Vcc;       // Điện áp cấp cho cảm biến (5V)
    float _Ro;        // Điện trở tham chiếu (hiệu chuẩn)
    float _slope;     // Hệ số m (đường cong log-log)
    float _intercept; // Hệ số b (đường cong log-log)
    float _divider;   // Hệ số chia áp (nếu dùng mạch giảm 5V -> 3.3V)

    float readADC_Avg(int samples = 10);    // đọc trung bình nhiều mẫu
    float readRsOnce();                     // đọc Rs tức thời

public:
    MQ2(int pin, float RL = 10000.0, float Vcc = 5.0, float divider = 1.0);

    void begin(float Ro_calib);
    void setGasCurve(float slope, float intercept);
    float calibrateRo(int samples = 50, int delayMs = 200); // hiệu chuẩn Ro trong không khí sạch
    float readRs(int samples = 10);
    float getRatio();
    float readPPM();        // trả về nồng độ không khí tính theo ppm.
};

#endif
