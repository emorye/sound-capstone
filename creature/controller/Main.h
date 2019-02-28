#ifndef _MAIN_H_
#define _MAIN_H_

#include <cinttypes>
#include <Wire.h> // ?

#include "Oled.h"
#include "Speaker.h"

#define VREF 3.3 // ?

#define LED_PIN 13 // ?

#define CONT_CYCLE_TIME 1000 // ? 
#define CLEAR_TIME 60000 // ?

class Main {
 public:
  Main(); // Main Constructor. Doesn't do anything atm.
  ~Main(); // Main Destructor. Also doesn't do anything atm.

  int i;
  Oled *oled;
  Speaker *speaker;

  // Run after construction but before loop.
  void setup();

  // Called during main loop.
  void loop();

};

#endif  // _MAIN_H_
