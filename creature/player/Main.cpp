#include "Main.h"
#include "Debug.h"

#include <cmath>


Main::Main() {
  i = 0;
}

void Main::setup() {
  Serial.begin(115200);
  delay(3000);
  
  Serial.print(F("Initializing"));

  delay(100);
  pinMode(LED_PIN, OUTPUT);

  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  oled.display();
  oled.clearDisplay();
  oled.init();
  oled.setBatteryVisible(false);
  updateDisplay();
}

void Main::loop() {
  
  digitalWrite(LED_PIN, HIGH);
  
  updateDisplay();
}

void Main::updateDisplay() {
  
  oled.clearDisplay();

  oled.setCursor(0, 0);
  oled.print("Hello!");
  oled.setCursor(0, 24);
  oled.print("Tick " + String(i++));
  oled.display();
}

Main::~Main() {

}
