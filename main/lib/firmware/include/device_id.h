#ifndef DEVICE_ID_H
#define DEVICE_ID_H

#include <Arduino.h>
#include <vector>
#include <utility>
#include <stdio.h>

extern const std::vector<std::pair<uint64_t, const char*>> device_ids;
String GetDeviceId();

#endif // DEVICE_ID_H