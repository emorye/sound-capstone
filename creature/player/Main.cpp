#include "Main.h"
#include "Debug.h"

Main::Main() {
  i = 0;
  txp = 0;
  rxp = 0;
}

void Main::setup() {

  // Init Serial and wait for it to finish initializing
  Serial.begin(115200);
  delay(1000);

  oled = new Oled();
  speaker = new Speaker();
  radio = new Radio();
  
  speaker->init();
  oled->init();
  radio->init();
  
  // Speaker Stuff
  speaker->setVolume(0);
  speaker->play("TRACK002.mp3");
  speaker->midi();
  speaker->zeldasLullaby(127);
  
}

void Main::loop() {

  // oled->present("Hello!\n\nTX: " + String(txp) + " RX: "+ String(rxp) + "\nLoop: " + String(i++));

  if(radio->poll()){
    uint8_t buf [RH_RF69_MAX_MESSAGE_LEN] = {0};
    radio->rx(buf, sizeof(buf));
    rxp++;
    if(buf[0] == 69) speaker->zeldasLullaby(127);
    if(buf[0] == 70) speaker->sineTest();
    if(buf[0] == 4) speaker->pressNote(buf[1], buf[2], buf[3]);
    oled->present(String(buf[0]) + " "+String(buf[1]) + " "+String(buf[2]) + " "+String(buf[3]));
  }

}

Main::~Main() {

}
