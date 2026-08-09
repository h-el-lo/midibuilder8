#pragma once
#include <Arduino.h>

class Encoder {

private:
  uint8_t _pinA, _pinB;
  volatile uint8_t _lastState = 0;
  volatile int8_t _accum = 0;
  volatile int16_t _delta = 0;

  static Encoder* _instance;
  static portMUX_TYPE _mux;

  uint8_t readState();
  static void IRAM_ATTR isrTrampoline();  // Declaration only -- defined out-of-line in Encoder.cpp.
  void IRAM_ATTR handleInterrupt();

public:
  Encoder(uint8_t pinA, uint8_t pinB);
  void begin();
  int16_t getDelta();
};