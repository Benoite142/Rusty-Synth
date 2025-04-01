#include "midi_setup.hpp"
#include <algorithm>
#include <iostream>
#include <mutex>

MidiSetup::MidiSetup() {}

snd_seq_t *MidiSetup::midiSetup() {

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

void MidiSetup::midiSniffer(NoteMap *note_map, std::mutex *note_map_lock) {
  if (!seq_handle) {
    std::cerr << "Error: seq_handle is NULL\n";
    return;
  }

  size_t idx = 0;

  while (true) {
    if (snd_seq_event_input(seq_handle, &event) < 0) {
      std::cout << "Error with the MIDI event stream\n";
    } else {
      if (event->type == SND_SEQ_EVENT_NOTEON) {
        short note = event->data.note.note;
        int velocity = event->data.note.velocity;

        note_map_lock->lock();
        auto it = std::find(note_map->notes.begin(), note_map->notes.end(),
                            Note{.note_value = -1});

        if (it == note_map->notes.end() || !it->released) {
          note_map->notes[idx++ % 2] = {.note_value = note, .released = false};
          *note_map->has_updated_value = true;
        }

        note_map_lock->unlock();

      } else if (event->type == SND_SEQ_EVENT_NOTEOFF) {
        short note = event->data.note.note;

        note_map_lock->lock();
        auto it = std::find(note_map->notes.begin(), note_map->notes.end(),
                            Note{.note_value = note});
        if (it != note_map->notes.end()) {
          it->released = true;
          *note_map->has_updated_value = true;
        }
        note_map_lock->unlock();
      }
      snd_seq_free_event(event);
    }
  }
}
