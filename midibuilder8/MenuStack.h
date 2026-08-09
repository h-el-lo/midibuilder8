#pragma once
#include "MenuTypes.h"

// One entry per menu "level" the user has drilled into.
// Storing selectedIndex/scrollOffset per level means that when you pop
// back out, the previous page reappears exactly as you left it.
struct NavState {
    Menu*   menu;
    uint8_t selectedIndex;
    uint8_t scrollOffset;
};

template <uint8_t MAX_DEPTH = 8>
class MenuStack {
public:
    // Clears history and opens rootMenu as level 0 (home of the settings tree).
    void reset(Menu* rootMenu) {
        _size = 0;
        push(rootMenu);
    }

    void push(Menu* menu) {
        if (_size >= MAX_DEPTH) return; // guard: nesting too deep, ignore
        _stack[_size].menu          = menu;
        _stack[_size].selectedIndex = 0;
        _stack[_size].scrollOffset  = 0;
        _size++;
    }

    // Returns to the previous level. Returns false if already at the root
    // (caller should then treat Exit as "leave the settings system").
    bool pop() {
        if (_size <= 1) return false;
        _size--;
        return true;
    }

    bool atRoot() const { return _size <= 1; }

    // Explicit jump back to root, e.g. for a dedicated "Home" action.
    void popToRoot() {
        _size = (_size > 0) ? 1 : 0;
    }

    NavState& top()          { return _stack[_size - 1]; }
    uint8_t   depth() const  { return _size; }

private:
    NavState _stack[MAX_DEPTH];
    uint8_t  _size = 0;
};
