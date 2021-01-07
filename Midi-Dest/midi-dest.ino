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

#include <Audio.h>
#include <MIDI.h>
#include <ResponsiveAnalogRead.h>

#define LED_PIN_COUNT 4
#define BUTTON_PIN 2
#define UPPER_POT_INPUT 20
#define LOWER_POT_INPUT 21
#define DEBOUNCE_MS 100
#define DIVIDER_COUNT 15
#define NON_BLINDING_HIGH 64

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

// smooth readers for the analog pots
ResponsiveAnalogRead upperPotAnalog(UPPER_POT_INPUT, false);
ResponsiveAnalogRead lowerPotAnalog(LOWER_POT_INPUT, false);

// leds
int ledPins[LED_PIN_COUNT] = {3, 4, 5, 6};
long tick = 0;
// audio device stuff
AudioInputI2S audioInput;
AudioAnalyzeRMS input_1;
AudioAnalyzeRMS input_2;
AudioSynthWaveformDc dc2;
AudioSynthWaveformDc dc1;
AudioOutputI2S audioOutput;
AudioConnection patchCord1(audioInput, 0, input_1, 0);
AudioConnection patchCord2(audioInput, 1, input_2, 0);
AudioConnection patchCord3(dc1, 0, audioOutput, 0);
AudioConnection patchCord4(dc2, 0, audioOutput, 1);
AudioControlSGTL5000 sgtl5000_1;

int upperValue = 0;
int lowerValue = 0;
static double MAX_VOLTAGE = 3.16;
void setup()
{
    // start the serial connection
    Serial.begin(115200);

    // wait for serial monitor to open
    while(! Serial);
    Serial.println(F("START " __FILE__ " from " __DATE__));
    // allocate some memory for processing
    AudioMemory(12);

    // init the audio controller
    sgtl5000_1.enable();
    sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
    sgtl5000_1.volume(0.82);
    sgtl5000_1.adcHighPassFilterDisable();
    sgtl5000_1.lineOutLevel(13, 13);
    sgtl5000_1.lineInLevel(0, 0);
    sgtl5000_1.unmuteHeadphone();

    // set button mode
    pinMode(BUTTON_PIN, INPUT);

    // set led modes
    for (int i = 0; i < LED_PIN_COUNT; i++) {
        pinMode(ledPins[i], OUTPUT);
        analogWrite(ledPins[i], 0);
    }

    // start MIDI
    MIDI.begin(); // Launch MIDI and listen
    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);
    MIDI.setHandleClock(handleClock);
}

void handleClock(void)
{
    tick++;
    if(tick % 128 == 0) {
        Serial.println("clock: " + String(tick));
    }
}

void updateAnalogs()
{
    upperPotAnalog.update();
    lowerPotAnalog.update();

    int newUpper = map(upperPotAnalog.getValue(), 0, 1023, 0, 15);
    int newLower = map(lowerPotAnalog.getValue(), 0, 1023, 0, 15);
    if(newUpper != upperValue) {
        Serial.print("newUpper: ");
        Serial.println(newUpper);
        Serial.print("newUpper & 1: ");
        Serial.println(newUpper & 1);
        // update the led states
        analogWrite(ledPins[0], newUpper & 1 ? NON_BLINDING_HIGH : 0);
        analogWrite(ledPins[1], newUpper & 2 ? NON_BLINDING_HIGH : 0);    
        analogWrite(ledPins[2], newUpper & 4 ? NON_BLINDING_HIGH : 0);
        analogWrite(ledPins[3], newUpper & 8 ? NON_BLINDING_HIGH : 0);
        upperValue = newUpper;
    }

    if(newLower != lowerValue) {
        Serial.print("newLower: ");
        Serial.println(newLower);
        Serial.print("newLower & 1: ");
        Serial.println(newLower & 1);
        // update the led states
        analogWrite(ledPins[0], newLower & 1 ? NON_BLINDING_HIGH : 0);
        analogWrite(ledPins[1], newLower & 2 ? NON_BLINDING_HIGH : 0);    
        analogWrite(ledPins[2], newLower & 4 ? NON_BLINDING_HIGH : 0);
        analogWrite(ledPins[3], newLower & 8 ? NON_BLINDING_HIGH : 0);
        lowerValue = newLower;
    }
}

void loop()
{
    updateAnalogs();

    // read from MIDI which calls the handlers
    MIDI.read();
}


void handleNoteOn(byte channel, byte pitch, byte velocity)
{
    if(channel != upperValue)
    {
        Serial.print("handleNoteOn, wrong channel: ");
        Serial.println(channel);
        return;
    }

    // 1.0 = 3.13 v
    // 0.0 = 0 v
    // -1.0. = -3.13 v
    Serial.print("handleNoteOn, ");
    
    double offset_pitch = pitch - 60;
    Serial.print("offset: ");
    Serial.print(offset_pitch, 0);

    offset_pitch = offset_pitch / 12.0;
    Serial.print(" voltage: ");
    Serial.print(offset_pitch, 4);

    while(offset_pitch > MAX_VOLTAGE) {
        offset_pitch -= 1.0;
    }
    while (offset_pitch < -MAX_VOLTAGE) {
        offset_pitch += 1.0;
    }

    offset_pitch = offset_pitch / 5;


    Serial.print(" normalized: ");
    Serial.println(offset_pitch, 4);

    dc1.amplitude(1.0);
    dc2.amplitude(offset_pitch);
    
}

void handleNoteOff(byte channel, byte note, byte velocity)
{
    if(channel != upperValue)
    {
        Serial.print("handleNoteOff, wrong channel: ");
        Serial.println(channel);
        return;
    }

    Serial.println("handleNoteOff, off.");
    dc1.amplitude(0.0);

}
