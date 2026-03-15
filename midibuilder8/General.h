#ifndef GENERAL_H
#define GENERAL_H

#include "Knob.h"
#include "Joystick.h"
#include "TouchSensor.h"
#include "Multiplexer.h"
#include "MIDIHelper.h"

// =============================  MIDI VARIABLES  =============================
USBMIDI usbmidi;
uint8_t GLOBAL_MIDI_CHANNEL = 1;
uint8_t KEYS_CHANNEL = GLOBAL_MIDI_CHANNEL;
// ============================================================================

// ==============================  MUX VARIABLES  =============================
Mux Mux1(4, 5, 6, 7, 15, DIGITAL, INPUT_PULLUP);  // Mux 1 (Digital INPUT_PULLUP (keys[COLS] [0-7]) (buttons[read] [8-15])) digital
Mux Mux2(39, 38, 37, 36, 35, DIGITAL, OUTPUT);    // Mux 2 (Outputs (keys), KPS AND KPE (rows))
Mux Mux3(0, 45, 48, 47, 21, DIGITAL, OUTPUT);     // Mux 3 (Digital output of buttons)
Mux Mux4(16, 17, 18, 8, 3, ANALOG, INPUT);        // Mux 4 (Analog Input, Knobs and Faders)
// ============================================================================

// ==============================  JOYSTICK  ==================================
Joystick joystick(1, 2);
// ============================================================================

// ===============================  KNOBS  ====================================
const uint8_t NUM_OF_KNOBS = 16;
Knob knobSet[NUM_OF_KNOBS] = {
  Knob(0, 7),    // Channel volume - Knob 1
  Knob(1, 5),    // Knob 2
  Knob(2, 11),   // Knob 3
  Knob(3, 16),   // General Purpose Controller 1 - Knob 4
  Knob(4, 17),   // General Purpose Controller 2 - Knob 5
  Knob(5, 18),   // General Purpose Controller 3 - Knob 6
  Knob(6, 19),   // General Purpose Controller 4 - Knob 7
  Knob(7, 20),   // Fader 1
  Knob(8, 21),   // Fader 2
  Knob(9, 22),   // Fader 3
  Knob(10, 23),  // Fader 4
  Knob(11, 24),  // Fader 5
  Knob(12, 25),  // Fader 6
  Knob(13, 26),  // Fader 7
  Knob(14, 27),  // Fader 8
  Knob(15, 28),  // Fader 9
};
// ============================================================================

// ============================  TOUCH SENSORS  ===============================
const uint8_t NUM_OF_TOUCH_SENSORS = 4;
TouchSensor touchSensors[NUM_OF_TOUCH_SENSORS] = {
  TouchSensor(1, 66),
  TouchSensor(2, 67),
  TouchSensor(3, 68),
  TouchSensor(4, 69),
};
// ============================================================================



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
