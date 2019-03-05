#ifndef _MAIN_H_
#define _MAIN_H_

#include <cinttypes>
#include <Wire.h> // ?

#include "Oled.h"
#include "Speaker.h"
#include "Radio.h"

#define VREF 3.3

#define LED_PIN 13

#define OLED_WIDTH 21 // Width of the OLED in chars

#define CONT_CYCLE_TIME 1000 // ? 
#define CLEAR_TIME 60000 // ?

class Main {
 public:
  Main(); // Main Constructor. Doesn't do anything atm.
  ~Main(); // Main Destructor. Also doesn't do anything atm.

  Oled *oled;
  Speaker *speaker;
  Radio *radio;

  int i; // Loop number
  int txp; // Number of tx packets;
  int rxp; // Number of rx packets;

  // Run after construction but before loop.
  void setup();

  // Called during main loop.
  void loop();
  
 private:
  /** Measures battery voltage and updates OLED */
  void updateDisplay();

};

#endif  // _MAIN_H_
