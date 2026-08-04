#pragma once

// ================= ONBOARD RGB VARIABLES ==================
// The fade effect without the delay function (using "RGB_timer" and "micros()")

uint8_t r, g, b;

// Synthage default colors
uint8_t colors[12][3] = {
  { 51, 86, 255 },   // Color 1 - LightBlue // Done
  { 0, 0, 100 },     // Color 2 - Blue // Done
  { 101, 0, 205 },   // Color 3 - Indigo/Purple // Done
  { 80, 0, 87 },     // Color 4 - Violet // Done
  { 195, 0, 60 },    // Color 5 - Magenta // Done
  { 50, 3, 0 },      // Color 6 - Red // Done
  { 245, 65, 2 },    // Color 7 - Orange // Done
  { 255, 190, 0 },   // Color 8 - Yellow // Done
  { 180, 255, 0 },   // Color 9 - Lawn Green // Done
  { 120, 255, 0 },   // Color 10 - Green // Done
  { 72, 255, 51 },   // Color 11- Mint Green // Done
  { 75, 155, 214 },  // Color 12 - Cyan Done // Done
};

// Animation variables (Very similar to Synthage)
int timeon = 300;     // milliseconds
int timeoff = 0;      // milliseconds
uint8_t steps = 30;  // frames per cycle
unsigned long RGB_timer = 0;
int threshold = (timeon * 1000) / steps;
int RGB_count = 0;
uint8_t stage = 1;  // Begin animation at stage 1
// ==========================================================




inline void rgb(uint8_t R, uint8_t G, uint8_t B) {
  r = R;
  g = G;
  b = B;
}


inline void linear_stepper_forward_backward(uint8_t r, uint8_t g, uint8_t b, int steps, int timeon, int timeoff) {
  // This pattern is in four stages, increment, hold, decrement, hold.

  if (stage == 1) {
    if (RGB_count <= steps) {
      if (micros() - RGB_timer >= threshold) {
        BUTTON_STRIP.update(11, { ((r * RGB_count) / steps), ((g * RGB_count) / steps), ((b * RGB_count) / steps) });
        RGB_timer = micros();
        RGB_count += 1;
      }
    }

    if (RGB_count == steps) {
      RGB_count = 0;
      BUTTON_STRIP.update(11, { r, g, b });
      RGB_timer = micros();  // Reset the RGB_timer variable
      stage = 2;
    }


  } else if (stage == 2) {
    if (micros() - RGB_timer >= timeoff * 1000) {
      stage = 3;
    }

  } else if (stage == 3) {

    if (RGB_count <= steps) {
      if (micros() - RGB_timer >= threshold) {
        BUTTON_STRIP.update(11, { ((r * (steps - RGB_count)) / steps), ((g * (steps - RGB_count)) / steps), ((b * (steps - RGB_count)) / steps) });
        RGB_timer = micros();
        RGB_count += 1;
      }
    }

    if (RGB_count == steps) {
      RGB_count = 0;
      BUTTON_STRIP.update(11, { 0, 0, 0 });
      RGB_timer = micros();  // Reset the RGB_timer variable
      stage = 4;
    }

  } else if (stage == 4) {
    if (micros() - RGB_timer >= timeoff * 1000) {
      stage = 1;
    }
  }
}