#ifndef HARDWARE_H
#define HARDWARE_H

#define API_KEY "AIzaSyDFRkn1QS32il2LLOlLqR151wcyrW87_ZY"
#define DATABASE_URL "https://esp32project-2b70f-default-rtdb.europe-west1.firebasedatabase.app"

// #define _SSID "DIGI-JCt5"
// #define _PWD "kFygHqd4ty"

#define _SSID "AndroidAP883d"
#define _PWD "test1234"

#include <Arduino.h>
#include <ESP8266WiFi.h> 
#include <Firebase_ESP_Client.h>
#include <EEPROM.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

const int max_string_length = 100;
char inData[max_string_length]; 

const char end_char = '\n';

typedef struct
{
  uint16_t        satellite : 1;
  uint16_t        time : 1;
  uint16_t        date : 1;
  uint16_t        latitude : 1;
  uint16_t        longitude : 1;
  uint16_t        precision : 1;
  uint16_t        altitude : 1;
  uint16_t        speed_knots : 1;
  uint16_t        course : 1;

} gnss_valid_t;

typedef struct
{
  gnss_valid_t    valid;
  uint8_t         satellite;
  uint8_t         time_h;
  uint8_t         time_m;
  uint8_t         time_s;
  uint8_t		      time_ss;
  uint8_t         date_y;
  uint8_t         date_m;
  uint8_t         date_d;
  float           latitude_tmp;
  float           longitude_tmp;
  float           latitude_deg;
  float           longitude_deg;
  float           precision_m;
  float           altitude_m;
  float           speed_knots;
  float           course_deg;

} gnss_t;

typedef struct
{
  uint8_t         true_compass : 1;
  uint8_t         mag_compass : 1;

} compass_valid_t;

typedef struct
{
  compass_valid_t valid;
  float           true_course_deg;
  float           mag_course_deg;

} compass_t;


typedef struct {
	gnss_t gnss;
	compass_t compass;
} NMEA_t;

extern NMEA_t NMEA;

inline float nmea_convert(float raw_degrees);
void updateLatLong(void);

#endif