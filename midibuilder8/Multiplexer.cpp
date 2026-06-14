#include <sys/types.h>
#include <stdint.h>
#include "Multiplexer.h"

// Constructors
Mux::Mux(bool usesADS, uint8_t S0, uint8_t S1, uint8_t S2, uint8_t S3, /* Pin on ADS */ uint8_t signalPin, uint8_t mode, uint8_t type)
  : _usesADS(usesADS), _S0(S0), _S1(S1), _S2(S2), _S3(S3), _signalPin(signalPin), _mode(mode), _type(type) {
  pinMode(_S0, OUTPUT);
  pinMode(_S1, OUTPUT);
  pinMode(_S2, OUTPUT);
  pinMode(_S3, OUTPUT);
  if (!_usesADS) {
    // If signal pin is connected directlly to MCU
    pinMode(_signalPin, _mode);
  }
  Mux::validate();
}

Mux::Mux(uint8_t S0, uint8_t S1, uint8_t S2, uint8_t S3, uint8_t signalPin, uint8_t mode, uint8_t type)
  : Mux(false, S0, S1, S2, S3, signalPin, mode, type) {
}

// Getters
uint8_t Mux::getType() const {
  return _type;
}

uint8_t Mux::getMode() const {
  return _mode;
}




// Setters
void Mux::setType(uint8_t type) {
}

void Mux::setMode(uint8_t mode) {
  _mode = mode;
  pinMode(_signalPin, _mode);
}

// Methods
void Mux::validate() {
  _mode = (_mode == INPUT) ? INPUT : (_mode == INPUT_PULLUP) ? INPUT_PULLUP
                                   : (_mode == OUTPUT)       ? OUTPUT
                                                             : INPUT;

  _type = (_type == DIGITAL) ? DIGITAL : (_type == ANALOG) ? ANALOG
                                                           : DIGITAL;
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
  // Serial.print("Reading (");
  // Serial.print(_selectedChannel);
  // Serial.print("), ");

  if (_usesADS) {
    // Serial.print("ADS Read, ");
    if (_mode == INPUT) {
      if (_type == ANALOG) {
        // Serial.print("TYPE=ANALOG, ");
        // Serial.print("Value= ");
        ADSManager.selectChannel(_signalPin);
        // Serial.println(ADSManager.read());
        return ADSManager.read();

      } else if (_type == DIGITAL) {
        // Serial.print("TYPE=DIGITAL, ");
        // Serial.print("Value= ");
        ADSManager.selectChannel(_signalPin);
        // Serial.println(map(constrain(ADSManager.read(), 0, ADS_RAW_MAX), 0, ADS_RAW_MAX, 0, 1));
        return map(constrain(ADSManager.read(), 0, ADS_RAW_MAX), 0, ADS_RAW_MAX, 0, 1);

      } else {
        Serial.println("Error: Mux type cannot be determined");
      }
    } else {
      Serial.println("Error: Attempting to read from an output Mux!");
    }
  } else {
    // Serial.print("MUX read, ");
    if (_mode == INPUT) {
      // Serial.print("MODE=INPUT, ");
      if (_type == DIGITAL) {
        // Serial.print("TYPE=DIGITAL, ");
        // Serial.print("Value= ");
        // Serial.println(digitalRead(_signalPin));
        return digitalRead(_signalPin);
      } else if (_type == ANALOG) {
        // Serial.print("TYPE=ANALOG, ");
        // Serial.print("Value= ");
        // Serial.println(analogRead(_signalPin));
        return analogRead(_signalPin);
      } else {
        Serial.print("Error: Mux type cannot be determined");
      }
    } else if (_mode == INPUT_PULLUP) {
      // Serial.print("MODE=INPUT_PULLUP, ");
      // Serial.print("Value= ");
      // Serial.println(digitalRead(_signalPin));
      return digitalRead(_signalPin);
    } else {
      Serial.println("Error: Attempting to read from an output Mux!");
    }
    // Serial.println(".");
  }
}

void Mux::write(uint8_t state) {
  // This mux class shall not yet perform analogWrite.
  if (_mode == OUTPUT) {
    // Serial.print("Writing ");
    // Serial.print(state);
    // Serial.print(" to ");
    // Serial.println(_selectedChannel);
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
