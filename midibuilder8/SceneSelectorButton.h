#ifndef SCENE_SELECTOR_BUTTON_H
#define SCENE_SELECTOR_BUTTON_H

#include "Buttons.h"


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
  uint8_t _BANK_A_SCENE_CC;
  uint8_t _BANK_B_SCENE_CC;
  uint8_t _BANK_A_PARTS_CC;
  uint8_t _BANK_B_PARTS_CC;
  uint8_t _rgbIndex;  // index into the LED strip
  bool _scene1Bank1, _scene1Bank2, _part1Bank1;

  // Shared group state — all SceneSelectorButtons point to the same two variables
  static GroupMode _groupMode;
  static Bank _bank;

  // Shared color variables
  static uint8_t partOnColor[2][3] = {
    { 200, 230, 160 },  // BANK_A
    { 200, 230, 160 },  // BANK_B
  };
  static uint8_t partOffColor[2][3] = {
    { 0, 0, 0 },  // BANK_A
    { 0, 0, 0 },  // BANK_B
  };
  static uint8_t sceneSelectedColor[2][3] = {
    { 0, 0, 0 },  // BANK_A
    { 0, 0, 0 },  // BANK_B
  };
  static uint8_t sceneUnselectedColor[2][3] = {
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
      uint8_t BANK_A_SCENE_CC,
      uint8_t BANK_B_SCENE_CC,
      uint8_t BANK_A_PARTS_CC,
      uint8_t BANK_B_PARTS_CC,
      uint8_t rgbIndex, );

  // Getters

  // Setters
  void toggleGroupMode();
  void setBank();

  // Methods
  void onPress() override;
  void onRelease() override;

  void updateLEDButton();
  void updateSceneSection();
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