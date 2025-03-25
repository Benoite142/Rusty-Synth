#include "../envelope/envelope.hpp"
#include "oscillator.hpp"
#include <cmath>

SineOscillator::SineOscillator(float freq) : Oscillator(freq) {
  offset = 2 * PI * (freq / SAMPLE_RATE);
}

float SineOscillator::advance() {
  angle += offset;

  while (angle >= 2 * PI) {
    angle -= 2 * PI;
  }

  while (angle < 0) {
    angle += 2 * PI;
  }

  float amp = envelope.getAmplitude();
  return amp * std::sin(angle);
}

void SineOscillator::setFrequency(float new_freq) {
  frequency = new_freq;
  offset = 2 * PI * (frequency / SAMPLE_RATE);
}

void SineOscillator::noteOn() { envelope.noteOn(); }

void SineOscillator::noteOff() { envelope.noteOff(); }