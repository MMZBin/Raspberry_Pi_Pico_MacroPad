#ifndef MMZ_DIRECT_H
#define MMZ_DIRECT_H

#include "KeyReader.h"

template<uint8_t NUM_OF_KEYS>
class Direct : public KeyReader<NUM_OF_KEYS> {
public:
    Direct(uint8_t (&pins)[NUM_OF_KEYS])
     : PINS(pins), keys_{} {
        for (uint8_t pin : PINS) {
            pinMode(pin, INPUT_PULLUP);
        }
    }

    uint32_t (&getStateData())[KeyReader<NUM_OF_KEYS>::KEYBOARD_SIZE] { return keys_; }

    void read() {
        for (uint8_t i = 0; i < NUM_OF_KEYS; i++) {
            setState(keys_, i, !digitalRead(PINS[i]));
        }
    }

private:
    const uint8_t (&PINS)[NUM_OF_KEYS];
    uint32_t keys_[KeyReader<NUM_OF_KEYS>::KEYBOARD_SIZE];
};

#endif