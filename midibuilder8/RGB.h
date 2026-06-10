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

class RGBStrip : public Adafruit_NeoPixel {
private:
  uint8_t _LED_PIN;
  uint8_t _LED_COUNT;
  uint8_t _brightness;


public:
  // Constructors
  RGBStrip(uint8_t LED_COUNT, uint8_t LED_PIN, uint8_t brightness)
    : Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800), _brightness(brightness) {

    Adafruit_NeoPixel::begin();
    Adafruit_NeoPixel::setBrightness(_brightness);
    Adafruit_NeoPixel::clear();  // INITIALIZE NeoPixel strip object (REQUIRED)
    Adafruit_NeoPixel::show();   // Turn OFF all pixels ASAP
  };
  // Getters

  // Setters

  // Methods
  void clearPixel(uint8_t rgbIndex) {
    Adafruit_NeoPixel::setPixelColor(rgbIndex, Adafruit_NeoPixel::Color(0, 0, 0));
    Adafruit_NeoPixel::show();
  }

  void update(uint8_t rgbIndex, ColorStruct color) {
    Adafruit_NeoPixel::setPixelColor(rgbIndex, Adafruit_NeoPixel::Color(color.r, color.g, color.b));
    Adafruit_NeoPixel::show();
  };
};

extern RGBStrip BUTTON_RGB_STRIP;
extern RGBStrip INDICATOR_STRIP;

#endif