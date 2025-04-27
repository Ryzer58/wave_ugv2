/* Temperature readings appear to be a few degrees higher (3 - 4*C) than ambient temperature. Compared to the BME280 it does not measure humidity */

#ifndef ENV_H
#define ENV_H

#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp;

float ugv_temp;
float ugv_pressure;

bool probeBMP(int dev){
  unsigned bmpReady;
  bmpReady = bmp.begin(dev);
  
  if(!bmpReady){
    return false;
  }

  else{
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
    return true;
  }
}

void fetchBmp280Data(){

  float raw_value;
  float offset = 4.5; // based off of home thermostat readings
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

// Altitude can be calculated but not currently implemented

#endif