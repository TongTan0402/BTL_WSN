#include "payload_format.h"
#include <stdio.h>
#include "device_id.h"

String PayloadFormat(float temperature, float humidity, float pm25, float gas)
{
  String jsonPayload = "{";
  jsonPayload += "\"location\":\"" + GetDeviceId() + "\",";
  jsonPayload += "\"temp\":" + String(temperature, 1) + ",";
  jsonPayload += "\"humid\":" + String(humidity, 1) + ",";
  jsonPayload += "\"pm25\":" + String(pm25) + ",";
  jsonPayload += "\"gas\":" + String(gas);
  jsonPayload += "}";

  return jsonPayload;
}
