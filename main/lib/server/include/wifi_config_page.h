#ifndef WIFI_CONFIG_PAGE_H
#define WIFI_CONFIG_PAGE_H

#include <pgmspace.h>

// Trang web cấu hình WiFi với giao diện đẹp - Lưu trong PROGMEM
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Cấu hình WiFi - ESP32</title>
<style>
* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    min-height: 100vh;
    display: flex;
    justify-content: center;
    align-items: center;
    padding: 20px;
}

.container {
    background: white;
    border-radius: 20px;
    box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
    padding: 40px;
    max-width: 450px;
    width: 100%;
    animation: slideIn 0.5s ease-out;
}

@keyframes slideIn {
    from {
        opacity: 0;
        transform: translateY(-30px);
    }
    to {
        opacity: 1;
        transform: translateY(0);
    }
}

.header {
    text-align: center;
    margin-bottom: 30px;
}

.wifi-icon {
    font-size: 60px;
    margin-bottom: 10px;
    color: #667eea;
}

h2 {
    color: #333;
    font-size: 28px;
    font-weight: 600;
    margin-bottom: 10px;
}

.subtitle {
    color: #666;
    font-size: 14px;
}

.form-group {
    margin-bottom: 25px;
}

label {
    display: block;
    color: #555;
    font-weight: 500;
    margin-bottom: 8px;
    font-size: 14px;
}

input[type="text"],
input[type="password"] {
    width: 100%;
    padding: 14px 16px 14px 45px;
    border: 2px solid #e0e0e0;
    border-radius: 10px;
    font-size: 15px;
    transition: all 0.3s ease;
    background-color: #f8f9fa;
}

input[type="text"]:focus,
input[type="password"]:focus {
    outline: none;
    border-color: #667eea;
    background-color: white;
    box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
}

.input-icon {
    position: relative;
}

.input-icon::before {
    position: absolute;
    left: 16px;
    top: 50%;
    transform: translateY(-50%);
    font-size: 18px;
}

.wifi-input::before {
    content: '📶';
}

.pass-input::before {
    content: '🔒';
}

.toggle-password {
    position: absolute;
    right: 16px;
    top: 50%;
    transform: translateY(-50%);
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    color: white;
    border: none;
    border-radius: 6px;
    cursor: pointer;
    font-size: 12px;
    font-weight: 600;
    padding: 6px 12px;
    transition: all 0.3s ease;
}

.toggle-password:hover {
    transform: translateY(-50%) scale(1.05);
    box-shadow: 0 2px 8px rgba(102, 126, 234, 0.4);
}

.submit-btn {
    width: 100%;
    padding: 16px;
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    color: white;
    border: none;
    border-radius: 10px;
    font-size: 16px;
    font-weight: 600;
    cursor: pointer;
    transition: all 0.3s ease;
    box-shadow: 0 4px 15px rgba(102, 126, 234, 0.4);
}

.submit-btn:hover {
    transform: translateY(-2px);
    box-shadow: 0 6px 20px rgba(102, 126, 234, 0.6);
}

.submit-btn:active {
    transform: translateY(0);
}

.info-box {
    background: #f0f4ff;
    border-left: 4px solid #667eea;
    padding: 12px 16px;
    border-radius: 8px;
    margin-top: 20px;
    font-size: 13px;
    color: #555;
}

.info-box strong {
    color: #667eea;
}

@media (max-width: 480px) {
    .container {
        padding: 30px 20px;
    }
    h2 {
        font-size: 24px;
    }
}
</style>
</head>
<body>
<div class="container">
    <div class="header">
        <div class="wifi-icon">📡</div>
        <h2>Kết nối WiFi</h2>
        <p class="subtitle">Cấu hình kết nối cho ESP32</p>
    </div>
    <form action="/save" method="post">
        <div class="form-group">
            <label for="ssid">Tên mạng WiFi (SSID)</label>
            <div class="input-icon wifi-input">
                <input type="text" id="ssid" name="ssid" placeholder="Nhập tên WiFi" required>
            </div>
        </div>
        <div class="form-group">
            <label for="pass">Mật khẩu</label>
            <div class="input-icon pass-input">
                <input type="password" id="pass" name="pass" placeholder="Nhập mật khẩu WiFi" required>
                <button type="button" class="toggle-password" onclick="togglePassword()">Hiện</button>
            </div>
        </div>
        <button type="submit" class="submit-btn">🔌 Kết nối ngay</button>
        <div class="info-box">
            <strong>💡 Lưu ý:</strong> Thiết bị sẽ tự động khởi động lại sau khi kết nối thành công.
        </div>
    </form>
</div>
<script>
function togglePassword() {
    const passInput = document.getElementById('pass');
    const toggleBtn = document.querySelector('.toggle-password');
    if (passInput.type === 'password') {
        passInput.type = 'text';
        toggleBtn.textContent = 'Ẩn';
    } else {
        passInput.type = 'password';
        toggleBtn.textContent = 'Hiện';
    }
}
</script>
</body>
</html>
)rawliteral";

// Trang thông báo thành công - Lưu trong PROGMEM
const char successPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Kết nối thành công</title>
<style>
* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    background: linear-gradient(135deg, #11998e 0%, #38ef7d 100%);
    min-height: 100vh;
    display: flex;
    justify-content: center;
    align-items: center;
    padding: 20px;
}

.container {
    background: white;
    border-radius: 20px;
    box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
    padding: 50px 40px;
    max-width: 450px;
    width: 100%;
    text-align: center;
    animation: slideIn 0.5s ease-out;
}

@keyframes slideIn {
    from {
        opacity: 0;
        transform: scale(0.8);
    }
    to {
        opacity: 1;
        transform: scale(1);
    }
}

.success-icon {
    font-size: 80px;
    margin-bottom: 20px;
    animation: bounce 1s ease infinite;
}

@keyframes bounce {
    0%, 100% {
        transform: translateY(0);
    }
    50% {
        transform: translateY(-10px);
    }
}

h2 {
    color: #11998e;
    font-size: 28px;
    font-weight: 600;
    margin-bottom: 15px;
}

.message {
    color: #666;
    font-size: 16px;
    line-height: 1.6;
    margin-bottom: 25px;
}

.info-box {
    background: #e8f8f5;
    border: 2px solid #11998e;
    padding: 15px;
    border-radius: 10px;
    font-size: 14px;
    color: #555;
}

.spinner {
    display: inline-block;
    width: 30px;
    height: 30px;
    border: 3px solid #f3f3f3;
    border-top: 3px solid #11998e;
    border-radius: 50%;
    animation: spin 1s linear infinite;
    margin-top: 20px;
}

@keyframes spin {
    0% { transform: rotate(0deg); }
    100% { transform: rotate(360deg); }
}
</style>
</head>
<body>
<div class="container">
    <div class="success-icon">✅</div>
    <h2>Kết nối thành công!</h2>
    <p class="message">Thông tin WiFi đã được lưu.<br>ESP32 đang khởi động lại...</p>
    <div class="info-box">
        <strong>⏱️ Vui lòng đợi khoảng 10 giây</strong><br>
        Thiết bị sẽ tự động kết nối WiFi mới
    </div>
    <div class="spinner"></div>
</div>
</body>
</html>
)rawliteral";

#endif // WIFI_CONFIG_PAGE_H
