#include "buttons.h"
#include "Multiplexer.h"

// ─────────────────────────────────────────────
//  Extern references to multiplexers
//  (defined in your Multiplexer.cpp)
// ─────────────────────────────────────────────
extern Multiplexer Mux1;  // cathode mux (Z axis)
extern Multiplexer Mux3;  // anode mux   (X axis)


// ═════════════════════════════════════════════
//  Button (base)
// ═════════════════════════════════════════════

Button::Button(ButtonType type, uint8_t anodePin, uint8_t cathodePin, bool momentary)
  : _type(type), _anodePin(anodePin), _cathodePin(cathodePin), _momentary(momentary) {}

bool Button::readHardware() {
  bool reading = false;

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

  bool reading = readHardware();

  if (reading == _pState) return;  // no change

  _pState = reading;
  _lastUpdated = millis();

  if (_momentary) {
    _state = reading;
    if (_state) onPress();
    else onRelease();

  } else {
    // Latch: toggle on press only
    if (reading) {
      _state = !_state;
      if (_state) onPress();
      else onRelease();
    }
  }
}


// ═════════════════════════════════════════════
//  SelectorButton
// ═════════════════════════════════════════════

SceneSelectorButton::SceneSelectorButton(
                         uint8_t anodePin,
                         uint8_t cathodePin,
                         uint8_t index,
                         uint8_t rgbIndex,
                         uint8_t ccNumber,
                         GroupMode* groupMode,
                         Bank* bank,
                         RGB* rgb)
  : Button(XY_BUTTON, anodePin, cathodePin),
    _index(index),
    _rgbIndex(rgbIndex),
    _ccNumber(ccNumber),
    _groupMode(groupMode),
    _bank(bank),
    _midi(midi),
    _rgb(rgb) {

  // Momentary in scene mode, latch in preset mode
  _momentary = (*_groupMode == MODE_SCENE);
}

void SceneSelectorButton::onPress() {
  // Offset CC by bank: bank B shifts by 8
  uint8_t bankOffset = (*_bank == BANK_B) ? 8 : 0;

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


// ═════════════════════════════════════════════
//  NumpadButton
// ═════════════════════════════════════════════

NumpadButton::NumpadButton(ButtonType type,
                           uint8_t anodePin,
                           uint8_t cathodePin,
                           uint8_t ccNumber,
                           uint8_t ccValue,
                           MIDIHelper* midi)
  : Button(type, anodePin, cathodePin, true),  // always momentary
    _ccNumber(ccNumber),
    _ccValue(ccValue),
    _midi(midi) {}

void NumpadButton::onPress() {
  _midi->sendCC(_ccNumber, _ccValue);
}

void NumpadButton::onRelease() {
  // No action on release for numpad
}


// ═════════════════════════════════════════════
//  TransposeButton
// ═════════════════════════════════════════════

TransposeButton::TransposeButton(ButtonType type,
                                 uint8_t anodePin,
                                 uint8_t cathodePin,
                                 Direction direction,
                                 Keys* keys)
  : Button(type, anodePin, cathodePin, true),  // momentary
    _direction(direction),
    _keys(keys) {}

void TransposeButton::onPress() {
  if (_direction == UP) _keys->transposeUp();
  else _keys->transposeDown();
}

void TransposeButton::onRelease() {}


// ═════════════════════════════════════════════
//  OctaveButton
// ═════════════════════════════════════════════

OctaveButton::OctaveButton(ButtonType type,
                           uint8_t anodePin,
                           uint8_t cathodePin,
                           Direction direction,
                           Keys* keys)
  : Button(type, anodePin, cathodePin, true),
    _direction(direction),
    _keys(keys) {}

void OctaveButton::onPress() {
  if (_direction == UP) _keys->octaveUp();
  else _keys->octaveDown();
}

void OctaveButton::onRelease() {}


// ═════════════════════════════════════════════
//  ActionButton
// ═════════════════════════════════════════════

ActionButton::ActionButton(ButtonType type,
                           uint8_t anodePin,
                           uint8_t cathodePin,
                           void (*onPressCallback)(),
                           void (*onReleaseCallback)())
  : Button(type, anodePin, cathodePin, true),
    _onPressCallback(onPressCallback),
    _onReleaseCallback(onReleaseCallback) {}

void ActionButton::onPress() {
  if (_onPressCallback) _onPressCallback();
}

void ActionButton::onRelease() {
  if (_onReleaseCallback) _onReleaseCallback();
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

void initButtons(MIDIHelper* midi, RGB* rgb, Keys* keys) {

  uint8_t i = 0;

  // ── Group of Eight (XY buttons, indices 0–7) ──
  // { anodePin, cathodePin, index, rgbIndex, ccNumber }
  uint8_t groupDefs[8][5] = {
    { 5, 4, 0, 0, 20 },
    { 5, 3, 1, 1, 21 },
    { 5, 2, 2, 2, 22 },
    { 5, 1, 3, 3, 23 },
    { 6, 4, 4, 4, 24 },
    { 6, 3, 5, 5, 25 },
    { 6, 2, 6, 6, 26 },
    { 6, 1, 7, 7, 27 },
  };

  for (uint8_t g = 0; g < 8; g++) {
    buttonArray[i++] = new SceneSelectorButton(
      Button::XY_BUTTON,
      groupDefs[g][0], groupDefs[g][1],
      groupDefs[g][2], groupDefs[g][3], groupDefs[g][4],
      &groupMode, &groupBank,
      midi, rgb);
  }

  // ── Numpad (YZ buttons, 12 keys: 0–9, +, -) ──
  // { anodePin, cathodePin, ccNumber, ccValue }
  uint8_t numpadDefs[12][4] = {
    { 3, 4, 80, 0 },    // 0
    { 4, 2, 81, 1 },    // 1
    { 3, 2, 81, 2 },    // 2
    { 2, 2, 81, 3 },    // 3
    { 1, 2, 81, 4 },    // 4
    { 4, 3, 81, 5 },    // 5
    { 3, 3, 81, 6 },    // 6
    { 2, 3, 81, 7 },    // 7
    { 1, 3, 81, 8 },    // 8
    { 4, 4, 81, 9 },    // 9
    { 2, 4, 82, 127 },  // +
    { 1, 4, 82, 0 },    // -
  };

  for (uint8_t n = 0; n < 12; n++) {
    buttonArray[i++] = new NumpadButton(
      Button::YZ_BUTTON,
      numpadDefs[n][0], numpadDefs[n][1],
      numpadDefs[n][2], numpadDefs[n][3],
      midi);
  }

  // ── Transpose ──
  buttonArray[i++] = new TransposeButton(Button::XY_BUTTON, 2, 4, TransposeButton::UP, keys);
  buttonArray[i++] = new TransposeButton(Button::XY_BUTTON, 2, 3, TransposeButton::DOWN, keys);

  // ── XZ Button (Memory) ──
  buttonArray[i++] = new ActionButton(
    Button::XZ_BUTTON, 4, 1,
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