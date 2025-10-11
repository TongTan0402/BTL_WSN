#include "device_id.h"


const std::vector<std::pair<uint64_t, const char*>> device_ids = {
  {0x000074BEA9E342A8ULL, "A101"},
  {0x000074D42FA7DBCCULL, "B202"},
  {0x0000FC462F2B1838ULL, "C303"},
  {0x000074D42FA7DBCCULL, "D404"},
};


static String device_id = "";


String GetDeviceId()
{
  if(device_id != "") return device_id;

  uint64_t chipid = ESP.getEfuseMac();
  char ok = 0;
  for (const auto &entry : device_ids) 
  {
    if (entry.first == chipid) 
    {
      device_id = String(entry.second);
      ok = 1;
      break;
    }
  }

  if(!ok) {
    char buf[17];
    snprintf(buf, sizeof(buf), "%016llX", (unsigned long long)chipid);
    device_id = String(buf);
  }

  return device_id;
}
