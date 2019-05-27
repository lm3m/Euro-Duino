class SmoothReadings {
private:
    unsigned int maxReadings = 0;
    unsigned long readingCount = 0;
    unsigned long totalReading = 0;
    int *readings;

public:
    SmoothReadings(unsigned int maxReadings);
    int count();
    int addValue(int newReading);
};
