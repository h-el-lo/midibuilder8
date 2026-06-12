#ifndef RGB_H
#define RGB_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

struct ColorStruct {
  uint8_t r, g, b;
};

class RGBStrip {
private:
  Adafruit_NeoPixel _strip;
  uint8_t _brightness;

public:
  RGBStrip(uint8_t LED_COUNT, uint8_t LED_PIN, uint8_t brightness)
    : _strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800), _brightness(brightness){};

  void begin() {

// These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
// Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
#endif
    // END of Trinket-specific code.

    _strip.begin();
    _strip.setBrightness(_brightness);
    // _strip.clear();
    _strip.show();
  }

  void clearPixel(uint8_t rgbIndex) {
    _strip.setPixelColor(rgbIndex, _strip.Color(0, 0, 0));
    _strip.show();
  }

  void update(uint8_t rgbIndex, ColorStruct color) {
    _strip.setPixelColor(rgbIndex, _strip.Color(color.r, color.g, color.b));
    _strip.show();
  }
};

extern RGBStrip BUTTON_STRIP;
// extern RGBStrip INDICATOR_STRIP;

#endif