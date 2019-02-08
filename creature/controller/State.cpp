#include "Creature.h"
#include "State.h"

State::State(Creature& creature, char* const name) : _creature(creature) {
  strncpy(_name, name, 16);
};

char* State::getName() {
  return _name;
}

void State::playSound(uint8_t sound_idx) {
  // TODO: implement
}

void State::playEffect(uint8_t effect_idx) {
  // TODO: implement
}

bool State::rxPlaySound(uint8_t len, uint8_t* payload) {
  // TODO: implement
}

bool State::rxPlayEffect(uint8_t len, uint8_t* payload) {
  // TODO: implement
}

void State::txStartle(uint8_t strength, uint8_t id) {
  // TODO: implement
}

State* State::transition() {
  // TODO: implement
}

uint8_t* State::getGlobalWeights() {
  return _globalWeights;
}

