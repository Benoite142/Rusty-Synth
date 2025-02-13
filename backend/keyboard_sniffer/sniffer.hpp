#ifndef KEYBOARD_SNIFFER_HPP
#define KEYBOARD_SNIFFER_HPP

#include <X11/X.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <map>
#include <mutex>

class KeyboardSniffer {
private:
  Display *display;
  Window window;

public:
  KeyboardSniffer();
  void sniff(std::map<char, bool> *key_map, std::mutex *key_map_lock,
             void (*wake_up_fn)(void));
};

#endif
