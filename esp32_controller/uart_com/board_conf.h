/* Waveshare general driver for robots board:
 * This configuration file is derived form the board schematic and i2ctools on the Rasperry Pi to identify addresses on the bus. The board can be
 * used as a standalone for any given robotics project but is also included within the Waveshare UGV kits, UGV01, UGV02  and UGV03. The Microcontroller
 * is the ESP32-WROOM-UE featuring an external antenna mounted on top of the UGV chasis. Onboard are 4 sensors which are mapped out below with an I2C
 * header that allows for further expansion. Currently an OLED display integrated into the UGV chassis is the only additional component that uses this 
 * header. 
 * It has a Raspberry Pi header to allow for direct mounting to provide power and communication. A mirror header adjacent provides the option of adding
 * further Hats although this is currently concealled by the top cover. This header however cannot be used to reprogram the ESP32 however it still has 
 * to programmed via the central USBC port which is routed to the ESP32 through a CP2102A USB to serial converter. The outer USB C only connects to a 
 * secondary CP2102A USB to serial converter which is only utilised if you have a LIDAR unit present on the LIDAR header.
 * 
 * The board can handle a maximum input voltage of 16V input and has an onboard regulator that can provide 5V at up to 5A which is sufficient for most
 * Raspberry Pi models. A 3v3 power rail is also derived off the 5V rail, to provide power for the ESP32 and other onboard components. The schematic
 * also shows 2 interrupt lines from the QMI8658C although these do not appear to be routed anywhere.
 */

 
#ifndef BOARD_CONF_H
#define BOARD_CONF_H

/* Motor - A single TB6612FNG motor driver provides 2 channels, with a pair of motors joined to each channel to provide a total of 4 motors. 
 * Either 4 encoderless motors can be used or a combination of 2 encoder motors and 2 encoderless motors. It requires 2 Direction Drive
 * pins need to be define per channel (DDA & DDB) and a PWM Source to control the speed.
 * pulses. 
 */


#define PWMA 25         
#define DDA1 21
#define DDA2 17
#define PWMB 26          
#define DDB1 22       
#define DDB2 23        


/* The board features 2 encoder headers next to the cluster of 2 pin motor headers which share the same channel connections as the basic 2
 * pin headers but include 2 gpio as stated below and power. Note that gpio 16 and 27 are also routed to the generic pin header and can 
 * therefore be repurposed for additional gpio if not reserved for encoders.
 */
//#define ENCODER_EN
#ifdef ENCODER_EN
  #define ENCA_IN1 34
  #define ENCA_IN2 35
  #define ENCB_IN1 27
  #define ENCB_IN2 16
#else
  #define GPIO16 16
  #define GPIO27 27
#endif

/* General GPIO - A single row male pin header Located in the centre of the control board, enables interfacing with a select number of GPIO. Note that
 * GPIO 16 and GPIO 27 cannot be used for gpio if we are using encoded motors. The ESP32 gpio has a logic level of 3.3V. 
 */
#define GPIO4 4
#define GPIO5 5
 

/* Communication - Either UART or I2C are supported by the Raspberry Pi compatible pin header. For Serial communication the default serial 0 pins are 
 * used so no additional serial setup is required beyond the intial serial declaration. I2C uses a different allocation than the default for ESP32 So
 * the pins will need to be defined. The same I2C bus is also used internally for interacting with the onboard sensors so from the ESP32 side it will
 * still need to be enabled regardless of whether we are communicating with the Raspberry other I2C or not. The bus will still be physically connected
 * to the Raspberry Pi header so certain precautions need to be taken. Either destroy the bus prior to shutdown or disable deep sleep on the Raspberry
 * Pi to prevent accidental reboot because of activity on the bus which is a default feature on the Raspberry Pi when I2C is disabled.
 */
#define I2C_COMM
#ifdef I2C_COMM
  #define COMM_SDA 32
  #define COMM_SCL 33
#endif


/* IMU - This consist of QMI8658c (Accelerometer + Gyroscope) and AK09918 (Magnetometer). There is not currently a library that this sensor combination
 * however a standalone library exist for QMI8658C and separately an Adafruit library exist for using the AK09916 which is a similiar enough magnetometer
 * with ICM20948, another 6 DOF. These libraries should be close enough a point of reference to develop a more customised library better suited to the
 * hardware.
 */
//#define UGV_IMU
#ifdef UGV_IMU
  #define MAG_ADDR 0x0c
  #define GYRO_ADDR 0x6b
#endif


/* Power Monitoring - An INA219 provides basic voltage and current monitoring of the main power supply. It provides an indication of the overall battery
 * health and not individual cell statuses. This information can be used to define safe thresholds for an organised shutdown rather than being cut off
 * unexpectedly when the UPS undervoltage protection kicks in. Therefore this lower limit will need to be define above the cut off limit. The UPS module
 * itself is a closed loop system so the INA219 is our only form of insight. It is worth noting that the UPS also has a builtin INA219 however it is not
 * useable without connecting to the header on the back of the UPS module. 
 */
#define POWER_MON
#ifdef POWER_MON
  #define INA219_ADDR 0x42
  #define INA219_SHUNT 0.01f
#endif


/* Pressure/Temperature - A BMP280 is shown in the schematics and confirmed to exist when probed at the address listed in the schematic although not used
 * in the original UGV demo code. It can be used for detecting barometric pressure although its usefulness as an environmental temperature sensor appears 
 * limited as it shows higher than ambient temperature and aside from this experiences a degree of fluctuation.
 */
//#define BMP_EN
#ifdef BMP_EN
  #define BMP_ADDR 0x77
#endif

/* Servo serial bus - There are two 3 pin headers which can allow multiple compatible servos to be attached, thanks to the serial bus mechanism whereby the
 * the servos used are addressable. This need to be assigned a spare UART port in order to function on the pins listed below. It is intended to be used
 * with ST3215 based servos.
 */
//#define SERIAL_SERVO
#ifdef SERIAL_SERVO
  #define SER_TX 18
  #define SER_RX 19
#endif


/* SD card holder - The board is populated with a micro SD card holder which can be accessible by configuring an SPI interface on the pins listed below. It
 * does not leverage the dedicate esp32 SD/MMC peripheral. 
 */
//#define SD_CARD_SLOT
#ifdef SD_CARD_SLOT
  #define SD_MISO 12
  #define SD_MOSI 13
  #define SD_CLK 14
  #define SD_CS 15
#endif

#endif