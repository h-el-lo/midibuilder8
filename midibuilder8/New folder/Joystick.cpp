#include "Joystick.h"
#include "MIDIHelper.h"

void PitchMod() {
  mux3_ch(wheel);
  int wheelReading = analogRead(signal3);
  wheelState = constrain(wheelReading, wheelMin, wheelMax);
  int wheelVar = abs(wheelState - wheelPrevState);

  if (wheelVar > wheelThreshold) {
    if (wheelState != wheelPrevState) {

      if (wheelState < wheelMid) {
        // If wheel is in the lower section
        pitchState = map(wheelState, wheelMin, wheelMid, 0, 8191);
        if (pitchState > 7700) {
          pitchState = 8191;
        }
        if (pitchState != pitchPrevState) {
          pitchBend(channel, pitchState);
          pitchPrevState = pitchState;
        }
        wheelPrevState = wheelState;
      } else if (wheelState > wheelMid) {
        // if wheel is in the upper section
        wheelCCState = map(wheelState, wheelMid, wheelMax, 0, 127);
        if (wheelCCState < 23) {
          wheelCCState = 0;
        } else if (wheelCCState >= 124) {
          wheelCCState = 127;
        }

        if (wheelCCState != wheelCCPrevState) {
          controlChange(channel, 1, wheelCCState);  // Modulation Wheel
          wheelCCPrevState = wheelCCState;
        }

        wheelPrevState = wheelState;
      }
    }
  }
}