#include "global_controller.hpp"
#include "../messager/messager.hpp"
#include "../midi/midi_setup.hpp"
#include <functional>
#include <iostream>
#include <string>

std::vector<std::string> split_string(std::string str, char splitting_char) {
  std::vector<std::string> splitted;
  std::string current;
  for (auto it = str.begin(); it != str.end(); ++it) {
    if (*it == splitting_char) {
      splitted.push_back(current);
      current.clear();
    } else {
      current.push_back(*it);
    }
  }

  splitted.push_back(current);

  return splitted;
}

size_t GlobalController::selectDevice(std::vector<std::string> *device_names) {
  // need to sync back with messenger (condition_variable)
  // to know when the selection has been completed
  // because this operation should be synced
  messager.sendMessage("select devices");
  for (auto device = device_names->begin(); device != device_names->end();
       device++) {
    messager.sendMessage(*device);
  }
  messager.sendMessage("end of select devices");
  {
    std::unique_lock lock{wait_response_mutex};
    wait_response.wait(lock);
  }

  return selected_id;
}

void GlobalController::handleMessageReception(std::string message) {
  if (message.compare("start-grabbing-keyboard-input") == 0) {
    startKeyboardGrab();
  } else if (message.substr(0, message.length() - 2)
                 .compare("selected device index") == 0) {
    size_t idx =
        std::stoi(message.substr(message.length() - 2, message.length() - 2));
    notifyDeviceSelection(idx);
  } else if (message.substr(0, 2).compare("op") == 0) {
    // this means we have an update for an operator
    std::vector<std::string> message_parts = split_string(message, ' ');
    synth.updateOperator(std::stoi(message_parts[1]), message_parts[2],
                         std::stod(message_parts[3]));
  } else if (message.substr(0, 3).compare("lfo") == 0) {
    std::vector<std::string> message_parts = split_string(message, ' ');
    synth.updateLFO(std::stoi(message_parts[1]), message_parts[2],
                    std::stod(message_parts[3]));
  }

  else {
    std::cout << "message received does not match any " << message << std::endl;
    messager.sendMessage(message.append(" from server ;)"));
  }
}

void GlobalController::notifyDeviceSelection(size_t device_idx) {
  selected_id = device_idx;
  std::unique_lock lock{wait_response_mutex};
  wait_response.notify_one();
}

GlobalController::GlobalController()
    : synth{std::bind(&GlobalController::selectDevice, this,
                      std::placeholders::_1)},
      sniffer{}, messager{std::bind(&GlobalController::handleMessageReception,
                                    this, std::placeholders::_1)} {}

void GlobalController::startRunning() {
  std::atomic_bool sniffer_failed = false, midi_synth_failed = false,
                   keyboard_synth_failed = false, midi_setup_failed = false;

  NoteMap note_map = makeEmptyNoteMap();
  std::mutex note_map_mutex;

  MidiSetup midi_input;

  // TODO change so the constructor does not throw
  // like segmenting what can be done in constructor (no errors)
  // and what will be done in init (returning -1 in case of failure)
  try {
    midi_input = MidiSetup();
  } catch (const char *err) {
    std::cout << "Could not create the MIDI setup class\n";
    std::cout << err << std::endl;
    return;
  }

  if (!midi_input.midiSetup()) {
    std::cerr << "Could not initialize MIDI.\n";
    return;
  }

  std::thread messager_thread{[this]() {
    messager.startContext(&comm_established_mutex, &comm_established);
  }};

  std::thread sniffer_thread =
      std::thread([this, &note_map, &note_map_mutex, &sniffer_failed]() {
        if (sniffer.init() < 0) {
          sniffer_failed = true;
          return;
        }

        while (true) {
          waitForKeyboardGrab();

          if (sniffer.sniff(&note_map, &note_map_mutex) < 0) {
            sniffer_failed = true;
            return;
          }

          messager.sendMessage("end-keyboard-grab");
        }
      });

  // sync with messenger's established connection
  {
    std::unique_lock lock{comm_established_mutex};
    comm_established.wait(lock);
  }
  std::thread keyboard_synth_thread =
      std::thread([this, &note_map, &note_map_mutex, &keyboard_synth_failed]() {
        synth.start_keyboard(&note_map, &note_map_mutex);
        // catch the end of the keyboard synth and let the user know
        keyboard_synth_failed = true;
      });

  std::thread midi_input_thread = std::thread(
      [&note_map, &midi_input, &note_map_mutex, &midi_setup_failed]() {
        midi_input.midiSniffer(&note_map, &note_map_mutex);
        // catch the end of the midi sniffer and let the user know
        midi_setup_failed = true;
      });

  while (true) {
    if (midi_setup_failed) {
      std::cout << "midi setup thread has failed, closing program\n";
      exit(-1);
    }

    if (sniffer_failed) {
      std::cout << "keyboard sniffer thread has failed, closing program\n";
      exit(-1);
    }

    if (keyboard_synth_failed) {
      std::cout << "keyboard sound thread has failed, closing program\n";
      exit(-1);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  // not really necessary since the program runs indefinitely for now
  sniffer_thread.join();
  midi_input_thread.join();
  keyboard_synth_thread.join();
  messager_thread.join();

  freeNoteMap(&note_map);
}

void GlobalController::startKeyboardGrab() {
  std::lock_guard lock(start_grabbing_mutex);
  start_grabbing.notify_one();
}

void GlobalController::waitForKeyboardGrab() {
  std::unique_lock lock(start_grabbing_mutex);
  start_grabbing.wait(lock);
}
