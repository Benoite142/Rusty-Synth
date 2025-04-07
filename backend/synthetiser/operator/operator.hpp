#ifndef OPERATOR_HPP
#define OPERATOR_HPP
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

public:
  Operator(size_t numberOfVoices, float amplitude, EnvelopeADSR envelope,
           Waveform waveform);
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
};

#endif
