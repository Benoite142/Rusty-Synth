#include "oscillator.hpp"
#include <cstdlib>

NoiseOscillator::NoiseOscillator(float freq) : Oscillator(freq) {}

float NoiseOscillator::advance() {

  float amp = envelope.getAmplitude();
  return amp * ((float)rand() / (float)RAND_MAX) - 1.0f;
}

void NoiseOscillator::noteOn() { envelope.noteOn(); }

void NoiseOscillator::noteOff() { envelope.noteOff(); }