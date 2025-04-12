#include "../constants.h"
#include "filters.hpp"
#include <cmath>

LowPassFilter::LowPassFilter(float cutoff) {
  float resistor_capacitor = 1.0f / (2.0f * M_PI * cutoff);
  float derived_time = 1.0f / SAMPLE_RATE;
  alpha = derived_time / (resistor_capacitor + derived_time);
  prevY = 0.0f;
}

float LowPassFilter::process(float sample) {
  prevY += alpha * (sample - prevY);
  return prevY;
}

void LowPassFilter::setCutoff(float new_cutoff) {
  cutoff = MINCUTOFF * std::pow(MAXCUTOFFLOWPASS / MINCUTOFF, new_cutoff);

  float resistor_capacitor = 1.0f / (2.0f * M_PI * cutoff);
  alpha = resistor_capacitor / (resistor_capacitor + 1.0f / SAMPLE_RATE);
  prevY = 0.0f;
}
