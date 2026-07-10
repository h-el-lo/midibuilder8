#pragma once

#include "MIDIHelper.h"
#include "Knob.h"
// #include "Joystick.h"
// #include "TouchSensor.h"

// =============================  MIDI VARIABLES  =============================
USBMIDI usbmidi("ANNIHILO");
uint8_t GLOBAL_MIDI_CHANNEL = 1;
uint8_t KEYS_CHANNEL = GLOBAL_MIDI_CHANNEL;
// ============================================================================

// // ==============================  JOYSTICK  ==================================
// Joystick joystick(1, 2);
// // ============================================================================

// // ===============================  SLIDER  ===================================
Knob Slider = Knob(/*usesADS*/ true, ADS_SLIDER_CHANNEL, /*CC*/1); // Modulation Wheel
// Knob Slide = Knob(/*usesADS*/ true, ADS_PITCH_CHANNEL, /*CC*/7); // Modulation Wheel
// // ============================================================================

// ===============================  KNOBS  ====================================
const uint8_t NUM_OF_KNOBS = 16;
Knob_On_Mux knobSet[NUM_OF_KNOBS] = {
  // Knob_On_Mux(controllingMux, knob_pin_mux_Channel, CC_Number_for_knob),
  Knob_On_Mux(Mux4, 0, 7),    // Channel volume - Knob 1
  Knob_On_Mux(Mux4, 1, 14),   // Knob 2
  Knob_On_Mux(Mux4, 2, 15),   // Knob 3
  Knob_On_Mux(Mux4, 3, 16),   // General Purpose Controller 1 - Knob 4
  Knob_On_Mux(Mux4, 4, 17),   // General Purpose Controller 2 - Knob 5
  Knob_On_Mux(Mux4, 5, 18),   // General Purpose Controller 3 - Knob 6
  Knob_On_Mux(Mux4, 6, 19),   // General Purpose Controller 4 - Knob 7
  Fader(Mux4, 7, 20),   // Fader 1
  Fader(Mux4, 8, 21),   // Fader 2
  Fader(Mux4, 9, 22),   // Fader 3
  Fader(Mux4, 10, 23),  // Fader 4
  Fader(Mux4, 11, 24),  // Fader 5
  Fader(Mux4, 12, 25),  // Fader 6
  Fader(Mux4, 13, 26),  // Fader 7
  Fader(Mux4, 14, 27),  // Fader 8
  Fader(Mux4, 15, 28),  // Fader 9
};
// ============================================================================

// // ============================  TOUCH SENSORS  ===============================
// const uint8_t NUM_OF_TOUCH_SENSORS = 4;
// TouchSensor touchSensors[NUM_OF_TOUCH_SENSORS] = {
//   TouchSensor(1, 66),
//   TouchSensor(2, 67),
//   TouchSensor(3, 68),
//   TouchSensor(4, 69),
// };
// // ============================================================================


// =======================================  METHODS  ========================================
void updateKnobs() {
  for (uint8_t i = 0; i < NUM_OF_KNOBS; i++) {
    knobSet[i].update();
  }
}

// void updateTouchSensors() {
//   for (uint8_t i = 0; i < NUM_OF_TOUCH_SENSORS; i++) {
//     touchSensors[i].update();
//   }
// }
