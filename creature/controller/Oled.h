#ifndef _OLED_H_
#define _OLED_H_

#include <Arduino.h>
#include <Adafruit_FeatherOLED.h> // Adafruit Feather OLED BSP

#define OLED_WIDTH 21 // Width of the OLED in chars
#define OLED_HEIGHT 4 // Height of the OLED in chars


class Oled {
  public:
    Oled(); // Oled Constructor. Doesn't do anything atm.
    ~Oled(); // Oled Destructor. Also doesn't do anything atm.
    
    // Initialize the OLED Display
    void init();

    // Display some text. new line using \n, four line capacity.
    void present(String text);
    
  private:
    Adafruit_FeatherOLED oled = Adafruit_FeatherOLED(); // Adafruit OLED Helper Class.

};
#endif  // _OLED_H_
