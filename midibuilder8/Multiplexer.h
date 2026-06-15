#ifndef MULTIPLEXER_H
#define MULTIPLEXER_H

#include <Arduino.h>
#include "ADSManager.h"

#define ANALOG 0X1
#define DIGITAL 0X2

class Mux {
private:
  uint8_t _S0;  // Connected to MCU
  uint8_t _S1;  // Connected to MCU
  uint8_t _S2;  // Connected to MCU
  uint8_t _S3;  // Connected to MCU
  uint8_t _signalPin;
  bool _usesADS = false;
  uint8_t _type;
  uint8_t _mode;
  uint8_t _selectedChannel = 255;

public:
  // Constructors
  Mux(bool usesADS, uint8_t S0, uint8_t S1, uint8_t S2, uint8_t S3, /* Pin on ADS */ uint8_t signalPin, uint8_t mode, uint8_t type);  // INPUT, INPUT_PULLUP and OUTPUT
  Mux(uint8_t S0, uint8_t S1, uint8_t S2, uint8_t S3, uint8_t signalPin, uint8_t mode, uint8_t type);                                 // INPUT, INPUT_PULLUP and OUTPUT

  // Getters
  uint8_t getType() const;
  uint8_t getMode() const;

  // Setters
  void setType(uint8_t type);
  void setMode(uint8_t mode);

  // Methods
  bool usesADS();
  void validate();
  void selectChannel(uint8_t channel);
  uint16_t read();
  uint16_t readChannel(uint8_t channel);
  void write(uint8_t state);
  void writeToChannel(uint8_t channel, uint8_t state);
};

extern Mux Mux1;  // Mux 1 (Digital INPUT_PULLUP (keys[COLS] [0-7]) (buttons[read] [8-15])) digital
extern Mux Mux2;  // Mux 2 (Outputs (keys), KPS AND KPE (rows))
extern Mux Mux3;  // Mux 3 (Digital output of buttons)
extern Mux Mux4;  // Mux 4 (Analog Input, Knobs and Faders)

#endif
