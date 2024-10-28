/* Display - Integrated into the chassis is an SSD1306 oled i2c driven display. This is connected to the control board via
 * the i2c header. The monochromatic display provides a simple and quick way display core stats about the UGV.
 */

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "power_sys.h"


#define DISP_ADDR   0x3c
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void testDisplay(){
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.println("Hello, world!");
  display.display();

}

void displayPowerData(powerData *data){

  float v_supply = data->volt_bus;
  float curr_supply = data->current_mA;
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(F("UGV control V0.2"));
  display.setCursor(0, 10);
  display.print(F("volts:"));
  display.setCursor(80,10);
  display.print(v_supply);
  display.setCursor(0, 20);
  display.print(F("Current(mA):"));
  display.setCursor(75,20);
  display.print(curr_supply);
  display.display();

}

