# Pico MacroPad ライブラリ
[Click here to view the document in English.](/README.md)

Arduinoでマクロパッド(キーボード)の制御ができるようになるライブラリです。

このライブラリはキー入力の処理とマクロの管理を行うだけなので、HIDとして使用するには任意のHIDライブラリが必要です。(例: [Keyboardライブラリ](https://github.com/arduino-libraries/Keyboard))


このREADMEは`Keyboard.h`を使用する前提になっているため、キーボードの詳しい機能については[Keyboard](https://github.com/arduino-libraries/Keyboard)を確認してください。

__このライブラリはSTLを使用するため、STLが利用できる環境でのみ動作します。__
AVRマイコンなどデフォルトで対応していないプラットフォームの場合、別途ライブラリのインストールなどが必要になる場合があります。

## 概要
- キーマトリクスまたは直接接続したボタン、あるいは任意の配線のキー入力を読み取り、イベントを管理します。
- それぞれのキーにマクロを割り当てることができ、カスタムマクロも定義できます。
- レイヤー機能により、キーマップを簡単に切り替えることが出来ます。
- プロファイル機能により、レイヤーのセットを簡単に切り替えることが出来ます。

## 機能
- `Do`
    - カスタムマクロを簡潔に定義できるようにするマクロです。
    - `Key`型の`key`という引数をとります。
    - 例: ```Do { Keyboard.print("Hello, world!"); }```
- `After`
    - `macroDelay()`関数で遅延した後に実行する内容を簡単に定義できるようにするマクロです。
    - 外側のスコープの`key`をキャプチャします。
    - 例: ```macroDelay(1000, After { Keyboard.print("Hello, world!"); });```

- `NONE`
    - 単なる`nullptr`の別名です。
    - 無効なキー割当を示すときに使えます。

- `pressTo(keycode)`
    - 通常のキーボードのようにそのキーを押している間PCに文字を送信するマクロを返します。
    - 対応しているHIDライブラリ(現在は`Keyboard.h`のみ)でのみ使用できます。

- `mod(keycode, keycode)`
    - キーをタップした際に第一引数のキーを入力し、ホールドした際に第二引数のキーを入力するマクロを返します。
    - 対応しているHIDライブラリ(現在は`Keyboard.h`のみ)でのみ使用できます。

- `PRESS_A, PRESS_B,...`
    - `pressTo()`関数の単純なラッパーです。

## `MacroPad` について
- このライブラリの中心的なクラスです。
    - `init(LayeredKeymap)`
        - マクロパッドにキーマップを登録します。
        - 初期化の際に必ず実行してください。
    - `KEYS`
        - すべてのキーに対応する`Key`オブジェクトの配列です。
    - `LAYERS`
        - レイヤーを管理する`Layer`クラスのオブジェクトです。


## `Key` について
- このライブラリでは各キーに`Key`オブジェクトが割り当てられ、それが各キーの状態を管理します。
- 下記のインターフェースは一部抜粋しています。
    - `Event` 列挙体
        - 各キーで発生しているイベントを表します。
            |イベント名|条件|
            |----|----|
            |SINGLE|短く一回押した時(排他)|
            |LONG|長押しした時(排他)|
            |DOUBLE|短い間隔で2回連打した時(排他)|
            |TAP|短く一回押した時|
            |HOLD|長押しした時|
            |RISING_EDGE|キーが押された瞬間|
            |FALLING_EDGE|キーが離された瞬間|
            |CHANGE_INPUT|キーの入力が切り替わった瞬間|
            |PRESSED|キーが押されている間|
            |RELEASED|キーが離されている間|
        - `SINGLE`, `DOUBLE`, `LONG`は互いに排他的に動作します。(一度にどれか一つしか発生しません。)
            - 例えば、`SINGLE`はキーが離された後ダブルクリックされないか待機してからイベントが発生しますが、`TAP`はキーが離された瞬間にイベントが発生します。

    - `init(longThreshold, doubleThreshold, holdThreshold, debounceTime)`
        - 長押しと判定する時間、ダブルクリックと判定する猶予、ホールドと判定する時間、デバウンス時間を指定します。
        - 例: `Key::init(1000, 500, 10);`
    - `bool hasOccurred(Key::Event)`
        - そのキーで指定したイベントが発生しているかどうかを調べます。
        - 例: `key.hasOccurred(Key::Event::SINGLE)`
    - `uint32_t getStateDuration()`
        - 最後に入力が切り替わってから何ミリ秒経ったかを返します。
        - 例: `key.getStateDuration()`
    - `uint8_t getCountOfClick()`
        - キーが何回連打されたかを表します。
        - `doubleThreshold`以上に間隔が開くとリセットされます。
        - このメソッドを使えば、255連打まで個別にイベントを設定できます。
    - `bool isPressed()`
        - キーが押されているかを返します。
        - `hasOccurred(Key::Event::PRESSED)`と同じです。
    - `uint32_t getPressTime()`
        - キーが押されている時間を返します。
        - 押されていない場合、`0`を返します。
    - `uint16_t getIndex()`
        - キーの番号を返します。

## カスタムマクロについて
※__｢`Do`マクロ｣の｢マクロ｣は`#define`ディレクティブで置換される構文を指します。これ以降、特に断りなく｢マクロ｣といった場合はキーイベントに対応して実行されるプログラムのことを指します。__
- カスタムマクロは`Do`マクロを使用して定義します。
    - マクロは実行される際、実行元の`Key`オブジェクトの参照を`key`引数として受け取ります。
- 内部では好きな処理を行うことが出来ますが、マクロの処理は同期的に行われるため、できるだけ速く処理できるように実装することをおすすめします。
- マクロはインデックスが小さい順に実行されます。

- パラメータを取るマクロを定義したい場合、__マクロとなる関数を返す関数(クロージャ)__ の形で定義する必要があります。

- カスタムマクロの例:
```cpp
// キーが離されたときに"Hello, world!"と入力するマクロ
auto greet = Do {
    if (key.hasOccurred(Key::Event::FALLING_EDGE)) {
        Keyboard.println("Hello, world!");
    }
};
// もちろん、通常の関数を使用して定義することも出来ます。
void greet(Key key) {
    if (key.hasOccurred(Key::Event::FALLING_EDGE)) {
        Keyboard.println("Hello, world!");
    }
}
```
- パラメータを取るカスタムマクロの例
```cpp
// そのキーが押されている間、指定した文字を入力するマクロ
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

#### `macroDelay(ms, func)`関数
- マクロ内で一定時間遅延させたい処理がある場合に使用します。
- 第一引数に待機時間(ミリ秒)、第二引数に実行させたい関数を渡します。
- 第二引数の関数は`After`マクロを使用して定義すると簡潔に表記できます。
- マクロ内で`delay()`関数を使うとすべての処理が止まってしまうため、特別な理由がない限りこの関数で代替してください。
- 時間の経過はポーリング式で判定されるため、精度はあまり高くありません。
- 例: ```macroDelay(1000, After { Keyboard.print("Hello, world!"); });```

### レイヤー機能について
- `MacroPad`のインスタンスを生成したときに指定した数のレイヤーが使用できます。(最大255)
- `MacroPad::init()`メソッドにキーマップを渡す際、指定したレイヤー数分のキーマップの配列が必要です。(サンプルコード参照)
- `Layer`クラスがレイヤーの管理を担当します。
    - `set(layer)`
        - 指定したレイヤーに移動します。
        - 存在しないレイヤーを指定した場合は何もしません。
        - 例: ```macroPad.LAYERS.set(1)```
    - `reset()`
        - 一つ前のレイヤーに戻ります。
        - 同じレイヤーに二度移動した後など、意図したとおりに動作しない場合があります。
    - `uint8_t get()`
        - 現在のレイヤーのインデックスを返します。
- `LayerUtil`クラスを使うことで、レイヤーを切り替えるマクロが簡単に設定できるようになります。
    - `Macro to(layer)`
        - キーを押した時に指定したレイヤーに移動するマクロを返します。
        - 例: ```layer.to(1)```
    - `Macro back(layer)`
        - キーを離したときに指定したレイヤーに移動するマクロを返します。
        - 例: ```layer.back(0)```
    - `Macro reset()`
        - 一つ前のレイヤーに戻るマクロを返します。
        - 例: ```layer.reset()```

### プロファイル機能について
- 使い方は概ねレイヤー機能と同じです。
    - `MacroPad::PROFILES`からアクセスします。

### キー入力の処理について
- このライブラリのキー入力はプラグイン式になっているため、`KeyReader`クラスを継承することでカスタムのキー入力アルゴリズムを定義することが出来ます。
    - `MacroPad`クラスのコンストラクタに渡したインスタンスがキー入力の読み取りに使用されます。

- キー入力を格納するデータ構造は符号なし32ビット整数型の一次配列となっていて、ビットごとに各キーの入力状態を表します。(一要素につき32キーまで)
    - ビットのインデックスはキーマップのインデックスと対応します。
        - つまり、キーマップの0番目のキーは0番目の要素の0ビット目にあたります。

#### `KeyReader`クラス
- 抽象クラスです。
- このクラスを継承したクラスがキー入力を管理します。
- `uint32_t (&getStateData())[KEYBOARD_SIZE]`メソッド
    - キー入力のデータを格納する配列の参照を返します。
- `void read()`メソッド
    - キー入力の状態を更新します。
    - `MacroPad`インスタンスの`update()`メソッドが呼び出されるたびに実行されます。
    - `MacroPad`インスタンスはこのメソッドを実行した後に`getStateData()`メソッドで返された配列を確認し、各キーのイベントをチェックします。
