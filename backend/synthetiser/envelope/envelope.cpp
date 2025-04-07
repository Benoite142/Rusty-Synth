#include "envelope.hpp"

EnvelopeADSR::EnvelopeADSR(double attackTime, double decayTime,
                           double sustainAmplitude, double releaseTime)
    : attackTime{attackTime}, decayTime{decayTime},
      sustainAmplitude{sustainAmplitude}, releaseTime{releaseTime} {}

double EnvelopeADSR::getAttackTime() { return attackTime; }
double EnvelopeADSR::getDecayTime() { return decayTime; }
double EnvelopeADSR::getSustainAmplitude() { return sustainAmplitude; }
double EnvelopeADSR::getReleaseTime() { return releaseTime; }

void EnvelopeADSR::setAttackTime(double value) { attackTime = value; }
void EnvelopeADSR::setDecayTime(double value) { decayTime = value; }
void EnvelopeADSR::setSustainAmplitude(double value) {
  sustainAmplitude = value;
}
void EnvelopeADSR::setReleaseTime(double value) { releaseTime = value; }
