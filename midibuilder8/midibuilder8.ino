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

// ============================================================================
Keys keys;
Pitch_Wheel PitchWheel;
Damper_Pedal DamperPedal = { 12 };
// ============================================================================

void setup() {
  Setup();
}

void loop() {

  // Serial.println("Mainloop runnning!");  // DEBUGGER

  linear_stepper_forward_backward(r, g, b, steps, timeon, timeoff);  // Perform is_running sequence indicator
  keys.updateKeys();                                                 // read through keys
  DamperPedal.update();
  scanButtons();
  static uint8_t ADSCycleCount = 0;
  // ============================  ADS1115 READS EVERY N CYCLES  ============================
  if (ADSCycleCount == 0) {
    PitchWheel.update();
    Slider.update();
  }
  ADSCycleCount++;
  if (ADSCycleCount >= 3) ADSCycleCount = 0;  // Reset cycle count every nth cycle
  // ========================================================================================


  linear_stepper_forward_backward(r, g, b, steps, timeon, timeoff);  // Perform is_running sequence indicator
  keys.updateKeys();                                                 // read through keys
  // ======================  UPDATE SCREEN BASED ON ENCODER VALUES  =========================
  encoder.updateScreenValues();
  // menuController_update();  // new — polls the encoder, moves selection / edits value
  // ========================================================================================

  static uint8_t knobCycleCount = 0;
  // ============  READ THROUGH ALL KNOBS AND FADERS ON MUX4 EVERY N CYCLES  ================
  if (knobCycleCount == 0) updateKnobs();
  knobCycleCount++;
  if (knobCycleCount >= 5) knobCycleCount = 0;  // Reset cycle count every nth cycle
  // ExpressionPedal.update();
  // ========================================================================================


  // ========================================================================================
  // joystick.update();
  // updateTouchSensors();
  // ========================================================================================
}

#endif /* ARDUINO_USB_MODE */
