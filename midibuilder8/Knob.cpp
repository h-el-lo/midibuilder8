#include "Knob.h"
#include "MIDIHelper.h"

// ================================== REGULAR KNOB CLASS ================================================
// ======================================================================================================
// Constructors
// Protected Knob constructor
Knob::Knob(uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue, uint8_t channel, bool isEnabled, bool configurablePin)
  : _potPin(potPin), _CCNumber(CCNumber), _minCCValue(minCCValue), _maxCCValue(maxCCValue), _channel(channel), _isEnabled(isEnabled) {
  if (configurablePin) {
    Knob::setPinMode();
  }
}

// Publicly callable Knob constructors
Knob::Knob(uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue, uint8_t channel, bool isEnabled)
  : Knob(potPin, CCNumber, minCCValue, maxCCValue, channel, isEnabled, true) {
}

Knob::Knob(uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue, uint8_t channel)
  : Knob(potPin, CCNumber, minCCValue, maxCCValue, channel, true) {
}

Knob::Knob(uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue)
  : Knob(potPin, CCNumber, minCCValue, maxCCValue, GLOBAL_MIDI_CHANNEL) {
}

Knob::Knob(uint8_t potPin, uint8_t CCNumber)
  : Knob(potPin, CCNumber, (uint8_t)0, (uint8_t)127) {
}

Knob::Knob(uint8_t potPin, uint8_t CCNumber, bool usesADS, uint8_t pinOnADS)
  : Knob(potPin, CCNumber) {
  _usesADS = usesADS;
  _pinOnADS = pinOnADS;
}

// Getters
uint8_t Knob::getCCNumber() const {
  return _CCNumber;
}

Knob::MinMax Knob::getMinMax() const {
  return { _minCCValue, _maxCCValue };
}

// Setters
void Knob::setPinMode() {
  // virtual method
  pinMode(_potPin, INPUT);
}

void Knob::setMIDIChannel(uint8_t channel) {
  _channel = channel;
}

void Knob::setAnalogMin(uint16_t minAnalogValue) {
  _minAnalogValue = constrain(minAnalogValue, 0, ADC_MAX);
}

void Knob::setAnalogMax(uint16_t maxAnalogValue) {
  _maxAnalogValue = constrain(maxAnalogValue, 0, ADC_MAX);
}

void Knob::setCCMin(uint8_t minCCValue) {
  _minCCValue = constrain(minCCValue, 0, 127);
}

void Knob::setCCMax(uint8_t maxCCValue) {
  _maxCCValue = constrain(maxCCValue, 0, 127);
}

// Methods
void Knob::enable() {
  _isEnabled = true;
}

void Knob::disable() {
  _isEnabled = false;
}

void Knob::readKnob() {
  if (_usesADS) {
    ADSManager.selectChannel(_pinOnADS);
    _potState = ADSManager.read();
  } else {
    _potState = analogRead(_potPin);
  }
}

void Knob::validateAnalogRead() {
  if (_potState == -1) {
    // -1 is returned from the ADS Manager if conversion not ready
    // if conversion not read, use last correct reading
    _potState = _potPState;
    return;
  }
  _potState = (_potState >= _minAnalogValue && _potState <= _maxAnalogValue) ? _potState : constrain(_potState, _minAnalogValue, _maxAnalogValue);
  // The ternary operator allows for conditional error logging OUT_OF_RANGE_ERROR should need be in future
  // else, the statement "_potState = constrain(_potState, _minAnalogValue, _maxAnalogValue)" would suffice
}

void Knob::update() {
  if (Knob::_isEnabled) {
    readKnob();
    validateAnalogRead();
    _midiState = map(_potState, _minAnalogValue, _maxAnalogValue, _minCCValue, _maxCCValue);
    _potIncrement = abs(_midiState - _midiPState);
    controlChange(_channel, _CCNumber, _midiState);

    // if (_potIncrement > _potThreshold) {
    //   snapshot = millis();
    // }

    // _potTimer = millis() - snapshot;

    // if (_potTimer < POT_TIMEOUT) {
    //   controlChange(_channel, _CCNumber, _midiState);
    //   _midiPState = _midiState;
    // }
  }
}
// ======================================================================================================
// ======================================================================================================


// ======================================================= KNOB ON MUX ======================================================
// ==========================================================================================================================
// Constructors
Knob_On_Mux::Knob_On_Mux(Mux& mux, uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue, uint8_t channel, bool isEnabled)
  : _mux(mux), Knob(potPin, CCNumber, minCCValue, maxCCValue, channel, isEnabled, false) {
}

Knob_On_Mux::Knob_On_Mux(Mux& mux, uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue, uint8_t channel)
  : Knob_On_Mux(mux, potPin, CCNumber, minCCValue, maxCCValue, channel, true) {
}

Knob_On_Mux::Knob_On_Mux(Mux& mux, uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue)
  : Knob_On_Mux(mux, potPin, CCNumber, minCCValue, maxCCValue, GLOBAL_MIDI_CHANNEL) {
}

Knob_On_Mux::Knob_On_Mux(Mux& mux, uint8_t potPin, uint8_t CCNumber)
  : Knob_On_Mux(mux, potPin, CCNumber, 0, 127) {
}
// Getters

// Setters
void Knob_On_Mux::setPinMode() {
  // virtual no-op method
}

// Methods
void Knob_On_Mux::readKnob() {
  _potState = _mux.readChannel(_potPin);
}

// ==========================================================================================================================
// ==========================================================================================================================
