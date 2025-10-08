#ifndef DHT11_H
#define DHT11_H

#include <Arduino.h>
class DHT11 {
public:
    struct Data {
        float temperature;  // Nhiệt độ (°C)
        float humidity;     // Độ ẩm (%)
    };

private:
    int _pin;
    bool _readRawData(uint8_t data[5]);
    Data result = {0, 0};
    
public:
    DHT11(int pin);
    Data read();  // Đọc cảm biến, trả về cả 3 giá trị

};

#endif
