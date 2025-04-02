#include "envelope.hpp"

EnvelopeADSR::EnvelopeADSR(double attackTime, double decayTime,
                           double sustainAmplitude, double releaseTime)
    : attackTime{attackTime}, decayTime{decayTime},
      sustainAmplitude{sustainAmplitude}, releaseTime{releaseTime} {}

double EnvelopeADSR::getAttackTime() { return attackTime; }

double EnvelopeADSR::getDecayTime() { return decayTime; }
double EnvelopeADSR::getSustainAmplitude() { return sustainAmplitude; }

double EnvelopeADSR::getReleaseTime() { return releaseTime; }
