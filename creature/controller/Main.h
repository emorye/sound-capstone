#ifndef _MAIN_H_
#define _MAIN_H_

#include <cinttypes>
#include <Wire.h> // ?

#include "Oled.h"
#include "Speaker.h"
#include "Radio.h"

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
  Radio *radio;
  uint8_t errcts[16] = {0};

  uint8_t sw[16][2] = {{0x11, 0x11}, {0x22, 0x22}, {0x33, 0x33},
  {0x44, 0x44},{0x55, 0x55},{0x66, 0x66},{0x77, 0x77},{0x88, 0x88},{0x99, 0x99},
  {0xAA, 0xAA},{0xBB, 0xBB},{0xCC, 0xCC},{0xDD, 0xDD},{0xEE, 0xEE},{0xFF, 0xFF}};

  uint8_t syncwords[2] = {0x11, 0x11};

  uint8_t lastClient = 0;

  // Run after construction but before loop.
  void setup();

  // Called during main loop.
  void loop();

};

#endif  // _MAIN_H_
