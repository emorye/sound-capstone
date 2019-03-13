#include "Main.h"
#include "Debug.h"



Main::Main() {
  i = 0;
}

void Main::setup() {

  // Init Serial and wait for it to finish initializing
  Serial.begin(500000);
  delay(1000);

  oled = new Oled();
  radio = new Radio();
  
  oled->init();
  radio->init();

  radio->resync(syncwords);

  oled->present("Starting Controller");
}

void Main::loop() {
  
  digitalWrite(LED_PIN, HIGH);
  // oled->present("Controller!\n\n\nLoop: " + String(i++));

  while(Serial.available()){
    String cmd = Serial.readStringUntil(' ');
    // oled->present(cmd);
    cmd.toLowerCase();
    if(cmd.equals("raw")){
      //int channel, int pitch, int velocity
      int client = Serial.readStringUntil(' ').toInt(); // Number of messages
      int len = Serial.readStringUntil(' ').toInt(); // Number of messages
      uint8_t msg[60] = {0}; // Message to send Bufer
      msg[0] = 99; // First byte = header
      msg[1] = (errcts[client])++;
      msg[2] = len; // Second byte = overall length
      int ptr = 3; // Pointer to next byte to fill
      for (int i= 0; i < len; i++) {
        int ilen = Serial.readStringUntil(' ').toInt(); // length of segment
        msg[ptr++] = ilen;
        for (int j =0; j < ilen; j++) {
          msg[ptr++] = Serial.readStringUntil(' ').toInt();
        }
      }
      String x;
      for (int i = 0; i < 15; i++){
        x += String(msg[i]) + " ";
      }
      if (client != lastClient){
        radio->resync(sw[client]);
        lastClient = client;
      }
      oled->present(String(client) + " " + x);
      radio->tx(msg, ptr);
    } if (cmd.equals("zero")) {
      uint8_t msg[2] = {};
      msg[0] = 6;
      for (int i = 0; i < 16; i++) {
        if (i != lastClient){
          radio->resync(sw[i]);
          lastClient = i;
        }
        radio->tx(msg, 1);
      }
      oled->present("Zero all");
    }
  }
}

Main::~Main() {

}
