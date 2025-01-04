/* Temperature readings are not to far off by about 2 - 3 degrees, unlike the bme it does not measure humidity */

#ifndef ENV_H
#define ENV_H

#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp;

float ugv_temp;
float ugv_pressure;

void fetchBmp280Data(){

  float raw_value;
  float offset = 4.5; //based off of home thermostat readings
  raw_value = bmp.readTemperature();
  ugv_temp = raw_value - offset;

  ugv_pressure = bmp.readPressure();
}

void printBmp280Data(){
  Serial.print(F("Temp: "));
  Serial.print(ugv_temp);
  Serial.println("*C");

  Serial.print(F("Pressure: "));
  Serial.print(ugv_pressure);
  Serial.println(" Pa");
  
}

#endif