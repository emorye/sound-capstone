#ifndef _RADIO_H_
#define _RADIO_H_

#include <Arduino.h>

#include <RH_RF69.h> // RFM69 BSP

#define RFM69_FREQ 915.0 // Tx/Rx at 915 Mhz.
#define RFM69_CS 8 // Chip Select?
#define RFM69_INT 3 // RFM69 Interrupt pin
#define RFM69_RST 4 // RST Pin

class Radio {
  public:
    Radio(); // Radio Constructor. Doesn't do anything atm.
    ~Radio(); // Radio Destructor. Also doesn't do anything atm.
    
    // Initialize the Radio
    void init();

    // Change sync words
    void resync(const uint8_t *syncword);

    // Send a message over the radio
    void tx(uint8_t *msg, uint8_t len);

    // Receive a message
    bool poll();
    void rx(uint8_t *rx_str, uint8_t len);
    
  private:
    RH_RF69 rf69 = RH_RF69(RFM69_CS, RFM69_INT);

};
#endif  // _RADIO_H_
