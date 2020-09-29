
#include "Audio.h"
#include <Wire.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <elapsedMillis.h>
#include <MIDI.h>

#define SERIAL_OUT

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

#define ledPinCount 4
int ledPins[ledPinCount] = { 3, 4, 5, 6 };
int ledPos = 0;

elapsedMillis timeElapsed[2];
elapsedMillis timeElapsed_Serial[2];
elapsedMillis drumUpperTriggered;
elapsedMillis drumLowerTriggered;

int displayRefreshInterval = 25;
int drumTrigger;
long count = 0;

#define DRUMPOOL_SIZE 20
#define DRUMMIXER_SIZE 5
#define OUTPUTMIXER_SIZE 2
// GUItool: begin automatically generated code
AudioSynthSimpleDrum    drumPool[DRUMPOOL_SIZE];
AudioMixer4             drumMixers[DRUMMIXER_SIZE];
AudioMixer4             outputMixers[OUTPUTMIXER_SIZE];
AudioConnection*        drumConnections[DRUMPOOL_SIZE+DRUMMIXER_SIZE+OUTPUTMIXER_SIZE];

AudioSynthSimpleDrum     drumLower01;      //xy=113,1089
AudioSynthSimpleDrum     drumLower02;      //xy=113,1089
AudioSynthSimpleDrum     drumLower03;      //xy=113,1089
AudioSynthSimpleDrum     drumLower04;      //xy=113,1089
AudioSynthSimpleDrum     drumLower05;      //xy=113,1089
AudioSynthSimpleDrum     drumLower06;      //xy=113,1089
AudioSynthSimpleDrum     drumLower07;      //xy=113,1089
AudioSynthSimpleDrum     drumLower08;      //xy=113,1089
AudioSynthSimpleDrum     drumLower09;      //xy=113,1089
AudioSynthSimpleDrum     drumLower10;      //xy=113,1089
AudioSynthSimpleDrum     drumUpper01;      //xy=132,923
AudioSynthSimpleDrum     drumUpper02;      //xy=132,923
AudioSynthSimpleDrum     drumUpper03;      //xy=132,923
AudioSynthSimpleDrum     drumUpper04;      //xy=132,923
AudioSynthSimpleDrum     drumUpper05;      //xy=132,923
AudioSynthSimpleDrum     drumUpper06;      //xy=132,923
AudioSynthSimpleDrum     drumUpper07;      //xy=132,923
AudioSynthSimpleDrum     drumUpper08;      //xy=132,923
AudioSynthSimpleDrum     drumUpper09;      //xy=132,923
AudioSynthSimpleDrum     drumUpper10;      //xy=132,923
AudioMixer4              outputMixer01; 
AudioMixer4              outputMixer02; 
AudioMixer4              outputMixer03; 
AudioMixer4              outputMixer04; 
AudioMixer4              outputMixer05;
AudioMixer4              outputMixerMixer; 
AudioMixer4              outputMixerOut; 
AudioOutputI2S           audioOutput;    //xy=1214,1156
AudioConnection          patchCord01(drumUpper01, 0, outputMixer01, 0);
AudioConnection          patchCord02(drumLower01, 0, outputMixer01, 1);
AudioConnection          patchCord03(drumUpper02, 0, outputMixer01, 2);
AudioConnection          patchCord04(drumLower02, 0, outputMixer01, 3);

AudioConnection          patchCord11(drumUpper03, 0, outputMixer02, 0);
AudioConnection          patchCord12(drumLower03, 0, outputMixer02, 1);
AudioConnection          patchCord13(drumUpper04, 0, outputMixer02, 2);
AudioConnection          patchCord14(drumLower04, 0, outputMixer02, 3);

AudioConnection          patchCord21(drumUpper05, 0, outputMixer03, 0);
AudioConnection          patchCord22(drumLower05, 0, outputMixer03, 1);
AudioConnection          patchCord23(drumUpper06, 0, outputMixer03, 2);
AudioConnection          patchCord24(drumLower06, 0, outputMixer03, 3);

AudioConnection          patchCord31(drumUpper07, 0, outputMixer04, 0);
AudioConnection          patchCord32(drumLower07, 0, outputMixer04, 1);
AudioConnection          patchCord33(drumUpper08, 0, outputMixer04, 2);
AudioConnection          patchCord34(drumLower08, 0, outputMixer04, 3);

AudioConnection          patchCord41(drumUpper09, 0, outputMixer05, 0);
AudioConnection          patchCord42(drumLower09, 0, outputMixer05, 1);
AudioConnection          patchCord43(drumUpper10, 0, outputMixer05, 2);
AudioConnection          patchCord44(drumLower10, 0, outputMixer05, 3);

AudioConnection          patchCord51(outputMixer01, 0, outputMixerMixer, 0);
AudioConnection          patchCord52(outputMixer02, 0, outputMixerMixer, 1);
AudioConnection          patchCord53(outputMixer03, 0, outputMixerMixer, 2);
AudioConnection          patchCord54(outputMixer04, 0, outputMixerMixer, 3);

AudioConnection          patchCord61(outputMixer05, 0, outputMixerOut, 0);
AudioConnection          patchCord62(outputMixerMixer, 0, outputMixerOut, 1);

AudioConnection          patchCord99(outputMixerOut, 0, audioOutput, 0);

AudioControlSGTL5000     audioShield;    //xy=452,1366
                     // GUItool: end automatically generated code

// Arrays for Upper / Lower Drum Voices
AudioSynthSimpleDrum* upperDrums[10] = { &drumUpper01,&drumUpper02,&drumUpper03,&drumUpper04,&drumUpper05,&drumUpper06,&drumUpper07,&drumUpper08,&drumUpper09,&drumUpper10 };
AudioSynthSimpleDrum* lowerDrums[10] = { &drumLower01,&drumLower02,&drumLower03,&drumLower04,&drumLower05,&drumLower06,&drumLower07,&drumLower08,&drumLower09,&drumLower10 };
int upperCount = 0;
int lowerCount = 0;
// GUItool: end automatically generated code

//---------------CV INPUT--------------


//-----DRUMS-----------
#define DRUM_COUNT 2
#define DRUMS_COUNT 10
#define DRUM_UPPER 0
#define DRUM_LOWER 1
#define MIN_DRUM_FREQ 60
#define MAX_DRUM_FREQ 5000
#define MIN_DRUM_LENGTH 10
#define MAX_DRUM_LENGTH 2000

int drumFreq[2] = { 100,500 };
int drumFreq_new[2];

int drumLength[2] = { 2000,450 };
int drumLength_new[2];

double drumPitchMod[2] = { 0.5,0.5 };
double drumPitchMod_new[2];

double drumSecondMix[2] = { 1.0,1.0 };
double drumSecondMix_new[2];

double mixerGain[3] = {1,0, 0};

void doSomeStuffWithNoteOn(byte channel, byte pitch, byte velocity)
{
}

void doSomeStuffWithNoteOff(byte channel, byte pitch, byte velocity)
{
//  Serial.print("channel: ");
//  Serial.println(channel);
//  Serial.print("pitch: ");
//  Serial.println(pitch);

  if(pitch == DRUM_UPPER + 1) {
    for(int i = 0; i < DRUMS_COUNT; ++i) {
      if(!upperDrums[i]->done()) {
        continue;
      }
      upperDrums[i]->noteOn();
      Serial.print("upperDrums: ");
      Serial.println(i);
      return;
    }
    Serial.println("upperDrums drop");
  }
  if(pitch == DRUM_LOWER + 1) {
    for(int i = 0; i < DRUMS_COUNT; ++i) {
      if(!lowerDrums[i]->done()) {
        continue;
      }
      lowerDrums[i]->noteOn();
      Serial.print("lowerDrums: ");
      Serial.println(i);
      return;
    }
    Serial.println("lowerDrums drop");
  }
  if(pitch == 3) {
      Serial.print("cpu=");
      Serial.print(AudioProcessorUsage());
      Serial.print(",");
      Serial.print(AudioProcessorUsageMax());
      Serial.print("    ");
      Serial.print("Memory: ");
      Serial.print(AudioMemoryUsage());
      Serial.print(",");
      Serial.print(AudioMemoryUsageMax());
      Serial.println();
  }

}


void setup()
{

  Serial.begin(9600);
  Serial.println("Here we go.");
#ifdef SERIAL_OUT
  Serial.println("Here we go.");
#endif
  MIDI.begin(6);                      // Launch MIDI and listen
  MIDI.setHandleNoteOff(doSomeStuffWithNoteOff);
  MIDI.setHandleNoteOn(doSomeStuffWithNoteOn);  
  //-----AUDIO-----
  AudioMemory(32);

  audioShield.enable();
  audioShield.inputSelect(AUDIO_INPUT_LINEIN);
  audioShield.volume(1.0);
  audioShield.adcHighPassFilterDisable();
  audioShield.lineInLevel(0, 0);

  //initial Configuration
    unsigned int curMixer = 0;
    unsigned int curMixerPin = 0;
    unsigned int curConnection = 0;
    unsigned int i = 0;
    for(; i < DRUMPOOL_SIZE; ++i) {
        if(curMixerPin == 4) {
            ++curMixer;
            curMixerPin = 0;
        }
        drumConnections[i] = new AudioConnection(drumPool[i], 0, drumMixers[curMixer], curMixerPin);
        ++curMixerPin;
    }

    unsigned int curOutputPin = 0;
    unsigned int curOutputMixer = 0;
    unsigned int j = 0;
    for(; j < DRUMMIXER_SIZE; ++j) {
        if(curOutputPin == 4) {
            ++curOutputMixer;
            curOutputPin = 0;
        }
        drumConnections[i+j] = new AudioConnection(drumMixer[j], 0, outputMixers[curOutputMixer], curOutputPin);
        ++curOutputPin;
    }

    unsigned int k = 0;
    curOutputPin = 0;
    for(; k < OUTPUTMIXER_SIZE; ++k) {
        drumConnections[i+j+k] = new AudioConnection(outputMixers[k], 0, outputMixerOut, curOutputPin);
        ++curOutputPin;
    }

  for (int i = 0; i < DRUMS_COUNT; i++) {
    upperDrums[i]->frequency(drumFreq[DRUM_UPPER]);
    upperDrums[i]->length(drumLength[DRUM_UPPER]);
    upperDrums[i]->secondMix(drumSecondMix[DRUM_UPPER]);
    upperDrums[i]->pitchMod(drumPitchMod[DRUM_UPPER]);
    lowerDrums[i]->frequency(drumFreq[DRUM_LOWER]);
    lowerDrums[i]->length(drumLength[DRUM_LOWER]);
    lowerDrums[i]->secondMix(drumSecondMix[DRUM_LOWER]);
    lowerDrums[i]->pitchMod(drumPitchMod[DRUM_LOWER]);
  }  
  for (int i = 0; i < ledPinCount; i++)
    pinMode(ledPins[i], OUTPUT);
}

void advanceLED()
{
  digitalWrite(ledPins[ledPos % ledPinCount], LOW);
  ledPos++;  
  digitalWrite(ledPins[ledPos % ledPinCount], HIGH);
}

void loop()
{     
  if (MIDI.read())                    // If we have received a message
  {
      advanceLED();
  }
}