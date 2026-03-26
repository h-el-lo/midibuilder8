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


#endif