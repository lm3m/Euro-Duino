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
    Melodic
};


class Progression {
private:
    unsigned int ScaleProgression [4][7] = {
        {0, 2, 4, 5, 7, 9, 11},
        {0, 2, 3, 5, 7, 8, 10},
        {0, 2, 3, 5, 7, 8, 11},
        {0, 2, 3, 5, 7, 9, 11}
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
    unsigned long GetNextNote(Order order, int distance = 0);
};
