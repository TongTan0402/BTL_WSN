#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import paho.mqtt.client as mqtt
import time

# Cấu hình MQTT
broker = "test.mosquitto.org"
port = 1883

device_ids = ["A101", "B202", "C303", "D404"]

while True:
  # Danh sách devices
  print("Nhập STT device cần cấu hình wifi:")
  print("1. A101")
  print("2. B202")
  print("3. C303")
  print("4. D404")
  device_id = input("\nNhập STT device: ")

  if device_id in ['1', '2', '3', '4']:
    break
  else:
    print("STT device không hợp lệ. Vui lòng thử lại.\n")

device_id = device_ids[int(device_id) - 1]


print("\nĐang kết nối MQTT...")

# Tạo client với API version mới (CallbackAPIVersion.VERSION2)
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.connect(broker, port, 60)

# Gửi message tới từng device
topic = f"ota/{device_id}/wifi_config"
client.publish(topic, "true", qos=1, retain=True)
print(f"Đã gửi lệnh wifi_config tới {device_id}")

print("Hoàn thành!")
client.disconnect()