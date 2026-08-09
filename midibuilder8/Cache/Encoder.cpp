#include <Arduino.h>
#include "Encoder.h"
#include "Screen.h"
#include "Keys.h"
#include "MIDIHelper.h"

// Define static members
Encoder* Encoder::instance = nullptr;

// Constructors
Encoder::Encoder(uint8_t PIN_A, uint8_t PIN_B) {
  _PIN_A = PIN_A;
  _PIN_B = PIN_B;
}

Encoder::Encoder()
  : Encoder(2, 1) {
}

// Getters

// Setters

// Methods

// This method shall be called in case of change in pin numbering, encoder resolution or rotation
void Encoder::initializeEncoder() {
  // Configure encoder pins as inputs with pull-up resistors
  pinMode(_PIN_A, INPUT);  // External pullup resistors are used
  pinMode(_PIN_B, INPUT);  // External pullup resistors are used
  _lastEncoded = readState();

  // Attach interrupts for encoder channels/pins
  attachInterrupt(digitalPinToInterrupt(_PIN_A), updateEncoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(_PIN_B), updateEncoderISR, CHANGE);

  // Bind ISR to this instance
  instance = this;
}

uint8_t Encoder::readState() {
  uint8_t MSB = digitalRead(_PIN_A);    // Most significant bit
  uint8_t LSB = digitalRead(_PIN_B);    // Least significant bit
  return (uint8_t)((MSB << 1) | LSB);  // Convert to single number
}

void Encoder::updateEncoderISR() {
  if (instance != nullptr) {
    instance->updateEncoder();  // Call the actual member function
  }
}

// Interrupt service routine for encoder - removed artificial limits
void Encoder::updateEncoder() {

  uint8_t encoded = readState();      
  uint8_t sum = (_lastEncoded << 2) | encoded;  // Add it to previous encoded value

  // Determine direction based on state changes
  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) _encoderPos++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) _encoderPos--;

  _lastEncoded = encoded;  // Store this value for next time
  // Serial.print("Encoder ORIGINAL value: "); // DEBUGGER
  // Serial.println(_encoderPos); // DEBUGGER
}

int8_t Encoder::consumeDelta() {
  // portDISABLE_INTERRUPTS(); // should be used only within ISRs
  // portENABLE_INTERRUPTS(); // should be used only within ISRs

  int16_t d;
  noInterrupts();  // should be called for only a very short period, three lines at the most
  // else, the watchdog thinks the program has frozen even after just a few milliseconds and reboots

  _encoderVal = _encoderPos / 4;
  if (_encoderVal != _prevEncoderVal) {
    d = _encoderVal - _prevEncoderVal;
    _prevEncoderVal = _encoderVal;
  }
  interrupts();
  return d;
}

void Encoder::updateScreenValues() {

  int8_t d = consumeDelta();
  if (d) {
    switch (Screen::_page) {
      case Screen::PAGE_TRANSPOSE:
        keys.updateTranspose(d);
        break;
      case Screen::PAGE_CHANNEL:
        updateChannel(d);
        break;
      default:
        break;
    };
  }
}

// ===========================  ENCODER OBJECT  ===============================
Encoder encoder;
// ============================================================================