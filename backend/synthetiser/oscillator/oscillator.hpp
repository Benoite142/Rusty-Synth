#ifndef OSCILLATOR_HPP
#define OSCILLATOR_HPP

// not used here but used in all oscillator types
#include "../constants.h"
#include "../envelope/envelope.hpp"

class Oscillator {
protected:
  float frequency;
  float angle = 0.0f;
  float offset = 0.0f;
  EnvelopeADSR envelope;

public:
  Oscillator(float freq);
  virtual float advance() = 0;
  virtual void setFrequency(float freq) = 0;
  virtual void noteOn() = 0;
  virtual void noteOff() = 0;
};

class SineOscillator : public Oscillator {
public:
  SineOscillator(float freq);
  float advance() override;
  void setFrequency(float freq) override;
  void noteOn() override;
  void noteOff() override;
};

class SquareOscillator : public Oscillator {
public:
  SquareOscillator(float freq);
  float advance() override;
  void setFrequency(float freq) override;
  void noteOn() override;
  void noteOff() override;
};

class TriangleOscillator : public Oscillator {
public:
  TriangleOscillator(float freq);
  float advance() override;
  void setFrequency(float freq) override;
  void noteOn() override;
  void noteOff() override;
};

class SawOscillator : public Oscillator {
public:
  SawOscillator(float freq);
  float advance() override;
  void setFrequency(float freq) override;
  void noteOn() override;
  void noteOff() override;
};

class NoiseOscillator : public Oscillator {
public:
  NoiseOscillator(float freq);
  float advance() override;
  void noteOn() override;
  void noteOff() override;
};

#endif
