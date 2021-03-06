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

#pragma once 
enum Note {
    C = 0,
    C_Sharp = 1,
    D_Flat = 1,
    D = 2,
    D_Sharp = 3,
    E_Flat = 3,
    E = 4,
    F = 5,
    F_Sharp = 6,
    G_Flat = 6,
    G = 7,
    G_Sharp = 8,
    A_Flat = 8,
    A = 9,
    A_Sharp = 10,
    B_Flat = 10,
    B = 11,
    Highest_Note = 11
};


enum Order {
    Forward,
    Backward,
    Skip,
    Reset
};

enum Scale {
    Major = 0,
    Minor,
    Harmonic,
    Melodic,
    I_IV_V,
    I_V_VI_IV,
    I_IV_I_V_I,
    III_VI_II_V
};

struct Scale_Progression {
    unsigned int size;
    unsigned int progression[12];
};



const unsigned int NUM_SCALES = 8;
class Progression {
private:
    Scale_Progression ScaleProgression [NUM_SCALES] = {
        {7, {0, 2, 4, 5, 7, 9, 11}},
        {7, {0, 2, 3, 5, 7, 8, 10}},
        {7, {0, 2, 3, 5, 7, 8, 11}},
        {7, {0, 2, 3, 5, 7, 9, 11}},
        {3, {0, 5, 7}},
        {4, {0, 7, 9, 5}},
        {5, {0, 5, 0, 6, 0}},
        {4, {4, 9, 2, 7}}
    };

    unsigned int octave; 
    unsigned long maxValue;
    Note root;
    Scale scale;
    unsigned int currentPos;
    unsigned long noteTable[12];

    void GenerateNoteTable();

public: 
    Progression(unsigned int octave, unsigned long maxValue) {
        this->octave = octave; // should be 0 - 4 (1v per octave and 5v max)
        this->maxValue = maxValue; // max output value to use
        this->root = Note::C;
        this->scale = Scale::Major;
        currentPos = 0;
        GenerateNoteTable();
        
    }

    void SetRoot(Note root);
    void SetScale(Scale scale);
    unsigned long GetCurrentNote();
    Note GetRoot();
    Scale GetScale();
    unsigned long GetNextNote(Order order, int distance = 0);
};
