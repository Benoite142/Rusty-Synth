#include "oscillator.hpp"
#include "../constants.h"
#include <cmath>

Oscillator::Oscillator(float freq, Waveform wave, EnvelopeADSR *env,
                       float peakAmplitude)
    : frequency{freq}, waveform(wave) {
  offset_sine_square = 2 * PI * (freq / SAMPLE_RATE);
  offset_triangle_saw = freq / SAMPLE_RATE;
  envelope = env;
  peakAmplitude = peakAmplitude;
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

  getAmplitude();
  return amplitude * std::sin(sine_angle);
}

float Oscillator::advance_square() {
  square_angle += offset_sine_square;
  while (square_angle >= 2 * (float)PI)
    square_angle -= 2 * (float)PI;

  while (square_angle < 0)
    square_angle += 2 * (float)PI;
  getAmplitude();
  if (square_angle < (float)PI)
    return amplitude;
  else
    return -amplitude;
}

float Oscillator::advance_saw() {
  saw_angle += offset_triangle_saw;
  while (saw_angle > 1.0f) {
    saw_angle -= 1.0f;
  }
  while (saw_angle < 0.0f) {
    saw_angle += 1.0f;
  }
  getAmplitude();
  return amplitude * ((saw_angle * 2.0f) - 1.0f);
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
  getAmplitude();

  return amplitude * ((ret * 2.0f) - 1.0f);
}

void Oscillator::setFrequency(float new_freq) {
  frequency = new_freq;
  offset_sine_square = 2 * PI * (frequency / SAMPLE_RATE);
  offset_triangle_saw = frequency / SAMPLE_RATE;
}

void Oscillator::noteOn() {
  if (state == IDLE || state == RELEASE) {
    state = ATTACK;
  }
}

void Oscillator::noteOff() {
  if (state == RELEASE) {
    return;
  }
  state = RELEASE;
  releaseStartAmplitude = amplitude;
}

void Oscillator::getAmplitude() {
  switch (state) {
  case ATTACK:
    amplitude += (peakAmplitude / envelope->getAttackTime()) * DELTA_TIME;
    if (amplitude >= peakAmplitude && state != DECAY) {
      amplitude = peakAmplitude;
      state = DECAY;
    }
    break;
  case DECAY:
    amplitude -= ((peakAmplitude - envelope->getSustainAmplitude()) /
                  envelope->getDecayTime()) *
                 DELTA_TIME;
    if (amplitude <= envelope->getSustainAmplitude()) {

      amplitude = envelope->getSustainAmplitude();
      state = SUSTAIN;
    }
    break;

  case SUSTAIN:
    amplitude = envelope->getSustainAmplitude();
    return;
    break;

  case RELEASE:
    amplitude -=
        (releaseStartAmplitude / envelope->getReleaseTime()) * DELTA_TIME;
    if (amplitude <= 0.001) {
      amplitude = 0.0;
      state = IDLE;
    }
    break;

  case IDLE:
    amplitude = 0.0;
    break;
  }
}
