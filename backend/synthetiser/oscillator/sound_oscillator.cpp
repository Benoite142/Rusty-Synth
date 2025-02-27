#include "oscillator.hpp"
#include <cstdlib>

NoiseOscillator::NoiseOscillator(float amplitude, float freq)
    : Oscillator(amplitude, freq) {}

float NoiseOscillator::advance() {

  return amplitude * ((float)rand() / (float)RAND_MAX) - 1.0f;
}