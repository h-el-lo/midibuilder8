#pragma once
#include <Arduino.h>
#include "driver/pulse_cnt.h"

// Quadrature encoder decoded entirely in hardware via the ESP32/ESP32-S3
// PCNT (Pulse Counter) peripheral. Unlike attachInterrupt()-based decoding,
// no GPIO interrupt ever fires for encoder motion -- the PCNT unit counts
// and validates quadrature transitions in silicon, so fast/bouncy encoder
// activity can never delay or preempt other interrupt-driven peripherals
// (e.g. the I2C driver's completion interrupt), which is what was
// corrupting the LCD.
//
// getDelta() returns signed "detents" moved since it was last called
// (4 raw quadrature counts = 1 detent, matching the mechanical encoder's
// 4 electrical transitions per click).
//
// Requires the ESP-IDF v5.x pulse_cnt driver, which is what arduino-esp32
// core 3.x bundles. If you're on arduino-esp32 core 2.x, this header won't
// compile -- that core ships the older driver/pcnt.h API instead, which
// uses a different (non handle-based) function set. Say the word if you
// need the legacy-driver version instead.
class Encoder {
public:
    Encoder(uint8_t pinA, uint8_t pinB) : _pinA(pinA), _pinB(pinB) {}

    void begin() {
        pinMode(_pinA, INPUT_PULLUP);
        pinMode(_pinB, INPUT_PULLUP);

        pcnt_unit_config_t unit_config = {};
        unit_config.high_limit = HIGH_LIMIT;
        unit_config.low_limit  = LOW_LIMIT;
        pcnt_new_unit(&unit_config, &_unit);

        // Reject pulses shorter than 1us (contact bounce) in hardware --
        // this replaces the software debounce/bounce-flood problem that
        // was flooding the old ISR at high turn speed.
        pcnt_glitch_filter_config_t filter_config = {};
        filter_config.max_glitch_ns = 1000;
        pcnt_unit_set_glitch_filter(_unit, &filter_config);

        pcnt_chan_config_t chan_a_config = {};
        chan_a_config.edge_gpio_num  = _pinA;
        chan_a_config.level_gpio_num = _pinB;
        pcnt_channel_handle_t chan_a = nullptr;
        pcnt_new_channel(_unit, &chan_a_config, &chan_a);

        pcnt_chan_config_t chan_b_config = {};
        chan_b_config.edge_gpio_num  = _pinB;
        chan_b_config.level_gpio_num = _pinA;
        pcnt_channel_handle_t chan_b = nullptr;
        pcnt_new_channel(_unit, &chan_b_config, &chan_b);

        // Standard hardware quadrature decode: each channel counts up on
        // one edge direction and down on the other, gated by the other
        // pin's current level -- the hardware equivalent of the old
        // software transition table.
        pcnt_channel_set_edge_action(chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE);
        pcnt_channel_set_level_action(chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE);
        pcnt_channel_set_edge_action(chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE);
        pcnt_channel_set_level_action(chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE);

        pcnt_unit_add_watch_point(_unit, HIGH_LIMIT);
        pcnt_unit_add_watch_point(_unit, LOW_LIMIT);

        pcnt_unit_enable(_unit);
        pcnt_unit_clear_count(_unit);
        pcnt_unit_start(_unit);
    }

    // Returns accumulated detents since the last call, then resets to 0.
    // Positive = clockwise, negative = counter-clockwise. If direction
    // comes out backwards for your wiring, swap the two
    // pcnt_channel_set_edge_action() calls above rather than negating
    // here.
    int16_t getDelta() {
        int rawCount = 0;
        pcnt_unit_get_count(_unit, &rawCount);

        int diff = rawCount - _lastRawCount;
        _lastRawCount = rawCount;

        _accum += diff;
        int16_t detents = (int16_t)(_accum / 4);
        _accum -= detents * 4;
        return detents;
    }

private:
    // Generous range so overflow in practice never happens for a menu
    // encoder polled regularly; PCNT wraps at these limits.
    static constexpr int HIGH_LIMIT = 30000;
    static constexpr int LOW_LIMIT  = -30000;

    uint8_t _pinA, _pinB;
    pcnt_unit_handle_t _unit = nullptr;
    int _lastRawCount = 0;
    int _accum = 0;
};