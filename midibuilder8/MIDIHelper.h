#ifndef MIDIHELPER_H
#define MIDIHELPER_H

#include <Arduino.h>
#include <MIDIUSB.h>

extern uint8_t GLOBAL_MIDI_CHANNEL;
extern uint8_t KEYS_CHANNEL;


// These functions are defined as inline to prevent multiple definition errors when
// header is called in different files

inline void noteOn(byte channel, byte note, byte velocity) {
  midiEventPacket_t event = { 0x09, 0x90 | channel, note, velocity };
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

inline void noteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
  midiEventPacket_t event = { 0x08, 0x80 | channel, note, velocity };
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

inline void controlChange(uint8_t channel, uint8_t control, uint8_t value) {
  midiEventPacket_t event = { 0x0B, 0xB0 | channel, control, value };
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

inline void pitchBend(uint8_t channel, int value) {
  midiEventPacket_t event = { 0x0E, 0xE0 | channel, value & 0x7F, (value >> 7) & 0x7F };
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

inline void centerPitchWheel() {
  uint8_t value = 0;
  midiEventPacket_t event = { 0x0E, 0xE0 | KEYS_CHANNEL, value & 0x7F, (value >> 7) & 0x7F };
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

// Maybe in future versions, implement a special function for sendMIDI.
// This function should send the midi MESSAGES IN THE BUFFRER
// Buffer size shall be 64 messages(max length of keypresses per time)/ should be enough for knobs and special units (pedals, pitch wheel etc.)

#endif