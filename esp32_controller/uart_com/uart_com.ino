
#include "board_conf.h"
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

char move;

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
  initMotors();
  //Serial.setPins(COMM_RX, COMM_TX);
  Serial.begin(115200);
  
}


void loop() {
    if(Serial.available() > 0){
    String jsonString = Serial.readString();
    StaticJsonDocument<200> cmd;
    deserializeJson(cmd, jsonString);

    throt_a = cmd["mota_speed"];
    throt_b = cmd["motb_speed"];

    bool status = setMotors(throt_a, throt_b);
    if (status == true){
      Serial.println("Throttle error: invalid value sent!");
    }
  }

  delay(500);
  stopMotors();

}
