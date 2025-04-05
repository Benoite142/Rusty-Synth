#include "synth.hpp"
#include "../utils/note_map.hpp"
#include "constants.h"
#include "envelope/envelope.hpp"
#include "operator/operator.hpp"
#include "oscillator/oscillator.hpp"
#include <iostream>

Synth::Synth(
    std::function<size_t(std::vector<std::string> *)> selectDeviceCallback)
    : synth_operator{2, 0.5f, EnvelopeADSR{}, Waveform::SAW} {
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
}
