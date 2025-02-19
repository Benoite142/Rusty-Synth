#include "synth.hpp"
#include "../utils/key_map.hpp"
#include "../utils/sound_conversions.hpp"
#include "../utils/synth_utils.hpp"
#include "constants.h"
#include "oscillator/oscillator.hpp"
#include <chrono>
#include <thread>

Synth::Synth(Oscillator *osc) { this->osc = osc; }

void Synth::start(std::binary_semaphore *bufferInputSemaphore,
                  std::binary_semaphore *bufferOutputSemaphore,
                  std::vector<short> *buffer, KeyMap *km,
                  std::mutex *map_mutex) {

  bool should_play = false;

  while (true) {
    map_mutex->lock();
    if (*km->has_updated_value) {
      // this should update the frequencies
      if (*km->keys != '/') {
        should_play = true;
        osc->setFrequency(calculate_frequency(FindKeyIndex(*km->keys)));

      } else {
        should_play = false;
      }

      *km->has_updated_value = false;
    }

    map_mutex->unlock();

    if (should_play) {
      writeDataToBuffer(buffer, osc, SAMPLE_RATE / 10);
      bufferInputSemaphore->release();
      bufferOutputSemaphore->acquire();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
