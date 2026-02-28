#include "Screen.h"
#include <Wire.h>
// #include <LiquidCrystal_I2C.h>

// Constructors
Screen::Screen(uint8_t LCD_ADDR, uint8_t SDA_PIN, uint8_t SCL_PIN, uint8_t LCD_COLS, uint8_t LCD_ROWS)
  : LiquidCrystal_I2C(LCD_ADDR, LCD_COLS, LCD_ROWS), _LCD_ADDR(LCD_ADDR), _SDA_PIN(SDA_PIN), _SCL_PIN(SCL_PIN), _LCD_COLS(LCD_COLS), _LCD_ROWS(LCD_ROWS), {

  Wire.begin(_SDA_PIN, _SCL_PIN);
  LiquidCrystal_I2C::init();
  backlight();
}

Screen::Screen(uint8_t SDA_PIN, uint8_t SCL_PIN, uint8_t LCD_COLS, uint8_t LCD_ROWS)
  : Screen(0x27, SDA_PIN, SCL_PIN, LCD_COLS, LCD_ROWS) {
}

Screen::Screen(uint8_t LCD_ADDR, uint8_t LCD_COLS, uint8_t LCD_ROWS)
  : Screen(LCD_ADDR, 21, 22, LCD_COLS, LCD_ROWS) {
}

Screen::Screen(uint8_t LCD_COLS, uint8_t LCD_ROWS)
  : Screen(0x27, 21, 22, LCD_COLS, LCD_ROWS) {
}

Screen::Screen()
  : Screen(0x27, 21, 22, 16, 2) {
}










Screen::init() {

}