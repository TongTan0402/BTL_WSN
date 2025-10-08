#ifndef __PAYLOAD_FORMAT__
#define __PAYLOAD_FORMAT__
#include <Arduino.h>
#include <stdint.h>

String PayloadFormat(float temperature, float humidity, float pm25, float gas);

#endif