#include "../keyboard_sniffer/sniffer.hpp"
#include <iostream>
#include <map>
#include <mutex>
#include <thread>

void wake_up() { std::cout << "woke up!\n"; }

int main() {
  KeyboardSniffer sniffer;
  try {
    sniffer = KeyboardSniffer();
  } catch (std::string err) {
    std::cout << "Could not create the sniffer\n";
    std::cout << err << std::endl;
    return -1;
  }

  // to see if we could simply have hardcoded array to have better performances
  // e.g. hardcoding for keys 'a' to 'l'
  std::map<char, bool> key_map{};
  std::mutex key_map_mutex;

  std::thread sniffer_thread =
      std::thread([&key_map, &sniffer, &key_map_mutex]() {
        sniffer.sniff(&key_map, &key_map_mutex, wake_up);
      });

  sniffer_thread.join();
}
