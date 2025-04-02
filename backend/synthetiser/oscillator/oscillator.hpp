#ifndef OSCILLATOR_HPP
#define OSCILLATOR_HPP

#include "../envelope/envelope.hpp"

enum class Waveform { SINE, TRIANGLE, SAW, SQUARE };
class Oscillator {
private:
  float frequency;
  float sine_angle = 0.0f;
  float saw_angle = 0.0f;
  float triangle_angle = 0.0f;
  float square_angle = 0.0f;
  float offset_triangle_saw = 0.0f;
  float offset_sine_square = 0.0f;
  float advance_sine();
  float advance_square();
  float advance_saw();
  float advance_triangle();
  void getAmplitude();
  Waveform waveform;
  EnvelopeADSR *envelope;
  float peakAmplitude;
  double releaseStartAmplitude = 0.0;
  double amplitude = 0.0;
  EnvelopeState state = IDLE;

public:
  Oscillator(float freq, Waveform wave, EnvelopeADSR *envelope,
             float peakAmplitude);
  float advance();
  void setFrequency(float freq);
  void noteOn();
  void noteOff();
  void setWaveform(Waveform newWaveform);
};

#endif
