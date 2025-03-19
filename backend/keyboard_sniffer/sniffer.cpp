#include "sniffer.hpp"
#include "../utils/sound_conversions.hpp"
#include "../utils/x_utils.hpp"
#include <X11/Xlib.h>
#include <iostream>
#include <mutex>

KeyboardSniffer::KeyboardSniffer() {

  int status = XInitThreads();
  if (status == 0) {
    throw "Could not open XinitThreads";
  }

  display = XOpenDisplay(0);
  if (display == NULL) {
    throw "Could not open display";
  }
}

void KeyboardSniffer::sniff(NoteMap *note_map, std::mutex *note_map_lock) {

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
  int error;

  while (true) {
    XLockDisplay(display);

    error = XNextEvent(display, &event);

    if (error < 0) {
      std::cout << "error reading xevent\n";
    }

    if (event.type == KeyPress) {
      if (*mapKeyCodeToString(event.xkey) == 'E') {
        std::cout << "exiting sniffer\n";
        break;
      }
      note_map_lock->lock();
      // key press triggers multiple times
      // bypassing repeats by checking if it not currently active
      if (*note_map->notes == -1) {
        *note_map->notes = findMidiNote(*mapKeyCodeToString(event.xkey));
        *note_map->has_updated_value = true;
      }

      note_map_lock->unlock();
    }

    if (event.type == KeyRelease) {
      // only catching releases once, so no need to double check in the map
      note_map_lock->lock();
      *note_map->notes = -1;
      *note_map->has_updated_value = true;
      note_map_lock->unlock();
    }

    XUnlockDisplay(display);
  }

  XUngrabKey(display, AnyKey, AnyModifier, root);
}
