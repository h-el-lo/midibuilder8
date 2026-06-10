#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode

void setup() {}
void loop() {}

#else

// 64 KEYS, VELOCITY SENSITIVE, 4 MUXS, SUSTAIN PEDAL, EXPRESSION PEDAL, 16 KNOBS, 46 BUTTONS, PITCHWHEEL, SLIDER(MODWHEEL)

// #include <avr/wdt.h>
#include "ADSManager.h"
#include "Keys.h"
#include "DamperPedal.h"
#include "ExpressionPedal.h"
// #include "PitchWheel.h"
#include "General.h"

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

  // wdt_enable(WDTO_250MS);
  USB.begin();
  usbmidi.begin();

  // Begin ADS Manager
  ADSManager.begin();
  attachInterrupt(digitalPinToInterrupt(ADS_ALRT_MCU_PIN), onConvReady, FALLING);


  // initialize LCD Screen
  // lcd.init();

  ExpressionPedal.init();


  // delay(2000);  //WDT Stabilization delay
}

void loop() {
  // Reset watchdog timer
  // wdt_reset();

  // ================================  READ THROUGH KEYS  ===================================
  keys.updateKeys();
  // ========================================================================================

  // ==============================  UPDATE SPECIAL UNITS  ==================================
  DamperPedal.update();
  ExpressionPedal.update();
  // updatePitchWheel();
  Slider.update();
  // joystick.update();
  // ========================================================================================

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

  // ==========================  UPDATE TOUCH SENSOR READINGS  ==============================
  // updateTouchSensors();
  // ========================================================================================

  // ==============================  READ THROUGH BUTTONS  ==================================

  // ========================================================================================
}

#endif
