// 最終更新: 2026-07-28 22:23:00 JST
// 1 <= i <= n に対する floor(n / i) の異なる値と、その値を取る区間を列挙する。
// 異なる商は高々 2*sqrt(n) 個なので、計算量・返却配列はともに O(sqrt(n))。

struct FloorQuotientRange {
    ll quotient;
    ll left;
    ll right;
};

// i の昇順、すなわち quotient の降順で返す。
// 各要素は floor(n / i) = quotient となる i の閉区間 [left, right] を表す。
// n = 0 のときは空配列を返す。
vector<FloorQuotientRange> enumerate_floor_quotient_ranges(ll n) {
    assert(n >= 0);

    vector<FloorQuotientRange> ranges;
    for (ll left = 1; left <= n;) {
        ll quotient = n / left;
        ll right = n / quotient;
        ranges.push_back({quotient, left, right});
        if (right == n) break;
        left = right + 1;
    }
    return ranges;
}

// floor(n / i) の異なる値を昇順で返す。
// n = 0 のときは空配列を返す。
vector<ll> enumerate_floor_quotients(ll n) {
    vector<FloorQuotientRange> ranges = enumerate_floor_quotient_ranges(n);
    vector<ll> quotients;
    quotients.reserve(ranges.size());
    for (auto it = ranges.rbegin(); it != ranges.rend(); ++it) {
        quotients.push_back(it->quotient);
    }
    return quotients;
}
