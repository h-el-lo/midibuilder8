#include <Arduino.h>
#include "Screen.h"
#include "MIDIHelper.h"
#include "Keys.h"

// Constructors
Screen::Screen(uint8_t LCD_ADDR, uint8_t SDA_PIN, uint8_t SCL_PIN, uint8_t LCD_COLS, uint8_t LCD_ROWS)
  : _lcd(LCD_ADDR, LCD_COLS, LCD_ROWS), _SDA_PIN(SDA_PIN), _SCL_PIN(SCL_PIN), _LCD_COLS(LCD_COLS), _LCD_ROWS(LCD_ROWS) {
}

Screen::Screen(uint8_t SDA_PIN, uint8_t SCL_PIN, uint8_t LCD_COLS, uint8_t LCD_ROWS)
  : Screen(0x27, SDA_PIN, SCL_PIN, LCD_COLS, LCD_ROWS) {
}

Screen::Screen(uint8_t LCD_ADDR, uint8_t LCD_COLS, uint8_t LCD_ROWS)
  : Screen(LCD_ADDR, 21, 47, LCD_COLS, LCD_ROWS) {
}

Screen::Screen(uint8_t LCD_COLS, uint8_t LCD_ROWS)
  : Screen(0x27, 21, 47, LCD_COLS, LCD_ROWS) {
}

Screen::Screen()
  : Screen(0x27, 21, 47, 20, 4) {
}

void Screen::init() {
  _lcd.init();
  _lcd.backlight();

  instance = this;
  Serial.println("Screen is initialized.");
}

void Screen::printHome() {
  if (_page != PAGE_HOME) {
    _page = PAGE_HOME;
    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.print("Channel: ");
  }
  _lcd.setCursor(8, 0);
  _lcd.printf("%2d", GLOBAL_MIDI_CHANNEL);
  _lcd.setCursor(16, 0);
  uint8_t batteryPercentage = 100;
  _lcd.printf("%3d", batteryPercentage);
  _lcd.print("%");

  _lcd.setCursor(0, 1);
  _lcd.print("Transpose: ");
  _lcd.setCursor(11, 1);
  _lcd.printf("%3d", keys.getTranspose());
}

void Screen::printHomeHandler() {
  if (instance != nullptr) {
    instance->printHome();
  }
}

void Screen::printTranspose() {
  if (_page != PAGE_TRANSPOSE) {
    _page = PAGE_TRANSPOSE;
    _lcd.clear();
    _lcd.setCursor(3, 1);
    _lcd.print("Transpose: ");
  }
  _lcd.setCursor(12, 1);
  // Serial.print("Transpose from screenprint is: ");
  // Serial.println(keys.getTranspose());
  _lcd.printf("%4d", keys.getTranspose());
}

void Screen::printChannel() {

  if (_page != PAGE_CHANNEL) {
    _page = PAGE_CHANNEL;
    _lcd.clear();
    _lcd.setCursor(5, 1);
    _lcd.print("Channel: ");
  }
  _lcd.setCursor(12, 1);
  _lcd.printf("%4d", GLOBAL_MIDI_CHANNEL);
}

void Screen::knobsEnablePage() {
  //no op
}


// ============================  SCREEN OBJECT  ============================
// Screen screen(0x27, 20, 4);
Screen screen;
// =========================================================================