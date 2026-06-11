#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>
#include "Multiplexer.h"
#include "MIDIHelper.h"
#include "keys.h"
#include "RGB.h"


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

  // Methods
  // Scans hardware, debounces, calls onPress/onRelease on change
  void read();

  // ── Derived classes implement these ──
  virtual void onPress() = 0;
};


// ─────────────────────────────────────────────
//  Numpad Button  (0–9, +, -)
//  Sends MIDI CC, no RGB
// ─────────────────────────────────────────────
class NumpadButton : public Button {
private:
  uint8_t _CCNumber;

public:
  // Constructors
  NumpadButton(uint8_t anodePin, uint8_t cathodePin, uint8_t CCNumber);

  // Methods
  void onPress() override;
};


#define GET_BIT(BYTE, BIT) ((BYTE >> BIT) & 0x01)
#define CLEAR_BYTE(BYTE) (BYTE &= 0X00)
#define INVERT_BIT(BYTE, BIT) (BYTE ^= (1 << BIT))
#define SET_SCENE_BIT(BYTE, BIT) (BYTE = (1 << BIT))

class SceneSelectorButton : public Button {
public:
  enum GroupMode {
    MODE_SCENE,  // CC64, momentary
    MODE_PARTS,  // CC0/127, latch
  };

  enum Bank {
    BANK_A,
    BANK_B,
  };

private:
  uint8_t _index;
  uint8_t _rgbIndex;  // index into the LED strip

  // the correct rgb index for entire strip must correctly be derived before anything
  // There are 8 scene selector buttons on the midi keyboard.
  // _rgbindex of button calling the method - its _index [indexing starts at 0]
  static uint8_t _rgbStartIndex;

  // Shared group state — all SceneSelectorButtons point to the same two variables
  inline static GroupMode _groupMode = MODE_SCENE;
  inline static Bank _bank = BANK_A;

  inline static uint8_t _SCENE_CC[2] = { 0 };
  inline static uint8_t _PARTS_CC[2] = { 0 };


  // Rather than use an 8x2 uint8_t matrix, consuming 16 bytes, we shall employ bit packing.
  // Using an array of 8 bytes, 1 byte per item, every bit within every byte shall represent a data point
  // The macros GET_BIT, SET_SCENE_BIT, CLEAR_BYTE and INVERT_BIT shall then be used to manipulate the byte arrays
  // This method consumes 8 bytes alone per 8x2 matrix

  // Arrays to keep track of present states of kps and kpe data for all keys
  // Shared scene and parts arrays.
  inline static byte sceneState[2]{
    0b10000000,
    0b00000000,
  };

  inline static byte partsState[2]{
    0b00000000,
    0b00000000,
  };

  // Shared color variables
  inline static ColorStruct partsOnColor[2] = {
    { 200, 230, 160 },  // BANK_A
    { 200, 230, 160 },  // BANK_B
  };
  inline static ColorStruct partsOffColor[2] = {
    { 0, 0, 0 },  // BANK_A
    { 0, 0, 0 },  // BANK_B
  };
  inline static ColorStruct sceneSelectedColor[2] = {
    { 0, 0, 0 },  // BANK_A
    { 0, 0, 0 },  // BANK_B
  };
  inline static ColorStruct sceneUnselectedColor[2] = {
    { 0, 0, 0 },  // BANK_A
    { 0, 0, 0 },  // BANK_B
  };


public:
  // Constructors
  SceneSelectorButton(
    uint8_t anodePin,
    uint8_t cathodePin,
    uint8_t index,
    uint8_t BANK_A_SCENE_CC,
    uint8_t BANK_B_SCENE_CC,
    uint8_t BANK_A_PARTS_CC,
    uint8_t BANK_B_PARTS_CC,
    uint8_t rgbIndex);

  // Getters

  // Setters
  static void toggleGroupMode();
  static void setBankTo_A();
  static void setBankTo_B();

  // Methods
  void onPress() override;
  static void clearallparts();

  void updateRGB(uint8_t rgbIndex);
  static void updateRGBSection();
};


class GeneralPurposeCCButton : public Button {
private:
  uint8_t _CCNumber;
  uint8_t _rgbIndex;
  ColorStruct _color;

public:
  GeneralPurposeCCButton(uint8_t anodePin, uint8_t cathodePin, uint8_t CCNumber, uint8_t rgbIndex, ColorStruct color);
  void onPress() override;
  void updateRGB();
};


class PresetSelectorButton {
private:
public:
};


// ─────────────────────────────────────────────
//  Action Button
//  Home, Settings, Exit etc. — no MIDI, no RGB
//  Accepts a plain function pointer as its action
// ─────────────────────────────────────────────
class ActionButton : public Button {
protected:
  void (*_onPressCallback)();  // injected at construction

public:
  ActionButton(ButtonType type,
               uint8_t anodePin,
               uint8_t cathodePin,
               void (*onPressCallback)());

  void onPress() override;
};


class RGBActionButton : public ActionButton {
private:
  uint8_t _rgbIndex;
  ColorStruct _color;

public:
  RGBActionButton(ButtonType type,
                  uint8_t anodePin,
                  uint8_t cathodePin,
                  uint8_t rgbIndex,
                  ColorStruct color,
                  void (*onPressCallback)());
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
void initButtons();
// Call in loop()
void scanButtons();


#endif