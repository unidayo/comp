// 最終更新: 2026-07-12 14:27:44 JST
/*
 * Mo2D
 *
 * 2つの整数パラメータ (x, y) を持つ状態を動かす Mo。
 *
 * 通常の Mo は区間 [l, r) を動かすが、
 * これは状態 (x, y) をクエリごとの目標 (x_i, y_i) に動かす。
 *
 * 【コールバックの意味】
 *
 * inc_x(x, y) : 現在状態 (x, y) から (x+1, y) にする
 * dec_x(x, y) : 現在状態 (x, y) から (x-1, y) にする
 * inc_y(x, y) : 現在状態 (x, y) から (x, y+1) にする
 * dec_y(x, y) : 現在状態 (x, y) から (x, y-1) にする
 *
 * 重要：
 * コールバックに渡される (x, y) は、必ず「移動前の現在状態」。
 *
 * そのため、
 *
 *   --x 済みなのか？
 *   x++ する前なのか？
 *
 * を考えなくてよい。
 *
 * 【初期状態】
 *
 * run の sx, sy で指定する。
 * 省略すると (0, 0) から始まる。
 */
struct Mo2D {
    int max_x;
    vector<pair<int, int>> qs;
    vector<int> order;

    Mo2D(int max_x) : max_x(max_x) {}

    int add_query(int x, int y) {
        int id = (int)qs.size();
        qs.emplace_back(x, y);
        return id;
    }

    int size() const { return (int)qs.size(); }

    template <class IncX, class DecX, class IncY, class DecY, class Output>
    void run(IncX inc_x, DecX dec_x, IncY inc_y, DecY dec_y, Output out, int sx = 0, int sy = 0) {
        int q = (int)qs.size();
        if (q == 0) return;

        int block = max(1, int(max_x / sqrt(q)));

        order.resize(q);
        iota(order.begin(), order.end(), 0);

        sort(order.begin(), order.end(), [&](int a, int b) {
            auto [ax, ay] = qs[a];
            auto [bx, by] = qs[b];

            int ab = ax / block;
            int bb = bx / block;

            if (ab != bb) return ab < bb;
            return (ab & 1) ? ay > by : ay < by;
        });

        int x = sx;
        int y = sy;

        for (int id : order) {
            auto [nx, ny] = qs[id];

            while (x < nx) {
                inc_x(x, y);
                x++;
            }
            while (x > nx) {
                dec_x(x, y);
                x--;
            }
            while (y < ny) {
                inc_y(x, y);
                y++;
            }
            while (y > ny) {
                dec_y(x, y);
                y--;
            }

            out(id);
        }
    }
};
