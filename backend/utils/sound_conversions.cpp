#include "sound_conversions.hpp"
#include "../synthetiser/constants.h"
#include <cmath>

int16_t floatTo16bits(float fFloat) {
  double dDouble = (double)fFloat;

  dDouble *= 32'767.0f;

  if (dDouble < -32'767.0f) {
    dDouble = -32'767.0f;
  } else if (dDouble > 32'767.0f) {
    dDouble = 32'767.0f;
  }

  return static_cast<int16_t>(dDouble);
}

int8_t floatTo8bits(float fFloat) {
  double dDouble = (double)fFloat;

  dDouble *= 127.0f;

  if (dDouble < -127.0f) {
    dDouble = -127.0f;
  } else if (dDouble > 127.0f) {
    dDouble = 127.0f;
  }

  return static_cast<int8_t>(dDouble);
}

// Calculate the frequency of the sound based on the note played
double calculate_frequency(int midi_input_note) {
  return A4_FREQUENCY * pow(2.0, (midi_input_note - A4_MIDI_NOTE) / 12.0);
}

// Finds the key index associated with the MIDI keyboard
int findKeyIndex(char keyPressed) {
  for (int i = 0; i < TAILLE_ARRAY_CLAVIER; ++i) {
    if (CLAVIER_INPUT[i] == keyPressed) {
      return MIDI_NOTES[i];
    }
  }
  return -1;
}