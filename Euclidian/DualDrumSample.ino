
#include "Audio.h"
#include <Wire.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <elapsedMillis.h>

#define SERIAL_OUT

elapsedMillis timeElapsed;
elapsedMillis timeElapsed_Serial;
elapsedMillis drumUpperTriggered;
elapsedMillis drumLowerTriggered;

int displayRefreshInterval = 25;
int drumTrigger;

// GUItool: begin automatically generated code
AudioSynthSimpleDrum     drumLower;      //xy=113,1089
AudioSynthSimpleDrum     drumUpper;      //xy=132,923
AudioInputI2S            audioInput;     //xy=194,1361
AudioAnalyzePeak         peakCVUpper;    //xy=508,1405
AudioAnalyzePeak         peakCVLower;    //xy=514,1466
AudioMixer4              outputMixer; 
AudioOutputI2S           audioOutput;    //xy=1214,1156

AudioConnection          patchCord3(audioInput, 0, peakCVUpper, 0);
AudioConnection          patchCord4(audioInput, 1, peakCVLower, 0);

AudioConnection          patchCord1(drumLower, 0, filterLower, 0);
AudioConnection          patchCord2(drumUpper, 0, filterUpper, 0);
AudioConnection          patchCord17(drumUpper, 0, outputMixer, 0);
AudioConnection          patchCord18(drumLower, 0, outputMixer, 1);
AudioConnection          patchCord19(outputMixer, 0, audioOutput, 0);

AudioControlSGTL5000     audioShield;    //xy=452,1366
                     // GUItool: end automatically generated code

// Arrays for Upper / Lower Drum Voices
AudioAnalyzePeak*         peak[2];
AudioSynthSimpleDrum* drums[2] = { &drumUpper,&drumLower };
// GUItool: end automatically generated code

//---------------CV INPUT--------------

double peakCV[2] = { 0.0,0.0 };
bool lastState[2] = { false, false };
bool shouldTrigger[2] = {false, false};

//-----DRUMS-----------
#define DRUM_COUNT 2
#define DRUM_UPPER 0
#define DRUM_LOWER 1
#define MIN_DRUM_FREQ 60
#define MAX_DRUM_FREQ 5000
#define MIN_DRUM_LENGTH 10
#define MAX_DRUM_LENGTH 2000

int drumFreq[2] = { 100,500 };
int drumFreq_new[2];

int drumLength[2] = { 750,450 };
int drumLength_new[2];

double drumPitchMod[2] = { 0.5,0.5 };
double drumPitchMod_new[2];

double drumSecondMix[2] = { 1.0,1.0 };
double drumSecondMix_new[2];

double mixerGain[3] = {1,0, 0};

void setup()
{

  Serial.begin(9600);
  Serial.println("Here we go.");
#ifdef SERIAL_OUT
  Serial.println("Here we go.");
#endif

  //-----AUDIO-----
  AudioMemory(14);

  audioShield.enable();
  audioShield.inputSelect(AUDIO_INPUT_LINEIN);
  audioShield.volume(1.0);
  audioShield.adcHighPassFilterDisable();
  audioShield.lineInLevel(0, 0);

  peak[DRUM_UPPER] = &peakCVUpper;
  peak[DRUM_LOWER] = &peakCVLower;

  //initial Configuration
  for (int i = 0; i < DRUM_COUNT; i++) {
    drums[i]->frequency(drumFreq[i]);
    drums[i]->length(drumLength[i]);
    drums[i]->secondMix(drumSecondMix[i]);
    drums[i]->pitchMod(drumPitchMod[i]);
  }

  digitalWrite(ledPins[3], LOW);
  digitalWrite(ledPins[2], LOW);
  digitalWrite(ledPins[1], LOW);
  digitalWrite(ledPins[0], LOW);
}

void loop()
{    
    if(shouldTrigger[i] && timeElapsed > 42) {
      drums[i]->noteOn();
      shouldTrigger[i] = false;
    }

    //read the CV Inputs / Trigger
    if (peak[i]->available()) {
      peakCV[i] = peak[i]->read();
    }

    //play the drums when the peak value is > 0.5 and the former drum hit has ended
    if (peakCV[i] > 0.9) {
      if(!lastState[i]) {
          Serial.print("CV: ");
          Serial.print(peakCV[i]);
          Serial.print(" Trigger: ");
          Serial.println(i);
          drums[i]->noteOff();
          shouldTrigger[i] = true;
          lastState[i] = true;
          drumUpperTriggered = 0;
          timeElapsed = 0;
      }
    }
    else {
        lastState[i] = false;
    }
  }
 
#ifdef SERIAL_OUT
    if (timeElapsed_Serial > 1000 && updated) {
      for (int i = 0; i < DRUM_COUNT; i++) {
        Serial.print("DRUM");
        Serial.print(i);
        Serial.print("---");
  
        Serial.print("Freq:");
        Serial.print(drumFreq[i]);
        Serial.print("---");
  
        Serial.print("Length:");
        Serial.print(drumLength[i]);
        Serial.print("---");
  
        Serial.print("2nd:");
        Serial.print(drumSecondMix[i]);
        Serial.print("---");
  
        Serial.print("PitchMod:");
        Serial.print(drumPitchMod[i]);
        Serial.print("---");
  
        Serial.print("FilterMode:");
        Serial.print(filterModeText[filterModeIndex[i]]);
        Serial.print("---");
  
        Serial.print("FilterFreq:");
        Serial.print(filterFreq[i]);
        Serial.print("---");
  
        Serial.print("FilterReso:");
        Serial.print(filterReso[i]);
        Serial.print("---");
  
        Serial.print("Reverb:");
        Serial.print(reverbLvl[i] * 100);
        Serial.print("%");
        Serial.print("---");
  
        Serial.print("Roomsize:");
        Serial.print(roomsize[i] * 100);
        Serial.print("%");
        Serial.print("---");
  
        Serial.print("Dampening:");
        Serial.print(dampening[i] * 100);
        Serial.print("%");
        Serial.print("---");
        Serial.print("PotValue:");
        Serial.println(potValue[i]);
  
        if (i == DRUM_LOWER)
  
        {
          timeElapsed_Serial = 0;
          Serial.println("---");
        }
      }
    }

#endif
}
