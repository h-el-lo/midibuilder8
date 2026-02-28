#ifndef DAMPERPEDAL_H
#define DAMPERPEDAL_H

#include <Arduino.h>
#include "MIDIHelper.h"

uint8_t susPin;  // This pin is connected directly to the MCU board, not a multiplexer
uint8_t susState, susPrevState;
uint8_t damperDebounceTime;
unsigned long lastUpdatedTime = 0;

void setSustainPinMode() {
  pinMode(susPin, INPUT_PULLUP);
}

void checkForSustain() {
  if (millis() - lastUpdatedTime >= damperDebounceTime) {
    susState = map(digitalRead(susPin), 0, 1, 0, 127);

    if (susState != susPrevState) {
      controlChange(KEYS_CHANNEL, 64, susState);
      susPrevState = susState;
      lastUpdatedTime = millis();
    }
  }
}
#endif