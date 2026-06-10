#ifndef RGB_H
#define RGB_H

#include <Arduino.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN 14

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 29

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

class RGB_Strip {
private:
  Adafruit_NeoPixel _strip;
  uint8_t _brightness;
  uint8_t _LED_COUNT public : void begin() {
    _strip.begin();
  };

public:
  // Constructors

  // Getters

  // Setters
  void setBrightness();
  void setPixelcolor(uint8_t pixel, hex color)

  // Methods
  void begin();
  
void clearPixel() {
  _strip.setPixelColor(rgbIndex, pixels.Color(0, 0, 0));
       _strip.show();
}
  void update(uint8_t rgbIndex, uint8_t (&color)[3]) {
    _strip.setPixelColor(rgbIndex, pixels.Color(color[0], color[1], color[2]));
    _strip.show();
  };
};

extern RGB_Strip BUTTON_RGB_STRIP;
extern RGB_Strip INDICATOR_STRIP;

#endif