#include "TouchSensor.h"
#include "MIDIHelper.h"

// Constructors

TouchSensor::TouchSensor(uint8_t pinNumber, uint8_t CCNumber, bool isInverted)
  : _pinNumber(pinNumber), _CCNumber(CCNumber), _inverted(isInverted) {
  init();
}

TouchSensor::TouchSensor(uint8_t pinNumber, uint8_t CCNumber)
  : TouchSensor(pinNumber, CCNumber, false) {
}

// Getters

// Setters
void TouchSensor::setMIDIChannel(uint8_t channel);

// Methods
void TouchSensor::init() {
  pinMode(_pinNumber, INPUT_PULLUP);
  _MIDIChannel = GLOBAL_MIDI_CHANNEL;
}

void TouchSensor::invert() {
  _inverted = !_inverted;
};

void TouchSensor::updateMIDI() {
  controlChange(_MIDIChannel, _CCNumber, _MIDIState);
}

void TouchSensor::update() {
  if (abs(millis() - _lastUpdatedTime) >= _debounceTime) {
    _state = digitalRead(_pinNumber);
    _state = !_state ? _inverted : _state;

    if (_state != _pState) {
      _MIDIState = map(_state, 0, 1, 0, 127);
      updateMIDI();
      _pState = _state;
    }
  }
}