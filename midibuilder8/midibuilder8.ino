#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else

// 64 KEYS, VELOCITY SENSITIVE, 4 MUXS, SUSTAIN PEDAL, EXPRESSION PEDAL, 16 KNOBS, 46 BUTTONS, PITCHWHEEL, SLIDER(MODWHEEL)

#include "ADSManager.h"
#include "General.h"
#include "DamperPedal.h"
// #include "ExpressionPedal.h"
// #include "PitchWheel.h"
#include "Buttons.h"
#include "RGB.h"

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
  Serial.begin(921600);

  // Begin MIDI
  usbmidi.begin();
  USB.begin();

  Serial.println("Hello, world!");
  delay(2000);

  // Set analog read resolution to 12 bits
  analogReadResolution(12);

  // Begin ADS Manager
  ADSManager.begin();
  attachInterrupt(digitalPinToInterrupt(ADS_ALRT_MCU_PIN), onConvReady, FALLING);

  // initialize LCD Screen
  // screen.init();

  // Initialize Button manager and buttons
  BUTTON_STRIP.begin();
  // INDICATOR_STRIP.begin();
  initButtons();



  // ExpressionPedal.init();

  // Serial.print("Mux1 mode is ");
  // Serial.println(Mux1.getMode());
  // Serial.print("Mux2 mode is ");
  // Serial.println(Mux2.getMode());
  // Serial.print("Mux3 mode is ");
  // Serial.println(Mux3.getMode());
  // Serial.print("Mux4 mode is ");
  // Serial.println(Mux4.getMode());

  Serial.println("Let's get started!");
  delay(2000);
}

void loop() {
  // Reset watchdog timer
  // wdt_reset();

  // Serial.println("Mainloop runnning!");

  // ================================  READ THROUGH KEYS  ===================================
  keys.updateKeys();
  // ========================================================================================

  // ==============================  UPDATE SPECIAL UNITS  ==================================
  DamperPedal.update();
  // ExpressionPedal.update();
  // updatePitchWheel();
  Slider.update();
  // joystick.update();
  // // ========================================================================================

  // ============  READ THROUGH ALL KNOBS AND FADERS ON MUX4 EVERY 5 CYCLES  ================
  if (cycleCount == 0) {
    updateKnobs();
  }
  cycleCount++;

  // Reset cycle count
  if (cycleCount >= 5) {
    cycleCount = 0;
  }
  // ========================================================================================

  // ==============================  READ THROUGH BUTTONS  ==================================
  scanButtons();
  // ========================================================================================

  // ==========================  UPDATE TOUCH SENSOR READINGS  ==============================
  // updateTouchSensors();
  // ========================================================================================
}

#endif /* ARDUINO_USB_MODE */
