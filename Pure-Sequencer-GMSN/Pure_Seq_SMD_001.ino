#include "fscale.h"
#include "mux.h"

#define RANDOM 0
#define FORWARD 1
#define PENDULUM 2

#define PLAY_BUTTON 5

int internal_clock_rate_RAW;
int internal_clock_rate;
int trigLength = 50;
int trigLengthVar;
byte Direction = 0;
boolean forwardBackward = true;
boolean clockHigh = false;
boolean resetFlag = true;
boolean buttonFlag = true;
boolean resetSteps = false;
byte STEP = 0;

boolean PLAY = false;

boolean clkReadFlag = true;
int sweepCompare;

unsigned long cMillis;
unsigned long pMillis = 0;
unsigned long clkMillis = 0;

void setup(){
  //gates 0-5 outputs port c
  DDRC = B111111;
  DDRB = B111111;
  
  //Clock in
  pinMode(2, INPUT);
  pinMode(4, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);

  pinMode(5, OUTPUT);
  pinMode(10, OUTPUT);
  
  /*
  DDRD = B11111110;
  pinMode(8, OUTPUT);
  //digitalWrite(8, HIGH); 
  pinMode(9, OUTPUT);
  //digitalWrite(9, HIGH); 
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH); 
  pinMode(18, INPUT);
  pinMode(19, INPUT);
  pinMode(16, INPUT);
  pinMode(17, INPUT);
  */
  pMillis = millis();

  trigLengthVar = trigLength;
  digitalWrite(PLAY_BUTTON, HIGH);
  //PORTD |= (1 << 5);
}


void loop(){
  directionToggle();
  if(digitalRead(4) == HIGH && buttonFlag){
    digitalWrite(PLAY_BUTTON, HIGH);
    PLAY = false;
    buttonFlag = false;
    sweepCompare = (1023 - analogRead(A7));
  }
  if(digitalRead(4) == LOW && !buttonFlag){
    digitalWrite(PLAY_BUTTON, LOW);
    buttonFlag = true;
  }

  while(!PLAY){
    if(digitalRead(4) == LOW && !buttonFlag){
      buttonFlag = true;
      delay(20);
    }
    if(digitalRead(4) == HIGH && buttonFlag){
      buttonFlag = false;
      PLAY = true;
    }
    
    if(abs((1023 - analogRead(A7)) - sweepCompare) >= 20){
      sweepCompare = 1024;
    }
    if(sweepCompare == 1024){
      STEP = (1023 - analogRead(A7))/129;
      muxGate(STEP);
      muxPot(STEP);
    }
  }

  if(digitalRead(3) == LOW && resetFlag){
    switch(Direction) {
      case PENDULUM:
        forwardBackward = !forwardBackward;
        break;
      case FORWARD:
        resetSteps = true;
        break;
      case RANDOM:
      // dropthrough
      default:
        STEP = random(8);
        break;
    }

    pMillis = cMillis - internal_clock_rate;
    resetFlag = false;
  }
  if(digitalRead(3) == HIGH && !resetFlag){
    resetFlag = true;
  }

  cMillis = millis();

  if(clockHigh){
    if(cMillis - clkMillis >= trigLengthVar){
      clockHigh = false;
      //PORTB &= ~1;
      PORTB &= ~(1 << 2);
      //PORTB |= (1 << 2);
    }
  }

  internal_clock_rate_RAW = (1023 - analogRead(A7));
  internal_clock_rate = fscale( 0, 1023, 1500, 7, internal_clock_rate_RAW, 10);

  if(internal_clock_rate < (trigLength * 2)){
    trigLengthVar = internal_clock_rate/2;
  }
  else{
    trigLengthVar = trigLength;
  }


  if(cMillis - pMillis >= internal_clock_rate){
    pMillis = cMillis;

    clockHigh = true;
    clkMillis = cMillis;
    //PORTB |= 1;
    PORTB |= (1 << 2);
    //PORTB &= ~(1 << 2);

    muxGate(STEP);
    muxPot(STEP);
  }

  if(digitalRead(2) == LOW && clkReadFlag){
    switch(Direction)
    {
      case FORWARD:
        if(resetSteps) {
          STEP = 0;
          resetSteps = false;
        }
        else {
          STEP++;
          STEP = STEP % 8; 
        }
        break;
      case RANDOM:
        STEP = random(8);
        break;
      case PENDULUM:
      // fallthrough
      default:
        if(forwardBackward) {
          STEP++;
          if(STEP > 7){
            forwardBackward = false;
            STEP = 6;
          }
        }
        else {
          STEP--;
          if(STEP > 8){
            forwardBackward = true;
            STEP = 1;
          }
        } 
        break;
    }

    muxGate(STEP);
    muxPot(STEP);
    clkReadFlag = false;
  }
  if(digitalRead(2) == HIGH && !clkReadFlag){
    clkReadFlag = true;
  }
}


void directionToggle(){
  if(digitalRead(7) == HIGH){
    Direction = FORWARD;
  }
  else if(digitalRead(6) == HIGH){
    Direction = PENDULUM;
  }
  else{
    Direction = RANDOM;
  }
}
