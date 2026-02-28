#ifndef MUXHELPER_H
#define MUXHELPER_H

#include "Multiplexer.h"

Mux Mux1(15, 14, 16, 10, A0, DIGITAL, INPUT_PULLUP);  // Mux 1 (Digital INPUT_PULLUP (keys[COLS] [0-7]) (buttons[read] [8-15])) digital
Mux Mux2(9, 8, 7, 6, A1, DIGITAL, OUTPUT);             // Mux 2 (Outputs (keys), KPS AND KPE (rows))
Mux Mux3(5, 4, 3, 2, A2, DIGITAL, OUTPUT);             // Mux 3 (Digital output of buttons)
Mux Mux4(5, 4, 3, 2, A2, ANALOG, INPUT);               // Mux 4 (Analog Input, Knobs and Faders)

#endif