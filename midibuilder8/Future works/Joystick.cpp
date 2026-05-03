#include "Joystick.h"
#include "MIDIHelper.h"

// Constructors
Joystick::Joystick(uint8_t xAxisPin, uint8_t yAxisPin, uint8_t yUpperCC, uint8_t yLowerCC)
  : _xAxisPin(xAxisPin), _yAxisPin(yAxisPin), _yUpperCC(yUpperCC), _yLowerCC(yLowerCC), axisCenter(512) {
  pinMode(_xAxisPin, INPUT);
  pinMode(_yAxisPin, INPUT);
  deadzoneRange = 5;
}

Joystick::Joystick(uint8_t xAxisPin, uint8_t yAxisPin)
  : Joystick(xAxisPin, yAxisPin, 1, 2) {  // Modulation Wheel CC01 [upper], Breath Controller CC02 [lower]
}

// Getters

// Setters
void Joystick::setDeadzoneRange() {
  // Pass
}

void Joystick::setYUpperCC() {
  // Pass
}

void Joystick::setYLowerCC() {
  // Pass
}

// Methods
void Joystick::readXAxis() {
  _xState = analogRead(_xAxisPin);
}

void Joystick::readYAxis() {
  _yState = analogRead(_yAxisPin);
}

void Joystick::updateXAxis() {
  readXAxis();
  _variation = abs(_xState - _xPrevState);

  if (_variation > _threshold) {
    _xLastUpdatedTime = millis();
  }

  axisTimeDifferential = millis() - _xLastUpdatedTime;
  _xState = constrain(_xState, 0, 1023);

  if (axisTimeDifferential < TIMEOUT) {
    if ((_xState <= axisCenter + deadzoneRange) || (_xState <= axisCenter - deadzoneRange)) {
      centerPitchWheel();  // Center the pitch wheel
    } else {
      pitchBend(KEYS_CHANNEL, map(_xState, 0, 1023, -8192, 8191));
    }
    _xPrevState = _xState;
  }
}

void Joystick::updateYAxis() {
  readYAxis();
  _variation = abs(_yState - _yPrevState);

  if (_variation > _threshold) {
    _yLastUpdatedTime = millis();
  }

  axisTimeDifferential = millis() - _yLastUpdatedTime;
  _yState = constrain(_yState, 0, 1023);

  if (axisTimeDifferential < TIMEOUT) {
    if ((_yState <= axisCenter + deadzoneRange) || (_yState <= axisCenter - deadzoneRange)) {
      // If Wheel reading of Y Axis is within deadzone range, set value of both yUpperCC and yLowerCC to 0
      controlChange(GLOBAL_MIDI_CHANNEL, _yLowerCC, 0);
      controlChange(GLOBAL_MIDI_CHANNEL, _yUpperCC, 0);

    } else if (_yState > axisCenter + deadzoneRange) {
      // Upper section of Y Axis
      controlChange(GLOBAL_MIDI_CHANNEL, _yUpperCC, map(_yState, 512, 1023, 0, 127));

    } else if (_yState < axisCenter + deadzoneRange) {
      // Lower section of Y Axis
      controlChange(GLOBAL_MIDI_CHANNEL, _yUpperCC, map(_yState, 512, 0, 0, 127));
    } else {
      // Log PitchWheel Error
      Serial.println("PITCH_WHEEL_ERROR: Y_POT reading out of recognizable range.");
    }
    _yPrevState = _yState;
  }
}

void Joystick::update() {
  updateXAxis();
  updateYAxis();
}
