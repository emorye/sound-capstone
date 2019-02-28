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
  speaker = new Speaker();
  radio = new Radio();
  
  speaker->init();
  oled->init();
  radio->init();
  
  // Speaker Stuff
  speaker->setVolume(70);
  speaker->play("TRACK002.mp3");
  speaker->midi();
  speaker->zeldasLullaby(127);

  // Arbitrarily turn on the LED boi.
  pinMode(LED_PIN, OUTPUT);

  
}

void Main::loop() {
  
  digitalWrite(LED_PIN, HIGH);
  oled->present("Hello!\n\n\nLoop: " + String(i++));

  if(radio->poll()){
    uint8_t buf [RH_RF69_MAX_MESSAGE_LEN] = {0};
    radio->rx(buf, sizeof(buf));
    speaker->zeldasLullaby(127);
  }

}

Main::~Main() {

}
