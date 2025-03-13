#include "../keyboard_sniffer/sniffer.hpp"
#include "../synthetiser/synth.hpp"
#include "../utils/key_map.hpp"
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

int main() {
  KeyboardSniffer sniffer;
  try {
    sniffer = KeyboardSniffer();
  } catch (std::string err) {
    std::cout << "Could not create the sniffer\n";
    std::cout << err << std::endl;
    return -1;
  }

  // logic for knowing if threads are finished (i.e. error for now)
  // this needs rethinking as most errors are not caught with an exception
  // pattern but simply lead to early exits with error codes a solution would be
  // to return a value from functions
  std::atomic_bool sniffer_failed = false, midi_failed = false,
                   keyboard_synth_failed = false;

  KeyMap key_map = makeEmptyKeyMap();
  std::mutex key_map_mutex;

  SineOscillator sineOsc{0.5f, 440.0f};

  Synth synth{&sineOsc};

  std::thread sniffer_thread = std::thread(
      [&key_map, &sniffer, &key_map_mutex, &synth, &sniffer_failed]() {
        sniffer.sniff(&key_map, &key_map_mutex);
        // if ever we break out of the sniffer
        // it means an error occured and we want to let the user know
        sniffer_failed = true;
      });

  std::thread synth_thread =
      std::thread([&synth, &key_map, &key_map_mutex, &keyboard_synth_failed]() {
        synth.start_keyboard(&key_map, &key_map_mutex);
        // catch the end of the keyboard synth and let the user know
        keyboard_synth_failed = true;
      });

  std::thread midi_thread =
      std::thread([&synth, &key_map, &key_map_mutex, &midi_failed]() {
        synth.start_midi(&key_map, &key_map_mutex);
        midi_failed = true;
      });

  while (true) {

    if (sniffer_failed) {
      std::cout << "keyboard sniffer thread has failed, closing program\n";
      exit(-1);
    }

    if (keyboard_synth_failed) {
      std::cout << "keyboard sound thread has failed, closing program\n";
      exit(-1);
    }

    if (midi_failed) {
      std::cout << "midi thread has failed, closing program\n";
      exit(-1);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  // not really necessary since the program runs indefinitely for now
  sniffer_thread.join();
  synth_thread.join();
  midi_thread.join();

  freeKeyMap(&key_map);
}
