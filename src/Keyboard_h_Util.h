#ifndef MMZ_KEYBOARD_H_UTIL_H
#define MMZ_KEYBOARD_H_UTIL_H

#include "Key.h"

inline KeyAssign pressTo(uint8_t pressKey) {
    return [pressKey](Key key) {
        if (key.hasOccurred(Key::Event::RISING_EDGE)) {
            Keyboard.press(pressKey);
        } else if (key.hasOccurred(Key::Event::FALLING_EDGE)) {
            Keyboard.release(pressKey);
        }
    };
}

inline KeyAssign mod(uint8_t tap, uint8_t hold) {
    return [tap, hold](Key key) {
        if (key.hasOccurred(Key::Event::TAP)) {
            Keyboard.press(tap);
            Keyboard.release(tap);
        } else {
            if (key.hasOccurred(Key::Event::HOLD)) {
                Keyboard.press(hold);
            }
            if (key.hasOccurred(Key::Event::RELEASED)) {
                Keyboard.release(hold);
            }

        }
    };
}

inline KeyAssign combo(uint8_t a, uint8_t b, )

#endif

