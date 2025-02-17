#include "sniffer.hpp"
#include "../utils/x_utils.hpp"
#include <iostream>

KeyboardSniffer::KeyboardSniffer() {
  display = XOpenDisplay(0);
  if (display == NULL) {
    throw "Could not open display";
  }
}

void KeyboardSniffer::sniff(KeyMap *key_map, std::mutex *key_map_lock) {
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
      if (*key_map->keys == '0') {
        *key_map->keys = *mapKeyCodeToString(event.xkey);
        *key_map->has_updated_value = true;
      }
      key_map_lock->unlock();
    }

    if (event.type == KeyRelease) {
      // only catching releases once, so no need to double check in the map
      key_map_lock->lock();
      *key_map->keys = '0';
      *key_map->has_updated_value = true;
      key_map_lock->unlock();
    }
  }

  XUngrabKey(display, AnyKey, AnyModifier, root);
}
