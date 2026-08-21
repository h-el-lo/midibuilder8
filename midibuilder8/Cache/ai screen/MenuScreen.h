#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>   // https://github.com/johnrickman/LiquidCrystal_I2C
#include "MenuTypes.h"
#include "MenuStack.h"

// Renamed from "Screen" -> "MenuScreen" because this project already has a
// Screen.h / global `screen` object (see Buttons.cpp: screen.printHomeHandler).
// This class ONLY owns the settings-menu LCD, not the whole display.
//
// Renders a Menu's item list into a 20x4 window, keeping the selected row
// scrolled into view, plus a full-screen editor for VALUE_INT items.
class MenuScreen {
public:
    MenuScreen(uint8_t i2cAddr, uint8_t sdaPin, uint8_t sclPin, uint8_t cols = 20, uint8_t rows = 4)
        : _lcd(i2cAddr, cols, rows), _sda(sdaPin), _scl(sclPin), _cols(cols), _rows(rows) {}

    void begin() {
        // Wire.begin(_sda, _scl);   // ESP32-S3: custom SDA/SCL pins (21 / 47)
        _lcd.init();
        _lcd.backlight();
    }

    // Draws the current page. Mutates nav.scrollOffset so the selection
    // stays on-screen -- call this any time selection or menu changes.
    void renderMenu(NavState& nav) {
        Menu* menu = nav.menu;
        uint8_t count = menu->itemCount;

        // Keep selection within the visible window.
        if (nav.selectedIndex < nav.scrollOffset) {
            nav.scrollOffset = nav.selectedIndex;
        } else if (nav.selectedIndex >= nav.scrollOffset + _rows) {
            nav.scrollOffset = nav.selectedIndex - _rows + 1;
        }
        if (count > _rows && nav.scrollOffset > count - _rows) {
            nav.scrollOffset = count - _rows;
        }
        if (count <= _rows) nav.scrollOffset = 0;

        _lcd.clear();
        for (uint8_t row = 0; row < _rows; row++) {
            uint8_t itemIdx = nav.scrollOffset + row;
            if (itemIdx >= count) break;

            MenuItem& item = menu->items[itemIdx];

            _lcd.setCursor(0, row);
            _lcd.print(itemIdx == nav.selectedIndex ? '>' : ' ');

            if (item.type == MenuItemType::VALUE_INT) {
                char buf[8];
                snprintf(buf, sizeof(buf), "%d", *item.valuePtr);
                uint8_t labelSpace = _cols - 1 - (uint8_t)strlen(buf) - 1;
                printTruncated(item.label, labelSpace);
                _lcd.setCursor(_cols - (uint8_t)strlen(buf), row);
                _lcd.print(buf);
            } else {
                printTruncated(item.label, _cols - 1);
            }
        }

        // Up/down scroll indicators in the corners, when the list overflows.
        if (count > _rows) {
            _lcd.setCursor(_cols - 1, 0);
            _lcd.print(nav.scrollOffset > 0 ? '^' : ' ');
            if (_rows > 1) {
                _lcd.setCursor(_cols - 1, _rows - 1);
                _lcd.print((nav.scrollOffset + _rows < count) ? 'v' : ' ');
            }
        }
    }

    // Full-screen editor shown while a VALUE_INT item is being adjusted.
    void renderValueEditor(const char* label, int value, int minV, int maxV) {
        _lcd.clear();
        _lcd.setCursor(0, 0);
        printTruncated(label, _cols);

        _lcd.setCursor(0, 1);
        _lcd.print("Value: ");
        _lcd.print(value);

        _lcd.setCursor(0, 2);
        _lcd.print("Range: ");
        _lcd.print(minV);
        _lcd.print("..");
        _lcd.print(maxV);

        _lcd.setCursor(0, 3);
        _lcd.print("Enter=Save Exit=Cancel");
    }

private:
    void printTruncated(const char* text, uint8_t maxLen) {
        uint8_t len = (uint8_t)strlen(text);
        for (uint8_t i = 0; i < maxLen; i++) {
            _lcd.print(i < len ? text[i] : ' ');
        }
    }

    LiquidCrystal_I2C _lcd;
    uint8_t _sda, _scl;
    uint8_t _cols, _rows;
};
