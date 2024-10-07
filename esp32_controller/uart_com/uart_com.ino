#include "board_conf.h"
#include <ArduinoJson.h>
#include <INA219_WE.h>

const uint8_t resolution = 8;

int freq = 100000;
int channel_A = 0;
int channel_B = 1;

int throttle = 140;
int throttle_min = 70;
int throttle_max = 250; 
int throt_a;
int throt_b;

unsigned long fetch_time;
//unsigned long run_time = 1000;
//unsigned long prev_rtime;
unsigned long sample_rate = 3000;
unsigned long last_sampled;



INA219_WE ina219 = INA219_WE(INA219_ADDR);

void initMotors(){
  pinMode(DDA1, OUTPUT);
  pinMode(DDA2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(DDB1, OUTPUT);
  pinMode(DDB2, OUTPUT);
  pinMode(PWMB, OUTPUT);

  ledcAttachChannel(PWMA, freq, resolution, channel_A);
  ledcAttachChannel(PWMB, freq, resolution, channel_B);

}

int initDevices(){
  
  int error_stat;
  Wire.setPins(COMM_SDA, COMM_SCL);
  Wire.begin();

  if(!ina219.init()){
    error_stat = 1;
  }

  else{
    ina219.setBusRange(BRNG_16);
    ina219.setShuntSizeInOhms(0.01);
  }

  return error_stat;
}

bool setMotors(int mota, int motb){

  bool throttle_error = false;
  if (mota > 0) {
    
    if (mota <= throttle_max) {
      digitalWrite(DDA1, LOW); //Direction is controlled by whatever pin is set high
      digitalWrite(DDA2, HIGH);
    }

    else {
      throttle_error = true;
    }
  }

  else{
    int rev_max = -throttle_max;
    
    if (mota > rev_max){
      digitalWrite(DDA1, HIGH);
      digitalWrite(DDA2, LOW);
    }

    else {
      throttle_error = true;
    }
  }

  if (motb > 0) {
    
    if (motb <= throttle_max){
      digitalWrite(DDB1, LOW);
      digitalWrite(DDB2, HIGH);
    }

    else {
      throttle_error = true;
    }
  }

  else{
    int rev_max = -throttle_max;
    
    if (motb > rev_max){
      digitalWrite(DDB1, HIGH);
      digitalWrite(DDB2, LOW);
    }

    else {
      throttle_error = true;
    }
  }

  if (!throttle_error){
    ledcWrite(PWMA, mota);
    ledcWrite(PWMB, motb);
  }
  
  return throttle_error;
}

void stopMotors(){
  ledcWrite(PWMA, 0);
  ledcWrite(PWMB, 0);
}

void ina219Info(){
  float shuntVoltage_mV = 0.0;
  float loadVoltage_V = 0.0;
  float busVoltage_V = 0.0;
  float current_mA = 0.0;
  float power_mW = 0.0; 
  bool ina219_overflow = false;

  shuntVoltage_mV = ina219.getShuntVoltage_mV();
  busVoltage_V = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getBusPower();
  loadVoltage_V  = busVoltage_V + (shuntVoltage_mV/1000);
  ina219_overflow = ina219.getOverflow();

  Serial.print("Shunt Voltage [mV]: "); Serial.println(shuntVoltage_mV);
  Serial.print("Bus Voltage [V]: "); Serial.println(busVoltage_V);
  Serial.print("Load Voltage [V]: "); Serial.println(loadVoltage_V);
  Serial.print("Current[mA]: "); Serial.println(current_mA);
  Serial.print("Bus Power [mW]: "); Serial.println(power_mW);
  if(!ina219_overflow){
    Serial.println("Values OK - no overflow");
  }
  else{
    Serial.println("Overflow! Choose higher PGAIN");
  }
  Serial.println();

}

void setup() {
  
  Serial.begin(115200);
  initMotors();
  int board_status = initDevices();
  
  if (board_status > 0){
    Serial.print("Error configuring ");
    
    if (board_status == 1){
      Serial.print("INA219");
    }
  }
}

void loop() {
    if(Serial.available() > 0){
      String jsonString = Serial.readString();
      StaticJsonDocument<200> cmd;
      deserializeJson(cmd, jsonString);

      throt_a = cmd["mota"];
      throt_b = cmd["motb"];

      bool status = setMotors(throt_a, throt_b);
      if (status == true){
        Serial.println("Throttle error: value out of range!");
      }
      
    fetch_time = millis();
    if(fetch_time - last_sampled >= sample_rate){
      ina219Info();
      last_sampled = fetch_time;
    }
    
  }

  delay(500);
  stopMotors();
}