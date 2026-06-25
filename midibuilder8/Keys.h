#pragma once

#include "Multiplexer.h"
#include "MIDIHelper.h"
#include "Screen.h"

#define GET_BIT(array, row, col) ((array[row] >> col) & 0x01)
#define SET_BIT(array, row, col) (array[row] |= (1 << col))
#define CLEAR_BIT(array, row, col) (array[row] &= ~(1 << col))

struct Keys {
private:
  // ==============================  KEYS VARIABLES  =====================================
  // KEYSCAN MATRIX VARIABLES
  static constexpr uint8_t COL_NUM = 8;
  static constexpr uint8_t ROW_NUM = 8;

  const uint8_t PROGMEM nums[ROW_NUM][COL_NUM] = {
    // Array  of midi note numbers C1 (36) to D#6 (99), 64 notes in total.
    { 36, 37, 38, 39, 40, 41, 42, 43 },
    { 44, 45, 46, 47, 48, 49, 50, 51 },
    { 52, 53, 54, 55, 56, 57, 58, 59 },
    { 60, 61, 62, 63, 64, 65, 66, 67 },
    { 68, 69, 70, 71, 72, 73, 74, 75 },
    { 76, 77, 78, 79, 80, 81, 82, 83 },
    { 84, 85, 86, 87, 88, 89, 90, 91 },
    { 92, 93, 94, 95, 96, 97, 98, 99 },
  };

  enum KeyState {
    KEY_IDLE,
    KEY_HALF_PRESSED,
    KEY_FULL_PRESSED,
    KEY_RELEASING,
    KEY_STUCK,
    KEY_AWAITING_RECOVERY,
    KEY_ERROR,  // SANITY CHECK RETURNS IMPOSSIBLE VALUES, KPS BEFORE KPE
    KEY_FAULT,  // HAS PHYSICAL STUCK KEY FAULT, DISABLE / TURN OFF KEY
  };

  KeyState keyState[ROW_NUM][COL_NUM] = { KEY_IDLE };

  uint8_t cols[COL_NUM] = { 0, 1, 2, 3, 4, 5, 6, 7 };  // Blue cols (Mux1 0 - 7) input_pullup
  // uint8_t cols[COL_NUM] = { 8, 9, 10, 11, 12, 13, 14, 15 };  // Blue cols (Mux1 8 - 15) input_pullup
  uint8_t KPS[ROW_NUM] = { 0, 1, 2, 3, 4, 5, 6, 7 };        // Brown rows (Mux2 0 - 7), output
  uint8_t KPE[ROW_NUM] = { 8, 9, 10, 11, 12, 13, 14, 15 };  // White rows (Mux2 8 - 15), output

  // Rather than use an 8x8 matrix, consuming 64 bytes per matrix, we shall employ bit packing.
  // Using an array of 8 bytes, 1 byte per item, every bit within every byte shall represent a data point
  // The macros GET_BIT, SET_BIT and CLEAR_BIT shall then be used to manipulate the arrays
  // This method consumes 8 bytes alone per 8x8 matrix

  // Arrays to keep track of present states of kps and kpe data for all keys
  byte kps[ROW_NUM] = { 0 };
  byte kpe[ROW_NUM] = { 0 };

  // Array to keep track of previous states of kps and kpe data for all keys
  uint8_t pState[2][ROW_NUM][COL_NUM] = { 0 };  // pState[2] for kps[x][y] and kpe[x][y]
  uint8_t temp;                                 // variable for temporary storage

  // TIMER VARIABLES
  unsigned long timeOfStart[ROW_NUM][COL_NUM] = { 0 };  // time of keypress start kps[x][y]
  unsigned long timeOfEnd[ROW_NUM][COL_NUM] = { 0 };    // time of keypress end kpe[x][y]
  int16_t keyTravelTime;

  inline static int8_t transpose = 0;
  static constexpr int8_t transposeLowerLimit = -24;
  static constexpr int8_t transposeUpperLimit = 24;
  uint8_t note, velocity;
  uint8_t vel_min = 0;
  uint8_t vel_max = 45;

  // 2d 8 x 8 array to store pressed notes in memory
  // in case of a transpose after a keypress, this aids correct tracking and sending of pending noteoff messages
  uint8_t pressed_notes[ROW_NUM][COL_NUM] = { 0 };

  // A key logs as key_stuck when held past this amount of time in milliseconds.
  uint16_t KEY_STUCK_TIME_THRESHOLD = 25000;

  uint8_t CONSECUTIVE_KEY_STUCK_COUNT[ROW_NUM][COL_NUM] = { 0 };

  uint8_t KEY_STUCK_COUNT[ROW_NUM][COL_NUM] = { 0 };

  enum KeyErrorCode {
    ERROR1,                // kpe before kps
    ERROR_INVALID_TIMING,  // kpe takes too long to read after kps (10s) - timing sanity check
    ERROR_KEY_STUCK,       // two consecutive key_stucks.
    ERROR_ADC_FAULT
  };
  //  ===========================================================================

public:
  static int8_t getTranspose() {
    return transpose;
  }

   void updateTranspose(int8_t increment) {
    transpose = constrain(transpose + increment, transposeLowerLimit, transposeUpperLimit);
    Serial.println(getTranspose());
    screen.printTranspose();
  }

  inline static void transposeUp() {
    if (transpose + 1 <= transposeUpperLimit) transpose++;
    // Serial.print("Button Press Transpose Up: ");
    // Serial.println(getTranspose());
    screen.printTranspose();
  }

  inline static void transposeDown() {
    if (transpose - 1 >= transposeLowerLimit) transpose--;
    // Serial.print("Button Press Transpose Down: ");
    // Serial.println(getTranspose());
    screen.printTranspose();
  }

  inline static void octaveUp() {
    if (transpose + 12 <= transposeUpperLimit) transpose += 12;
    // Serial.println(transpose);
    screen.printTranspose();
  }

  inline static void octaveDown() {
    if (transpose - 12 >= transposeLowerLimit) transpose -= 12;
    // Serial.println(transpose);
    screen.printTranspose();
  }

  void setKeysChannel(uint8_t channel) {
    allNotesOff();
    KEYS_CHANNEL = channel;
  }

private:
  bool checkStuckKey(uint8_t x, uint8_t y) {
    if (keyState[x][y] == KEY_HALF_PRESSED) {
      return (millis() - timeOfStart[x][y] >= KEY_STUCK_TIME_THRESHOLD);
    } else if (keyState[x][y] == KEY_FULL_PRESSED || keyState[x][y] == KEY_RELEASING) {
      return (millis() - timeOfEnd[x][y] >= KEY_STUCK_TIME_THRESHOLD);
    }
    return false;  // KEY_IDLE, KEY_STUCK, KEY_ERROR
  }

  // Return to these functions
  void resetStuckKey(uint8_t x, uint8_t y) {
    KEY_STUCK_COUNT[x][y] += 1;
    noteOff(GLOBAL_MIDI_CHANNEL, pressed_notes[x][y], 127);
    keyState[x][y] = KEY_AWAITING_RECOVERY;
  }

  void scanKey(uint8_t x, uint8_t y) {
    // Reset key if the key is stuck
    if (keyState[x][y] == KEY_STUCK) {
      KEY_STUCK_COUNT[x][y] += 1;
      resetStuckKey(x, y);
      // Enact later error logging after 2 consecutive key_stuck
    }

    // if keyState is IDLE or HALF_PRESSED
    if ((keyState[x][y] == KEY_IDLE) || (keyState[x][y] == KEY_HALF_PRESSED) || (keyState[x][y] == KEY_AWAITING_RECOVERY)) {

      // Shift mux to Keypress-start (KPS) channel and read the digital input of note[x][y]
      Mux2.writeToChannel(KPS[x], LOW);
      temp = !Mux1.readChannel(cols[y]);
      Mux2.write(HIGH);  // Still tentative, might remove later

      if (temp != pState[0][x][y]) {
        timeOfStart[x][y] = (temp == 1) ? millis() : 0;
        // kps[x][y] = temp;
        if (temp) {
          SET_BIT(kps, x, y);
        } else {
          CLEAR_BIT(kps, x, y);
        }
        pState[0][x][y] = temp;
      }

      // Check for stuck key
      if (checkStuckKey(x, y)) {
        keyState[x][y] = KEY_STUCK;
        // Serial.print("Key ");
        // Serial.print(note);
        // Serial.println(" STUCK!");
      }

      // Shift mux to Keypress-end (KPE) channel and read the digital input of note[x][y]
      Mux2.writeToChannel(KPE[x], LOW);
      temp = !Mux1.readChannel(cols[y]);
      Mux2.write(HIGH);

      if (temp != pState[1][x][y]) {
        timeOfEnd[x][y] = (temp == 1) ? millis() : 0;
        temp ? SET_BIT(kpe, x, y) : CLEAR_BIT(kpe, x, y);
        pState[1][x][y] = temp;
      }
    }
  }

  void checkPressLevel(uint8_t x, uint8_t y) {
    if (keyState[x][y] != KEY_RELEASING) {

      if (keyState[x][y] == KEY_AWAITING_RECOVERY) {

        if (!GET_BIT(kps, x, y) && !GET_BIT(kpe, x, y)) {
          keyState[x][y] = KEY_IDLE;
          // Serial.print("Key ");
          // Serial.print(note);
          // Serial.println(" RECOVERED!");
          return;

        } else {
          return;
        }
      }

      if (GET_BIT(kps, x, y) && !GET_BIT(kpe, x, y)) {

        keyState[x][y] = KEY_HALF_PRESSED;
        // Serial.print("Key ");
        // Serial.print(note);
        // Serial.println(" is half pressed");

      } else if (GET_BIT(kps, x, y) && GET_BIT(kpe, x, y)) {

        keyState[x][y] = KEY_FULL_PRESSED;
        // Serial.print("Key ");
        // Serial.print(note);
        // Serial.println(" is fully pressed");


      } else if (!GET_BIT(kps, x, y) && GET_BIT(kpe, x, y)) {
        keyState[x][y] = KEY_ERROR;
        Serial.println("Error with key " + String(note - 36) + ". kpe before kps");
        keyState[x][y] = KEY_AWAITING_RECOVERY;
        // Remember to state the error code, log the error and increment the error counter.
      } else if (!GET_BIT(kps, x, y) && !GET_BIT(kpe, x, y)) {
        keyState[x][y] = KEY_IDLE;
      }
    }
  }

  void performTimingSanityCheck(uint8_t x, uint8_t y) {
    // Check for timing anomilaies
    keyTravelTime = timeOfEnd[x][y] - timeOfStart[x][y];

    // Sanity check: physically impossible timing
    if (keyTravelTime == 0) {
      // Both switches triggered simultaneously - hardware glitch
      keyTravelTime = vel_max / 2;  // Use medium velocity
    }
  }

  void checkForKeyReleasing(uint8_t x, uint8_t y) {
    // Check for stuck key
    if (checkStuckKey(x, y)) {
      keyState[x][y] = KEY_STUCK;
      // Serial.print("Key ");
      // Serial.print(note);
      // Serial.println(" STUCK!");
    }

    // Shift mux to Keypress-start (KPS) channel and read the digital input of note[x][y]
    Mux2.writeToChannel(KPS[x], LOW);
    !Mux1.readChannel(cols[y]) ? SET_BIT(kps, x, y) : CLEAR_BIT(kps, x, y);  // kps[x][y] = !Mux1.readChannel(cols[y]);

    // Shift mux to Keypress-end (KPE) channel and read the digital input of note[x][y]
    Mux2.writeToChannel(KPE[x], LOW);
    !Mux1.readChannel(cols[y]) ? SET_BIT(kpe, x, y) : CLEAR_BIT(kpe, x, y);  // kpe[x][y] = !Mux1.readChannel(cols[y]);
    Mux2.write(HIGH);                                                        // Tentative, might remove later

    // Send noteoff when key release complete
    if (!GET_BIT(kps, x, y) && !GET_BIT(kpe, x, y)) {
      noteOff(GLOBAL_MIDI_CHANNEL, pressed_notes[x][y], velocity);
      // Serial.print("Note ");
      // Serial.print(note);
      // Serial.println(" released (Note Off)");

      keyState[x][y] = KEY_IDLE;
      timeOfStart[x][y] = 0;
      timeOfEnd[x][y] = 0;
    }
  }

  void updateKey(uint8_t x, uint8_t y) {

    note = pgm_read_byte(&nums[x][y]) + transpose;
    // Serial.print("key selected is ");
    // Serial.println(note);
    scanKey(x, y);
    checkPressLevel(x, y);

    // Sends a noteOn midi message when keypress is complete
    if (keyState[x][y] == KEY_FULL_PRESSED) {
      performTimingSanityCheck(x, y);
      // Serial.println(keyTravelTime); // DEBUGGER
      velocity = map(constrain(keyTravelTime, vel_min, vel_max), vel_max, vel_min, 5, 127);
      noteOn(GLOBAL_MIDI_CHANNEL, note, velocity);
      // Serial.print("Note ");
      // Serial.print(note);
      // Serial.println(" pressed (Note On)");


      pressed_notes[x][y] = note;
      keyState[x][y] = KEY_RELEASING;
    } else if (keyState[x][y] == KEY_RELEASING) {
      checkForKeyReleasing(x, y);
    }
  }

public:
  void updateKeys() {
    // READ THROUGH ALL KEYS
    for (uint8_t x = 0; x < ROW_NUM; x++) {
      for (uint8_t y = 0; y < COL_NUM; y++) {
        updateKey(x, y);
      }
    }
  }
};


extern Keys keys;

// if keystuck, free key, log keystuck, increment count:
// if 2 consecutive stuck key; disable key with KEY_ERROR state, log key as not working.
// Possibly save this setting to EEPROM for reference on boot.
// once 5 total stuck key recorded during uptime; log key as faulty, save to EEPROM.