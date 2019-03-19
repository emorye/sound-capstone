#include "Oled.h"

Oled::Oled() {
}

  
void Oled::init(){
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  oled.display();
  oled.clearDisplay();
  oled.init();
  oled.setBatteryVisible(false);
}

void Oled::present(String text) {
  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.print(text);
  oled.display();
}

Oled::~Oled() {
}
