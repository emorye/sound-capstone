#include "Main.h"
#include "Debug.h"



Main::Main() {
  i = 0;
}

void Main::setup() {

  // Init Serial and wait for it to finish initializing
  Serial.begin(115200);
  delay(1000);

  oled = new Oled();
  radio = new Radio();
  
  oled->init();
  radio->init();


}

void Main::loop() {
  
  digitalWrite(LED_PIN, HIGH);
  oled->present("Hello!\n\n\nLoop: " + String(i++));

  if(Serial.available()){
    String cmd = Serial.readStringUntil('\n');
    cmd.toLowerCase();
    if(cmd.equals("zelda")){
      // Send some arbitrary packet
      uint8_t msg[1] = {69};
      radio->tx(msg, 1);
    }
    if(cmd.equals("sine")){
      // Send some arbitrary packet
      uint8_t msg[1] = {70};
      radio->tx(msg, 1);
    }
  }


}

Main::~Main() {

}
