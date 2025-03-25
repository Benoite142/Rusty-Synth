#include "oscillator.hpp"

SawOscillator::SawOscillator(float freq) : Oscillator(freq) {
  offset = freq / SAMPLE_RATE;
}

float SawOscillator::advance() {
  angle += offset;
  while (angle > 1.0f) {
    angle -= 1.0f;
  }
  while (angle < 0.0f) {
    angle += 1.0f;
  }
  float amp = envelope.getAmplitude();
  return amp * ((angle * 2.0f) - 1.0f);
}

void SawOscillator::setFrequency(float new_freq) {
  frequency = new_freq;
  offset = frequency / SAMPLE_RATE;
}

void SawOscillator::noteOn() { envelope.noteOn(); }

void SawOscillator::noteOff() { envelope.noteOff(); }