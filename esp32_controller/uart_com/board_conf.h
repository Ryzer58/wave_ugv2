/*  Waveshare general driver for robots board:
 *  
 * This configuration file is derived form the board schematic and i2ctools on the Rasperry Pi to identify addresses on the bus. The board can be
 * used as a standalone for any given robotics project but also forms the heart of the Waveshare UGV kits, UGV01, UGV02  and UGV03. The Microcontroller 
 * is the ESP32-WROOM-UE featuring an external antenna mounted on top of the UGV chasis. Onboard are 4 sensors as identified below and two Pi compatible 
 * headers to mount the Raspberry Pi or any other board with a Pi compatible pinout. Serial communication to the ESP32 can be direct through the Pi 
 * header or via the CP2102A to the middle USB C port, which is the only means of programming the ESP32. The outer USB C only appears to be routed to 
 * the lidar header.
 * All sensors are communicated with via the i2c bus and an i2c header allows for addtional devices but is currently only used for connecting the oled 
 * display module inegrated into the UGV chasis. The board can handle a maximum of 16V input and regulates it down to 5V at 5A which is more than 
 * sufficient to power the Raspberry Pi. This is further regulated down to 3.3V for the ESP and 1.8V for the sensors. There are two interrupt lines
 * brought out from the primary IMU which do not appear to be mapped anywhere on the ESP.
 *
 *
 * Motor - A single motor IC with two driver channels. To provide a total of 4 motors each channel has two motors paralled together 
 * There are two 6 pin headers that share the same motor connections as the 4 two pin headers plus additonal IO for reading encoder 
 * pulses. For each of the control pins responsible for setting the motor direction, we will refer to it as the driver direction (DD)
 * for each channel. If using encoderless motors, then we pins still be used for gpio 
 */
#define PWMA 25         
#define DDA1 21
#define DDA2 17
#define PWMB 26          
#define DDB1 22       
#define DDB2 23        

#ifdef ENCODER_EN
  #define ENCA_IN1 34
  #define ENCA_IN2 35
  #define ENCB_IN1 27
  #define ENCB_IN2 16
#else
  #define GPIO2 16
  #define GPIO3 27
#endif

/* General GPIO - A single row male pin header Located in the centre of the control board, enables interfacing with a select number of GPIO. Note that
 * GPIO 16 and GPIO 27 cannot be used for gpio if we are using encoded motors. The ESP32 gpio has a logic level of 3.3V. 
 */
#define GPIO0 4
#define GPIO1 5
 

/* Communication - Either UART or I2C are supported by the Raspberry Pi compatible pin header. For Serial no further configuration is needed as it
 * adopts the default serial0 pin mapping. I2C in contrast requires using custom pins as defined below and we need to consider that we will also be
 * communicating with sensors on the same bus. Unfortunately the Pi hardware does not support operating multimaster mode. Note that even if not 
 * using I2C, the pins remain physically connected to the bus lines via the header. To avoid conflict I2C should be disabled on the Raspberry. 
 * However when pin 3 is configured in GPIO mode, the underlying hardware enables the Pi to 'wake' from a lower power state when pulled low. This
 * extends to even when the Pi has been instructed to shutdown. Unfortunately due to the constant connection, the background i2c acitivity will
 * lead to this mechanism being triggered. On Newer Raspberry Pi models which include an eeprom bootloader an easy work around is to simple disable 
 * this behavior in the configuration file settings. A prefered approuch as attempted below would be to instead disable all I2C communication
 * on the ESP32 via an instruction before the Raspberry Pi commences shutdown.
 */
#define I2C_COMM
#ifdef I2C_COMM
  #define COMM_SDA 32
  #define COMM_SCL 33
#endif


/* IMU - A combination of the AK09918 and QMI8658c. The QMI8658c is a 6 Dof, gyroscope and an accelerometer while the AK09918 is purely a Magnometer.
 * Currently there are seperate libraries that support the QMI8658c but not yet in conjuction with AK09918. The Adafruit ICM20x library features a 
 * similiar Ak09916 library but instead targeted towards the ICM20948. Regardless this library should provide a good enough base reference to begin
 * working withe ak09918. Aside from this seedstudio are the only other vendor who use similiar hardware.
 */
//#define UGV_IMU
#ifdef UGV_IMU
  #define MAG_ADDR 0x0c
  #define GYRO_ADDR 0x6b
#endif


/* Power Monitoring - An INA219 provides monitoring of voltage and current of the main power supply. This is not ideal for working with Li-ion batteries
 * but better than nothing. To prevent any cell from dropping below voltage and causing an unexpect shutdown we need to set a safety margin at which to
 * trigger a shutdown alert. The UPS module does feature under voltage cut of to protect the cells from going to low however this should not be relied on 
 */
#define POWER_MON
#ifdef POWER_MON
  #define INA219_ADDR 0x42
  #define BATT_LOW 10.6
  #define BATT_CRTC 10.3
#endif


/* Pressure/Temperature - A BMP280 present in the schematics but not used the ugv base demo code. Seperate testing confirms that it is indeed present 
 * does work but the reported temperature appears to be slightly higher than room temperature. Intially I suspected this may be caused by Radiant heat 
 * from the Raspberry Pi which may skewing the reading. Further investigation is needed.
 */
#define BMP_EN
#ifdef BMP_EN
  #define BMP_ADDR 0x77
#endif

/* Servo serial bus - There are two 3 pin headers which can allow multiple compatible servos to be attached. The reason being is that these servos are
 * addressable. An adapter ic takes the serial input and converts the logic into a protocal the servo can understand. These servos support feedback which
 * can be recieved along rx line.
 */
#ifdef SERIAL_SERVO
  #define SER_TX 18
  #define SER_RX 19
#endif


/* SD card holder - Micro SD card holder (Todo) */

#ifdef SD_CARD_SLOT
  #define SD_MISO 19
  #define SD_MOSI 23
  #define SD_CLK 18
  #define SD_CS 5
#endif