#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>
#include "MuxHelper.h"

class Button {
private:


public:
  // Constructors

  // Getters

  // Setters

  // Methods
};


uint8_t XY_array[23][2] = {
  { 1, 4 },  // M1
  { 1, 3 },  // M2
  { 1, 2 },  // DEFAULT A
  { 1, 1 },  // DEFAULT B
  { 2, 4 },  // TRANSPOSE +
  { 2, 3 },  // TRANSPOSE -
  { 2, 1 },  // FADE IN/OUT
  { 3, 4 },  // TOUCH
  { 3, 3 },  // SUSTAIN
  { 3, 2 },  // VIBRATION
  { 3, 1 },  // DRUM
  { 4, 1 },  // INTRO/END
  { 5, 4 },  // SYNC
  { 5, 3 },  // START/STOP
  { 5, 1 },  // FILL-IN
  { 6, 4 },  // CHORD OFF
  { 6, 3 },  // SINGLE
  { 6, 2 },  // FINGER
  { 6, 1 },  // FULL CHORD
  { 7, 4 },  // TEMPO UP
  { 7, 3 },  // TEMPO DOWN
  { 8, 2 },  // RHYTHM VOL +
  { 8, 1 },  // RHYTHM VOL -
};

uint8_t XZ_array[1][2] = {
  { 4, 1 },  // MEMORY
};

uint8_t YZ_array[22][2] = {
  { 3, 1 },  // TONE
  { 2, 1 },  // RHYTHM
  { 1, 1 },  // SONG
  { 4, 2 },  // 1
  { 3, 2 },  // 2
  { 2, 2 },  // 3
  { 1, 2 },  // 4
  { 4, 3 },  // 5
  { 3, 3 },  // 6
  { 2, 3 },  // 7
  { 1, 3 },  // 8
  { 4, 4 },  // 9
  { 3, 4 },  // 0
  { 2, 4 },  // +
  { 1, 4 },  // -
  { 4, 5 },  // DEMO
  { 3, 5 },  // LESSON A
  { 2, 5 },  // LESSON B
  { 4, 6 },  // RECORD
  { 3, 6 },  // R-PLAY
  { 2, 6 },  // PROG
  { 1, 6 },  // P-PLAY
};


// SET PINS Y1 - Y4 DIRECTLY ON MCU


// in scene mode, use cc64 to trigger all scenes, momentary

// in parts mode, use cc0 and 127 to trigger on or off - latch




// Mux { s0, s1, s2, s3, sig}
// Mux 1 {15, 16, 17, 18, 36}// Keys Input cols[blue], buttons x
// Mux 2 {42, 41, 40, 39, 38}// 0utput rows
// Mux 3 {4, 7, 6, 7 37}// buttons y
// Mux 4 {8, 3, 46, 9, ADS1115 a3}// buttons y
// Buttons z {13, 12, 11, 10}


// Buttons X ( mux 2 [1 - 8]) = [15, 14, 13, 12, 11, 10, 9, 8]
// Buttons Y ( mux 3 [1 - 6]) = [15, 14, 13, 12, 11, 10]
// Keys Output kps[blue] (mux 1) = [0, 1, 2, 3, 4, 5, 6, 7]
// Keys Output kpe[blue] (mux 1) = [8, 9, 10, 11, 12, 13, 14, 15]
// Keys Input cols[blue] (mux 2 [1 - 8]) = [0, 1, 2, 3, 4, 5, 6, 7]
// Faders 1- 9 (mux 4) = [0, 1, 2, 3, 4, 5, 6, 7, 8]
// Knobs 1- 7 (mux 4) = [9, 10, 11, 12, 13, 14, 15]


#endif