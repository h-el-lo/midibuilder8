#include "MenuController.h"
#include "MenuTypes.h"
#include "MenuStack.h"
#include "MenuScreen.h"
#include "Encoder.h"

// Encoder is wired directly to the MCU
static constexpr uint8_t PIN_ENC_A = 2;
static constexpr uint8_t PIN_ENC_B = 1;

// 20x4 I2C LCD, custom S3 I2C bus pins.
static constexpr uint8_t I2C_SDA       = 21;
static constexpr uint8_t I2C_SCL       = 47;
static constexpr uint8_t LCD_I2C_ADDR  = 0x27; // try 0x3F if the screen stays blank

// ------------------------------------------------------------------
// Peripherals owned by the menu controller
// ------------------------------------------------------------------
static Encoder    encoder(PIN_ENC_A, PIN_ENC_B);
static MenuScreen  menuScreen(LCD_I2C_ADDR, I2C_SDA, I2C_SCL, 20, 4);
static MenuStack<8> navStack;

// ------------------------------------------------------------------
// The actual settings (backing variables for VALUE_INT items)
// ------------------------------------------------------------------
int settingMin        = 0;
int settingMax         = 100;
int settingBrightness  = 50;
int settingContrast    = 50;
int settingTimeout     = 30;
int settingVolume      = 10;
int settingThreshold   = 5;
int settingOffset      = 0;
int settingGain        = 1;

static void actionResetDefaults() {
    settingMin = 0;        settingMax = 100;
    settingBrightness = 50; settingContrast = 50;
    settingTimeout = 30;    settingVolume = 10;
    settingThreshold = 5;   settingOffset = 0; settingGain = 1;
}

// ------------------------------------------------------------------
// Menu tree. "Advanced" is a nested submenu to show arbitrary depth --
// add more SUBMENU items / more levels the same way.
// ------------------------------------------------------------------
static MenuItem advancedItems[] = {
    makeValueItem("Gain",   &settingGain,   1, 10, 1),
    makeValueItem("Offset", &settingOffset, -50, 50, 1),
    makeBackItem(),
};
static Menu advancedMenu = { "Advanced", advancedItems, 3 };

static MenuItem mainItems[] = {
    makeValueItem ("Set Minimum",    &settingMin,        0,    1000, 1),
    makeValueItem ("Set Maximum",    &settingMax,         0,    1000, 1),
    makeValueItem ("Brightness",     &settingBrightness,  0,    100,  5),
    makeValueItem ("Contrast",       &settingContrast,    0,    100,  5),
    makeValueItem ("Timeout (s)",    &settingTimeout,     5,    300,  5),
    makeValueItem ("Volume",         &settingVolume,      0,    20,   1),
    makeValueItem ("Threshold",      &settingThreshold,   0,    50,   1),
    makeSubmenuItem("Advanced",      &advancedMenu),
    makeActionItem ("Reset Defaults", actionResetDefaults),
};
static Menu mainMenu = { "Settings", mainItems, 9 };

// ------------------------------------------------------------------
// App state machine
// ------------------------------------------------------------------
enum class AppMode : uint8_t { HOME, MENU, EDIT_VALUE };
static AppMode mode = AppMode::HOME;

static MenuItem* editingItem = nullptr;
static int       editScratch = 0;

static void showHome() {
    // Hook this into your existing home screen here if desired
    // (e.g. call into the project's other Screen.h / `screen` object).
}

void menuController_begin() {
    encoder.begin();
    menuScreen.begin();
    showHome();
}

void menuController_goHomeExplicit() {
    navStack.popToRoot();
    mode = AppMode::HOME;
    showHome();
}

// ------------------------------------------------------------------
// Button callbacks -- these ARE the onPress() handlers (called
// synchronously from Button::read() via ActionButton::onPress()).
// Debounce/edge-detection already happened upstream in Buttons.cpp.
// ------------------------------------------------------------------

void menuController_onSettingsPressed() {
    if (mode == AppMode::HOME) {
        navStack.reset(&mainMenu);   // fresh history -- the one explicit reset point
        mode = AppMode::MENU;
        menuScreen.renderMenu(navStack.top());
    }
    // If already inside the menu, Settings is a no-op here; wire it to
    // menuController_goHomeExplicit() instead if you'd rather it jump
    // straight back to the root menu from anywhere.
}

void menuController_onEnterPressed() {
    if (mode == AppMode::MENU) {
        NavState& nav = navStack.top();
        Menu* menu = nav.menu;
        MenuItem& item = menu->items[nav.selectedIndex];

        switch (item.type) {
            case MenuItemType::SUBMENU:
                navStack.push(item.submenu);
                menuScreen.renderMenu(navStack.top());
                break;

            case MenuItemType::VALUE_INT:
                editingItem = &item;
                editScratch = *item.valuePtr;
                mode = AppMode::EDIT_VALUE;
                menuScreen.renderValueEditor(item.label, editScratch,
                                              item.minValue, item.maxValue);
                break;

            case MenuItemType::ACTION:
                if (item.action) item.action();
                menuScreen.renderMenu(nav);
                break;

            case MenuItemType::BACK:
                if (navStack.pop()) menuScreen.renderMenu(navStack.top());
                else { mode = AppMode::HOME; showHome(); }
                break;
        }

    } else if (mode == AppMode::EDIT_VALUE) {
        // Commit the edited value.
        if (editingItem) *editingItem->valuePtr = editScratch;
        editingItem = nullptr;
        mode = AppMode::MENU;
        menuScreen.renderMenu(navStack.top());
    }
}

void menuController_onExitPressed() {
    if (mode == AppMode::MENU) {
        // Core requirement: go back exactly one level in history.
        // Only fall through to HOME if we were already at the root.
        if (navStack.pop()) {
            menuScreen.renderMenu(navStack.top());
        } else {
            mode = AppMode::HOME;
            showHome();
        }

    } else if (mode == AppMode::EDIT_VALUE) {
        // Cancel -- discard scratch value, don't touch the real setting.
        editingItem = nullptr;
        mode = AppMode::MENU;
        menuScreen.renderMenu(navStack.top());
    }
}

// ------------------------------------------------------------------
// Called every loop() -- the encoder is polled (not event-driven like
// the buttons), so this is where rotation gets applied.
// ------------------------------------------------------------------
void menuController_update() {
    if (mode == AppMode::HOME) return;

    int16_t delta = encoder.getDelta();
    if (delta == 0) return;

    if (mode == AppMode::MENU) {
        NavState& nav = navStack.top();
        int newIndex = (int)nav.selectedIndex + delta;
        newIndex = constrain(newIndex, 0, nav.menu->itemCount - 1);
        nav.selectedIndex = (uint8_t)newIndex;
        menuScreen.renderMenu(nav);

    } else if (mode == AppMode::EDIT_VALUE && editingItem) {
        editScratch += delta * editingItem->step;
        editScratch = constrain(editScratch, editingItem->minValue, editingItem->maxValue);
        menuScreen.renderValueEditor(editingItem->label, editScratch,
                                      editingItem->minValue, editingItem->maxValue);
    }
}
