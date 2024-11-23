#ifndef MMZ_KEYBOARD_H_UTIL_H
#define MMZ_KEYBOARD_H_UTIL_H

#include "Key.h"

inline Macro pressTo(uint8_t pressKey) {
    return [pressKey](Key key) {
        if (key.hasOccurred(Key::Event::RISING_EDGE)) {
            Keyboard.press(pressKey);
        } else if (key.hasOccurred(Key::Event::FALLING_EDGE)) {
            Keyboard.release(pressKey);
        }
    };
}

inline Macro mod(uint8_t tap, uint8_t hold) {
    return [tap, hold](Key key) {
        if (key.hasOccurred(Key::Event::TAP)) {
            Keyboard.press(tap);
            Keyboard.release(tap);
        } else {
            if (key.hasOccurred(Key::Event::HOLD)) {
                Keyboard.press(hold);
            }
            if (key.hasOccurred(Key::Event::FALLING_EDGE)) {
                Keyboard.release(hold);
            }
        }
    };
}
inline Macro mod(Macro tap, Macro hold) {
    return [tap, hold](Key key) {
        if (key.hasOccurred(Key::Event::TAP)) {
            tap(key);
        } else if (key.hasOccurred(Key::Event::HOLD)) {
            hold(key);
        }
    };
}


//inline Macro combo(uint8_t a, uint8_t b, )

#endif

