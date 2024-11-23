# Pico MacroPad Library

_The English used in this library is based on machine translations from Japanese._

[日本語のREADMEはこちらから](/README.ja.md)

This is a library for controlling a macro pad (keyboard) using Arduino.

This library only handles key input processing and macro management, so you will need a separate HID library to use it as an HID device (e.g., [Keyboard library](https://github.com/arduino-libraries/Keyboard)).

This README assumes the use of `Keyboard.h`, so for detailed information about keyboard functionality, please refer to [Keyboard](https://github.com/arduino-libraries/Keyboard).

__This library uses STL, so it only works in environments where STL is supported.__
For platforms like AVR microcontrollers that do not support STL by default, additional library installation may be required.

## Features
- `Do`
    - A macro that allows concise definition of custom macros.
    - Takes an argument `key` of type `Key`.
    - Example: ```Do { Keyboard.print("Hello, world!"); }```

- `After`
    - A macro for easily defining content to execute after a delay using the `macroDelay()` function.
    - Captures the `key` from the outer scope.
    - Example: ```macroDelay(1000, After { Keyboard.print("Hello, world!"); });```

- `NONE`
    - An alias for `nullptr`.
    - Used to indicate an invalid key assignment.

- `pressTo(keycode)`
    - Returns a macro that sends a character to the PC while the key is held down, functioning like a regular keyboard key.
    - Works only with supported HID libraries (currently only `Keyboard.h`).

- `mod(keycode, keycode)`
    - Returns a macro that inputs the first argument when the key is tapped and the second argument when the key is held.
    - Works only with supported HID libraries (currently only `Keyboard.h`).

- `PRESS_A, PRESS_B,...`
    - Simple wrappers for the `pressTo()` function.

## About `MacroPad`
- The central class of this library.
    - `init(LayeredKeymap)`
        - Registers a keymap to the macro pad.
        - Must be executed during initialization.
    - `KEYS`
        - An array of `Key` objects corresponding to all keys.
    - `LAYERS`
        - An object of the `Layer` class that manages layers.

## About `Key`
- In this library, each key is assigned a `Key` object, which manages the state of each key.
- Below is a partial excerpt of the interface:

    - `Event` Enumeration
        - Represents events occurring on each key.
            | Event Name       | Condition                                         |
            |------------------|--------------------------------------------------|
            | SINGLE           | Pressed briefly once (mutually exclusive)        |
            | LONG             | Long press (mutually exclusive)                  |
            | DOUBLE           | Double-pressed in a short interval (mutually exclusive) |
            | TAP              | Briefly pressed once                             |
            | HOLD             | Long press                                       |
            | RISING_EDGE      | Key pressed momentarily                          |
            | FALLING_EDGE     | Key released momentarily                         |
            | CHANGE_INPUT     | Key input state changed                          |
            | PRESSED          | While the key is pressed                         |
            | RELEASED         | While the key is released                        |
        - `SINGLE`, `DOUBLE`, and `LONG` operate mutually exclusively (only one occurs at a time).
            - For example, `SINGLE` waits to ensure there’s no double-click before triggering the event after the key is released, while `TAP` triggers the event the moment the key is released.

    - `init(longThreshold, doubleThreshold, holdThreshold, debounceTime)`
        - Specifies the time thresholds for recognizing a long press, double-click, hold, and debounce.
        - Example: `Key::init(1000, 500, 10);`
    - `bool hasOccurred(Key::Event)`
        - Checks whether the specified event has occurred for the key.
        - Example: `key.hasOccurred(Key::Event::SINGLE)`
    - `uint32_t getStateDuration()`
        - Returns the elapsed time in milliseconds since the last input state change.
        - Example: `key.getStateDuration()`
    - `uint8_t getCountOfClick()`
        - Returns how many times the key was clicked in succession.
        - Resets if the interval exceeds `doubleThreshold`.
        - Using this method, you can define individual events for up to 255 successive clicks.
    - `bool isPressed()`
        - Returns whether the key is currently pressed.
        - Equivalent to `hasOccurred(Key::Event::PRESSED)`.
    - `uint32_t getPressTime()`
        - Returns the duration the key has been pressed.
        - Returns `0` if the key is not pressed.
    - `uint16_t getIndex()`
        - Returns the key’s index.

## About Custom Macros
**Note:** _The term "macro" in the context of "`Do` macro" refers to a program executed in response to key events. Unless otherwise stated, "macro" will refer to this program hereafter._

- Custom macros are defined using the `Do` macro.
    - When executed, the macro receives a reference to the `Key` object that triggered it as the `key` argument.
- While custom processing can be implemented internally, it is recommended to keep the implementation as fast as possible since the macro is executed synchronously.
- Macros are executed in ascending order of their index.

- To define macros that take parameters, you must define them in the form of a **function that returns a function (closure)**.

- Example of a custom macro:
```cpp
// A macro that types "Hello, world!" when the key is released
auto greet = Do {
    if (key.hasOccurred(Key::Event::FALLING_EDGE)) {
        Keyboard.println("Hello, world!");
    }
};
// Alternatively, you can define macros using regular functions.
void greet(Key key) {
    if (key.hasOccurred(Key::Event::FALLING_EDGE)) {
        Keyboard.println("Hello, world!");
    }
}
```
- Example of a parameterized custom macro:
```cpp
// A macro that types a specified character while the key is pressed
inline Macro pressTo(uint8_t pressKey) {
    return [pressKey](Key key) {
        if (key.hasOccurred(Key::Event::RISING_EDGE)) {
            Keyboard.press(pressKey);
        } else if (key.hasOccurred(Key::Event::FALLING_EDGE)) {
            Keyboard.release(pressKey);
        }
    };
}
```

#### `macroDelay(ms, func)` Function
- Used to implement delayed processing within a macro.
- Pass the waiting time (in milliseconds) as the first argument and the function to execute as the second argument.
- The second argument’s function can be concisely written using the `After` macro.
- Avoid using the `delay()` function inside macros unless necessary, as it halts all processing. Instead, use this function as a substitute.
- Time progression is determined via polling, so precision may not be high.
- Example: ```macroDelay(1000, After { Keyboard.print("Hello, world!"); });```

### About the Layer Feature
- You can use a specified number of layers when creating an instance of `MacroPad` (up to 255 layers).
- When passing a keymap to the `MacroPad::init()` method, you must provide an array of keymaps for the specified number of layers (see sample code).
- The `Layer` class manages the layers.
    - `set(layer)`
        - Switches to the specified layer.
        - Does nothing if the specified layer does not exist.
        - Example: ```macroPad.LAYERS.set(1)```
    - `reset()`
        - Returns to the previous layer.
        - May not behave as expected if the same layer is set twice consecutively.
    - `uint8_t get()`
        - Returns the current layer index.
- The `LayerUtil` class simplifies creating macros for layer switching.
    - `KeyAssign to(layer)`
        - Returns a macro to switch to the specified layer when the key is pressed.
        - Example: ```layer.to(1)```
    - `KeyAssign back(layer)`
        - Returns a macro to switch to the specified layer when the key is released.
        - Example: ```layer.back(0)```
    - `KeyAssign reset()`
        - Returns a macro to revert to the previous layer.
        - Example: ```layer.reset()```

### Key Input Processing
- Key input in this library is implemented as a plugin system, allowing you to define custom key input algorithms by extending the `KeyReader` class.
    - The instance passed to the `MacroPad` class constructor will be used to read key inputs.

- The data structure that stores key input states is an array of unsigned 32-bit integers, with each bit representing the input state of a key (up to 32 keys per element).
    - The bit index corresponds to the keymap index.
        - For instance, the 0th key in the keymap corresponds to the 0th bit of the 0th array element.

#### `KeyReader` Class
- An abstract class.
- Classes that inherit from this class manage key inputs.
- `uint32_t (&getStateData())[KEYBOARD_SIZE]` Method
    - Returns a reference to the array that stores key input states.
- `void read()` Method
    - Updates the state of key inputs.
    - Called each time the `update()` method of the `MacroPad` instance is invoked.
    - After executing this method, the `MacroPad` instance checks the array returned by the `getStateData()` method and processes key events accordingly.
