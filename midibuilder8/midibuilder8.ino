// WORKING ALL KEYS NOTEON, NOTE OFF, VELOCITY SENSITIVE
// ALL KEYS, 3 MUXS, PEDAL
#include <avr/wdt.h>
#include "Keys.h"
#include "DamperPedal.h"
#include "PitchWheel.h"
#include "General.h"

// ============================  PROGRAM VARIABLES  ===========================
uint8_t cycleCount = 0;
// ============================================================================


// =============================  KEYS VARIABLES  =============================

// ============================================================================


void setup() {
  // put your setup code here, to run once:
  wdt_enable(WDTO_250MS);
  // Serial.begin(115200);
  setSustainPinMode();
}

void loop() {
  // Reset watchdog timer
  wdt_reset();

  // ================================  READ THROUGH KEYS  ===================================
  updateKeys();
  // ========================================================================================

  // ==============================  UPDATE SPECIAL UNITS  ==================================
  checkForSustain();
  updatePitchWheel();
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

  // ==========================  UPDATE TOUCH SENSOR REANINGS  ==============================
  updateTouchSensors();
  // ========================================================================================

  // ==============================  READ THROUGH BUTTONS  ==================================

  // ========================================================================================
}
