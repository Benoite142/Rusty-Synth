#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP

enum EnvelopeState { IDLE, ATTACK, DECAY, SUSTAIN, RELEASE };

class EnvelopeADSR {
private:
  double attackTime;       // attack knob
  double decayTime;        // decay knob
  double sustainAmplitude; // sustain knob
  double releaseTime;      // release knob

public:
  EnvelopeADSR(double attackTime = 1.6, double decayTime = 1.5,
               double sustainAmplitude = 0.5, double releaseTime = 1.5);
  double getAttackTime();
  double getDecayTime();
  double getSustainAmplitude();
  double getReleaseTime();
  void setAttackTime(double value);
  void setDecayTime(double value);
  void setSustainAmplitude(double value);
  void setReleaseTime(double value);
};
#endif
