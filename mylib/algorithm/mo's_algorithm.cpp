#include <bits/stdc++.h>
using namespace std;

/*
 * Mo's Algorithm（区間クエリ用）
 *
 * 【使い方】
 *
 * 1. Mo mo(n); を作る（n = 配列サイズ）
 *
 * 2. mo.add_query(l, r);
 *    半開区間 [l, r) を追加する
 *
 * 3. run(...) に以下を渡す：
 *
 *    add_l(i)     : i を左側に追加
 *    add_r(i)     : i を右側に追加
 *    remove_l(i)  : i を左側から削除
 *    remove_r(i)  : i を右側から削除
 *    out(qid)     : 現在区間の答えを保存
 *
 * 4. mo.run(...) を呼ぶと、
 *    全クエリが自動で最適順に処理される
 *
 * ※ add/remove は対称に書くこと
 * ※ クエリはオフライン専用
 */

struct Mo {
    int n;
    vector<pair<int, int>> qs;
    vector<int> order;

    Mo(int n) : n(n) {}

    void add_query(int l, int r) { qs.emplace_back(l, r); }

    template <class AddL, class AddR, class RemoveL, class RemoveR, class Output>
    void run(AddL add_l, AddR add_r, RemoveL rem_l, RemoveR rem_r, Output out) {
        int q = qs.size();
        int block = max(1, int(n / sqrt(q)));

        order.resize(q);
        iota(order.begin(), order.end(), 0);

        sort(order.begin(), order.end(), [&](int a, int b) {
            auto [al, ar] = qs[a];
            auto [bl, br] = qs[b];
            if (al / block != bl / block) return al < bl;
            return (al / block & 1) ? ar > br : ar < br;
        });

        int l = 0, r = 0;

        for (int idx : order) {
            auto [nl, nr] = qs[idx];

            while (l > nl) add_l(--l);
            while (r < nr) add_r(r++);
            while (l < nl) rem_l(l++);
            while (r > nr) rem_r(--r);

            out(idx);
        }
    }
};
