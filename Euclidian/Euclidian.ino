/* ============================================================ */
//
// Program: ArdCore Dual Euclidian Trigger Sequencer
//
// Description: An implementation of a dual trigger sequencer
// using Euclidian Rhythm concepts
//
// Thanks to Robin Price (http://crx091081gb.net/)
// for the use of his algorithm for Euclidean
// Rhythm Generation.
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

// constants related to the Arduino Nano pin use
const int clkIn = DigitalIn1Pin; // the digital (clock) input
const int digPin = DigitalOut1Pin; // the digital output pins
//const int pinOffset = 5; // the first DAC pin (from 5-12)
const int trigTime = 25; // 25 ms trigger timing

// variables for interrupt handling of the clock input
volatile int clkState = LOW;

// variables used to control the current DIO output states
int digState = LOW; // start with both set low
unsigned long digMilli = 0; // a place to store millis()

// the euclidian rhythm settings
int inSteps=0;
int inPulses=0;
int inRotate=0;
byte lastClock=LOW;
byte thisClock=LOW;
byte doClock = LOW;
int offCount = 0;
const int maxSteps = 32;
int arrEuclid[maxSteps];

int in1Pin = 0;
int in1Pot = 0;
int in2Pin = 0;
int in2Pot = 0;

unsigned long currPulse = 0;
unsigned long offPulse = 0;
unsigned int note = 51;
unsigned int ocatvePos = 0;
unsigned int note2 = 100;
unsigned int ocatvePos2 = 11;
int doCalc = 0;
int offCountSteps = 6;

const unsigned int maxReadings = 10;
struct SmoothReadings {
  unsigned int readingCount = 0;
  unsigned int totalReading = 0;
  int readings[maxReadings];

  void init() {
    this->readingCount = 0;
    this->totalReading = 0;
    for(int i = 0; i < maxReadings; i++) {
      this->readings[i] = 0;
    }
  }

  int count() {
    return maxReadings < this->readingCount ? maxReadings : this->readingCount;
  }

  int addValue(int newReading){
    this->totalReading = this->totalReading - this->readings[this->readingCount % maxReadings];
    this->totalReading += newReading;
    this->readings[this->readingCount % maxReadings] = newReading;
    this->readingCount++;
    return totalReading / this->count();
  }
};

SmoothReadings readings1;
SmoothReadings readings2;

void GetAnalogs(void)
{
 // in1Pin = analogRead(analogIn1Pin);
 in1Pot = readings1.addValue(analogRead(analogPot1Pin));
 // in2Pin = analogRead(analogIn2Pin);
 in2Pot = readings2.addValue(analogRead(analogPot2Pin));
 inSteps = (in1Pot >> 5) + 1;
 //inSteps += (in1Pin >> 6);
 offCountSteps = inSteps;
 inPulses = ( in2Pot >> 5) + 1;
 //inPulses += ( in2Pin >> 6);
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
  pinMode(analogPot2Pin, INPUT_PULLUP);

  readings1.init();
  readings2.init();
  // Note: Interrupt 0 is for pin 2 (clkIn)
  //attachInterrupt(0, isr, RISING);
  Serial.begin(9600);
  Serial.println("start...");
  Serial.println(A2);
  // get the analog reading to set up the system
  GetAnalogs();
  inRotate = 0;
  
  euCalc(0);
}

void loop()
{
  // check to see if the clock as been set
  thisClock = digitalRead(clkIn);
  if (thisClock == HIGH && lastClock == LOW) {
    currPulse++;
    doClock = HIGH;
  }

  lastClock = thisClock;
 
  if (doClock == HIGH) {
    doClock = LOW;
    inRotate++;
    int outPulse = 0;
    int myPulse = (currPulse) % maxSteps;
    outPulse = arrEuclid[myPulse];

    dumpState(outPulse);

    if (outPulse > 0) {      
      digState = HIGH;
      digMilli = millis();
      if( digitalRead(Switch1Dwn) ){
        analogWrite(analogOut1Pin, ~note);
        digitalWrite(DigitalOut1Pin, HIGH);
      }
      if( digitalRead(Switch2Dwn) ){
        analogWrite(analogOut2Pin, ~note2);
        digitalWrite(DigitalOut2Pin, HIGH);
      }

      ocatvePos = nextOcatve(ocatvePos);
      ocatvePos2 = nextOcatve(ocatvePos2);
      note = 51 + (4.5 * (ocatvePos % 12));
      note2 = 51 + (4.5 * (ocatvePos2 % 12));
    }
    else {
      digState = LOW;
      digitalWrite(DigitalOut1Pin, LOW);
      digitalWrite(DigitalOut2Pin, LOW);
      offCount++;
      offPulse++;
    }
  }
  
  // read the inputs in case we need to change
  doCalc = 0;
  int inStepsOld = inSteps;
  int inPulsesOld = inPulses;
  GetAnalogs();

  if (inStepsOld != inSteps || inPulsesOld != inPulses) {
    doCalc = 1;
  }
  
  if (doCalc) {
    dumpInput(inPulsesOld, inStepsOld);    
    euCalc(0);
  }
}

int nextOcatve(int ocatvePos) {
  if(inRotate%2) {
    ocatvePos += 2;
  }
  else {
    ocatvePos -= 2;
  }
  return ocatvePos;
}

// euCalc(int) - create a Euclidean Rhythm array.
//
// NOTE: Thanks to Robin Price for his excellent implementation, and for
// making the source code available on the Interwebs.
// For more info, check out: http://crx091081gb.net/
// ----------------------------------------------------------------------
void euCalc(int ar) {
  int loc = 0;
  
  // clear the array to start
  for (int i=0; i<maxSteps; i++) {
    arrEuclid[i] = 0;
  }
  
  if ((inPulses >= inSteps) || (inSteps == 1)) {
    if (inPulses >= inSteps) {
      for (int i = 0; i < inSteps && loc < maxSteps; i++) {
        arrEuclid[loc] = 1;
        loc++;
      }
    }
  } else {
    int offs = inSteps - inPulses;
    if (offs >= inPulses) {
      int ppc = offs / inPulses;
      int rmd = offs % inPulses;
      
      for (int i = 0; i < inPulses && loc < maxSteps; i++) {
        arrEuclid[loc] = 1;
        loc++;
        for (int j = 0; j < ppc && loc < maxSteps; j++) {
          arrEuclid[loc] = 0;
          loc++;
        }
        if (i < rmd && loc < maxSteps) {
          arrEuclid[loc] = 0;
          loc++;
        }
      }
    } else {
      int ppu = (inPulses - offs) / offs;
      int rmd = (inPulses - offs) % offs;
        
      for (int i = 0; i < offs && loc < maxSteps; i++) {
        arrEuclid[loc] = 1;
        loc++;
        if(loc < maxSteps) {
          arrEuclid[loc] = 0;
          loc++;
        }
        for (int j = 0; j < ppu && loc < maxSteps; j++) {
          arrEuclid[loc] = 1;
          loc++;
        }
        if (i < rmd && loc < maxSteps) {
          arrEuclid[loc] = 1;
          loc++;
        }
      }
    }
  }
  dumpEuclid();
}

void dumpEuclid() {
  Serial.print("arrEuclid: ");
  for(int i = 0; i < maxSteps; i++){
    Serial.print(arrEuclid[i]);
    Serial.print(".");
  }
  Serial.println(" ");
}

void dumpState(int outPulse) {
  Serial.print("outPluse: ");
  Serial.print(outPulse);
  Serial.print(" inPulses: ");
  Serial.print(inPulses);
  Serial.print(" inSteps: ");
  Serial.print(inSteps);
  Serial.print(" currPulse: ");
  Serial.print(currPulse);
  Serial.print(" inRotate: ");
  Serial.print(inRotate);
  Serial.print(" ocatvePos: ");
  Serial.print(ocatvePos);
  Serial.print(" note: ");
  Serial.println(note);
  Serial.print(" ocatvePos2: ");
  Serial.print(ocatvePos2);
  Serial.print(" note2: ");
  Serial.println(note2);
  Serial.print("pin 1: ");
  Serial.print(in1Pin);
  Serial.print(" pot 1: ");
  Serial.print(in1Pot);
  Serial.print(" pin 2: ");
  Serial.print(in2Pin);
  Serial.print(" pot 2: ");
  Serial.println(in2Pot);
}

void dumpInput(int inPulsesOld, int inStepsOld) {
  Serial.print("Pulses old: ");
  Serial.print(inPulsesOld);
  Serial.print(" new: ");
  Serial.print(inPulses);
  Serial.print("Steps old: ");
  Serial.print(inStepsOld);
  Serial.print(" new: ");
  Serial.print(inSteps);
  Serial.print(" pin 1: ");
  Serial.print(in1Pin);
  Serial.print(" pot 1: ");
  Serial.print(in1Pot);
  Serial.print(" pin 2: ");
  Serial.print(in2Pin);
  Serial.print(" pot 2: ");
  Serial.println(in2Pot);
}
// ===================== end of program =======================rmd
