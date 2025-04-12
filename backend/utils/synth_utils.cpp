#include "synth_utils.hpp"
#include "../synthetiser/constants.h"
#include "note_map.hpp"
#include "sound_conversions.hpp"
#include <iostream>

void writeDataToBuffer(std::vector<float> *buffer, Oscillator *osc,
                       int nombreTick) {

  buffer->clear();
  for (int i = 0; i < nombreTick; ++i) {
    // remember to do conversions here and not assume
    // that 32 bit floats will be the default
    buffer->push_back(osc->advance());
  }
}

void async_player_callback(snd_async_handler_t *ahandler) {
  snd_pcm_t *handle = snd_async_handler_get_pcm(ahandler);
  private_data *data = static_cast<private_data *>(
      snd_async_handler_get_callback_private(ahandler));
  float *buffer = data->buffer;
  snd_pcm_sframes_t avail;
  int error = 0;
  avail = snd_pcm_avail_update(handle);
  while (avail >= BUFFER_SIZE) {

    data->map_mutex->lock();
    if (data->note_map->has_updated_value) {
      for (size_t i = 0; i < data->note_map->notes.size(); ++i) {

        // now update the frequencies
        Note note = data->note_map->notes[i];
        if (!note.released) {
          auto freq = calculate_frequency(note.note_value);

          data->synth_operator->updateFrequency(i, freq);
          data->synth_operator2->updateFrequency(i, freq);
          data->synth_operator3->updateFrequency(i, freq);
          data->synth_operator4->updateFrequency(i, freq);
        } else {
          data->synth_operator->releaseNote(i);
          data->synth_operator2->releaseNote(i);
          data->synth_operator3->releaseNote(i);
          data->synth_operator4->releaseNote(i);
        }
      }
    }
    data->map_mutex->unlock();

    for (int i = 0; i < BUFFER_SIZE; ++i) {
      data->synth_operator->advanceLFO();
      float mixed_sample = 0.0f;
      mixed_sample += data->synth_operator->advance();
      mixed_sample += data->synth_operator2->advance();
      mixed_sample += data->synth_operator3->advance();
      mixed_sample += data->synth_operator4->advance();
      mixed_sample = data->synth_operator->process(mixed_sample);
      buffer[i] = mixed_sample * 0.25f;
      // so the notes dont clip with themeselves
      // might need to find a better way to do this

      buffer[i] /= data->synth_operator->getNumberOfVoices();
    }
    error = snd_pcm_writei(handle, buffer, BUFFER_SIZE);
    if (*data->is_recording) {
      data->recording->write(reinterpret_cast<char *>(buffer),
                             BUFFER_SIZE * sizeof(float));
    }

    if (error < 0) {
      std::cout << "error in callback write " << snd_strerror(error)
                << std::endl;
      if (try_recover_pipe(handle, error) < 0) {
        std::cout << "could not recover pipe\n";
        exit(-1);
      }
    }
    if (error != BUFFER_SIZE) {
      // this probably breaks the pipe
      // exit is quite excessive
      std::cout << "callback did not write expected\n";
      exit(-1);
    }

    avail = snd_pcm_avail_update(handle);
  }
}

int try_recover_pipe(snd_pcm_t *handle, int err) {
  std::cout << "recovering...\n";
  return snd_pcm_recover(handle, err, 0);
}

/////////////
/// this whole section has been adapted from the juce source code
/// https://github.com/juce-framework/JUCE/blob/master/modules/juce_audio_devices/native/juce_ALSA_linux.cpp

void getDeviceSampleRates(snd_pcm_t *handle) {
  snd_pcm_hw_params_t *hwParams;
  snd_pcm_hw_params_alloca(&hwParams);

  if (snd_pcm_hw_params_any(handle, hwParams) >= 0 &&
      snd_pcm_hw_params_test_rate(handle, hwParams, 44'100, 0) == 0) {
    std::cout << "rate 44'100 is supported\n";
  }
}

void getDeviceNumChannels(snd_pcm_t *handle, unsigned int *minChans,
                          unsigned int *maxChans) {
  snd_pcm_hw_params_t *params;
  snd_pcm_hw_params_alloca(&params);

  if (snd_pcm_hw_params_any(handle, params) >= 0) {
    snd_pcm_hw_params_get_channels_min(params, minChans);
    snd_pcm_hw_params_get_channels_max(params, maxChans);

    // some virtual devices (dmix for example) report 10000 channels , we have
    // to clamp these values
    *maxChans = std::min(*maxChans, 256u);
    *minChans = std::min(*minChans, *maxChans);
  } else {
    std::cout << "could not get number of channels for device\n";
  }
}

void getDeviceProperties(const std::string &deviceID, unsigned int &minChansOut,
                         unsigned int &maxChansOut) {

  minChansOut = maxChansOut = 0;

  if (deviceID.empty())
    return;

  snd_pcm_info_t *info;
  snd_pcm_info_alloca(&info);

  snd_pcm_t *pcmHandle;

  if (snd_pcm_open(&pcmHandle, deviceID.c_str(), SND_PCM_STREAM_PLAYBACK,
                   SND_PCM_NONBLOCK) >= 0) {
    getDeviceNumChannels(pcmHandle, &minChansOut, &maxChansOut);
    getDeviceSampleRates(pcmHandle);

    snd_pcm_close(pcmHandle);
  }
}

/// end of juce adapted code
//////////////////////
