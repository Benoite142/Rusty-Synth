#include "low_frequency_oscillator.hpp"
#include "../constants.h"
#include <cmath>

LowFrequencyOscillator::LowFrequencyOscillator(float freq, Waveform wave,
                                               float amplitude)
    : frequency{freq}, waveform(wave), amplitude(amplitude) {
  offset_sine_square = 2 * PI * (frequency / SAMPLE_RATE);
  offset_triangle_saw = frequency / SAMPLE_RATE;
}

void LowFrequencyOscillator::setWaveForm(Waveform newWaveform) {
  waveform = newWaveform;
}

void LowFrequencyOscillator::setFrequencyRate(float new_freq) {
  // max lfo rate to 5
  frequency = new_freq * 5;
  offset_sine_square = 2 * PI * (frequency / SAMPLE_RATE);
  offset_triangle_saw = frequency / SAMPLE_RATE;
}

void LowFrequencyOscillator::setAmplitudeAmount(float new_amp) {
  // max amount to 0.5
  amplitude = new_amp * 0.5;
}

float LowFrequencyOscillator::advance() {
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
    return 1.0f;
  }
}

float LowFrequencyOscillator::advance_sine() {

  while (sine_angle >= 2 * PI) {
    sine_angle -= 2 * PI;
  }

  while (sine_angle < 0) {
    sine_angle += 2 * PI;
  }

  return -amplitude * std::sin(sine_angle) + (1 - amplitude);
}
float LowFrequencyOscillator::advance_saw() {
  while (saw_angle > 1.0f) {
    saw_angle -= 1.0f;
  }
  while (saw_angle < 0.0f) {
    saw_angle += 1.0f;
  }
  return -amplitude * ((saw_angle * 2.0f) - 1.0f) + (1 - amplitude);
}
float LowFrequencyOscillator::advance_square() {

  while (square_angle >= 2 * (float)PI)
    square_angle -= 2 * (float)PI;

  while (square_angle < 0)
    square_angle += 2 * (float)PI;
  if (square_angle < (float)PI)
    return -amplitude + (1 - amplitude);
  else
    return amplitude - (1 + amplitude);
}
float LowFrequencyOscillator::advance_triangle() {
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

  return -amplitude * ((ret * 2.0f) - 1.0f) + (1 - amplitude);
}

void LowFrequencyOscillator::advance_angle() {
  sine_angle += offset_sine_square;

  triangle_angle += offset_triangle_saw;

  saw_angle += offset_triangle_saw;

  square_angle += offset_sine_square;
}
