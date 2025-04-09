#ifndef OPERATOR_HPP
#define OPERATOR_HPP
#include "../low_frequency_oscillator/low_frequency_oscillator.hpp"
#include "../oscillator/oscillator.hpp"
#include <cstddef>
#include <vector>

class Operator {
private:
  std::vector<Oscillator> oscs;
  size_t numberOfVoices;
  float amplitude;
  EnvelopeADSR envelope;
  Waveform waveform;
  LowFrequencyOscillator *lfo_1;
  float lfo1Multiplier = 1.0f;

public:
  Operator(size_t numberOfVoices, float amplitude, EnvelopeADSR envelope,
           Waveform waveform, LowFrequencyOscillator *lfo_1);
  void set_operator_amplitude(float amplitude);
  size_t getNumberOfVoices();
  void set_number_of_voices(size_t numberOfVoices);
  void setWaveForm(Waveform newWaveform);
  void updateFrequency(size_t index, double note);
  float advance();
  void releaseNote(size_t index);
  void updateAttack(double value);
  void updateDecay(double value);
  void updateSustain(double value);
  void updateRelease(double value);
  void setLFO1Enabled(bool enabled);
};

#endif
