#pragma once
#include <Arduino.h>

// Interrupt-driven quadrature encoder for ESP32-S3.
// Wired directly to the MCU (NOT on the button mux) -- default pins 2 (A) / 1 (B).
// Pin A drives the interrupt; both A and B support interrupts on ESP32-S3, but only
// A needs one since we read B in software on every A edge.
//
// getDelta() returns signed "detents" moved since it was last called
// (most mechanical encoders emit 4 electrical transitions per detent/click).
//
// NOTE: single static instance pointer -> supports ONE encoder at a time,
// which matches a single-encoder menu system.
class Encoder {
public:
    Encoder(uint8_t pinA, uint8_t pinB) : _pinA(pinA), _pinB(pinB) {}

    void begin() {
        pinMode(_pinA, INPUT_PULLUP);
        pinMode(_pinB, INPUT_PULLUP);
        _instance = this;
        _lastState = readState();
        attachInterrupt(digitalPinToInterrupt(_pinA), isrTrampoline, CHANGE);
    }

    // Returns accumulated detents since the last call, then resets to 0.
    // Positive = clockwise, negative = counter-clockwise.
    int16_t getDelta() {
        int16_t d;
        portENTER_CRITICAL(&_mux);
        d = _delta;
        _delta = 0;
        portEXIT_CRITICAL(&_mux);
        return d;
    }

private:
    static void IRAM_ATTR isrTrampoline() {
        if (_instance) _instance->handleInterrupt();
    }

    uint8_t readState() {
        return (uint8_t)((digitalRead(_pinA) << 1) | digitalRead(_pinB));
    }

    // Runs in interrupt context -- keep it fast, no Serial, no allocation.
    void IRAM_ATTR handleInterrupt() {
        uint8_t state = readState();
        // Standard quadrature transition table -> +1/-1/0 per edge.
        static const int8_t table[16] = {
             0, -1,  1,  0,
             1,  0,  0, -1,
            -1,  0,  0,  1,
             0,  1, -1,  0
        };
        uint8_t idx = (uint8_t)((_lastState << 2) | state);
        int8_t movement = table[idx & 0x0F];

        portENTER_CRITICAL_ISR(&_mux);
        _accum += movement;
        if (_accum >= 4)       { _delta++; _accum = 0; }
        else if (_accum <= -4) { _delta--; _accum = 0; }
        portEXIT_CRITICAL_ISR(&_mux);

        _lastState = state;
    }

    uint8_t _pinA, _pinB;
    volatile uint8_t  _lastState = 0;
    volatile int8_t   _accum = 0;
    volatile int16_t  _delta = 0;

    static Encoder* _instance;
    static portMUX_TYPE _mux;
};
