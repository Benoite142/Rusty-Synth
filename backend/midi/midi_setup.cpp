#include "midi_setup.hpp"
#include <iostream>

MidiSetup::MidiSetup() {}

snd_seq_t *MidiSetup::midiSetup() {
  int port;

  if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) {
    std::cerr << "Error opening the ALSA sequencer. \n";
  }

  snd_seq_set_client_name(seq_handle, "Synthesiser-Duo_Application");

  if (snd_seq_create_simple_port(seq_handle, "Input",
                                 SND_SEQ_PORT_CAP_WRITE |
                                     SND_SEQ_PORT_CAP_SUBS_WRITE,
                                 SND_SEQ_PORT_TYPE_APPLICATION) < 0) {
    std::cerr << "Error creating a MIDI input port for the application. \n";
  }
  std::cout << "MIDI input setup completed. \n";

  return seq_handle;
}