#ifndef SOUND_PLAYER
#define SOUND_PLAYER

#include "../synthetiser/oscillator/oscillator.hpp"
#include "../utils/note_map.hpp"
#include <alsa/asoundlib.h>
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

public:
  SoundPlayer();
  void playAsync(float *buffer, std::vector<Oscillator> *osc, NoteMap *note_map,
                 std::mutex *map_mutex);

private:
  void setup_pipe();
  bool setParameters(unsigned int sampleRate, int numChannels, int bufferSize);
};

#endif
