# プログラミング言語 soramame #

soramameは以下のような特徴を持つオレオレ言語（自作言語）です。

* 静的型付き言語（ローカル変数は型推論）
* 並列実行とチャンネル通信
* 継続
* クロージャ
* ユーザ定義演算子
* 末尾呼び出し最適化

LRパーサから手作りです。
バイトコードへコンパイルし、仮想マシン上で実行します。


スライド: https://bitbucket.org/matsud224/soramame/downloads/soramame_slide.pdf

soramame言語の解説文書：https://bitbucket.org/matsud224/soramame/downloads/soramame_lime52.pdf


### ビルド ###
```
make release_glut_
```
（GLUTのインストールが必要です）

GLUT抜きでビルド:
```
make release
```

### 実行 ###
```
./soramame sample/qsort.js
```
バイトコードの逆アセンブル：
```
./soramame -b sample/qsort.js
```

Ubuntu15.10(64bit)で動作確認しました。

サンプルプログラムはsampleディレクトリにあります。
sample/sample_readme.txtにサンプルプログラムの説明が書いてあります。

いくつかのサンプルの実行にはGLUTが必要です。
Ubuntuの場合は、
```
sudo apt-get install freeglut3 freeglut3-dev
```
でインストールできます。