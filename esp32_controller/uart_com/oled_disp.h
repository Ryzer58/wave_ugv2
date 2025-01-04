/* Display - Mounted into the UGV chassis is the SSD1306 oled display, communicating over i2c. This is connected via the i2c header on the controller board.
 * The monochromatic display provides a simple and quick way display core stats about the UGV.
 */

#ifndef OLED_DISP_H
#define OLED_DISP_H


#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "power_sys.h"


#define DISP_ADDR   0x3c
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void bootScreen(){
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(F("UGV02"));
  display.setTextSize(1);
  display.setCursor(0, 24);
  display.print(F("V0.6"));
  display.setCursor(28, 24);
  display.print(F("Wait on signal"));
  display.display();

}

void displayPowerData(powerData *data){

  float v_supply = data->volt_bus;
  float curr_supply = data->current_mA;
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(F("volts:"));
  display.setCursor(80,0);
  display.print(v_supply);
  display.setCursor(0, 8);
  display.print(F("Current(mA):"));
  display.setCursor(75,8);
  display.print(curr_supply);
  display.display();

}

void clearRow(int row){
  int cursor_val = 0;
  if (row == 1){
    cursor_val = 8;
  }

  else if (row == 2){
    cursor_val = 16;
  }

  else if (row == 3){
    cursor_val = 24;
  }

  display.setCursor(0, cursor_val);
  display.print(F("                    "));
}
 /* Todo - consider the best approuch for integrating temperature sensor data into our display data*/

#endif