#include "Sound.h"
#include "Midi.h"

// Define your new Midi sounds here! Make sure to add them to the header and Midi::SOUNDS.

// Black-throated Blue Warbler
Sound Midi::m1(
    VS1053_BANK_MELODY,
    124,
    127,
    30,
    (uint8_t[]) {110, 108, 106, 112, 111, 110, 109, 108, 105, 109, 112, 114, 113, 114, 113, 112, 111, 108, 106, 104,
                 107, 115, 114, 115, 116, 115, 116, 115, 116, 115},
    (uint16_t[]) {21, 14, 7, 7, 10, 38, 3, 4, 3, 4, 3, 4, 3, 4, 7, 13, 18, 3, 3, 4, 4, 10, 4, 27, 4, 7, 3, 3, 4, 3},
    -20,
    1
);
