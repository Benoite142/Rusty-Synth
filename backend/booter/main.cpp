#include "../keyboard_sniffer/sniffer.hpp"
#include "../sound_player/sound_player.hpp"
#include "../synthetiser/synth.hpp"
#include "../utils/key_map.hpp"
#include <iostream>
#include <mutex>
#include <semaphore>
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

  KeyMap key_map = makeEmptyKeyMap();
  std::mutex key_map_mutex;

  std::binary_semaphore bufferInput{1}, bufferOutput{0};

  SineOscillator sineOsc{0.5f, 440.0f};

  Synth synth{&sineOsc};

  std::thread sniffer_thread =
      std::thread([&key_map, &sniffer, &key_map_mutex, &synth]() {
        sniffer.sniff(&key_map, &key_map_mutex);
      });

  std::vector<short> buffer[BUFFER_SIZE];

  std::thread synth_thread = std::thread([&synth, &bufferInput, &bufferOutput,
                                          &buffer, &key_map, &key_map_mutex]() {
    synth.start_keyboard(&bufferInput, &bufferOutput, std::ref(buffer),
                         &key_map, &key_map_mutex);
  });
  std::thread midi_thread =
      std::thread([&synth, &bufferInput, &bufferOutput, &buffer]() {
        synth.start_midi(&bufferInput, &bufferOutput, std::ref(buffer));
      });

  SoundPlayer sound_player{};

  std::thread sound_thread =
      std::thread([&sound_player, &buffer, &bufferInput, &bufferOutput]() {
        sound_player.playSound(std::ref(buffer), &bufferInput, &bufferOutput);
      });

  sniffer_thread.join();
  synth_thread.join();
  midi_thread.join();
  sound_thread.join();

  freeKeyMap(&key_map);
}
