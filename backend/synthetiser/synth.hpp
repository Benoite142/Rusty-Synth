#ifndef SYNTH
#define SYNTH

#include "../utils/key_map.hpp"
#include "oscillator/oscillator.hpp"
#include <alsa/asoundlib.h>
#include <condition_variable>
#include <mutex>

class Synth {
private:
  Oscillator *osc;

public:
  Synth(Oscillator *osc);
  void start_keyboard(KeyMap *km, std::mutex *map_mutex);
  void start_midi(KeyMap *km, std::mutex *map_mutex);

  bool midi_input(snd_seq_t *seq_handle, bool should_play);
};

#endif
