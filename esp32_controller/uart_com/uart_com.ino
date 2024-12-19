
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
bool motion;

const byte bufferSize = 256;
char inboundBuffer[bufferSize];
bool newData = false;

// Todo outbound buffer

unsigned long fetch_time;
unsigned long run_time = 1000;
unsigned long last_run;
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

  else{
    display.clearDisplay();
    display.display();
  }

  /* Todo implement bmp280 next */

  return error_stat;
}

/*
 * The I2C header connection remains physically connected to the I2C line even if not proactively as a mean of communication. When
 * the Raspberry's pins are configured in GPIO mode, then gpio 3 can be used to 'wake' the Raspberry Pi when being pulled low due
 * to how the underlying hardware works. This means that background occuring on the I2C bus can cause the Pi to be rebooted after
 * beings instructed to shutdown. On Raspberry Pi's that feature an eeprom an easy work around it is to simple disable this 
 * behavior in the configuration settings. A prefered approuch as attempted below would be to instead disable all I2C communication
 * on the ESP32 via an instruction before the Raspberry Pi commences shutdown.
 *
 */
int deactivateDevices(){
  ina219.powerDown();
  Wire.end();
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

  else {
    bootScreen();
  }
}

void loop() {
    
  powerData ugvStatus;

  fetchSerial();
  processData();

  fetch_time = millis();
  if(fetch_time - last_sampled >= sample_rate){
    ina219Info(&ugvStatus);
    displayPowerData(&ugvStatus);
    last_sampled = fetch_time;
  }

  if (motion == true){
    if(fetch_time - last_run >= run_time){
      stopMotors();
      last_run = fetch_time;
      motion = false;
    }
  }
}

void fetchSerial(){
  static byte ndx = 0;
  char endpoint = '\n';
  char rc;

  while(Serial.available() > 0 && newData == false){
    rc = Serial.read();

    if (rc != endpoint){
      inboundBuffer[ndx] = rc;
      ndx++;

      if (ndx >= bufferSize){
        ndx = bufferSize -1;
      }
    } 

    else {
      inboundBuffer[ndx] = '\0';
      ndx = 0;
      newData = true;
    } 
  }
}

void processData(){
  if (newData == true){

    StaticJsonDocument<bufferSize> cmd;
    DeserializationError error  = deserializeJson(cmd, inboundBuffer);

    if(error){
      Serial.print("Failed to parse JSON: ");
      Serial.println(error.c_str());
      return;
    }

    const char* command = cmd["comd"];
      
    if(strcmp(command,"m") == 0){
        
      throt_a = cmd["mota"];
      throt_b = cmd["motb"];
      bool rangeErr = setMotors(throt_a, throt_b);
      if (rangeErr == true){
        Serial.println("Throttle error: value out of range!");
      }

      else {
        motion = true;
      }
    }

    else if(strcmp(command,"p") == 0){
      deactivateDevices();
      Serial.println("okay to shutdown!");
    }

    else {
      Serial.println("Error operation not supported");
    }

    /* TODO - Implement further functionality such as request sensor data */
  }
}