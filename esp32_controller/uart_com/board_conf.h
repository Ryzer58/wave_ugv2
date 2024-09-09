/*  Configuration of Waveshare general driver board:
 *  The board includes a handful of sensors as well
 *  as motor driver chip. Optionally it allows for
 *  the inclusion of a Lidar as well as bus driven
 *  Servos.
 *
 */

 /* Motor - It featuresa single motor IC with two driver channels. To provide a total of 4 motors each channel has two motors paired in parallel 
  * Aside from this it has to 6 pin headers for connecting motors with an embedded encoder, the connections for which are also listed below but
  * Currently excluded. For each pin we will refer to it as the driver direction (DD) for each channel.
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


/* Communication - Either UART or I2C are supported by the Raspberry Pi compatible to header. Note that if using then given that all the integrated
 * sensors communicate over the I2C bus, it will be more straightforwared for the SBC to talk directly with them instead. UART uses the default
 * ESP32 defined pins however I2C uses a custom mapping as defined below 
 */
#define COMM_SDA 32
#define COMM_SCL 33


/* IMU - Consisting of the AK09918 and QMI8658c. The QMI8658c is a 6 dof, gyroscope and an accelerometer while the AK09918 acts as a Magnometer.
 * This has still yet to be implemented. Currently standalone support exist for the QMI8658c but not the AK09918. There is a library that supports
 * the AK09918 im conjuction with another IMU so it should be possible to extract the functions and rebase a library with it.
 */
// #define MAG_ADDR 0xc0
// #define GYRO_ADDR 0x6b


/* Power Monitoring - An INA219 allows for monitoring of the overall voltage and current consumed. Not ideal for working with Li-ion batteries but
 * this is better than nothing so to prevent any cell from dropping below voltage we should set a safety margin. 
 */
// #define INA219_ADDR 0X42

/* Pressure/Temperature - A BMP280 is shown in the schematics but not used in any of the demo code but seperate testing confirms this is present and
 * works 
 */
// #define BMP_ADDR 0x70

/* General GPIO - Located on the single row pin header, provides a generic male pin header for interfacing with a select number of GPIO, use with caution
 * as some of the IO Could already be used elsewhere such as for the encoders if enabled above. 
 */
// #defne GPIO0 4
// #define GPIO1 5
// #define GPIO2 16
// #define GPIO3 27

