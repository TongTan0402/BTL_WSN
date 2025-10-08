#include "payload_format.h"
#include <vector>
#include <utility>
#include <stdio.h>

static const std::vector<std::pair<uint64_t, const char*>> nodes = {
  {0x000074D42FA7DBCCULL, "A101"},
  {0x000074D42FA7DBCCULL, "B202"},
  {0x000074D42FA7DBCCULL, "C303"},
  {0x000074D42FA7DBCCULL, "D404"},
};

static String node = "";


String GetNode()
{
  if(node != "") return node;

  uint64_t chipid = ESP.getEfuseMac();
  char ok = 0;
  for (const auto &entry : nodes) 
  {
    if (entry.first == chipid) 
    {
      node = String(entry.second);
      ok = 1;
      break;
    }
  }

  if(!ok) {
    char buf[17];
    snprintf(buf, sizeof(buf), "%016llX", (unsigned long long)chipid);
    node = String(buf);
  }

  return node;
}

String PayloadFormat(float temperature, float humidity, float pm25, float gas)
{
  // test location cycling
  static const char *location[] = {"A101", "B202", "C303", "D404"};
  static char index = 0;

  index = (index + 1) % 4;

  String jsonPayload = "{";
  jsonPayload += "\"location\":\"" + String(location[index]) + "\",";
  jsonPayload += "\"temp\":" + String(temperature, 1) + ",";
  jsonPayload += "\"humid\":" + String(humidity, 1) + ",";
  jsonPayload += "\"pm25\":" + String(pm25) + ",";
  jsonPayload += "\"gas\":" + String(gas);
  jsonPayload += "}";

  return jsonPayload;
}
