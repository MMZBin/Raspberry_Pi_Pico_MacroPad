# Pico MacroPad Library

_The English used in this library is based on machine translations from Japanese._

[日本語のREADMEはこちらから](/README.ja.md)

This is a library for controlling a macro pad (keyboard) using Arduino.

This library only handles key input processing and macro management, so you will need a separate HID library to use it as an HID device (e.g., [Keyboard library](https://github.com/arduino-libraries/Keyboard)).

This README assumes the use of `Keyboard.h`, so for detailed information about keyboard functionality, please refer to [Keyboard](https://github.com/arduino-libraries/Keyboard).

__This library uses STL, so it only works in environments where STL is supported.__
For platforms like AVR microcontrollers that do not support STL by default, additional library installation may be required.

# Documentation Translation

## Overview
- Reads key inputs from a key matrix, directly connected buttons, or custom wiring and manages events.
- Each key can be assigned a macro, and custom macros can also be defined.
- Layers enable seamless switching of key mappings.
- Profiles allow easy switching between sets of layers.

## Features

### `Do`
- A macro that enables concise definition of custom macros.
- Accepts a `Key`-type argument named `key`.
- Example:
  ```cpp
  Do { Keyboard.print("Hello, world!"); }
  ```

### `After`
- A macro for easily defining actions to execute after a delay set with the `macroDelay()` function.
- Captures the `key` variable from the outer scope.
- Example:
  ```cpp
  macroDelay(1000, After { Keyboard.print("Hello, world!"); });
  ```

### `NONE`
- Simply an alias for `nullptr`.
- Used to indicate an invalid key assignment.

### `pressTo(keycode)`
- Returns a macro that sends a key's character to the PC while the key is held down, like a standard keyboard.
- Works only with supported HID libraries (currently `Keyboard.h`).

### `mod(keycode, keycode)`
- Returns a macro that sends the first argument's keycode when tapped and the second argument's keycode when held.
- Works only with supported HID libraries (currently `Keyboard.h`).

### `PRESS_A, PRESS_B,...`
- Simple wrappers for the `pressTo()` function.

---

## About `MacroPad`
- The central class of this library.
  - `init(LayeredKeymap)`
    - Registers a keymap with the MacroPad.
    - Must be called during initialization.
  - `KEYS`
    - An array of `Key` objects corresponding to all keys.
  - `LAYERS`
    - Manages layers via the `Layer` class.

---

## About `Key`
- Each key in the library is assigned a `Key` object, which manages its state.
- A selection of the interface is described below:

### `Event` Enumeration
- Represents events occurring on each key:
    | Event Name      | Condition                                |
    |-----------------|------------------------------------------|
    | SINGLE          | Single short press (exclusive)          |
    | LONG            | Long press (exclusive)                  |
    | DOUBLE          | Double press in quick succession (exclusive) |
    | TAP             | Single short press                      |
    | HOLD            | Long press                              |
    | RISING_EDGE     | When the key is pressed                 |
    | FALLING_EDGE    | When the key is released                |
    | CHANGE_INPUT    | When the input state changes            |
    | PRESSED         | While the key is pressed                |
    | RELEASED        | While the key is released               |

- `SINGLE`, `DOUBLE`, and `LONG` are mutually exclusive (only one occurs at a time).
    - For instance, `SINGLE` occurs after waiting to confirm no double press, while `TAP` occurs immediately upon release.

### Key Methods
- `init(longThreshold, doubleThreshold, holdThreshold, debounceTime)`
    - Defines thresholds for detecting long presses, double presses, hold actions, and debounce intervals.
    - Example: `Key::init(1000, 500, 10);`
- `bool hasOccurred(Key::Event)`
    - Checks if the specified event has occurred for the key.
    - Example: `key.hasOccurred(Key::Event::SINGLE)`
- `uint32_t getStateDuration()`
    - Returns the time in milliseconds since the last input state change.
    - Example: `key.getStateDuration()`
- `uint8_t getCountOfClick()`
    - Returns the number of times the key has been clicked.
    - Resets if the interval exceeds `doubleThreshold`.
    - Allows events for up to 255 clicks.
- `bool isPressed()`
    - Returns whether the key is pressed.
    - Equivalent to `hasOccurred(Key::Event::PRESSED)`.
- `uint32_t getPressTime()`
    - Returns the duration the key has been pressed.
    - Returns `0` if the key is not pressed.
- `uint16_t getIndex()`
    - Returns the key's index.

---

## About Custom Macros
> **Note:** The term "macro" in "`Do` macro" refers to syntax replaced by the `#define` directive. Below, "macro" refers to the program executed in response to key events unless stated otherwise.

- Define custom macros using the `Do` macro.
    - Macros receive the `Key` object reference as the `key` argument when executed.
- Macros can perform any desired processing, but since execution is synchronous, they should be implemented for quick processing.
- Macros execute in order of ascending index.

- To define macros that take parameters, define them as **functions that return a macro (closures).**

### Examples of Custom Macros
```cpp
// A macro that types "Hello, world!" when the key is released
auto greet = Do {
    if (key.hasOccurred(Key::Event::FALLING_EDGE)) {
        Keyboard.println("Hello, world!");
    }
};
// Alternatively, use a regular function
void greet(Key key) {
    if (key.hasOccurred(Key::Event::FALLING_EDGE)) {
        Keyboard.println("Hello, world!");
    }
}
```

#### Parameterized Macro Example
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

---

### `macroDelay(ms, func)` Function
- Use this function to introduce delays within macros.
- Takes two arguments: wait time (in milliseconds) and the function to execute.
- Use the `After` macro to define the second argument concisely.
- Avoid using `delay()` in macros, as it halts all processing. Use `macroDelay()` instead unless absolutely necessary.
- Time is determined via polling, so precision is limited.
- Example:
  ```cpp
  macroDelay(1000, After { Keyboard.print("Hello, world!"); });
  ```

---

### Layer Features
- `MacroPad` supports up to 255 layers.
- When passing a keymap to `MacroPad::init()`, provide an array of keymaps for the desired number of layers.
- `Layer` class manages layers:
    - `set(layer)`
        - Switches to the specified layer.
        - Does nothing if the layer does not exist.
        - Example: `macroPad.LAYERS.set(1)`
    - `reset()`
        - Reverts to the previous layer.
        - May behave unexpectedly if a layer is revisited multiple times.
    - `uint8_t get()`
        - Returns the current layer's index.

- The `LayerUtil` class simplifies layer-switching macro creation:
    - `Macro to(layer)`
        - Returns a macro to switch to the specified layer when pressed.
        - Example: `layer.to(1)`
    - `Macro back(layer)`
        - Returns a macro to switch to the specified layer when released.
        - Example: `layer.back(0)`
    - `Macro reset()`
        - Returns a macro to revert to the previous layer.
        - Example: `layer.reset()`

---

### Profile Features
- Usage is similar to layers.
    - Use `MacroPad::initWithProfiles()` instead of `MacroPad::init()` and pass a `ProfiledLayers` type (array of layers).
    - Access profiles through `MacroPad::PROFILES`.

---

### Key Input Processing
- The library uses a plugin-based system for key input. You can define custom input algorithms by inheriting from the `KeyReader` class.
    - Pass the custom instance to the `MacroPad` constructor for custom key input processing.

- Key states are stored as an array of unsigned 32-bit integers, with each bit representing a key's input state (up to 32 keys per array element).
    - The bit index corresponds to the keymap index.
        - For example, the 0th key in the keymap corresponds to the 0th bit of the 0th array element.

#### `KeyReader` Class
- Abstract class for managing key input.
- `uint32_t (&getStateData())[KEYBOARD_SIZE]`
    - Returns a reference to the array storing key input data.
- `void read()`
    - Updates key states.
    - Called each time the `update()` method of the `MacroPad` instance is invoked.
    - The `MacroPad` instance verifies the key events after executing this method and checks the array returned by `getStateData()`.