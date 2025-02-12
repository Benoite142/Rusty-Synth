#ifndef OSCILLATOR_HPP
#define OSCILLATOR_HPP

class Oscillator {
private:
  float frequency;
  float amplitude;
  float angle = 0.0f;
  float offset = 0.0f;

public:
  Oscillator(float amplitude, float freq);
  float advance();
};

#endif
