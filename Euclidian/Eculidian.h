/* ============================================================ */
//
// Program: ArdCore Dual Euclidian Trigger Sequencer
//
// Description: An implementation of a dual trigger sequencer
// using Euclidian Rhythm concepts
//
// Thanks to Robin Price (http://crx091081gb.net/)
// for the use of his algorithm for Euclidean
// Rhythm Generation.
//
// I/O Usage:
// A0: Steps for rhythm A
// A1: Steps for rhythm B
// A2: Pulses for rhythm A
// A3: Pulses for rhythm B
// Digital Out 1: Rhythm A output
// Digital Out 2: Rhythm B output
// Clock In: External clock input
// Analog Out: Echoes the clock input
//
// Input Expander: unused
// Output Expander: 8 bits of output exposed
//
// Created: 25 Sept 2012
// Modified:
//
// ============================================================
//
// License:
//
// This software is licensed under the Creative Commons
// "Attribution-NonCommercial license. This license allows you
// to tweak and build upon the code for non-commercial purposes,
// without the requirement to license derivative works on the
// same terms. If you wish to use this (or derived) work for
// commercial work, please contact 20 Objects LLC at our website
// (www.20objects.com).
//
// For more information on the Creative Commons CC BY-NC license,
// visit http://creativecommons.org/licenses/
//
// ================= start of global section ==================

// euCalc(int) - create a Euclidean Rhythm array.
//
// NOTE: Thanks to Robin Price for his excellent implementation, and for
// making the source code available on the Interwebs.
// For more info, check out: http://crx091081gb.net/
// ----------------------------------------------------------------------
void euCalc(int* arrEuclid, int numPulses, int numSteps, int maxSteps) {
  int loc = 0;
  
  // clear the array to start
  for (int i = 0; i < maxSteps; i++) {
    arrEuclid[i] = 0;
  }
  
  if ((numPulses >= numSteps) || (numSteps == 1)) {
    if (numPulses >= numSteps ) {
      for (int i = 0; i < numSteps && loc < maxSteps; i++) {
        arrEuclid[loc] = 1;
        loc++;
      }
    }
  } else {
    int offs = numSteps - numPulses;
    if (offs >= numPulses) {
      int ppc = offs / numPulses;
      int rmd = offs % numPulses;
      
      for (int i = 0; i < numPulses && loc < maxSteps; i++) {
        arrEuclid[loc] = 1;
        loc++;
        for (int j = 0; j < ppc && loc < maxSteps; j++) {
          arrEuclid[loc] = 0;
          loc++;
        }
        if (i < rmd && loc < maxSteps) {
          arrEuclid[loc] = 0;
          loc++;
        }
      }
    } else {
      int ppu = (numPulses - offs) / offs;
      int rmd = (numPulses - offs) % offs;
        
      for (int i = 0; i < offs && loc < maxSteps; i++) {
        arrEuclid[loc] = 1;
        loc++;
        if(loc < maxSteps) {
          arrEuclid[loc] = 0;
          loc++;
        }
        for (int j = 0; j < ppu && loc < maxSteps; j++) {
          arrEuclid[loc] = 1;
          loc++;
        }
        if (i < rmd && loc < maxSteps) {
          arrEuclid[loc] = 1;
          loc++;
        }
      }
    }
  }
}
