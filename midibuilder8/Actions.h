#pragma once
#include "Keys.h"
#include "Screen.h"

inline void home() {
  Serial.println("Home Button Pressed");
}

inline void settings() {
  Serial.println("Settings Button Pressed");
}

inline void exit() {
  Serial.println("Exit Button Pressed");
}

inline void enter() {
  Serial.println("Enter Button Pressed");
} 



// add a proper home screen to screen
// issues may be with i2c, power d