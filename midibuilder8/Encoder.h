#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

class Encoder {
private:
  volatile int16_t _delta = 0;
  volatile long _encoderPos;
  volatile long _lastEncoded = 0;
  volatile int16_t _encoderVal = 0;
  volatile int16_t _prevEncoderVal = 0;

  static Encoder* instance;
  // Rotary encoder pins for steering
  uint8_t _PIN_A;
  uint8_t _PIN_B;

  // int _prevEncoderPos;  // Previous values for change detection

public:
  // Constructor
  Encoder(uint8_t ENCODER_PIN_A, uint8_t ENCODER_PIN_B);
  Encoder();

  // Getters

  // Setters

  // Methods
  void initializeEncoder();
  void updateEncoder();
  static void IRAM_ATTR updateEncoderISR();
  int8_t consumeDelta();

  void updateScreenValues();
};

extern Encoder encoder;

#endif