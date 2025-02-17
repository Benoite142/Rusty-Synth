#ifndef KEYBOARD_SNIFFER_HPP
#define KEYBOARD_SNIFFER_HPP

#include "../utils/key_map.hpp"
#include <X11/X.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <mutex>

class KeyboardSniffer {
private:
  Display *display;
  Window window;

public:
  KeyboardSniffer();
  void sniff(KeyMap *key_map, std::mutex *key_map_lock);
};

#endif
