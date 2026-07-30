# 提出用ACL展開コード

`mylib/acl/*.cpp` は、外部ACLを使えないジャッジへ貼り付けるためのコードです。

公式ACLのヘッダごとのinclude guardを保持しているため、例えば`convolution.cpp`と`modint.cpp`をどちらの順で貼り、さらに同じ部品を複数回貼っても再定義されません。

再生成:

```bash
python3 scripts/render_acl_snippets.py
```

一部だけ再生成:

```bash
python3 scripts/render_acl_snippets.py convolution modint
```

生成元は`/usr/lib/ac-library/atcoder/*.hpp`です。`#include <atcoder/...>`を再帰的に展開し、公式のguardは削除しません。
