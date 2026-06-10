#include "buttons.h"
#include "Multiplexer.h"
#include "MIDIHelper.h"

// ═════════════════════════════════════════════
//  Button (base)
// ═════════════════════════════════════════════

Button::Button(ButtonType type, uint8_t anodePin, uint8_t cathodePin)
  : _type(type), _anodePin(anodePin), _cathodePin(cathodePin) {}

bool Button::readHardware() {
  static bool reading = false;

  if (_type == XY_BUTTON) {
    Mux3.writeToChannel(_anodePin, LOW);
    pinMode(_cathodePin, INPUT_PULLUP);
    reading = !digitalRead(_cathodePin);
    Mux3.writeToChannel(_anodePin, HIGH);

  } else if (_type == XZ_BUTTON) {
    Mux3.writeToChannel(_anodePin, LOW);
    reading = !Mux1.readChannel(_cathodePin);
    Mux3.writeToChannel(_anodePin, HIGH);

  } else if (_type == YZ_BUTTON) {
    pinMode(_anodePin, OUTPUT);
    digitalWrite(_anodePin, LOW);
    reading = !Mux1.readChannel(_cathodePin);
    digitalWrite(_anodePin, HIGH);
  }

  return reading;
}

void Button::read() {
  if (millis() - _lastUpdated < DEBOUNCE_MS) return;

  _state = readHardware();

  if (_state == _pState) return;  // no change

  _lastUpdated = millis();
  _pState = _state;

  if (_state) {
    onPress();
  } else {
    // Do nothing
  }
}


// ═════════════════════════════════════════════
//  NumpadButton
// ═════════════════════════════════════════════
NumpadButton::NumpadButton(uint8_t anodePin, uint8_t cathodePin, uint8_t CCNumber)
  : Button(YZ_BUTTON, anodePin, cathodePin), _CCNumber(CCNumber) {}

void NumpadButton::onPress() {
  controlChange(GLOBAL_MIDI_CHANNEL, _CCNumber, 64);
}


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
    SET_SCENE_BIT(sceneState[_bank], _index);
    CLEAR_BYTE(sceneState[((_bank == BANK_A) ? BANK_B : BANK_A)]);
    controlChange(GLOBAL_MIDI_CHANNEL, _SCENE_CC[_bank], 64);
    updateRGBSection();
  } else {
    INVERT_BIT(partsState[_bank], _index);
    uint8_t value = map(GET_BIT(partsState[_bank], _index), 0, 1, 0, 127);
    controlChange(GLOBAL_MIDI_CHANNEL, _PARTS_CC[_bank], value);
    updateRGB(_rgbIndex);
  }
}


void SceneSelectorButton::clearallparts() {
  CLEAR_BYTE(partsState[BANK_A]);
  for (uint8_t i = 0; i < 8; i++) {
    controlChange(GLOBAL_MIDI_CHANNEL, _PARTS_CC[BANK_A], 0);
  }

  CLEAR_BYTE(partsState[BANK_B]);
  for (uint8_t i = 0; i < 8; i++) {
    controlChange(GLOBAL_MIDI_CHANNEL, _PARTS_CC[BANK_B], 0);
  }
}

void SceneSelectorButton::updateRGB(uint8_t rgbIndex) {
  if (_groupMode == MODE_PARTS) {
    if GET_BIT (partsState[_bank], _index) {
      BUTTON_RGB_STRIP.update(_rgbIndex, partsOnColor[_bank]);
    } else {
      BUTTON_RGB_STRIP.update(_rgbIndex, partsOffColor[_bank]);
    }
  }
}


void SceneSelectorButton::updateRGBSection() {
  // the correct rgb index for entire strip must correctly be derived before anything
  // There are 8 scene selector buttons on the midi keyboard.
  // _rgbindex of button calling the method - its _index [indexing starts at 0]
  uint8_t startIndex = _rgbIndex - _index;

  if (_groupMode == MODE_SCENE) {
    for (uint8_t i = 0; i < 8; i++) {
      if (GET_BIT(sceneState[_bank], i)) {
        BUTTON_RGB_STRIP.update(startIndex + i, sceneSelectedColor[_bank]);
      } else {
        BUTTON_RGB_STRIP.update(startIndex + i, sceneUnselectedColor[_bank]);
      }
    }
  } else {
    for (uint8_t i = 0; i < 8; i++) {
      if (GET_BIT(partsState[_bank], i)) {
        BUTTON_RGB_STRIP.update(startIndex + i, partsOnColor[_bank]);
      } else {
        BUTTON_RGB_STRIP.update(startIndex + i, partsOffColor[_bank]);
      }
    }
  }
}


// ═════════════════════════════════════════════
//  GeneralPurposeCCButton
// ═════════════════════════════════════════════

GeneralPurposeCCButton::GeneralPurposeCCButton(
  uint8_t anodePin,
  uint8_t cathodePin,
  uint8_t CCNumber,
  uint8_t rgbIndex,
  ColorStruct color)
  : Button(YZ_BUTTON, anodePin, cathodePin), _CCNumber(CCNumber), _rgbIndex(rgbIndex), _color(color) {
  updateRGB();
}

void GeneralPurposeCCButton::onPress() {
  controlChange(GLOBAL_MIDI_CHANNEL, _CCNumber, 64);
}

void GeneralPurposeCCButton::updateRGB() {
  BUTTON_RGB_STRIP.update(_rgbIndex, _color);
}




// ═════════════════════════════════════════════
//  ActionButton
// ═════════════════════════════════════════════
ActionButton::ActionButton(ButtonType type,
                           uint8_t anodePin,
                           uint8_t cathodePin,
                           void (*onPressCallback)())
  : Button(type, anodePin, cathodePin),
    _onPressCallback(onPressCallback) {}

void ActionButton::onPress() {
  if (_onPressCallback) _onPressCallback();
}


// ═════════════════════════════════════════════
//  ButtonManager
// ═════════════════════════════════════════════

ButtonManager::ButtonManager(Button** buttons, uint8_t count)
  : _buttons(buttons), _count(count) {}

void ButtonManager::scan() {
  for (uint8_t i = 0; i < _count; i++) {
    _buttons[i]->read();
  }
}


// ═════════════════════════════════════════════
//  Composition Root
//  All button instances live here
//  Called once from setup()
// ═════════════════════════════════════════════

// ── Shared group context ──
static SceneSelectorButton::GroupMode groupMode = SceneSelectorButton::MODE_PARTS;
static SceneSelectorButton::Bank groupBank = SceneSelectorButton::BANK_A;

// ── Button instance storage ──
static const uint8_t BUTTON_COUNT = 46;
static Button* buttonArray[BUTTON_COUNT];
static ButtonManager* manager = nullptr;

void initButtons() {

  uint8_t i = 0;

  // ── Group of Eight (XY buttons, indices 0–7) ──
  // { anodePin, cathodePin, index, rgbIndex, BANK_A_SCENE_CC, BANK_B_SCENE_CC, BANK_A_PARTS_CC, BANK_B_PARTS_CC, rgbIndex}
  uint8_t sceneSelectorDefs[8][8] = {
    { 5, 4, 0, 46, 54, 102, 110, 22 },
    { 6, 4, 1, 47, 55, 103, 111, 23 },
    { 6, 3, 2, 48, 56, 104, 112, 24 },
    { 6, 2, 3, 49, 57, 105, 113, 25 },
    { 6, 1, 4, 50, 58, 106, 114, 26 },
    { 5, 1, 5, 51, 59, 107, 115, 27 },
    { 4, 1, 6, 52, 60, 108, 116, 28 },
    { 2, 1, 7, 53, 61, 109, 117, 29 },
  };

  for (uint8_t g = 0; g < 8; g++) {
    buttonArray[i++] = new SceneSelectorButton(
      sceneSelectorDefs[g][0], sceneSelectorDefs[g][1],
      sceneSelectorDefs[g][2], sceneSelectorDefs[g][3],
      sceneSelectorDefs[g][4], sceneSelectorDefs[g][5],
      sceneSelectorDefs[g][6], sceneSelectorDefs[g][7]);
  }

  // ── Numpad (YZ buttons, 12 keys: 0–9, +, -) ──
  // { anodePin, cathodePin, ccNumber}
  uint8_t numpadDefs[12][4] = {
    { 4, 2, 29 },   // 1
    { 3, 2, 30 },   // 2
    { 2, 2, 31 },   // 3
    { 1, 2, 62 },   // 4
    { 4, 3, 63 },   // 5
    { 3, 3, 85 },   // 6
    { 2, 3, 86 },   // 7
    { 1, 3, 87 },   // 8
    { 4, 4, 89 },   // 9
    { 3, 4, 90 },   // 0
    { 2, 4, 102 },  // +
    { 1, 4, 103 },  // -
  };

  for (uint8_t n = 0; n < 12; n++) {
    buttonArray[i++] = new NumpadButton(
      numpadDefs[n][0], numpadDefs[n][1],
      numpadDefs[n][2]);
  }


  // ── GeneralPurposeCC (YZ buttons, 4 buttons: 0–9, +, -) ──
  // { anodePin, cathodePin, ccNumber, color}
  uint8_t GeneralPurposeCCDefs[4][5] = {
    { 4, 2, 29, 1, { 0, 150, 220 } },  // 1
    { 3, 2, 30, 2, { 0, 150, 220 } },  // 2
    { 2, 2, 31, 3, { 0, 150, 220 } },  // 3
    { 1, 2, 62, 4, { 0, 150, 220 } },  // 4
  };

  for (uint8_t n = 0; n < 4; n++) {
    buttonArray[i++] = new GeneralPurposeCCButton(
      GeneralPurposeCCDefs[n][0], GeneralPurposeCCDefs[n][1],
      GeneralPurposeCCDefs[n][2]);
  }



  // ── XZ Button (Memory) ──
  buttonArray[i++] = new ActionButton(
    XZ_BUTTON, 4, 1,
    []() { /* TODO: memory press action  */ },
    []() { /* TODO: memory release action */ });

  // ── Remaining XY action buttons ──
  // Add yours here following the same pattern:
  // buttonArray[i++] = new ActionButton(Button::XY_BUTTON, anodePin, cathodePin, onPress, onRelease);

  // ── RGB strip buttons ──
  // buttonArray[i++] = new RGBButton(Button::XY_BUTTON, anodePin, cathodePin, rgbIndex, rgb);

  manager = new ButtonManager(buttonArray, i);
}

// Call in loop()
void scanButtons() {
  if (manager) manager->scan();
}