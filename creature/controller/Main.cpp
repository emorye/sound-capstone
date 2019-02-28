#include "Main.h"
#include "Debug.h"



Main::Main() {
  i = 0;
}

// Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ, CARDCS);

void Main::setup() {

  // Init Serial and wait for it to finish initializing
  Serial.begin(115200);
  delay(1000);

  oled = new Oled();
  radio = new Radio();
  
  oled->init();
  radio->init();
  

  // Arbitrarily turn on the LED boi.
  pinMode(LED_PIN, OUTPUT);

  uint8_t msg[2] = {1, 2};
  radio->tx(msg, 2);

  
}

void Main::loop() {
  
  digitalWrite(LED_PIN, HIGH);
  oled->present("Hello!\n\n\nLoop: " + String(i++));

}

Main::~Main() {

}
