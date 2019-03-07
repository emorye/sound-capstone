#ifndef _SPEAKER_H_
#define _SPEAKER_H_

#include <Arduino.h>
#include <Adafruit_VS1053.h>

#define VS1053_RESET   -1     // VS1053 reset pin (not used!)
#define VS1053_CS       6     // VS1053 chip select pin (output)
#define VS1053_DCS     10     // VS1053 Data/command select pin (output)
#define CARDCS          5     // Card chip select pin
#define VS1053_DREQ     9     // VS1053 Data request, ideally an Interrupt pin

#define MIDI_NOTE_ON  0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_CHAN_MSG 0xB0
#define MIDI_CHAN_BANK 0x00
#define MIDI_CHAN_VOLUME 0x07
#define MIDI_CHAN_PROGRAM 0xC0


// This is some patch to set vs10xx to midi mode.
const unsigned short sVS1053b_Realtime_MIDI_Plugin[28] = { /* Compressed plugin */
  0x0007, 0x0001, 0x8050, 0x0006, 0x0014, 0x0030, 0x0715, 0xb080, /*    0 */
  0x3400, 0x0007, 0x9255, 0x3d00, 0x0024, 0x0030, 0x0295, 0x6890, /*    8 */
  0x3400, 0x0030, 0x0495, 0x3d00, 0x0024, 0x2908, 0x4d40, 0x0030, /*   10 */
  0x0200, 0x000a, 0x0001, 0x0050,
};

#define VS1053_MIDI Serial1

class Speaker {
  public:
    Speaker(); // Radio Constructor. Doesn't do anything atm.
    ~Speaker(); // Radio Destructor. Also doesn't do anything atm.
    
    // Initialize the Speaker
    int init();

    // Play an MP3 in blocking mode.
    // Waits until current MP3 is finished (unconfirmed).
    void play(const char *songFile);

    // wait till current song is done playing;
    void wait();

    // Set the volume 0 to 254 higher is quieter.
    void setVolume(int vol);

    // Switch to midi mode and mp3 mode respectively
    void midi();
    void mp3();
    
    // Set the midi instrument, and its volume
    void setInstrument(int channel, int instrument);
    void setInstrumentVolume(int channel, int volume);
    
    // Play a note, midi pitch, duration in ms, velocity 0-127, higher numbers louder.
    void playNote(int channel, int pitch, int velocity, int duration);

    void pressNote(int channel, int pitch, int velocity);
    void releaseNote(int channel, int pitch, int velocity);

    // Send a raw midi command
    void midiRaw(const uint8_t *msg, uint8_t len);
    
    // Example to play Zelda's Lullaby
    void zeldasLullaby(int velocity);

    // Test sine beep
    void sineTest();

    
  private:
    Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ, CARDCS);
    bool midiMode;
    int lastIntstrument;
    void midiCmd(uint8_t bit1, uint8_t bit2, uint8_t bit3);

    

};
#endif  // _RADIO_H_
