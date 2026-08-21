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
void Encoder::init() {
  // Configure encoder pins as inputs with pull-up resistors
  pinMode(_PIN_A, INPUT);  // External pullup resistors are used
  pinMode(_PIN_B, INPUT);  // External pullup resistors are used
  instance = this;         // Bind ISR to this instance
  _lastState = readState();

  attachInterrupt(digitalPinToInterrupt(_PIN_A), updateEncoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(_PIN_B), updateEncoderISR, CHANGE);
}

uint8_t Encoder::readState() {
  uint8_t MSB = digitalRead(_PIN_A);   // Most significant bit
  uint8_t LSB = digitalRead(_PIN_B);   // Least significant bit
  return (uint8_t)((MSB << 1) | LSB);  // Convert to single number
}

void Encoder::updateEncoderISR() {
  if (instance != nullptr) {
    instance->handleInterrupt(); 
  }
}

// Interrupt service routine for encoder - removed artificial limits
void Encoder::handleInterrupt() {

  uint8_t state = readState();
  uint8_t sum = (_lastState << 2) | state;  // Add it to previous encoded value

  // Determine direction based on state changes
  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) _accum++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) _accum--;
  // noInterrupts();
  if (_accum >= 4) {
    _delta++;
    _accum = 0;
  } else if (_accum <= -4) {
    _delta--;
    _accum = 0;
  }
  // interrupts();
  _lastState = state;
}

int8_t Encoder::consumeDelta() {
  // portDISABLE_INTERRUPTS(); // should be used only within ISRs
  // portENABLE_INTERRUPTS(); // should be used only within ISRs

  int16_t d;
  // noInterrupts();  // should be called for only a very short period, three lines at the most
  // else, the watchdog thinks the program has frozen even after just a few milliseconds and reboots
  d = _delta;
  _delta = 0;
  // interrupts();
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