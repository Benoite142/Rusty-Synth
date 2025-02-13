#include "sniffer.hpp"
#include "../utils/x_utils.h"
#include <iostream>

KeyboardSniffer::KeyboardSniffer() {
  display = XOpenDisplay(0);
  if (display == NULL) {
    throw "Could not open display";
  }
}

void KeyboardSniffer::sniff(std::map<char, bool> *key_map,
                            std::mutex *key_map_lock,
                            void (*wake_up_fn)(void)) {
  // find the correct window here
  Window root = DefaultRootWindow(display);

  XGrabKey(display, AnyKey, AnyModifier, root, False, GrabModeAsync,
           GrabModeAsync);

  int supported_autorepeat_detect;

  XkbSetDetectableAutoRepeat(display, true, &supported_autorepeat_detect);

  if (supported_autorepeat_detect) {
    std::cout << "supported\n";
  } else {
    std::cout << "not suported :(\n";
  }

  XEvent event;

  while (true) {
    XNextEvent(display, &event);

    if (event.type == KeyPress) {
      if (*mapKeyCodeToString(event.xkey) == 'q') {
        return;
      }

      key_map_lock->lock();
      // key press triggers multiple times
      // bypassing repeats by checking if it not currently active
      if (key_map->find(*mapKeyCodeToString(event.xkey)) == key_map->end() ||
          !key_map->find(*mapKeyCodeToString(event.xkey))->second) {
        key_map->insert_or_assign(*mapKeyCodeToString(event.xkey), true);
        wake_up_fn();
      }
      key_map_lock->unlock();
    }

    if (event.type == KeyRelease) {
      // only catching releases once, so no need to double check in the map
      key_map_lock->lock();
      key_map->insert_or_assign(*mapKeyCodeToString(event.xkey), false);
      wake_up_fn();
      key_map_lock->unlock();
    }
  }

  XUngrabKey(display, AnyKey, AnyModifier, root);
}
