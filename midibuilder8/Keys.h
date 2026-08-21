#pragma once
#include "Multiplexer.h"
#include "MIDIHelper.h"
#include "Screen.h"

struct Keys {
private:
  // ==============================  KEYS VARIABLES  =====================================
  // KEYSCAN MATRIX VARIABLES
  static constexpr uint8_t COL_NUM = 8;
  static constexpr uint8_t ROW_NUM = 8;

  int cols[COL_NUM] = { 0, 1, 2, 3, 4, 5, 6, 7 };       // Blue cols (Mux2 0 - 7) input_pullup
  int KPS[ROW_NUM] = { 0, 1, 2, 3, 4, 5, 6, 7 };        // Brown rows (Mux1 0 - 7), output
  int KPE[ROW_NUM] = { 8, 9, 10, 11, 12, 13, 14, 15 };  // White rows (Mux1 8 - 15), output

  uint16_t nums[ROW_NUM][COL_NUM] = {
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

  uint8_t note, velocity;
  uint8_t vel_min = 0;
  uint8_t vel_max = 45;

  inline static int8_t transpose = 0;
  static constexpr int8_t transposeLowerLimit = -24;
  static constexpr int8_t transposeUpperLimit = 24;

  // *******************************  KEYS STATE TRACKING *******************************
  // Array to keep track of previous states of kps and kpe data for all keys
  int pState[2][ROW_NUM][COL_NUM] = { 0 };  // pState[2] for kps[x][y] and kpe[x][y]
  int temp;                                 // variable for temporary storage
  // Arrays to keep track of present states of kps and kpe data for all keys
  bool kps[ROW_NUM][COL_NUM] = { 0 };
  bool kpe[ROW_NUM][COL_NUM] = { 0 };

  // The "not_ready[x][y]" variable name is used here because using "ready[x][y] = 1" would
  // set just ready[0][0] to "1", and all other elements to "0". The logic is then inverted
  // in variable naming and assignment to "0" instead. This way, one saves the stress of
  // having to hardcode the array, giving flexibility when modifying the program.

  // bool ready[ROW_NUM][COL_NUM] = { 1 };
  bool not_ready[ROW_NUM][COL_NUM] = { 0 };
  bool is_pressed[ROW_NUM][COL_NUM] = { 0 };

  // TIMER VARIABLES
  unsigned long timer[2][ROW_NUM][COL_NUM] = { 0 };  // timer[2] for kps[x][y] and kpe[x][y]
  int timing;

  // 2d 8 x 8 array to store pressed notes in memory
  // in case of a transpose after a keypress, this aids correct tracking and sending of pending noteoff messages
  uint8_t pressed_notes[ROW_NUM][COL_NUM] = { 0 };
  //  ===========================================================================


  // ============================  MIDI VARIABLES  =============================
  const uint8_t channel = 0;
  // ===========================================================================

public:
  static int8_t getTranspose() {
    return transpose;
  }

  void updateTranspose(int8_t increment) {
    transpose = constrain(transpose + increment, transposeLowerLimit, transposeUpperLimit);
    // Serial.println(getTranspose());
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

  void updateKeys() {

    // ==============================  READ THROUGH THE KEYS  ===============================
    for (int x = 0; x < ROW_NUM; x++) {

      for (int y = 0; y < COL_NUM; y++) {

        note = nums[x][y] + transpose;
        // if the selected note "nums[x][y]" is ready to be pressed, i.e, !not_ready
        if (!not_ready[x][y]) {

          // Shift mux to Keypress-start (KPS) channel and read the digital input of note[x][y]
          Mux2.writeToChannel(KPS[x], LOW);
          temp = !Mux1.readChannel(cols[y]);
          Mux2.write(HIGH);

          // if change recorded in kps of note
          if (temp != pState[0][x][y]) {
            if (temp == 1) {
              // begin a timer for the note, and re-record new state in
              timer[0][x][y] = millis();
              kps[x][y] = 1;
              pState[0][x][y] = temp;
            } else {
              timer[0][x][y] = 0;
              kps[x][y] = 0;
              pState[0][x][y] = temp;
            }
          }

          // Shift mux to Keypress-end (KPE) channel and read the digital input of note[x][y]
          Mux2.writeToChannel(KPE[x], LOW);
          temp = !Mux1.readChannel(cols[y]);
          Mux2.write(HIGH);

          if (temp != pState[1][x][y]) {
            if (temp == 1) {
              timer[1][x][y] = millis();
              kpe[x][y] = 1;
              pState[1][x][y] = temp;
            } else {
              timer[1][x][y] = 0;
              kpe[x][y] = 0;
              pState[1][x][y] = temp;
            }
          }

          if (kps[x][y] && kpe[x][y]) {
            // Declare key[x][y] as "pressed" and not ready to read another keypress
            is_pressed[x][y] = true;
            not_ready[x][y] = true;
          }
        }

        // Sends a noteOn midi message when keypress is complete
        if (is_pressed[x][y]) {
          timing = abs(int(timer[1][x][y] - timer[0][x][y]));
          velocity = map(constrain(timing, vel_min, vel_max), vel_max, vel_min, 10, 127);
          noteOn(GLOBAL_MIDI_CHANNEL, note, velocity);
          is_pressed[x][y] = false;
          pressed_notes[x][y] = note;
        }

        if (not_ready[x][y]) {

          Mux2.writeToChannel(KPS[x], LOW);
          kps[x][y] = !Mux1.readChannel(cols[y]);

          Mux2.writeToChannel(KPE[x], LOW);
          kpe[x][y] = !Mux1.readChannel(cols[y]);

          Mux2.write(HIGH);
          if (!kps[x][y] && !kpe[x][y]) {
            noteOff(GLOBAL_MIDI_CHANNEL, pressed_notes[x][y], velocity);
            not_ready[x][y] = false;
          }
        }
      }
    }
    // =======================================================================================
  }
};

extern Keys keys;