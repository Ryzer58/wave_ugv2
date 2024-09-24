/*  Configuration of Waveshare general driver for robotics board based on the schematics and probing using i2ctools on the Rasperry Pi it forms the 
 *  heart of the Waveshare UGV01 abd UGV02 kit. Onboard is a handful of sensors and the ability to interact with the Raspberry Pi and similiar 
 *  boards that adopt the Pi form factor. The i2c header is only for connecting an oled display module but in theory could be used for connecting
 *  other i2c devices. Not currently included are ports for connecting a certain type of serial bus servo. There is a Lidar connection but this 
 *  appears to phyiscally seperated from the main ESP32 controller, merely acting as a serial to usb c adapter.
 *
 */

 /* Motor - A single motor IC with two driver channels. To provide a total of 4 motors each channel has two motors paralled together 
  * There are two 6 pin headers that share the same motor connections plus additonal IO, these are for motors that include an encoder.
  * For each of the control pins we will refer to it as the driver direction (DD) for each channel. If not using motors with an encoder
  * then the encoder pins should be disabled.
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
 * of managing the sensors in additon to the esp32.nIt will be more straightforwared to talk to the SBC by UART. UART uses the default pin mapping
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
// #define INA219_ADDR 0X42
// #define BATT_LOW 10.5
// #define BATT_CRTC 10.2


/* Pressure/Temperature - A BMP280 present in the schematics but not used the ugv base demo code. Seperate testing confirms that this does indeed work
 * however the reported temperature did seem a bit high. Prehaps the Radiant heat from the Raspberry Pi is skewing the reading. Further invvestigation
 * is needed.
 */
// #define BMP_ADDR 0x70


/* General GPIO - A single row male pin header Located in the centre of the control board, enables interfacing with a select number of GPIO. Note that
 * GPIO 16 and GPIO 27 cannot be used if using motors with an embedded encoder. The ESP32 operates at 3v3 logic as well. 
 */
// #defne GPIO0 4
// #define GPIO1 5
// #define GPIO2 16
// #define GPIO3 27

