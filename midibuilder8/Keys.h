#ifndef KEYS_H
#define KEYS_H

#include <Arduino.h>
#include "MuxHelper.h"
#include "MIDIHelper.h"

#define GET_BIT(array, row, col) ((array[row] >> col) & 0x01)
#define SET_BIT(array, row, col) (array[row] |= (1 << col))
#define CLEAR_BIT(array, row, col) (array[row] &= ~(1 << col))

// ==============================  KEYS VARIABLES  =====================================
// KEYSCAN MATRIX VARIABLES
const uint8_t COL_NUM = 8;
const uint8_t ROW_NUM = 8;

uint8_t transpose = 12;
uint8_t note, velocity;
uint8_t vel_min = 0;
uint8_t vel_max = 45;

const uint8_t PROGMEM nums[ROW_NUM][COL_NUM] = {
  // Array  of midi note numbers C1 (24) to D#6 (87), 64 notes in total.
  { 24, 25, 26, 27, 28, 29, 30, 31 },
  { 32, 33, 34, 35, 36, 37, 38, 39 },
  { 40, 41, 42, 43, 44, 45, 46, 47 },
  { 48, 49, 50, 51, 52, 53, 54, 55 },
  { 56, 57, 58, 59, 60, 61, 62, 63 },
  { 64, 65, 66, 67, 68, 69, 70, 71 },
  { 72, 73, 74, 75, 76, 77, 78, 79 },
  { 80, 81, 82, 83, 84, 85, 86, 87 },
};

uint8_t cols[COL_NUM] = { 0, 1, 2, 3, 4, 5, 6, 7 };       // Blue cols (Mux1 0 - 7) input_pullup
uint8_t KPS[ROW_NUM] = { 0, 1, 2, 3, 4, 5, 6, 7 };        // Brown rows (Mux2 0 - 7), output
uint8_t KPE[ROW_NUM] = { 8, 9, 10, 11, 12, 13, 14, 15 };  // White rows (Mux2 8 - 15), output

enum KeyErrorCode {
  ERROR1,                // kpe before kps
  ERROR_INVALID_TIMING,  // kpe takes too long to read after kps (10s) - timing sanity check
  ERROR_KEY_STUCK,
  ERROR_ADC_FAULT
};

// Array to keep track of previous states of kps and kpe data for all keys
uint8_t pState[2][ROW_NUM][COL_NUM] = { 0 };  // pState[2] for kps[x][y] and kpe[x][y]
uint8_t temp;                                 // variable for temporary storage

// Rather than use an 8x8 matrix, consuming 64 bytes per matrix, we shall employ bit packing.
// Using an array of 8 bytes, 1 byte per item, every bit within every byte shall represent a data point
// The macros GET_BIT, SET_BIT and CLEAR_BIT shall then be used to manipulate the arrays
// This method consumes 8 bytes alone per 8x8 matrix

// Arrays to keep track of present states of kps and kpe data for all keys
bool kps[ROW_NUM][COL_NUM] = { 0 };
bool kpe[ROW_NUM][COL_NUM] = { 0 };

// byte kps[ROW_NUM] = { 0 };
// byte kpe[ROW_NUM] = { 0 };


enum KeyState {
  KEY_IDLE,
  KEY_HALF_PRESSED,
  KEY_FULL_PRESSED,
  KEY_RELEASING,
  KEY_STUCK,
  KEY_ERROR
};

// uint8_t keyFault[ROW_NUM][COL_NUM] = { 0 };

KeyState keyState[ROW_NUM][COL_NUM] = { KEY_IDLE };

// TIMER VARIABLES
unsigned long timeOfStart[ROW_NUM][COL_NUM] = { 0 };  // time of keypress start kps[x][y]
unsigned long timeOfEnd[ROW_NUM][COL_NUM] = { 0 };    // time of keypress end kpe[x][y]
int16_t time;
//  ===========================================================================

void allNotesOff() {
  // Set all notes of on selected channel
  controlChange(KEYS_CHANNEL, 123, 127);
  // Set CC123 back to 0
  controlChange(KEYS_CHANNEL, 123, 0);
}

void setKeysChannel(uint8_t channel) {
  allNotesOff();
  KEYS_CHANNEL = channel;
}

bool checkStuckKey(uint8_t x, uint8_t y) {
  // Check if 10 seconds have passed since last key update HALF_PRESS or FULL_PRESS
  if (keyState[x][y] == KEY_HALF_PRESSED) {
    if (millis() - timeOfStart[x][y] >= 10000) {
      return 1;
    } else {
      return 0;
    }
  } else if (keyState[x][y] == KEY_FULL_PRESSED || keyState[x][y] == KEY_RELEASING) {
    if (millis() - timeOfEnd[x][y] >= 10000) {
      return 1;
    } else {
      return 0;
    }
  }
}

// Return to these functions
void resetKey(uint8_t x, uint8_t y) {
  timeOfStart[x][y] = 0;
  timeOfEnd[x][y] = 0;
  keyState[x][y] = KEY_IDLE;

  noteOff(KEYS_CHANNEL, note, 127);
}

void scanKey(uint8_t x, uint8_t y) {
  // Reset key if the key is stuck
  if (keyState[x][y] == KEY_STUCK) {
    resetKey(x, y);
    // Enact later error logging after 5 consecutive key_stuck
  }

  // if keyState is IDLE or HALF_PRESSED
  if ((keyState[x][y] == KEY_IDLE) || (keyState[x][y] == KEY_HALF_PRESSED)) {

    // Shift mux to Keypress-start (KPS) channel and read the digital input of note[x][y]
    Mux2.writeToChannel(KPS[x], LOW);
    temp = !Mux1.readChannel(cols[y]);
    Mux2.write(HIGH);  // Still tentative, might remove later

    if (temp != pState[0][x][y]) {
      timeOfStart[x][y] = (temp == 1) ? millis() : 0;
      kps[x][y] = temp;
      pState[0][x][y] = temp;
    }

    // Check for stuck key
    if (checkStuckKey(x, y)) {
      keyState[x][y] = KEY_STUCK;
    }

    // Shift mux to Keypress-end (KPE) channel and read the digital input of note[x][y]
    Mux2.writeToChannel(KPE[x], LOW);
    temp = !Mux1.readChannel(cols[y]);
    Mux2.write(HIGH);

    if (temp != pState[1][x][y]) {
      timeOfEnd[x][y] = (temp == 1) ? millis() : 0;
      kpe[x][y] = temp;
      pState[1][x][y] = temp;
    }
  }
}

void checkPressLevel(uint8_t x, uint8_t y) {
  if ((kps[x][y] == 1) && (kpe[x][y] == 0)) {
    keyState[x][y] = KEY_HALF_PRESSED;
  } else if (kps[x][y] && kpe[x][y]) {
    keyState[x][y] = KEY_FULL_PRESSED;
  } else if ((kps[x][y] == 0) && (kpe[x][y] == 1)) {
    keyState[x][y] = KEY_ERROR;
    Serial.println("Error with key " + String(note - 23) + ". kpe before kps");
    // Remember to state the error code, log the error and increment the error counter.
  }
}

void performTimingSanityCheck(uint8_t x, uint8_t y) {
  // Check for timing anomilaies
  time = abs(timeOfEnd[x][y] - timeOfStart[x][y]);

  // Sanity check: physically impossible timing
  if (time == 0) {
    // Both switches triggered simultaneously - hardware glitch
    time = vel_max / 2;  // Use medium velocity
  }
}

void checkForKeyReleasing(uint8_t x, uint8_t y) {
  // Check for stuck key
  if (checkStuckKey(x, y)) {
    keyState[x][y] = KEY_STUCK;
  }

  // Shift mux to Keypress-start (KPS) channel and read the digital input of note[x][y]
  Mux2.writeToChannel(KPS[x], LOW);
  kps[x][y] = !Mux1.readChannel(cols[y]);

  // Shift mux to Keypress-end (KPE) channel and read the digital input of note[x][y]
  Mux2.writeToChannel(KPE[x], LOW);
  kpe[x][y] = !Mux1.readChannel(cols[y]);

  Mux2.write(HIGH);  // Tentative, might remove later
  if (!kps[x][y] && !kpe[x][y]) {
    noteOff(KEYS_CHANNEL, note, velocity);
    keyState[x][y] = KEY_IDLE;
    timeOfStart[x][y] = 0;
    timeOfEnd[x][y] = 0;
  }
}

void updateKey(uint8_t x, uint8_t y) {

  note = pgm_read_byte(&nums[x][y]) + transpose;
  scanKey(x, y);
  checkPressLevel(x, y);

  // Sends a noteOn midi message when keypress is complete
  if (keyState[x][y] == KEY_FULL_PRESSED) {
    performTimingSanityCheck(x, y);
    // Serial.println(time); // DEBUGGER
    velocity = map(constrain(time, vel_min, vel_max), vel_max, vel_min, 5, 127);
    noteOn(KEYS_CHANNEL, note, velocity);
    keyState[x][y] = KEY_RELEASING;
  }

  if (keyState[x][y] == KEY_RELEASING) {
    checkForKeyReleasing(x, y);
  }
}

void updateKeys() {
  // READ THROUGH ALL KEYS
  for (uint8_t y = 0; y < COL_NUM; y++) {
    for (uint8_t x = 0; x < ROW_NUM; x++) {
      updateKey(x, y);
    }
  }
}

#endif