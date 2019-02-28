#include "Creature.h"
#include "Debug.h"
#include "State.h"
#include "Controller.h"

#include <cmath>

// TODO: put your kit number here
#define KIT_NUM 0

// Returns current battery voltage
inline float getBatteryVoltage() {
  // Update battery field with a little weighting to stop rapid changes.
  return analogRead(VBAT_PIN)  * 2 * VREF / 1024;
}

Creature::Creature() {
  // Initialize _next to be the Controller state, so we will immediately transition into it on the first loop.
  _next = new Controller(*this);
  _state = nullptr;

  pinMode(ID_PIN, INPUT_PULLUP);
  _kitNum = 0;
  _addr = 0;

  // Parens zero initialize
  _creatureDistances = new int8_t[GLOBALS.NUM_CREATURES + 1]();
  _creatureStates = new uint8_t[GLOBALS.NUM_CREATURES + 1]();
  _creatureTimes = new unsigned long[GLOBALS.NUM_CREATURES + 1]();
  
  _battery = getBatteryVoltage();

  _lastStartle = millis();
  _lastLoop = millis();

//  Adafruit_BluefruitLE_SPI a = ;
}

int Creature::_get_int() {
  String modeStr = Serial.available() ? Serial.readStringUntil(' ') : "00";
  char modeCharStr[modeStr.length()];
  modeStr.toCharArray(modeCharStr, modeStr.length() + 1);
  
  int i1 = (int) strtol(modeCharStr, NULL, 16);
  int i2 = (int) strtol(modeCharStr, NULL, 0);

  return i1 > i2 ? i1 : i2;
}

void Creature::_initializeBluetooth() {
  if (!_blu.begin(false)) { // verbose
    dprintln(F("Failed to intialize BLUEFRUIT module"));
    while (1);
  }
  //_blu.echo(false);

  /*if (!_blu.factoryReset()) {
    dprintln(F("Could not factory reset"));
    while (1);
  }
  Serial.print("D");
  dprintln(F("BLUEFRUIT info:"));
  //#ifdef DEBUG
  _blu.info();
  //#endif
  Serial.print("E");
  
  Serial.print("F");*/
  // #ifdef DEBUG
  // _blu.info();
  // #endif
  _blu.verbose(false);
  _blu.setMode(BLUEFRUIT_MODE_DATA);
}

void Creature::_processCommand() {
  if (Serial.available()) {
    String str = Serial.readStringUntil(' ');
    str.toLowerCase();
    if (str.equals("stop")) {
      if (Serial.available()) {
        uint8_t dst = (uint8_t) Serial.parseInt();
        if (dst > 0) {
          if (!tx(PID_STOP, _addr, dst, 0, nullptr)) {
            Serial.print("ERROR ");
          }
          Serial.print(dst == 255 ? "Stopping all creatures" : "Stopping creature #");
          if (dst != 255) {
            Serial.print(dst);
          }
          Serial.println();
        }
      }
    } else if (str.equals("start")) {
      if (Serial.available()) {
        uint8_t dst = (uint8_t) Serial.parseInt();
        if (dst > 0) {
          Serial.readStringUntil(' ');
          uint8_t mode = (uint8_t) _get_int();
          uint8_t state = (uint8_t) _get_int();
          uint8_t pld[2];
          pld[0] = mode;
          pld[1] = state;
          bool res = tx(PID_START, _addr, dst, 2, pld);

          if (!res) {
            Serial.print("ERROR ");
          }
          Serial.print(dst == 255 ? "Starting all creatures" : "Starting creature #");
          if (dst != 255) {
            Serial.print(dst);
          }
          Serial.print(" with mode 0x");
          Serial.print(mode, HEX);
          Serial.print(" and state");
          Serial.println(state, HEX);
          Serial.println();
        }
      }
    } else if (str.equals("sound")) {
      if (Serial.available()) {
        uint8_t dst = (uint8_t) Serial.parseInt();
        if (dst > 0) {
          Serial.readStringUntil(' ');
          uint8_t sound = (uint8_t) _get_int();
          bool res = tx(PID_PLAY_SOUND, _addr, dst, 1, &sound);

          if (!res) {
            Serial.print("ERROR ");
          }
          Serial.print(dst == 255 ? "Broadcasting to all creatures" : "Telling creature #");
          if (dst != 255) {
            Serial.print(dst);
          }
          Serial.print(" to play sound 0x");
          Serial.println(sound, HEX);
          Serial.println();
        }
      }
    } else if (str.equals("effect")) {
      if (Serial.available()) {
       uint8_t dst = (uint8_t) Serial.parseInt();
        if (dst > 0) {
          Serial.readStringUntil(' ');
          uint8_t effect = (uint8_t) _get_int();

          bool res = tx(PID_PLAY_EFFECT, _addr, dst, 1, &effect);

          if (!res) {
            Serial.print("ERROR ");
          }
          Serial.print(dst == 255 ? "Broadcasting to all creatures" : "Telling creature #");
          if (dst != 255) {
            Serial.print(dst);
          }
          Serial.print(" to play effect 0x");
          Serial.println(effect, HEX);
          Serial.println();
        }
      }
    } else if (str.equals("set")) {
      if (Serial.available()) {
        uint8_t globalIndex = (uint8_t) Serial.parseInt();
        if (globalIndex >= 0 && globalIndex < GLOBALS_LEN) {
          Serial.readStringUntil(' ');

          if (!Serial.available()) {
            Serial.print("Global index #");
            Serial.print(globalIndex);
            Serial.print(" == ");
            Serial.println(GLOBALS_NAMES[globalIndex]);
          } else {
            Serial.print("Adjusting global ");
            Serial.print(GLOBALS_NAMES[globalIndex]);
            Serial.print(" from ");
            void* ptr = (((char*)&GLOBALS) + GLOBALS_OFFSETS[globalIndex]);
          
            if (globalIndex == 7) {
              Serial.print(*(float*) ptr);
              Serial.print(" to ");
              float val = Serial.parseFloat();
              *(float*) ptr = val;
              Serial.println(*(float*) ptr);
            } else if (globalIndex == 8 || globalIndex == 0) {
              Serial.print(*(uint16_t*) ptr);
              Serial.print(" to ");
              uint16_t val = (uint16_t) Serial.parseInt();
              *(uint16_t*) ptr = val;
              Serial.println(*(uint16_t*) ptr);
            }else {
              Serial.print(*(uint8_t*) ptr);
              Serial.print(" to ");
              uint8_t val = (uint8_t) Serial.parseInt();
              if (globalIndex == 6) {
                _updateGlobals(*(uint8_t*) ptr, val);
              }
              *(uint8_t*) ptr = val;
              Serial.println(*(uint8_t*) ptr);
            }
          }
          Serial.println();
        } else {
          Serial.print("ERROR, globals index ");
          Serial.print(globalIndex);
          Serial.print(" is greater than max index ");
          Serial.println(GLOBALS_LEN - 1);
          Serial.println();
        }
      }
    } else if (str.equals("packet")) {
      if (Serial.available()) {
        uint8_t pid = (uint8_t) Serial.parseInt();
        uint8_t src = (uint8_t) Serial.parseInt();
        uint8_t dst = (uint8_t) Serial.parseInt();
        Serial.readStringUntil(' ');
        
        uint8_t pld[RH_RF69_MAX_MESSAGE_LEN];
        uint8_t sz = 0;
        while (Serial.available() && sz < RH_RF69_MAX_MESSAGE_LEN) {
          pld[sz] = _get_int();
          sz++;
        }

        bool res = tx(pid, src, dst, sz, pld);
      }
    } else if (str.equals("update")) {
       tx(0, _addr, 0xFF, sizeof(GLOBALS), (uint8_t*)&GLOBALS);
    } else if (str.equals("states")) {
      unsigned long now = millis();
      for (int i = 1; i < GLOBALS.NUM_CREATURES + 1; i++) {
        Serial.print("Creature ");
        Serial.print(i);
        Serial.print("\tState ");
        _creatureTimes[i] ? Serial.print("0x") : Serial.print("");
        _creatureTimes[i] ? Serial.print(_creatureStates[i], HEX) : Serial.print("N/A");
        Serial.print("\tLast Contact: ");
        _creatureTimes[i] ? Serial.print(now - _creatureTimes[i]) : Serial.print("N/A");
        Serial.println(_creatureTimes[i] ? "ms" : "");
      }
      Serial.println();
    } else if (str.equals("globals")) {

      Serial.print("0\t TX_POWER                    ==   ");
      Serial.println(GLOBALS.TX_POWER);

      Serial.print("1\t STARTLE_RAND_MIN            ==   ");
      Serial.println(GLOBALS.STARTLE_RAND_MIN);

      Serial.print("2\t STARTLE_RAND_MAX            ==   ");
      Serial.println(GLOBALS.STARTLE_RAND_MAX);

      Serial.print("3\t STARTLE_MAX                 ==   ");
      Serial.println(GLOBALS.STARTLE_MAX);

      Serial.print("4\t STARTLE_THRESHOLD           ==   ");
      Serial.println(GLOBALS.STARTLE_THRESHOLD);

      Serial.print("5\t STARTLE_DECAY               ==   ");
      Serial.println(GLOBALS.STARTLE_DECAY);

      Serial.print("6\t NUM_CREATURES               ==   ");
      Serial.println(GLOBALS.NUM_CREATURES);

      Serial.print("7\t STARTLE_THRESHOLD_DECAY     ==   ");
      Serial.println(GLOBALS.STARTLE_THRESHOLD_DECAY);

      Serial.print("8\t CYCLE_TIME                  ==   ");
      Serial.println(GLOBALS.CYCLE_TIME);

      Serial.println();

    } else if (str.equals("help")) {

      Serial.println("globals");
      Serial.println("   Prints the current global values\n");

      Serial.println("states");
      Serial.println("   Prints the current creature states & time since last transmission\n");

      Serial.println("update");
      Serial.println("   Force transmits globals to all creatures\n");
      
      Serial.println("stop <creature number>");
      Serial.println("   Stops the given creature\n");

      Serial.println("start <creature number> <mode> <state>");
      Serial.println("   Starts the given creature into the given mode (0x00 for specified state, 0x01 for continue) and state (0x00 for random)\n");

      Serial.println("sound <creature number> <sound number>");
      Serial.println("   Commands the given creature to play the specified sound\n");

      Serial.println("effect <creature number> <effect number>");
      Serial.println("   Commands the given creature to play the specified effect\n");

      Serial.println("set <global index> <value>");
      Serial.println("\tSets the specified global to the given value\n");

      Serial.println("packet <byte> ... <byte>");
      Serial.println("\tSends a given packet\n");

      Serial.println();
    } else {
      Serial.println("ERROR: INVALID COMMAND");
      Serial.println(str);
    }
    Serial.readString();
  }
}

void Creature::_updateGlobals(uint8_t sizeOld, uint8_t sizeNew) {
  int8_t* newDistances = new int8_t[sizeNew + 1]();
  uint8_t* newStates = new uint8_t[sizeNew + 1]();
  unsigned long* newTimes = new unsigned long[sizeNew + 1]();

  int overlap = sizeOld > sizeNew ? sizeNew : sizeOld;
  for (int i = 0; i < overlap; i++) {
    newDistances[i] = _creatureDistances[i];
    newStates[i] = _creatureStates[i];
    newTimes[i] = _creatureTimes[i];
  }

  delete[] _creatureDistances;
  delete[] _creatureStates;
  delete[] _creatureTimes;

  _creatureDistances = newDistances;
  _creatureStates = newStates;
  _creatureTimes = newTimes;
}


void Creature::_updateCreature(uint8_t id) {
  if (id < GLOBALS.NUM_CREATURES) {
    _creatureTimes[id] = millis();
  }
}

void Creature::loop() {
  unsigned long thisLoop = millis();
  unsigned long dt = thisLoop - _lastLoop;

  _pollRadio();
  _processCommand();

  // Only trigger state loops and transitions every CYCLE_TIME ms
  if (dt > CONT_CYCLE_TIME) {
    for (int i = 1; i < GLOBALS.NUM_CREATURES + 1; i++) {
      if (thisLoop - _creatureTimes[i] > 60000) {
        _creatureTimes[i] = 0;
        _creatureStates[i] = 0;
      }
    }
    
    _updateDisplay();

    if (_next != NULL) {
      // We have a predefined next state, transition immediately
      _transition(_next);
      _next = nullptr;
    } else if (_remainingRepeats > 0) {
      // This state should be repeated more. Call loop and decrement
      _state->loop(dt);
      _remainingRepeats--;
    } else {
      // We're out of repeats, do a transition
      _transition(_state->transition());
      _state->loop(dt);
    }

    _lastLoop = thisLoop;
  }
}

bool Creature::_rx(uint8_t pid, uint8_t srcAddr, uint8_t len, uint8_t* payload, int8_t rssi) {
  _rxCount++;
  _updateDistance(srcAddr, rssi);

  _updateCreature(srcAddr);

  switch (pid) {
    case PID_SET_GLOBALS:
      return true;
    case PID_STOP:
      return true;
    case PID_START:
      return true;
    case PID_PLAY_SOUND:
      return true;
    case PID_PLAY_EFFECT:
      return true;
    case PID_BROADCAST_STATES:
      return true;
    case PID_STARTLE:
      return true;
    case PID_SEND_STATE:
      _receiveState(srcAddr, payload[0]);
      return true;
    default:
      Serial.print(F("Received packet of unknown type: "));
      Serial.print(pid, HEX);
      return false;
  }
}

void Creature::_updateDistance(uint8_t addr, int8_t rssi) {
}

bool Creature::_rxSetGlobals(uint8_t len, uint8_t* payload) {
  return true;
}

void Creature::_rxStop() {
}

bool Creature::_rxStart(uint8_t len, uint8_t* payload) {
  return true;
}

bool Creature::_rxBroadcastStates(uint8_t len, uint8_t* payload) {
  return true;
}

bool Creature::tx(const uint8_t pid, const uint8_t src_addr, const uint8_t dst_addr, const uint8_t len, uint8_t* const payload) {
  if (len + 3 > RH_RF69_MAX_MESSAGE_LEN) {
    Serial.print(F("Packet length "));
    Serial.print(len);
    Serial.print(F(" bytes is longer than max of "));
    Serial.print(RH_RF69_MAX_MESSAGE_LEN - 3);
    Serial.println(F(" bytes"));
    return false;
  }

  _buf[0] = pid;
  _buf[1] = src_addr;
  _buf[2] = dst_addr;
  memcpy(_buf + 3, payload, len);

  Serial.print(F("Sending pid 0x"));
  if (pid < 16) {
    dprint('0');
  }
  Serial.print(pid, HEX);
  Serial.print(F(" with "));
  Serial.print(len);
  Serial.print(F(" bytes to address "));
  Serial.print(dst_addr);
  Serial.print(F(" from address "));
  Serial.println(src_addr);

  for (uint8_t* i = &_buf[3]; i < &_buf[len + 3]; i++) {
    if (*i < 16) {
      dprint('0');
    }
    dprint(*i, HEX);
    dprint(' ');
  }
  dprintln();

  if (_buf[len + 2] == 0) {
    // NULL terminated, print as string too.
    dprintln((char*) &_buf[3]);
  }
  dprintln();

  _rf69.send((const uint8_t*) &_buf, len + 3);
  _rf69.waitPacketSent();
  _txCount++;
  return true;
}

void Creature::_txSendState(uint8_t oldState, uint8_t newState) {
  uint8_t payload[] = {oldState, newState};
  tx(PID_SEND_STATE, _addr, CONTROLLER_ADDR, sizeof(payload), payload);
}

void Creature::_receiveState(uint8_t src, uint8_t st) {
  if (src < GLOBALS.NUM_CREATURES) {
    _creatureStates[src] = st;
  }
}

void Creature::_pollRadio() {
  // Poll radio for packets
  if (_rf69.waitAvailableTimeout(100)) {
    uint8_t len = sizeof(_buf);

    // Should be a message available
    if (!_rf69.recv(_buf, &len)) {
      Serial.println("Failed to receive packet");
      return;
    }

    uint8_t pid, srcAddr, dstAddr;
    pid = _buf[0];
    srcAddr = _buf[1];
    dstAddr = _buf[2];

    Serial.print(F("Received pid 0x"));
    if (pid < 16) {
      dprint('0');
    }
    Serial.print(pid, HEX);
    Serial.print(F(" with "));
    Serial.print(len - 3);
    Serial.print(F(" bytes from address "));
    Serial.print(srcAddr);
    Serial.print(F(" for address "));
    Serial.println(dstAddr);

    for (uint8_t* i = &_buf[3]; i < &_buf[len]; i++) {
      if (*i < 16) {
        dprint('0');
      }
      dprint(*i, HEX);
      dprint(' ');
    }
    dprintln();
    if (_buf[len - 1] == 0) {
      dprintln((char*) &_buf[3]);
    }

    Serial.println();
    Serial.println();

    if (dstAddr == _addr || dstAddr == BROADCAST_ADDR) {
      if (!_rx(pid, srcAddr, len - 3, &_buf[3], _rf69.lastRssi())) {
        Serial.println(F("Packet not handled correctly. Payload dump:"));
        for (uint8_t* i = &_buf[3]; i < &_buf[len]; i++) {
          Serial.print(*i, HEX);
          Serial.print(' ');
        }
      }
    }
  }
}

void Creature::_transition(State* const state) {
  State* const old = _state;
  _state = state;

  if (state != old) {
    if (old != nullptr) {
      delete old;
      _txSendState(old->getId(), state->getId());
    } else {
      _txSendState(0, state->getId());
    }
  }

  _remainingRepeats = _state->getNumRepeats();
}

void Creature::_updateDisplay() {
  _battery = 0.95 * _battery + 0.05 * getBatteryVoltage();

  oled.clearDisplay();
  oled.setBattery(_battery);
  oled.renderBattery();

  oled.setCursor(0, 0);
  oled.print(F("TXRX:"));
  oled.print(_txCount);
  oled.print(F("/"));
  oled.println(_rxCount);

  oled.setCursor(0, 9);
  oled.print(_state ? _state->getName() : "None");

  oled.setCursor((OLED_WIDTH - 3) * 6, 9);
  oled.print("#");
  oled.print(_addr);

  oled.display();
}

void Creature::setup() {
  Serial.print(F("Booting kit: "));
  Serial.println(KIT_NUM);
  Serial.print(F("Address: "));
  Serial.println(_addr);

  delay(100);
  pinMode(LED_PIN, OUTPUT);

  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  oled.display();
  oled.clearDisplay();
  oled.init();
  oled.setBatteryVisible(true);
  _updateDisplay();

  strip.begin();
  strip.setBrightness(5);
  strip.show();

  _initializeBluetooth();

  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);
  // Manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!_rf69.init() || !_rf69.setFrequency(RFM69_FREQ)) {
    Serial.println(F("RFM69 radio init failed"));
    while (1);
  }
  _rf69.setTxPower(GLOBALS.TX_POWER, true);

  uint8_t key[] = {0x98, 0xe8, 0xac, 0xe6, 0xfa, 0xca, 0xc1, 0xb8,
                   0xf7, 0xf2, 0x18, 0xc3, 0x5c, 0xce, 0x96, 0x65};
  _rf69.setEncryptionKey(key);

  Serial.print(F("RFM69 radio @ "));
  Serial.print(RFM69_FREQ);
  Serial.println(F("MHz"));
}

Creature::~Creature() {
  delete[] _creatureDistances;
  delete[] _creatureStates;
  delete[] _creatureTimes;
}
