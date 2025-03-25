#include "x_utils.hpp"
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <iostream>

char *mapKeyCodeToString(XKeyEvent event) {

  KeySym key = XkbKeycodeToKeysym(event.display, event.keycode, 0, 0);
  return XKeysymToString(key);
}

// the following 2 functions are derived from
// https://gist.github.com/caiorss/639df76864d014ead12936fbd361be73#file-x11-tool-cpp
std::string x11_get_window_name(Display *display, Window window) {
  XTextProperty name;

  return XGetWMName(display, window, &name)
             ? std::string(reinterpret_cast<char *>(name.value))
             : "";
}

/** The synth window is unnamed for some reason
 *  The way we find it is by identifying its child window
 *  That child window has the interface's name in it
 *
 *  That means we have to get all root windows
 *  And then for every window found, query children
 *
 *  The layout looks a little like this
 *
 *  root {
 *    some_window {
 *      window_child1
 *      window_child2
 *    }
 *    synth_window {
 *      window_child_with_name
 *    }
 *    // other windows...
 *  }
 */
std::optional<Window> X11findWindow(Display *display) {
  // this is the return value
  std::optional<Window> synth_window;

  // setup for querying windows
  Window root_window = DefaultRootWindow(display);

  // fields for XQueryTree
  Window root_return{};
  Window parent_return{};
  // List of children window to be returned
  Window *children_return{};
  // Number of children windows
  unsigned int nchildren_return{};

  int result = XQueryTree(
      display,          // Display
      root_window,      // window to query
      &root_return,     // Root window return
      &parent_return,   // Return paramameter for pointer to parent window
      &children_return, // List of children to be returned
      &nchildren_return // Number of children elements in (children_return )
  );

  if (result == 0) {
    std::cout << "Failed to query root windows " << result << std::endl;
    return synth_window;
  }

  for (auto n = 0; n < nchildren_return; n++) {
    // query all root's children windows
    // fields for XQueryTree
    Window child_root_return{};
    Window child_parent_return{};
    // List of children window to be returned
    Window *child_children_return{};
    // Number of children windows
    unsigned int child_nchildren_return{};

    int result = XQueryTree(
        display,              // Display
        children_return[n],   // window to query
        &child_root_return,   // Root window return
        &child_parent_return, // Return paramameter for pointer to parent window
        &child_children_return, // List of children to be returned
        &child_nchildren_return // Number of children elements in
                                // (child_children_return )
    );

    if (result == 0) {
      std::cout << "Failed to query 1st level windows\n";
      return synth_window;
    }

    for (auto child_idx = 0; child_idx < child_nchildren_return; child_idx++) {
      if (x11_get_window_name(display, child_children_return[child_idx])
              .compare("rusty synth") == 0) {
        synth_window = std::make_optional(children_return[n]);
      }
    }

    XFree(child_children_return);
  }

  // Dispose, release memory allocated for children windows return
  XFree(children_return);

  return synth_window;
}
