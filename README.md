# README #

オレオレ言語です。LRパーサから手作りです。バイトコードへコンパイルし、仮想マシン上で実行します。

部誌の記事(pp.1-16): http://www.kitcc.org/share/lime/lime52.pdf

kernelvm名古屋でのスライド: http://www.slideshare.net/matsud224/soramame

### 特徴 ###

* 静的型付き言語（ローカル変数は型推論）
* 並列実行とチャンネル通信
* 継続
* クロージャ
* ユーザ定義演算子
* 末尾呼び出し最適化

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
./language1 sample/qsort.js
```
バイトコードの逆アセンブル：
```
./language1 -b sample/qsort.js
```

Ubuntu15.10(64bit)で動作確認しました。

サンプルプログラムはsampleディレクトリにあります。
sample/sample_readme.txtにサンプルプログラムの説明が書いてあります。
いくつかのサンプルの実行にはGLUTが必要です。