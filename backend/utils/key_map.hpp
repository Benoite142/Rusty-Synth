#ifndef KEY_MAP
#define KEY_MAP

struct KeyMap {
  bool *has_updated_value;
  char *keys;
};

KeyMap makeEmptyKeyMap();
void freeKeyMap(KeyMap *km);

#endif
