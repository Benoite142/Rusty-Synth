#include "synth.hpp"
#include "../midi/midi_setup.hpp"
#include "../sound_player/sound_player.hpp"
#include "../utils/key_map.hpp"
#include "../utils/sound_conversions.hpp"
#include "constants.h"
#include "oscillator/oscillator.hpp"
#include <iostream>

Synth::Synth(Oscillator *osc) { this->osc = osc; }

void Synth::start_keyboard(KeyMap *km, std::mutex *map_mutex) {

  SoundPlayer async_player{};

  float buffer[BUFFER_SIZE];

  // playing in async mode still uses up a thread
  async_player.playAsync(buffer, this->osc, km, map_mutex);

  // for now asserting here since we should never reach
  assert(false);
}

void Synth::start_midi(KeyMap *km, std::mutex *map_mutex) {
  MidiSetup midi;
  snd_seq_t *seq_handle = midi.midiSetup();
  bool should_play = false;

  std::cout << "MIDI input is currently not available\n";

  while (true) {
    should_play = midi_input(seq_handle, should_play);

    /*
    if (should_play) {
      writeDataToBuffer(buffer, osc, SAMPLE_RATE / 10);
      bufferInputSemaphore->release();
      bufferOutputSemaphore->acquire();
    }
    */
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
