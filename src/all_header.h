#ifndef __ALL_HEADER_H__
#define __ALL_HEADER_H__

#include <Arduino.h>
#include <stdlib.h>

// Server and Payload Format Headers
#include <WiFi.h>
#include <server.h>
#include <payload_format.h>

// Sensor Headers
#include <DHT11.h>
#include <MP25.h>
#include <MQ2.h>

// Google Apps Script URL
const char* scriptURL = "https://script.google.com/macros/s/AKfycbwhTF9oBVzyj8QJrQak8oJ33r3Y5sb9E3t3pU96MB9cQt6CT6TCBKiZiFTQa9Oolbc/exec";

// Pin Definitions
#define PIN_MQ2       34
#define PIN_DHT       4
#define PIN_PM_LED    23
#define PIN_PM_ADC    32

#endif // __ALL_HEADER_H__