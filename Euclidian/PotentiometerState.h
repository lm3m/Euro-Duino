class PotentiometerState {
private:
    int m_curState ;
public:
    PotentiometerState(int initialState = 0){
        m_curState = initialState;
    }

    bool updateState(int newState) {
        if(newState != this->m_curState){
            this->m_curState = newState;
            return true;
        }
        else {
            return false;
        }
    }
    int curState() {
        return m_curState;
    }
};