#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class Screen : public LiquidCrystal_I2C {
private:
  uint8_t _SDA_PIN;
  uint8_t _SCL_PIN;
  uint8_t _LCD_ROWS;
  uint8_t _LCD_COLS;
  uint8_t _LCD_ADDR;

public:
  // Constructors
  Screen(uint8_t LCD_ADDR, uint8_t SDA_PIN, uint8_t SCL_PIN, uint8_t LCD_COLS, uint8_t LCD_ROWS);
  Screen(uint8_t SDA_PIN, uint8_t SCL_PIN, uint8_t LCD_COLS, uint8_t LCD_ROWS);
  Screen(uint8_t LCD_ADDR, uint8_t LCD_COLS, uint8_t LCD_ROWS);
  Screen(uint8_t LCD_COLS, uint8_t LCD_ROWS);
  Screen();

  // Getters

  // Setters

  // Methods
  void init();
};


#endif