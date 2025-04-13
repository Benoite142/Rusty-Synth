#include "note_map.hpp"
#include <algorithm>
#include <cstdlib>
#include <vector>

NoteMap makeEmptyNoteMap() {
  bool *b = static_cast<bool *>(std::malloc(sizeof(bool)));
  *b = false;
  return NoteMap{
      .has_updated_value = b,
      .notes = std::vector<Note>(2, Note{.note_value = -1, .released = true}),
      .current_voices = 0};
}

bool operator==(Note note_a, Note note_b) {
  return note_a.note_value == note_b.note_value;
}

void freeNoteMap(NoteMap *nm) { free(nm->has_updated_value); }

void updateNoteMapSize(NoteMap *nm, std::size_t new_size) {
  if (new_size < 1 || new_size > 10) {
    return;
  }
  *nm->has_updated_value = true;
  nm->current_voices = 0;

  if (new_size > nm->notes.size()) {
    nm->notes.resize(new_size);
    std::fill(nm->notes.begin(), nm->notes.end(),
              Note{.note_value = -1, .released = true});
  } else {
    nm->notes.resize(new_size);
  }
}
