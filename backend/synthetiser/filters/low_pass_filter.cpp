#include "../constants.h"
#include "filters.hpp"
#include <cmath>

LowPassFilter::LowPassFilter(float cutoff) { setCutoff(cutoff); }

float LowPassFilter::process(float sample) {
  prevY += alpha * (sample - prevY);
  return prevY;
}

void LowPassFilter::setCutoff(float new_cutoff) {
  cutoff = std::pow(MAX_CUTOFF_LOWPASS, (0.5f * (1 - new_cutoff)));
  float resistor_capacitor = 1.0f / (2.0f * M_PI * cutoff);
  alpha = (1.0f / SAMPLE_RATE) / (resistor_capacitor + 1.0f / SAMPLE_RATE);
  prevY = 0.0f;
}
