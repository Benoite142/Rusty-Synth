#include "oscillator.hpp"
#include <cmath>

SineOscillator::SineOscillator(float amplitude, float freq): Oscillator(amplitude, freq){
    offset = 2 * PI * (freq / SAMPLE_RATE);
}

float SineOscillator::advance(){
    angle += offset;

  while (angle >= 2 * PI) {
    angle -= 2 * PI;
  }

  while (angle < 0) {
    angle += 2 * PI;
  }

  return amplitude * std::sin(angle);
}