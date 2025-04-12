#ifndef SYNTH
#define SYNTH

#include "../sound_player/sound_player.hpp"
#include "../utils/note_map.hpp"
#include "filters/filters.hpp"
#include "low_frequency_oscillator/low_frequency_oscillator.hpp"
#include "operator/operator.hpp"
#include <alsa/asoundlib.h>
#include <cstddef>
#include <functional>
#include <mutex>
#include <vector>

class Synth {
private:
  SoundPlayer *async_player;
  size_t numberOfVoices = 2;
  Operator synth_operator;
  Operator synth_operator2;
  Operator synth_operator3;
  Operator synth_operator4;
  LowFrequencyOscillator lfo_1;
  HighPassFilter high_pass_filter;
  LowPassFilter low_pass_filter;
  std::function<size_t(std::vector<std::string> *)> selectDeviceCallback;

public:
  Synth(std::function<size_t(std::vector<std::string> *)> selectDeviceCallback);
  ~Synth();
  void start_keyboard(NoteMap *nm, std::mutex *map_mutex);
  void updateOperator(size_t operator_index, std::string operator_field,
                      double value);
  void updateLFO(size_t lfo_index, std::string lfo_field, double value);
  void startRecording();
  void stopRecording();
  void updateLowPassFilter(double value);
  void updateHighPassFilter(double value);
  void setNumberOfVoices(size_t newNumberOfVoices);
};

#endif
