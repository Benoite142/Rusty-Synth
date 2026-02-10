#include "midi_setup.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <mutex>
#include <vector>

MidiSetup::MidiSetup() {}

void MidiSetup::midiSetup(
    std::function<int(std::vector<std::string> *)> device_selection_callback) {

  if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) {
    std::cerr << "Error opening the ALSA sequencer. \n";
  }
  snd_seq_set_client_name(seq_handle, "Synthesiser-Duo-Application");

  if (snd_seq_create_simple_port(seq_handle, "Input",
                                 SND_SEQ_PORT_CAP_WRITE |
                                     SND_SEQ_PORT_CAP_SUBS_WRITE,
                                 SND_SEQ_PORT_TYPE_APPLICATION) < 0) {
    std::cerr << "Error creating a MIDI input port for the application. \n";
  }

  // list available MIDI devices
  std::vector<std::pair<int, int>> devices;
  std::vector<std::string> device_names;

  snd_seq_client_info_t *cinfo;
  snd_seq_client_info_alloca(&cinfo);

  int device_number = 0;
  int client = -1;

  while (snd_seq_query_next_client(seq_handle, cinfo) >= 0) {
    client = snd_seq_client_info_get_client(cinfo);
    std::string client_name = snd_seq_client_info_get_name(cinfo);

    snd_seq_port_info_t *port_info;
    snd_seq_port_info_alloca(&port_info);

    snd_seq_port_info_set_client(port_info, client);

    // on doit le mettre a -1 sinon on skip les ports 0 :/
    snd_seq_port_info_set_port(port_info, -1);

    while (snd_seq_query_next_port(seq_handle, port_info) >= 0) {
      int port = snd_seq_port_info_get_port(port_info);
      std::string port_name = snd_seq_port_info_get_name(port_info);

      // skip ports and clients that we do want the application to connect to
      // (ex:synth application)
      unsigned int caps = snd_seq_port_info_get_capability(port_info);
      if (!(caps & SND_SEQ_PORT_CAP_READ)) {
        continue;
      }

      devices.emplace_back(client, port);
      device_names.push_back(client_name + " - " + port_name);

      std::cout << "Device " << device_number++ << ": " << client << ":" << port
                << " - " << client_name << " (" << port_name << ")\n";
    }
  }

  int selected_index = device_selection_callback(&device_names);

  std::cout << "received select device " << selected_index << std::endl;

  if (selected_index >= 0 && selected_index < devices.size()) {
    int selected_client = devices[selected_index].first;
    int selected_port = devices[selected_index].second;
    std::cout << "Selected device: " << selected_client << ":" << selected_port
              << " - " << device_names[selected_index] << "\n";
    if (snd_seq_connect_from(seq_handle, 0, selected_client, selected_port) <
        0) {
      std::cerr << "Error connecting to MIDI device.\n";
    }
  } else {
    std::cerr << "Invalid selection.\n";
  }

  std::cout << "MIDI input setup completed. \n";
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

        if (velocity == 0) {
          note_map_lock->lock();
          for (auto &n : note_map->notes) {
            if (n.note_value == note && !n.released) {
              n.released = true;
              note_map->current_voices--;
              *note_map->has_updated_value = true;
              break;
            }
          }
          note_map_lock->unlock();

        } else {
          note_map_lock->lock();
          bool already_playing = false;
          for (const auto &n : note_map->notes) {
            if (n.note_value == note && !n.released) {
              already_playing = true;
              break;
            }
          }

          if (!already_playing) {

            note_map->notes[idx++ % note_map->notes.size()] = {
                .note_value = note, .released = false};
            note_map->current_voices++;
            *note_map->has_updated_value = true;
          }

          note_map_lock->unlock();
        }

      } else if (event->type == SND_SEQ_EVENT_NOTEOFF) {
        short note = event->data.note.note;

        note_map_lock->lock();
        auto it = std::find(note_map->notes.begin(), note_map->notes.end(),
                            Note{.note_value = note});
        if (it != note_map->notes.end()) {
          it->released = true;
          note_map->current_voices--;
          *note_map->has_updated_value = true;
        }
        note_map_lock->unlock();
      }
      snd_seq_free_event(event);
    }
  }
}
