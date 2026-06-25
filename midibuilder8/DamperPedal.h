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
      susState = map(!Mux3.readChannel(susPin), 0, 1, 0, 127);

      if (susState != susPrevState) {
        controlChange(GLOBAL_MIDI_CHANNEL, 64, susState);
        susPrevState = susState;
        lastUpdatedTime = millis();
      }
    }
  }
};

Damper_Pedal DamperPedal = { 13 };  // on Mux3
