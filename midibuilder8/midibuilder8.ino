// WORKING ALL KEYS NOTEON, NOTE OFF, VELOCITY SENSITIVE
// ALL KEYS, 3 MUXS, PEDAL
#include <MIDIUSB.h>
#include <avr/wdt.h>

// =================== MUX VARIABLES =======================
// Mux 1 (Outputs (keys), KPS AND KPE (rows))
#define S10 15
#define S11 14
#define S12 16
#define S13 10
#define signal A0

// Mux 2 (Inputs (keys) (columns)) digital
#define S20 9
#define S21 8
#define S22 7
#define S23 6
#define signal2 A1

// Mux 3 (Analog input for Potentiometers and other control changes like damper pedal)
#define S30 5
#define S31 4
#define S32 3
#define S33 2
#define signal3 A2
// ===========================================================


// ==============================  KEYS VARIABLES  =====================================
// KEYSCAN MATRIX VARIABLES
const uint8_t COL_NUM = 8;
const uint8_t ROW_NUM = 8;

uint8_t cols[COL_NUM] = { 0, 1, 2, 3, 4, 5, 6, 7 };       // Blue cols (Mux2 0 - 7) input_pullup
uint8_t KPS[ROW_NUM] = { 0, 1, 2, 3, 4, 5, 6, 7 };        // Brown rows (Mux1 0 - 7), output
uint8_t KPE[ROW_NUM] = { 8, 9, 10, 11, 12, 13, 14, 15 };  // White rows (Mux1 8 - 15), output

enum ErrorCodes {
  ERROR1,  // kpe before kps
  ERROR2,  // kpe takes too long to read after kps (10s) - timing sanity check
  ERROR3
};


// Array to keep track of previous states of kps and kpe data for all keys
uint8_t pState[2][ROW_NUM][COL_NUM] = { 0 };  // pState[2] for kps[x][y] and kpe[x][y]
uint8_t temp;                                 // variable for temporary storage
// Arrays to keep track of present states of kps and kpe data for all keys
bool kps[ROW_NUM][COL_NUM] = { 0 };
bool kpe[ROW_NUM][COL_NUM] = { 0 };

enum KeyState {
  KEY_IDLE,
  KEY_HALF_PRESSED,
  KEY_FULL_PRESSED,
  KEY_RELEASING,
  KEY_ERROR
};

KeyState keyState[ROW_NUM][COL_NUM] = { KEY_IDLE };

// TIMER VARIABLES
unsigned long timer[2][ROW_NUM][COL_NUM] = { 0 };  // timer[2] for kps[x][y] and kpe[x][y]
int time;
//  ===========================================================================


// ============================  MIDI VARIABLES  =============================
// Channel chan;
const uint8_t channel = 0;  // chan.get();
uint8_t note, vel, velocity;
uint8_t vel_min = 0;
uint8_t vel_max = 45;


const uint8_t PROGMEM nums[ROW_NUM][COL_NUM] = {
  // Array  of midi note numbers C1 (24) to D#6 (87), 64 notes in total.
  { 24, 25, 26, 27, 28, 29, 30, 31 },
  { 32, 33, 34, 35, 36, 37, 38, 39 },
  { 40, 41, 42, 43, 44, 45, 46, 47 },
  { 48, 49, 50, 51, 52, 53, 54, 55 },
  { 56, 57, 58, 59, 60, 61, 62, 63 },
  { 64, 65, 66, 67, 68, 69, 70, 71 },
  { 72, 73, 74, 75, 76, 77, 78, 79 },
  { 80, 81, 82, 83, 84, 85, 86, 87 },
};

// Transpose transposer(0, 24, -24);
// int transpose = transposer.get();

uint8_t transpose = 12;
// ==========================================================


// ===================  POTENTIOMETER VARIABLES  =======================

// Global Analog Input Variables
const uint8_t N_ANALOGS = 5;
uint8_t analogPins[N_ANALOGS] = { 0, 1, 2, 3, 4 };  // (Mux3 0 - 7) input_pullup

// Potentiometer Variables
const uint8_t N_POTS = 5;
uint8_t potPin[N_POTS] = { 0, 1, 2, 3, 4 };  // (Mux3 0 - 7) input_pullup
uint8_t potCC[N_POTS] = { 27, 26, 25, 24, 3 };

int potReading[N_POTS] = { 0 };
int potState[N_POTS] = { 0 };
int potPState[N_POTS] = { 0 };

uint8_t midiState[N_POTS] = { 0 };
uint8_t midiPState[N_POTS] = { 0 };

byte potThreshold = 15;
const int POT_TIMEOUT = 300;
unsigned long pPotTime[N_POTS] = { 0 };
unsigned long potTimer[N_POTS] = { 0 };

// Wheel Variables
uint8_t wheel = 5;  // (Mux3, ch5)
int wheelMin = 510;
int wheelMax = 785;
int wheelMid = ceil((wheelMin + wheelMax) / 2);
int wheelReading = 0;
byte wheelThreshold = 2;
int wheelState, wheelPrevState;
int pitchState, pitchPrevState;
uint8_t wheelCCState, wheelCCPrevState;

// =====================  SUSTAIN PEDAL VARIABLES  =========================
uint8_t sustainPin = 6;  // Mux3, ch7
uint8_t susState = 0;
uint8_t susPrevState = 0;

// Transpose transpose;


void setup() {
  // put your setup code here, to run once:
  wdt_enable(WDTO_250MS);

  pinMode(S10, OUTPUT);
  pinMode(S11, OUTPUT);
  pinMode(S12, OUTPUT);
  pinMode(S13, OUTPUT);
  pinMode(signal, OUTPUT);
  digitalWrite(signal, HIGH);

  pinMode(S20, OUTPUT);
  pinMode(S21, OUTPUT);
  pinMode(S22, OUTPUT);
  pinMode(S23, OUTPUT);
  pinMode(signal2, INPUT_PULLUP);

  pinMode(S30, OUTPUT);
  pinMode(S31, OUTPUT);
  pinMode(S32, OUTPUT);
  pinMode(S33, OUTPUT);
  pinMode(signal3, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop() {
  // Reset watchdog timer
  wdt_reset();

  // ==============================  READ THROUGH THE KEYS  ===============================
  for (uint8_t y = 0; y < COL_NUM; y++) {

    for (uint8_t x = 0; x < ROW_NUM; x++) {

      note = pgm_read_byte(&nums[x][y]) + transpose;

      // if (keyState[x][y] == (KEY_IDLE or KEY_HALF_PRESSED))
      if ((keyState[x][y] == KEY_IDLE) || (keyState[x][y] == KEY_HALF_PRESSED)) {

        // Shift mux to Keypress-start (KPS) channel and read the digital input of note[x][y]
        mux_ch(KPS[x]);
        digitalWrite(signal, LOW);
        mux2_ch(cols[y]);
        temp = !digitalRead(signal2);
        digitalWrite(signal, HIGH);

        if (temp != pState[0][x][y]) {
          timer[0][x][y] = (temp == 1) ? millis() : 0;
          kps[x][y] = temp;
          pState[0][x][y] = temp;
        }

        // Shift mux to Keypress-end (KPE) channel and read the digital input of note[x][y]
        mux_ch(KPE[x]);
        digitalWrite(signal, LOW);
        mux2_ch(cols[y]);
        temp = !digitalRead(signal2);
        digitalWrite(signal, HIGH);

        if (temp != pState[1][x][y]) {
          timer[1][x][y] = (temp == 1) ? millis() : 0;
          kpe[x][y] = temp;
          pState[1][x][y] = temp;
        }
      }

      if ((kps[x][y] == 1) && (kpe[x][y] == 0)) {
        keyState[x][y] = KEY_HALF_PRESSED;
      } else if ((kps[x][y] == 1) && (kpe[x][y] == 1)) {
        keyState[x][y] = KEY_FULL_PRESSED;
      } else if ((kps[x][y] == 0) && (kpe[x][y] == 1)) {
        keyState[x][y] = KEY_ERROR;
        Serial.println("Error with key " + String(note - 23) + ". kpe before kps");
        // Remember to state the error code, log the error and increment the error counter.
      }

      // Sends a noteOn midi message when keypress is complete
      if (keyState[x][y] == KEY_FULL_PRESSED) {
        time = abs(int(timer[1][x][y] - timer[0][x][y]));
        Serial.println(time);
        velocity = map(constrain(time, vel_min, vel_max), vel_max, vel_min, 5, 127);
        noteOn(0, note, velocity);
        keyState[x][y] = KEY_RELEASING;
      }


      if (keyState[x][y] == KEY_RELEASING) {

        mux_ch(KPS[x]);
        digitalWrite(signal, LOW);
        mux2_ch(cols[y]);
        kps[x][y] = !digitalRead(signal2);

        mux_ch(KPE[x]);
        digitalWrite(signal, LOW);
        mux2_ch(cols[y]);
        kpe[x][y] = !digitalRead(signal2);

        digitalWrite(signal, HIGH);
        if (!kps[x][y] && !kpe[x][y]) {
          noteOff(0, note, velocity);
          keyState[x][y] = KEY_IDLE;
        }
      }
    }
  }
  // =======================================================================================


  // ============  READ THROUGH ALL POTS MINUS PITCH AND MOD WHEELS  =====================
  for (uint8_t i = 0; i < N_POTS; i++) {

    mux3_ch(potPin[i]);
    potReading[i] = analogRead(signal3);
    potState[i] = potReading[i];
    midiState[i] = map(potState[i], 0, 1023, 127, 0);

    int potVar = abs(potState[i] - potPState[i]);

    if (potVar > potThreshold) {
      pPotTime[i] = millis();
    }

    potTimer[i] = millis() - pPotTime[i];

    if (potTimer[i] < POT_TIMEOUT) {
      if (midiState[i] != midiPState[i]) {
        controlChange(channel, potCC[i], midiState[i]);
        midiPState[i] = midiState[i];
      }
      potPState[i] = potState[i];
    }
  }
  // ========================================================================================


  // // Modulation Wheel (Joystick Y)
  // // =========================================================
  // // The Modulation Wheel has to be written separately,
  // // because of the difference in the range of values
  // // - 127 to 127, precisely.

  // mux3_ch(modWheel);
  // int modReading = analogRead(signal3);
  // modState = modReading;
  // modMidiState = map(modReading, 0, 1023, -127, 127);
  // int modVar = abs(modState - modPrevState);

  // if (modVar > potThreshold) {
  //   pModTime = millis();
  // }

  // modTimer = millis() - pModTime;

  // if (modTimer < POT_TIMEOUT) {
  //   if (modMidiState != modMidiPState) {
  //     if (modMidiState >= 0) {
  //       // Send Modulation coarse (CC 1)
  //       controlChange(channel, 1, modMidiState);
  //     } else {
  //       // Send modulationm LSB fine/smooth (CC 33)
  //       controlChange(channel, 33, abs(modMidiState));
  //     }
  //     modMidiPState = modMidiState;
  //   }
  //   modPrevState = modState;
  // }
  // //==========================================================



  // Pitch Wheel (Joystick X)
  //=========================================================
  PitchMod();
  //==========================================================


  // Sustain Pedal
  //=========================================================
  mux3_ch(sustainPin);
  bool susRead = !digitalRead(signal3);
  susState = map(susRead, 0, 1, 0, 127);

  if (susState != susPrevState) {
    controlChange(channel, 64, susState);
    susPrevState = susState;
  }
  //==========================================================
}


void mux_ch(uint8_t channel) {
  digitalWrite(S10, channel & 0x01);
  digitalWrite(S11, (channel >> 1) & 0x01);
  digitalWrite(S12, (channel >> 2) & 0x01);
  digitalWrite(S13, (channel >> 3) & 0x01);
  delayMicroseconds(50);
}

void mux2_ch(uint8_t channel) {
  digitalWrite(S20, channel & 0x01);
  digitalWrite(S21, (channel >> 1) & 0x01);
  digitalWrite(S22, (channel >> 2) & 0x01);
  digitalWrite(S23, (channel >> 3) & 0x01);
  delayMicroseconds(50);
}

void mux3_ch(uint8_t channel) {
  digitalWrite(S30, channel & 0x01);
  digitalWrite(S31, (channel >> 1) & 0x01);
  digitalWrite(S32, (channel >> 2) & 0x01);
  digitalWrite(S33, (channel >> 3) & 0x01);
  delayMicroseconds(50);
}

void noteOn(byte channel, byte note, byte velocity) {
  midiEventPacket_t event = { 0x09, 0x90 | channel, note, velocity };
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

void noteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
  midiEventPacket_t event = { 0x08, 0x80 | channel, note, velocity };
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

void controlChange(uint8_t channel, uint8_t control, uint8_t value) {
  midiEventPacket_t event = { 0x0B, 0xB0 | channel, control, value };
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

void pitchBend(uint8_t channel, int value) {
  midiEventPacket_t event = { 0x0E, 0xE0 | channel, value & 0x7F, (value >> 7) & 0x7F };
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

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

int findIndex(int arr[], int size, int target) {
  for (int i = 0; i < size; i++) {
    if (arr[i] == target) {
      return i;  // Return the index if a match is found.
    }
  }

  return -1;  // Return =1 if the target is not found in the array.
}
