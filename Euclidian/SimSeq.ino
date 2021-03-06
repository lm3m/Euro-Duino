/* ============================================================ */
//
// Program: Dual Sequencer Using Euclidean Logic for triggering steps
//
// Description: A step sequencer that bases on/off gates on a series of
//   Euclidean sequences
//
//
// I/O Usage:
// A0: Steps for rhythm A
// A1: Steps for rhythm B
// A2: Pulses for rhythm A
// A3: Pulses for rhythm B
// Digital Out 1: Rhythm A output
// Digital Out 2: Rhythm B output
// Clock In: External clock input
// Analog Out: Echoes the clock input
//
// Input Expander: unused
// Output Expander: 8 bits of output exposed
//
// Created: 25 Sept 2012
// Modified:
//
// ============================================================
//
// License:
//
// This software is licensed under the Creative Commons
// "Attribution-NonCommercial license. This license allows you
// to tweak and build upon the code for non-commercial purposes,
// without the requirement to license derivative works on the
// same terms. If you wish to use this (or derived) work for
// commercial work, please contact 20 Objects LLC at our website
// (www.20objects.com).
//
// For more information on the Creative Commons CC BY-NC license,
// visit http://creativecommons.org/licenses/
//
// ================= start of global section ==================


#include "Progression.h"
#include "SwitchState.h"
#include "SmoothReadings.h"
#include "PotentiometerState.h"
#include "Euclidian.h"

#define _DEBUG 0

// These constants won't change.  They're used to give names
// to the pins used:
const int analogIn1Pin = A0;  // Analog Input 1
const int analogIn2Pin = A1; // Analog Input 2
const int analogPot1Pin = A2;  // Analog Input 1
const int analogPot2Pin = A3; // Analog Input 2
const int analogOut1Pin = 5;  // Analog Output 1
const int analogOut2Pin = 6; // Analog Output 2
const int DigitalIn1Pin = 8;  // Digital Input 1
const int DigitalIn2Pin = 9;  // Digital Input 2
const int DigitalOut1Pin = 3;  // Digital Output 1
const int DigitalOut2Pin = 4;  // Digital Output 2
const int Switch1Up = A4;  // Switch 1 Up
const int Switch1Dwn = A5;  // Switch 1 Dwn
const int Switch2Up = 7;  // Switch 2 Up
const int Switch2Dwn = 2;  // Switch 2 Dwn

const int STATE_A = 0;
const int STATE_B = 1;
// the euclidian rhythm settings
int numSteps = 0;
int numPulsesA = 0;
int numPulsesB = 0;
int inRotate = 0;
byte lastClock = LOW;
const int maxSteps = 32;
int arrayA[maxSteps];
int arrayB[maxSteps];

int in1Pin = 0;
int in1Pot = 0;
int in2Pin = 0;
int in2Pot = 0;
volatile int isr_count = 0;
unsigned long currPulse = 0;

SwitchState switch1State;
SwitchState switch2State;

PotentiometerState* pot1State;
PotentiometerState* pot2State;

const unsigned int maxReadings = 10;
SmoothReadings* readings1;
SmoothReadings* readings2;

Progression* progression1;
Progression* progression2;

void GetSwitchStates() {
  switch1State = !digitalRead(Switch1Up) == 1 ? SwitchState::Up : SwitchState::Middle;
  switch1State = !digitalRead(Switch1Dwn) == 1 ? SwitchState::Down : switch1State;
  switch2State = !digitalRead(Switch2Up) == 1 ? SwitchState::Up : SwitchState::Middle;
  switch2State = !digitalRead(Switch2Dwn) == 1 ? SwitchState::Down : switch2State;
}

void GetAnalogs(void) {
  in1Pin = analogRead(analogIn1Pin);
  in1Pot = (readings1->addValue(analogRead(analogPot1Pin)) >> 5) + 1;
  in2Pin = analogRead(analogIn2Pin);
  in2Pot = (readings2->addValue(analogRead(analogPot2Pin) >> 5) + 1);
  // numSteps = (in1Pot >> 5) + 1;
  //numSteps += (in1Pin >> 6);
  // numPulses = ( in2Pot >> 5) + 1;
  //numPulses += ( in2Pin >> 6);
  GetSwitchStates();
}


// ==================== start of setup() ======================
void setup()
{
  pinMode(DigitalIn1Pin, INPUT);
  pinMode(DigitalIn2Pin, INPUT);
  pinMode(DigitalOut1Pin, OUTPUT); 
  pinMode(DigitalOut2Pin, OUTPUT); 
  pinMode(Switch1Up, INPUT_PULLUP);
  pinMode(Switch1Dwn, INPUT_PULLUP);
  pinMode(Switch2Up, INPUT_PULLUP);
  pinMode(Switch2Dwn, INPUT_PULLUP);
    
  pinMode(analogIn1Pin, INPUT);
  pinMode(analogPot1Pin, INPUT_PULLUP);
  pinMode(analogIn2Pin, INPUT);
  pinMode(analogPot2Pin, INPUT);

  readings1 = new SmoothReadings(maxReadings);
  readings2 = new SmoothReadings(maxReadings);
  pot1State = new PotentiometerState();
  pot2State = new PotentiometerState();
  progression1 = new Progression(1, 255);
  progression2 = new Progression(1, 255);
  progression1->SetRoot(Note::A_Flat);
  progression2->SetRoot(Note::F);
  progression1->SetScale(Scale::I_IV_I_V_I);
  progression2->SetScale(Scale::III_VI_II_V);

  // Note: Interrupt 0 is for pin 2 (DigitalIn1Pin)
  //attachInterrupt(digitalPinToInterrupt(DigitalIn2Pin), isrHandler, CHANGE);
  //attachInterrupt(0, isr, RISING);
  Serial.begin(9600);
  Serial.println("start...");
  // get the analog reading to set up the system
  GetAnalogs();
  inRotate = 0;
  numSteps = maxSteps;
  numPulsesA = maxSteps/2;
  numPulsesB = maxSteps/2;
  euCalc(arrayA, numPulsesA, numSteps, maxSteps);
  euCalc(arrayB, numPulsesB, numSteps, maxSteps);
  dumpEuclid();
}

void isrHandler() {
  isr_count++;  
}

Note mapPotToNote(int potState) {
  return (Note) (12 * (potState / maxSteps));
}

Scale mapPotToScale(int potState) {
  return (Scale) (NUM_SCALES * (potState/ maxSteps));
}

void updateState(int state) {
  Note newRoot;
  Scale newScale;
  switch (switch2State)
  {
  case SwitchState::Up:
    newRoot = mapPotToNote(pot2State->curState());
    if(state == STATE_A) {
      progression1->SetRoot(newRoot);
    }
    else {
      progression2->SetRoot(newRoot);
    }
    break;

  case SwitchState::Middle:
    newScale = mapPotToScale(pot2State->curState());
    if(state == STATE_A) {
      progression1->SetScale(newScale);
    }
    else {
      progression2->SetScale(newScale);
    }
    break;

  case SwitchState::Down:
    if(state == STATE_A) {
      numPulsesA = pot2State->curState();      
    }
    else {
      numPulsesB = pot2State->curState();      
    }
    break;

  default:
    break;
  }
}

void updateGlobal() {
  switch (switch2State)
  {
  case SwitchState::Up:
    // update randomness of A
    break;
  
  case SwitchState::Middle:
    // update randomness of B
    break;
 
  case SwitchState::Down:
    numSteps = pot2State->curState();
    break;
 
  default:
    break;
  }
}

void updatePotState(int potIndex) {
  if(potIndex != 2) {
    return;
  }

  switch (switch1State)
  {
  case SwitchState::Up:
    updateState(STATE_A);
    break;
  
  case SwitchState::Middle:
    updateState(STATE_B);
    break;
 
  case SwitchState::Down:
    updateGlobal();
    break;
 
  default:
    break;
  }

}

void loop()
{
  int oldInSteps = numSteps;
  int oldInPulsesA = numPulsesA;
  int oldInPulsesB = numPulsesB;
  // check to see if the clock as been set
  byte thisClock = digitalRead(DigitalIn1Pin);
  bool doClock = false;
  if (thisClock == HIGH && lastClock == LOW) {
    currPulse++;
    doClock = true;;
  }
  lastClock = thisClock;
 
  if (doClock) {
    dumpInput(oldInPulsesA, oldInPulsesB, oldInSteps);    

    inRotate++;
    int outPulseA = arrayA[(currPulse) % numSteps];
    int outPulseB = arrayB[(currPulse) % numSteps];

    dumpState(outPulseA, outPulseB);

    if (outPulseA > 0) {      
      unsigned long note;
      if(currPulse % numSteps == 0) {
        note = progression1->GetNextNote(Order::Reset);
      }
      else {
        note = progression1->GetNextNote(Order::Forward);
      }

      if(switch1State != SwitchState::Down){
        analogWrite(analogOut1Pin, ~note);
        digitalWrite(DigitalOut1Pin, HIGH);
        Serial.println("A: ON");
      }
    }
    else {
      digitalWrite(DigitalOut1Pin, LOW);
      Serial.println("A: OFF");
    }

    if (outPulseB > 0) {      
      unsigned long note2;
      if(currPulse % numSteps == 0) {
        note2 = progression2->GetNextNote(Order::Reset);     
      }
      else {
        note2 = progression2->GetNextNote(Order::Forward);
      }

      if(switch1State != SwitchState::Down){
        analogWrite(analogOut2Pin, ~note2);
        digitalWrite(DigitalOut2Pin, HIGH);
        Serial.println("B: ON");
      }
    }
    else {
      digitalWrite(DigitalOut2Pin, LOW);
      Serial.println("B: OFF");
    }
  }


  // read the inputs in case we need to change
  GetAnalogs();
  if(switch1State == SwitchState::Down) {
      digitalWrite(DigitalOut1Pin, LOW);
      digitalWrite(DigitalOut2Pin, LOW);
  }
  if(pot1State->updateState(in1Pot)) {
    updatePotState(1);
  }

  if(pot2State->updateState(in2Pot)) {
    updatePotState(2);
  }

  if (oldInSteps != numSteps) {
    euCalc(arrayA, numPulsesA);
    euCalc(arrayB, numPulsesB);
  }

  if(oldInPulsesA != numPulsesA){
    euCalc(arrayA, numPulsesA);
  }

  if(oldInPulsesB != numPulsesB){
    euCalc(arrayB, numPulsesB);
  }

}


void dumpEuclid() {
  Serial.print("arrayA: ");
  for(int i = 0; i < maxSteps; i++){
    Serial.print(arrayA[i]);
    Serial.print(".");
  }
  Serial.println(" ");
  Serial.print("arrayB: ");
  for(int i = 0; i < maxSteps; i++){
    Serial.print(arrayB[i]);
    Serial.print(".");
  }
  Serial.println(" ");

}

void dumpState(int outPulseA, int outPulseB) {
  #if _DEBUG
  Serial.print("outPluseA: ");
  Serial.print(outPulseA);
  Serial.print(" outPluseB: ");
  Serial.print(outPulseB);
  Serial.print(" numPulsesA: ");
  Serial.print(numPulsesA);
  Serial.print(" numPulsesB: ");
  Serial.print(numPulsesB);
  Serial.print(" numSteps: ");
  Serial.print(numSteps);
  Serial.print(" currPulse: ");
  Serial.print(currPulse);
  Serial.print(" inRotate: ");
  Serial.println(inRotate);
  Serial.print(" root note 1: ");
  Serial.print(progression1->GetRoot());
  Serial.print(" scale 1: ");
  Serial.print(progression1->GetScale());
  Serial.print(" note 1: ");
  Serial.println(progression1->GetCurrentNote());
  Serial.print(" root note 2: ");
  Serial.print(progression2->GetRoot());
  Serial.print(" scale 2: ");
  Serial.print(progression2->GetScale());
  Serial.print(" note2: ");
  Serial.println(progression2->GetCurrentNote());
  #endif
  #if 0
  Serial.print(" pin 1: ");
  Serial.print(in1Pin);
  Serial.print(" pot 1: ");
  Serial.print(in1Pot);
  Serial.print(" pin 2: ");
  Serial.print(in2Pin);
  Serial.print(" pot 2: ");
  Serial.println(in2Pot);
  #endif
}

void dumpInput(int oldInPulsesA, int oldInPulsesB, int oldInSteps) {
  #if _DEBUG
  Serial.print("Pulses old A: ");
  Serial.print(oldInPulsesA);
  Serial.print(" new: ");
  Serial.print(numPulsesA);
  Serial.print(" Pulses old B: ");
  Serial.print(oldInPulsesB);
  Serial.print(" new: ");
  Serial.print(numPulsesB);
  Serial.print(" Steps old: ");
  Serial.print(oldInSteps);
  Serial.print(" new: ");
  Serial.print(numSteps);
  Serial.print(" pin 1: ");
  Serial.print(in1Pin);
  Serial.print(" pot 1: ");
  Serial.print(in1Pot);
  Serial.print(" pin 2: ");
  Serial.print(in2Pin);
  Serial.print(" pot 2: ");
  Serial.print(in2Pot);
  Serial.print(" Switch1Dwn: ");
  Serial.print(!digitalRead(Switch1Dwn));
  Serial.print(" Switch1Up: ");
  Serial.print(!digitalRead(Switch1Up));
  Serial.print(" Switch2Dwn: ");
  Serial.print(!digitalRead(Switch2Dwn));
  Serial.print(" Switch2Up: ");
  Serial.print(!digitalRead(Switch2Up));
  Serial.print(" switch1State: ");
  Serial.print(switch1State);
  Serial.print(" switch2State: ");
  Serial.println(switch2State);
  #endif
}
// ===================== end of program =======================rmd
