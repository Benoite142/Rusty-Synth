#include "sniffer.hpp"
#include "../utils/sound_conversions.hpp"
#include "../utils/x_utils.hpp"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <chrono>
#include <iostream>
#include <mutex>
#include <optional>
#include <thread>

int KeyboardSniffer::init() {
  int status = XInitThreads();
  if (status == 0) {
    std::cout << "could not init x11 threads\n";
    return -1;
  }

  display = XOpenDisplay(0);
  if (display == NULL) {
    std::cout << "could not open default display\n";
    return -1;
  }

  std::optional<Window> possible_window;

  size_t attempt = 0;
  while (attempt++ < 10) {
    possible_window = X11findWindow(display);

    if (possible_window.has_value()) {
      window = possible_window.value();
      return 0;
    }

    // timeout 1 second to let interface boot
    // if after 10 tries it doesnt boot we abandon
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  std::cout << "could not find interface\n";
  return -1;
}

void KeyboardSniffer::sniff(NoteMap *note_map, std::mutex *note_map_lock) {

  // find the correct window here
  Window root = DefaultRootWindow(display);

  XGrabKeyboard(display, window, false, GrabModeAsync, GrabModeAsync,
                CurrentTime);

  int supported_autorepeat_detect;

  XkbSetDetectableAutoRepeat(display, true, &supported_autorepeat_detect);
  if (supported_autorepeat_detect) {
    std::cout << "supported\n";
  } else {
    std::cout << "not suported :(\n";
  }

  XEvent event;
  int error;
  size_t idx = 0;
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
      short midi_note = findMidiNote(*mapKeyCodeToString(event.xkey));

      auto it = std::find(note_map->notes.begin(), note_map->notes.end(),
                          Note{.note_value = midi_note});
      if (it == note_map->notes.end() || it->released) {
        note_map->notes[idx++ % note_map->notes.size()] = {
            .note_value = midi_note, .released = false};
        *note_map->has_updated_value = true;
      }
      note_map_lock->unlock();
    }

    if (event.type == KeyRelease) {
      short midi_note = findMidiNote(*mapKeyCodeToString(event.xkey));
      auto it = std::find(note_map->notes.begin(), note_map->notes.end(),
                          Note{.note_value = midi_note});
      note_map_lock->lock();
      if (it != note_map->notes.end()) {
        it->released = true;
        *note_map->has_updated_value = true;
      }

      note_map_lock->unlock();
    }

    XUnlockDisplay(display);
  }

  XUngrabKey(display, AnyKey, AnyModifier, root);
}
