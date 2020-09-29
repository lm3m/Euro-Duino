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
