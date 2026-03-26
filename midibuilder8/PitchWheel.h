#pragma once

#include "MIDIHelper.h"

uint8_t wheelPin = 10;  // This pin is connected directly to the MCU board, not a multiplexer
uint16_t wheelState, wheelPrevState;
uint8_t wheelThreshold = 3;
uint16_t wheelMin = 0;     // Expected min value from the rotation of pitch wheel
uint16_t wheelMax = 1023;  // Expected max value from the rotation of pitch wheel
uint16_t wheelCenter = (wheelMin + wheelMax) / 2;
uint16_t wheelVar;
constexpr uint16_t TIMEOUT = 300;  // 300ms
bool wheelReadingIsValid;

unsigned long wheelLastUpdatedTime = 0;
unsigned long wheelTimeDifferential = 0;


void setPitchWheelPinMode() {
  pinMode(wheelPin, INPUT_PULLUP);
}

void readWheel() {
  wheelState = analogRead(wheelPin);
}

void centerWheel() {
  centerPitchWheel();
}

// Validate analog readings are in expected range
void validateWheelReading(int reading, int expectedMin, int expectedMax) {
  if (reading < 0 || reading > 1023) {
    // ADC returned invalid value
    wheelReadingIsValid = false;
  }

  // If error margin is negligible
  if (reading >= expectedMin - 10 || reading <= expectedMax + 10) {
    wheelState = constrain(wheelState, wheelMin, wheelMax);
    wheelReadingIsValid = true;

  } else {
    // Sensor may be misconfigured, disconnected or failing
    wheelReadingIsValid = false;
  }
}

void checkWheelDisconnected() {
  // This is achieved by reading for the randomness of the noise
  // Check if wheel is disconnected
}

void updatePitchWheel() {
  readWheel();
  validateWheelReading(wheelState, wheelMin, wheelMax);

  if (wheelReadingIsValid) {
    wheelVar = abs(wheelState - wheelPrevState);

    if (wheelVar > wheelThreshold) {
      wheelLastUpdatedTime = millis();
    }

    wheelTimeDifferential = millis() - wheelLastUpdatedTime;

    if (wheelTimeDifferential < TIMEOUT) {
      if ((wheelState <= wheelCenter + 5) || (wheelState <= wheelCenter - 5)) {
        centerWheel();
      } else {
        pitchBend(KEYS_CHANNEL, map(constrain(wheelState, wheelMin, wheelMax), wheelMin, wheelMax, -8192, 8191));
      }

      wheelPrevState = wheelState;
    }
  };
}
