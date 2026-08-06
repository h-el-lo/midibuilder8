# MIDI Builder 8

A custom MIDI keyboard controller firmware project in active development, evolved from the original ATmega328U4 microcontroller implementation.

## Overview

**MIDI Builder 8** is the latest iteration of a personal MIDI keyboard controller project, written primarily in C++ (65.2%) with supporting C code (34.8%). This repository represents the culmination of multiple development stages, refining hardware abstraction, code organization, and performance from earlier prototypes.

**Status**: 🚧 **In Active Development** - Core functionality working, advanced features and error handling in progress.

## Project Evolution

### Stage 1: ATmega328U4 Foundation
The project originated with the **ATmega328U4 microcontroller**, a common choice for Arduino-based MIDI projects. This initial stage established:
- Basic MIDI protocol implementation
- Key scanning and debouncing logic
- USB communication via the native USB hardware on the ATmega328U4
- Fundamental firmware structure

### Stage 2: Controllerbuilder (Mini MIDI Controller)
The project evolved into **controllerbuilder** (created June 2024), a more polished implementation:
- Focused on a **mini MIDI controller** configuration
- Refined C++ architecture for better code organization
- Enhanced feature set over the original ATmega implementation
- Positioned as a functional, portable MIDI input device

### Stage 3: Controllerbuilder2 (Interim Refinement)
**Controllerbuilder2** represented transitional improvements:
- Further optimization and refinement of the codebase
- Experimental features or hardware variations
- Bridge between controllerbuilder and full-scale implementations

### Stage 4: MIDI Builder 8 (Current - In Development)
- **64-key velocity-sensitive keyboard** (8×8 matrix)
- Modular codebase
- Integration with peripheral controls: knobs, faders, pedals
- LCD display and addressable LED feedback (NeoPixels)

## Comparison with Earlier Projects

| Aspect | controllerbuilder | MIDI Builder 8 |
|--------|------------------|----------------|
| **Scope** | Mini MIDI controller | Full 64-key keyboard |
| **Language** | C++ | C++ (65.2%) + C (34.8%) |
| **Focus** | Compact input device | Extended key layout with peripherals |
| **Maturity** | Established | In active development |
| **Code Organization** | Modular design | Enhanced modularity with state machines |
| **Status** | Stable | Core working, features in progress |

## Current Implementation

### ✅ Working Features
- **Key Matrix**: Velocity-sensitive 64-key (8×8) scanning with state machine handling
- **MIDI Output**: USB MIDI communication
- **Multiplexing**: 4 multiplexers configured for efficient I/O
  - Mux1: Digital input (key columns, buttons)
  - Mux2: Digital output (key rows)
  - Mux3: Digital output (button control)
  - Mux4: Analog input (knobs, faders)
- **Peripheral Support**: 5 configurable knobs/faders with CC control
- **User Interface**: 20×4 LCD display and addressable LED strips (NeoPixels)

## Architecture

### Hardware Setup
- **Microcontroller**: Arduino with USB support (e.g., Arduino Leonardo-compatible)
- **Key Mechanism**: 8×8 velocity-sensitive matrix using dual-switch detection
- **Multiplexing**: 4-channel multiplexers reduce pin count
- **Peripherals**: Analog knobs/faders, digital buttons, LCD display, addressable LEDs

## Technical Details

### Key Features
- **Velocity Sensitivity**: Dual-switch mechanism measures time between switch closures
- **State Tracking**: Advanced state machine detects stuck keys, hardware faults, and timing anomalies
- **Bit Packing**: Efficient memory usage - 8 bytes per 8×8 matrix instead of 64

### MIDI Specification
- **Protocol**: USB MIDI, BLE MIDI
- **Channels**: Configurable (defaults to channel 1)
- **CC Support**: 16+ continuous controllers (knobs/faders)

## Getting Started

### Prerequisites
- Arduino IDE with USB support
- AVR-GCC toolchain
- Target microcontroller (Leonardo, Micro, or equivalent with USB support)
- External libraries:
  - `Adafruit_NeoPixel.h` (LED control)
  - `LiquidCrystal_I2C.h` (LCD display)
  - USBMIDI library

### Hardware Requirements
- Microcontroller with USB and sufficient GPIO
- 64 key switches (velocity-sensitive with dual-point switches)
- 4× 16-channel multiplexers (CD4067 or equivalent)
- 20×4 I2C LCD display
- Addressable LED strip (WS2812B/NeoPixel)
- Knobs/faders for control
- Optional: Sustain pedal, expression pedal

### Installation & Build
1. Clone or download this repository
2. Open `midibuilder8.ino` in Arduino IDE
3. Install required libraries via Library Manager
4. Configure pin mappings in `General.h` and hardware files
5. Compile and upload to target microcontroller

### Configuration
Edit the following files to customize:
- **`Keys.h`**: Key matrix layout, note mappings, state parameters
- **`General.h`**: Mux pin assignments, MIDI channel, knob configuration
- **`Multiplexer.h`**: Mux control pins and mode settings

## Known Limitations & TODOs

### Incomplete Features (See `Future works/` directory)
- [ ] Joystick/XY pad control
- [ ] Pedal integration (currently DamperPedal only)

### Known Issues

## Development Notes

This project demonstrates the evolution from simple single-microcontroller implementations to a sophisticated, modular firmware architecture. The hybrid C++/C approach leverages:
- **C++**: Object-oriented abstractions (Multiplexer, Knob classes) for clean code structure
- **C**: Performance-critical sections and low-level hardware operations

### Design Decisions
- **State Machines**: Detect stuck keys and hardware faults reliably
- **Bit Packing**: Minimize memory footprint for 8×8 matrices
- **Multiplexing**: Reduce GPIO pin count from 16+ to 4 control lines
- **Modular Headers**: Each subsystem isolated in dedicated files for maintainability

## License

Not specified. See the repository for any applicable license terms.

## Contributing

This is a personal project, but improvements, bug reports, or feature suggestions are welcome.

---

**Note**: This is a personal project for custom MIDI keyboard development. MIDI Builder 8 is under active development—core functionality is working, but advanced error handling, validation, and peripheral support are still in progress. Not recommended for production use until error handling and testing are completed.

**Last Updated**: June 3, 2026
