
#include "board_conf.h"
#include "power_sys.h"
#include "oled_disp.h"
#include "env.h"
#include <ArduinoJson.h>

/* Motors Configuration data */
const uint8_t resolution = 8;
int freq = 40000;
int throttle = 140;
int throttle_min = 70;
int throttle_max = 250; 
int throt_a;
int throt_b;
bool mot_run = false;

/* Communication buffers - for now just focus on the incoming buffer as serial print statements are easier to read
 * but later data should also be returned in json format to be processed on the Raspberru Pi other SBC.
 */
const byte bufferSize = 64;
char inboundBuffer[bufferSize];
bool newData = false;
bool start;

/* outboundBuffer - Todo */

bool bus_active = true;

/* UGV device timings - For Now use millis() as our current scheduling frane  */

const long motor_timeout = 5000;
const long time_to_read = 100;

unsigned long sample_rate = 5000;
unsigned long start_time = 0;
unsigned long last_check = 0;
unsigned long last_sampled = 0;

void initMotors(){
  pinMode(DDA1, OUTPUT);
  pinMode(DDA2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(DDB1, OUTPUT);
  pinMode(DDB2, OUTPUT);
  pinMode(PWMB, OUTPUT);

  ledcAttach(PWMA, freq, resolution);
  ledcAttach(PWMB, freq, resolution);
  
}

 /* To avoid accidently restarting the Raspberry Pi when instructed to shutdown, first disable the I2C bus. (See board.conf) */
int deactivateBus(){
  ina219.powerDown();
  delay(50); // Wait for the INA219 to shutdown before disabling the bus otherwise the ESP32 seems to crash
  Wire.end();
  delay(50);
  return 0;
}

bool setMotors(int mota, int motb){

  bool throttle_err = false;
  int mota_val;
  int motb_val;

  if(mota > 0){
    digitalWrite(DDA1, LOW); //Direction is controlled by whatever pin is set high
    digitalWrite(DDA2, HIGH);
    //Serial.println("Forward");
  }

  else{
    digitalWrite(DDA1, HIGH);
    digitalWrite(DDA2, LOW);
  }

  if(motb > 0){
    digitalWrite(DDB1, LOW); 
    digitalWrite(DDB2, HIGH);
  }

  else{
    digitalWrite(DDB1, HIGH);
    digitalWrite(DDB2, LOW);
  }

  mota_val = abs(mota);
  motb_val = abs(motb);

  if(mota >= throttle_min && mota <= throttle_max && motb >= throttle_min && motb <= throttle_max){
    
    ledcWrite(PWMA, mota_val);
    ledcWrite(PWMB, motb_val);
    
    Serial.print("Motors - (A ");
    Serial.print(mota_val);
    Serial.print(",B: ");
    Serial.print(motb_val);
    Serial.println(" )");

    mot_run = true;
    start_time = millis();
  }

  else{
    throttle_err = true;
    mot_run = false;
  }
    
  return throttle_err;
}

void stopMotors(){
  ledcWrite(PWMA, 0);
  ledcWrite(PWMB, 0);
  Serial.println("Stopping motors!");
  mot_run = false;
}

void setup() {
  bool status = true;
  
  Serial.begin(115200);

  Wire.setPins(COMM_SDA, COMM_SCL);
  Wire.begin();

  delay(50); // Delay startup otherwise INA219 does not appear to register correctly
  status = probeIna219();
  if (status == false){
    Serial.println("INA219 Error");
  }

  status = probeBMP(BMP_ADDR);
  if (status == false){
    Serial.println("BMP280 Error");
  }

  /* Todo - The IMU consist of 2 seperate pieces of hardware that could be checked seperately or together.
   * status = probeIMU();
   * if (status == false){
   *     Serial.println("IMU Error");
   *}
   */

  
  status = probeSSD1306(DISP_ADDR);
  if (status == false){
    Serial.println("OLED Error");
  }

  else {
    bootScreen();
  }
  
  initMotors();
  
  mot_run = 0;
  
  fetchBmp280Data();
  if(ugv_temp <= 5.00){
    Serial.println("Warn low temp");
    // TODO - possible print a frost graphic or temperature icon with exclaimation mark
  }

}

void loop() {

  ugvPower powerData;

  unsigned long current_time = millis();

  fetchSerial();
  processData();

  if (mot_run == true){

    if (current_time - start_time >= motor_timeout){
      
      stopMotors();
      mot_run = false;
    }
  }
  
  if (bus_active == true){
    unsigned long health_timer = millis();
    if(health_timer - last_sampled >= sample_rate){
      ina219Get(&powerData);
      if(!powerData.batt_nom){
        sample_rate = 2000;
      }
      displayPowerData(powerData.volt_bus, powerData.current_mA);
      last_sampled = health_timer;
    }
  }
}

void fetchSerial(){
  static byte ndx = 0;
  char endpoint = '\n';
  char rc;

  if(Serial.available() > 0 && newData == false){
    rc = Serial.read();

    if (rc != endpoint){
      inboundBuffer[ndx] = rc;
      ndx++;

      if (ndx >= bufferSize){
        ndx = bufferSize - 1;
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
  bool rangeErr;
  if (newData == true){

    StaticJsonDocument<bufferSize> cmd;
    DeserializationError error  = deserializeJson(cmd, inboundBuffer, bufferSize);

    if(error){
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      newData = false;
      return;
    }

    const char* command = cmd["comd"];
    
    switch(command[0]){

      case 'm': // Motion command - {"comd":"m","mota":100,"motb":100}
        Serial.println("starting motors: ");
        throt_a = cmd["mota"];
        throt_b = cmd["motb"];
        rangeErr = setMotors(throt_a, throt_b);
        if (rangeErr == true){
          Serial.println("Throttle error: value out of range!");
          mot_run = false;
        }

        break;

      case 'h': // Stop motors
        stopMotors(); 
        break;
      
      case 't':
        fetchBmp280Data();
        printBmp280Data();
        break;

      case 'p': // Power down command - consider also adding a delay option
        deactivateBus();
        bus_active = false;
        Serial.println("okay to shutdown!");
        break;

      default:
        Serial.println("Error invalid input!");
        break;
    }

    /* TODO - Implement further functionality such as request sensor data */
    newData = false;
  }
}