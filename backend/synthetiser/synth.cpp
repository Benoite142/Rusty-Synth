#include "synth.hpp"
#include "../utils/note_map.hpp"
#include "constants.h"
#include "filters/filters.hpp"
#include "low_frequency_oscillator/low_frequency_oscillator.hpp"
#include "operator/operator.hpp"
#include "oscillator/oscillator.hpp"
#include <boost/asio/error.hpp>
#include <cstddef>
#include <iostream>
#include <string>

Synth::Synth(
    std::function<size_t(std::vector<std::string> *)> selectDeviceCallback)
    : lfo_1(0, Waveform::SINE, 0), numberOfVoices(2), low_pass_filter(0),
      high_pass_filter(MIN_CUTOFF),
      synth_operators{{numberOfVoices, 0.5f, Waveform::SINE, &lfo_1,
                       &low_pass_filter, &high_pass_filter},
                      {numberOfVoices, 0.0f, Waveform::SINE, &lfo_1,
                       &low_pass_filter, &high_pass_filter},
                      {numberOfVoices, 0.0f, Waveform::SINE, &lfo_1,
                       &low_pass_filter, &high_pass_filter},
                      {numberOfVoices, 0.0f, Waveform::SINE, &lfo_1,
                       &low_pass_filter, &high_pass_filter}} {
  this->selectDeviceCallback = selectDeviceCallback;
}

Synth::~Synth() { free(async_player); }

void Synth::start_keyboard(NoteMap *nm, std::mutex *map_mutex) {
  async_player = new SoundPlayer(selectDeviceCallback);
  float buffer[BUFFER_SIZE];

  // playing in async mode still uses up a thread
  async_player->playAsync(buffer, synth_operators, &synth_operators[1],
                          &synth_operators[2], &synth_operators[3], nm,
                          map_mutex);

  // for now asserting here since we should never reach
  assert(false);
}

void Synth::updateOperator(size_t operator_index, std::string operator_field,
                           std::variant<double, std::string> value) {
  if (operator_field.compare("waveform") == 0) {
    if (std::get<std::string>(value).compare("sine") == 0) {
      synth_operators[operator_index].setWaveForm(Waveform::SINE);
      return;
    }

    if (std::get<std::string>(value).compare("square") == 0) {
      synth_operators[operator_index].setWaveForm(Waveform::SQUARE);
      return;
    }

    if (std::get<std::string>(value).compare("saw") == 0) {
      synth_operators[operator_index].setWaveForm(Waveform::SAW);
      return;
    }

    if (std::get<std::string>(value).compare("triangle") == 0) {
      synth_operators[operator_index].setWaveForm(Waveform::TRIANGLE);
      return;
    }
  }

  if (operator_field.compare("volume") == 0) {
    synth_operators[operator_index].set_operator_amplitude(
        std::get<double>(value));
  }

  if (operator_field.compare("attack") == 0) {
    synth_operators[operator_index].updateAttack(5 * std::get<double>(value));
    return;
  }
  if (operator_field.compare("decay") == 0) {
    synth_operators[operator_index].updateDecay(2 * std::get<double>(value));
    return;
  }
  if (operator_field.compare("sustain") == 0) {
    synth_operators[operator_index].updateSustain(std::get<double>(value));
    return;
  }
  if (operator_field.compare("release") == 0) {
    synth_operators[operator_index].updateRelease(5 * std::get<double>(value));
    return;
  }
  if (operator_field.compare("link-lfo") == 0) {
    synth_operators[operator_index].setLFO1Enabled(std::get<double>(value));
    return;
  }
}

void Synth::updateLFO(size_t lfo_index, std::string lfo_field,
                      std::variant<double, std::string> value) {

  if (lfo_field.compare("waveform") == 0) {
    if (std::get<std::string>(value).compare("sine") == 0) {
      lfo_1.setWaveForm(Waveform::SINE);
      return;
    }

    if (std::get<std::string>(value).compare("square") == 0) {
      lfo_1.setWaveForm(Waveform::SQUARE);
      return;
    }

    if (std::get<std::string>(value).compare("saw") == 0) {
      lfo_1.setWaveForm(Waveform::SAW);
      return;
    }

    if (std::get<std::string>(value).compare("triangle") == 0) {
      lfo_1.setWaveForm(Waveform::TRIANGLE);
      return;
    }
  }

  if (lfo_field.compare("amount") == 0) {
    lfo_1.setAmplitudeAmount(std::get<double>(value));
    return;
  }
  if (lfo_field.compare("rate") == 0) {
    lfo_1.setFrequencyRate(std::get<double>(value));
    return;
  }
}

void Synth::startRecording() { async_player->startRecording(); }

void Synth::stopRecording() { async_player->stopRecording(); }

void Synth::updateLowPassFilter(double value) {
  low_pass_filter.setCutoff(value);
}

void Synth::updateHighPassFilter(double value) {
  high_pass_filter.setCutoff(value);
}

void Synth::setNumberOfVoices(size_t new_voices_number) {
  numberOfVoices = new_voices_number;
  for (int i = 0; i != 4; ++i) {
    synth_operators[i].set_number_of_voices(new_voices_number);
  }
}
