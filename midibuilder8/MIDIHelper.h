#pragma once

#include <Arduino.h>
#include <USB.h>
#include <USBMIDI.h>
#include "Screen.h"

extern USBMIDI usbmidi;
extern uint8_t GLOBAL_MIDI_CHANNEL;
extern uint8_t KEYS_CHANNEL;

// These functions are defined as inline to prevent multiple definition errors when
// header is called in different files

inline void updateChannel(int8_t value) {
  GLOBAL_MIDI_CHANNEL = constrain(GLOBAL_MIDI_CHANNEL + value, 1, 16);
  screen.printChannel();
}

inline void channelUp() {
  updateChannel(1);
  // Serial.println(GLOBAL_MIDI_CHANNEL);
}

inline void channelDown() {
  updateChannel(-1);
  // Serial.println(GLOBAL_MIDI_CHANNEL);
}

inline void noteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
  usbmidi.noteOn(note, velocity, channel);
}

inline void noteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
  usbmidi.noteOff(note, velocity, channel);
}

inline void controlChange(uint8_t channel, uint8_t control, uint8_t value) {
  usbmidi.controlChange(control, value, channel);
}

inline void pitchBend(uint8_t channel, int16_t value) {
  // The generic "int" or "byte" data types cannot be used here to represent values greater than 256 ()
  // this is because it comprises of just 8 bits, with a max possible permutation of 256 (2**8)
  // Thus, we must explicitly specicy to use the 16 bits variant (int16_t or uint16_t) to represent
  // a range of 0 - 16383 or -8192 to 8191. (2**14)
  usbmidi.pitchBend((int16_t)value, channel);
}

inline void centerPitchWheel() {
  uint8_t value = 0;
  pitchBend(GLOBAL_MIDI_CHANNEL, value);
}

inline void allNotesOff() {
  // Send all notes off on selected channel
  controlChange(GLOBAL_MIDI_CHANNEL, 123, 127);
  // Set CC123 back to 0
  controlChange(GLOBAL_MIDI_CHANNEL, 123, 0);
}

inline void allSoundsOff() {
  allNotesOff();
  controlChange(GLOBAL_MIDI_CHANNEL, 120, 64);
}

// Maybe in future versions, implement a special function for sendMIDI.
// This function should send the midi MESSAGES IN THE BUFFRER
// Buffer size shall be 64 messages(max length of keypresses per time) / should be enough for knobs and special units (pedals, pitch wheel etc.)
