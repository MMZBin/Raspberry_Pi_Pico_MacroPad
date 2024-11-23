/*
    MIT License

    Copyright (c) 2024 MMZBin

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include <Arduino.h>
#include "Key.h"

/* public */

Key::Key()
 : lastTransTime_(0), countOfClick_(0), eventFlags_(0), index_(0),
   hasOccurred_(0), callback_() {}

void Key::registerMacro(const Macro func) { callback_ = func; }
void Key::removeMacro() { registerMacro(nullptr); }

void Key::emulate(const Event type) { hasOccurred_ |= (1 << static_cast<uint8_t>(type)); }
void Key::clear(const Event type) { hasOccurred_ &= ~(1 << static_cast<uint8_t>(type)); }

void Key::update(const bool isPressed) {
    hasOccurred_ = 0;

    const uint32_t now = millis();
    const uint32_t elapsedTime = now - lastTransTime_;

    if (elapsedTime >= DEBOUNCE_TIME) {
        if (isPressed) { onPress(now, elapsedTime); }
        else { onRelease(now, elapsedTime); }
    }

    //isPressBak_ = isPressed; // 前回の値を更新
    setFlag(EventFlag::PRESS_BAK, isPressed);
}

void Key::invoke() const {
    if (callback_ != nullptr) { callback_(*this); }
}

bool Key::hasOccurred(const Event type) const { return hasOccurred_ & (1 << static_cast<uint8_t>(type)); }

uint32_t Key::getStateDuration() const { return millis() - lastTransTime_; }
uint8_t Key::getCountOfClick() const { return countOfClick_; }

uint32_t Key::LONG_THRESHOLD = 500;
uint32_t Key::DOUBLE_THRESHOLD = 200;
uint32_t Key::HOLD_THRESHOLD = 200;
uint32_t Key::DEBOUNCE_TIME = 20;
