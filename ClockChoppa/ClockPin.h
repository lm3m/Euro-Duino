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
