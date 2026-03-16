#include "Multiplexer.h"

// Constructors
Mux::Mux(uint8_t S0, uint8_t S1, uint8_t S2, uint8_t S3, uint8_t signalPin, uint8_t type, uint8_t mode)
  : _S0(S0), _S1(S1), _S2(S2), _S3(S3), _signalPin(signalPin), _type(type), _mode(mode) {

  pinMode(_S0, OUTPUT);
  pinMode(_S1, OUTPUT);
  pinMode(_S2, OUTPUT);
  pinMode(_S3, OUTPUT);
  pinMode(_signalPin, _mode);

  Mux::validate();
}

// Getters

// Setters
void Mux::setType() {
}

void Mux::setMode() {
}

// Methods
void Mux::validate() {
  _type = (_type == DIGITAL) ? DIGITAL : (_type == ANALOG) ? ANALOG
                                                           : DIGITAL;

  _mode = (_mode == INPUT) ? INPUT : (_mode == INPUT_PULLUP) ? INPUT_PULLUP
                                   : (_mode == OUTPUT)       ? OUTPUT
                                                             : INPUT;
}

void Mux::shiftSignalTo(uint8_t channel) {
  digitalWrite(_S0, channel & 0x01);
  digitalWrite(_S1, (channel >> 1) & 0x01);
  digitalWrite(_S2, (channel >> 2) & 0x01);
  digitalWrite(_S3, (channel >> 3) & 0x01);
  delayMicroseconds(50);  // for signal stabilization
}

uint16_t Mux::read() {
  if (_mode == INPUT || _mode == INPUT_PULLUP) {
    if (_type == DIGITAL) {
      return digitalRead(_signalPin);
    } else if (_type == ANALOG) {
      return analogRead(_signalPin);
    } else {
      Serial.println("Error on Mux : invalid mux type");
    }
  }
}

void Mux::write(uint8_t state) {
  // This mux class shall not yet perform analogWrite.
  if (_mode == OUTPUT) {
    digitalWrite(_signalPin, state);
  } else {
    Serial.println("Invalid Mux operation: attempting digitalWrite on an Input mux");
  }
}

uint16_t Mux::readChannel(uint8_t channel) {
  shiftSignalTo(channel);
  return read();
}

void Mux::writeToChannel(uint8_t channel, uint8_t state) {
  Mux::shiftSignalTo(channel);
  write(state);
};