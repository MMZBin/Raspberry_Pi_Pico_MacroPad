#ifndef MMZ_MACRO_DELAY_H
#define MMZ_MACRO_DELAY_H

#include <Arduino.h>
#include <functional>
#include <vector>

using CallbackFunc = std::function<void()>;

#define After [key]()

class MacroDelay {
public:
    // Callback function is called after specified time.
    static void delay(uint32_t ms, CallbackFunc func) {
        callbacks_.push_back(LazyCallback(ms, func));
    }

    static inline void invoke() {
        uint32_t now = millis();

        //Limit execution to only once per millisecond, as it is called from the update() method of all instances.
        if ((now - lastInvokedTime_) == 0) { return; }
        lastInvokedTime_ = now;

        uint8_t size = callbacks_.size();

        for (uint8_t i = 0; i < size; i++) {
            if (callbacks_[i].executeTime > now) { continue; }

            callbacks_[i].func();
            callbacks_.erase(callbacks_.begin() + i);

            //The 'i' and 'size' are set to -1 because the element is packed by one.
            if (i != 0) { i--; }
            size--;
        }
    }

private:
    MacroDelay() {}

    struct LazyCallback {
    public:
        LazyCallback(uint32_t waitMs, CallbackFunc f) : func(f), executeTime(millis() + waitMs) {}

        CallbackFunc func;
        uint32_t executeTime; // Time to execute.
    };

    static std::vector<LazyCallback> callbacks_;
    static uint32_t lastInvokedTime_;
};


std::vector<typename MacroDelay::LazyCallback> MacroDelay::callbacks_;
uint32_t MacroDelay::lastInvokedTime_ = 0;

inline void macroDelay(uint32_t ms, CallbackFunc func) { MacroDelay::delay(ms, func); }

#endif
