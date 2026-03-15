#include "Knob.h"
#include "MIDIHelper.h"

// Constructors
Knob::Knob(uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue, uint8_t channel, bool isEnabled)
  : _potPin(potPin), _CCNumber(CCNumber), _minCCValue(minCCValue), _maxCCValue(maxCCValue), _channel(channel), _isEnabled(isEnabled) {
}

Knob::Knob(uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue, uint8_t channel)
  : Knob(potPin, CCNumber, minCCValue, maxCCValue, channel, true) {
}

Knob::Knob(uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue)
  : Knob(potPin, CCNumber, minCCValue, maxCCValue, GLOBAL_MIDI_CHANNEL, true) {
}

Knob::Knob(uint8_t potPin, uint8_t CCNumber)
  : Knob(potPin, CCNumber, 0, 127, GLOBAL_MIDI_CHANNEL, true) {
}

// Getters
uint8_t Knob::getCCNumber() const {
  return _CCNumber;
}

Knob::MinMax Knob::getMinMax() const {
  return { _minCCValue, _maxCCValue };
}

// Setters
void Knob::setMIDIChannel(uint8_t channel) {
  _channel = channel;
}

void Knob::setMin(uint8_t minCCValue) {
  _minCCValue = constrain(minCCValue, 0, 127);
}

void Knob::setMax(uint8_t maxCCValue) {
  _maxCCValue = constrain(maxCCValue, 0, 127);
}

// Methods
void Knob::disable() {
}

void Knob::readKnob() {
  _potState = Mux4.readChannel(_potPin);
}

void Knob::validateAnalogRead(uint16_t reading) {
  _potState = reading ? (reading >= 0 && reading <= 1023) : constrain(reading, 0, 1023);
  // Log OUT_OF_RANGE_ERROR -- Knob X
}

void Knob::update() {
  if (Knob::_isEnabled) {
    readKnob();
    validateAnalogRead(_potState);
    _midiState = map(_potState, 0, 1023, _minCCValue, _maxCCValue);

    static uint16_t potIncrement = abs(_potState - _potPState);

    if (potIncrement > _potThreshold) {
      snapshot = millis();
    }

    static uint16_t potTimer = millis() - snapshot;

    if (potTimer < POT_TIMEOUT) {
      if (_midiState != _midiPState) {
        controlChange(_channel, _CCNumber, _midiState);
        _midiPState = _midiState;
      }
      _potPState = _potState;
    }
  }
}
