#ifndef LOW_FREQUENCY_OSCILLATOR
#define LOW_FREQUENCY_OSCILLATOR
#include "../oscillator/oscillator.hpp"

class LowFrequencyOscillator {
private:
  float frequency;
  float amplitude;
  float offset_sine_square;
  float offset_triangle_saw;
  float sine_angle;
  float saw_angle;
  float triangle_angle;
  float square_angle;
  Waveform waveform;

  float advance_sine();
  float advance_saw();
  float advance_square();
  float advance_triangle();

public:
  LowFrequencyOscillator(float freq, Waveform wave, float amplitude);
  float advance();
  void setWaveForm(Waveform newWaveform);
  void setFrequencyRate(float new_freq);
  void setAmplitudeAmount(float new_amp);
  void advance_angle();
};

#endif
