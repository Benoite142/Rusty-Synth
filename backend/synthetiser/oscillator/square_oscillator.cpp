#include "oscillator.hpp"

SquareOscillator::SquareOscillator(float amplitude, float freq): Oscillator(amplitude,freq){
  offset = 2 * PI * (freq / SAMPLE_RATE);
}

float SquareOscillator::advance() {
  angle += offset;
  while (angle >= 2 * (float)PI)
    angle -= 2 * (float)PI;
        
  while (angle < 0)
    angle += 2 * (float)PI;

  if (angle < (float) PI)
      return amplitude;
  else
      return -amplitude;
}
