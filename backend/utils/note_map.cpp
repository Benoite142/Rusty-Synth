#include "note_map.hpp"
#include <cstdlib>
#include <vector>

NoteMap makeEmptyNoteMap() {
  bool *b = static_cast<bool *>(std::malloc(sizeof(bool)));
  *b = false;
  return NoteMap{
      .has_updated_value = b,
      .notes = std::vector<Note>(2, Note{.note_value = -1, .released = true})};
}

bool operator==(Note note_a, Note note_b) {
  return note_a.note_value == note_b.note_value;
}

void freeNoteMap(NoteMap *nm) { free(nm->has_updated_value); }
