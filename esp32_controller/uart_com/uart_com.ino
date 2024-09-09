
#include "board_conf.h"
// #include <ArduinoJson.h>

const uint8_t resolution = 8;

int freq = 100000;
int channel_A = 0;
int channel_B = 1;

int throttle = 140;
int min_throttle = 80;
int max_throttle = 250; 

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

void dirNormal(){
  digitalWrite(DDA1, LOW);
  digitalWrite(DDA2, HIGH);
  digitalWrite(DDB1, LOW);
  digitalWrite(DDB2, HIGH);
}

void dirInversed(){
  digitalWrite(DDA1, HIGH);
  digitalWrite(DDA2, LOW);
  digitalWrite(DDB1, HIGH);
  digitalWrite(DDB2, LOW);
}

void spinNormal(){
  digitalWrite(DDA1, LOW);
  digitalWrite(DDA2, HIGH);
  digitalWrite(DDB1, HIGH);
  digitalWrite(DDB2, LOW);
}

void spinInversed(){
  digitalWrite(DDA1, HIGH);
  digitalWrite(DDA2, LOW);
  digitalWrite(DDB1, LOW);
  digitalWrite(DDB2, HIGH);
}

void setup() {
  initMotors();
  //Serial.setPins(COMM_RX, COMM_TX);
  Serial.begin(115200);
  
}

void loop() {
  //Not currently used
  //Serial.println("Hello World");
  if(Serial.available() > 0){
    move = Serial.read();
  }

  if (move == 'w'){
    dirNormal();
    ledcWrite(PWMA, throttle);
    ledcWrite(PWMB, throttle);

  }
  if (move == 's'){
    dirInversed();
    ledcWrite(PWMA, throttle);
    ledcWrite(PWMB, throttle);

  }
  if (move == 'a'){
    spinNormal();
    ledcWrite(PWMA, throttle);
    ledcWrite(PWMB, throttle);

  }
  if (move == 'd'){
    spinInversed();
    ledcWrite(PWMA, throttle);
    ledcWrite(PWMB, throttle);
    
  }
  if (move == ','){
    if (throttle > min_throttle){
      throttle = throttle - 10;
      Serial.println("Accel");
    }
  }
  if (move == '.'){
    if (throttle < max_throttle){
      throttle = throttle + 10;
      Serial.println("Deccel");
    }
  }

  delay(500);
  ledcWrite(PWMA, 0);
  ledcWrite(PWMB, 0);

}
