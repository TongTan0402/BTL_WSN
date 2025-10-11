# Hướng dẫn nhanh: Ghi dữ liệu cảm biến vào Google Sheets bằng Apps Script

Tài liệu này mô tả cách tạo một Google Sheets + Apps Script Web App để nhận dữ liệu từ thiết bị (POST JSON) và ghi vào bảng.

## Mục tiêu
- Nhận POST request chứa JSON từ thiết bị (ESP/MCU) hoặc server.
- Ghi mỗi bản ghi thành một hàng trong Google Sheets với các trường: Timestamp, Node, Temperature, Humidity, FineDust.

## Yêu cầu
- Tài khoản Google với quyền tạo Google Sheets và Apps Script.
- URL Web App (Apps Script) sẽ nhận POST từ thiết bị — nếu muốn an toàn hơn, cân nhắc thêm xác thực.

## 1. Tạo Google Sheet
1. Mở Google Sheets và tạo một bảng mới (ví dụ: `SensorData`).
2. Ở hàng đầu tiên (header) đặt các cột sau: `Timestamp | Node | Temperature | Humidity | FineDust`

## 2. Tạo Apps Script (Web App)
1. Trong Google Sheets, vào menu `Extensions` → `Apps Script`.
2. Xóa nội dung mặc định và dán function sau vào file `Code.gs`:

```javascript
function doPost(e) {
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
  try {
    var data = JSON.parse(e.postData.contents);
    // Thêm hàng: thời gian hiện tại, tên node/air monitoring, nhiệt độ, độ ẩm, bụi mịn
    sheet.appendRow([
      new Date(),
      data.Node || '',
      data.Temperature || '',
      data.Humidity || '',
      data.FineDust || ''
    ]);
    return ContentService.createTextOutput('OK');
  } catch (err) {
    return ContentService.createTextOutput('ERROR: ' + err.message);
  }
}
```

3. Bấm `Deploy` → `New deployment`.
4. Chọn `Type: Web app`.
   - `Execute as`: Me
   - `Who has access`: Anyone
5. Bấm `Deploy`, cho phép quyền nếu được yêu cầu, rồi copy **Web app URL** (ví dụ: `https://script.google.com/macros/s/AKfycbx1234xyz/exec`).

> Lưu ý bảo mật: Chế độ `Anyone` sẽ cho phép mọi nguồn POST tới URL này. Nếu ứng dụng thật sự cần bảo mật, hãy thêm token API trong payload/header và kiểm tra trong `doPost`, hoặc giới hạn domain bằng các cơ chế bổ sung.

## 3. Ví dụ payload JSON (theo định dạng bạn cung cấp)
Thiết bị của bạn gửi payload theo dạng (ví dụ C++/Arduino):

```cpp
String payload = "{\"Node\":\"" + GetNode() + "\",\"Temperature\":" + String(temp) + ",\"Humidity\":" + String(humidity) + ",\"FineDust\":" + String(fine_dust) + "}";
```

Ví dụ JSON thực tế khi gửi:

```json
{
  "Node": "Node_1",
  "Temperature": 27.3,
  "Humidity": 55.1,
  "FineDust": 12.4
}
```

## 4. Kiểm thử nhanh bằng curl
Thay `YOUR_WEBAPP_URL` bằng URL bạn nhận được từ bước Deploy.

```bash
curl -X POST -H "Content-Type: application/json" \
  -d '{"Node":"Node_1","Temperature":27.3,"Humidity":55.1,"FineDust":12.4}' \
  "YOUR_WEBAPP_URL"
```

Nếu trả về `OK` và bạn thấy một hàng mới trong Google Sheet — cấu hình thành công.

## 5. Tích hợp với thiết bị (ESP/MCU)
- Trên ESP/ESP32/ESP8266 dùng HTTP client để POST JSON tới `YOUR_WEBAPP_URL`.
- Đảm bảo thiết bị có kết nối Internet (Wi-Fi) và thời gian (timestamp) nếu cần.

## 6. Gợi ý mở rộng
- Thêm xác thực: gửi token trong header hoặc payload và kiểm tra trong `doPost`.
- Thêm validate dữ liệu trước khi ghi (kiểm tra kiểu, giới hạn phạm vi).
- Nếu lưu lượng cao, cân nhắc lưu tạm vào queue rồi batch ghi vào Sheets để tránh bị giới hạn API.

## 7. Khắc phục lỗi thường gặp
- `ERROR` từ Apps Script: mở `Executions` trong Apps Script để xem logs và lỗi chi tiết.
- Không thấy dữ liệu trong Sheets: kiểm tra đúng sheet đang active, đúng column order và không có lỗi JSON.
