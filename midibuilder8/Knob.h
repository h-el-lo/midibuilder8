#ifndef KNOB_H
#define KNOB_H

#include <Arduino.h>
#include "Multiplexer.h"
#include "MIDIHelper.h"

// Analog Inputs for Knobs and Faders alike

// ========================  KNOB  =========================
// =========================================================
// Regular knobs connected directly to MCU's ADC pins

class Knob {
protected:
  bool _usesADS = false;
  const uint8_t _potPin;  // Connected to MCU, ADS or MUX
  uint8_t _CCNumber;
  uint16_t _minAnalogValue = 0;     // Maximum analog value from potentiometer readings (0 - 1023) Read in 10 bits
  uint16_t _maxAnalogValue = 4096;  // Maximum analog value from potentiometer readings (0 - 1023) Read in 10 bits
  uint8_t _minCCValue = 0;          // Minimum CC value
  uint8_t _maxCCValue = 127;        // Maximum CC value
  int16_t _potState, _potPState;
  uint8_t _midiState = 0;
  uint8_t _midiPState = 0;
  uint8_t _channel;
  bool _isEnabled;

  unsigned long snapshot = millis();  // Pot time recorder snapshot
  uint16_t _potIncrement = 0;
  uint16_t _potTimer = 0;
  static constexpr uint8_t _potThreshold = 3;
  static constexpr uint16_t POT_TIMEOUT = 300;

  // Constructors
private:
  Knob(bool usesADS, uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max, uint8_t channel, bool isEnabled, bool configurablePin);
protected:
  Knob(uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max, uint8_t channel, bool isEnabled, bool configurablePin);

public:
  // Use constructors beginning with "usesADS" only when you intend on using
  // the knob objects with ADC as there is no fallback of setPinMode
  Knob(bool usesADS, uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue, uint8_t channel, bool isEnabled);
  Knob(bool usesADS, uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue, uint8_t channel);
  Knob(bool usesADS, uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue);
  Knob(bool usesADS, uint8_t potPin, uint8_t CCNumber);
  Knob(bool usesADS, uint8_t potPin);

  Knob(uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max, uint8_t channel, bool isEnabled);
  Knob(uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max, uint8_t channel);
  Knob(uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max);
  Knob(uint8_t potPin, uint8_t CCNumber);
  Knob(uint8_t potPin);

  struct MinMax {
    uint8_t min, max;
  };  // A struct to return the min and max values of the knob when getMinMax() is called.

  // Getters
  uint8_t getCCNumber() const;
  uint8_t getMin() const;
  uint8_t getMax() const;
  MinMax getMinMax() const;  // Returns min and max CC values of knob
  virtual uint8_t getMIDIChannel() const;

  // Setters
  virtual void setPinMode();
  virtual void setMIDIChannel(uint8_t channel);
  void setAnalogMin(uint16_t minAnalogValue);
  void setAnalogMax(uint16_t maxAnalogValue);
  void setCCMin(uint8_t CCMinValue);
  void setCCMax(uint8_t CCMaxValue);

  // Methods
  void enable();
  void disable();
  virtual void readKnob();
  void validateAnalogRead();
  virtual void update();
};
// =========================================================
// =========================================================


// ====================== KNOB ON MUX ======================
// =========================================================
// Modified knob class to read Analog from Multiplexer.
// Inherits the Knob class

class Knob_On_Mux : public Knob {
protected:
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


// ========================= FADER =========================
// =========================================================
// Modified knob class to read Analog from Multiplexer.
// Inherits the KnobOnMux class

class Fader : public Knob_On_Mux {
public:
  enum Bank {
    BANK_A,
    BANK_B
  };

private:
  inline static Bank _bank = BANK_A;
  inline static uint8_t _channel = GLOBAL_MIDI_CHANNEL;

public:
  // Constructors
  Fader(Mux& mux, uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max, uint8_t channel, bool isEnabled);
  Fader(Mux& mux, uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max, uint8_t channel);
  Fader(Mux& mux, uint8_t potPin, uint8_t CCNumber, uint8_t min, uint8_t max);
  Fader(Mux& mux, uint8_t potPin, uint8_t CCNumber);

  // Getters
  uint8_t getMIDIChannel() const override;

  // Setters
  void toggleBank();
  void setMIDIChannel(uint8_t channel) override{ /* no-op method*/ };

  // Methods
};
// =========================================================
// =========================================================


#endif