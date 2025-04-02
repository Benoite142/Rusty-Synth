#include "synth.hpp"
#include "../utils/note_map.hpp"
#include "constants.h"
#include "envelope/envelope.hpp"
#include "operator/operator.hpp"
#include "oscillator/oscillator.hpp"

Synth::Synth(
    std::function<size_t(std::vector<std::string> *)> selectDeviceCallback)
    : synth_operator{2, 0.5f, EnvelopeADSR{}, Waveform::SQUARE} {
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
