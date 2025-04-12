#include "../constants.h"
#include "filters.hpp"
#include <cmath>

HighPassFilter::HighPassFilter(float cutoff) : cutoff{cutoff} {
  float resistor_capacitor = 1.0f / (2.0f * M_PI * cutoff);
  alpha = resistor_capacitor / (resistor_capacitor + 1.0f / SAMPLE_RATE);
  prevX = 0.0f;
  prevY = 0.0f;
}

float HighPassFilter::process(float sample) {
  float output = alpha * (prevY + sample - prevX);
  prevX = sample;
  prevY = output;
  return output;
}

void HighPassFilter::setCutoff(float new_cutoff) {
  cutoff = MINCUTOFF * std::pow(MAXCUTOFFHIGHPASS / MINCUTOFF, new_cutoff);

  float resistor_capacitor = 1.0f / (2.0f * M_PI * cutoff);
  alpha = resistor_capacitor / (resistor_capacitor + 1.0f / SAMPLE_RATE);
  prevX = 0.0f;
  prevY = 0.0f;
}
