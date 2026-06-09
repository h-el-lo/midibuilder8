#include "SceneSelectorButton.h"





// ═════════════════════════════════════════════
//  SelectorButton
// ═════════════════════════════════════════════

SceneSelectorButton::SceneSelectorButton(
  uint8_t anodePin,
  uint8_t cathodePin,
  uint8_t BANK_A_SCENE_CC,
  uint8_t BANK_B_SCENE_CC,
  uint8_t BANK_A_PARTS_CC,
  uint8_t BANK_B_PARTS_CC,
  uint8_t rgbIndex)
  : Button(XY_BUTTON, anodePin, cathodePin),
    _BANK_A_SCENE_CC(BANK_A_SCENE_CC),
    _BANK_B_SCENE_CC(BANK_B_SCENE_CC),
    _BANK_A_PARTS_CC(BANK_A_PARTS_CC),
    _BANK_B_PARTS_CC(BANK_B_PARTS_CC),
    _rgbIndex(rgbIndex) {

  _groupMode = MODE_SCENE;
  _bank = BANK_A;
}

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

void SceneSelectorButton::onPress() {
  // Offset CC by bank: bank B shifts by 8
  uint8_t bankOffset = (_bank == BANK_B) ? 8 : 0;

  if (*_groupMode == MODE_SCENE) {
    // Scene mode: momentary CC64
    _midi->sendCC(64, 127, _index + bankOffset);
  } else {
    // Preset mode: latch CC0/127
    _midi->sendCC(_ccNumber, 127, _index + bankOffset);
  }

  updateLED();
}

void SceneSelectorButton::onRelease() {
  if (*_groupMode == MODE_SCENE) {
    uint8_t bankOffset = (*_bank == BANK_B) ? 8 : 0;
    _midi->sendCC(64, 0, _index + bankOffset);
  }
  // Preset mode holds its LED state — no LED change on release
}

void SceneSelectorButton::updateLED() {
  if (_rgbIndex == 255) return;  // sentinel: no LED assigned
 
  if (*_groupMode == MODE_SCENE) {
    _rgb->setColor(_rgbIndex, _state ? 0x00FF00 : 0x000000);  // green when active
  } else {
    _rgb->setColor(_rgbIndex, _state ? 0x0000FF : 0x000000);  // blue when selected
  }
}
