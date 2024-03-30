#include <bits/stdc++.h>

#include <atcoder/all>

/* namespace */
using namespace std;
using namespace atcoder;

/* data_structure */
template <class T>
using priority_queue_min = priority_queue<T, vector<T>, greater<T>>;
template <class T>
using priority_queue_max = priority_queue<T, vector<T>>;

template <class T>
auto vec2d(size_t n1, size_t n2, T init = T()) {
    return vector(n1, vector(n2, init));
}
template <class T>
auto vec3d(size_t n1, size_t n2, size_t n3, T init = T()) {
    return vector(n1, vector(n2, vector(n3, init)));
}
template <class T>
auto vec4d(size_t n1, size_t n2, size_t n3, size_t n4, T init = T()) {
    return vector(n1, vector(n2, vector(n3, vector(n4, init))));
}
template <class T>
auto evec2d(size_t n1) {
    return vector(n1, vector<T>());
}
template <class T>
auto evec3d(size_t n1, size_t n2) {
    return vector(n1, vector(n2, vector<T>()));
}
template <class T>
auto evec4d(size_t n1, size_t n2, size_t n3) {
    return vector(n1, vector(n2, vector(n3, vector<T>())));
}