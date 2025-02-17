#ifndef X_UTILS
#define X_UTILS

#include <X11/Xlib.h>

/**
 * @brief maps a keycode event's keycode to it's corresponding char
 *
 * @param event: the key event of which we want to extract the character
 * representation
 *
 * @return: a pointer to the character
 *
 */
char *mapKeyCodeToString(XKeyEvent event);

#endif
