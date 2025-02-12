#include "oscillator.hpp"
#include "constants.h"
#include <cmath>

// right now is just sine oscillator
Oscillator::Oscillator(float amplitude, float freq)
    : amplitude{amplitude}, frequency{freq} {
  offset = 2 * PI * (frequency / SAMPLE_RATE);
}

float Oscillator::advance() {
  angle += offset;

  while (angle >= 2 * PI) {
    angle -= 2 * PI;
  }

  while (angle < 0) {
    angle += 2 * PI;
  }

  return amplitude * std::sin(angle);
}
