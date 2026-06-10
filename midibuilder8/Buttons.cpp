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

  // when scene select is changed to part mode,
  update the rgb strip according to the saved values of the states of each button
    consider creating an array of these states.
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