#pragma once

#include "ADSManager.h"
#include "MIDIHelper.h"
#include "Screen.h"

#include <Wire.h>
#define SDA_PIN 21
#define SCL_PIN 47

#include "RGB.h"
#include "Flasher.h"
#include "Encoder.h"
#include "Buttons.h"

// #include "MenuController.h"


inline void Setup() {

  Serial.begin(921600);

  BUTTON_STRIP.begin();
  // INDICATOR_STRIP.begin();

  // Set up SYNTHAGE SUPER BUTTON animation
  uint8_t sel = 1;  // Color selection
  uint8_t* color = colors[sel - 1];
  rgb(color[0], color[1], color[2]);

  // Begin MIDI
  USB.begin();
  usbmidi.begin();  // USB MIDI INSTANCE

  BLEMIDI_MIDI.begin();  // BLE MIDI INSTANCE

  BLEMIDI_TRANSPORT.setHandleConnected([]() {
    BLE_MIDI_IS_CONNECTED = true;
    BUTTON_STRIP.update(10, { 0, 255, 0 });
  });

  BLEMIDI_TRANSPORT.setHandleDisconnected([]() {
    BLE_MIDI_IS_CONNECTED = false;
    // rgbLedWrite(RGB_BUILTIN, 255, 0, 0);
    BUTTON_STRIP.update(10, { 255, 0, 0 });
  });

  // Begin Wire
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);

  ADSManager.begin();  // Begin ADS Manager

  // initialize LCD Screen
  screen.init();
  screen.printHome();

  encoder.init();  // initialize encoder

  analogReadResolution(12);  // Set analog read resolution to 12 bits

  initButtons();  // Initialize Button manager and buttons
  // menuController_begin();  // new — encoder.begin() + menuScreen.begin() (Wire.begin(21,47))

  // ExpressionPedal.init();
}