
#include "Speaker.h"

//void sciWrite(uint8_t addr, uint16_t data); 
//
// Plugin to put VS10XX into realtime MIDI mode
// Originally from http://www.vlsi.fi/fileadmin/software/VS10XX/vs1053b-rtmidistart.zip
// Permission to reproduce here granted by VLSI solution.
//

void midiCmd(uint8_t bit1, uint8_t bit2, uint8_t bit3) {
  VS1053_MIDI.write(bit1);
  VS1053_MIDI.write(bit2);  
  VS1053_MIDI.write(bit3);
}


Speaker::Speaker(){
  midiMode = false;
}

int Speaker::init(){
  
    pinMode(8, INPUT_PULLUP); // This is required for some reason.

    VS1053_MIDI.begin(31250);
    delay(1000);
        
    if (!musicPlayer.begin()) { // initialise the music player
       return 0; // Failed to init music player.
    }
    if (!SD.begin(CARDCS)) {
      return 0; // SD Card not inserted or missing. 
    }
    
    musicPlayer.softReset(); // Reset from the last time the speaker was used.

    musicPlayer.sineTest(0x44, 5); // Play a tetst sound

    musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int

    midiMode = false; // We are not yet in midi mode. This is just for us to keep track
    
    return 1; // Successfully init'd music player;
}

void Speaker::play(const char *songFile){
  if(midiMode) mp3();
  
  wait(); // Wait for previous to finish;
  
  // Then play the file
  musicPlayer.playFullFile(songFile);
}

void Speaker::wait(){
  if(midiMode) {
    mp3();
    return;
  }
  while (!musicPlayer.stopped());
}

void Speaker::setVolume(int volume){
  musicPlayer.setVolume(volume, volume); // Set the volume low by default. Valid values from 0 to 254, lower numbers louder.
}

void Speaker::midi(){
  midiMode = true;
  
  int i = 0;

  while (i<sizeof(sVS1053b_Realtime_MIDI_Plugin)/sizeof(sVS1053b_Realtime_MIDI_Plugin[0])) {
    unsigned short addr, n, val;
    addr = sVS1053b_Realtime_MIDI_Plugin[i++];
    n = sVS1053b_Realtime_MIDI_Plugin[i++];
    while (n--) {
      val = sVS1053b_Realtime_MIDI_Plugin[i++];
      musicPlayer.sciWrite(addr, val);
    }
  }
}

void Speaker::mp3(){
  midiMode = false;
  musicPlayer.softReset();
}

void Speaker::setInstrument(int channel, int instrument){
    instrument--; // page 32 has instruments starting with 1 not 0 :(
  
    VS1053_MIDI.write(MIDI_CHAN_PROGRAM | channel);  
    delay(10);
    VS1053_MIDI.write(instrument);
    delay(10);
}

void Speaker::setInstrumentVolume(int channel, int volume){
  midiCmd(MIDI_CHAN_MSG | channel, MIDI_CHAN_VOLUME, volume);
}

void Speaker::pressNote(int channel, int pitch, int velocity){
  midiCmd(MIDI_NOTE_ON | channel, pitch, velocity);
}

void Speaker::releaseNote(int channel, int pitch, int velocity){
  midiCmd(MIDI_NOTE_OFF | channel, pitch, velocity);
}
    
void Speaker::playNote(int channel, int pitch, int velocity, int duration){
  pressNote(channel, pitch, velocity);
  delay(duration);
  releaseNote(channel, pitch, velocity);
}

    
void Speaker::zeldasLullaby(int velocity){
  int notes[13] =     {71, 74, 69, 67, 69, 71, 74, 69, 71, 74, 81, 79, 74};
  int durations[13] = {1000, 500, 1000, 250, 250, 1000, 500, 1500, 1000, 500, 1000, 500, 1500};
  int len = 13;

  setInstrument(0, 80); //Ocarina
  
  for (uint8_t i = 0; i < len; i++) {
    playNote(0, notes[i], velocity, durations[i]);
  }

}



Speaker::~Speaker() {
}
