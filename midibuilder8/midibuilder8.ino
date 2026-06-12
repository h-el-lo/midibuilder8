#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else

// #include "USB.h"
// #include "USBMIDI.h"

// USBMIDI MIDI("SWEETBOX-SYNTHAGE");  // Creates the MIDI device with specific descriptor
// USBMIDI MIDI("ESP MIDI Device");

// 64 KEYS, VELOCITY SENSITIVE, 4 MUXS, SUSTAIN PEDAL, EXPRESSION PEDAL, 16 KNOBS, 46 BUTTONS, PITCHWHEEL, SLIDER(MODWHEEL)

#include "ADSManager.h"
#include "General.h"
#include "DamperPedal.h"
// #include "ExpressionPedal.h"
// #include "PitchWheel.h"
// #include "Buttons.h"

// ============================  PROGRAM VARIABLES  ===========================
uint8_t cycleCount = 0;
// ============================================================================

// =============================  KEYS VARIABLES  =============================

// ============================================================================


void IRAM_ATTR onConvReady() {
  ADSManager.onConvReady();
}

void setup() {
  // put your setup code here, to run once:
  // Serial.begin(921600);

  // Begin MIDI
  usbmidi.begin();
  USB.begin();
  Serial.println("Hello, world!");
  delay(2000);

  // Begin ADS Manager
  ADSManager.begin();
  attachInterrupt(digitalPinToInterrupt(ADS_ALRT_MCU_PIN), onConvReady, FALLING);

  // Initialize Button manager and buttons
  // initButtons();


  // initialize LCD Screen
  // lcd.init();

  // ExpressionPedal.init();

  Serial.println("Let's get started!");
}

void loop() {
  // Reset watchdog timer
  // wdt_reset();

  Serial.println("Mainloop runnning!");

  // ================================  READ THROUGH KEYS  ===================================
  keys.updateKeys();
  // ========================================================================================

  // ==============================  UPDATE SPECIAL UNITS  ==================================
  DamperPedal.update();
  // ExpressionPedal.update();
  // updatePitchWheel();
  // Slider.update();
  // joystick.update();
  // ========================================================================================

  // // ============  READ THROUGH ALL KNOBS AND FADERS ON MUX4 EVERY 5 CYCLES  ================
  // if (cycleCount == 0) {
  //   updateKnobs();
  // }
  // cycleCount++;

  // // Reset cycle count
  // if (cycleCount >= 5) {
  //   cycleCount = 0;
  // }
  // // ========================================================================================

  // ==========================  UPDATE TOUCH SENSOR READINGS  ==============================
  // updateTouchSensors();
  // ========================================================================================

  // ==============================  READ THROUGH BUTTONS  ==================================
  // scanButtons();
  // ========================================================================================
}

#endif /* ARDUINO_USB_MODE */
