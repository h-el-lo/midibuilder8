#pragma once

// Owns the settings-menu state machine, the encoder, and the menu LCD.
//
// Wiring:
//  - setup(): call menuController_begin() (after Wire/I2C-owning code, if any)
//  - loop():  call menuController_update() every iteration (polls the encoder)
//  - Buttons.cpp initButtons(): the Settings/Enter/Exit RGBActionButton
//    entries point their callback at the three onXPressed() functions below.
//    Button debounce/edge-detection is already handled by Button::read(),
//    so these functions ARE the press handlers -- no extra polling needed.

void menuController_begin();
void menuController_update();

void menuController_onSettingsPressed();
void menuController_onEnterPressed();
void menuController_onExitPressed();

// Explicit "abandon history, go home" -- wire to a long-press or idle
// timeout if desired. Not used by Settings/Enter/Exit themselves, since
// those must preserve history per the spec.
void menuController_goHomeExplicit();
