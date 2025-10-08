# Hướng dẫn sử dụng các thư viện đọc cảm biến DHT11, MQ2, MP25

## 1. Cảm biến MQ2 (Gas Sensor)
### Mô tả
- Cảm biến MQ2 đo nồng độ khí dễ cháy như LPG, CH₄, CO, khói...  
- Thư viện đã bao gồm các hàm **hiệu chuẩn (Ro)**, **đọc Rs**, và **tính nồng độ ppm** theo công thức logarit từ datasheet.
### Các hàm chính
- **MQ2(int pin, float RL, float Vcc, float divider):**     tạo đối tượng cảm biến.
- **void begin(float Ro):**                                 Gán giá trị Ro đã hiệu chuẩn.
- **float calibrateRo(int samples=50, int delayMs=200):**   Hiệu chuẩn Ro trong không khí sạch.
- **float readRs():**                                       Đọc điện trở Rs tức thời. 
- **float getRatio():**                                     Trả về tỉ lệ Rs/Ro.
- **float readPPM():**                                      Tính nồng độ khí (ppm).
- void setGasCurve(float slope, float intercept)       Tùy chỉnh đường cong cho loại khí khác.

## 2. Cảm biến DHT11 (Temperature & Humidity)
### Mô tả
- Đọc nhiệt độ và độ ẩm môi trường qua giao tiếp 1-Wire.
- Thư viện này đã bao gồm xử lý lỗi checksum và timeout.
### Các hàm chính
- **DHT11(int pin):** Tạo đối tượng cảm biến
- **bool read(float &temp, float &hum):** Đọc dữ liệu nhiệt độ, độ ẩm
- **bool readRawData():** Hàm nội bộ đọc xung 1-Wire từ cảm biến

## 3. Cảm biến MP25 (Bụi mịn GP2Y1010AU0F)
### Mô tả
- Cảm biến bụi GP2Y1010 đo mật độ bụi (µg/m³) bằng nguyên lý quang học phản xạ.
- Thư viện hỗ trợ lấy mẫu ADC và chuyển đổi giá trị điện áp sang mật độ bụi.
  
### Các hàm chính
- **MP25(int ledPin, int adcPin, float Vcc):** Khởi tạo cảm biến.
- **float readDustDensity():** Đọc mật độ bụi µg/m³ (đã hiệu chỉnh theo datasheet).

## 4. Ví dụ tổng hợp(main.cpp) 

