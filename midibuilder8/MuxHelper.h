#pragma once

#include "Multiplexer.h"

extern Mux Mux1;  // Mux 1 (Digital INPUT_PULLUP (keys[COLS] [0-7]) (buttons[read] [8-15])) digital
extern Mux Mux2;  // Mux 2 (Outputs (keys), KPS AND KPE (rows))
extern Mux Mux3;  // Mux 3 (Digital output of buttons)
extern Mux Mux4;  // Mux 4 (Analog Input, Knobs and Faders)
