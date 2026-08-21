#include "Knob.h"
#include "MIDIHelper.h"

// ================================== REGULAR KNOB CLASS ================================================
// ======================================================================================================
// Constructors

// Private Knob constructor
Knob::Knob(bool usesADS, uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue, uint8_t channel, bool isEnabled, bool configurablePin)
  : _usesADS(usesADS), _potPin(potPin), _CCNumber(CCNumber), _minCCValue(minCCValue), _maxCCValue(maxCCValue), _channel(channel), _isEnabled(isEnabled) {
  if (!_usesADS) {
    if (configurablePin) {
      Knob::setPinMode();
    }
  }
}

// Protected Knob constructor
Knob::Knob(uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue, uint8_t channel, bool isEnabled, bool configurablePin)
  : Knob(false, potPin, CCNumber, minCCValue, maxCCValue, channel, isEnabled, configurablePin) {
}

// Publicly callable Knob constructors

// Use constructors beginning with "usesADS" only when you intend on using
// the knob objects with ADC as there is no fallback of setPinMode
Knob::Knob(bool usesADS, uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue, uint8_t channel, bool isEnabled)
  : Knob(usesADS, potPin, CCNumber, minCCValue, maxCCValue, channel, isEnabled, false) {
}

Knob::Knob(bool usesADS, uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue, uint8_t channel)
  : Knob(usesADS, potPin, CCNumber, minCCValue, maxCCValue, channel, true) {
}

Knob::Knob(bool usesADS, uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue)
  : Knob(usesADS, potPin, CCNumber, minCCValue, maxCCValue, GLOBAL_MIDI_CHANNEL) {
}

Knob::Knob(bool usesADS, uint8_t potPin, uint8_t CCNumber)
  : Knob(usesADS, potPin, CCNumber, (uint8_t)0, (uint8_t)127) {
}
Knob::Knob(bool usesADS, uint8_t potPin)
  : Knob(usesADS, potPin, 255) {
}




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

Knob::Knob(uint8_t potPin)
  : Knob(potPin, 255) {
}


// Getters
uint8_t Knob::getCCNumber() const {
  return _CCNumber;
}

Knob::MinMax Knob::getMinMax() const {
  return { _minCCValue, _maxCCValue };
}

uint8_t Knob::getMIDIChannel() const {
  return _channel;
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
  _minAnalogValue = constrain(minAnalogValue, 0, 4096);  // 12 bits read, declared in setup()
}

void Knob::setAnalogMax(uint16_t maxAnalogValue) {
  _maxAnalogValue = constrain(maxAnalogValue, 0, 4096);  // 12 bits read, declared in setup()
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
    ADSManager.selectChannel(_potPin);
    _potState = ADSManager.read();
  } else {
    _potState = analogRead(_potPin);
  }
}

void Knob::validateAnalogRead() {
  if (_usesADS) {
    if (_potState == -255) {
      // -255 is returned from the ADS Manager if conversion not ready
      // if conversion not read, use last correct reading
      _potState = _potPState;
      return;

    } else {
      _potState = constrain(_potState, 0, ADS_RAW_MAX);     // smooth out other non -1 negative readings
      _potState = map(_potState, 0, ADS_RAW_MAX, 0, 4096);  // Map 16bits _potstate to 12 bits read
    }
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

    if (_potIncrement > _potThreshold) {
      snapshot = millis();
    }

    _potTimer = millis() - snapshot;

    if (_potTimer < POT_TIMEOUT) {
      controlChange(getMIDIChannel(), _CCNumber, _midiState);
      _potPState = _potState;
      _midiPState = _midiState;
    }
  }
}
// ======================================================================================================
// ======================================================================================================


// ======================================================= KNOB ON MUX ======================================================
// ==========================================================================================================================
// Constructors
Knob_On_Mux::Knob_On_Mux(Mux& mux, uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue, uint8_t channel, bool isEnabled)
  : Knob(potPin, CCNumber, minCCValue, maxCCValue, channel, isEnabled, false), _mux(mux) {
  _usesADS = _mux.usesADS();
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


// ========================================================== FADER =========================================================
// ==========================================================================================================================
// Constructors
Fader::Fader(Mux& mux, uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue, uint8_t channel, bool isEnabled)
  : Knob_On_Mux(mux, potPin, CCNumber, minCCValue, maxCCValue, channel, isEnabled) {}

Fader::Fader(Mux& mux, uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue, uint8_t channel)
  : Fader(mux, potPin, CCNumber, minCCValue, maxCCValue, channel, true) {
}

Fader::Fader(Mux& mux, uint8_t potPin, uint8_t CCNumber, uint8_t minCCValue, uint8_t maxCCValue)
  : Fader(mux, potPin, CCNumber, minCCValue, maxCCValue, GLOBAL_MIDI_CHANNEL) {
}

Fader::Fader(Mux& mux, uint8_t potPin, uint8_t CCNumber, uint8_t channel)
  : Fader(mux, potPin, CCNumber, 0, 127, channel) {
}

Fader::Fader(Mux& mux, uint8_t potPin, uint8_t CCNumber)
  : Fader(mux, potPin, CCNumber, 0, 127) {
}
// Getters
uint8_t Fader::getMIDIChannel() const {
  return _channel;
}

// Setters
void Fader::toggleBank() {
  _bank = _bank == BANK_A ? BANK_B : BANK_A;
  _channel = _bank == BANK_A ? GLOBAL_MIDI_CHANNEL : 5;
}

// Methods

// ==========================================================================================================================
// ==========================================================================================================================
