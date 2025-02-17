#ifndef SOUND_PLAYER
#define SOUND_PLAYER

#include <alsa/asoundlib.h>
#include <semaphore>
#include <vector>

class SoundPlayer {
private:
  snd_pcm_t *handle;

public:
  SoundPlayer();
  void playSound(std::vector<short> *buffer,
                 std::binary_semaphore *bufferInputSemaphore,
                 std::binary_semaphore *bufferOutputSemaphore);
};

#endif
