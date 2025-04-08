#include "operator.hpp"
#include <cstddef>
#include <iostream>
#include <vector>

Operator::Operator(size_t numberOfVoices, float amplitude,
                   EnvelopeADSR envelope, Waveform waveform,
                   LowFrequencyOscillator *lfo_1, LowFrequencyOscillator *lfo_2)
    : numberOfVoices(numberOfVoices), amplitude(amplitude), envelope(envelope),
      waveform(waveform), lfo_1(lfo_1), lfo_2(lfo_2) {
  set_number_of_voices(numberOfVoices);
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

void Operator::setLFO1Enabled(bool enabled) {
  lfo1Multiplier = enabled ? 1.0f : 0.0f;
}

void Operator::setLFO2Enabled(bool enabled) {
  lfo2Multiplier = enabled ? 1.0f : 0.0f;
}

float Operator::advance() {
  float sum = 0;
  for (auto it = oscs.begin(); it != oscs.end(); ++it) {
    sum += it->advance();
  }
  float lfo1 = lfo_1->advance();
  float lfo2 = lfo_2->advance();

  float mod1 = lfo1Multiplier * lfo1 + (1.0f - lfo1Multiplier);
  float mod2 = lfo2Multiplier * lfo2 + (1.0f - lfo2Multiplier);

  return sum * mod1 * mod2;
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
