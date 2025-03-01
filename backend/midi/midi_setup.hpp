#ifndef MIDI_SETUP
#define MIDI_SETUP

#include <alsa/asoundlib.h>

class MidiSetup {
private:
  snd_seq_t *seq_handle;

public:
  MidiSetup();
  snd_seq_t *midiSetup();
};
#endif