#pragma once

#include <USB.h>
#include <USBMIDI.h>

extern USBMIDI usbmidi;


#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32.h>
// #include <hardware/BLEMIDI_ESP32_NimBLE.h>

typedef BLEMIDI_NAMESPACE::BLEMIDI_Transport<BLEMIDI_NAMESPACE::BLEMIDI_ESP32> BleMidiTransport_t; // Using ESP default BLE stack
// typedef BLEMIDI_NAMESPACE::BLEMIDI_Transport<BLEMIDI_NAMESPACE::BLEMIDI_ESP32_NimBLE> BleMidiTransport_t; // Using NimBLE
typedef MIDI_NAMESPACE::MidiInterface<BleMidiTransport_t> BleMidiInterface_t;

extern BleMidiTransport_t BLEMIDI_TRANSPORT;
extern BleMidiInterface_t BLEMIDI_MIDI;

extern uint8_t GLOBAL_MIDI_CHANNEL;
extern uint8_t KEYS_CHANNEL;
extern bool BLE_MIDI_IS_CONNECTED;

void updateChannel(int8_t value);
void channelUp();
void channelDown();
void noteOn(uint8_t channel, uint8_t note, uint8_t velocity);
void noteOff(uint8_t channel, uint8_t note, uint8_t velocity);
void controlChange(uint8_t channel, uint8_t control, uint8_t value);
void pitchBend(uint8_t channel, int16_t value);
void centerPitchWheel();
void allNotesOff();
void allSoundsOff();