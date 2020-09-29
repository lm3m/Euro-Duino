//
// Program ed3_rls
// 
//  Description: Random Looping Sequencer, with quantisation option and variable loop length
//
//  I/O Usage:
//
//    pot1: chance of change for rand looping seq 
//    pot2: Scale choice, full ccw is unquantised, or choice of available scales 
//    sw1: length of loop UP=4 bytes Middle=2 bytes down = 1 byte
//    sw2: quantisation range UP=full range Middle=1/2 range Down=1 octave
//    dig1in: clock in
//    dig2in:
//    cv1in: seem noisy, not used
//    cv2in:
//    Dig1Out: trigger bit change 
//    Dig2Out: trigger on no bit change
//    CV1Out: quantised low byte of loop
//    CV2Out: unquantised low byte of loop
//
// created Jan 31, 2015
// Gregor McNish
// http://github.com/eclectics
//
// ============================================================
//
//  License:
//
//  This software is licensed under the Creative Commons
//  "Attribution-NonCommercial license. This license allows you
//  to tweak and build upon the code for non-commercial purposes,
//  without the requirement to license derivative works on the
//  same terms. If you wish to use this (or derived) work for
//  commercial work, please contact  the author.
//
//  For more information on the Creative Commons CC BY-NC license,
//  visit http://creativecommons.org/licenses/
//
//  ================= start of global section ==================
#include "ClockPin.h"
#define _INTERRUPT 1
// I/O pins used on the Euro-Duino module.
const int pot1=A2;
const int pot2=A3;
const int switches[]={A4,A5,7,2}; //1 up/down, 2 up/down
const int dig1in=8;
const int dig2in=9;
const int cv1in=A0;
const int cv2in=A1;
const int dig1out=3;
const int dig2out=4;
const int cv1out=5;
const int cv2out=6;
const byte UP=255;
const byte DOWN=128;
byte controls[]={pot1,pot2,cv1in,cv2in};
byte digout[]={dig1out,dig2out};
byte sw[2]={0,0}; //switch state
int ctrlvals[4];

// Variables uses by the sketch

volatile unsigned long clockState;
volatile unsigned long lastClockState;
volatile unsigned long nextClockState;
int pot2Val;
bool sendState;

ClockPin *digitalOut1;
ClockPin *digitalOut2;
ClockPin *cvOut1;
ClockPin *cvOut2;

#ifdef _INTERRUPT
ISR (PCINT0_vect){ //isr for pins 8to13
//check only pin 8 RISING
//doesn't matter if triggered by other things
    ++clockState;
}
#endif

void setup() {
    // Set up I/O pins
    pinMode(dig1in, INPUT);
    pinMode(dig2in, INPUT);
    pinMode(dig1out, OUTPUT); 
    pinMode(dig2out, OUTPUT);
    pinMode(cv1out, OUTPUT);
    pinMode(cv2out, OUTPUT);
    for (int i=0;i<4;i++){
        pinMode(switches[i], INPUT_PULLUP);
    }
    //pinMode(pot1, INPUT);
    //pinMode(pot2, INPUT);

    digitalOut1 = new ClockPin(dig1out, 2);
    digitalOut2 = new ClockPin(dig2out, 4);
    cvOut1 = new ClockPin(cv1out, 16);
    cvOut2 = new ClockPin(cv2out, 16);
#ifdef _INTERRUPT
  // set up clock interrupt
    PCMSK0 |= bit (PCINT0);  // want pin 0
    PCIFR  |= bit (PCIF0);   // clear any outstanding interrupts
    PCICR  |= bit (PCIE0);   // enable pin change interrupts for D8 to D13
#endif
    Serial.begin(9600);
    Serial.println("start...");
    clockState = nextClockState = lastClockState = 0;
    sendState = false;
}


void stopAllPins() {
    digitalWrite(dig1out, LOW);
    digitalWrite(dig2out, LOW);
    digitalWrite(cv1out, LOW);
    digitalWrite(cv2out, LOW);
}

void startAllPins() {
    digitalWrite(dig1out, HIGH);
    digitalWrite(dig2out, HIGH);
    digitalWrite(cv1out, HIGH);
    digitalWrite(cv2out, HIGH);
}

void loop(){
#ifndef _INTERRUPT
    if(digitalRead(dig1in)) {
        ++clockState;
    }
#endif

    if(clockState != nextClockState) {
        nextClockState = clockState;
        if(nextClockState != lastClockState + 1){
            Serial.print("Drop: ");
            Serial.print(lastClockState);
            Serial.print(" to ");
            Serial.print(nextClockState);
            Serial.print(".\n");
        }
        int pin1state = digitalOut1->tick(nextClockState);
        //Serial.print("Pin 1 step: ");
        //Serial.println(pin1state);
        int pin2state = digitalOut2->tick(nextClockState);
        if(pin2state != -1) {
            Serial.print("Pin 2 step: ");
            Serial.print(pin2state);
            Serial.print(" at tick: ");
            Serial.println(nextClockState);
        }
        cvOut1->tick(nextClockState);
        cvOut2->tick(nextClockState);

        int testPot1 = analogRead(pot1);
        int testPot2 = analogRead(pot2)>>5;

        if(testPot2 != pot2Val) {
            pot2Val = testPot2;
            Serial.print("pot 1: ");
            Serial.print(testPot1);
            Serial.print(" pot 2: ");
            Serial.print(testPot2);
            
            Serial.println(".");
            int divider = pot2Val;
            divider = divider<<2;
            int offset = divider/2.0;
            if(divider <= 1) {
                offset = 0;
                divider = 1;
            }
            Serial.print("divider: ");
            Serial.println(divider);
            
            Serial.print("offset: "); 
            Serial.println(offset);
            
            digitalOut1->divider(divider);
            digitalOut2->divider(divider, offset);
        }

        
        lastClockState = nextClockState;
    }
}


//  ===================== end of program =======================
