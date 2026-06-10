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
  _groupMode = !_groupMode;

  if (_groupMode == MODE_SCENE) {
    // LOAD SCENE ARRAY, BANK A AND BANK B
    // UPDATE THE RGB ACCORDING TO SELECTED BANK
  } else if (_groupMode == MODE_PARTS) {
    // LOAD PARTS ARRAY, BANK A AND BANK B
    // UPDATE THE RGB ACCORDING TO SELECTED BANK
  }
}

void SceneSelectorButton::setBank(Bank bank) {
  _bank = bank;
}

// Methods
void SceneSelectorButton::onPress() {
  // Offset CC by bank: bank B shifts by 8
  uint8_t bankOffset = (_bank == BANK_B) ? 8 : 0;

  if (_groupMode == MODE_SCENE) {
    SET_SCENE_BIT(partsState, _bank, _index);
    controlChange(GLOBAL_MIDI_CHANNEL, SCENE_CC[_bank], 64);
  } else {
    INVERT_PARTS_BIT(partsState, _bank, _index);
    uint8_t value = map(GET_BIT(partsState, _bank, _index), 0, 1, 0, 127);
    controlChange(GLOBAL_MIDI_CHANNEL, _PARTS_CC[_bank], value);
  }

  updateRGB();
}


void clearallparts() {}


void SceneSelectorButton::updateRGB() {
    if(_groupMode == MODE_SCENE) {
        for (uint8_t i= 0; i< 8; i++) {

        }

    } else
}

void SceneSelectorButton::updateRGB(_rgbIndex) {
  if (_groupMode == MODE_PARTS) {
    if GET_BIT (partsState, _bank, _index) {
      BUTTON_RGB_STRIP.update(_rgbIndex, partsOnColor[_bank]);
    } else {
        BUTTON_RGB_STRIP.update(_rgbIndex, partsOffColor[_bank]);
    }
  } else {

  }
}
