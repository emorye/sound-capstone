#include "Radio.h"

Radio::Radio() {
}

  
void Radio::init(){
  if (!rf69.init())
    Serial.println("init failed");
  if (!rf69.setFrequency(RFM69_FREQ))
    Serial.println("setFrequency failed");
    
  rf69.setTxPower(14, true);

  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

  rf69.setEncryptionKey(key);
}

void Radio::tx(uint8_t *msg, uint8_t len){
  rf69.send(msg, len);
  rf69.waitPacketSent();
}

void Radio::rx(void (*service_routine)(uint8_t *rx_str, uint8_t len)){
  if (rf69.available()) {
    // Message available
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len)) {
      service_routine(buf, len);
    }
    else {
      Serial.println("recv failed");
    }
  }
}
    
Radio::~Radio() {
}
