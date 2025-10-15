#ifndef WIFI_POWER_CONFIG_H
#define WIFI_POWER_CONFIG_H

#include <WiFi.h>

// ============================================================================
// WiFi Power Management Configuration
// ============================================================================

// Các mức công suất phát sóng WiFi (dBm)
// Công suất càng thấp càng tiết kiệm pin, nhưng khoảng cách phát sóng giảm
typedef enum {
    POWER_ULTRA_LOW  = WIFI_POWER_MINUS_1dBm,  // -1 dBm:  Cực kỳ tiết kiệm, khoảng cách rất ngắn (~1m)
    POWER_VERY_LOW   = WIFI_POWER_2dBm,        //  2 dBm:  Rất tiết kiệm, khoảng cách ngắn (~2-3m)
    POWER_LOW        = WIFI_POWER_5dBm,        //  5 dBm:  Tiết kiệm, khoảng cách trung bình (~5m)
    POWER_MEDIUM_LOW = WIFI_POWER_8_5dBm,      //  8.5dBm: Cân bằng tốt (~10m) - Khuyên dùng cho AP mode
    POWER_MEDIUM     = WIFI_POWER_11dBm,       // 11 dBm:  Trung bình (~15m)
    POWER_MEDIUM_HIGH= WIFI_POWER_13dBm,       // 13 dBm:  Khá cao (~20m)
    POWER_HIGH       = WIFI_POWER_15dBm,       // 15 dBm:  Cao (~25m)
    POWER_VERY_HIGH  = WIFI_POWER_17dBm,       // 17 dBm:  Rất cao (~30m)
    POWER_ULTRA_HIGH = WIFI_POWER_19dBm,       // 19 dBm:  Cực cao (~40m)
    POWER_MAX        = WIFI_POWER_19_5dBm      // 19.5dBm: Tối đa (~50m) - Tiêu thụ năng lượng cao nhất
} wifi_power_level_t;

// ============================================================================
// Cấu hình khuyến nghị theo use case
// ============================================================================

// AP Mode (cấu hình WiFi) - Khoảng cách gần, ưu tiên tiết kiệm năng lượng
#define WIFI_POWER_AP_MODE      POWER_MEDIUM_LOW    // 8.5 dBm

// Station Mode (kết nối router) - Cân bằng giữa tầm phát và tiết kiệm pin
#define WIFI_POWER_STATION_MODE POWER_MEDIUM        // 11 dBm

// Battery Mode (chạy pin) - Tối ưu tiết kiệm năng lượng
#define WIFI_POWER_BATTERY_MODE POWER_LOW           // 5 dBm

// Performance Mode (nguồn điện ổn định) - Ưu tiên tín hiệu ổn định
#define WIFI_POWER_PERFORMANCE  POWER_HIGH          // 15 dBm

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * @brief Thiết lập công suất WiFi và in thông tin
 * @param power Mức công suất muốn thiết lập
 * @param mode Tên chế độ (để hiển thị log)
 */
inline void setWifiPowerWithLog(wifi_power_t power, const char* mode) {
    WiFi.setTxPower(power);
    
    const char* powerStr;
    const char* rangeStr;
    
    switch(power) {
        case WIFI_POWER_MINUS_1dBm: powerStr = "-1 dBm";   rangeStr = "~1m";    break;
        case WIFI_POWER_2dBm:       powerStr = "2 dBm";    rangeStr = "~2-3m";  break;
        case WIFI_POWER_5dBm:       powerStr = "5 dBm";    rangeStr = "~5m";    break;
        case WIFI_POWER_8_5dBm:     powerStr = "8.5 dBm";  rangeStr = "~10m";   break;
        case WIFI_POWER_11dBm:      powerStr = "11 dBm";   rangeStr = "~15m";   break;
        case WIFI_POWER_13dBm:      powerStr = "13 dBm";   rangeStr = "~20m";   break;
        case WIFI_POWER_15dBm:      powerStr = "15 dBm";   rangeStr = "~25m";   break;
        case WIFI_POWER_17dBm:      powerStr = "17 dBm";   rangeStr = "~30m";   break;
        case WIFI_POWER_19dBm:      powerStr = "19 dBm";   rangeStr = "~40m";   break;
        case WIFI_POWER_19_5dBm:    powerStr = "19.5 dBm"; rangeStr = "~50m";   break;
        default:                    powerStr = "Unknown";  rangeStr = "N/A";    break;
    }
    
    Serial.printf("💡 WiFi Power [%s]: %s (Range: %s)\n", mode, powerStr, rangeStr);
}

/**
 * @brief Ước tính mức tiêu thụ năng lượng theo công suất WiFi
 * @param power Mức công suất hiện tại
 * @return Mức tiêu thụ tương đối (%)
 */
inline int estimatePowerConsumption(wifi_power_t power) {
    switch(power) {
        case WIFI_POWER_MINUS_1dBm: return 30;   // 30% công suất tối đa
        case WIFI_POWER_2dBm:       return 35;
        case WIFI_POWER_5dBm:       return 45;
        case WIFI_POWER_8_5dBm:     return 55;
        case WIFI_POWER_11dBm:      return 65;
        case WIFI_POWER_13dBm:      return 75;
        case WIFI_POWER_15dBm:      return 85;
        case WIFI_POWER_17dBm:      return 90;
        case WIFI_POWER_19dBm:      return 95;
        case WIFI_POWER_19_5dBm:    return 100;  // 100% công suất tối đa
        default:                    return 0;
    }
}

// ============================================================================
// Modem Sleep Mode - Tiết kiệm năng lượng tối đa khi WiFi idle
// ============================================================================

/**
 * @brief Bật chế độ modem sleep để tiết kiệm năng lượng
 * ESP32 sẽ tắt WiFi radio khi không có data transfer
 * Tiết kiệm ~20-40% năng lượng trong chế độ idle
 */
inline void enableWifiModemSleep() {
    WiFi.setSleep(true);  // Bật WIFI_PS_MIN_MODEM
    Serial.println("💤 WiFi Modem Sleep: ENABLED (Power saving ~20-40%)");
}

/**
 * @brief Tắt chế độ modem sleep để performance tốt hơn
 */
inline void disableWifiModemSleep() {
    WiFi.setSleep(false);
    Serial.println("⚡ WiFi Modem Sleep: DISABLED (Performance mode)");
}

#endif // WIFI_POWER_CONFIG_H
