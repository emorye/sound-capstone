#include "Main.h"
#include "Debug.h"

Main::Main() {
  i = 0;
}

void Main::setup() {

  // Init Serial and wait for it to finish initializing
  Serial.begin(115200);
  delay(1000);

  // Arbitrarily turn on the LED boi.
  pinMode(LED_PIN, OUTPUT);

  oled.init();

}

void Main::loop() {
  
  digitalWrite(LED_PIN, HIGH);
  oled.present("Hello!\n\n\nLoop: " + String(i++));

}

Main::~Main() {

}
