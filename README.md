# MIDI Builder 8

A firmware project for a custom MIDI keyboard controller, evolved from the original ATmega328U4 microcontroller implementation.

## Overview

**MIDI Builder 8** is the latest iteration of a personal MIDI keyboard controller project, written primarily in C++ (65.2%) with supporting C code (34.8%). This repository represents the culmination of multiple development stages, refining hardware abstraction, code organization, and performance from earlier prototypes.

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

### Stage 4: MIDI Builder 8 (Current)
The **current stage** consolidates all lessons learned:
- Full keyboard implementation (8+ keys in the naming convention suggests an octa-keypad or extended layout)
- Mature C++/C hybrid codebase balancing performance and readability
- Enhanced modularity and maintainability
- Production-ready firmware for custom MIDI keyboards

## Comparison with Earlier Projects

| Aspect | controllerbuilder | MIDI Builder 8 |
|--------|------------------|----------------|
| **Scope** | Mini MIDI controller | Full keyboard |
| **Language** | C++ | C++ (65.2%) + C (34.8%) |
| **Focus** | Compact input device | Extended key layout |
| **Maturity** | Established | Production-ready |
| **Code Organization** | Modular design | Enhanced modularity |

## Architecture

The firmware is structured to handle:
- **Key Matrix Scanning**: Efficient debouncing and state tracking
- **MIDI Protocol**: Complete USB/UART MIDI communication
- **Performance Optimization**: Balance between C performance and C++ abstraction

## Technical Details

- **Primary Language**: C++
- **Supporting Language**: C
- **Target Hardware**: ATmega-class microcontrollers (evolved from ATmega328U4)
- **Protocol**: MIDI over USB/Serial
- **Use Case**: Custom MIDI keyboard input for music production/performance

## Getting Started

### Prerequisites
- Arduino IDE or compatible development environment
- AVR-GCC toolchain for compilation
- Target microcontroller (ATmega or compatible)

### Build & Upload
Compile and upload the firmware to your target microcontroller using the Arduino IDE or command-line toolchain.

### Configuration
Modify the keyboard layout, key mappings, and MIDI channel settings in the configuration header files as needed for your hardware setup.

## Features

- ✅ Full MIDI protocol support
- ✅ Debounced key scanning
- ✅ USB and/or serial MIDI output
- ✅ Customizable key mappings
- ✅ Optimized for embedded performance

## Development Notes

This project demonstrates the evolution from simple single-microcontroller implementations to a more sophisticated, modular firmware architecture. The hybrid C++/C approach leverages the strengths of both languages:
- **C++**: Object-oriented abstractions for cleaner code structure
- **C**: Performance-critical sections and hardware-level operations

## License

Not specified. See the repository for any applicable license terms.

## Contributing

For improvements, bug fixes, or feature additions, please refer to the repository's contribution guidelines.

---

**Note**: This is a personal project for custom MIDI keyboard development. The evolution from controllerbuilder through MIDI Builder 8 reflects iterative refinement toward a production-grade input controller.
