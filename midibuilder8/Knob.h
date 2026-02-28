#ifndef KNOB_H
#define KNOB_H

#include <Arduino.h>
#include "MuxHelper.h"

// Analog Input for Knobs and Faders alike
class Knob {
private:
  uint8_t _potPin;  // Mux channel connected to potentiometer
  uint8_t _CCNumber;
  uint8_t _channel;
  uint8_t _min;  // Minimum CC value
  uint8_t _max;  // Maximum CC value
  uint16_t _potState, _potPState;
  uint8_t _midiState = 0;
  uint8_t _midiPState = 0;
  unsigned long snapshot;  // Pot time recorder snapshot
  inline static const uint8_t _potThreshold = 15;
  inline static const uint8_t POT_TIMEOUT = 300;

public:
  // Constructors
  Knob(uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max, uint8_t channel);
  Knob(uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max);
  Knob(uint8_t potPin, uint8_t CCNumber);

  // Getters
  uint8_t getCCNumber();
  uint8_t getMinMax();  // Returns min and max CC values of knob

  // Setters
  void setMIDIChannel(uint8_t channel);
  void setMin(uint8_t value);
  void setMax(uint8_t value);

  // Methods
  void update();
  void readKnob();
  void validateAnalogRead(uint16_t reading);
};


#endif