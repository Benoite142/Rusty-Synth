#ifndef OSCILLATOR_HPP
#define OSCILLATOR_HPP

// not used here but used in all oscillator types
#include "../envelope/envelope.hpp"
enum class Waveform { SINE, TRIANGLE, SAW, SQUARE };
class Oscillator {
protected:
  float frequency;
  float sine_angle = 0.0f;
  float saw_angle = 0.0f;
  float triangle_angle = 0.0f;
  float square_angle = 0.0f;
  float offset_triangle_saw = 0.0f;
  float offset_sine_square = 0.0f;
  EnvelopeADSR envelope;
  float advance_sine();
  float advance_square();
  float advance_saw();
  float advance_triangle();
  Waveform waveform;

public:
  Oscillator(float freq, Waveform wave);
  float advance();
  void setFrequency(float freq);
  void noteOn();
  void noteOff();
  void setWaveform(Waveform newWaveform);
};

#endif
