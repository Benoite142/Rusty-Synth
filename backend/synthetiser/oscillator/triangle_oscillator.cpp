#include "oscillator.hpp"

TriangleOscillator::TriangleOscillator(float amplitude, float freq): Oscillator(amplitude, freq){
    offset = freq/ SAMPLE_RATE;
  }
  
  float TriangleOscillator::advance(){
    angle += offset;
    while (angle > 1.0f)
    {
        angle -= 1.0f;
    }
    while (angle < 0.0f)
        angle += 1.0f;
    
    float ret;
    if (angle <= 0.5f)
        ret = angle * 2;
    else
        ret = (1.0f - angle) * 2;
    
    return amplitude * ((ret * 2.0f) - 1.0f);
  }
  