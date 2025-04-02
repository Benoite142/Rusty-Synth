#ifndef OPERATOR_HPP
#define OPERATOR_HPP
#include "../oscillator/oscillator.hpp"
#include <cstddef>
#include <vector>

class Operator {
protected:
  std::vector<Oscillator> oscs;
  size_t numberOfVoices;
  float amplitude;
  EnvelopeADSR envelope;
  Waveform waveform;

public:
  Operator(size_t numberOfVoices, float amplitude, EnvelopeADSR envelope,
           Waveform waveform);
  void set_operator_amplitude(float amplitude);
  size_t getNumberOfVoices();
  void set_number_of_voices(size_t numberOfVoices);
  void setWaveForm(Waveform newWaveform);
  void updateFrequency(size_t index, double note);
  float advance();
  void release(size_t index);
};

#endif
