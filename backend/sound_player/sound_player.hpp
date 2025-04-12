#ifndef SOUND_PLAYER
#define SOUND_PLAYER

#include "../synthetiser/operator/operator.hpp"
#include "../utils/note_map.hpp"
#include <alsa/asoundlib.h>
#include <atomic>
#include <fstream>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

class SoundPlayer {
private:
  snd_async_handler_t *ahandler;
  snd_pcm_t *handle;
  std::string selected_device;
  int bitDepth;
  int latency;
  int numChannelsRunning;
  std::atomic<bool> is_recording;
  std::ofstream temp_output_buffer;

public:
  SoundPlayer(
      std::function<size_t(std::vector<std::string> *)> selectDeviceCallback);
  void playAsync(float *buffer, Operator *synth_operator,
                 Operator *synth_operator2, Operator *synth_operator3,
                 Operator *synth_operator4, NoteMap *note_map,
                 std::mutex *map_mutex);
  void startRecording();
  void stopRecording();

private:
  void setup_pipe();
  bool setParameters(unsigned int sampleRate, int numChannels, int bufferSize);
};

#endif
