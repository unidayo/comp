template <ranges::random_access_range Range>
    requires equality_comparable<ranges::range_value_t<Range>>
vector<size_t> kmp(const Range &s, const Range &t) {
    // 文字列S中にTが出現する位置を全列挙
    using T = ranges::range_value_t<Range>;
    const size_t N = ranges::size(s);
    const size_t M = ranges::size(t);
    vector<size_t> res;
    if (M == 0) return res;

    vector<size_t> table(M + 1, 0);
    for (size_t i = 1, j = 0; i < M;) {
        if (t[i] == t[j])
            table[++i] = ++j;
        else if (j == 0)
            ++i;
        else
            j = table[j];
    }

    for (size_t i = 0, j = 0; i < N;) {
        if (s[i] == t[j]) {
            ++i;
            ++j;
            if (j == M) {
                res.push_back(i - j);
                j = table[j];
            }
        } else if (j == 0) {
            ++i;
        } else {
            j = table[j];
        }
    }

    return res;
}