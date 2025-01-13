
#include "board_conf.h"
#include "oled_disp.h"
//#include "power_sys.h"
#include "env.h"
#include <esp_timer.h>
#include <ArduinoJson.h>

const uint8_t resolution = 8;

int freq = 40000;

int throttle = 140;
int throttle_min = 70;
int throttle_max = 250; 
int throt_a;
int throt_b;
bool motion = false;
bool active = true;

const byte bufferSize = 64;
char inboundBuffer[bufferSize];
bool newData = false;
bool start;

// Todo outbound buffer

//unsigned long fetch_time;
signed long sample_rate = 5000;
unsigned long last_sampled;
//const long check_interval = 100;

hw_timer_t *motion_timer = NULL;

void IRAM_ATTR motion_ISR()
{
  stopMotors();
  motion = false;
}

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


int initBus(){
  
  int error_stat = 0;
  Wire.setPins(COMM_SDA, COMM_SCL);
  Wire.begin();

  delay(50); // Delay startup otherwise INA219 does not appear to register correctly
  if(!ina219.init()){
    error_stat = 1;
  }

  else{
    ina219Setup();
  }

  unsigned bmp_state;
  bmp_state = bmp.begin(BMP_ADDR);
  
  if(!bmp_state){
    error_stat = 3;
  }

  else{
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  }

  if(!display.begin(SSD1306_SWITCHCAPVCC, DISP_ADDR)){
    error_stat = 2;
  }

  else{
    display.clearDisplay();
    display.display();
  }

  return error_stat;
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
    Serial.print("Motor A: ");
    Serial.println(mota_val);

    ledcWrite(PWMB, motb_val);
    Serial.print("Motor B: ");
    Serial.println(motb_val);

    motion = true;
    start = true;
  }

  else{
    throttle_err = true;
  }
    
  return throttle_err;
}

void stopMotors(){
  ledcWrite(PWMA, 0);
  ledcWrite(PWMB, 0);
  Serial.println("Stopping motors!");
}

void setup() {
  
  Serial.begin(115200);
  initMotors();
  
  uint8_t board_status = initBus();
  
  if (board_status > 0){
    Serial.print("Error configuring ");
    
    if (board_status == 1){
      Serial.println("INA219");
    }

    if (board_status == 2){
      Serial.println("SSD1306");
    }

    if (board_status == 3){
      Serial.println("BMP280");
    }

    /* Todo - This can either be considered as 1 unit or 2 units given that there are two physcial components
    if (board_status == 4){
      Serial.println("IMU");
    } */
  }

  else {
    bootScreen();
  }

  motion = 0;
  fetchBmp280Data();
  if(ugv_temp <= 5.00){
    Serial.println("Warn low temp");
  }

}

void loop() {
    
  powerData ugvStatus;

  fetchSerial();
  processData();

  /* Does not preform as expected */
  
  if (motion){
    motion_timer = timerBegin(80000000);
    timerAttachInterrupt(motion_timer, &motion_ISR);
    timerAlarm(motion_timer, 2000000, true, 0);
    //timerStart(motion_timer); 
  }
  else{
    timerStop(motion_timer);
  }

  if (active == true){
    unsigned long health_timer = millis();
    if(health_timer - last_sampled >= sample_rate){
      ina219Get(&ugvStatus);
      if(!ugvStatus.batt_nom){
        sample_rate = 2000;
      }
      displayPowerData(&ugvStatus);
      last_sampled = health_timer;
    }
  }


  /*
  if(motion == true){
    delay(2000);
    stopMotors();
    motion = false;
  } */

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
          motion = false;
        }

        else {
          motion = true;
          start = true;
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
        active = false;
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