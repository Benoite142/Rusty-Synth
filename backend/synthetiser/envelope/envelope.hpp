#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP

enum EnvelopeState { IDLE, ATTACK, DECAY, SUSTAIN, RELEASE };

class EnvelopeADSR {
private:
  double attackTime;       // attack knob
  double decayTime;        // decay knob
  double sustainAmplitude; // sustain knob
  double releaseTime;      // release knob
  double startAmplitude;   // operator volume
  double releaseStartAmplitude = 0.0;
  double amplitude = 0.0;
  EnvelopeState state = IDLE;

public:
  EnvelopeADSR(double attackTime = 1.6, double decayTime = 1.5,
               double sustainAmplitude = 0.5, double releaseTime = 1.5,
               double startAmplitude = 1.0);
  double getAmplitude();

  void noteOn();

  void noteOff();
};
#endif
