#include "synth.hpp"
#include "../utils/note_map.hpp"
#include "constants.h"
#include "oscillator/oscillator.hpp"

Synth::Synth(
    std::function<size_t(std::vector<std::string> *)> selectDeviceCallback) {
  Oscillator Osc{0.0f, Waveform::SINE};
  osc = new std::vector<Oscillator>{Osc, Osc};
  this->selectDeviceCallback = selectDeviceCallback;
}

Synth::~Synth() {
  free(osc);
  free(async_player);
}

void Synth::start_keyboard(NoteMap *nm, std::mutex *map_mutex) {
  async_player = new SoundPlayer(selectDeviceCallback);
  float buffer[BUFFER_SIZE];

  // playing in async mode still uses up a thread
  async_player->playAsync(buffer, osc, nm, map_mutex);

  // for now asserting here since we should never reach
  assert(false);
}
