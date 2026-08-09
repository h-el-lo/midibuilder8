#pragma once
#include <Arduino.h>

enum class MenuItemType : uint8_t {
    SUBMENU,    // opens another Menu (pushed onto the nav stack)
    VALUE_INT,  // editable integer, bounded by min/max
    ACTION,     // fires a callback immediately, no sub-page
    BACK        // optional explicit "< Back" row (pops the nav stack)
};

struct Menu; // forward decl, MenuItem needs to point to it

struct MenuItem {
    const char* label;
    MenuItemType type;
    Menu* submenu;         // used by SUBMENU
    int*  valuePtr;        // used by VALUE_INT
    int   minValue;        // used by VALUE_INT
    int   maxValue;        // used by VALUE_INT
    int   step;            // used by VALUE_INT
    void (*action)();      // used by ACTION
};

struct Menu {
    const char* title;
    MenuItem*   items;
    uint8_t     itemCount;
};

// ---- Helper "constructors" -------------------------------------------
// Plain aggregates avoid relying on C++14 default member initializers
// (older AVR cores default to C++11), these keep menu tables readable.

inline MenuItem makeSubmenuItem(const char* label, Menu* submenu) {
    return MenuItem{ label, MenuItemType::SUBMENU, submenu, nullptr, 0, 0, 0, nullptr };
}

inline MenuItem makeValueItem(const char* label, int* valuePtr,
                               int minV, int maxV, int step = 1) {
    return MenuItem{ label, MenuItemType::VALUE_INT, nullptr, valuePtr, minV, maxV, step, nullptr };
}

inline MenuItem makeActionItem(const char* label, void (*action)()) {
    return MenuItem{ label, MenuItemType::ACTION, nullptr, nullptr, 0, 0, 0, action };
}

inline MenuItem makeBackItem(const char* label = "< Back") {
    return MenuItem{ label, MenuItemType::BACK, nullptr, nullptr, 0, 0, 0, nullptr };
}
