# Pico MacroPad Library
[日本語のREADMEはこちらから](/README.ja.md)

This is a library for controlling a macro pad (keyboard) using Arduino.

This library only handles key input processing and macro management, so you will need a separate HID library to use it as an HID device (e.g., [Keyboard library](https://github.com/arduino-libraries/Keyboard)).

This README assumes the use of `Keyboard.h`, so for detailed information about keyboard functionality, please refer to [Keyboard](https://github.com/arduino-libraries/Keyboard).

__This library uses STL, so it only works in environments where STL is supported.__
For platforms like AVR microcontrollers that do not support STL by default, additional library installation may be required.

# Features
- `Do`
    - A macro that allows concise definition of custom macros.
    - Takes a `Key`-type argument named `key`.
    - Example: ```Do { Keyboard.print("Hello, world!"); }```
- `After`
    - A macro that simplifies defining actions to be executed after a delay with the `macroDelay()` function.
    - Captures the `key` from the outer scope.
    - Example: ```macroDelay(1000, After { Keyboard.print("Hello, world!"); });```

- `NONE`
    - An alias for `nullptr`.
    - Can be used to indicate an invalid key assignment.

- `pressTo()`
    - Returns a macro that sends a character to the PC while the key is pressed, similar to a standard keyboard.
    - Can only be used with supported HID libraries (currently `Keyboard.h` only).

- `PRESS_A, PRESS_B, ...`
    - Simple wrappers around the `pressTo()` function.

## About `MacroPad`
- The central class of this library.
    - `init(LayeredKeymap)`
        - Registers a keymap with the macro pad.
        - Must be executed during initialization.
    - `KEYS`
        - An array of `Key` objects corresponding to all keys.
    - `LAYERS`
        - An object of the `Layer` class that manages layers.

## About `Key`
- Each key is assigned a `Key` object in this library, which manages the state of each key.
- Below is a partial list of the interface:
    - `Event` Enum
        - Represents events occurring for each key.
            |Event Name|Condition|
            |----|----|
            |SINGLE|When pressed briefly once|
            |LONG|When pressed and held|
            |DOUBLE|When double-pressed in quick succession|
            |RISING_EDGE|The moment the key is pressed|
            |FALLING_EDGE|The moment the key is released|
            |CHANGE_INPUT|When key input switches|
            |PRESSED|While the key is being pressed|
            |RELEASED|While the key is released|

    - `init(longThreshold, doubleThreshold, debounceTime)`
        - Specifies the duration for long presses, the grace period for double-clicks, and debounce time.
        - Example: `Key::init(1000, 500, 10);`
    - `bool hasOccurred(Key::Event)`
        - Checks whether the specified event has occurred for the key.
        - Example: `key.hasOccurred(Key::Event::SINGLE)`
    - `uint32_t getStateDuration()`
        - Returns the time in milliseconds since the last input change.
        - Example: `key.getStateDuration()`
    - `uint8_t getCountOfClick()`
        - Returns the number of times the key has been clicked.
        - Resets if the interval exceeds `doubleThreshold`.
        - Allows configuration of events up to 255 clicks.
    - `bool isPressed()`
        - Returns whether the key is pressed.
        - Equivalent to `hasOccurred(Key::Event::PRESSED)`.
    - `uint32_t getPressTime()`
        - Returns the duration the key has been pressed.
        - Returns `0` if not pressed.
    - `uint16_t getIndex()`
        - Returns the key's index.

## About Custom Macros
※__The term "macro" in "`Do` macro" refers to a construct replaced by the `#define` directive. Hereafter, "macro" refers to a program executed in response to key events.__  
- Custom macros are defined using the `Do` macro.
    - Macros receive a reference to the executing `Key` object as the `key` argument.
- While internal processing can be arbitrary, macro processing is synchronous, so it is recommended to implement them for quick execution.
- Macros are executed in ascending order of their indices.

### `macroDelay(ms, func)` Function
- Used when certain actions in a macro need to be delayed.
- Takes the delay duration (milliseconds) as the first argument and the function to execute as the second.
- The second argument can be concisely defined using the `After` macro.
- Avoid using the `delay()` function within macros unless necessary, as it halts all processing. Use this function as a substitute.
- Time is measured using polling, so precision is limited.
- Example: ```macroDelay(1000, After { Keyboard.print("Hello, world!"); });```

### About Layers
- You can use layers equal to the number specified when creating an instance of `MacroPad` (up to 255).
- When passing the keymap to the `MacroPad::init()` method, you need an array of keymaps for the specified number of layers (see sample code).
- The `Layer` class manages the layers.
    - `set(layer)`
        - Switches to the specified layer.
        - Does nothing if the specified layer does not exist.
        - Example: ```macroPad.LAYERS.set(1)```
    - `reset()`
        - Returns to the previous layer.
        - May not work as intended if the same layer is switched to twice in succession.
    - `uint8_t get()`
        - Returns the index of the current layer.
- The `LayerUtil` class simplifies the configuration of macros for layer switching.
    - `KeyAssign to(layer)`
        - Returns a macro that switches to the specified layer when the key is pressed.
        - Example: ```layer.to(1)```
    - `KeyAssign back(layer)`
        - Returns a macro that switches to the specified layer when the key is released.
        - Example: ```layer.back(0)```
    - `KeyAssign reset()`
        - Returns a macro that returns to the previous layer.
        - Example: ```layer.reset()```

--- 

Let me know if you'd like further adjustments or clarifications!