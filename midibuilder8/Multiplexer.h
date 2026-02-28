#ifndef MULTIPLEXER_H
#define MULTIPLEXER_H

#include <Arduino.h>

#define ANALOG 0X1
#define DIGITAL 0X2

class Mux {
private:
  uint8_t _S0;
  uint8_t _S1;
  uint8_t _S2;
  uint8_t _S3;
  uint8_t _signalPin;
  uint8_t _type;
  uint8_t _mode;

public:
  // Constructors
  Mux(uint8_t S0, uint8_t S1, uint8_t S2, uint8_t S3, uint8_t signalPin, uint8_t type, uint8_t mode);  // INPUT, INPUT_PULLUP and OUTPUT

  // Getters

  // Setters

  // Methods
  void validate();
  void shiftSignalTo(uint8_t channel);
  uint16_t read();
  uint16_t readChannel(uint8_t channel);
  void write(uint8_t state);
  void writeToChannel(uint8_t channel, uint8_t state);
};

#endif
