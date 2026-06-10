#ifndef SCENE_SELECTOR_BUTTON_H
#define SCENE_SELECTOR_BUTTON_H

#include "Buttons.h"

#define GET_BIT(array, row, col) ((array[row] >> col) & 0x01)
#define SET_BIT(array, row, col) (array[row] |= (1 << col))
#define CLEAR_BIT(array, row, col) (array[row] &= ~(1 << col))
#define INVERT_PARTS_BIT(array, row, col) (array[row] ^= (1 << col))
#define SET_SCENE_BIT(array, row, index) (array[row] = (1 << col))

// ─────────────────────────────────────────────
//  Group of Eight Button
//  Scene / Preset selector with bank support
//  Sends MIDI CC, modifies RGB LED strip
// ─────────────────────────────────────────────
class SceneSelectorButton : public Button {
public:
  enum GroupMode {
    MODE_SCENE,  // CC64, momentary
    MODE_PARTS,  // CC0/127, latch
  };

  GroupMode operator!(GroupMode groupMode) {
    return (mode == MODE_PARTS) ? MODE_SCENE : MODE_PARTS;
  }

  enum Bank {
    BANK_A,
    BANK_B,
  };

private:
  inline static uint8_t _SCENE_CC[2] = { 0 };
  inline static uint8_t _PARTS_CC[2] = { 0 };
  uint8_t _index;
  uint8_t _rgbIndex;  // index into the LED strip

  // Shared group state — all SceneSelectorButtons point to the same two variables
  inline static GroupMode _groupMode = MODE_SCENE;
  inline static Bank _bank = BANK_A;


  // Rather than use an 8x2 uint8_t matrix, consuming 64 bytes per matrix, we shall employ bit packing.
  // Using an array of 8 bytes, 1 byte per item, every bit within every byte shall represent a data point
  // The macros GET_BIT, SET_BIT and CLEAR_BIT shall then be used to manipulate the arrays
  // This method consumes 8 bytes alone per 8x8 matrix

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
  static constexpr uint8_t partOnColor[2][3] = {
    { 200, 230, 160 },  // BANK_A
    { 200, 230, 160 },  // BANK_B
  };
  static constexpr uint8_t partOffColor[2][3] = {
    { 0, 0, 0 },  // BANK_A
    { 0, 0, 0 },  // BANK_B
  };
  static constexpr uint8_t sceneSelectedColor[2][3] = {
    { 0, 0, 0 },  // BANK_A
    { 0, 0, 0 },  // BANK_B
  };
  static constexpr uint8_t sceneUnselectedColor[2][3] = {
    { 0, 0, 0 },  // BANK_A
    { 0, 0, 0 },  // BANK_B
  };

  //   Shared scene and parts arrays.
  uint


    public :
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
  void setBank();

  // Methods
  void onPress() override;

  void updateRGB();
  void updateRGBSection();
};



// ─────────────────────────────────────────────
//  Scene Selector Handler
//  Owns nothing — just scans an array of Button*
// ─────────────────────────────────────────────
class ButtonManager {
private:
  SceneSelectorButton** _buttons;
  uint8_t _count;

public:
  ButtonManager(Button** buttons, uint8_t count);

  // Call this in loop()
  void scan();
};



#endif