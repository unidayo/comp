auto convex_hull(vector<pair<ll, ll>> &XY) {
    const int N = ssize(XY);
    ranges::sort(XY, [](auto p, auto q) {
        if (p.first != q.first) {
            return p.first < q.first;
        }
        return p.second < q.second;
    });
    vector<pair<ll, ll>> qs;

    auto cross = [](pair<ll, ll> a, pair<ll, ll> b, pair<ll, ll> c) {
        return (b.first - a.first) * (c.second - a.second) - (b.second - a.second) * (c.first - a.first);
    };
    int k = 0;
    for (auto p : XY) {
        while (k > 1 && cross(qs[k - 1], qs[k - 2], p) > 0) {
            qs.pop_back();
            k--;
        }
        qs.emplace_back(p);
        k++;
    }
    int t = ssize(qs);
    for (int i = N - 2; i >= 0; i--) {
        auto p = XY[i];
        while (k > t && cross(qs[k - 1], qs[k - 2], p) > 0) {
            qs.pop_back();
            k--;
        }
        qs.emplace_back(p);
        k++;
    }
    qs.pop_back();
    return qs;
}