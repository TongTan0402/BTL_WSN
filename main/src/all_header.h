#ifndef __ALL_HEADER_H__
#define __ALL_HEADER_H__

#include <Arduino.h>
#include <stdlib.h>

// Server and Payload Format Headers
#include <server.h>
#include <payload_format.h>

// Sensor Headers
#include <DHT11.h>
#include <MP25.h>
#include <MQ2.h>

#define PIN_MQ2    34
#define PIN_DHT    4
#define PIN_PM_LED 23
#define PIN_PM_ADC 32

#endif // __ALL_HEADER_H__