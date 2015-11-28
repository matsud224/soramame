# README #

オレオレ言語です。LRパーサから手作りです(yaccは使ってません)。バイトコードへコンパイルし、仮想マシン上で実行します。

sampleディレクトリにたくさんのサンプルがあります（拡張子がjsなのはシンタックスハイライトしてくれるから）。テトリスやライフゲーム、マインスイーパもあります！

部誌の記事(pp.1-16): http://www.kitcc.org/share/lime/lime52.pdf
kernelvm名古屋でのスライド: http://www.slideshare.net/matsud224/soramame

### 特徴 ###

* 静的型付き言語（ローカル変数は型推論）
* 並列実行とチャンネル通信
* 継続
* クロージャ
* ユーザ定義演算子
* 末尾呼び出し最適化

### Build ###
```
make release_glut_
```
GLUT抜きの方は:
```
make release
```