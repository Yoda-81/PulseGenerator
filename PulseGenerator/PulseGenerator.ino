/*
 * Pulse Generator
 * 
 * Simple flip-flop pulse generator with adjustable minimal, maximal and 
 * current working frequency. Adjusting is perfomed by rotary encoder,
 * shown on display.
 * 
 * The circuit:
 * - Arduino Nano
 * - Rotary encoder KY-040
 * - I2C OLED display 0,96" 128x64 (SSD-1306)
 * 
 * @author https://github.com/Konajka
 * @version 0.1 2019-05-02
 *      Base implementation
 * @version 0.2 2019-05-13
 *      Added rotary encoder support.
 *      Added menu login.   
 */

#include <Arduino.h>
#include "lib/RotaryEncoder.h"
#include "lib/QMenu.h"

// Define to turn on serial link logging
#define SERIAL_LOG

#define ENCODER_CLK 5
#define ENCODER_DT 4
#define ENCODER_SW 3

// Menu constants
#define MENU_SETUP 1
#define MENU_MIN_FREQ 11
#define MENU_MAX_FREQ 12
#define MENU_PULSE_RATIO 13
#define MENU_CURVE_SHAPE_SUBMENU 14
#define MENU_CURVE_SHAPE_LINEAR 141
#define MENU_CURVE_SHAPE_QUADRATIC 142
#define MENU_FREQ_FLOATING 15
#define MENU_FREQ_UNITS_SUBMENU 16
#define MENU_FREQ_UNITS_RPM 161
#define MENU_FREQ_UNITS_HZ 162
#define MENU_EXIT 17

/** Rotary encoder controller */
RotaryEncoder encoder(ENCODER_CLK, ENCODER_DT, ENCODER_SW);

/** Menu controller and renederer */
QMenu menu(MENU_SETUP, "Setup");
QMenuListRenderer menuRenderer(&menu, 3);

/** Application status holder */
enum Status { running, navigation } status = running;

/** Current working frequency */
word frequency;

/** Settings */
struct Settings {
    word minFreq;
    word maxFreg;
    byte freqRatio;
    byte curveShape;
    bool freqFloating;
    byte freqUnits;
} settings;

/* Initialization */
void setup() {
    #ifdef SERIAL_LOG
    Serial.begin(9600);
    Serial.println("Pulse generator");
    #endif

    // Setup encoder
    encoder.begin();
    encoder.setOnChange(encoderOnChange);
    encoder.setOnClick(encoderOnClick);
    encoder.setOnLongClick(encoderOnLongClick);

    // Create menu structure
    menu.getRoot()
        ->setMenu(QMenuItem::create(MENU_MIN_FREQ, "Minimal frequency"))
        ->setNext(QMenuItem::create(MENU_MAX_FREQ, "Maximal frequency"))
        ->setNext(QMenuItem::create(MENU_PULSE_RATIO, "Pulse ratio"))
        ->setNext(QMenuItem::create(MENU_CURVE_SHAPE_SUBMENU, "Acceleration curve shape"))
            ->setMenu(QMenuItem::create(MENU_CURVE_SHAPE_LINEAR, "Linear acceleration"))
            ->setNext(QMenuItem::create(MENU_CURVE_SHAPE_QUADRATIC, "Quadratic acceleration"))
            ->getBack()        
        ->setNext(QMenuItem::create(MENU_FREQ_FLOATING, "Frequency floating"))
        ->setNext(QMenuItem::create(MENU_FREQ_UNITS_SUBMENU, "Frequency units"))
            ->setMenu(QMenuItem::create(MENU_FREQ_UNITS_RPM, "Rotates per minute"))
            ->setNext(QMenuItem::create(MENU_FREQ_UNITS_HZ, "Hertz"))
            ->getBack()  
        ->setNext(QMenuItem::create(MENU_EXIT, "Exit"));

    //Set menu events
    menu.setOnActiveItemChanged(activeItemChanged);
    menu.setOnItemUtilized(onItemUtilized);

    // Setup menu rederer
    menuRenderer.setOnRenderItem(onRenderMenuItem);    

    // Load settings
    // TODO load

    //Set initial frequency
    frequency = settings.minFreq;    
}

/* Main Loop */
void loop() {  
    encoder.update();
    renderScreen();
}

/** Render screen via status model */
void renderScreen() {
    // if refresh timeout ...
    // render data if not redered by menu controller...
}

/* Encoder rotation event */
void encoderOnChange(RotaryEncoderOnChangeEvent event) {  
    if (event.direction == left) {
        QMenuItem* item = menu.prev();
        #ifdef SERIAL_LOG
        Serial.print("Menu move left: ");
        if (item != NULL) {
            Serial.print(item->getCaption());
        }
        Serial.println();
        #endif
    } else if (event.direction == right) {
        QMenuItem* item = menu.next();
        #ifdef SERIAL_LOG
        Serial.print("Menu move right: ");
        if (item != NULL) {
            Serial.print(item->getCaption());
        }
        Serial.println();
        #endif
    }
}

/* Encoder click event */
void encoderOnClick() {
    QMenuItem* item = menu.enter();
    #ifdef SERIAL_LOG
    Serial.print("Menu enter: ");
    if (item != NULL) {
        Serial.print(item->getCaption());
    }
    Serial.println();
    #endif
}

/* Encoder long click event */
void encoderOnLongClick() {
    QMenuItem* item = menu.back();
    #ifdef SERIAL_LOG
    Serial.print("Menu get back: ");
    if (item != NULL) {
        Serial.print(item->getCaption());
    }
    Serial.println();
    #endif
}

/* Menu item changed */
void activeItemChanged(QMenuActiveItemChangedEvent event) {
    #ifdef SERIAL_LOG
    Serial.print("Menu changed ");
    if (event.oldActiveItem != NULL) {
        Serial.print(event.oldActiveItem->getCaption());
    }
    Serial.print(" -> ");
    if (event.newActiveItem != NULL) {
        Serial.print(event.newActiveItem->getCaption());
    }
    Serial.println();
    #endif
}

/* Menu item used */
void onItemUtilized(QMenuItemUtilizedEvent event) {
    #ifdef SERIAL_LOG
    Serial.print("Menu utilized ");
    if (event.utilizedItem != NULL) {
        Serial.print(event.utilizedItem->getCaption());
    }
    Serial.println();
    #endif
}

/* Render menu item */
void onRenderMenuItem(QMenuRenderItemEvent event) {
    #ifdef SERIAL_LOG
    if (event.isActive) {
        Serial.print("|[ ");
    } else {
        Serial.print("|  ");
    }
    Serial.print(event.item->getCaption());
    padChar(' ', 20 - strlen(event.item->getCaption()));
    if (event.isActive) {
        Serial.print(" ]|");
    } else {
        Serial.print("  |");
    }
    Serial.println();
    #endif
}

#ifdef SERIAL_LOG
void padChar(char c, int count) {
    for (int index = 0; index < count; index++) {
        Serial.print(c);
    } 
}
#endif
