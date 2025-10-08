#ifndef DHT11_H
#define DHT11_H

#include <Arduino.h>

class DHT11 {
private:
    int _pin;               //Chân ADC DHT11
    float _temperature;     // Lưu nhiệt độ đọc được
    float _humidity;        // Lưu độ ẩm đọc được
    bool _isValid;          // Cờ cho biết lần đọc gần nhất có hợp lệ (checksum ok)

    bool _readRawData(uint8_t data[5]);     // Hàm nội bộ đọc 5 byte thô

public:
    DHT11(int pin);             //khai báo chân
    bool read();                // Gọi để đọc nhiệt độ, độ ẩm
    float getTemperature();     // Trả nhiệt độ đã đọc
    float getHumidity();        // Trả độ ẩm đã đọc
    bool isValid();             // Kiểm tra lần đọc có hợp lệ không
};

#endif
