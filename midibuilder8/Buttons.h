#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>
#include "Multiplexer.h"
#include "MIDIHelper.h"
#include "keys.h"
#include "rgb.h"

// ─────────────────────────────────────────────
//  Forward declarations
// ─────────────────────────────────────────────
class ButtonManager;





enum ButtonType {
  XY_BUTTON,  // anode: Mux3, cathode: MCU pin
  XZ_BUTTON,  // anode: Mux3, cathode: Mux1
  YZ_BUTTON   // anode: MCU pin, cathode: Mux1
};


// ─────────────────────────────────────────────
//  Abstract Base Class
// ─────────────────────────────────────────────
class Button {
protected:
  ButtonType _type;
  uint8_t _anodePin;
  uint8_t _cathodePin;
  bool _state = false;
  bool _pState = false;
  unsigned long _lastUpdated = 0;

  static const uint8_t DEBOUNCE_MS = 20;

  // Hardware read — shared by all button types
  bool readHardware();

public:
  // Constructors
  Button(ButtonType type, uint8_t anodePin, uint8_t cathodePin);
  virtual ~Button() {}

  // Getters
  bool getState() const {
    return _state;
  }

  // Setters

  // Methods
  // Scans hardware, debounces, calls onPress/onRelease on change
  void read();

  // ── Derived classes implement these ──
  virtual void onPress() = 0;
};


class CCButton : public Button {
protected:
  _CCNumber;

public:
// Constructors
  CCButton(ButtonType type, uint8_t anodePin, uint8_t cathodePin, uint8_t CCNumber);

  // Getters

  // Setters

  // Methods
  void onPress() override;
};


// ─────────────────────────────────────────────
//  Numpad Button  (0–9, +, -)
//  Sends MIDI CC, no RGB
// ─────────────────────────────────────────────
class NumpadButton : public CCButton {
private:
  uint8_t _ccNumber;

public:
  NumpadButton(uint8_t anodePin,
               uint8_t cathodePin,
               uint8_t ccNumber);
};


// ─────────────────────────────────────────────
//  Transpose Button
//  Modifies global transpose in Keys
// ─────────────────────────────────────────────
class TransposeButton : public Button {
public:
  enum Direction { UP,
                   DOWN };

private:
  Keys* _keys;
  Direction _direction;

public:
  TransposeButton(ButtonType type,
                  uint8_t anodePin,
                  uint8_t cathodePin,
                  Direction direction,
                  Keys* keys);

  void onPress() override;
  void onRelease() override;
};


// ─────────────────────────────────────────────
//  Octave Button
//  Modifies global octave in Keys
// ─────────────────────────────────────────────
class OctaveButton : public Button {
public:
  enum Direction { UP,
                   DOWN };

private:
  Keys* _keys;
  Direction _direction;

public:
  OctaveButton(ButtonType type,
               uint8_t anodePin,
               uint8_t cathodePin,
               Direction direction,
               Keys* keys);

  void onPress() override;
  void onRelease() override;
};


// ─────────────────────────────────────────────
//  Action Button
//  Home, Settings, Exit etc. — no MIDI, no RGB
//  Accepts a plain function pointer as its action
// ─────────────────────────────────────────────
class ActionButton : public Button {
private:
  void (*_onPressCallback)();    // injected at construction
  void (*_onReleaseCallback)();  // optional, can be nullptr

public:
  ActionButton(ButtonType type,
               uint8_t anodePin,
               uint8_t cathodePin,
               void (*onPressCallback)(),
               void (*onReleaseCallback)() = nullptr);

  void onPress() override;
  void onRelease() override;
};


// ─────────────────────────────────────────────
//  Button Manager
//  Owns nothing — just scans an array of Button*
// ─────────────────────────────────────────────
class ButtonManager {
private:
  Button** _buttons;
  uint8_t _count;

public:
  ButtonManager(Button** buttons, uint8_t count);

  // Call this in loop()
  void scan();
};


// ─────────────────────────────────────────────
//  Composition root — call once in setup()
// ─────────────────────────────────────────────
void initButtons(MIDIHelper* midi, RGB* rgb, Keys* keys);


#endif