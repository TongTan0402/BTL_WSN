#include "DHT11.h"

// Khởi tạo các giá trị ban đầu
DHT11::DHT11(int pin) {
    _pin = pin;
    pinMode(_pin, INPUT);
    _temperature = 0;
    _humidity = 0;
    _isValid = false;
}

bool DHT11::_readRawData(uint8_t data[5]) {
    uint8_t cnt = 7, idx = 0;
    for (int i = 0; i < 5; i++) data[i] = 0;

    // Bắt đầu tín hiệu
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delay(18);
    digitalWrite(_pin, HIGH);
    delayMicroseconds(40);
    pinMode(_pin, INPUT);

    // Chờ phản hồi
    unsigned long start = micros();
    while (digitalRead(_pin) == HIGH)
        if (micros() - start > 80) return false;

    // Bỏ qua tín hiệu phản hồi đầu
    start = micros();
    while (digitalRead(_pin) == LOW)
        if (micros() - start > 80) return false;
    start = micros();
    while (digitalRead(_pin) == HIGH)
        if (micros() - start > 80) return false;

    // Đọc 40 bit dữ liệu
    for (int i = 0; i < 40; i++) {
        start = micros();
        while (digitalRead(_pin) == LOW)
            if (micros() - start > 80) return false;

        unsigned long t = micros();
        while (digitalRead(_pin) == HIGH)
            if (micros() - t > 80) return false;

        if ((micros() - t) > 40)
            data[idx] |= (1 << cnt);

        if (cnt == 0) {
            cnt = 7;
            idx++;
        } else cnt--;
    }

    // Kiểm tra checksum
    return (uint8_t)(data[0] + data[1] + data[2] + data[3]) == data[4];
}

bool DHT11::read() {
    uint8_t data[5];
    if (!_readRawData(data)) {
        _isValid = false;
        return false;
    }

    _humidity = data[0];        // phần nguyên độ ẩm
    _temperature = data[2];     // phần nguyên nhiệt độ
    _isValid = true;
    return true;
}

float DHT11::getTemperature() {
    return _temperature;
}

float DHT11::getHumidity() {
    return _humidity;
}

bool DHT11::isValid() {
    return _isValid;
}
