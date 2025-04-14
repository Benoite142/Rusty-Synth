#include "global_controller.hpp"
#include "../messager/messager.hpp"
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

void GlobalController::executeMessageAsync(std::string message) {
  if (message.compare("select-midi-device") == 0) {
    connectMidi();
  }
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
    if (message_parts[2].compare("waveform") == 0) {
      synth.updateOperator(std::stoi(message_parts[1]) - 1, message_parts[2],
                           message_parts[3]);
    } else if (message_parts.size() == 5) {
      synth.updateOperator(std::stoi(message_parts[1]) - 1, message_parts[2],
                           message_parts[4]);
    } else {
      synth.updateOperator(std::stoi(message_parts[1]) - 1, message_parts[2],
                           std::stod(message_parts[3]));
    }
  } else if (message.substr(0, 3).compare("lfo") == 0) {
    std::vector<std::string> message_parts = split_string(message, ' ');
    if (message_parts[2].compare("waveform") == 0) {
      synth.updateLFO(std::stoi(message_parts[1]), message_parts[2],
                      message_parts[3]);
    } else {
      synth.updateLFO(std::stoi(message_parts[1]), message_parts[2],
                      std::stod(message_parts[3]));
    }
  } else if (message.compare("start-recording") == 0) {
    synth.startRecording();
  } else if (message.compare("stop-recording") == 0) {
    synth.stopRecording();
    wav_writer.writeWav();
  } else if (message.substr(0, 3).compare("lpf") == 0) {
    synth.updateLowPassFilter(std::stod(split_string(message, ' ')[1]));
  } else if (message.substr(0, 3).compare("hpf") == 0) {
    synth.updateHighPassFilter(std::stod(split_string(message, ' ')[1]));
  } else if (message.substr(0, 13).compare("number-voices") == 0) {
    size_t voices = std::stoi(split_string(message, '-')[2]);
    synth.setNumberOfVoices(voices);
    note_map_mutex.lock();
    updateNoteMapSize(&note_map, voices);
    note_map_mutex.unlock();
  } else if (message.compare("select-midi-device") == 0) {
    pending_messages.push(message);
    std::unique_lock lock{pending_messages_mutex};
    pending_messages_signal.notify_one();
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
                                    this, std::placeholders::_1)},
      wav_writer{} {
  midi_controller = MidiSetup();
}

void GlobalController::startRunning() {
  std::atomic_bool sniffer_failed = false, midi_synth_failed = false,
                   keyboard_synth_failed = false, midi_setup_failed = false;

  std::thread async_messaging{[this]() {
    while (true) {
      {
        std::unique_lock lock{pending_messages_mutex};
        pending_messages_signal.wait(lock);
      }

      executeMessageAsync(pending_messages.front());
      pending_messages.pop();
    }
  }};

  note_map = makeEmptyNoteMap();

  std::thread messager_thread{[this]() {
    messager.startContext(&comm_established_mutex, &comm_established);
  }};

  std::thread keyboard_sniffer_thread = std::thread([this, &sniffer_failed]() {
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
  std::thread synth_thread = std::thread([this, &keyboard_synth_failed]() {
    synth.start_keyboard(&note_map, &note_map_mutex);
    // catch the end of the keyboard synth and let the user know
    keyboard_synth_failed = true;
  });

  std::thread midi_input_thread = std::thread([this, &midi_setup_failed]() {
    {
      std::unique_lock lock(midi_input_mutex);
      grab_midi_input.wait(lock);
    }
    midi_controller.midiSniffer(&note_map, &note_map_mutex);
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
  keyboard_sniffer_thread.join();
  midi_input_thread.join();
  synth_thread.join();
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

void GlobalController::connectMidi() {
  midi_controller.midiSetup(
      std::bind(&GlobalController::selectDevice, this, std::placeholders::_1));

  {
    std::unique_lock lock(midi_input_mutex);
    grab_midi_input.notify_one();
  }
}
