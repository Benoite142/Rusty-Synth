#include "key_map.hpp"
#include <cstdlib>

KeyMap makeEmptyKeyMap() {
  bool *b = static_cast<bool *>(std::malloc(sizeof(bool)));
  char *c = static_cast<char *>(std::malloc(sizeof(char)));
  *b = false;
  *c = '/';
  return KeyMap{.has_updated_value = b, .keys = c};
}

void freeKeyMap(KeyMap *km) {
  free(km->has_updated_value);
  free(km->keys);
}
