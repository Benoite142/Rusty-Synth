#include "synth.hpp"
#include "../utils/note_map.hpp"
#include "constants.h"
#include "envelope/envelope.hpp"
#include "filters/filters.hpp"
#include "low_frequency_oscillator/low_frequency_oscillator.hpp"
#include "operator/operator.hpp"
#include "oscillator/oscillator.hpp"
#include <cstddef>
#include <iostream>
#include <string>

Synth::Synth(
    std::function<size_t(std::vector<std::string> *)> selectDeviceCallback)
    : lfo_1(5, Waveform::SINE, 0.3), low_pass_filter(10000.0f),
      high_pass_filter(5000.0f), synth_operator{2,
                                                0.5f,
                                                EnvelopeADSR{},
                                                Waveform::SQUARE,
                                                &lfo_1,
                                                &low_pass_filter,
                                                &high_pass_filter} {
  this->selectDeviceCallback = selectDeviceCallback;
}

Synth::~Synth() { free(async_player); }

void Synth::start_keyboard(NoteMap *nm, std::mutex *map_mutex) {
  async_player = new SoundPlayer(selectDeviceCallback);
  float buffer[BUFFER_SIZE];

  // playing in async mode still uses up a thread
  async_player->playAsync(buffer, &synth_operator, nm, map_mutex);

  // for now asserting here since we should never reach
  assert(false);
}

void Synth::updateOperator(size_t operator_index, std::string operator_field,
                           double value) {
  std::cout << "tried to update operator " << operator_index << "'s "
            << operator_field << " with " << value << std::endl;
  if (operator_field.compare("attack") == 0) {
    synth_operator.updateAttack(value);
    return;
  }
  if (operator_field.compare("decay") == 0) {
    synth_operator.updateDecay(value);
    return;
  }
  if (operator_field.compare("sustain") == 0) {
    synth_operator.updateSustain(value);
    return;
  }
  if (operator_field.compare("release") == 0) {
    synth_operator.updateRelease(value);
    return;
  }
  if (operator_field.compare("link-lfo") == 0) {
    synth_operator.setLFO1Enabled(value);
    return;
  }
}

void Synth::updateLFO(size_t lfo_index, std::string lfo_field, double value) {

  std::cout << "tried to update lfo " << lfo_index << "'s " << lfo_field
            << " with" << value << std::endl;
  if (lfo_field.compare("amount") == 0) {
    lfo_1.setAmplitudeAmount(value);
    return;
  }
  if (lfo_field.compare("rate") == 0) {
    lfo_1.setFrequencyRate(value);
    return;
  }
}

void Synth::startRecording() { async_player->startRecording(); }

void Synth::stopRecording() { async_player->stopRecording(); }

void Synth::updateLowPassFilter(double value) {
  std::cout << "tried to update lp filter " << " with" << value << std::endl;
  low_pass_filter.setCutoff(value);
  return;
}

void Synth::updateHighPassFilter(double value) {
  std::cout << "tried to update lp filter " << " with" << value << std::endl;
  high_pass_filter.setCutoff(value);
  return;
}
