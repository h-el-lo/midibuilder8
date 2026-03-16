// 64 KEYS, VELOCITY SENSITIVE, 4 MUXS, SUSTAIN PEDAL, EXPRESSION PEDAL, 16 KNOBS

// #include <avr/wdt.h>
#include "Keys.h"
#include "DamperPedal.h"
#include "PitchWheel.h"
#include "General.h"

// ============================  PROGRAM VARIABLES  ===========================
uint8_t cycleCount = 0;
// ============================================================================

// =============================  KEYS VARIABLES  =============================

// ============================================================================

#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode

void setup() {}
void loop() {}

#else

void setup() {
  // put your setup code here, to run once:
  // wdt_enable(WDTO_250MS);
  // Serial.begin(115200);
  setSustainPinMode();
}

void loop() {
  // Reset watchdog timer
  // wdt_reset();

  // ================================  READ THROUGH KEYS  ===================================
  updateKeys();
  // ========================================================================================

  // ==============================  UPDATE SPECIAL UNITS  ==================================
  checkForSustain();
  updatePitchWheel();
  slider.update();
  joystick.update();
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

#endif 





// mux 1 (Reader)
// gpio 4, 5, 6, 7, 15             DONE

// mux 4 (Knobs and faders)
// gpio 16, 17, 18, 8, 3             DONE

// expression pedal
// connection checker pin gpio 46
// adc reader pin gpio 9

// pitch wheel gpio
// 10                                DONE




// joystick xy,
// gpio 1, 2 adc1ch0, ch1

// sustain pedal
// gpio 42                           DONE

// LCD Screen sda, scl
// gpio 41, 40

// mux 2 (keys)
// gpio 39, 38, 37, 36, 35           DONE

// mux3 (buttons)
// gpio 0, 45, 48, 47, 21            DONE
