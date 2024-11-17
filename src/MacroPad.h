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

#ifndef MMZ_KEYBOARD_H
#define MMZ_KEYBOARD_H

#include <Arduino.h>

#include "MatrixKeypad.h"
#include "Key.h"
#include "Delay.h"
#include "Layer.h"
#include "Util.h"

#define Do [](Key key)

template<uint8_t NUM_OF_LAYERS, uint8_t ROWS, uint8_t COLS>
class MacroPad {
public:
    static constexpr uint16_t NUM_OF_KEYS = ROWS * COLS;
    static constexpr uint8_t getNumOfLayers() { return NUM_OF_LAYERS; }

    MacroPad(uint8_t (&rowPins)[ROWS], uint8_t (&colPins)[COLS])
     : keyboard_(rowPins, colPins),
       LAYERS(Layer<NUM_OF_KEYS, NUM_OF_LAYERS>(KEYS)) {
        static_assert((NUM_OF_LAYERS > 0), "'NUM_OF_LAYERS' must be 1 or greater.");
        static_assert((ROWS > 0), "'ROWS' must be 1 or greater.");
        static_assert((COLS > 0), "'COLS' must be 1 or greater.");
        static_assert((NUM_OF_KEYS < UINT16_MAX), "The total number of keys (including invalid keys) must be 65535 or less.");

        for (uint16_t i = 0; i < ROWS * COLS; i++) {
            KEYS[i].setIndex(i);
        }
    }

    void init(LayeredKeymap<NUM_OF_KEYS, NUM_OF_LAYERS> layeredKeymap) {
        LAYERS.init(layeredKeymap);

        LAYERS.set(0);
    }

    void update() {
        static constexpr uint8_t READ_BITS_ZERO_INDEXING =  MatrixKeypad<ROWS, COLS>::READ_BITS_ZERO_INDEXING;
        uint32_t (&isPressed)[MatrixKeypad<ROWS, COLS>::KEYBOARD_SIZE] = keyboard_.read();

        for (uint16_t i = 0; i < NUM_OF_KEYS; i++) {
            KEYS[i].update(isPressed[i / READ_BITS_ZERO_INDEXING] & (1 << (i % READ_BITS_ZERO_INDEXING)));
        }

        for (uint16_t i = 0; i < NUM_OF_KEYS; i++) {
            KEYS[i].invoke();
        }

        MacroDelay::invoke();
    }

    LayerUtil<NUM_OF_KEYS, NUM_OF_LAYERS> getLayerUtil() { return LayerUtil<NUM_OF_KEYS, NUM_OF_LAYERS>(LAYERS); }


    std::array<Key, NUM_OF_KEYS> KEYS;
    Layer<NUM_OF_KEYS, NUM_OF_LAYERS> LAYERS;

private:
    MatrixKeypad<ROWS, COLS> keyboard_;
};

#endif
