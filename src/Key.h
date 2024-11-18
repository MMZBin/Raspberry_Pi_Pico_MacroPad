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

#ifndef MMZ_KEY_H
#define MMZ_KEY_H

#include <functional>
#include <array>

class Key;

using KeyAssign = std::function<void(Key)>;
template<uint16_t NUM_OF_KEYS>
using Keymap = std::array<KeyAssign, NUM_OF_KEYS>;
template <uint16_t NUM_OF_KEYS, uint8_t NUM_OF_LAYERS>
using LayeredKeymap = std::array<Keymap<NUM_OF_KEYS>, NUM_OF_LAYERS>;

class Key {
public:
    // イベントの種類
    // (排他)とついているイベントは同時にどれか一つしか発生しません。
    // Only one of the events marked with (exclusive) can occur at a time.
    enum class Event : uint8_t {
        SINGLE,       // 1 .短押し       (排他) Short press  (exclusive)
        LONG,         // 2 .長押し       (排他) Long press   (exclusive)
        DOUBLE,       // 3 .ダブルクリック(排他) Double click (exclusive)
        TAP,          // 4 .短押し              Short press
        HOLD,         // 5 .長押し              Long press
        RISING_EDGE,  // 6 .立ち上がりエッジ     Rising edge
        FALLING_EDGE, // 7 .立ち下がりエッジ     Falling edge
        CHANGE_INPUT, // 8 .状態が変わったとき   When the input changes
        PRESSED,      // 9 .押している間毎回     Every time the button is pressed
        RELEASED,     // 10.離している間毎回     Every time the button is released
    };

    void static init(const uint32_t longThreshold=500, const uint32_t doubleThreshold=100,
                     const uint32_t holdThreshold=200, const uint32_t debounceTime=20) {
        longThreshold_ = longThreshold;
        doubleThreshold_ = doubleThreshold;
        holdThreshold_ = holdThreshold;
        debounceTime_ = debounceTime;
    }

    Key();

    void registerMacro(const KeyAssign callback);
    void removeMacro();

    void emulate(const Event type);

    void update(const bool isPressed);  //状態を更新する

    void invoke() const;

    bool hasOccurred(const Event type) const;

    uint32_t getStateDuration() const;
    uint8_t getCountOfClick() const;

    inline bool isPressed() const { return hasOccurred(Event::PRESSED); }
    inline uint32_t getPressTime() const { return (isPressed()) ? getStateDuration() : 0; }

    inline uint16_t getIndex() { return index_; }

    inline void setIndex(const uint16_t index) {
        if (getFlag(EventFlag::INITIALIZED)) { return; }
        index_ = index;
        setFlag(EventFlag::INITIALIZED, true);
    }

private:
    enum class EventFlag : uint8_t {
        PRESS_BAK,
        HANDLED,
        LONG_HANDLED,
        HOLD_HANDLED,
        INITIALIZED
    };

    inline void onPress(const uint32_t now, const uint32_t elapsedTime) {
        emit(Event::PRESSED);

        //立ち上がりエッジのときの処理
        if (!getFlag(EventFlag::PRESS_BAK)) { onRisingEdge(now); }

        if (((now - lastTransTime_) >= holdThreshold_) && (!getFlag(EventFlag::HOLD_HANDLED))) {
            emit(Event::HOLD);
            //isHoldPressed_ = true;
            setFlag(EventFlag::HOLD_HANDLED, true);
        }

        //長押し判定の時間を過ぎたら
        if ((!getFlag(EventFlag::HANDLED)) && (now - lastTransTime_ > longThreshold_)) {
            emit(Event::LONG);
            // isHandled_ = true;
            // isLongPressed_ = true;
            setFlag(EventFlag::HANDLED, true);
            setFlag(EventFlag::LONG_HANDLED, true);
        }
    }

    inline void onRelease(const uint32_t now, const uint32_t elapsedTime) {
        emit(Event::RELEASED);

        //時間を過ぎた&ダブルクリック待ち(再度押されなかったとき)
        if (elapsedTime > doubleThreshold_) {
            if (((countOfClick_ == 1)) && (!getFlag(EventFlag::LONG_HANDLED))) {
                emit(Event::SINGLE);
            }
            countOfClick_ = 0;
        }

        //立ち下がりエッジのときの処理
        if (getFlag(EventFlag::PRESS_BAK)) { onFallingEdge(now, elapsedTime); }

        //isHandled_ = false;
        setFlag(EventFlag::HANDLED, false);
    }

    inline void onRisingEdge(const uint32_t now) {
        emit(Event::RISING_EDGE);
        emit(Event::CHANGE_INPUT);

        lastTransTime_ = now; //押し始めた時間を記録

        //未処理&ダブルクリック待ちのとき
        if ((countOfClick_ == 1) && (!getFlag(EventFlag::HANDLED))) {
            emit(Event::DOUBLE);
            //isHandled_ = true;
            setFlag(EventFlag::HANDLED, true);
        }

        countOfClick_++;
    }

    inline void onFallingEdge(const uint32_t now, const uint32_t elapsedTime) {
        emit(Event::FALLING_EDGE);
        emit(Event::CHANGE_INPUT);

        if (elapsedTime < holdThreshold_) {
            emit(Event::TAP);
        }

        lastTransTime_ = now; //離し始めた時間を記録
        // isLongPressed_ = false;
        // isHoldPressed_ = false;
        setFlag(EventFlag::LONG_HANDLED, false);
        setFlag(EventFlag::HOLD_HANDLED, false);
    }

    inline void emit(const Event type) { hasOccurred_ |= (1 << static_cast<uint8_t>(type)); }

    inline bool getFlag(const EventFlag flag) const { return eventFlags_ & (1 << static_cast<uint8_t>(flag)); }
    inline void setFlag(const EventFlag flag, const bool mode) {
        if (mode) { eventFlags_ |= (1 << static_cast<uint8_t>(flag)); }
        else { eventFlags_ &= ~(1 << static_cast<uint8_t>(flag)); }
    }

    static uint32_t longThreshold_, doubleThreshold_, holdThreshold_, debounceTime_;
    static constexpr uint8_t NUM_OF_EVENTS = 8;

    //bool isPressBak_, isHandled_, isLongPressed_, isHoldPressed_, isInitialized_;
    uint8_t countOfClick_;
    uint8_t eventFlags_;
    uint16_t index_;
    uint32_t lastTransTime_;

    uint16_t hasOccurred_; //0番目のビットが短押し,1番目のビットが長押し...のように対応している

    KeyAssign callback_;
};

#endif
