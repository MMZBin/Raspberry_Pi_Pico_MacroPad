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

#ifndef MMZ_MATRIX_H
#define MMZ_MATRIX_H

#include <Arduino.h>
#include "KeyReader.h"

template<uint8_t ROWS, uint8_t COLS>
class Matrix : public KeyReader<ROWS * COLS> {
public:
    Matrix(uint8_t (&rowPins)[ROWS], uint8_t (&colPins)[COLS])
     : ROW_PINS(rowPins), COL_PINS(colPins), keys_{} {
        // memcpy(ROW_PINS, rowPins, sizeof(rowPins));
        // memcpy(COL_PINS, colPins, sizeof(colPins));

        for (uint8_t pin : ROW_PINS) {
            pinMode(pin, OUTPUT);
            digitalWrite(pin, HIGH);
        }

        for (uint8_t pin : COL_PINS) {
            pinMode(pin, INPUT_PULLUP);
        }
    }

    uint32_t (&getStateData())[KeyReader<ROWS * COLS>::KEYBOARD_SIZE] { return keys_; }

    void read() {
        uint16_t index = 0;

        for (const uint8_t rowPin : ROW_PINS) {
            digitalWrite(rowPin, LOW);
            for (const uint8_t colPin : COL_PINS) {
                //The status is written in order from LSB, and the index of keys is increased by one for each READ_BIT (32 in this case).
                //If the 33rd key is pressed, index / Matrix::READ_BITS = 1, index % Matrix::READ_BITS - 1 = 2, so the second bit of the second element is 1.
                ReaderData::setState(keys_, index, !digitalRead(colPin));

                index++;
            }
            digitalWrite(rowPin, HIGH);
        }
    }

private:
    const uint8_t (&ROW_PINS)[ROWS];
    const uint8_t (&COL_PINS)[COLS];

    uint32_t keys_[KeyReader<ROWS * COLS>::KEYBOARD_SIZE];
};

#endif
