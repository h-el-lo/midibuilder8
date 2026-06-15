#pragma once

#include "Knob.h"

// A Struct is created in order to avoid conflicting locals
struct Expression_Pedal {
  Knob knob;
  const uint8_t CONNECTION_CHECKER_PIN;
  bool isConnected;


  void init() {
    knob.disable();
    pinMode(CONNECTION_CHECKER_PIN, INPUT_PULLUP);
    isConnected = !digitalRead(CONNECTION_CHECKER_PIN);  // !digitalRead is used because of INPUT_PULLUP

    if (isConnected) {
      knob.enable();
    }
  }

  void check_pedal_connection() {
    if (!digitalRead(CONNECTION_CHECKER_PIN) != isConnected) {
      if (digitalRead(CONNECTION_CHECKER_PIN)) {
        knob.enable();
      } else {
        knob.disable();
      }
    }
  }

  void update() {
    check_pedal_connection();
    knob.update();
  }
};

Expression_Pedal ExpressionPedal = { Knob(/*usesADS*/ true, /*pinOnADS*/ ADS_EXPR_CHANNEL, 11, ), 42, false };
