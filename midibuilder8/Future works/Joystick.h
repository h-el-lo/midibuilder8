#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <Arduino.h>

class Joystick {
private:
  // X variables
  uint8_t _xAxisPin;
  uint16_t _xState;
  uint16_t _xPrevState;
  unsigned long _xLastUpdatedTime;

  // Y variables
  uint8_t _yAxisPin;
  uint8_t _yUpperCC;
  uint8_t _yLowerCC;
  uint16_t _yState;
  uint16_t _yPrevState;
  unsigned long _yLastUpdatedTime;

  // Shared variables
  uint16_t axisTimeDifferential;
  uint16_t axisCenter; // Theoretical center of axes
  uint8_t _threshold;
  uint16_t deadzoneRange;
  uint16_t _variation;
  uint8_t TIMEOUT;


public:
  // Constructors
  Joystick(uint8_t xAxisPin, uint8_t yAxisPin, uint8_t yUpperCC, uint8_t yLowerCC);
  Joystick(uint8_t xAxisPin, uint8_t yAxisPin);

  // Getters

  // Setters
  void setDeadzoneRange();
  void setYUpperCC();
  void setYLowerCC();

  // Methods
  void readXAxis();
  void readYAxis();
  void updateXAxis();
  void updateYAxis();
  void update();
};


#endif