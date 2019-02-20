#ifndef _MAIN_H_
#define _MAIN_H_

#include <cinttypes>

#include <Adafruit_FeatherOLED.h> // Adafruit Feather OLED BSP
#include <RH_RF69.h> // RFM69 BSP
#include <Wire.h> // ?

#define VREF 3.3

#define LED_PIN 13

#define OLED_WIDTH 21 // Width of the OLED in chars

#define RFM69_FREQ 915.0 // Tx/Rx at 915 Mhz.
#define RFM69_CS 8 // Chip Select?
#define RFM69_INT 3 // RFM69 Interrupt pin
#define RFM69_RST 4 // RST Pin

#define CONT_CYCLE_TIME 1000 // ? 
#define CLEAR_TIME 60000 // ?

class Main {
 public:
  Main(); // Main Constructor. Doesn't do anything atm.
  ~Main(); // Main Destructor. Also doesn't do anything atm.

  Adafruit_FeatherOLED oled = Adafruit_FeatherOLED(); // Adafruit OLED Helper Class.

  int i;

  // Run after construction but before loop.
  void setup();

  // Called during main loop.
  void loop();
  
 private:
  /** Measures battery voltage and updates OLED */
  void updateDisplay();

};

#endif  // _MAIN_H_
