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

#endif
