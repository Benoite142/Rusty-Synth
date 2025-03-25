#ifndef SYNTH
#define SYNTH

#include "../sound_player/sound_player.hpp"
#include "../utils/note_map.hpp"
#include "oscillator/oscillator.hpp"
#include <alsa/asoundlib.h>
#include <mutex>

class Synth {
private:
  Oscillator *osc;
  SoundPlayer *async_player;

public:
  Synth(Oscillator *osc);
  void start_keyboard(NoteMap *nm, std::mutex *map_mutex);
  void start_midi(NoteMap *nm, std::mutex *map_mutex);
};

#endif
