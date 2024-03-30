/* ios*/
struct fast_ios {
    fast_ios() {
        ios::sync_with_stdio(false);
        cin.tie(nullptr);
        cout << fixed << setprecision(15);
    }
} fast_ios;

template <int m>
istream &operator>>(istream &is, static_modint<m> &x) {
    ll val;
    is >> val;
    x = val;
    return is;
}

template <int m>
ostream &operator<<(ostream &os, const static_modint<m> &x) {
    os << x.val();
    return os;
}

template <class T1, class T2>
istream &operator>>(istream &is, pair<T1, T2> &pr) {
    is >> pr.first >> pr.second;
    return is;
}

template <class T1, class T2>
ostream &operator<<(ostream &os, const pair<T1, T2> &pr) {
    os << pr.first << ' ' << pr.second;
    return os;
}

template <class... T>
void input(T &...a) {
    (cin >> ... >> a);
}
template <class T>
void input(vector<T> &v) {
    ranges::for_each(v, [](T &x) { cin >> x; });
}

template <class T, class... Ts>
void print(const T &a, const Ts &...b) {
    ((cout << a), ..., (cout << ' ' << b));
    cout << '\n';
}
template <class T>
void print(const vector<T> &v, string sep = " ") {
    if (!v.empty()) {
        auto p = prev(end(v));
        for_each(begin(v), p, [&sep](T x) { cout << x << sep; });
        cout << *p;
    }
    cout << '\n';
}
template <class T>
void print(const vector<T> &v, int l, int r, string sep = " ") {
    if (l < r) {
        auto p = begin(v) + min((int)ssize(v) - 1, r - 1);
        for_each(v.begin() + l, p, [&sep](auto x) { cout << x << sep; });
        cout << *p;
    }
    cout << '\n';
}