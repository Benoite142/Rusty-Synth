#include "synth.hpp"
#include "../utils/note_map.hpp"
#include "constants.h"
#include "oscillator/oscillator.hpp"

Synth::Synth() {
  Oscillator Osc{0.0f, Waveform::SINE};
  osc = new std::vector<Oscillator>{Osc, Osc};
  async_player = new SoundPlayer();
}

void Synth::start_keyboard(NoteMap *nm, std::mutex *map_mutex) {
  float buffer[BUFFER_SIZE];

  // playing in async mode still uses up a thread
  async_player->playAsync(buffer, osc, nm, map_mutex);

  // for now asserting here since we should never reach
  assert(false);
}
