#include "envelope.hpp"
#include "../constants.h"

EnvelopeADSR::EnvelopeADSR(double attackTime, double decayTime,
                           double sustainAmplitude, double releaseTime,
                           double startAmplitude)
    : attackTime{attackTime}, decayTime{decayTime},
      sustainAmplitude{sustainAmplitude}, releaseTime{releaseTime},
      startAmplitude{startAmplitude} {}

double EnvelopeADSR::getAmplitude() {
  switch (state) {
  case ATTACK:
    amplitude += (startAmplitude / attackTime) * DELTA_TIME;
    if (amplitude >= startAmplitude && state != DECAY) {
      amplitude = startAmplitude;
      state = DECAY;
    }
    break;
  case DECAY:
    amplitude -= ((startAmplitude - sustainAmplitude) / decayTime) * DELTA_TIME;
    if (amplitude <= sustainAmplitude) {

      amplitude = sustainAmplitude;
      state = SUSTAIN;
    }
    break;

  case SUSTAIN:
    amplitude = sustainAmplitude;
    break;

  case RELEASE:
    amplitude -= (releaseStartAmplitude / releaseTime) * DELTA_TIME;
    if (amplitude <= 0.001) {
      amplitude = 0.0;
      state = IDLE;
    }
    break;

  case IDLE:
    amplitude = 0.0;
    break;
  }
  return amplitude;
}

void EnvelopeADSR::noteOn() {
  if (state == IDLE || state == RELEASE) {
    state = ATTACK;
  }
}

void EnvelopeADSR::noteOff() {
  state = RELEASE;
  releaseStartAmplitude = amplitude;
}
