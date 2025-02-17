#include "x_utils.hpp"
#include <X11/XKBlib.h>

char *mapKeyCodeToString(XKeyEvent event) {

  KeySym key = XkbKeycodeToKeysym(event.display, event.keycode, 0, 0);
  return XKeysymToString(key);
}
