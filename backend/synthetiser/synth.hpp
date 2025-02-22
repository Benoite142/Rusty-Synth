#ifndef SYNTH
#define SYNTH

#include "../utils/key_map.hpp"
#include "oscillator/oscillator.hpp"
#include <alsa/asoundlib.h>
#include <mutex>
#include <semaphore>
#include <vector>

class Synth {
private:
  Oscillator *osc;

public:
  Synth(Oscillator *osc);
  void start(std::binary_semaphore *bufferInputSemaphore,
             std::binary_semaphore *bufferOutputSemaphore,
             std::vector<short> *buffer, KeyMap *km, std::mutex *map_mutex);
  bool midi_input(snd_seq_t *seq_handle, bool should_play);
};

#endif
