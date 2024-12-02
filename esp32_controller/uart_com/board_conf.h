/*  Waveshare general driver for robots board:
 *  
 *  This configuration file is based on information obtained form the schematics and using i2ctools on the Rasperry Pi to probe the bus. It can be
 *  used as a standalone board but also forms the heart of the Waveshare UGV01 abd UGV02 kit. The Microcontroller is the ESP32-WROOM-UE which 
 *  features an external antenna mounted on top of the UGV chasis. Onboard is a handful of sensors and a Pi compatible header to interact with the 
 *  Raspberry Pi and any other boards that adopt the same pinout. The central USB C port is an addtional means of interfacing and the primary means
 *  of programming the ESP32. The other USB C appears to be only routed to lidar input, acting in isolation from the rest of the control board.
 *  The i2c header currently only provides a way of connecting the oled display module that is inegrated into the UGV chasis although it should work
 *  other i2c devices so long as it supports 3v3 operation. The board can handle a maximum of 16V and regulates it down to 5V at 5A which is more 
 *  than sufficient for also powering the Raspberry Pi from.
 *
 */

/* Motor - A single motor IC with two driver channels. To provide a total of 4 motors each channel has two motors paralled together 
 * There are two 6 pin headers that share the same motor connections as the 4 two pin headers plus additonal IO for reading encoder 
 * pulses. For each of the control pins responsible for setting the motor direction, we will refer to it as the driver direction (DD)
 * for each channel. If using encoderless motors, then we can save pins for other use cases by disabling encoder assignment 
 */
#define PWMA 25         
#define DDA1 21
#define DDA2 17
#define PWMB 26          
#define DDB1 22       
#define DDB2 23        

// #define ENCA_IN1 34
// #define ENCA_IN2 35
// #define ENCB_IN1 27
// #define ENCB_IN2 16


/* Communication - Either UART or I2C are supported by the Raspberry Pi compatible pin header. Note that if communicating via I2C, all the sensors
 * available on the controller board area also attached to the I2C bus therefore if undertaking this route the SBC will taken on the extra overhead
 * of managing the sensors in additon to the esp32. It will be more straightforwared to talk to the SBC by UART. UART uses the default pin mapping
 * but I2C uses a custom mapping as defined below 
 */
#define COMM_SDA 32
#define COMM_SCL 33


/* IMU - A combination of the AK09918 and QMI8658c. The QMI8658c is a 6 dof, gyroscope and an accelerometer while the AK09918 is purely a Magnometer.
 * Currently there are standalone libraries that support the QMI8658c but yet to find one that supports it in conjuction with AK09918. A library does 
 * exist that does support the AK09918 in with another IMU so it should be possible by using this library as a point of reference along with reviewing
 * the waveshare source code.
 */
// #define MAG_ADDR 0xc0
// #define GYRO_ADDR 0x6b


/* Power Monitoring - An INA219 provides monitoring of voltage and current of the main power supply. This is not ideal for working with Li-ion batteries
 * but better than nothing. To prevent any cell from dropping below voltage and causing an unexpect shutdown we need to set a safety margin at which to
 * trigger a shutdown alert. The UPS module does feature under voltage cut of to protect the cells from going to low however this should not be relied on 
 */
#define INA219_ADDR 0x42
#define BATT_LOW 10.6
#define BATT_CRTC 10.3


/* Pressure/Temperature - A BMP280 present in the schematics but not used the ugv base demo code. Seperate testing confirms that this does indeed work
 * however the reported temperature did seem a bit high. Prehaps the Radiant heat from the Raspberry Pi is skewing the reading. Further invvestigation
 * is needed.ervo bus signal
 */
// #define BMP_ADDR 0x70

/* Servo serial bus - There are two 3 pin headers which can allows numerous compatible servos to be chained together. An adapter ic takes the serial input
 * and converts to servo bus. It is also possible to recieve feedback from the servos.
 */
// #define SER_TX 18
// #define SER_RX 19

/* General GPIO - A single row male pin header Located in the centre of the control board, enables interfacing with a select number of GPIO. Note that
 * GPIO 16 and GPIO 27 cannot be used if using motors with an embedded encoder. The ESP32 operates at 3v3 logic as well. 
 */
// #defne GPIO0 4
// #define GPIO1 5
// #define GPIO2 16
// #define GPIO3 27

/* SD card holder - Todo */
//#define SD-MISO 19
//#define SD_MOSI 23
//#define SD_CLK 18
//#define SD_CS 5

