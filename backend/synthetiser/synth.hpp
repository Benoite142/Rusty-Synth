#ifndef SYNTH
#define SYNTH

#include "../sound_player/sound_player.hpp"
#include "../utils/note_map.hpp"
#include "oscillator/oscillator.hpp"
#include <alsa/asoundlib.h>
#include <mutex>
#include <vector>

class Synth {
private:
  std::vector<Oscillator> *osc;
  SoundPlayer *async_player;

public:
  Synth();
  void start_keyboard(NoteMap *nm, std::mutex *map_mutex);
};

#endif
