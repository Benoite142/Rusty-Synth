#ifndef NOTE_MAP
#define NOTE_MAP

#include <vector>

struct Note {
  short note_value;
  bool released;
};

struct NoteMap {
  bool *has_updated_value;
  std::vector<Note> notes;
};

bool operator==(Note, Note);

NoteMap makeEmptyNoteMap();
void freeNoteMap(NoteMap *nm);

#endif
