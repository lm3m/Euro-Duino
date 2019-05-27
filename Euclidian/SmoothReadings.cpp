 #include "SmoothReadings.h"
 
 SmoothReadings::SmoothReadings(unsigned int maxReadings) {
    this->readingCount = 0;
    this->totalReading = 0;
    this->maxReadings = maxReadings;
    this->readings = new int[maxReadings];
    for(int i = 0; i < maxReadings; i++) {
      this->readings[i] = 0;
    }
  }

  int SmoothReadings::count() {
    return maxReadings < this->readingCount ? maxReadings : this->readingCount;
  }

  int SmoothReadings::addValue(int newReading){
    this->totalReading = this->totalReading - this->readings[this->readingCount % maxReadings];
    this->totalReading += newReading;
    this->readings[this->readingCount % maxReadings] = newReading;
    this->readingCount++;
    return totalReading / this->count();
  }
