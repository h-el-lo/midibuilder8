#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else

// 64 KEYS, VELOCITY SENSITIVE, 4 MUXS, SUSTAIN PEDAL, EXPRESSION PEDAL, 16 KNOBS, 46 BUTTONS, PITCHWHEEL, SLIDER(MODWHEEL)

#include "ADSManager.h"
#include "Screen.h"

#include "Keys.h"
#include "DamperPedal.h"
#include "PitchWheel.h"
// #include "ExpressionPedal.h"

#include "General.h"
#include "Buttons.h"
#include "Encoder.h"
#include "RGB.h"

#include <Wire.h>
#define SDA_PIN 21
#define SCL_PIN 47

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
  // put your setup code here, to run once:
  Serial.begin(921600);

  // Begin MIDI
  usbmidi.begin();
  USB.begin();
  // // allow USB time to initialize correctly
  // delay(2000);

  // Begin Wire
  Wire.begin(21, 47);
  Wire.setClock(400000);

  // Begin ADS Manager
  ADSManager.begin();

  // initialize LCD Screen
  screen.init();
  screen.printHome();

  // initialize encoder
  encoder.initializeEncoder();

  // Set analog read resolution to 12 bits
  analogReadResolution(12);

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

  // Serial.println("Mainloop runnning!"); // DEBUGGER

  // ================================  READ THROUGH KEYS  ===================================
  keys.updateKeys();
  // ========================================================================================

  // ==============================  UPDATE SPECIAL UNITS  ==================================
  DamperPedal.update();
  // PitchWheel.update();
  // Slider.update();
  // ExpressionPedal.update();
  // joystick.update();
  // // ========================================================================================

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

  // ==============================  READ THROUGH BUTTONS  ==================================
  // scanButtons();
  // ========================================================================================

  // ======================  UPDATE SCREEN BASED ON ENCODER VALUES  =========================
  encoder.updateScreenValues();
  // ========================================================================================

  // ==========================  UPDATE TOUCH SENSOR READINGS  ==============================
  // updateTouchSensors();
  // ========================================================================================
}

#endif /* ARDUINO_USB_MODE */
