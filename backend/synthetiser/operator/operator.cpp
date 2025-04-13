#include "operator.hpp"
#include "../filters/filters.hpp"
#include <cstddef>
#include <iostream>
#include <vector>

Operator::Operator(size_t numberOfVoices, float amplitude, Waveform waveform,
                   LowFrequencyOscillator *lfo_1,
                   LowPassFilter *low_pass_filter,
                   HighPassFilter *high_pass_filter)
    : numberOfVoices(numberOfVoices), amplitude(amplitude), waveform(waveform),
      lfo_1(lfo_1), low_pass_filter(low_pass_filter),
      high_pass_filter(high_pass_filter) {
  set_number_of_voices(numberOfVoices);
}

void Operator::set_number_of_voices(size_t newNumOfVoices) {
  if (oscs.size() != newNumOfVoices) {
    numberOfVoices = newNumOfVoices;
    oscs.clear();
    Oscillator osc{0.0f, waveform, &envelope, amplitude};
    for (int i = 0; i != numberOfVoices; ++i) {
      oscs.push_back(osc);
    }
  }
}

size_t Operator::getNumberOfVoices() { return numberOfVoices; }

void Operator::set_operator_amplitude(float new_amp) {
  if (amplitude != new_amp) {
    amplitude = new_amp;
    for (auto it = oscs.begin(); it != oscs.end(); ++it) {
      it->setPeakAmplitude(amplitude);
    }
  }
}

void Operator::setWaveForm(Waveform newWaveform) {
  if (waveform != newWaveform) {
    waveform = newWaveform;
    for (int i = 0; i != numberOfVoices; ++i) {
      oscs[i].setWaveform(newWaveform);
    }
  }
}

void Operator::updateFrequency(size_t index, double note) {
  oscs[index].setFrequency(note);
  oscs[index].noteOn();
}

void Operator::setLFO1Enabled(bool enabled) {
  lfo1Multiplier = enabled ? 1.0f : 0.0f;
}

float Operator::advance() {
  float sum = 0;
  for (auto it = oscs.begin(); it != oscs.end(); ++it) {
    sum += it->advance();
  }
  float lfo1 = lfo_1->advance();
  // apply lfo
  float mod1 = lfo1Multiplier * lfo1 + (1.0f - lfo1Multiplier);
  return sum * mod1 * amplitude;
}

float Operator::process(float sample) {
  // apply low pass filter
  float audio_signal = low_pass_filter->process(sample);
  // apply high pass filter
  return high_pass_filter->process(audio_signal);
}
void Operator::advanceLFO() { lfo_1->advance_angle(); }

void Operator::releaseNote(size_t index) { oscs[index].noteOff(); }

void Operator::updateAttack(double value) {
  envelope.setAttackTime(value < 0.01 ? 0.01 : value);
}

void Operator::updateDecay(double value) {
  envelope.setDecayTime(value < 0.01 ? 0.01 : value);
}

void Operator::updateSustain(double value) {
  envelope.setSustainAmplitude(amplitude * value);
}

void Operator::updateRelease(double value) {
  envelope.setReleaseTime(value < 0.01 ? 0.01 : value);
}
