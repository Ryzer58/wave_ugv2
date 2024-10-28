
#include "board_conf.h"
#include "oled_disp.h"
//#include "power_sys.h"
#include <ArduinoJson.h>

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

  if(!display.begin(SSD1306_SWITCHCAPVCC, DISP_ADDR)){
    error_stat = 2;
  }

  display.clearDisplay();
  display.display();

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



void setup() {
  
  Serial.begin(115200);
  initMotors();
  int board_status = initDevices();
  
  if (board_status > 0){
    Serial.print("Error configuring ");
    
    if (board_status == 1){
      Serial.println("INA219");
    }

    if (board_status == 2){
      Serial.println("SSD1306");
    }
  }
}

void loop() {
    
    powerData ugvStatus;

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
    }
      
    fetch_time = millis();
    if(fetch_time - last_sampled >= sample_rate){
      ina219Info(&ugvStatus);
      displayPowerData(&ugvStatus);
      last_sampled = fetch_time;
    }

    delay(500);
    stopMotors();
}