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
  const uint8_t _potPin;  // Mux channel connected to potentiometer
  bool configurablePin;
  bool _isEnabled;
  uint8_t _CCNumber;
  uint8_t _channel;
  uint16_t _minAnalogValue = 0;     // Maximum analog value from potentiometer readings (0 - 1023) Read in 10 bits
  uint16_t _maxAnalogValue = 4096;  // Maximum analog value from potentiometer readings (0 - 1023) Read in 10 bits
  uint8_t _minCCValue = 0;          // Minimum CC value
  uint8_t _maxCCValue = 127;        // Maximum CC value
  uint16_t _potState, _potPState;
  uint8_t _midiState = 0;
  uint8_t _midiPState = 0;

  unsigned long snapshot = millis();  // Pot time recorder snapshot
  uint16_t _potIncrement = 0;
  uint16_t _potTimer = 0;
  static constexpr uint8_t _potThreshold = 4;
  static constexpr uint16_t POT_TIMEOUT = 300;
  static constexpr uint16_t ADC_MAX = 1023;

  Knob(uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max, uint8_t channel, bool isEnabled, bool configurablePin);

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
  virtual void setPinMode();
  void setMIDIChannel(uint8_t channel);
  void setAnalogMin(uint16_t minAnalogValue);
  void setAnalogMax(uint16_t maxAnalogValue);
  void setCCMin(uint8_t CCMinValue);
  void setCCMax(uint8_t CCMaxValue);

  // Methods
  void enable();
  void disable();
  virtual void readKnob();
  void validateAnalogRead();
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

public:
  // Constructors
  Knob_On_Mux(Mux& mux, uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max, uint8_t channel, bool isEnabled);
  Knob_On_Mux(Mux& mux, uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max, uint8_t channel);
  Knob_On_Mux(Mux& mux, uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max);
  Knob_On_Mux(Mux& mux, uint8_t potPin, uint8_t CCNumber);

  // Getters

  // Setters
  // This is a no-op method, a safety net if Knob_On_mux::setPinMode is ever called.
  // This way, it doesn't default to Knob::setPinMode
  void setPinMode() override;

  // Methods
  void readKnob() override;
};
// =========================================================
// =========================================================


#endif