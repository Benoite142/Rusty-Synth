#include "note_map.hpp"
#include <cstdlib>

NoteMap makeEmptyNoteMap() {
  bool *b = static_cast<bool *>(std::malloc(sizeof(bool)));
  short *c = static_cast<short *>(std::malloc(sizeof(short)));
  *b = false;
  *c = -1;
  return NoteMap{.has_updated_value = b, .notes = c};
}

void freeNoteMap(NoteMap *nm) {
  free(nm->has_updated_value);
  free(nm->notes);
}
