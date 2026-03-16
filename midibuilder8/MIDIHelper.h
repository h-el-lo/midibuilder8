#ifndef MIDIHELPER_H
#define MIDIHELPER_H

#include <Arduino.h>
#include "USB.h"
#include "USBMIDI.h"

extern USBMIDI usbmidi;
extern uint8_t GLOBAL_MIDI_CHANNEL;
extern uint8_t KEYS_CHANNEL;

// These functions are defined as inline to prevent multiple definition errors when
// header is called in different files

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
  usbmidi.pitchBend(value, channel);
}

inline void centerPitchWheel() {
  uint8_t value = 0;
  pitchBend(KEYS_CHANNEL, value);
}

inline void allNotesOff() {
  // Set all notes of on selected channel
  controlChange(KEYS_CHANNEL, 123, 127);
  // Set CC123 back to 0
  controlChange(KEYS_CHANNEL, 123, 0);
}

// Maybe in future versions, implement a special function for sendMIDI.
// This function should send the midi MESSAGES IN THE BUFFRER
// Buffer size shall be 64 messages(max length of keypresses per time)/ should be enough for knobs and special units (pedals, pitch wheel etc.)

#endif