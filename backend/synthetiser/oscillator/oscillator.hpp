#ifndef OSCILLATOR_HPP
#define OSCILLATOR_HPP

// not used here but used in all oscillator types
#include "../constants.h"

class Oscillator {
protected:
  float frequency;
  float amplitude;
  float angle = 0.0f;
  float offset = 0.0f;

public:
  Oscillator(float amplitude, float freq);
  virtual float advance() = 0;
  virtual void setFrequency(float freq) = 0;
};

class SineOscillator : public Oscillator {
public:
  SineOscillator(float amplitude, float freq);
  float advance() override;
  void setFrequency(float freq) override;
};

class SquareOscillator : public Oscillator {
public:
  SquareOscillator(float amplitude, float freq);
  float advance() override;
  void setFrequency(float freq) override;
};

class TriangleOscillator : public Oscillator {
public:
  TriangleOscillator(float amplitude, float freq);
  float advance() override;
  void setFrequency(float freq) override;
};

class SawOscillator : public Oscillator {
public:
  SawOscillator(float amplitude, float freq);
  float advance() override;
  void setFrequency(float freq) override;
};

class NoiseOscillator : public Oscillator {
public:
  NoiseOscillator(float amplitude, float freq);
  float advance() override;
};

#endif
