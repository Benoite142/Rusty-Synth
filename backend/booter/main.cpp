#include "../keyboard_sniffer/sniffer.hpp"
#include "../midi/midi_setup.hpp"
#include "../synthetiser/synth.hpp"
#include "../utils/note_map.hpp"
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

int main() {
  KeyboardSniffer sniffer = KeyboardSniffer();
  MidiSetup midi_input;

  if (sniffer.init() < 0) {
    return -1;
  }

  try {
    midi_input = MidiSetup();
  } catch (const char *err) {
    std::cout << "Could not create the MIDI setup class\n";
    std::cout << err << std::endl;
    return -1;
  }

  if (!midi_input.midiSetup()) {
    std::cerr << "Could not initialize MIDI.\n";
    return -1;
  }
  // logic for knowing if threads are finished (i.e. error for now)
  // this needs rethinking as most errors are not caught with an exception
  // pattern but simply lead to early exits with error codes a solution would be
  // to return a value from functions
  std::atomic_bool sniffer_failed = false, midi_synth_failed = false,
                   keyboard_synth_failed = false, midi_setup_failed = false;

  NoteMap note_map = makeEmptyNoteMap();
  std::mutex note_map_mutex;

  SineOscillator sineOsc{0.5f, 440.0f};

  Synth synth{&sineOsc};

  std::thread sniffer_thread =
      std::thread([&note_map, &sniffer, &note_map_mutex, &sniffer_failed]() {
        sniffer.sniff(&note_map, &note_map_mutex);
        // if ever we break out of the sniffer
        // it means an error occured and we want to let the user know
        sniffer_failed = true;
      });

  std::thread midi_input_thread = std::thread(
      [&note_map, &midi_input, &note_map_mutex, &midi_setup_failed]() {
        midi_input.midiSniffer(&note_map, &note_map_mutex);
        // catch the end of the midi sniffer and let the user know
        midi_setup_failed = true;
      });

  std::thread keyboard_synth_thread = std::thread(
      [&synth, &note_map, &note_map_mutex, &keyboard_synth_failed]() {
        synth.start_keyboard(&note_map, &note_map_mutex);
        // catch the end of the keyboard synth and let the user know
        keyboard_synth_failed = true;
      });

  std::thread midi_synth_thread =
      std::thread([&synth, &note_map, &note_map_mutex, &midi_synth_failed]() {
        // temp commented out
        // synth.start_midi(&note_map, &note_map_mutex);
        // catch the end of the midi synth and let the user know
        while (true) {
          std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        }
        midi_synth_failed = true;
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

    if (midi_synth_failed) {
      std::cout << "midi thread has failed, closing program\n";
      exit(-1);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  // not really necessary since the program runs indefinitely for now
  sniffer_thread.join();
  midi_input_thread.join();
  keyboard_synth_thread.join();
  midi_synth_thread.join();

  freeNoteMap(&note_map);
}
