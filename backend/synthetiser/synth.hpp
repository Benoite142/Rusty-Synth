#ifndef SYNTH
#define SYNTH

#include "../utils/note_map.hpp"
#include "oscillator/oscillator.hpp"
#include <alsa/asoundlib.h>
#include <condition_variable>
#include <mutex>

class Synth {
private:
  Oscillator *osc;

public:
  Synth(Oscillator *osc);
  void start_keyboard(NoteMap *nm, std::mutex *map_mutex);
  void start_midi(NoteMap *nm, std::mutex *map_mutex);
};

#endif
