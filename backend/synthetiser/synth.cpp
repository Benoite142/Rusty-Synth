#include "synth.hpp"
#include "../midi/midi_setup.hpp"
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
  MidiSetup midiSetup{};
  snd_seq_t *seq_handle = midiSetup.get_seq_handle();

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

    should_play = midi_input(seq_handle, should_play);

    if (should_play) {
      writeDataToBuffer(buffer, osc, SAMPLE_RATE / 10);
      bufferInputSemaphore->release();
      bufferOutputSemaphore->acquire();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

// Plays one time for now, can't hold
bool Synth::midi_input(snd_seq_t *seq_handle, bool should_play) {
  snd_seq_event_t *event;
  if (snd_seq_event_input(seq_handle, &event) >= 0) {

    if (event->type == SND_SEQ_EVENT_NOTEON) {
      int note = event->data.note.note;
      int velocity = event->data.note.velocity;
      if (velocity > 0) {
        should_play = true;
        osc->setFrequency(calculate_frequency(note));
      }
    } else if (event->type == SND_SEQ_EVENT_NOTEOFF) {
      should_play = false;
    }
    snd_seq_free_event(event);
  }
  return should_play;
}