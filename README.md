# Pico MacroPad Library
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
- In this library, each key is assigned a `Key` object that manages its state.
- Below are some excerpts from its interface:
    - `Event` Enum
        - Represents events occurring for each key.
            | Event Name    | Condition                              |
            |---------------|----------------------------------------|
            | SINGLE        | Short single press (exclusive)        |
            | LONG          | Long press (exclusive)                |
            | DOUBLE        | Double press in quick succession (exclusive)      |
            | TAP           | Short single press                    |
            | RISING_EDGE   | Moment the key is pressed             |
            | FALLING_EDGE  | Moment the key is released            |
            | CHANGE_INPUT  | Moment input changes                  |
            | PRESSED       | While the key is pressed              |
            | RELEASED      | While the key is released             |
        - `SINGLE`, `DOUBLE`, and `LONG` are mutually exclusive. Only one can occur at a time.
            - Specifically, `SINGLE` triggers after the key is released and no double-click occurs, whereas `TAP` triggers the moment the key is released.

    - `init(longThreshold, doubleThreshold, holdThreshold, debounceTime)`
        - Sets thresholds for long press, double-click interval, hold detection, and debounce time.
        - Example: `Key::init(1000, 500, 10);`

    - `bool hasOccurred(Key::Event)`
        - Checks if a specific event has occurred for the key.
        - Example: `key.hasOccurred(Key::Event::SINGLE)`

    - `uint32_t getStateDuration()`
        - Returns the duration in milliseconds since the last input change.
        - Example: `key.getStateDuration()`

    - `uint8_t getCountOfClick()`
        - Returns the number of times the key has been pressed in succession.
        - Resets if the interval exceeds `doubleThreshold`.
        - Can handle events for up to 255 consecutive presses.

    - `bool isPressed()`
        - Returns whether the key is currently pressed.
        - Equivalent to `hasOccurred(Key::Event::PRESSED)`.

    - `uint32_t getPressTime()`
        - Returns the duration the key has been pressed.
        - Returns `0` if the key is not pressed.

    - `uint16_t getIndex()`
        - Returns the index of the key.

## About Custom Macros
※__The term "macro" in "`Do` macro" refers to syntax replaced by the `#define` directive. From here on, "macro" will refer to the program executed in response to key events.__
- Custom macros are defined using the `Do` macro.
    - When executed, the macro receives a reference to the `Key` object of the invoking key as the `key` argument.
- You can implement any desired logic internally, but since macro processing is synchronous, it's recommended to keep the processing time as short as possible.
- Macros are executed in ascending order of their indices.

- To define macros with parameters, you need to create __a function that returns a function (closure)__ as the macro.

### `macroDelay(ms, func)` Function
- Used when you want to delay certain processing in a macro.
- Takes the delay time (milliseconds) as the first argument and the function to execute as the second.
- The function in the second argument can be concisely defined using the `After` macro.
- Avoid using the `delay()` function inside macros as it halts all processing. Use this function instead, unless absolutely necessary.
- Note: The timing accuracy is low as it uses a polling-based approach.
- Example: ```macroDelay(1000, After { Keyboard.print("Hello, world!"); });```

### About Layer Functionality
- The number of layers is determined when creating an instance of `MacroPad`. (Maximum: 255)
- When passing keymaps to the `MacroPad::init()` method, you need an array of keymaps for the specified number of layers (see sample code).
- The `Layer` class manages layers:
    - `set(layer)`
        - Switches to the specified layer.
        - Does nothing if the specified layer does not exist.
        - Example: ```macroPad.LAYERS.set(1)```

    - `reset()`
        - Reverts to the previous layer.
        - May not behave as intended if the same layer was entered multiple times.

    - `uint8_t get()`
        - Returns the index of the current layer.

- The `LayerUtil` class simplifies assigning macros to switch layers:
    - `KeyAssign to(layer)`
        - Returns a macro that switches to the specified layer when the key is pressed.
        - Example: ```layer.to(1)```

    - `KeyAssign back(layer)`
        - Returns a macro that switches to the specified layer when the key is released.
        - Example: ```layer.back(0)```

    - `KeyAssign reset()`
        - Returns a macro that reverts to the previous layer.
        - Example: ```layer.reset()```
