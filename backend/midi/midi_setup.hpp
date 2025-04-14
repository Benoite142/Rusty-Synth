#ifndef MIDI_SETUP
#define MIDI_SETUP

#include "../utils/note_map.hpp"
#include <alsa/asoundlib.h>
#include <functional>
#include <mutex>
#include <string>

class MidiSetup {
private:
  snd_seq_t *seq_handle;
  snd_seq_event_t *event;

public:
  MidiSetup();
  void midiSetup(
      std::function<int(std::vector<std::string> *)> device_selection_callback);
  void midiSniffer(NoteMap *note_map, std::mutex *note_map_lock);
};
#endif
