#ifndef _MIDI_H_
#define _MIDI_H_

#include "stdbool.h"
#include <cinttypes>

#include "Sound.h"

#define VS1053_MIDI Serial1

// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 31
#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x79

// Add any extra instruments that you may need here.
// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 32 for more!
#define VS1053_GM1_OCARINA 80
#define VS1053_GM1_SLAP_BASS_1 36
#define VS1053_GM1_CELLO 43
#define VS1053_GM1_APPLAUSE 127

#define MIDI_NOTE_ON  0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_NOTE_ALL_OFF_1 0x7b
#define MIDI_NOTE_ALL_OFF_2 0x7f
#define MIDI_CHAN_MSG 0xB0
#define MIDI_CHAN_BANK 0x00
#define MIDI_CHAN_VOLUME 0x07
#define MIDI_CHAN_PROGRAM 0xC0

#define CPU_HZ 48000000
#define TIMER_PRESCALER_DIV 1000

void tcReset();
bool tcIsSyncing();

class Midi {
 private:
  static Sound m1;

 public:
  /** Array of all sounds, in a fixed order, used to assign an index to each. Add your sounds to this! */
  static constexpr Sound *SOUNDS[] =
      {nullptr,       // 00
       &m1,           // 1
     };

  /**
   * Use this to set the current sound gesture.
   *
   * @param soundIdx Index of new sound to play in Midi::SOUNDS.
   * @param loop Whether or not the sound should loop when finished.
   * @param transpose Amount each note should be shifted in the sound.
   * @param duration_offset Amount each note's duration should be shifted in the sound.
   * @param retrograde Whether or not the sound should be played in reverse.
   * @param instrument Instrument override or negative for default sound instrument.
   */
  static void setSound(uint8_t soundIdx, bool loop=false, uint8_t transpose=0, uint16_t duration_offset=0, bool retrograde=false, int16_t instrument=-1);

  /**
   * @returns the index of the current sound gesture.
   */
  static uint8_t getSoundIdx();

  /**
   * @returns the currently playing sound gesture.
   */
  static Sound* getSound();

  /**
   * @return true if the current sound should loop, false otherwise.
   */
  static bool loop();

  /**
   * @return true if the current sound should be played in reverse, false otherwise.
   */
  static bool retrograde();

  /**
   * @returns the instrument for the current sound gesture.
   */
  static uint8_t instrument();

  /**
   * @returns the note transpose for the current sound gesture.
   */
  static uint8_t transpose();

  /**
   * @returns the duration offset for the current sound gesture.
   */
  static uint16_t duration_offset();

  static void setup();
 private:
  /** Index of current sound being played. */
  volatile static uint8_t _currentIdx;

  /** Current sound being played. */
  static Sound* volatile _current;

  /** Whether or not the currently playing sound should loop. */
  volatile static bool _loop;

  /** Whether or not the current sound gesture should be played in reverse. */
  volatile static bool _retrograde;

  /** Instrument with which to play the current sound.. */
  volatile static uint8_t _instrument;

  /** Global transpose applied to all sounds, independent of a specific sound's transpose. */
  volatile static uint8_t _transpose;

  /** Global duration offset applied to all sounds, independent of a specific sound's duration offset.  */
  volatile static uint16_t _duration_offset;

  static void tcStartCounter();
  static void tcConfigure(int frequencyHz);
};

#endif  // _MIDI_H_
