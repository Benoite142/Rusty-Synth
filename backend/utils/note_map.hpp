#ifndef NOTE_MAP
#define NOTE_MAP

struct NoteMap {
  bool *has_updated_value;
  short *notes;
};

NoteMap makeEmptyNoteMap();
void freeNoteMap(NoteMap *nm);

#endif
