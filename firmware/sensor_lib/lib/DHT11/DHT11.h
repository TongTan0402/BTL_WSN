#ifndef DHT11_H
#define DHT11_H

#include <Arduino.h>
class DHT11 {
private:
    int _pin;
    bool _readRawData(uint8_t data[5]);
public:
    struct Data {
        float temperature;  // Nhiệt độ (°C)
        float humidity;     // Độ ẩm (%)
        bool valid;         // Cờ kiểm tra dữ liệu hợp lệ
    };

    DHT11(int pin);
    Data read();  // Đọc cảm biến, trả về cả 3 giá trị

};

#endif
