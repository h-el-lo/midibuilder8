#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode

void setup() {}
void loop() {}

#else

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN 14

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 29

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);




// 64 KEYS, VELOCITY SENSITIVE, 4 MUXS, SUSTAIN PEDAL, EXPRESSION PEDAL, 16 KNOBS

// #include <avr/wdt.h>
#include "Keys.h"
#include "DamperPedal.h"
// #include "ExpressionPedal.h"
// #include "PitchWheel.h"
#include "General.h"

// ============================  PROGRAM VARIABLES  ===========================
uint8_t cycleCount = 0;
// ============================================================================

// =============================  KEYS VARIABLES  =============================

// ============================================================================







void setupmisc() {

  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.
  Wire.begin(21, 47);

  lcd.init();  // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Hello, world!");
  lcd.setCursor(2, 1);
  lcd.print("Ywrobot Arduino!");
  lcd.setCursor(0, 2);
  lcd.print("Arduino LCM IIC 2004");
  lcd.setCursor(2, 3);
  lcd.print("Power By Ec-yuan!");

  strip.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();             // Turn OFF all pixels ASAP
  strip.setBrightness(80);  // Set BRIGHTNESS to about 1/5 (max = 255)



  // strip.setPixelColor(i, strip.Color(255, 0, 125));  //  Set pixel's color (in RAM)
  // strip.setPixelColor(i, strip.Color(0, 0, 200));  //  Set pixel's color (in RAM)


  // (0, 150, 255);    //  Set to bluish cyan
  // (255, 0, 125);    //  Set to bright magenta
  // (255, 50, 0);     //  Set to orange
  // (255, 150, 255);  //  Set to rose gold
  // (100, 150, 150);  //  Set to grey
  // (30, 0, 110);     //  Set to indigo
  // (170, 170, 170);  //  Set to white-ish
  // (10, 10, 255);    //  Set to blue-ish


  strip.setPixelColor(0, strip.Color(0, 255, 0));
  strip.show();
  delay(20);
  strip.setPixelColor(1, strip.Color(255, 255, 0));
  strip.show();
  delay(20);
  strip.setPixelColor(2, strip.Color(255, 0, 255));
  strip.show();
  delay(20);
  strip.setPixelColor(3, strip.Color(255, 255, 255));
  strip.show();
  delay(20);


  strip.setPixelColor(4, strip.Color(0, 255, 0));  //  Set to green-ish
  strip.show();
  delay(20);


  strip.setPixelColor(5, strip.Color(255, 150, 255));  //  Set pixel's color (in RAM)
  strip.show();
  delay(20);
  strip.setPixelColor(6, strip.Color(100, 150, 150));  //  Set to rose gold
  strip.show();
  delay(20);


  strip.setPixelColor(7, strip.Color(10, 10, 255));  //  Set to cyan
  strip.show();
  delay(20);
  strip.setPixelColor(8, strip.Color(255, 255, 0));  //  Set to yellow white (cream)
  strip.show();
  delay(20);
  strip.setPixelColor(9, strip.Color(90, 15, 0));  //  Set to yellow white (cream)
  strip.show();
  delay(20);


  for (int i = 10; i < 16; i++) {  // For each pixel in strip...
    strip.setPixelColor(i, strip.Color(255, 235, 200));
    // strip.setPixelColor(i, strip.Color(10, 10, 255));
    // strip.setPixelColor(i, strip.Color(255, 255, 0));
    // strip.setPixelColor(i, strip.Color(255, 230, 195));
    // strip.setPixelColor(i, strip.Color(255, 200, 180));
    // strip.setPixelColor(i, strip.Color(100, 150, 150));
    strip.show();  //  Update strip to match
    delay(20);     //  Pause for a moment
  }

  strip.setPixelColor(16, strip.Color(170, 170, 170));  //  Set pixel's color (in RAM)
  // strip.setPixelColor(21, strip.Color(255, 150, 255));  //  Set pixel's color (in RAM)
  strip.show();
  delay(20);

  for (int i = 17; i < 21; i++) {  // For each pixel in strip...
    // strip.setPixelColor(i, strip.Color(255, 150, 255));  //  Set pixel's color (in RAM)
    // strip.setPixelColor(i, strip.Color(190, 200, 200));  //  Set pixel's color (in RAM)
    // strip.setPixelColor(i, strip.Color(100, 150, 150));  //  Set pixel's color (in RAM)
    strip.setPixelColor(i, strip.Color(170, 170, 170));  //  Set pixel's color (in RAM)
    strip.show();                                        //  Update strip to match
    delay(20);                                           //  Pause for a moment
  }


  for (int i = 21; i < 23; i++) {                      // For each pixel in strip...
    strip.setPixelColor(i, strip.Color(0, 150, 255));  //  Set to bluish cyan
    // strip.setPixelColor(i, strip.Color(255, 0, 125));  //  Set to bright magenta
    // strip.setPixelColor(i, strip.Color(255, 50, 0));  //  Set to orange
    strip.show();  //  Update strip to match
    delay(20);     //  Pause for a moment
  }


  for (int i = 23; i < 25; i++) {                      // For each pixel in strip...
    strip.setPixelColor(i, strip.Color(10, 10, 255));  //  Set to bluish cyan
    // strip.setPixelColor(i, strip.Color(255, 0, 125));  //  Set to bright magenta
    // strip.setPixelColor(i, strip.Color(255, 50, 0));  //  Set to orange
    strip.show();  //  Update strip to match
    delay(20);     //  Pause for a moment
  }


  for (int i = 25; i < 27; i++) {  // For each pixel in strip...
    // strip.setPixelColor(i, strip.Color(0, 150, 255));  //  Set to bluish cyan
    strip.setPixelColor(i, strip.Color(255, 0, 125));  //  Set to bright magenta
    // strip.setPixelColor(i, strip.Color(255, 50, 0));  //  Set to orange
    strip.show();  //  Update strip to match
    delay(20);     //  Pause for a moment
  }


  for (int i = 27; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(30, 0, 110));  //  Set to indigo
    strip.show();                                     //  Update strip to match
    delay(20);                                        //  Pause for a moment
  }


  delay(200);
}



void setup() {
  setupmisc();


  // put your setup code here, to run once:
  // wdt_enable(WDTO_250MS);
  USB.begin();
  usbmidi.begin();
  // Serial.begin(115200);
  INITIALIZE_SUSTAIN_PEDAL();
  // INITIALIZE_EXPRESSION_PEDAL();
  // delay(2000);  //WDT Stabilization delay
}

void loop() {
  // Reset watchdog timer
  // wdt_reset();

  // ================================  READ THROUGH KEYS  ===================================
  updateKeys();
  // ========================================================================================

  // ==============================  UPDATE SPECIAL UNITS  ==================================
  updateSustainPedal();
  // updateExpressionPedal();
  // updatePitchWheel();
  // slider.update();
  // joystick.update();
  // ========================================================================================

  // ============  READ THROUGH ALL KNOBS AND FADERS ON MUX4 EVERY 5 CYCLES  ================
  // if (cycleCount == 0) {
  //   updateKnobs();
  // }
  // cycleCount++;

  // // Reset cycle count
  // if (cycleCount >= 5) {
  //   cycleCount = 0;
  // }
  // ========================================================================================

  // ==========================  UPDATE TOUCH SENSOR READINGS  ==============================
  // updateTouchSensors();
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



// joystick 2
// mux 1,2 10
// mux 3 4
// screen 2
// rgb led 1
// expression pedal checker pin
// damper pedal 1

// ina battery management i2c, screen, ads 1115 2
// ads1115 [mux 3 signal, pitch wheel, slider, expression pedal analog pin]
