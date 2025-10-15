#include "payload_format.h"
#include <stdio.h>
#include "device_id.h"

String PayloadFormat(float temperature, float humidity, float pm25, float gas)
{
  // static char *devices[] = {"A101", "B202", "C303"};
  // static int numDevices = sizeof(devices) / sizeof(devices[0]);
  // static int deviceIndex = 0;
  // deviceIndex = (deviceIndex + 1) % numDevices;

  String jsonPayload = "{";
  jsonPayload += "\"location\":\"" + GetDeviceId() + "\",";
  jsonPayload += "\"temp\":" + String(temperature, 1) + ",";
  jsonPayload += "\"humid\":" + String(humidity, 1) + ",";
  jsonPayload += "\"pm25\":" + String(pm25) + ",";
  jsonPayload += "\"gas\":" + String(gas);
  jsonPayload += "}";

  return jsonPayload;
}
