#ifndef __PAYLOAD_FORMAT__
#define __PAYLOAD_FORMAT__
#include <Arduino.h>
#include <stdint.h>

String PayloadFormat(float temp, float humidity, float fine_dust);

#endif