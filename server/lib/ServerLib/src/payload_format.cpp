#include "payload_format.h"
#include <vector>
#include <utility>

// List of known node IDs and their names. Made static so it's constructed once.
static const std::vector<std::pair<uint64_t, const char*>> nodes = {
  {0x1A2B3C4D56789ULL, "Node_1"},
  // Add more mappings here: {0x..., "Node_X"},
};

static String node = "Unknown_Node";


String GetNode()
{
  if(node.substring(0, 4).equalsIgnoreCase("Node")) 
  {
    return node;
  }

  uint64_t chipid = ESP.getEfuseMac();
  for (const auto &entry : nodes) 
  {
    if (entry.first == chipid) 
    {
      node = String(entry.second);
    }
  }

  return node;
}

String PayloadFormat(float temp, float humidity, float fine_dust)
{
  String payload = "{\"Node\":\"" + GetNode() + "\",\"Temperature\":" + String(temp) + ",\"Humidity\":" + String(humidity) + ",\"FineDust\":" + String(fine_dust) + "}";
  
  return payload;
}
