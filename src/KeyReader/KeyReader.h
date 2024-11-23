#ifndef MMZ_KEY_READER_H
#define MMZ_KEY_READER_H

namespace ReaderData {
    static constexpr uint8_t READ_BITS = 32;
    static constexpr uint8_t READ_BITS_ZERO_INDEXING = READ_BITS - 1;

    const inline uint8_t getIndex(const uint16_t index) { return index / READ_BITS; }
    const inline uint8_t getDigit(const uint16_t index) { return index % READ_BITS_ZERO_INDEXING; }

    template<uint8_t SIZE>
    const inline void setState(uint32_t (&array)[SIZE], const uint16_t index, const bool state) {
        if (state) {
            array[getIndex(index)] |= (1 << getDigit(index));
        } else {
            array[getIndex(index)] &= ~(1 << getDigit(index));
        }
    }

    template<uint8_t NUM_OF_KEYS>
    constexpr inline uint8_t calcKeyboardSize() { return (NUM_OF_KEYS + READ_BITS - 1) / READ_BITS; }
}

template<uint16_t NUM_OF_KEYS>
class KeyReader {
public:
    static constexpr uint8_t KEYBOARD_SIZE = ReaderData::calcKeyboardSize<NUM_OF_KEYS>();
    static constexpr uint16_t getNumOfKeys() { return NUM_OF_KEYS; }

    virtual uint32_t (&getStateData())[KEYBOARD_SIZE] = 0;

    virtual void read() = 0;

    virtual ~KeyReader() = default;
};

#endif
