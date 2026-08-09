#pragma once

#include "ADSManager.h"
#include "MIDIHelper.h"
// #include "Screen.h"

#include <Wire.h>
#define SDA_PIN 21
#define SCL_PIN 47

#include "RGB.h"
#include "Flasher.h"
#include "Encoder.h"
#include "Buttons.h"

#include "MenuController.h"


inline void Setup() {
  // put your setup code here, to run once:
  Serial.begin(921600);

  // Initialize Button manager and buttons
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
    // digitalWrite(LED_BUILTIN, HIGH);
    // rgbLedWrite(RGB_BUILTIN, 0, 255, 0);
    BUTTON_STRIP.update(10, { 0, 255, 0 });
  });

  BLEMIDI_TRANSPORT.setHandleDisconnected([]() {
    BLE_MIDI_IS_CONNECTED = false;
    // digitalWrite(LED_BUILTIN, LOW);
    // rgbLedWrite(RGB_BUILTIN, 255, 0, 0);
    BUTTON_STRIP.update(10, { 255, 0, 0 });
  });
  // BUTTON_STRIP.update(11, { 75, 155, 214 });
  // BUTTON_STRIP.update(12, { 75, 155, 214 });
  // BUTTON_STRIP.update(13, { 75, 155, 214 });
  // BUTTON_STRIP.update(14, { 255, 0, 0 });
  // BUTTON_STRIP.update(15, { 75, 155, 214 });

  // Begin Wire
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);

  // Begin ADS Manager
  ADSManager.begin();



  // // initialize LCD Screen
  // screen.init();
  // screen.printHome();

  // // initialize encoder
  // encoder.initializeEncoder();

  // Set analog read resolution to 12 bits
  analogReadResolution(12);

  initButtons();
  menuController_begin();  // new — encoder.begin() + menuScreen.begin() (Wire.begin(21,47))

  // ExpressionPedal.init();

  // Serial.print("Mux1 mode is ");
  // Serial.println(Mux1.getMode());
  // Serial.print("Mux2 mode is ");
  // Serial.println(Mux2.getMode());
  // Serial.print("Mux3 mode is ");
  // Serial.println(Mux3.getMode());
  // Serial.print("Mux4 mode is ");
  // Serial.println(Mux4.getMode());

  // Serial.println("Let's get started!");
  // delay(2000);
}