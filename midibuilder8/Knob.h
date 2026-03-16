#ifndef KNOB_H
#define KNOB_H

#include <Arduino.h>
#include "Multiplexer.h"

// Analog Inputs for Knobs and Faders alike

// ========================  KNOB  =========================
// =========================================================
// Regular knobs connected directly to MCU's ADC pins

class Knob {
protected:
  uint8_t _potPin;  // Mux channel connected to potentiometer
  bool _isEnabled;
  uint8_t _CCNumber;
  uint8_t _channel;
  uint16_t _minAnalogValue;  // Maximum analog value from potentiometer readings (0 - 1023) Read in 10 bits
  uint16_t _maxAnalogValue;  // Maximum analog value from potentiometer readings (0 - 1023) Read in 10 bits
  uint8_t _minCCValue;       // Minimum CC value
  uint8_t _maxCCValue;       // Maximum CC value
  uint16_t _potState, _potPState;
  uint8_t _midiState = 0;
  uint8_t _midiPState = 0;
  unsigned long snapshot;  // Pot time recorder snapshot
  inline static const uint8_t _potThreshold = 15;
  inline static const uint8_t POT_TIMEOUT = 300;

public:
  // Constructors
  Knob(uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max, uint8_t channel, bool isEnabled);
  Knob(uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max, uint8_t channel);
  Knob(uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max);
  Knob(uint8_t potPin, uint8_t CCNumber);

  struct MinMax {
    uint8_t min, max;
  };  // A struct to return the min and max values of the knob when getMinMax() is called.

  // Getters
  uint8_t getCCNumber() const;
  MinMax getMinMax() const;  // Returns min and max CC values of knob

  // Setters
  void setMIDIChannel(uint8_t channel);
  void setAnalogMin(uint16_t minAnalogValue);
  void setAnalogMax(uint16_t maxAnalogValue);
  void setCCMin(uint8_t CCMinValue);
  void setCCMax(uint8_t CCMaxValue);

  // Methods
  void enable();
  void disable();
  void readKnob();
  void validateAnalogRead(uint16_t reading);
  void update();
};
// =========================================================
// =========================================================


// ====================== KNOB ON MUX ======================
// =========================================================
// Modified knob class to read Analog from Multiplexer.
// Inherits the Knob class

class Knob_On_Mux : public Knob {
private:
  Mux& _mux;
  uint8_t _potPin;  // Mux channel connected to potentiometer

public:
  // Constructors
  Knob_On_Mux(Mux& mux, uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max, uint8_t channel, bool isEnabled);
  Knob_On_Mux(Mux& mux, uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max, uint8_t channel);
  Knob_On_Mux(Mux& mux, uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max);
  Knob_On_Mux(Mux& mux, uint8_t potPin, uint8_t CCNumber);

  // Getters

  // Setters

  // Methods
  void readKnob();
};
// =========================================================
// =========================================================


#endif