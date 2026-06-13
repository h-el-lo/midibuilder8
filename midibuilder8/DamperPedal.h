#pragma once

#include "MIDIHelper.h"
#include "Multiplexer.h"

// A Struct is created in order to avoid the possibility of conflicting locals
struct Damper_Pedal {
  uint8_t susPin;  // This pin is connected to Mux1 (INPUT_PULLUP)
  uint8_t susState = 0;
  uint8_t susPrevState = 0;
  uint8_t damperDebounceTime = 100;  // 100ms
  unsigned long lastUpdatedTime = millis();

  void update() {
    if (millis() - lastUpdatedTime >= damperDebounceTime) {
      uint16_t storer = !Mux1.readChannel(susPin);
      susState = map(!Mux1.readChannel(susPin), 0, 1, 0, 127);

      if (susState != susPrevState) {
        controlChange(KEYS_CHANNEL, 64, susState);
        susPrevState = susState;
        lastUpdatedTime = millis();
      }
    }
  }
};

Damper_Pedal DamperPedal = { 15 };  // on Mux1
