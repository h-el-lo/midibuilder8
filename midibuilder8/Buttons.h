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
  bool _momentary = true;  // true = momentary, false = latch
  unsigned long _lastUpdated = 0;

  static const uint8_t DEBOUNCE_MS = 20;

  // Hardware read — shared by all button types
  bool readHardware();

public:
  Button(ButtonType type, uint8_t anodePin, uint8_t cathodePin, bool momentary = true);
  virtual ~Button() {}

  // Scans hardware, debounces, calls onPress/onRelease on change
  void read();

  bool getState() const {
    return _state;
  }
  bool getMomentary() const {
    return _momentary;
  }

  // ── Derived classes implement these ──
  virtual void onPress() = 0;
  virtual void onRelease() = 0;
};


// ─────────────────────────────────────────────
//  Group of Eight Button
//  Scene / Preset selector with bank support
//  Sends MIDI CC, modifies RGB LED strip
// ─────────────────────────────────────────────
class SceneSelectorButton : public Button {
public:
  enum GroupMode {
    MODE_SCENE,  // CC64, momentary
    MODE_PARTS  // CC0/127, latch
  };

  enum Bank {
    BANK_A,
    BANK_B
  };

private:
  uint8_t _index;     // 0–7 position within the group
  uint8_t _rgbIndex;  // index into the LED strip
  uint8_t _ccNumber;  // assigned CC number

  // Shared group state — all GroupButtons point to the same two variables
  GroupMode* _groupMode;
  Bank* _bank;

public:
  CCButton(ButtonType type,
           uint8_t anodePin,
           uint8_t cathodePin,
           uint8_t index,
           uint8_t rgbIndex,
           uint8_t ccNumber,
           GroupMode* groupMode,
           Bank* bank,
           MIDIHelper* midi,
           RGB* rgb);

  void onPress() override;
  void onRelease() override;

  void updateLED();
};


// ─────────────────────────────────────────────
//  Numpad Button  (0–9, +, -)
//  Sends MIDI CC, no RGB
// ─────────────────────────────────────────────
class NumpadButton : public Button {
private:
  MIDIHelper* _midi;
  uint8_t _ccNumber;
  uint8_t _ccValue;

public:
  NumpadButton(ButtonType type,
               uint8_t anodePin,
               uint8_t cathodePin,
               uint8_t ccNumber,
               uint8_t ccValue,
               MIDIHelper* midi);

  void onPress() override;
  void onRelease() override;
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