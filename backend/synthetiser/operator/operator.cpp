#include "operator.hpp"
#include <cstddef>
#include <iostream>
#include <vector>

Operator::Operator(size_t numberOfVoices, float amplitude,
                   EnvelopeADSR envelope, Waveform waveform)
    : numberOfVoices(numberOfVoices), amplitude(amplitude), envelope(envelope),
      waveform(waveform) {
  set_number_of_voices(numberOfVoices);
  lfo_1 = new LowFrequencyOscillator(5, Waveform::SINE, 0.3);
}

void Operator::set_number_of_voices(size_t newNumOfVoices) {
  if (oscs.size() != newNumOfVoices) {
    numberOfVoices = newNumOfVoices;
    oscs.clear();
    Oscillator osc{0.0f, waveform, &envelope, amplitude};
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
  for (auto it = oscs.begin(); it != oscs.end(); ++it) {
    sum += it->advance();
  }
  return lfo_1->advance() * sum;
}

void Operator::releaseNote(size_t index) { oscs[index].noteOff(); }

void Operator::updateAttack(double value) {
  std::cout << "operator attack\n";
  envelope.setAttackTime(value);
}

void Operator::updateDecay(double value) {
  std::cout << "operator decay\n";
  envelope.setDecayTime(value);
}

void Operator::updateSustain(double value) {
  std::cout << "operator sustain\n";
  envelope.setSustainAmplitude(value);
}

void Operator::updateRelease(double value) {
  std::cout << "operator release\n";
  envelope.setReleaseTime(value);
}
