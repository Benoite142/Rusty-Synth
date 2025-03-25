#include "oscillator.hpp"

TriangleOscillator::TriangleOscillator(float freq) : Oscillator(freq) {
  offset = freq / SAMPLE_RATE;
}

float TriangleOscillator::advance() {
  angle += offset;
  while (angle > 1.0f) {
    angle -= 1.0f;
  }
  while (angle < 0.0f)
    angle += 1.0f;

  float ret;
  if (angle <= 0.5f)
    ret = angle * 2;
  else
    ret = (1.0f - angle) * 2;
  float amp = envelope.getAmplitude();

  return amp * ((ret * 2.0f) - 1.0f);
}

void TriangleOscillator::setFrequency(float new_freq) {
  frequency = new_freq;
  offset = frequency / SAMPLE_RATE;
}

void TriangleOscillator::noteOn() { envelope.noteOn(); }

void TriangleOscillator::noteOff() { envelope.noteOff(); }