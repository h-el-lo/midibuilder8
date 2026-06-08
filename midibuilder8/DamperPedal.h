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
};

Damper_Pedal damperPedal = { 15 }; // on Mux1

void updateSustainPedal() {
  if (millis() - damperPedal.lastUpdatedTime >= damperPedal.damperDebounceTime) {
    uint16_t storer = !Mux1.readChannel(damperPedal.susPin);
    damperPedal.susState = map(!Mux1.readChannel(damperPedal.susPin), 0, 1, 0, 127);

    if (damperPedal.susState != damperPedal.susPrevState) {
      controlChange(KEYS_CHANNEL, 64, damperPedal.susState);
      damperPedal.susPrevState = damperPedal.susState;
      damperPedal.lastUpdatedTime = millis();
    }
  }
}
