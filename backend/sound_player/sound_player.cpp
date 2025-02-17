#include "sound_player.hpp"
#include <iostream>

SoundPlayer::SoundPlayer() {
  int error;
  snd_pcm_hw_params_t *params;
  int dir;

  // In case of errors when getting audio device
  error = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
  if (error < 0) {
    std::cout << "error open " << error << std::endl;
  }
  error =
      snd_pcm_set_params(handle, SND_PCM_FORMAT_S16_LE,
                         SND_PCM_ACCESS_RW_INTERLEAVED, 1, 44'100, 1, 100'000);
  if (error < 0) {
    std::cout << "error set params " << error << std::endl;
  }

  // fix later pls
  unsigned int sampleRate = 44'100;

  // params for playback audio
  snd_pcm_hw_params_alloca(&params);
  snd_pcm_hw_params_any(handle, params);
  snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
  snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
  snd_pcm_hw_params_set_channels(handle, params, 1);
  snd_pcm_hw_params_set_rate_near(handle, params, &sampleRate, &dir);

  if (snd_pcm_hw_params(handle, params) < 0) {
    std::cerr << "Error setting PCM parameters\n";
    exit(-1);
  }
}

void SoundPlayer::playSound(std::vector<short> *buffer,
                            std::binary_semaphore *bufferInputSemaphore,
                            std::binary_semaphore *bufferOutputSemaphore) {

  while (true) {
    bufferInputSemaphore->acquire();
    snd_pcm_prepare(handle);
    if (buffer->size()) {
      // make sure snd_pcm_write_i is synched and buffer can be cleared
      // will need dual buffer to write, if we try to write and pcm is in use
      // call snd_pcm_wait() with SND_PCM_WAIT_DRAIN as value
      snd_pcm_writei(handle, buffer->data(), buffer->size());
    }
    bufferOutputSemaphore->release();
  }

  snd_pcm_drain(handle);
  snd_pcm_close(handle);
}
