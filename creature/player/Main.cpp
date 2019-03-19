#include "Main.h"
#include "Debug.h"

Main::Main() {
  i = 0;
  txp = 0;
  rxp = 0;
}

void Main::setup() {

  // Init Serial and wait for it to finish initializing
  Serial.begin(50000);
  delay(1000);

  oled = new Oled();
  speaker = new Speaker();
  radio = new Radio();
  
  speaker->init();
  oled->init();
  
  radio->init();
  radio->resync(sw[controllernum]);
  
  // Speaker Stuff
  speaker->setVolume(0);
  speaker->play("TRACK002.mp3");
  speaker->midi();
//  speaker->zeldasLullaby(127);
  oled->present("Creature Num: " + String(controllernum) + "\nVersion: " + String(_version) + "\nSyncword: 0x" + String(sw[controllernum][0], HEX) + ", 0x" + String(sw[controllernum][1], HEX));
  
}

void Main::loop() {

   // oled->present("Hello!\nSyncwords: {0x" + String(syncwords[0], HEX) + ", 0x" + String(syncwords[1], HEX) +"}\nRX: "+ String(rxp) + "\nLoop: " + String(i++));

  if(radio->poll()){
    uint8_t buf [RH_RF69_MAX_MESSAGE_LEN] = {0};
    radio->rx(buf, sizeof(buf));
    rxp++;
    if(buf[0] == 69) speaker->zeldasLullaby(127);
    if(buf[0] == 70) speaker->sineTest();
    if(buf[0] == 4) speaker->pressNote(buf[1], buf[2], buf[3]);
    if(buf[0] == 5) speaker->releaseNote(buf[1], buf[2], buf[3]);
    if(buf[0] == 6) speaker->midiRawCancel();
    if(buf[0] == 99){
//      oled->present(String(errct) + " " + String(buf[1]-errct));
      if(buf[1]-errct > 1){
        oled->present("Err" + String(errct));
        speaker->midiRawCancel();
      }
      if(buf[1] != errct){
        uint8_t len = buf[2];
        uint8_t ptr = 3;
        for (int i = 0; i < len; i++){
          uint8_t ilen = buf[ptr++];
          speaker->midiRaw(&(buf[ptr]), ilen); 
          ptr += ilen;
          errct = buf[1];
        }

      }
    }

  }

}

Main::~Main() {

}
