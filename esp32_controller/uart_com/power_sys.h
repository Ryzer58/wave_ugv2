#ifndef POWER_SYS_H
#define POWER_SYS_H

#include <INA219_WE.h>

#define BATT_LOW 10.6
#define BATT_CRITC 10.3
#define BATT_NOM 11.1

INA219_WE ina219 = INA219_WE(INA219_ADDR);

typedef struct {
  float volt_shunt;
  float volt_load;
  float volt_bus;
  float current_mA;
  float power_mW;
  bool overflowing = false;
} powerData;

void ina219Get(powerData *data){                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
  data->volt_shunt = ina219.getShuntVoltage_mV();
  data->volt_bus = ina219.getBusVoltage_V();
  data->current_mA = ina219.getCurrent_mA();
  data->power_mW = ina219.getBusPower();
  data->volt_load  = data->volt_bus + (data->volt_shunt/1000);
  data->overflowing = ina219.getOverflow();

}

void ina219Print(powerData *data){

  Serial.print("Shunt Voltage [mV]: "); Serial.println(data->volt_shunt);
  Serial.print("Bus Voltage [V]: "); Serial.println(data->volt_bus);
  Serial.print("Load Voltage [V]: "); Serial.println(data->volt_load);
  Serial.print("Current[mA]: "); Serial.println(data->current_mA);
  Serial.print("Bus Power [mW]: "); Serial.println(data->power_mW);
  
  if(!data->overflowing){
    Serial.println("Values OK - no overflow");
  }
  else{
    Serial.println("Overflow! Choose higher PGAIN");
  }
}

/* Todo improve on voltage threshold detection */

#endif POWER_SYS_H