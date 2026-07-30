// 最終更新: 2026-07-12 14:27:44 JST
struct Manacher {
    vector<int> odd;
    vector<int> even;

    // s[l, r) が回文かを O(1) で判定する
    bool is_palindrome(int l, int r) const {
        assert(0 <= l && l <= r && r <= ssize(odd));
        int len = r - l;
        if (len == 0) return true;
        if (len % 2 == 1) {
            return odd[(l + r) / 2] >= len / 2 + 1;
        }
        return even[(l + r) / 2] >= len / 2;
    }
};

template <ranges::random_access_range Range>
    requires ranges::sized_range<Range> && equality_comparable<ranges::range_value_t<Range>>
Manacher manacher(const Range &s) {
    const int n = ssize(s);
    Manacher res{vector<int>(n), vector<int>(n)};

    // odd[i]: 中心 i の奇数長回文の半径（中心を含む）
    for (int i = 0, l = 0, r = -1; i < n; ++i) {
        int k = (i > r ? 1 : min(res.odd[l + r - i], r - i + 1));
        while (i - k >= 0 && i + k < n && s[i - k] == s[i + k]) ++k;
        res.odd[i] = k;
        if (i + k - 1 > r) {
            l = i - k + 1;
            r = i + k - 1;
        }
    }

    // even[i]: i - 1 と i の間を中心とする偶数長回文の半径
    for (int i = 0, l = 0, r = -1; i < n; ++i) {
        int k = (i > r ? 0 : min(res.even[l + r - i + 1], r - i + 1));
        while (i - k - 1 >= 0 && i + k < n && s[i - k - 1] == s[i + k]) ++k;
        res.even[i] = k;
        if (i + k - 1 > r) {
            l = i - k;
            r = i + k - 1;
        }
    }

    return res;
}
