# RECRUIT 日本橋ハーフマラソン 2021〜増刊号〜 農場王X 解法

<https://atcoder.jp/contests/rcl-contest-2021-long/tasks/rcl_contest_2021_long_a>

## ログ

### 2 日目

始めていきたい。

1 位は 1 ケースあたり 4,600,000 点取っているらしい。
かなり多くて驚く。
野菜ひとつが終盤でも 1000 点前後であることを考えるとその 4600 個分である。
166 個目の収穫機が買えてしまう。
収穫機の超連結が前提になっているのだろう。
網目状に収穫機を置けばおいてほぼすべての野菜を全収穫機で収穫できそう。
ループは不要で実際には全域木のようになることを考えると、落ちてきたプランクトンを食べるイソギンチャクのような感じか。

イメージ (16x16, 収穫機 115 個):

```
.###...#....#...
...#.#.###..###.
...#...#....#...
####.###########
...#...#....#.#.
.#.#.#.#..###.#.
.#.#...#....#...
#######.########
...#...#....#...
.###.#.###..###.
...#...#.#..#...
...#...#....#...
################
...#...#....#.#.
.###.###..###.#.
...#...#....#...
```

### 3 日目

収穫機の移動は距離が無関係らしい。
隣接 4 近傍だけだと勝手に勘違いしていました。
以下のようにすれば任意の箇所を 3 手で回収できる。

```
................
................
################
.......#........
.......#........
.......#........
.......#........
################
.......#........
.......#........
.......#...#....
.......#...#....
################
...........#....
................
................
```

とりあえず実装できた。
意図通りに動いてくれている。
1 ケースあたり 950,000 点なので 4.6 倍ほど足りていない。
<https://atcoder.jp/contests/rcl-contest-2021-long/submissions/25676586>


序盤の伸びが悪い気がする。
序盤はスネークゲームみたいにしながらやるのがいい気がする。

ヘビモードを実装した。まだ点数が悪い。
2,600,000 点くらい。

買う収穫機の量を減らしたらすこし増えた。
3,300,000 点くらい。

現在の 1 位は 5,100,000 なのでさらに 1.5 倍くらいしないとだめ。
収穫機を買う費用を加味すると総計 6,000,000 点くらい稼いでいそう。
<https://www.wolframalpha.com/input/?i=let+n+%3D+50+in+%5Csum_i%5En+i%5E3>
