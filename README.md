# ゲームプログラマになる前に覚えておきたい技術

[ゲームプログラマになる前に覚えておきたい技術](https://www.shuwasystem.co.jp/book/9784798021188.html)
をやる。

# ビルド方法

```bash
cmake -B build
```

(個人的メモ)

```bash
cmake -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_COMPILER=clang++
```
