#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else

// 64 KEYS, VELOCITY SENSITIVE, 4 MUXS, SUSTAIN PEDAL, EXPRESSION PEDAL, 16 KNOBS, 46 BUTTONS, PITCHWHEEL, SLIDER(MODWHEEL)

#include "Keys.h"
#include "DamperPedal.h"
#include "PitchWheel.h"
// #include "ExpressionPedal.h"

#include "General.h"
#include "Setup.h"

// ============================  PROGRAM VARIABLES  ===========================
uint8_t cycleCount = 0;
// ============================================================================

// ===============================  KEYS  ==================================
Keys keys;
// =========================================================================

// =============================  PITCH WHEEL  ================================
Pitch_Wheel PitchWheel;
// ============================================================================

void setup() {
  Setup();
}

void loop() {

  // Serial.println("Mainloop runnning!"); // DEBUGGER

  // ==============================  PERFORM RGB LIGHTING  ===============================
  linear_stepper_forward_backward(r, g, b, steps, timeon, timeoff);
  // =====================================================================================

  // ================================  READ THROUGH KEYS  ===================================
  keys.updateKeys();
  // ========================================================================================

  // ==============================  UPDATE SPECIAL UNITS  ==================================
  DamperPedal.update();
  PitchWheel.update();
  Slider.update();
  // ExpressionPedal.update();
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

  // ==============================  READ THROUGH BUTTONS  ==================================
  scanButtons();
  // ========================================================================================

  // ======================  UPDATE SCREEN BASED ON ENCODER VALUES  =========================
  // encoder.updateScreenValues();
  menuController_update();  // new — polls the encoder, moves selection / edits value
  // ========================================================================================

  // ==========================  UPDATE TOUCH SENSOR READINGS  ==============================
  // updateTouchSensors();
  // ========================================================================================
}

#endif /* ARDUINO_USB_MODE */
