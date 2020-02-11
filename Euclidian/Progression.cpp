#include "Progression.h"

void Progression::SetRoot(Note root) {
    this->root = root;
    return;
}

void Progression::SetScale(Scale scale) {
    this->scale = scale;
    return;
}

Note Progression::GetRoot() {
  return this->root;
}

Scale Progression::GetScale(){
  return this->scale;
}

unsigned long Progression::GetCurrentNote() {
    Scale_Progression curProg = this->ScaleProgression[this->scale];
    unsigned int curNote = curProg.progression[this->currentPos % curProg.size];
    return this->noteTable[(Note)(curNote % Note::Highest_Note)];
}

unsigned long Progression::GetNextNote(Order order, int distance) {
    unsigned long note = this->GetCurrentNote();
    //update state
    switch (order)
    {
    case Order::Forward:
        currentPos++; 
        break;
    case Order::Backward:
        currentPos--;
        break;
    case Order::Skip:
        currentPos += distance;
        break;
    case Order::Reset:
    default:
        currentPos = 0;
        break;
    }
    return note;
}

void Progression::GenerateNoteTable() {
    float baseInterval = (this->maxValue / 5) * this->octave;
    float interval = (this->maxValue / 5) / 12;
    for(unsigned int i = 0; i < Note::Highest_Note; ++i) {
        this->noteTable[i] = (unsigned long)(baseInterval + (i * interval)); 
    }
}
