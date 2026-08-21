#include "MIDIHelper.h"

#include <Arduino.h>
#include "Screen.h"

// =============================  MIDI VARIABLES  =============================
USBMIDI usbmidi("ANNIHILO");

BleMidiTransport_t BLEMIDI_TRANSPORT("Annihilō BLE");
BleMidiInterface_t BLEMIDI_MIDI((BleMidiTransport_t&)BLEMIDI_TRANSPORT);

uint8_t GLOBAL_MIDI_CHANNEL = 1;
uint8_t KEYS_CHANNEL = GLOBAL_MIDI_CHANNEL;
bool BLE_MIDI_IS_CONNECTED = true;
// ============================================================================

void updateChannel(int8_t value) {
  GLOBAL_MIDI_CHANNEL = constrain(GLOBAL_MIDI_CHANNEL + value, 1, 16);
  screen.printChannel();
}

void channelUp() {
  updateChannel(1);
  // Serial.println(GLOBAL_MIDI_CHANNEL);
}

void channelDown() {
  updateChannel(-1);
  // Serial.println(GLOBAL_MIDI_CHANNEL);
}

void noteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
  usbmidi.noteOn(note, velocity, channel);
  if (BLE_MIDI_IS_CONNECTED) BLEMIDI_MIDI.sendNoteOn(note, velocity, channel);
}

void noteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
  usbmidi.noteOff(note, velocity, channel);
  if (BLE_MIDI_IS_CONNECTED) BLEMIDI_MIDI.sendNoteOff(note, velocity, channel);
}

void controlChange(uint8_t channel, uint8_t control, uint8_t value) {
  usbmidi.controlChange(control, value, channel);
  if (BLE_MIDI_IS_CONNECTED) BLEMIDI_MIDI.sendControlChange(control, value, channel);
}

void pitchBend(uint8_t channel, int16_t value) {
  // The generic "byte" data type cannot be used here to represent values greater than 256 ()
  // this is because it comprises of just 8 bits, with a max possible permutation of 256 (2**8)
  // Thus, we must explicitly specicy to use the 16 bits variant (int16_t or uint16_t) to represent
  // a range of 0 - 16383 or -8192 to 8191. (2**14)
  usbmidi.pitchBend((int16_t)value, channel);
  if (BLE_MIDI_IS_CONNECTED) BLEMIDI_MIDI.sendPitchBend(value, channel);
}

void centerPitchWheel() {
  uint8_t value = 0;
  pitchBend(GLOBAL_MIDI_CHANNEL, value);
}

void allNotesOff() {
  // Send all notes off on selected channel
  controlChange(GLOBAL_MIDI_CHANNEL, 123, 127);
  // Set CC123 back to 0
  controlChange(GLOBAL_MIDI_CHANNEL, 123, 0);
}

void allSoundsOff() {
  allNotesOff();
  controlChange(GLOBAL_MIDI_CHANNEL, 120, 64);
}
