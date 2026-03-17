#pragma once

#include "Knob.h"

// A Struct is created in order to avoid conflicting locals
struct Expression_Pedal {
  Knob knob;
  const uint8_t CONNECTION_CHECKER_PIN;
  bool isConnected;
};

Expression_Pedal Pedal = { Knob(2, 16), 42, false };

void INITIALIZE_EXPRESSION_PEDAL() {
  Pedal.knob.disable();
  pinMode(Pedal.CONNECTION_CHECKER_PIN, INPUT_PULLUP);
  Pedal.isConnected = !digitalRead(Pedal.CONNECTION_CHECKER_PIN);  // !digitalRead is used because of INPUT_PULLUP

  if (Pedal.isConnected) {
    Pedal.knob.enable();
  }
}

void check_pedal_connection() {
  if (!digitalRead(Pedal.CONNECTION_CHECKER_PIN) != Pedal.isConnected) {
    if (digitalRead(Pedal.CONNECTION_CHECKER_PIN)) {
      Pedal.knob.enable();
    } else {
      Pedal.knob.disable();
    }
  }
}

void updateExpressionPedal() {
  check_pedal_connection();
  Pedal.knob.update();
}