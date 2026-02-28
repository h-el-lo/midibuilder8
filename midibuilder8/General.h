#ifndef GENERAL_H
#define GENERAL_H

#include "Knob.h"
#include "TouchSensor.h"
#include "MuxHelper.h"

// =============================  MIDI VARIABLES  =============================
uint8_t GLOBAL_MIDI_CHANNEL = 1;
uint8_t KEYS_CHANNEL = GLOBAL_MIDI_CHANNEL;
// ============================================================================

// ==============================  MUX VARIABLES  =============================
Mux Mux1(15, 14, 16, 10, A0, DIGITAL, INPUT_PULLUP);  // Mux 1 (Digital INPUT_PULLUP (keys[COLS] [0-7]) (buttons[read] [8-15])) digital
Mux Mux2(9, 8, 7, 6, A1, DIGITAL, OUTPUT);            // Mux 2 (Outputs (keys), KPS AND KPE (rows))
Mux Mux3(5, 4, 3, 2, A2, DIGITAL, OUTPUT);            // Mux 3 (Digital output of buttons)
Mux Mux4(5, 4, 3, 2, A2, ANALOG, INPUT);              // Mux 4 (Analog Input, Knobs and Faders)
// ============================================================================

const uint8_t NUM_OF_KNOBS = 16;
Knob knobSet[NUM_OF_KNOBS] = {
  Knob(0, 1),    // Modulation Wheel (Fader)
  Knob(1, 7),    // Channel volume - Knob 1
  Knob(2, 11),   // Knob 2
  Knob(3, 11),   // Knob 3
  Knob(4, 16),   // General Purpose Controller 1 - Knob 4
  Knob(5, 17),   // General Purpose Controller 2 - Knob 5
  Knob(6, 18),   // General Purpose Controller 3 - Knob 6
  Knob(7, 19),   // General Purpose Controller 4 - Knob 7
  Knob(8, 20),   // Fader 1
  Knob(9, 21),   // Fader 2
  Knob(10, 22),  // Fader 3
  Knob(11, 23),  // Fader 4
  Knob(12, 24),  // Fader 5
  Knob(13, 25),  // Fader 6
  Knob(14, 26),  // Fader 7
  Knob(15, 27),  // Fader 8
};

const uint8_t NUM_OF_TOUCH_SENSORS = 4;
TouchSensor touchSensors[NUM_OF_TOUCH_SENSORS] = {
  TouchSensor(1, 66),
  TouchSensor(2, 67),
  TouchSensor(3, 68),
  TouchSensor(4, 69),
};



// =======================================  METHODS  ========================================
void updateKnobs() {
  for (uint8_t i = 0; i < NUM_OF_KNOBS; i++) {
    knobSet[i].update();
  }
}

void updateTouchSensors() {
  for (uint8_t i = 0; i < NUM_OF_TOUCH_SENSORS; i++) {
    touchSensors[i].update();
  }
}

#endif