#include "Screen.h"
#include <Wire.h>

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

  Serial.println("Screen init begun.");
  Wire.begin(_SDA_PIN, _SCL_PIN);
  _lcd.init();
  _lcd.backlight();

  instance = this;
  Serial.println("Screen is initialized.");
}

void Screen::printHome() {
  // Print a message to the LCD.
  _lcd.setCursor(3, 0);
  _lcd.print("Hello, world!");
  _lcd.setCursor(2, 1);
  _lcd.print("Ywrobot Arduino!");
  _lcd.setCursor(0, 2);
  _lcd.print("Arduino LCM IIC 2004");
  _lcd.setCursor(2, 3);
  _lcd.print("Power By Ec-yuan!");
}

void Screen::printHomeHandler() {
  if (instance != nullptr) {
    instance->printHome();
  }
}
