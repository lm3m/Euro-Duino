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

/*
 * This is a clock divider for MIDI using the 1010 Music Eurosheild (https://1010music.com/shielddocs)
 * Inputs:
 *   the audio/cv inputs are not used
 *   MIDI input the clock tick message is used
 *   upper and lower poteniometer control the clock division used for each output
 *   button triggers both outputs, turns on the lights, and resets the tick count
 * Outputs:
 *   both outputs supply CV triggers based on the divider (set using the pots)
 *   supported divisions are: 32nd, 16th, 8th, quarter, half, whole, 2x whole, 3x whole, 4x whole notes
 *   these divisions are based on the MIDI clock at 24 ticks per quarter note and will follow as BMP goes up and down
 */
#include <Audio.h>
#include <MIDI.h>
#include <ResponsiveAnalogRead.h>

#define LED_PIN_COUNT 4
#define BUTTON_PIN 2
#define UPPER_POT_INPUT 20
#define LOWER_POT_INPUT 21
#define DEBOUNCE_MS 100
#define DIVIDER_COUNT 9
#define NON_BLINDING_HIGH 64

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

/*
 * MIDI ticks are sent out 24 per quarter note.
 *     "Clock events are sent at a rate of 24 pulses per quarter note." from: https://en.wikipedia.org/wiki/MIDI_beat_clock
 * For time slices here are the definitions:
 * 1.      32nd notes (ticks/3) 
 * 2. sixteenth notes (ticks/6)
 * 3.    eighth notes (ticks/12)
 * 4.   quarter notes (ticks/24)
 * 5.      half notes (ticks/48)
 * 6.     whole notes (ticks/96)
 * 7.   2 whole notes (ticks/192)
 * 8.   3 whole notes (ticks/288)
 * 9.   4 whole notes (ticks/384)
 */
int clockDivider[DIVIDER_COUNT] = {3, 6, 12, 24, 48, 96, 192, 288, 384};
// clock divider state
int clockDivider1 = 0;
int clockDivider2 = 0;

// smooth readers for the analog pots
ResponsiveAnalogRead upperPotAnalog(UPPER_POT_INPUT, false);
ResponsiveAnalogRead lowerPotAnalog(LOWER_POT_INPUT, false);

// leds
int ledPins[LED_PIN_COUNT] = {3, 4, 5, 6};

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

// button state stuff
long lastButtonMS = 0;
int lastButtonState = -1;

// output trigger and tick state stuff
bool trig1 = true;
bool trig2 = true;
long tick = 0;
long triggered1 = 0;
long triggered2 = 0;

void handleClock(void)
{
    // MIDI clock ticks come in at 24 beats per quarter note
    // divider only triggers when the tick count is a multiple of the divider
    ++tick;
    if ((tick % clockDivider1) == 0)
    {
        Serial.println("clock: " + String(tick) + " clockDivider: " + String(clockDivider1));
        trig1 = true;
        ++triggered1;
    }
    else
    {
        trig1 = false;
    }
    if ((tick % clockDivider2) == 0)
    {
        Serial.println("clock: " + String(tick) + " clockDivider: " + String(clockDivider2));
        trig2 = true;
        ++triggered2;
    }
    else
    {
        trig2 = false;
    }
}

void setup()
{
    // allocate some memory for processing
    AudioMemory(12);

    // init the audio controller
    sgtl5000_1.enable();
    sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
    sgtl5000_1.volume(0.82);
    sgtl5000_1.adcHighPassFilterDisable();
    sgtl5000_1.lineInLevel(0, 0);
    sgtl5000_1.unmuteHeadphone();

    // set button mode
    pinMode(BUTTON_PIN, INPUT);

    // set led modes
    for (int i = 0; i < LED_PIN_COUNT; i++)
        pinMode(ledPins[i], OUTPUT);

    // start MIDI
    MIDI.begin(); // Launch MIDI and listen
    MIDI.setHandleClock(handleClock);
}

bool updateButton()
{
    // Check for button presses here...
    bool retVal = false;
    int buttonState = digitalRead(BUTTON_PIN);

    if (buttonState != lastButtonState)
    {
        long ms = millis();

        if (ms - lastButtonMS > DEBOUNCE_MS)
        {
            if (buttonState == LOW)
            {
                retVal = true;
            }
            lastButtonMS = ms;
            lastButtonState = buttonState;
        }
    }
    return retVal;
}

void updateAnalogs()
{
    upperPotAnalog.update();
    lowerPotAnalog.update();
}

void loop()
{
    // update inputs
    bool buttonPressed = updateButton();
    updateAnalogs();

    if (buttonPressed)
    {
        Serial.println("button pressed");
        // trigger everything
        trig1 = trig2 = true;
        triggered1 = triggered2 = -1;
        // reset ticks
        tick = 0;
    }

    // update the outputs
    /*
     * note on the AudioSynthWaveformDc: I really don't like this, but it seems to be the best way to output CV from the audio shield
     * should wrap this in a class that allows for access by either note/octave and/or voltage
     */
    dc1.amplitude(trig1 ? 1.0 : 0.0);
    dc2.amplitude(trig2 ? 1.0 : 0.0);

    // update the led states
    analogWrite(ledPins[2], (triggered1 & 1) > 0 ? NON_BLINDING_HIGH : LOW);
    analogWrite(ledPins[3], (triggered1 & 2) > 0 ? NON_BLINDING_HIGH : LOW);
    analogWrite(ledPins[0], (triggered2 & 1) > 0 ? NON_BLINDING_HIGH : LOW);
    analogWrite(ledPins[1], (triggered2 & 2) > 0 ? NON_BLINDING_HIGH : LOW);

    // update the divider state
    clockDivider1 = clockDivider[map(upperPotAnalog.getValue(), 0, 1023, 0, DIVIDER_COUNT - 1)];
    clockDivider2 = clockDivider[map(lowerPotAnalog.getValue(), 0, 1023, 0, DIVIDER_COUNT - 1)];

    // read from MIDI which calls the handlers
    MIDI.read();
}