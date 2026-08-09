#include "Encoder.h"

// Static member definitions.
Encoder* Encoder::_instance = nullptr;
portMUX_TYPE Encoder::_mux = portMUX_INITIALIZER_UNLOCKED;

Encoder::Encoder(uint8_t pinA, uint8_t pinB)
  : _pinA(pinA), _pinB(pinB) {}

void Encoder::begin() {
  pinMode(_pinA, INPUT_PULLUP);
  pinMode(_pinB, INPUT_PULLUP);
  _instance = this;
  _lastState = readState();
  attachInterrupt(digitalPinToInterrupt(_pinA), isrTrampoline, CHANGE);
  attachInterrupt(digitalPinToInterrupt(_pinB), isrTrampoline, CHANGE);
}

uint8_t Encoder::readState() {
  uint8_t MSB = digitalRead(_pinA);    // Most significant bit
  uint8_t LSB = digitalRead(_pinB);    // Least significant bit
  return (uint8_t)((MSB << 1) | LSB);  // Convert to single number
}

int16_t Encoder::getDelta() {
  int16_t d;
  portENTER_CRITICAL(&_mux);
  d = _delta;
  _delta = 0;
  portEXIT_CRITICAL(&_mux);
  return d;
}

void IRAM_ATTR Encoder::isrTrampoline() {
  if (_instance) _instance->handleInterrupt();
}

// Runs in interrupt context -- keep it fast, no Serial, no allocation.
void IRAM_ATTR Encoder::handleInterrupt() {
  uint8_t state = readState();

  uint8_t idx = (uint8_t)((_lastState << 2) | state);
  
  // Determine direction based on state changes
  if (idx == 0b1101 || idx == 0b0100 || idx == 0b0010 || idx == 0b1011) _accum++;
  if (idx == 0b1110 || idx == 0b0111 || idx == 0b0001 || idx == 0b1000) _accum--;

  portENTER_CRITICAL_ISR(&_mux);
  if (_accum >= 4) {
    _delta++;
    _accum = 0;
  } else if (_accum <= -4) {
    _delta--;
    _accum = 0;
  }
  portEXIT_CRITICAL_ISR(&_mux);

  _lastState = state;
}