// プロファイル機能のサンプル
// Profile feature sample

#include <Keyboard.h>
#define USE_KEYBOARD_H

#include <KeyReader/Matrix.h>
#include <MacroPad.h>

uint8_t rowPins[] = { 0, 1, 2 };
uint8_t colPins[] = { 3, 4, 5, 6 };

constexpr uint8_t NUM_OF_LAYERS   = 2;
constexpr uint8_t NUM_OF_PROFILES = 2;
constexpr uint8_t ROWS            = 3;
constexpr uint8_t COLS            = 4;

auto matrix = Matrix(rowPins, colPins);
MacroPad<matrix.getNumOfKeys(), NUM_OF_LAYERS, NUM_OF_PROFILES> macroPad(matrix);
auto layer = macroPad.getLayerUtil();
auto profile = macroPad.getProfileUtil();

void setup() {
    Keyboard.begin();

    auto test = Do {
        if (key.hasOccurred(Key::Event::SINGLE)) {
            Keyboard.println("single pressed.");
        } else if (key.hasOccurred(Key::Event::DOUBLE)) {
            Keyboard.println("double clicked.");
        } else if (key.hasOccurred(Key::Event::LONG)) {
            Keyboard.println("long pressed.");
            macroDelay(2000, After {
                Keyboard.println("2 seconds have passed.");

                macroDelay(1000, After {
                    Keyboard.println("3 seconds have passed.");
                });
            });
        }
    };
    auto greet = Do {
        if (key.hasOccurred(Key::Event::FALLING_EDGE)) {
            Keyboard.println("Hello, world!");
            if (macroPad.KEYS[3].hasOccurred(Key::Event::PRESSED)) {
                Keyboard.println("The fourth key is pressed.");
            }
        }
    };

    // レイヤー0の状態で11番目のボタンを押すとプロファイル0と1が切り替わる
    // When you press the 11th button while in layer 0, it will switch between profiles 0 and 1.
    LayeredKeymap<matrix.getNumOfKeys(), NUM_OF_LAYERS> profile0 = {{
        {{
            test   , PRESS_A, PRESS_B,
            PRESS_C, PRESS_D, PRESS_E,
            PRESS_F, PRESS_G, PRESS_H,
            PRESS_I, profile.to(1), layer.to(1)
        }},
        {{
            PRESS_J, PRESS_K, PRESS_L,
            PRESS_M, PRESS_N, PRESS_O,
            PRESS_P, PRESS_Q, PRESS_R,
            PRESS_S, PRESS_T, layer.reset()
        }}
    }};
    LayeredKeymap<matrix.getNumOfKeys(), NUM_OF_LAYERS> profile1 = {{
        {{
            greet  , PRESS_T, PRESS_U,
            PRESS_V, PRESS_W, PRESS_X,
            PRESS_Y, PRESS_Z, PRESS_H,
            PRESS_I, profile.to(0), layer.to(1)
        }},
        {{
            PRESS_J, PRESS_K, PRESS_L,
            PRESS_M, PRESS_N, PRESS_O,
            PRESS_P, PRESS_Q, PRESS_R,
            PRESS_S, PRESS_T, layer.reset()
        }}
    }};

    ProfiledLayers<matrix.getNumOfKeys(), NUM_OF_LAYERS, NUM_OF_PROFILES> profiles = { profile0, profile1 };

    macroPad.initWithProfiles(profiles); // プロファイルを登録する
                                         // Register a profile
}

void loop() {
    macroPad.update();
}
