/*
Copyright (c) 2020 Luke W. McCullough <lukaswm@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
