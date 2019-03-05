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
//  oled->present("Controller!\n\n\nLoop: " + String(i++));

  if(Serial.available()){
    String cmd = Serial.readStringUntil(' ');
//    oled->present(cmd);
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
    if(cmd.equals("pressnote")){
      //int channel, int pitch, int velocity
      uint8_t msg[4] = {4, 0, 0, 0};
      msg[1] = Serial.readStringUntil(' ').toInt();
      msg[2] = Serial.readStringUntil(' ').toInt();
      msg[3] = Serial.readStringUntil('\n').toInt();
       oled->present(String(msg[0]) + ' ' + String(msg[1]) + ' ' + String(msg[2]) + ' ' + String(msg[3]));
      radio->tx(msg, 4);
    }
  }


}

Main::~Main() {

}
