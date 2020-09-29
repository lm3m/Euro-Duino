/*
Copyright (c) 2019 Luke W. McCullough <lukaswm@gmail.com>

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

class ClockPin
{
protected:
    int m_pin;
    unsigned int m_divider;
    int m_triggerState;
    unsigned int m_offset;

    int step();
public:
    ClockPin(int pin, unsigned int divider=32, unsigned int offset=0);
    int tick(unsigned int tickCount);
    void divider(unsigned int divider, unsigned int offset=0);
};

ClockPin::ClockPin(int pin, unsigned int divider, unsigned int offset) :
    m_pin(pin),
    m_divider(divider),
    m_offset(offset)
{
    m_triggerState = LOW;
}

int ClockPin::tick(unsigned int tickCount) {
    int state = -1;
    // only do something on the divider or if we are on/high
    if(tickCount % m_divider == m_offset || m_triggerState) {
        state = step();
    }

    return state;
}

int ClockPin::step() {
    // toggle the state
    m_triggerState = !m_triggerState;
    digitalWrite(m_pin, m_triggerState);
    return m_triggerState;
}

void ClockPin::divider(unsigned int divider, unsigned int offset=0) {
    m_divider = divider;
    m_offset = offset;
}
