# bluefruit_send_windows

AdafruitのnRf52(Bluefruit)でBLE通信を行いパソコンと通信を行うプログラム．受信プログラム付き

---

## 概要

Feather nRF52840のBLE通信を用いてパソコンに双方向のコミュニケーションを行うためのプログラム．

システムは

``` system
    Feather nRF52840 <= notify & write => PC(python app)
```

という形で双方向の通信を行う．

---

## python appについて

Python Appはbleakというライブラリを用いてBLE通信を行うことにした．
これはpybluzなどと比較してOSの依存度がなく，どのOSでも動作することができるためである．
（動作確認は現状Windowsのみ）

``` commandprompt
pip install bleak
```

でインストールが可能となっている．
実行は

``` commandprompt
python address_check.py
```

でBluetoothのアドレスを取得する．
取得したaddressは/bleakoperation内の__init__メソッド内を変更する．
またBluetoothのGAT(UUID)もoperation内のインスタンスを変更する．
その後コールバックなどの設定を自分にあったものに合わせる．

通信の実行はmain.pyを実行するので

``` commandprompt
python main.py
```

で通信が開始する．
.vscodeの設定でdebugモードで実行もできるはず...(ファイルコピーしたのでパス等は自分のものに変更する必要ある！)

### 参考

1. [Pythonライブラリ bleakでWindows10/macOS/Linux上でtoioコア キューブを動かしてみる - Qiita](https://qiita.com/kenichih/items/8baa27b3aecc94dd8193)
2. [hbldh/bleak: Bluetooth Low Energy platform Agnostic Klient for Python](https://github.com/hbldh/bleak)
3. [bleak Documentation](https://buildmedia.readthedocs.org/media/pdf/bleak/stable/bleak.pdf)
4. [Win10環境下のpythonでBLE通信 - つれづれなる備忘録](https://atatat.hatenablog.com/entry/2020/07/09/003000)
---

## マイコン側について

使用したマイコンは[Adafruit Feather nRF52 Bluefruit LE](https://www.switch-science.com/catalog/3277/)を用いてBLE通信を行った．

BLE通信はwrite(マイコン側から見ると受信)はコールバックでnotifyはメソッドで呼び出すことで通信を行うようになっている
ほとんど記事を参考に作成したので書くことなし．

### 参考

1. [pat09313/testliff(Github)](https://github.com/pat09313/testliff)
2. [3.BLEモジュール、Adafruit_nRF52_Arduino　BLEサービスの作成 - Qiita()](https://qiita.com/Kosuke_Matsui/items/1cb4b60cbc89562c0d7e)
3. [Adafruit Bluefruit nRF52 で複数の BLE コネクションを張るには - Qiita](https://qiita.com/hktechno/items/02a58ff972a8e68db32d)
4. [初めてのBLE (22) Feather nRF52840 Sense④BLE その3 - Ard…](https://www.denshi.club/cookbook/arduino/feather-nrf52840-sense/ble-22-feather-nrf52840-sense4ble-3.html)
