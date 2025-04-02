#include "operator.hpp"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>

Operator::Operator(size_t numberOfVoices, float amplitude,
                   EnvelopeADSR envelope, Waveform waveform)
    : numberOfVoices(numberOfVoices), amplitude(amplitude), envelope(envelope),
      waveform(waveform) {
  set_number_of_voices(numberOfVoices);
}

void Operator::set_number_of_voices(size_t newNumOfVoices) {
  if (oscs.size() != newNumOfVoices) {
    numberOfVoices = newNumOfVoices;
    oscs.clear();
    Oscillator osc{0.0f, waveform};
    for (int i = 0; i < numberOfVoices; ++i) {
      oscs.push_back(osc);
    }
  }
}

size_t Operator::getNumberOfVoices() { return numberOfVoices; }

void Operator::set_operator_amplitude(float new_amp) {
  if (amplitude != new_amp) {
    amplitude = new_amp;
  }
}

void Operator::setWaveForm(Waveform newWaveform) {
  if (waveform != newWaveform) {
    waveform = newWaveform;
  }
}

void Operator::updateFrequency(size_t index, double note) {
  oscs[index].setFrequency(note);
  oscs[index].noteOn();
}

float Operator::advance() {
  float sum = 0;
  //   std::for_each(oscs.begin(), oscs.end(),
  // [&sum](Oscillator o) { sum += o.advance(); });
  for (auto it = oscs.begin(); it != oscs.end(); ++it) {
    sum += it->advance();
  }
  return sum;
}

void Operator::release(size_t index) { oscs[index].noteOff(); }
