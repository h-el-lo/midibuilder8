#pragma once

#include "Knob.h"
#include "MIDIHelper.h"

struct PitchWheel : public Knob {

private:
  uint8_t _potThreshold = 1;
  static constexpr uint16_t POT_TIMEOUT = 300; // May change later
  uint8_t DEADZONE_RANGE =25;
  uint16_t wheelCenter = (_minAnalogValue + _maxAnalogValue) / 2;


  void centerWheel() {
    centerPitchWheel();
  }


public:
  // Constructors
  PitchWheel()
    : Knob(/*usesADS*/ true, ADS_PITCH_CHANNEL) {
  }

  void update() override {
    if (_isEnabled) {
      readKnob();
      validateAnalogRead();

      _potIncrement = abs(_potState - _potPState);

      if (_potIncrement > _potThreshold) {
        snapshot = millis();
      }

      _potTimer = millis() - snapshot;

      if (_potTimer < POT_TIMEOUT) {
        if ((_potState <= wheelCenter + DEADZONE_RANGE) || (_potState <= wheelCenter - DEADZONE_RANGE)) {
          centerWheel();
        } else {
          pitchBend(KEYS_CHANNEL, map(constrain(_potState, _minAnalogValue, _maxAnalogValue), _minAnalogValue, _maxAnalogValue, -8192, 8191));
        }

        _potPState = _potState;
      }
    }
  }
};

extern PitchWheel pitchWheel;
