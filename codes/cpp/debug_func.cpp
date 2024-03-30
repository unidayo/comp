#include <bits/stdc++.h>
using namespace std;

const string colors[] = {"\x1b[35m"};


#ifdef LOCAL

#else
template <class T, class... Ts>
void debprint(const T &a, const Ts &...b) {}
template <class T>
void debvprint(vector<T> &v, string sep = " ") {}
template <class T>
void debvprint(T &v, int l, int r, string sep = " ") {}
#endif

template <class T, class... Ts>
void dprint(const T &a, const Ts &...b) {
    cout << "\x1b[35m";
    ((cout << a), ..., (cout << ' ' << b));
    cout << '\n';
    cout << "\x1b[m";
}
template <class T>
void dprint(vector<T> &v, string sep = " ") {
    cout << "\x1b[35m";
    if (!v.empty()) {
        auto p = prev(end(v));
        for_each(begin(v), p, [&sep](T x) { cout << x << sep; });
        cout << *p;
    }
    cout << '\n';
    cout << "\x1b[m";
}
template <class T>
void dprint(T &v, int l, int r, string sep = " ") {
    cout << "\x1b[35m";
    if (l < r) {
        auto p = v.begin() + (r - 1);
        for_each(v.begin() + l, p, [&sep](auto x) { cout << x << sep; });
        cout << *p;
    }
    cout << '\n';
    cout << "\x1b[m";
}