/*
    このライブラリはSTLを使用しているため、STLをサポートしている環境でのみ動作します。(主に32ビットマイコン)
    AVRマイコンなどデフォルトで対応していないものも専用ライブラリのインストールなどで使用できるようになる可能性があります。
*/
/*
    Since this library uses STL, it will only work in environments that support STL. (Mainly 32-bit microcontrollers)
    Some microcontrollers, such as AVR microcontrollers, which do not support STL by default, may become usable by installing a dedicated library.
*/

// ライブラリの詳しい仕様はドキュメントを確認してください。
// For detailed library specifications, please check the documentation.

#include <Keyboard.h>
#define USE_KEYBOARD_H // 使用するキーボードライブラリを指定(現在はKeyboard.hのみ対応)
                       // 指定しなかった場合、PRESS_Aなどのマクロは使用できません。(使用するライブラリに合わせて自分で実装する必要があります。)
                       // Specify the keyboard library to use (currently only Keyboard.h is supported)
                       // If not specified, macros such as PRESS_A cannot be used. (You will need to implement them yourself according to the library you are using.)

#include <KeyReader/Matrix.h> // キーマトリクス配線の場合。直接接続する場合は"Direct.h"を使用します。
                              // For key matrix wiring. Use "Direct.h" if you want to connect directly.
#include <MacroPad.h>

// マトリクスキーボードの行と列に使用するピンを指定(今回は3x4を想定、4x4の場合は一行無視することでこのサンプルコードのまま試せます。)
// Specify the pins to be used for the rows and columns of the matrix keyboard (3x4 is assumed in this case; if 4x4, you can try this sample code as is by ignoring one row).
uint8_t rowPins[] = { 0, 1, 2 };    // 行 row
uint8_t colPins[] = { 3, 4, 5, 6 }; // 列 col

constexpr uint8_t NUM_OF_LAYERS = 2; // レイヤーの枚数(1~255)
                                     // Number of Layers(1~255)
constexpr uint8_t ROWS          = 3; // 行の数(1~255)
                                     // Number of lines(1~255)
constexpr uint8_t COLS          = 4; // 列の数(1~255)
                                     //Number of columns(1~255)

// マクロパッドに対応するインスタンスを生成
// Create an instance corresponding to the macro pad
auto matrix = Matrix(rowPins, colPins);
MacroPad<matrix.getNumOfKeys(), 2> macroPad(matrix);

// レイヤーの移動を楽にする機能を使用できるようにする(任意)
// Enable functions that make it easier to move layers (optional)
auto layer = macroPad.getLayerUtil();

void setup() {
    Keyboard.begin();

    // キーを長押しと判定する時間、ダブルクリックと判定する猶予、デバウンス時間を設定することが出来ます。
    // You can set the time to judge a key as a long press, grace to judge a key as a double click, and debounce time.
    // Key::init(1000, 500, 200, 10);

    // ｢test｣という名前のカスタムマクロ(関数)を設定する
    // Set up a custom macro (function) named "test"
    auto test = Do {
        // マクロを登録したキーが一回押されたとき、"single pressed."とPCに入力する。
        // When the key on which the macro is registered is pressed once, "single pressed." is input to the PC.
        if (key.hasOccurred(Key::Event::SINGLE)) {
            Keyboard.println("single pressed.");
        // マクロを登録したキーがダブルクリックされたとき、"double clicked."とPCに入力する。
        // When a key with a registered macro is double-clicked, the PC will display "double clicked".
        } else if (key.hasOccurred(Key::Event::DOUBLE)) {
            Keyboard.println("double clicked.");
        // マクロを登録したキーが長押しされたとき"long pressed."とPCに入力し、2秒後に"2 seconds have passed.", その1秒後に"3 seconds have passed."と入力する。
        // When a key with a registered macro is pressed and held for a long time, the PC will enter "long pressed."",
        // followed by "2 seconds have passed." two seconds later and "3 seconds have passed." one second later.
        } else if (key.hasOccurred(Key::Event::LONG)) {
            Keyboard.println("long pressed.");
            macroDelay(2000, After {
                Keyboard.println("2 seconds have passed.");

                macroDelay(1000, After {
                    Keyboard.println("3 seconds have passed.");
                });
                // 注意: After{}ブロックの外に書かれた内容は遅延しません。
                // Note: Content written outside of an After{} block is not delayed.

                // Keyboard.print("Will be printed soon.")
            });
        }
    };

    // ｢greet｣という名前のカスタムマクロ
    // Set up a custom macro (function) named "greet"
    auto greet = Do {
        // マクロを登録したキーが離されたときに"Hello, world!"とPCに入力し、4番目のキーが押されていれば"The fourth key is pressed."とPCに入力する。
        // When the key on which the macro is registered is released, "Hello, world!"" is input to the PC, and if the fourth key is pressed, "The fourth key is pressed".
        if (key.hasOccurred(Key::Event::FALLING_EDGE)) {
            Keyboard.println("Hello, world!");
            if (macroPad.KEYS[3].hasOccurred(Key::Event::PRESSED)) {
                Keyboard.println("The fourth key is pressed.");
            }
        }
    };

    // キーマップを設定する(データ型の都合で{}を2重にする必要があります。)
    // キーの数と同じ要素数の配列をレイヤーの数並べてください。
    // レイヤー0のとき1番目のキーが押されたら"test"マクロが実行、2番目のキーが押されたら"PRESS_A"マクロが実行...のようになっています。
    // 12番目の"layer.to(1)"マクロによってレイヤー1に移動します。
    // 使用しないキーはNONE(nullptr)を指定してください。

    // Set keymap (need to double {} for data type convenience)
    // Arrange the number of layers in an array with the same number of elements as the number of keys.
    // At layer 0, if the first key is pressed, the "test" macro is executed, If the second key is pressed, the "PRESS_A" macro is executed... and so on.
    // The 12th "layer.to(1)" macro moves to layer 1.
    // Specify NONE (nullptr) for keys that are not used.
    LayeredKeymap<matrix.getNumOfKeys(), NUM_OF_LAYERS> layers = {{
        //レイヤー0(ベース) Layer 0 (base)
        {{
            test   , PRESS_A, PRESS_B,
            PRESS_C, PRESS_D, PRESS_E,
            PRESS_F, PRESS_G, PRESS_H,
            PRESS_I, PRESS_J, layer.to(1)
        }},
        //レイヤー1 Layer 1
        {{
            greet  , PRESS_K, PRESS_L,
            PRESS_M, PRESS_N, PRESS_O,
            PRESS_P, PRESS_Q, PRESS_R,
            PRESS_S, mod('a', 'b'), layer.reset()
        }}
    }};

    macroPad.init(layers); // キーマップをマクロパッドに登録する
                           // Register keymap to macro pad.
}

void loop() {
    macroPad.update(); // マクロパッドの状態を更新する(この処理は継続的に呼び出す必要があります。)
                       // Update macro pad status (this process must be called continuously)
}
