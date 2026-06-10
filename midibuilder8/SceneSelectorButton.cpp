#include "SceneSelectorButton.h"


// ═════════════════════════════════════════════
//  SelectorButton
// ═════════════════════════════════════════════

SceneSelectorButton::SceneSelectorButton(
  uint8_t anodePin,
  uint8_t cathodePin,
  uint8_t index,
  uint8_t BANK_A_SCENE_CC,
  uint8_t BANK_B_SCENE_CC,
  uint8_t BANK_A_PARTS_CC,
  uint8_t BANK_B_PARTS_CC,
  uint8_t rgbIndex)
  : Button(XY_BUTTON, anodePin, cathodePin),
    _index(index),
    _rgbIndex(rgbIndex) {
  _SCENE_CC[BANK_A] = BANK_A_SCENE_CC;
  _SCENE_CC[BANK_B] = BANK_B_SCENE_CC;
  _PARTS_CC[BANK_A] = BANK_A_PARTS_CC;
  _PARTS_CC[BANK_B] = BANK_B_PARTS_CC;
}

// Setters
void SceneSelectorButton::toggleGroupMode() {
  SceneSelectorButton::_groupMode = (SceneSelectorButton::_groupMode == MODE_SCENE) ? MODE_PARTS : MODE_SCENE;
  updateRGBSection();
}

void SceneSelectorButton::setBank(Bank bank) {
  SceneSelectorButton::_bank = bank;
  updateRGBSection();
}

// Methods
void SceneSelectorButton::onPress() {

  if (_groupMode == MODE_SCENE) {
    SET_SCENE_BIT(sceneState, _bank, _index);
    CLEAR_BYTE(sceneState, ((_bank == BANK_A)? BANK_B: BANK_A));
    controlChange(GLOBAL_MIDI_CHANNEL, _SCENE_CC[_bank], 64);
    updateRGBSection();
  } else {
    INVERT_BIT(partsState, _bank, _index);
    uint8_t value = map(GET_BIT(partsState, _bank, _index), 0, 1, 0, 127);
    controlChange(GLOBAL_MIDI_CHANNEL, _PARTS_CC[_bank], value);
    updateRGB(_rgbIndex);
  }
}


void SceneSelectorButton::clearallparts() {
  CLEAR_BYTE(partsState, BANK_A);
  for (uint8_t i = 0; i < 8; i++) {
    controlChange(GLOBAL_MIDI_CHANNEL, _PARTS_CC[BANK_A], 0);
  }

  CLEAR_BYTE(partsState, BANK_B);
  for (uint8_t i = 0; i < 8; i++) {
    controlChange(GLOBAL_MIDI_CHANNEL, _PARTS_CC[BANK_B], 0);
  }
}


void SceneSelectorButton::updateRGBSection() {
  // the correct rgb index for entire strip must correctly be derived before anything
  // There are 8 scene selector buttons on the midi keyboard.
  // _rgbindex of button calling the method - its _index [indexing starts at 0]
  uint8_t startIndex = _rgbIndex - _index;

  if (_groupMode == MODE_SCENE) {
    for (uint8_t i = 0; i < 8; i++) {
      if (GET_BIT(sceneState, _bank, i)) {
        BUTTON_RGB_STRIP.update(startIndex + i, sceneSelectedColorColor[_bank]);
      } else {
        BUTTON_RGB_STRIP.update(startIndex + i, sceneUnselectedColor[_bank]);
      }
    }
  } else {
    for (uint8_t i = 0; i < 8; i++) {
      if (GET_BIT(partsState, _bank, i)) {
        BUTTON_RGB_STRIP.update(startIndex + i, partOnColorColor[_bank]);
      } else {
        BUTTON_RGB_STRIP.update(startIndex + i, partOffColor[_bank]);
      }
    }
  }
}

void SceneSelectorButton::updateRGB(_rgbIndex) {
  if (_groupMode == MODE_PARTS) {
    if GET_BIT (partsState, _bank, _index) {
      BUTTON_RGB_STRIP.update(_rgbIndex, partsOnColor[_bank]);
    } else {
      BUTTON_RGB_STRIP.update(_rgbIndex, partsOffColor[_bank]);
    }
  }
}
