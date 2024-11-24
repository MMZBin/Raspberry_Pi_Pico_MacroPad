// シリアル出力のみを行うサンプル
// Sample for serial output only.

#include <KeyReader/Matrix.h>
#include <MacroPad.h>

uint8_t rowPins[] = { 0, 1, 2 };
uint8_t colPins[] = { 3, 4, 5, 6 };

constexpr uint8_t NUM_OF_LAYERS = 2;
constexpr uint8_t ROWS          = 3;
constexpr uint8_t COLS          = 4;

auto matrix = Matrix(rowPins, colPins);
MacroPad<matrix.getNumOfKeys(), 2> macroPad(matrix);
auto layer = macroPad.getLayerUtil();

void setup() {
    Serial.begin(9600);

    auto test = Do {
        if (key.hasOccurred(Key::Event::SINGLE)) {
            Serial.println("single pressed.");
        } else if (key.hasOccurred(Key::Event::DOUBLE)) {
            Serial.println("double clicked.");
        } else if (key.hasOccurred(Key::Event::LONG)) {
            Serial.println("long pressed.");
            macroDelay(2000, After {
                Serial.println("2 seconds have passed.");

                macroDelay(1000, After {
                    Serial.println("3 seconds have passed.");
                });
            });
        }
    };

    auto greet = Do {
        if (key.hasOccurred(Key::Event::FALLING_EDGE)) {
            Serial.println("Hello, world!");
            if (macroPad.KEYS[3].hasOccurred(Key::Event::PRESSED)) {
                Serial.println("The fourth key is pressed.");
            }
        }
    };

    // レイヤー0のとき1番目のキーが押されたら"test"マクロが実行され、レイヤー1のとき1番目のキーが押されたら"greet"マクロが実行される
    // If the first key is pressed on layer 0, the "test" macro is executed; if the first key is pressed on layer 1, the "greet" macro is executed.
    LayeredKeymap<matrix.getNumOfKeys(), NUM_OF_LAYERS> layers = {{
        //レイヤー0(ベース) Layer 0 (base)
        {{
            test   , NONE   , NONE   ,
            NONE   , NONE   , NONE   ,
            NONE   , NONE   , NONE   ,
            NONE   , NONE   , layer.to(1)
        }},
        //レイヤー1 Layer 1
        {{
            greet  , NONE   , NONE   ,
            NONE   , NONE   , NONE   ,
            NONE   , NONE   , NONE   ,
            NONE   , NONE   , layer.reset()
        }}
    }};

    ProfiledLayers<matrix.getNumOfKeys(), NUM_OF_LAYERS, 1> profiles = { layers };

    macroPad.init(profiles);
}

void loop() {
    macroPad.update();
}