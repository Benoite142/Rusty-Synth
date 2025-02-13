#include "constants.h"
#include "oscillator.hpp"
#include <X11/X.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <alsa/asoundlib.h>
#include <alsa/pcm.h>
#include <iostream>
#include <ostream>
#include <semaphore>
#include <thread>
#include <vector>

std::binary_semaphore semaphoreMainToAudio{0};
std::binary_semaphore semaphoreAudioToMain{0};
unsigned int sampleRate = 44100;

int16_t floatTo16bits(float fFloat) {
  double dDouble = (double)fFloat;

  dDouble *= 32'767.0f;

  if (dDouble < -32'767.0f) {
    dDouble = -32'767.0f;
  } else if (dDouble > 32'767.0f) {
    dDouble = 32'767.0f;
  }

  return static_cast<int16_t>(dDouble);
}

int8_t floatTo8bits(float fFloat) {
  double dDouble = (double)fFloat;

  dDouble *= 127.0f;

  if (dDouble < -127.0f) {
    dDouble = -127.0f;
  } else if (dDouble > 127.0f) {
    dDouble = 127.0f;
  }

  return static_cast<int8_t>(dDouble);
}

void writeDataToBuffer(std::vector<short> *buffer, Oscillator osc,
                       int nombreTick) {
  buffer->clear();
  for (int i = 0; i < nombreTick; ++i) {
    buffer->push_back(floatTo16bits(osc.advance()));
  }
}

void PlaySound(snd_pcm_t *pcmHandle, std::vector<short> *buffer) {

  while (true) {
    semaphoreMainToAudio.acquire();
    snd_pcm_prepare(pcmHandle);
    if (buffer->size()) {
      // make sure snd_pcm_write_i is synched and buffer can be cleared
      snd_pcm_writei(pcmHandle, buffer->data(), buffer->size());
    }
    semaphoreAudioToMain.release();
  }
}

int main() {

  using size_format = int16_t;

  int error;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  int dir;
  Oscillator osc{0.5f, 440.0f};

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

  // params for playback audio
  snd_pcm_hw_params_alloca(&params);
  snd_pcm_hw_params_any(handle, params);
  snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
  snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
  snd_pcm_hw_params_set_channels(handle, params, 1);
  snd_pcm_hw_params_set_rate_near(handle, params, &sampleRate, &dir);

  if (snd_pcm_hw_params(handle, params) < 0) {
    std::cerr << "Error setting PCM parameters\n";
    return -1;
  }

  std::vector<short> buffer[BUFFER_SIZE];

  std::thread soundThread = std::thread(PlaySound, handle, std::ref(buffer));

  while (true) {

    writeDataToBuffer(buffer, osc, sampleRate * 2);
    semaphoreMainToAudio.release();
    semaphoreAudioToMain.acquire();

    sleep(1);
  }

  snd_pcm_drain(handle);
  snd_pcm_close(handle);
  if (soundThread.joinable())
    soundThread.join();

  return 0;
}
