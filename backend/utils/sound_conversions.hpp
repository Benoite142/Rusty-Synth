#ifndef SOUND_CONVERSIONS
#define SOUND_CONVERSIONS

#include <cstdint>

int16_t floatTo16bits(float fFloat);

int8_t floatTo8bits(float fFloat);

double calculate_frequency(int midi_input_note);

int findKeyIndex(char keyPressed);

#endif
