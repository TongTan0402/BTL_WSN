#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import paho.mqtt.client as mqtt
import time

# Cấu hình MQTT
broker = "test.mosquitto.org"
port = 1883

# Danh sách devices
devices = ["A101", "B202", "C303", "D404"]

print("Đang kết nối MQTT...")

# Tạo client với API version mới (CallbackAPIVersion.VERSION2)
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.connect(broker, port, 60)

print("Gửi lệnh update tới các devices...")

# Gửi message tới từng device
for device in devices:
    topic = f"ota/{device}/update_firmware"
    client.publish(topic, "true", qos=1, retain=True)
    print(f"Đã gửi lệnh update tới {device}")

print("Hoàn thành!")
client.disconnect()