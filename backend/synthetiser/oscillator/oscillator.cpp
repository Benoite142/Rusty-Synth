#include "oscillator.hpp"
#include "../constants.h"
#include <cmath>

Oscillator::Oscillator(float freq, Waveform wave)
    : frequency{freq}, waveform(wave) {
  offset_sine_square = 2 * PI * (freq / SAMPLE_RATE);
  offset_triangle_saw = freq / SAMPLE_RATE;
}

void Oscillator::setWaveform(Waveform newWaveform) { waveform = newWaveform; }

float Oscillator::advance() {
  switch (waveform) {
  case Waveform::SAW:
    return advance_saw();

  case Waveform::SINE:
    return advance_sine();

  case Waveform::SQUARE:
    return advance_square();

  case Waveform::TRIANGLE:
    return advance_triangle();

  default:
    return 0.0f;
  }
}

float Oscillator::advance_sine() {
  sine_angle += offset_sine_square;

  while (sine_angle >= 2 * PI) {
    sine_angle -= 2 * PI;
  }

  while (sine_angle < 0) {
    sine_angle += 2 * PI;
  }

  float amp = envelope.getAmplitude();
  return amp * std::sin(sine_angle);
}

float Oscillator::advance_square() {
  square_angle += offset_sine_square;
  while (square_angle >= 2 * (float)PI)
    square_angle -= 2 * (float)PI;

  while (square_angle < 0)
    square_angle += 2 * (float)PI;

  if (square_angle < (float)PI)
    return envelope.getAmplitude();
  else
    return -envelope.getAmplitude();
}

float Oscillator::advance_saw() {
  saw_angle += offset_triangle_saw;
  while (saw_angle > 1.0f) {
    saw_angle -= 1.0f;
  }
  while (saw_angle < 0.0f) {
    saw_angle += 1.0f;
  }
  float amp = envelope.getAmplitude();
  return amp * ((saw_angle * 2.0f) - 1.0f);
}

float Oscillator::advance_triangle() {
  triangle_angle += offset_triangle_saw;
  while (triangle_angle > 1.0f) {
    triangle_angle -= 1.0f;
  }
  while (triangle_angle < 0.0f)
    triangle_angle += 1.0f;

  float ret;
  if (triangle_angle <= 0.5f)
    ret = triangle_angle * 2;
  else
    ret = (1.0f - triangle_angle) * 2;
  float amp = envelope.getAmplitude();

  return amp * ((ret * 2.0f) - 1.0f);
}

void Oscillator::setFrequency(float new_freq) {
  frequency = new_freq;
  offset_sine_square = 2 * PI * (frequency / SAMPLE_RATE);
  offset_triangle_saw = frequency / SAMPLE_RATE;
}

void Oscillator::noteOff() { envelope.noteOff(); }

void Oscillator::noteOn() { envelope.noteOn(); }