#ifndef SYNTH
#define SYNTH

#include "../sound_player/sound_player.hpp"
#include "../utils/note_map.hpp"
#include "low_frequency_oscillator/low_frequency_oscillator.hpp"
#include "operator/operator.hpp"
#include <alsa/asoundlib.h>
#include <functional>
#include <mutex>
#include <vector>

class Synth {
private:
  SoundPlayer *async_player;
  Operator synth_operator;
  LowFrequencyOscillator lfo_1;
  LowFrequencyOscillator lfo_2;
  std::function<size_t(std::vector<std::string> *)> selectDeviceCallback;

public:
  Synth(std::function<size_t(std::vector<std::string> *)> selectDeviceCallback);
  ~Synth();
  void start_keyboard(NoteMap *nm, std::mutex *map_mutex);
  void updateOperator(size_t operator_index, std::string operator_field,
                      double value);
};

#endif
