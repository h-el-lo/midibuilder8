#include <Arduino.h>
#include "Encoder.h"
#include "Screen.h"
#include "Keys.h"
#include "MIDIHelper.h"

// Define static members
Encoder* Encoder::instance = nullptr;

// Constructors
Encoder::Encoder(uint8_t PIN_A, uint8_t PIN_B) {
  _PIN_A = PIN_A;
  _PIN_B = PIN_B;
}

Encoder::Encoder()
  : Encoder(2, 1) {
}

// This method shall be called in case of change in pin numbering, encoder resolution or rotation
void Encoder::initializeEncoder() {
  // Configure encoder pins as inputs with pull-up resistors
  pinMode(_PIN_A, INPUT_PULLUP);
  pinMode(_PIN_B, INPUT_PULLUP);

  // Attach interrupts for encoder channels/pins
  attachInterrupt(digitalPinToInterrupt(_PIN_A), updateEncoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(_PIN_B), updateEncoderISR, CHANGE);

  // Bind ISR to this instance
  instance = this;
}

// Getters

// Setters

// Methods
void Encoder::updateEncoderISR() {
  if (instance != nullptr) {
    instance->updateEncoder();  // Call the actual member function
  }
}

// Interrupt service routine for encoder - removed artificial limits
void Encoder::updateEncoder() {

  int MSB = digitalRead(_PIN_A);  // Most significant bit
  int LSB = digitalRead(_PIN_B);  // Least significant bit

  int encoded = (MSB << 1) | LSB;           // Convert to single number
  int sum = (_lastEncoded << 2) | encoded;  // Add it to previous encoded value

  // Determine direction based on state changes
  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
    _encoderPos++;
  }
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
    _encoderPos--;
  }

  _lastEncoded = encoded;  // Store this value for next time
  _encoderVal = _encoderPos / 4;
  // Serial.print("Encoder ORIGINAL value: "); // DEBUGGER
  // Serial.println(_encoderPos); // DEBUGGER
}

void Encoder::updateScreenValues() {
  // portDISABLE_INTERRUPTS(); // should be used only within ISRs
  // portENABLE_INTERRUPTS(); // should be used only within ISRs

  noInterrupts();
  int16_t val = _encoderVal;
  int16_t pVal = _prevEncoderVal;
  interrupts();  // should be called for only a very short period, three lines at the most
  // else, the watchdog thinks the program has frozen even after just a few milliseconds and reboots

  // // DEBUGGER
  // Serial.print("Encoder modified value: ");
  // Serial.print(_encoderVal);
  // Serial.print(", Previous encoder modified value: ");
  // Serial.println(_prevEncoderVal);

  if (val > pVal) {
    // if encoder++

    // // DEBUGGER
    // Serial.print("Encoder modified value for plus: ");
    // Serial.print(_encoderVal);
    // Serial.print(", Previous encoder modified value: ");
    // Serial.println(_prevEncoderVal);

    _prevEncoderVal = val;

    if (Screen::_page == Screen::TRANSPOSE) {
      keys.transposeUp();
      // Serial.println("Yellow Jacket Plus.");
    } else if (Screen::_page == Screen::CHANNEL) {
      channelUp();
      // Serial.println("Blue Marlin Plus.");
    }
  } else if (_encoderVal < _prevEncoderVal) {
    // if encoder--

    // // DEBUGGER
    // Serial.print("Encoder modified value for minus: ");
    // Serial.print(_encoderVal);
    // Serial.print(", Previous encoder modified value: ");
    // Serial.println(_prevEncoderVal);

    _prevEncoderVal = val;

    if (Screen::_page == Screen::TRANSPOSE) {
      keys.transposeDown();
      // Serial.println("Yellow Jacket Minus.");
    } else if (Screen::_page == Screen::CHANNEL) {
      channelDown();
      // Serial.println("Blue Marlin Minus.");
    }
  }
};

// ===========================  ENCODER OBJECT  ===============================
Encoder encoder;
// ============================================================================