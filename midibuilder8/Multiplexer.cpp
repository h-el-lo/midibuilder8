#include <sys/types.h>
#include <stdint.h>
#include "Multiplexer.h"

// Constructors
Mux::Mux(uint8_t S0, uint8_t S1, uint8_t S2, uint8_t S3, uint8_t signalPin, uint8_t type, uint8_t mode, bool usesADS, uint8_t pinOnADS)
  : _S0(S0), _S1(S1), _S2(S2), _S3(S3), _signalPin(signalPin), _type(type), _mode(mode), _usesADS(usesADS), _pinOnADS(pinOnADS) {
  pinMode(_S0, OUTPUT);
  pinMode(_S1, OUTPUT);
  pinMode(_S2, OUTPUT);
  pinMode(_S3, OUTPUT);
  pinMode(_signalPin, _mode);
  Mux::validate();
}

Mux::Mux(uint8_t S0, uint8_t S1, uint8_t S2, uint8_t S3, uint8_t signalPin, uint8_t type, uint8_t mode)
  : Mux(S0, S1, S2, S3, signalPin, type, mode, false, 255) {
}



// Getters

// Setters
void Mux::setType(uint8_t type) {
}

void Mux::setMode(uint8_t mode) {
  _mode = mode;
  pinMode(_signalPin, _mode);
}

// Methods
void Mux::validate() {
  _type = (_type == DIGITAL) ? DIGITAL : (_type == ANALOG) ? ANALOG
                                                           : DIGITAL;

  _mode = (_mode == INPUT) ? INPUT : (_mode == INPUT_PULLUP) ? INPUT_PULLUP
                                   : (_mode == OUTPUT)       ? OUTPUT
                                                             : INPUT;
}

void Mux::selectChannel(uint8_t channel) {
  if (channel != _selectedChannel) {
  digitalWrite(_S0, channel & 0x01);
  digitalWrite(_S1, (channel >> 1) & 0x01);
  digitalWrite(_S2, (channel >> 2) & 0x01);
  digitalWrite(_S3, (channel >> 3) & 0x01);
  delayMicroseconds(5);  // for signal stabilization
  _selectedChannel = channel;
  }
}

uint16_t Mux::read() {
  if (_usesADS) {
    if (_mode == INPUT) {
      if (_type == ANALOG) {
        ADSManager.selectChannel(_pinOnADS);
        return ADSManager.read();
      } else if (_type == DIGITAL) {
        ADSManager.selectChannel(_pinOnADS);
        return map(constrain(ADSManager.read(), 0, ADS_RAW_MAX), 0, ADS_RAW_MAX, 0, 1);
      } else {
        Serial.println("Error: Mux type cannot be determined");
      }
    } else {
      Serial.println("Error: Attempting to read from an output Mux!");
    }
  } else {
    if (_mode == INPUT) {
      if (_type == DIGITAL) {
        return digitalRead(_signalPin);
      } else if (_type == ANALOG) {
        return analogRead(_signalPin);
      } else {
        Serial.println("Error: Mux type cannot be determined");
      }
    } else if (_mode == INPUT_PULLUP) {
      return digitalRead(_signalPin);
    } else {
      Serial.println("Error: Attempting to read from an output Mux!");
    }
  }
}

void Mux::write(uint8_t state) {
  // This mux class shall not yet perform analogWrite.
  if (_mode == OUTPUT) {
    digitalWrite(_signalPin, state);
  } else {
    Serial.println("Invalid Mux operation: attempting digitalWrite on an Input mux");
#warning "Invalid Mux operation: attempting digitalWrite on an Input mux";
  }
}

uint16_t Mux::readChannel(uint8_t channel) {
  Mux::selectChannel(channel);
  return read();
}

void Mux::writeToChannel(uint8_t channel, uint8_t state) {
  Mux::selectChannel(channel);
  write(state);
};
