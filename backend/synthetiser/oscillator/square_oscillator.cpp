#include "oscillator.hpp"

SquareOscillator::SquareOscillator(float freq) : Oscillator(freq) {
  offset = 2 * PI * (freq / SAMPLE_RATE);
}

float SquareOscillator::advance() {
  angle += offset;
  while (angle >= 2 * (float)PI)
    angle -= 2 * (float)PI;

  while (angle < 0)
    angle += 2 * (float)PI;

  if (angle < (float)PI)
    return envelope.getAmplitude();
  else
    return -envelope.getAmplitude();
}

void SquareOscillator::setFrequency(float new_freq) {
  frequency = new_freq;
  offset = 2 * PI * (frequency / SAMPLE_RATE);
}

void SquareOscillator::noteOn() { envelope.noteOn(); }

void SquareOscillator::noteOff() { envelope.noteOff(); }