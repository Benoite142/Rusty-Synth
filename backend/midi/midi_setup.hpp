#ifndef MIDI_SETUP
#define MIDI_SETUP

#include "../utils/note_map.hpp"
#include <alsa/asoundlib.h>
#include <mutex>
#include <thread>

class MidiSetup {
private:
  snd_seq_t *seq_handle;
  snd_seq_event_t *event;

public:
  MidiSetup();
  snd_seq_t *midiSetup();
  void midiSniffer(NoteMap *note_map, std::mutex *note_map_lock);
};
#endif