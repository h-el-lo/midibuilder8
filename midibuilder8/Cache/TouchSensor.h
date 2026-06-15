#ifndef TOUCHSENSOR_H
#define TOUCHSENSOR_H

#include <Arduino.h>

// Maybe later, update the touch sensor class to use toggle and momentary perhaps using two pins each on the MCU
class TouchSensor {
private:
  uint8_t _pinNumber;  // This pin is connected directly to the MCU board, not a multiplexer
  uint8_t _CCNumber;
  uint8_t _state;
  uint8_t _pState;
  uint8_t _MIDIState;
  uint8_t _MIDIChannel;
  bool _inverted;
  inline static uint8_t _debounceTime = 100;  // 100ms
  unsigned long _lastUpdatedTime;

public:
  // Constructors
  TouchSensor(uint8_t pinNumber, uint8_t CCNumber, bool isInverted);
  TouchSensor(uint8_t pinNumber, uint8_t CCNumber);

  // Getters

  // Setters
  void setMIDIChannel(uint8_t channel);

  // Methods
  void init();
  void invert();
  void updateMIDI();
  void update();
};


#endif