#pragma GCC optimize "Ofast"

#define NDEBUG

#include <bits/stdc++.h>

using namespace std;

#define JOIN(x, y) x ## y
#define likely(x) (__builtin_expect(!!(x), 1))
#define unlikely(x) (__builtin_expect(!!(x), 0))
#define UNREACHABLE Utility::exit_with_message(Utility::concat("[",__func__,": ",__LINE__,"] unreachable"));
#define UNIMPLEMENTED Utility::exit_with_message(Utility::concat("[",__func__,": ",__LINE__,"] unimplemented"));
#define EXIT_DEBUG(...) Utility::exit_with_message(Utility::concat("[",__func__,": ",__LINE__,"] ") + Utility::concat_with_space(__VA_ARGS__))
#define GET_VAL_NAME(...) #__VA_ARGS__
#define DUMP_TO(os, ...) Utility::dump_to(os, __VA_ARGS__)
#define DUMP(...) Utility::dump(__VA_ARGS__)
#define EDUMP(...) Utility::edump(__VA_ARGS__)
#define PRINT_TO(os, ...) os << GET_VAL_NAME(__VA_ARGS__) << ": ", Utility::print_to(os, __VA_ARGS__)
#define PRINT(...) cout << GET_VAL_NAME(__VA_ARGS__) << ": ", Utility::print(__VA_ARGS__)
#define EPRINT(...) cerr << GET_VAL_NAME(__VA_ARGS__) << ": ", Utility::eprint(__VA_ARGS__)
#define rep(i,n) for(int i=0, JOIN($,i)=static_cast<int>(n); i<JOIN($,i); ++i)
#define repr(i,l,r) for(int i=static_cast<int>(l), JOIN($,i)=static_cast<int>(r); i<JOIN($,i); ++i)
#define revrep(i,n) for(int i=static_cast<int>(n)-1; (i)>=0; --(i))
#define all(v) (v).begin(), (v).end()
#define rall(v) (v).rbegin(), (v).rend()

template<class T> using reverse_priority_queue = priority_queue<T, vector<T>, greater<T>>;
template<class T> void chmax(T& a, const T& b) { a = max(a, b); }
template<class T> void chmin(T& a, const T& b) { a = min(a, b); }
template<class T> T min(const deque<T>& d) { return *min_element(d.begin(), d.end()); }
template<class T> T max(const deque<T>& d) { return *max_element(d.begin(), d.end()); }
template<class T> T min(const vector<T>& v) { return *min_element(v.begin(), v.end()); }
template<class T> T max(const vector<T>& v) { return *max_element(v.begin(), v.end()); }
template<class T, size_t N> constexpr T min(const array<T, N>& a) { return *min_element(a.begin(), a.end()); }
template<class T, size_t N> constexpr T max(const array<T, N>& a) { return *max_element(a.begin(), a.end()); }
template<class... Args> constexpr auto min(const Args&... args) { return min(initializer_list<common_type_t<Args...>>{args...}); }
template<class... Args> constexpr auto max(const Args&... args) { return max(initializer_list<common_type_t<Args...>>{args...}); }
template<class T> T sum(const deque<T>& d) { return accumulate(d.begin(), d.end(), static_cast<T>(0)); }
template<class T> T sum(const vector<T>& v) { return accumulate(v.begin(), v.end(), static_cast<T>(0)); }
template<class T, size_t N> T sum(const array<T, N>& a) { return accumulate(a.begin(), a.end(), static_cast<T>(0)); }
template<class T> T sum(const initializer_list<T>& t) { return accumulate(t.begin(), t.end(), static_cast<T>(0)); }
template<class... Args> auto sum(const Args&... args) { return sum(initializer_list<common_type_t<Args...>>{args...}); }

template<class T> istream& operator >> (istream& is, list<T>& l) {
    for(auto& i : l) is >> i;
    return is;
}
template<class T> istream& operator >> (istream& is, deque<T>& d) {
    for(auto& i : d) is >> i;
    return is;
}
template<class T> istream& operator >> (istream& is, vector<T>& v) {
    for(auto& i : v) is >> i;
    return is;
}
template<class T, size_t N> istream& operator >> (istream& is, array<T, N>& a) {
    for(auto& i : a) is >> i;
    return is;
}
template<class T, class S> istream& operator >> (istream& is, pair<T, S>& p) {
    is >> p.first >> p.second;
    return is;
}

template<class T> ostream& operator << (ostream& os, const list<T>& l);
template<class T> ostream& operator << (ostream& os, const deque<T>& d);
template<class T> ostream& operator << (ostream& os, const vector<T>& v);
template<class T> ostream& operator << (ostream& os, const vector<vector<T>>& v);
template<class T, size_t N> ostream& operator << (ostream& os, const array<T, N>& a);
template<class T, size_t N, size_t M> ostream& operator << (ostream& os, const array<array<T, M>, N>& a);
template<class T> ostream& operator << (ostream& os, const set<T>& s);
template<class T, class S> ostream& operator << (ostream& os, const map<T,S>& m);
template<class T, class S> ostream& operator << (ostream& os, const pair<T, S>& p);

template<class T> ostream& operator << (ostream& os, const list<T>& l) {
    if (l.empty()) return os;
    os << l.front();
    for(auto itr = ++l.begin(); itr != l.end(); ++itr) os << " " << *itr;
    return os;
}
template<class T> ostream& operator << (ostream& os, const deque<T>& d) {
    if (d.empty()) return os;
    os << d.front();
    for(auto itr = d.begin()+1; itr != d.end(); ++itr) os << " " << *itr;
    return os;
}
template<class T> ostream& operator << (ostream& os, const vector<T>& v) {
    if (v.empty()) return os;
    os << v.front();
    for(auto itr = v.begin()+1; itr != v.end(); ++itr) os << " " << *itr;
    return os;
}
template<class T> ostream& operator << (ostream& os, const vector<vector<T>>& v) {
    if (v.empty()) return os;
    os << v.front();
    for(auto itr = v.begin()+1; itr != v.end(); ++itr) os << endl << *itr;
    return os;
}
template<class T, size_t N> ostream& operator << (ostream& os, const array<T, N>& a) {
    if (N == 0) return os;
    os << a.front();
    for(auto itr = a.begin()+1; itr != a.end(); ++itr) os << " " << *itr;
    return os;
}
template<class T, size_t N, size_t M> ostream& operator << (ostream& os, const array<array<T, M>, N>& a) {
    if (N == 0) return os;
    os << a.front();
    for(auto itr = a.begin()+1; itr != a.end(); ++itr) os << endl << *itr;
    return os;
}
template<class T> ostream& operator << (ostream& os, const set<T>& s) {
    if (s.empty()) return os;
    os << *s.begin();
    for(auto itr = ++s.begin(); itr != s.end(); ++itr) os << " " << *itr;
    return os;
}
template<class T, class S> ostream& operator << (ostream& os, const map<T,S>& m) {
    if (m.empty()) return os;
    os << m.begin()->first << " " << m.begin()->second;
    for(const auto& [key, val] : m) os << endl << key << " " << val;
    return os;
}
template<class T, class S> ostream& operator << (ostream& os, const pair<T, S>& p) {
    os << p.first << " " << p.second;
    return os;
}

namespace DirectionNS {
    using DirectionType = unsigned char;

    // [Caution] DO NOT change the following values
    constexpr DirectionType UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3;

    class Direction;

    class DirectionBase {
        DirectionType dir;
        constexpr DirectionBase() : dir() {}
        constexpr explicit DirectionBase(DirectionType dir) : dir(dir) {}

        friend Direction;
    public:
        constexpr bool operator==(const DirectionBase& another) const { return dir == another.dir; }
        constexpr bool operator!=(const DirectionBase& another) const { return dir != another.dir; }
        constexpr bool operator==(const Direction& another) const;
        constexpr bool operator!=(const Direction& another) const;


        [[nodiscard]] constexpr int dx() const { return (dir == RIGHT ? 1 : (dir == LEFT ? -1 : 0)); }
        [[nodiscard]] constexpr int dy() const { return (dir == DOWN  ? 1 : (dir == UP   ? -1 : 0)); }

        [[nodiscard]] constexpr char to_char() const {
            if (dir == UP) return 'U';
            else if (dir == RIGHT) return 'R';
            else if (dir == DOWN) return 'D';
            else return 'L';
        }

        constexpr void rotate_cw(unsigned int times)   { dir = (static_cast<unsigned int>(dir) + times  ) & 3; }
        constexpr void reverse(unsigned int times)     { dir = (static_cast<unsigned int>(dir) + times*2) & 3; }
        constexpr void rotate_ccw(unsigned int times)  { dir = (static_cast<unsigned int>(dir) + times*3) & 3; }
        constexpr void rotate_cw()    { rotate_cw(1);  }
        constexpr void reverse()      { reverse(1);    }
        constexpr void rotate_ccw()   { rotate_ccw(1); }
        [[nodiscard]] constexpr DirectionBase get_dir_rotated_cw(int times)   const { return DirectionBase((static_cast<unsigned int>(dir) + times  ) & 3); }
        [[nodiscard]] constexpr DirectionBase get_dir_reversed(int times)     const { return DirectionBase((static_cast<unsigned int>(dir) + times*2) & 3); }
        [[nodiscard]] constexpr DirectionBase get_dir_rotated_ccw(int times)  const { return DirectionBase((static_cast<unsigned int>(dir) + times*3) & 3); }
        [[nodiscard]] constexpr DirectionBase get_dir_rotated_cw()    const { return get_dir_rotated_cw(1); }
        [[nodiscard]] constexpr DirectionBase get_dir_reversed()      const { return get_dir_reversed(1); }
        [[nodiscard]] constexpr DirectionBase get_dir_rotated_ccw()   const { return get_dir_rotated_ccw(1); }

        [[nodiscard]] constexpr bool is_vertical()   const { return (dir == UP)    || (dir == DOWN); }
        [[nodiscard]] constexpr bool is_horizontal() const { return (dir == RIGHT) || (dir == LEFT); }
    };

    class Direction {
        DirectionBase dir;

        friend DirectionBase;
    public:
        static constexpr DirectionBase U{UP};
        static constexpr DirectionBase R{RIGHT};
        static constexpr DirectionBase D{DOWN};
        static constexpr DirectionBase L{LEFT};

        static constexpr array<DirectionBase, 4> All{U, R, D, L};

        constexpr Direction() : dir() {}
        constexpr Direction(DirectionBase dir) : dir(dir) {}

        constexpr bool operator==(const Direction& another) const { return dir == another.dir; }
        constexpr bool operator==(const DirectionBase& another) const { return dir == another; }
        constexpr bool operator!=(const Direction& another) const { return dir != another.dir; }
        constexpr bool operator!=(const DirectionBase& another) const { return dir != another; }

        [[nodiscard]] constexpr int dx() const { return dir.dx(); }
        [[nodiscard]] constexpr int dy() const { return dir.dy(); }
        [[nodiscard]] constexpr char to_char() const { return dir.to_char(); }

        constexpr void rotate_cw(unsigned int times)   { dir.rotate_cw(times);  }
        constexpr void reverse(unsigned int times)     { dir.reverse(times);    }
        constexpr void rotate_ccw(unsigned int times)  { dir.rotate_ccw(times); }
        constexpr void rotate_cw()    { rotate_cw(1);  }
        constexpr void reverse()      { reverse(1);    }
        constexpr void rotate_ccw()   { rotate_ccw(1); }
        [[nodiscard]] constexpr Direction get_dir_rotated_cw(int times)   const { return dir.get_dir_rotated_cw(times);  }
        [[nodiscard]] constexpr Direction get_dir_reversed(int times)     const { return dir.get_dir_reversed(times);    }
        [[nodiscard]] constexpr Direction get_dir_rotated_ccw(int times)  const { return dir.get_dir_rotated_ccw(times); }
        [[nodiscard]] constexpr Direction get_dir_rotated_cw()    const { return get_dir_rotated_cw(1); }
        [[nodiscard]] constexpr Direction get_dir_reversed()      const { return get_dir_reversed(1); }
        [[nodiscard]] constexpr Direction get_dir_rotated_ccw()   const { return get_dir_rotated_ccw(1); }

        [[nodiscard]] constexpr bool is_vertical()   const { return dir.is_vertical();   }
        [[nodiscard]] constexpr bool is_horizontal() const { return dir.is_horizontal(); }

        static constexpr Direction from_char(char c) {
            return (c == 'U' ? U : (c == 'R' ? R : (c == 'D' ? D : L)));
        }
        static constexpr Direction from_char_or(char c, Direction default_dir) {
            return (c == 'U' ? U : (c == 'R' ? R : (c == 'D' ? D : (c == 'L' ? L : default_dir))));
        }
        static constexpr optional<Direction> from_char_safely(char c) {
            if (c == 'U') return U;
            else if (c == 'R') return R;
            else if (c == 'D') return D;
            else if (c == 'L') return L;
            else return nullopt;
        }
    };

    class Path : public vector<Direction> {
    public:
        static Path from_string(const string& s) {
            Path result; result.reserve(s.size());
            for(const auto& c : s) result.push_back(Direction::from_char(c));
            return result;
        }
        static pair<Path, string> from_string_safely(const string& s) {
            Path result; result.reserve(s.size());
            for(int i=0, n=(int)s.size(); i<n; ++i) {
                auto op_dir = Direction::from_char_safely(s[i]);
                if (op_dir) result.push_back(op_dir.value());
                else return {result, s.substr(i,n-i)};
            }
            return {result, ""};
        }
        [[nodiscard]] string to_string() const {
            string result; result.reserve(this->size());
            for(const auto& dir : *this) result.push_back(dir.to_char());
            return result;
        }
    };

    constexpr bool DirectionBase::operator==(const Direction& another) const {
        return dir == another.dir.dir;
    }
    constexpr bool DirectionBase::operator!=(const Direction& another) const {
        return dir != another.dir.dir;
    }

    ostream& operator << (ostream& os, const DirectionBase& dir) {
        os << dir.to_char();
        return os;
    }
    ostream& operator << (ostream& os, const Direction& dir) {
        os << dir.to_char();
        return os;
    }
    ostream& operator << (ostream& os, const Path& path) {
        os << path.to_string();
        return os;
    }
}
using DirectionNS::Direction;
using DirectionNS::Path;

template<class T = double> struct Vec2 {
    T x, y;

    constexpr Vec2() : x(0), y(0) {}
    template<class N, class M> constexpr Vec2(N x, M y) : x(static_cast<T>(x)), y(static_cast<T>(y)) {}
    constexpr explicit Vec2(DirectionNS::DirectionBase dir) : x(dir.dx()), y(dir.dy()) {}
    constexpr explicit Vec2(Direction dir) : x(dir.dx()), y(dir.dy()) {}

    template<class S> constexpr explicit operator Vec2<S>() const { return Vec2<S>(x, y); }

    constexpr bool operator< (const Vec2<T>& another) const { return x == another.x ? y < another.y : x < another.x; }
    constexpr bool operator> (const Vec2<T>& another) const { return another < *this; }
    constexpr bool operator<=(const Vec2<T>& another) const { return !(*this > another); }
    constexpr bool operator>=(const Vec2<T>& another) const { return !(*this < another); }
    constexpr bool operator==(const Vec2<T>& another) const { return x == another.x && y == another.y; }
    constexpr bool operator!=(const Vec2<T>& another) const { return !(*this == another); }

    constexpr Vec2<T> operator+(const Vec2<T>& another) const { return { x + another.x, y + another.y }; }
    constexpr Vec2<T> operator-(const Vec2<T>& another) const { return { x - another.x, y - another.y }; }
    constexpr Vec2<T> operator*(const Vec2<T>& another) const { return { x * another.x, y * another.y }; }
    constexpr Vec2<T> operator/(const Vec2<T>& another) const { return { x / another.x, y / another.y }; }
    constexpr Vec2<T> operator%(const Vec2<T>& another) const { return { x % another.x, y % another.y }; }
    constexpr Vec2<T> operator+(T s) const { return { x+s, y+s }; }
    constexpr Vec2<T> operator-(T s) const { return { x-s, y-s }; }
    constexpr Vec2<T> operator*(T s) const { return { x*s, y*s }; }
    constexpr Vec2<T> operator/(T s) const { return { x/s, y/s }; }
    constexpr Vec2<T> operator%(T s) const { return { x%s, y%s }; }

    constexpr Vec2<T>& operator+=(const Vec2<T>& another) { *this = *this+another; return *this; }
    constexpr Vec2<T>& operator-=(const Vec2<T>& another) { *this = *this-another; return *this; }
    constexpr Vec2<T>& operator*=(const Vec2<T>& another) { *this = *this*another; return *this; }
    constexpr Vec2<T>& operator/=(const Vec2<T>& another) { *this = *this/another; return *this; }
    constexpr Vec2<T>& operator%=(const Vec2<T>& another) { *this = *this%another; return *this; }
    constexpr Vec2<T>& operator+=(T s) { *this = *this+s; return *this; }
    constexpr Vec2<T>& operator-=(T s) { *this = *this-s; return *this; }
    constexpr Vec2<T>& operator*=(T s) { *this = *this*s; return *this; }
    constexpr Vec2<T>& operator/=(T s) { *this = *this/s; return *this; }
    constexpr Vec2<T>& operator%=(T s) { *this = *this%s; return *this; }

    constexpr T dot  (const Vec2<T>& another) { return x * another.x + y * another.y; }
    constexpr T cross(const Vec2<T>& another) { return x * another.y - y * another.x; }

    constexpr T square() { return this->dot(*this); }

    constexpr double norm() { return sqrt(square()); }

    [[nodiscard]] T calc_mh_dist(const Vec2<T>& other) const { return abs(x-other.x) + abs(y-other.y); }
    [[nodiscard]] double calc_ec_dist(const Vec2<T>& other) const { return (*this - other).norm(); }
    [[nodiscard]] constexpr bool in_area(const Vec2<T>& area) const { return 0 <= x && x < area.x && 0 <= y && y < area.y; }

    [[nodiscard]] constexpr Vec2<T> xx() const { return {x, x}; }
    [[nodiscard]] constexpr Vec2<T> yx() const { return {y, x}; }
    [[nodiscard]] constexpr Vec2<T> yy() const { return {y, y}; }
};
template<class T> ostream& operator<<(ostream& os, const Vec2<T> v) {
    os << v.x << ' ' << v.y;
    return os;
}
template<class T> istream& operator>>(istream& is, Vec2<T>& v) {
    is >> v.x >> v.y;
    return is;
}

struct Pos : public Vec2<int> {
    constexpr Pos() : Vec2<int>() {}
    constexpr Pos(Vec2<int> p) : Vec2<int>(p) {}
    template<class N, class M> constexpr Pos(N x, M y) : Vec2<int>(x,y) {}
    constexpr explicit Pos(DirectionNS::DirectionBase dir) : Vec2<int>(dir) {}
    constexpr explicit Pos(Direction dir) : Vec2<int>(dir) {}

    constexpr void move(Direction dir) { *this += Pos(dir); }
    constexpr void move(const Pos& dp) { *this += dp; }
              void move(const Path& path) { for(const auto& dir : path) move(dir); }
    constexpr void move_toraly(Direction dir, const Pos& area) {
        move(dir);
        x = (x == -1 ? area.x-1 : (x==area.x ? 0 : x));
        y = (y == -1 ? area.y-1 : (y==area.y ? 0 : y));
    }
    constexpr void move_toraly(const Pos& dp, const Pos& area) {
        *this += (dp%area);
        x = (x + area.x) % area.x;
        y = (y + area.y) % area.y;
    }
              void move_toraly(const Path& path, const Pos& area) { for(const auto& dir : path) move_toraly(dir, area); }

    constexpr Pos get_moved_pos(Direction dir)    const { Pos p = *this; p.move(dir ); return p; }
    constexpr Pos get_moved_pos(const Pos& dp)    const { Pos p = *this; p.move(dp); return p; }
              Pos get_moved_pos(const Path& path) const { Pos p = *this; p.move(path); return p; }
    constexpr Pos get_moved_toraly_pos(Direction dir,    const Pos& area) const { Pos p = *this; p.move_toraly(dir,  area); return p; }
    constexpr Pos get_moved_toraly_pos(const Pos& dp,    const Pos& area) const { Pos p = *this; p.move_toraly(dp,   area); return p; }
              Pos get_moved_toraly_pos(const Path& path, const Pos& area) const { Pos p = *this; p.move_toraly(path, area); return p; }

    [[nodiscard]] int calc_mh_dist_toraly(const Pos& other, const Pos& area) const { return min(area.x-abs(x-other.x), abs(x-other.x)) + min(area.y-abs(y-other.y), abs(y-other.y)); }

    [[nodiscard]] constexpr int to_idx(int w) const { return y * w + x; }
    [[nodiscard]] constexpr int to_idx(const Pos& shape) const { return y * shape.x + x; }
    static constexpr Pos from_idx(int idx, int w) { return {idx%w, idx/w}; }
    static constexpr Pos from_idx(int idx, const Pos& shape) { return {idx%shape.x, idx/shape.x}; }
};

template<class T>
class Grid {
    size_t w,h;
    vector<T> v;
public:
    Grid() : w(), h(), v() {}
    Grid(size_t width, size_t height) : w(width), h(height), v(w*h) {}
    Grid(initializer_list<initializer_list<T>> init) : w(), h(), v() {
        h = init.size();
        for(const auto& i : init) w = w < i.size() ? i.size() : w;
        v.reserve(h*w);
        T* tmp = &v[0];
        for(const auto& i : init) {
            T* p = tmp;
            for(const auto& j : i) *p++ = j;
            tmp += w;
        }
    }
    void resize(size_t width, size_t height) {
        w = width;
        h = height;
        v.resize(h*w);
    }
    size_t height() const { return h; }
    size_t width()  const { return w; }
    size_t size()   const { return h*w; }
    void assign(size_t i, size_t j, const T& value) { v[i*w+j] = value; }
    void assign(const Pos& p, const T& value) { v[p.y*w+p.x] = value; }
    T* operator[](size_t idx) { return &v[idx*w]; }
    T& operator[](const Pos& p) { return v[p.y*w+p.x]; }
    const T* operator[](size_t idx) const { return &v[idx*w]; }
    const T& operator[](const Pos& p) const { return v[p.y*w+p.x]; }
    auto begin() { return v.begin(); }
    auto end()  { return v.begin() + w*h+1; }
};

template<class T> ostream& operator<<(ostream& os, const Grid<T> g) {
    for(int i=0, h=g.height(); i<h; ++i) {
        if (i) os << endl;
        for(int j=0, w=g.width(); j<w; ++j) {
            if (j) os << " ";
            os << g[i][j];
        }
    }
    return os;
}
template<class T> istream& operator>>(istream& is, Grid<T>& g) {
    for(int i=0, h=g.height(); i<h; ++i) {
        for(int j=0, w=g.width(); j<w; ++j) {
            is >> g[i][j];
        }
    }
    return is;
}

namespace Random {
    using uint = unsigned int;

    uint seed[4] = {1,2,3,4};

    void set_random_state(uint s) {
        const uint magic_number = 1812433253;
        for(int i=1; i<=4; i++){
            seed[i-1] = s = magic_number * (s^(s>>30U)) + (uint)i;
        }
    }
    uint rand_uint() {
        uint t = (seed[0]^(seed[0]<<11U));
        seed[0]=seed[1];
        seed[1]=seed[2];
        seed[2]=seed[3];
        return seed[3]=(seed[3]^(seed[3]>>19U))^(t^(t>>8U));
    }
    double rand_prob() { return (double)rand_uint() / ((double)UINT_MAX+1.); }
    uint rand_range(uint r) { return rand_uint()%r; }
    int rand_range(int l, int r) { assert(l < r); return static_cast<int>(rand_uint() % static_cast<uint>(r-l)) + l; }
    double simple_exp_rand() { return pow(2, rand_prob())-1; }
    uint simple_exp_rand(uint r) { return (simple_exp_rand())*r; }
    double exp_rand(double a) { return pow(simple_exp_rand(), a); }
    uint exp_rand(uint r, double a) { return exp_rand(a) * r; }
    bool rand_bool() { return rand_uint()&1; }

    template<class T> T& rand_elem(vector<T>& v) { return v[Random::rand_range(v.size())]; }
}

namespace Utility {
    void sleep_sec(size_t time) { this_thread::sleep_for(chrono::seconds(time)); }
    void sleep_millsec(size_t time) { this_thread::sleep_for(chrono::milliseconds(time)); }

    template<class T, size_t N, typename enable_if<(N > 0), nullptr_t>::type = nullptr> struct ndvector_impl { using type = vector<typename ndvector_impl<T, N - 1>::type>; };
    template<class T> struct ndvector_impl<T, 1> { using type = vector<T>; };
    template<class T, size_t N> using ndvector = typename ndvector_impl<T, N>::type;

    template<class T, size_t N, size_t... Ns> struct ndarray_impl { using type = array<typename ndarray_impl<T, Ns...>::type, N>; };
    template<class T, size_t N> struct ndarray_impl<T,N> { using type = array<T, N>; };
    template<class T, size_t... Ns> using ndarray = typename ndarray_impl<T, Ns...>::type;


    template<class T> T gen_vec() { return T(); }
    template<class T> T gen_vec_by(T init) { return init; }
    template<class T, class N, class... Ns> auto gen_vec(N n, Ns... ns) {
        auto tmp = gen_vec<T, Ns...>(ns...);
        return vector<decltype(tmp)>(n, tmp);
    }
    template<class T, class N, class... Ns> auto gen_vec_by(T init, N n, Ns... ns) {
        auto tmp = gen_vec_by<T, Ns...>(init, ns...);
        return vector<decltype(tmp)>(n, tmp);
    }
    template<class T> constexpr T gen_arr() { return T(); }
    template<class T> constexpr T gen_arr(T ini) { return static_cast<T>(ini); }
    template<class T, size_t x, size_t... xs> constexpr auto gen_arr() {
        auto tmp = gen_arr<T, xs...>();
        array<decltype(tmp), x> result{};
        for(auto& var : result) var = tmp;
        return result;
    }
    template<class T, size_t x, size_t... xs> constexpr auto gen_arr(T ini) {
        auto tmp = gen_arr<T, xs...>(ini);
        array<decltype(tmp), x> result{};
        for(auto& var : result) var = tmp;
        return result;
    }

    template<class... T> void input_from(istream& is, T&... a) { (is >> ... >> a); }
    template<class... T> void input(T&... a) { input_from(cin, a...); }

    void print_wall(ostream& os, const string& msg, size_t size=40) {
        size_t msg_size = msg.size();
        chmax(size, msg_size+6);
        size_t l = (size-msg_size)/2 - 2, r = (size-msg_size)/2-2;
        os << ' ';
        rep(i, l) os << '=';
        if (msg.empty()) os << "==";
        else os << ' ' << msg << ' ';
        rep(i, r) os << '=';
        os << ' ';
        os << endl;
    }
    void print_wall(ostream& os, size_t size=40) {
        chmax(size, (size_t)3);
        os << ' ';
        rep(_, size-2) os << '=';
        os << ' ';
        os << endl;
    }
    void print_wall(const string& msg, size_t size=40) {
        print_wall(cout, msg, size);
    }
    void print_wall(size_t size=40) {
        print_wall(cout, size);
    }
    void eprint_wall(const string& msg, size_t size=40) {
        print_wall(cerr, msg, size);
    }
    void eprint_wall(size_t size=40) {
        print_wall(cerr, size);
    }

    template<class T> void print_statistical_information(const vector<T> &v) {
        const T minimum = min(v);
        const T maximum = max(v);
        const T median = (v.size()%2 == 1 ? v[v.size()/2] : (v[v.size()/2-1] + v[v.size()/2]) / 2.);
        const T v_sum = sum(v);
        const double ave = static_cast<double>(v_sum) / v.size();
        const double var = accumulate(v.begin(), v.end(), 0.,  [](double acc, T i) { return acc + (double)i*(double)i; }) / v.size() - ave*ave;
        const double std = sqrt(var);

        print_wall("print statistical information", 39);
        cout << "minimum    : " << minimum << endl;
        cout << "maximum    : " << maximum << endl;
        cout << "median     : " << median << endl;
        cout << "sum        : " << v_sum << endl;
        cout << "ave        : " << ave << endl;
        cout << "var        : " << var << endl;
        cout << "std        : " << std << endl;
        cout << "ave ±  std : [" << ave - std << ", " << ave + std << "], 68.3%" << endl;
        cout << "ave ± 2std : [" << ave - 2*std << ", " << ave + 2*std << "], 95.4%" << endl;
        cout << "ave ± 3std : [" << ave - 3*std << ", " << ave + 3*std << "], 99.7%" << endl;
        print_wall(39);
    }

    template<class T> string show(const list<T>& l);
    template<class T> string show(const deque<T>& d);
    template<class T> string show(const vector<T>& v);
    template<class T, size_t N> string show(const array<T, N>& a);
    template<class T> string show(const set<T>& s);
    template<class T, class S> string show(const map<T, S>& m);
    template<class T, class S> string show(const pair<T, S>& p);

    string show(const char *c) { return string(c); }
    string show(char c) { return {c}; }
    string show(bool f) { return (f ? "true" : "false"); }
    string show(const string& s) { return s; }
    string show(const Path& p) { stringstream ss; ss << p; return ss.str(); }
    template<class T> string show(T a) { stringstream ss; ss << a; return ss.str(); }
    string show(unsigned char a) { return show((unsigned)a); }

    template<class T> string show(Vec2<T> v) { return "(" + show(v.x) + ", " + show(v.y) + ")"; }
    template<class T> string show(Grid<T> g) {
        string s = "{";
        for(int i=0, h=g.height(); i<h; ++i) {
            if (i) s += ", ";
            s.push_back('{');
            for(int j=0, w=g.width(); j<w; ++j) {
                if (j) s += ", ";
                s += show(g[i][j]);
            }
            s.push_back('}');
        }
        s.push_back('}');
        return s;
    }

    template<class T> string show(const list<T>& l) {
        if (l.empty()) return "{}";
        string s = "{";
        for(const auto &i : l) s += show(i) + ", ";
        s.pop_back();
        s.back() = '}';
        return s;
    }
    template<class T> string show(const deque<T>& d) {
        if (d.empty()) return "{}";
        string s = "{";
        for(const auto &i : d) s += show(i) + ", ";
        s.pop_back();
        s.back() = '}';
        return s;
    }
    template<class T> string show(const vector<T>& v) {
        if (v.empty()) return "{}";
        string s = "{";
        for(const auto &i : v) s += show(i) + ", ";
        s.pop_back();
        s.back() = '}';
        return s;
    }
    template<class T, size_t N> string show(const array<T, N>& a) {
        if (a.empty()) return "{}";
        string s = "{";
        for(const auto &i : a) s += show(i) + ", ";
        s.pop_back();
        s.back() = '}';
        return s;
    }
    template<class T> string show(const set<T>& s) {
        if (s.empty()) return "{}";
        string str = "{";
        for(const auto &i : s) str += show(i) + ", ";
        str.pop_back();
        str.back() = '}';
        return s;
    }
    template<class T, class S> string show(const map<T, S>& m) {
        if (m.empty()) return "{}";
        string s = "{";
        for(const auto &i : m) s += show(i.first) + ": " + show(i.second) + ", ";
        s.pop_back();
        s.back() = '}';
        return s;
    }
    template<class T, class S> string show(const pair<T, S>& p) { return "<" + show(p.first) + ", " + show(p.second) + ">"; }

    string concat_as_str(const string& delimiter) { return ""; }
    template<class T> string concat_as_str(const string& delimiter, T&& a) { return show(a); }
    template<class Head, class... Tail> string concat_as_str(const string& delimiter, Head&& head, Tail&&... tail) {
        return show(head) + delimiter + concat_as_str(delimiter, tail...);
    }

    template<class... Args> string concat(Args&&... args) { return concat_as_str("", args...); }
    template<class... Args> string concat_with_space(Args&&... args) { return concat_as_str(" ", args...); }
    template<class... Args> string concat_with_comma(Args&&... args) { return concat_as_str(", ", args...); }

    template<class... Args> void print_to(ostream& os, Args&&... args) {
        os << concat_with_comma(args...) << endl;
    }
    template<class... Args> void print(Args&&... args) {
        print_to(cout, args...);
    }
    template<class... Args> void eprint(Args&&... args) {
        print_to(cerr, args...);
    }

    void dump_to(ostream& os) { os << endl; }
    template<class Head, class... Args> void dump_to(ostream& os, Head&& head, Args&&... args) {
        os << head;
        if (sizeof...(Args) == 0) os << endl;
        else { os << " "; dump_to(os, args...); }
    }
    template<class... Args> void dump(Args&&... args) {
        dump_to(cout, args...);
    }
    template<class... Args> void edump(Args&&... args) {
        dump_to(cerr, args...);
    }

    template<class T> bool is_same(const T& a, const T& b) { return a == b; }
    template<class T> bool is_same(const vector<T>& v1, const vector<T>& v2) {
        if (v1.size() != v2.size()) return false;
        rep(i,v1.size()) if (!is_same(v1[i], v2[i])) return false;
        return true;
    }

    template<class T> void print_diff(const vector<T>& v1, const vector<T>& v2) {
        if (v1.size() != v2.size()) { PRINT(v1.size(), v2.size()); return; }
        rep(i,v1.size()) if (!is_same(v1[i], v2[i])) PRINT(i, v1[i], v2[i]);
    }

    template<class T> vector<T> get_subvec(const vector<T>& v, size_t pos, size_t n) {
        if (v.empty()) return {};
        chmin(pos, v.size());
        chmin(n, v.size()-pos);
        return {v.begin()+pos, v.begin()+pos+n};
    }

    template<class T> size_t hash_vec(const vector<T>& v) {
        size_t seed = v.size();
        for(const auto& i : v) seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
    template<class T> void shuffle(vector<T>& v) {
        for(size_t i = v.size(); --i>0;) swap(v[i], v[Random::rand_range(i)]);
    }

    template<class... Args> void exit_with_message(Args&&... args) {
        string message = concat_with_comma(args...);
        eprint_wall("EXIT WITH MESSAGE");
        cerr << message << endl;
        eprint_wall(39);
        exit(-1);
    }

    template<class T, class S, class Functor> vector<T> get_mapped(const vector<S>& v, const Functor& func) {
        vector<T> result; result.reserve(v.size());
        for(const auto& x : v) result.push_back(func(x));
        return result;
    }
    template<class T, class S, class Functor> vector<vector<T>> get_mapped(const vector<vector<S>>& v, const Functor& func) {
        vector<vector<T>> result; result.reserve(v.size());
        for(const auto& x : v) result.push_back(get_mapped<T>(x, func));
        return result;
    }
    template<class T, class S, size_t N, class Functor> constexpr array<T,N> get_mapped(const array<S,N>& a, const Functor& func) {
        array<T,N> result;
        rep(i,a.size()) result[i] = func(a[i]);
        return result;
    }
    template<class T, class S, size_t N, size_t M, class Functor> constexpr array<array<T,N>,M> get_mapped(const array<array<S, N>, M>& a, const Functor& func) {
        array<array<T,N>,M> result;
        rep(i,a.size()) result[i] = get_mapped<T>(a[i], func);
        return result;
    }

    template<class T, class Functor> void map(vector<T>& v, const Functor& func) {
        for(auto& x : v) func(x);
    }
    template<class T, class Functor> void map(vector<vector<T>>& v, const Functor& func) {
        for(auto& x : v) map(x,func);
    }
    template<class T, size_t N, class Functor> void map(array<T,N>& a, const Functor& func) {
        for(auto& x : a) func(x);
    }
    template<class T, size_t N, size_t M, class Functor> void map(array<array<T, N>, M>& a, const Functor& func) {
        for(auto& x : a) map(x,func);
    }

    static constexpr array<Pos, 4> dpos4() {
        return { Pos(Direction::U), Pos(Direction::R), Pos(Direction::D), Pos(Direction::L) };
    }
    static constexpr array<Pos, 8> dpos8() {
        constexpr Direction U = Direction::U, R = Direction::R, D = Direction::D, L = Direction::L;
        return { Pos(U), Pos(U)+Pos(R), Pos(R), Pos(R)+Pos(D), Pos(D), Pos(D)+Pos(L), Pos(L), Pos(L)+Pos(U) };
    }
}

class TimingDevice {
    chrono::system_clock::time_point m_start;
public:

    TimingDevice() : m_start(chrono::system_clock::now()) {}
    TimingDevice(bool start) : m_start(start ? chrono::system_clock::now() : chrono::system_clock::time_point::min()) {}

    void clear() { m_start = chrono::system_clock::time_point::min(); }
    void restart() { m_start = chrono::system_clock::now(); }
    bool is_started() const { return (m_start.time_since_epoch() != chrono::system_clock::time_point::min().time_since_epoch()); }
    unsigned long long get_sec() const {
        if (is_started()) return static_cast<unsigned long long>(chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - m_start).count());
        else return 0;
    }
    unsigned long long get_ms() const {
        if (is_started()) return static_cast<unsigned long long>(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - m_start).count());
        else return 0;
    }
    unsigned long long get_us() const {
        if (is_started()) return static_cast<unsigned long long>(chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - m_start).count());
        else return 0;
    }

    void print_sec(ostream& os) const { os << get_sec() << " [sec]" << endl; }
    void print_ms(ostream& os) const { os << get_ms() << " [ms]" << endl; }
    void print_us(ostream& os) const { os << get_us() << " [us]" << endl; }
    void print_elapsed(ostream& os) const {
        auto elapsed_us = static_cast<double>(get_us());
        os << fixed << setprecision(3);
        if (elapsed_us > 1e6) os << elapsed_us / 1e6 << " [sec]";
        else if (elapsed_us > 1e3) os << elapsed_us / 1e3 << " [ms]";
        else os << elapsed_us << " [us]";
        os << endl;
    }

    void print_sec() const { print_sec(cout); }
    void print_ms() const { print_ms(cout); }
    void print_us() const { print_us(cout); }
    void print_elapsed() const { print_elapsed(cout); }

    void eprint_sec() const { print_sec(cerr); }
    void eprint_ms() const { print_ms(cerr); }
    void eprint_us() const { print_us(cerr); }
    void eprint_elapsed() const { print_elapsed(cerr); }
};

class TimeManager : public TimingDevice {
    size_t ms_time_limit;
public:
    explicit TimeManager(size_t ms_time_limit) : TimingDevice(), ms_time_limit(ms_time_limit) {}
    [[nodiscard]] bool in_time_limit() const {
        return get_ms() < ms_time_limit;
    }
    [[nodiscard]] double progress() const {
        return static_cast<double>(get_ms()) / static_cast<double>(ms_time_limit);
    }
};

class SimulatedAnnealingManager : public TimeManager {
    double start_temp, end_temp;

    double temp() { return start_temp + (end_temp - start_temp) * progress(); }
    double prob(double score_diff) { return (score_diff >= 0 ? 1 : exp(score_diff / temp())); }

public:
    SimulatedAnnealingManager(size_t ms_time_limit, double start_temp, double end_temp) : TimeManager(ms_time_limit), start_temp(start_temp), end_temp(end_temp) {}
    bool permit_transition(double score_diff) { return prob(score_diff) >= Random::rand_prob(); }
};

template<class T, class S>
struct ComparableData {
    T rating;
    S data;
    ComparableData() : rating(), data() {}
    ComparableData(T rating, S data) : rating(rating), data(data) {}
    constexpr bool operator< (const ComparableData<T, S>& another) const { return rating < another.rating; }
    constexpr bool operator> (const ComparableData<T, S>& another) const { return another < *this; }
    constexpr bool operator<=(const ComparableData<T, S>& another) const { return !(*this > another); }
    constexpr bool operator>=(const ComparableData<T, S>& another) const { return !(*this < another); }
    constexpr bool operator==(const ComparableData<T, S>& another) const { return rating == another.rating; }
    constexpr bool operator!=(const ComparableData<T, S>& another) const { return !(*this == another); }
};
template<class T, class S> ostream& operator << (ostream& os, const ComparableData<T, S> cd) {
    os << Utility::concat("ComparableData {rating: ", cd.eval_val, ", data: ", cd.data, "}");
    return os;
}

template<class T>
class LimitedStorage {
    using Index = size_t;
public:
    struct Ticket {
        size_t key; Index storage_idx;
        Ticket() : key(), storage_idx() {}
        Ticket(size_t key, Index storage_idx) : key(key), storage_idx(storage_idx) {}
        friend ostream& operator << (ostream& os, const Ticket& tkt) {
            os << Utility::concat("Ticket {key: ", tkt.key, ", idx: ", tkt.storage_idx, "}");
            return os;
        }
    };

private:
    struct Unit {
        size_t key; T data;
        Unit() : key(), data() {}
        Unit(size_t key, T data) : key(key), data(data) {}
        friend ostream& operator << (ostream& os, const Unit& unit) {
            os << Utility::concat("Unit {key: ", unit.key, ", data: ", unit.data, "}");
            return os;
        }
    };

    const size_t max_storage_size;
    size_t pushed_count;
    queue<Index> popped;
    queue<Ticket> pushed;
    vector<Unit> storage;

    size_t get_insertable_storage_idx() {
        if (!popped.empty()) {
            Index idx = popped.front();
            popped.pop();
            return idx;
        }
        else if (storage.size() < max_storage_size) return storage.size();
        else {
            while(storage[pushed.front().storage_idx].key != pushed.front().key) { pushed.pop(); }
            Index idx = pushed.front().storage_idx;
            pushed.pop();
            return idx;
        }
    }

public:
    explicit LimitedStorage(size_t max_storage_size) : max_storage_size(max_storage_size), pushed_count(0), storage() {}
    void reserve(size_t n) { storage.reserve(min(n, max_storage_size)); }
    Ticket push(const T& data) {
        size_t key = pushed_count++;
        Index idx = get_insertable_storage_idx();
        if (idx == storage.size()) storage.emplace_back(key, data);
        else storage[idx] = Unit(key, data);
        pushed.emplace(key, idx);
        return Ticket(key, idx);
    }
    optional<T> pop(const Ticket& tkt) {
        if (storage[tkt.storage_idx].key != tkt.key) return nullopt;
        popped.push(tkt.storage_idx);
        storage[tkt.storage_idx].key = -1;
        return storage[tkt.storage_idx].data;
    }
    [[nodiscard]] optional<T> get(const Ticket& tkt) const {
        if (storage[tkt.storage_idx].key != tkt.key) return nullopt;
        return storage[tkt.storage_idx].data;
    }
    bool erase(const Ticket& tkt) {
        if (storage[tkt.storage_idx].key != tkt.key) return false;
        popped.push(tkt.storage_idx);
        storage[tkt.storage_idx].key = -1;
        return true;
    }
    void dump(ostream& os) {
        Utility::print_wall(os, "storage info", 40);
        PRINT_TO(os, max_storage_size);
        PRINT_TO(os, pushed_count);
        rep(i, storage.size()) os << Utility::concat("storage[",i,"]: ",storage[i]) << endl;
        Utility::print_wall(os, 40);
    }
    void dump() { dump(cout); }
    void edump() { dump(cerr); }
};

template<class T>
class depque {
    size_t $size;
    priority_queue<pair<T, char>> pq;
    reverse_priority_queue<pair<T, char>> rpq;
    void clean_pq() {
        while(!pq.empty() && pq.top().second == 1) {
            int count = 0;
            while(pq.top().second == 1) {
                count++;
                pq.pop();
            }
            while(count--) { pq.pop(); }
        }
    }
    void clean_rpq() {
        while(!rpq.empty() && rpq.top().second == -1) {
            int count = 0;
            while(rpq.top().second == -1) {
                rpq.pop();
            }
            while(count--) { rpq.pop(); }
        }
    }

public:
    depque() : $size(), pq(), rpq() {}
    void push(const T& val) { pq.emplace(val, -1); rpq.emplace(val, 1); ++$size; }
    void clear() {$size = 0; pq = decltype(pq)(); rpq = decltype(rpq)();}
    size_t size() const { return $size; }
    bool empty() const { return $size == 0; }

    T max() { clean_pq(); return pq.top().first; }
    T min() { clean_rpq(); return rpq.top().first; }

    void pop_max() { clean_pq(); rpq.emplace(pq.top().first, -1); pq.pop(); --$size; }
    void pop_min() { clean_rpq(); pq.emplace(rpq.top().first, -1); rpq.pop(); --$size; }
};

void Main(int argc, char*argv[]);
int main(int argc, char *argv[]) {
    Random::set_random_state(42);

    Main(argc, argv);

    return 0;
}


template<class T, size_t N> class weak_vector {
    size_t head;
    array<T, N> data;

public:
    weak_vector() : head(), data() {}
    size_t size() const { return head; }
    bool empty() const { return head == 0; }
    void clear() { head = 0; }
    void resize(size_t new_head) { head = new_head; }
    void push_back(const T& val) { data[head] = val; head++; }
    template<class... Args> void emplace_back(const Args&... args) { data[head] = {args...}; head++; }
    T get_and_pop_back() { assert(head > 0); return data[--head]; }
    void pop_back() { assert(head > 0); head--; }
    T back() const { return data[head-1]; }
    T front() const { return data[0]; }
    T* begin() { return data.begin(); }
    T* end() { return data.begin()+head; }
    T& operator[](size_t idx) { return data[idx]; }
    const T* begin() const { return data.begin(); }
    const T* end() const { return data.begin()+head; }
    const T& operator[](size_t idx) const { return data[idx]; }
};


// ここまでテンプレ

enum PosType {rest, dest};

struct ExPos {
    int order_id, visit_turn;
    Pos p;
    PosType type;
    ExPos *pair_pos;
};

struct Delivery {
    ExPos s, t;

};

constexpr int n = 1000;
constexpr int m = 50;
constexpr Pos center_pos(400,400);
array<Delivery, n> del;
Delivery st;

int calc_around_mh_dist(const vector<ExPos*> &visit_order, int idx) {
    return visit_order[idx-1]->p.calc_mh_dist(visit_order[idx]->p) + visit_order[idx]->p.calc_mh_dist(visit_order[idx+1]->p);
}
int calc_around_mh_dist_when_insert(const vector<ExPos*> &visit_order, int idx, Pos p) {
    return visit_order[idx]->p.calc_mh_dist(p) + p.calc_mh_dist(visit_order[idx+1]->p);
}
int calc_diff_dist_when_delete(const vector<ExPos*> &visit_order, int idx) {
    Pos &p = visit_order[idx-1]->p, &q = visit_order[idx]->p, &r = visit_order[idx+1]->p;
    return p.calc_mh_dist(r) - (p.calc_mh_dist(q) + q.calc_mh_dist(r));
}
int calc_diff_dist_when_delete_2pos(const vector<ExPos*> &visit_order, int idx) {
    Pos &p = visit_order[idx-1]->p, &q = visit_order[idx]->p, &r = visit_order[idx+1]->p, &s = visit_order[idx+2]->p;
    return p.calc_mh_dist(s) - (p.calc_mh_dist(q) + q.calc_mh_dist(r) + r.calc_mh_dist(s));
}

void Main(int argc, char *argv[]) {
    SimulatedAnnealingManager sa_mgr(1950, 100, 1e-3);
    st.s.p = st.t.p = center_pos;
    st.s.order_id = st.t.order_id = st.s.visit_turn = st.t.visit_turn = -1;
    st.s.type = rest;
    st.t.type = dest;
    st.s.pair_pos = &st.t;
    st.t.pair_pos = &st.s;

    rep(i,n) {
        cin >> del[i].s.p >> del[i].t.p;
        del[i].s.order_id = del[i].t.order_id = i+1;
        del[i].s.visit_turn = del[i].t.visit_turn = -1;
        del[i].s.pair_pos = &del[i].t;
        del[i].t.pair_pos = &del[i].s;
        del[i].s.type = rest;
        del[i].t.type = dest;
    }
    /*
     * 回収する料理の集合を決める
     * 訪れる順番を決める
     *
     *
     * 近傍
     * ある料理を消して、別の料理を全探索して決める(計算量重め
     * ある料理をinsertして、別の料理を全探索して消す(計算量普通
     * ある料理を運ぶのをやめて別の料理をinsertする(計算量軽め
     *
     * レストランは前から、目的地は後ろから累積minを取ることで最適な場所がわかる
     * 訪れる順番のswap
     *
     *
     * */

    array<bool, n> taken_order{};
    vector<ExPos*> visit_order;
    visit_order.push_back(&st.s);
    visit_order.back()->visit_turn = visit_order.size()-1;
    rep(i,m) {
        taken_order[i] = true;
        visit_order.push_back(&del[i].s);
        visit_order.back()->visit_turn = visit_order.size()-1;
    }
    rep(i,m) {
        visit_order.push_back(&del[i].t);
        visit_order.back()->visit_turn = visit_order.size()-1;
    }
    visit_order.push_back(&st.t);
    visit_order.back()->visit_turn = visit_order.size()-1;

    int now_mh_dist_sum = 0;
    rep(i,visit_order.size()-1) now_mh_dist_sum += visit_order[i]->p.calc_mh_dist(visit_order[i+1]->p);
    int min_mh_dist_sum = now_mh_dist_sum;

    array<pair<int,int>, (m+1)*2> dd_when_insert_R{};
    array<pair<int,int>, (m+1)*2> dd_when_insert_D{};
    vector<ExPos*> new_visit_order{};
    int loop_count = 0;
    while (sa_mgr.in_time_limit()) {
        loop_count++;
        int mode = Random::rand_range(2);

        int diff_mh_dist = 0;

        int idx0, idx1;
        pair<int,int> best_insert_pos;
        if (mode == 0) { // swap
            int loop_count_ = 0;
            idx0 = Random::rand_range(2*m)+1;
            idx1 = Random::rand_range(2*m)+1;
            while(loop_count_ < 1000) {
                loop_count_++;
                if (idx0 != idx1) {
                    if (visit_order[idx0]->type == rest) {
                        if (visit_order[idx1]->type == rest) {
                            if (visit_order[idx0]->visit_turn < visit_order[idx1]->pair_pos->visit_turn &&
                                visit_order[idx1]->visit_turn < visit_order[idx0]->pair_pos->visit_turn) break;
                        } else {
                            if (visit_order[idx0]->visit_turn > visit_order[idx1]->pair_pos->visit_turn &&
                                visit_order[idx1]->visit_turn < visit_order[idx0]->pair_pos->visit_turn) break;
                        }
                    } else {
                        if (visit_order[idx1]->type == rest) {
                            if (visit_order[idx0]->visit_turn < visit_order[idx1]->pair_pos->visit_turn &&
                                visit_order[idx1]->visit_turn > visit_order[idx0]->pair_pos->visit_turn) break;
                        } else {
                            if (visit_order[idx0]->visit_turn > visit_order[idx1]->pair_pos->visit_turn &&
                                visit_order[idx1]->visit_turn > visit_order[idx0]->pair_pos->visit_turn) break;
                        }
                    }
                }
                idx0 = Random::rand_range(2*m)+1;
                idx1 = Random::rand_range(2*m)+1;
            }
            int prev_dist = calc_around_mh_dist(visit_order, idx0) + calc_around_mh_dist(visit_order, idx1);
            swap(visit_order[idx0], visit_order[idx1]);
            int next_dist = calc_around_mh_dist(visit_order, idx0) + calc_around_mh_dist(visit_order, idx1);
            swap(visit_order[idx0], visit_order[idx1]);
            diff_mh_dist = next_dist - prev_dist;
        }
        else if (mode == 1) {
            idx0 = Random::rand_range(n);
            idx1 = Random::rand_range(n);
            while(taken_order[idx0] || !taken_order[idx1]) {
                idx0 = Random::rand_range(n);
                idx1 = Random::rand_range(n);
            }

            if (del[idx1].s.visit_turn+1 == del[idx1].t.visit_turn) {
                diff_mh_dist = calc_diff_dist_when_delete_2pos(visit_order, del[idx1].s.visit_turn);
            } else {
                diff_mh_dist = calc_diff_dist_when_delete(visit_order, del[idx1].s.visit_turn) + calc_diff_dist_when_delete(visit_order, del[idx1].t.visit_turn);
            }

//            PRINT(diff_mh_dist);

            const int target_to_delete_order_id = del[idx1].s.order_id;
            rep(i, visit_order.size()-1) {
                int prev = i, next = i+1;
                if (visit_order[i]->order_id == target_to_delete_order_id) {
                    if (visit_order[i-1]->order_id == target_to_delete_order_id) {
                        prev = i-2;
                    } else {
                        prev = i-1;
                    }
                }
                if (visit_order[i+1]->order_id == target_to_delete_order_id) {
                    if (visit_order[i+2]->order_id == target_to_delete_order_id) {
                        next = i+3;
                    } else next = i+2;
                }
                Pos &p = visit_order[prev]->p, q = del[idx0].s.p, &r = visit_order[next]->p;
                dd_when_insert_R[i].first = p.calc_mh_dist(q) + q.calc_mh_dist(r) - p.calc_mh_dist(r);
                q = del[idx0].t.p;
                dd_when_insert_D[i].first = p.calc_mh_dist(q) + q.calc_mh_dist(r) - p.calc_mh_dist(r);
                dd_when_insert_R[i].second = dd_when_insert_D[i].second = i;
            }
/*

            rep(i, visit_order.size()-1) {
                dd_when_insert_R[i].first = calc_around_mh_dist_when_insert(visit_order, i, del[idx0].s.p);
                dd_when_insert_D[i].first = calc_around_mh_dist_when_insert(visit_order, i, del[idx0].t.p);
                dd_when_insert_R[i].second = dd_when_insert_D[i].second = i;
            }

*/
            rep(i,visit_order.size()-2) {
                if (dd_when_insert_R[i+1].first > dd_when_insert_R[i].first) {
                    dd_when_insert_R[i+1] = dd_when_insert_R[i];
                }
            }
            revrep(i, visit_order.size()-2) {
                if (dd_when_insert_D[i].first > dd_when_insert_D[i+1].first) {
                    dd_when_insert_D[i] = dd_when_insert_D[i+1];
                }
            }

            int min_diff_dist = INT_MAX;
            rep(i,visit_order.size()-4) {
                int dd = dd_when_insert_R[i].first + dd_when_insert_D[i+3].first;
                if (min_diff_dist > dd) {
                    min_diff_dist = dd;
                    best_insert_pos = pair(dd_when_insert_R[i].second, dd_when_insert_D[i+3].second);
                }
            }
/*
            PRINT(best_insert_pos);
            PRINT(del[idx1].s.visit_turn, del[idx1].t.visit_turn);
            int i = best_insert_pos.first;
            PRINT(visit_order[i]->p, del[idx0].s.p, visit_order[i+1]->p);
            Pos p = visit_order[i]->p, q = del[idx0].s.p, r = visit_order[i+1]->p;
            PRINT(p.calc_mh_dist(q), q.calc_mh_dist(r), p.calc_mh_dist(r));
            PRINT(p.calc_mh_dist(q) + q.calc_mh_dist(r) - p.calc_mh_dist(r));
            i = best_insert_pos.second;

            p = visit_order[i]->p, q = del[idx0].t.p, r = visit_order[i+1]->p;
            PRINT(visit_order[i]->p, del[idx0].t.p, visit_order[i+1]->p);
            PRINT(p.calc_mh_dist(q), q.calc_mh_dist(r), p.calc_mh_dist(r));
            PRINT(p.calc_mh_dist(q) + q.calc_mh_dist(r) - p.calc_mh_dist(r));
            PRINT(min_diff_dist);

*/
            diff_mh_dist += min_diff_dist;
        } else if (mode == 3) {}

/*
        if (mode == 1) {
            PRINT(diff_mh_dist, idx0, idx1, best_insert_pos);
            continue;
        }
*/
        if (sa_mgr.permit_transition(-diff_mh_dist)) {
//            PRINT(diff_mh_dist, now_mh_dist_sum);
            now_mh_dist_sum += diff_mh_dist;
            if (min_mh_dist_sum > now_mh_dist_sum) {
                min_mh_dist_sum = now_mh_dist_sum;
            }



            if (mode == 0) {
                swap(visit_order[idx0], visit_order[idx1]);
                swap(visit_order[idx0]->visit_turn, visit_order[idx1]->visit_turn);
            }
            else if (mode == 1) {
                taken_order[idx0] = true;
                taken_order[idx1] = false;

                new_visit_order.clear();
                const int target_to_delete_order_id = del[idx1].s.order_id;
                rep(i,visit_order.size()) {
                    if (visit_order[i]->order_id != target_to_delete_order_id) {
                        new_visit_order.push_back(visit_order[i]);
                        new_visit_order.back()->visit_turn = new_visit_order.size()-1;
                    }
                    if (best_insert_pos.first == i) {
                        new_visit_order.push_back(&del[idx0].s);
                        new_visit_order.back()->visit_turn = new_visit_order.size()-1;
//                        PRINT("HI", i, new_visit_order[new_visit_order.size()-2]->p);
                    }
                    if (best_insert_pos.second == i) {
                        new_visit_order.push_back(&del[idx0].t);
                        new_visit_order.back()->visit_turn = new_visit_order.size()-1;
//                        PRINT("HO", i, new_visit_order[new_visit_order.size()-2]->p);
                    }
                }

                swap(visit_order, new_visit_order);
            }
        }
/*

        for(const auto& p : visit_order) cout << p->p << " "; cout << endl;
        int tmp_mh_dist_sum = 0;
        rep(i,visit_order.size()-1) tmp_mh_dist_sum += visit_order[i]->p.calc_mh_dist(visit_order[i+1]->p);
        if (now_mh_dist_sum != tmp_mh_dist_sum) EXIT_DEBUG(now_mh_dist_sum, tmp_mh_dist_sum);
*/
    }
    EPRINT(loop_count);
    EPRINT(now_mh_dist_sum);
    int score = lround(1e8 / (1000+now_mh_dist_sum));
    EPRINT(score);

    cout << m << " ";
    rep(i,n) if (taken_order[i]) cout << i+1 << " ";
    cout << endl;
    vector<Pos> ans;
    for(const auto& p : visit_order) if (ans.empty() || ans.back() != p->p) ans.push_back(p->p);
    cout << ans.size() << " ";
    for(const auto& p : ans) cout << p.x << " " << p.y << " ";
    cout << endl;
}

