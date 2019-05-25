#include "Progression.h"

void Progression::SetRoot(Note root) {
    this->root = root;
    return;
}

void Progression::SetScale(Scale scale) {
    this->scale = scale;
    return;
}

unsigned long Progression::GetCurrentNote() {
    unsigned int curNote = this->ScaleProgression[this->scale][this->currentPos % 7];
    return this->noteTable[(Note)(curNote % Note::Highest_Note)];
}

unsigned long Progression::GetNextNote(Order order, int distance) {
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
    case Order::Reset:
    default:
        currentPos = 0;
        break;
    }
    return this->GetCurrentNote();
}

void Progression::GenerateNoteTable() {
    float baseInterval = (this->maxValue / 5) * this->octave;
    float interval = (this->maxValue / 5) / 12;
    for(unsigned int i = 0; i < Note::Highest_Note; ++i) {
        this->noteTable[i] = (unsigned long)(baseInterval + (i * interval)); 
    }
}