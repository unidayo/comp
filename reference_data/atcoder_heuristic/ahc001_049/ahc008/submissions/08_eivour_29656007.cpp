
#ifdef ONLINE_JUDGE
#define NDEBUG
#pragma GCC optimize ("O3,unroll-loops")
#endif
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <variant>
#include <tuple>
#include <random>
#include <array>
#include <queue>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <thread>
#include <variant>
#include <atcoder/maxflow>
#include <optional>
#include <atcoder/mincostflow>
#include <xmmintrin.h>


using namespace std;
using namespace std::literals::string_literals;
using namespace atcoder;

using i32 = std::int32_t;
constexpr i32 INF = 1e9;

mt19937_64 rng(12345);
constexpr array<i32, 4> dy = {-1, 1, 0, 0};
constexpr array<i32, 4> dx = {0, 0, -1, 1};
constexpr array<char, 4> dirs = {'U', 'D', 'L', 'R'};
constexpr array<i32, 128> dir_i = []
{
    array<i32, 128> ret{};
    for (i32 i = 0; i < 128; i++)
    {
        ret[i] = -1;
    }
    for (i32 i = 0; i < 4; i++)
    {
        ret[dirs[i]] = i;
    }
    return ret;
}();

constexpr int S = 30;
struct Map
{
    array<array<char, S>, S> map = {};
    constexpr Map()
    {
        for (i32 i = 0; i < S; i++)
        {
            for (i32 j = 0; j < S; j++)
            {
                map[i][j] = '.';
            }
        }
    }
    Map(initializer_list<const char *> map)
    {
        i32 i = 0;
        for (const char *s : map)
        {
            for (i32 j = 0; j < S; j++)
            {
                assert(s[j] != '\0');
                this->map[i][j] = s[j];
            }
            i++;
        }
        assert(i == S);
    }
    Map(i32 y0, i32 x0, initializer_list<const char *> map)
    {
        i32 N = map.size();
        i32 M = std::strlen(*map.begin());
        i32 yl = max(y0, 0);
        i32 yr = min(y0 + i32(N), S);
        i32 xl = max(x0, 0);
        i32 xr = min(x0 + i32(M), S);
        for (i32 i = 0; i < S; i++)
        {
            for (i32 j = 0; j < S; j++)
            {
                this->map[i][j] = '.';
            }
        }
        for (i32 i = yl; i < yr; i++)
        {
            for (i32 j = xl; j < xr; j++)
            {
                this->map[i][j] = (*(map.begin() + i - y0))[j - x0];
            }
        }
    }

    Map(const Map &) = default;
    Map &operator=(const Map &) = default;

    array<array<i32, S>, S> distmap(i32 y, i32 x) const
    {
        array<array<i32, S>, S> distmap;
        for (i32 i = 0; i < S; i++)
        {
            for (i32 j = 0; j < S; j++)
            {
                distmap[i][j] = INF;
            }
        }
        // queue<tuple<i32, i32>> q;
        static vector<tuple<i32, i32>> buf0, buf1;
        buf0.clear();
        buf1.clear();

        // q.push({y, x});
        buf0.push_back({y, x});
        distmap[y][x] = 0;
        // while (!q.empty())
        // {
        //     auto [y, x] = q.front();
        while(!buf0.empty()){
            for(auto[y, x] : buf0){
                // q.pop();
                for (i32 i = 0; i < 4; i++)
                {
                    i32 ny = y + dy[i];
                    i32 nx = x + dx[i];
                    if (ny < 0 || ny >= S || nx < 0 || nx >= S)
                    {
                        continue;
                    }
                    if (distmap[ny][nx] == INF && map[ny][nx] != '#')
                    {
                        distmap[ny][nx] = distmap[y][x] + 1;
                        // q.push({ny, nx});
                        buf1.push_back({ny, nx});
                    }
                }
            }
            buf0.clear();
            swap(buf0, buf1);
        }
        return distmap;
    }



    char &operator()(i32 y, i32 x)
    {
        return map[y][x];
    }
    char operator()(i32 y, i32 x) const
    {
        return map[y][x];
    }

    vector<pair<i32, i32>> get_all(char c) const
    {
        vector<pair<i32, i32>> ret;
        for (i32 i = 0; i < S; i++)
        {
            for (i32 j = 0; j < S; j++)
            {
                if (map[i][j] == c)
                {
                    ret.push_back({i, j});
                }
            }
        }
        return ret;
    }
};


array<array<i32, S>, S> exit_time_map(const Map& walls, const Map& map)
{
    static vector<tuple<i32, i32>> buf0, buf1;
    buf0.clear();
    buf1.clear();

    array<array<i32, S>, S> distmap;

    for (i32 y = 0; y < S; y++)
    {
        for (i32 x = 0; x < S; x++)
        {
            if(map(y, x) == '.'){
                distmap[y][x] = 0;
                buf0.push_back({y, x});
            }else{
                distmap[y][x] = INF;
            }
        }
    }
    // queue<tuple<i32, i32>> q;
    
    // q.push({y, x});
    // while (!q.empty())
    // {
    //     auto [y, x] = q.front();
    while(!buf0.empty()){
        for(auto[y, x] : buf0){
            // q.pop();
            for (i32 i = 0; i < 4; i++)
            {
                i32 ny = y + dy[i];
                i32 nx = x + dx[i];
                if (ny < 0 || ny >= S || nx < 0 || nx >= S)
                {
                    continue;
                }
                if (distmap[ny][nx] == INF && walls(ny, nx) != '#')
                {
                    distmap[ny][nx] = distmap[y][x] + 1;
                    // q.push({ny, nx});
                    buf1.push_back({ny, nx});
                }
            }
        }
        buf0.clear();
        swap(buf0, buf1);
    }
    return distmap;
}
using u64 = std::uint64_t;
inline u64 next_combination(u64 sub) {
    int x = sub & -sub, y = sub + x;
    return (((sub & ~y) / x) >> 1) | y;
}

struct Chamber{
    Map map;
    vector<pair<i32, i32>> gates;
    vector<pair<i32, i32>> dependency;
    bool locked;
};
vector<Chamber> decompose_chambers(const Map& map){
    array<array<bool, S>, S> visited = {};
    array<array<bool, S>, S> visited2 = {};
    vector<Chamber> ret;

    auto dfs = [&](auto&& dfs, i32 y, i32 x, Chamber& chamber){
        
        if(y < 0 || y >= S || x < 0 || x >= S){
            return;
        }

        if(visited2[y][x] || visited[y][x]){
            return;
        }

        visited2[y][x] = true;
        if(map(y, x) == '#'){
            chamber.map(y, x) = '#';
            chamber.dependency.push_back({y, x});
        }
        else if(map(y, x) == '.'){
            chamber.map(y, x) = 'x';
            chamber.gates.push_back({y, x});
        }
        else if(map(y, x) == 'o'){
            visited[y][x] = true;
            chamber.map(y, x) = 'o';
            for (i32 i = 0; i < 4; i++)
            {
                i32 ny = y + dy[i];
                i32 nx = x + dx[i];
                dfs(dfs, ny, nx, chamber);
            }
        }else{
            assert(false);
        }
    };
    for(i32 y = 0; y < S; y++){
        for(i32 x = 0; x < S; x++){
            if(map(y, x) == 'o' && !visited[y][x]){
                visited2 = {};
                Chamber chamber;
                dfs(dfs, y, x, chamber);
                ret.push_back(chamber);
            }
        }
    }
    for(auto& chamber: ret){
        for(auto [y, x]: chamber.gates){
            for(i32 dir = 0; dir < 4; dir++){
                if(y + dy[dir] < 0 || y + dy[dir] >= S || x + dx[dir] < 0 || x + dx[dir] >= S){
                    continue;
                }
                if(chamber.map(y + dy[dir], x + dx[dir]) == 'o'){
                    chamber.map(y + dy[dir], x + dx[dir]) = 'y';
                }
            }
        }
    }
    return ret;
}


template<class F>
tuple<vector<i32>, i32> minimax_match(i32 n, i32 m, F&& cost_func){

    assert(n <= m);
    mf_graph<i32> g(n + m + 2);
    for(i32 i = 0; i < n; i++){
        g.add_edge(n + m, i, 1);
    }
    for(i32 i=0; i<m; i++){
        g.add_edge(n + i, n + m + 1, 1);
    }
    vector<tuple<i32, i32, i32>> edges;
    for(i32 i=0; i<n; i++){
        for(i32 j=0; j<m;j++){
            edges.emplace_back(cost_func(i, j), i, j);
        }
    }
    sort(begin(edges), end(edges));
    i32 f = 0;
    for(auto [cost, i, j] : edges){
        g.add_edge(i, j + n, 1);
        f += g.flow(n + m, n + m + 1, 1);
        if(f == n){
            vector<i32> result(n);
            for(auto [from, to, cap, flow] : g.edges()){
                if(from == n + m || to == n + m + 1){
                    continue;
                }
                if(flow >= 1){
                    result[from] = to - n;
                }
            }
            return {result, cost};
        }
    }
    assert(false);
    return {};
}


// template<class F>
// tuple<vector<i32>, i32> mincost_match(i32 n, i32 m, F&& cost_func){

//     assert(n <= m);
//     mf_graph<i32> g(n + m + 2);
//     for(i32 i = 0; i < n; i++){
//         g.add_edge(n + m, i, 1);
//     }
//     for(i32 i=0; i<m; i++){
//         g.add_edge(n + i, n + m + 1, 1);
//     }
//     vector<tuple<i32, i32, i32>> edges;
//     for(i32 i=0; i<n; i++){
//         for(i32 j=0; j<m;j++){
//             edges.emplace_back(cost_func(i, j), i, j);
//         }
//     }
//     sort(begin(edges), end(edges));
//     i32 f = 0;
//     for(auto [cost, i, j] : edges){
//         g.add_edge(i, j + n, 1);
//         f += g.flow(n + m, n + m + 1, 1);
//         if(f == n){
//             vector<i32> result(n);
//             for(auto [from, to, cap, flow] : g.edges()){
//                 if(from == n + m || to == n + m + 1){
//                     continue;
//                 }
//                 if(flow >= 1){
//                     result[from] = to - n;
//                 }
//             }
//             return {result, cost};
//         }
//     }
//     assert(false);
//     return {};
// }




template <class... F>
struct Overload : F...
{
    using F::operator()...;
};
template <class... F>
Overload(F &&...f) -> Overload<decay_t<F>...>;

const Map map0{{
    "ooo#.#oooo#.#oooo#.#oooo#.#ooo",
    "ooo...oooo...oooo...oooo...ooo",
    "ooo#.#oooo#.#oooo#.#oooo#.#ooo",
    "###...####...####...####...###",
    "ooo...oooo...oooo...oooo...ooo",
    "ooo#.#oooo#.#oooo#.#oooo#.#ooo",
    "###...####...####...####...###",
    "ooo...oooo...oooo...oooo...ooo",
    "ooo#.#oooo#.#oooo#.#oooo#.#ooo",
    "###...####...####...####...###",
    "ooo...oooo...oooo...oooo...ooo",
    "ooo#.#oooo#.#oooo#.#oooo#.#ooo",
    "###...####...####...####...###",
    "ooo...oooo...oooo...oooo...ooo",
    "ooo#.#oooo#.#oooo#.#oooo#.#ooo",
    "###...####...####...####...###",
    "ooo...oooo...oooo...oooo...ooo",
    "ooo#.#oooo#.#oooo#.#oooo#.#ooo",
    "###...####...####...####...###",
    "ooo...oooo...oooo...oooo...ooo",
    "ooo#.#oooo#.#oooo#.#oooo#.#ooo",
    "###...####...####...####...###",
    "ooo...oooo...oooo...oooo...ooo",
    "ooo#.#oooo#.#oooo#.#oooo#.#ooo",
    "###...####...####...####...###",
    "ooo...oooo...oooo...oooo...ooo",
    "ooo#.#oooo#.#oooo#.#oooo#.#ooo",
    "###...####...####...####...###",
    "ooo...oooo...oooo...oooo...ooo",
    "ooo#.#oooo#.#oooo#.#oooo#.#ooo",
}};


const Map map1{{
    "ooo#.#ooo#..#ooo#..#ooo#..#ooo",
    "ooo...ooo#...ooo#...ooo#...ooo",
    "ooo#.#ooo...#ooo...#ooo...#ooo",
    "###...###....###....###....###",
    "ooo...ooo#...ooo#...ooo#...ooo",
    "ooo#.#ooo...#ooo...#ooo...#ooo",
    "###...###....###....###....###",
    "ooo...ooo#...ooo#...ooo#...ooo",
    "ooo#.#ooo...#ooo...#ooo...#ooo",
    "###...###....###....###....###",
    "ooo...ooo#...ooo#...ooo#...ooo",
    "ooo#.#ooo...#ooo...#ooo...#ooo",
    "###...###....###....###....###",
    "ooo...ooo#...ooo#...ooo#...ooo",
    "ooo#.#ooo...#ooo...#ooo...#ooo",
    "###...###....###....###....###",
    "ooo...ooo#...ooo#...ooo#...ooo",
    "ooo#.#ooo...#ooo...#ooo...#ooo",
    "###...###....###....###....###",
    "ooo...ooo#...ooo#...ooo#...ooo",
    "ooo#.#ooo...#ooo...#ooo...#ooo",
    "###...###....###....###....###",
    "ooo...ooo#...ooo#...ooo#...ooo",
    "ooo#.#ooo...#ooo...#ooo...#ooo",
    "###...###....###....###....###",
    "ooo...ooo#...ooo#...ooo#...ooo",
    "ooo#.#ooo...#ooo...#ooo...#ooo",
    "###...###....###....###....###",
    "ooo...ooo#...ooo#...ooo#...ooo",
    "ooo#.#ooo...#ooo...#ooo...#ooo",
}};

const Map map2{{
    "ooo#ooo#..#ooooo#oooo#ooo#oooo",
    "#ooo#o....o#ooo#..#ooo#o..oooo",
    "o#ooo#..#ooo#o....o#ooo#..#ooo",
    "oo#o....o#ooo#..#ooo#o....o#oo",
    "ooo#..#ooo#o....o#ooo#..#ooo#o",
    "oo....o#ooo#..#ooo#o....o#ooo#",
    "o#..#ooo#o....o#ooo#..#ooo#o..",
    "#...o#ooo#..#ooo#o....o#ooo#..",
    "..#ooo#o....o#ooo#..#ooo#o...#",
    "..o#ooo#..#ooo#o....o#ooo#..#o",
    "#ooo#o....o#ooo#..#ooo#o....oo",
    "o#ooo#..#ooo#o....o#ooo#..#ooo",
    "oo#o....o#ooo#..#ooo#o....o#oo",
    "ooo#..#ooo#o....o#ooo#..#ooo#o",
    "oo....o#ooo#..#ooo#o....o#ooo#",
    "o#..#ooo#o....o#ooo#..#ooo#o..",
    "#...o#ooo#..#ooo#o....o#ooo#..",
    "..#ooo#o....o#ooo#..#ooo#o...#",
    "..o#ooo#..#ooo#o....o#ooo#..#o",
    "#ooo#o....o#ooo#..#ooo#o....oo",
    "o#ooo#..#ooo#o....o#ooo#..#ooo",
    "oo#o....o#ooo#..#ooo#o....o#oo",
    "ooo#..#ooo#o....o#ooo#..#ooo#o",
    "#o....o#ooo#..#ooo#o....o#ooo#",
    "o#..#ooo#o....o#ooo#..#ooo#o..",
    "oo..o#ooo#..#ooo#o....o#ooo#..",
    "oo#ooo#o....o#ooo#..#ooo#o....",
    "ooo#ooo#..#ooo#o....o#ooo#..#.",
    "oooo#o....o#ooo#..#ooo#o....o#",
    "ooooo#..#ooo#ooo.#ooooo#..#ooo",
}};

const Map map3{{
"ooooo#oooo#ooooo#oooo#ooooo#oo",
"#ooo#..#ooo#ooo#..#ooo#ooo#ooo",
"o#o....o#ooo#o....o#ooo#o..ooo",
"oo#..#ooo#ooo#..#ooo#ooo#..#oo",
"o....o#ooo#o....o#ooo#o....o#o",
"#..#ooo#ooo#..#ooo#ooo#..#ooo#",
"...o#ooo#o....o#ooo#o....o#ooo",
".#ooo#ooo#..#ooo#ooo#..#ooo#oo",
".o#ooo#o....o#ooo#o....o#ooo#o",
"ooo#ooo#..#ooo#ooo#..#ooo#ooo#",
"#ooo#o....o#ooo#o....o#ooo#o..",
"o#ooo#..#ooo#ooo#..#ooo#ooo#..",
"oo#o....o#ooo#o....o#ooo#o...#",
"ooo#..#ooo#ooo#..#ooo#ooo#..#o",
"oo....o#ooo#o....o#ooo#o....oo",
"o#..#ooo#ooo#..#ooo#ooo#..#ooo",
"#...o#ooo#o....o#ooo#o....o#oo",
"..#ooo#ooo#..#ooo#ooo#..#ooo#o",
"..o#ooo#o....o#ooo#o....o#ooo#",
"#ooo#ooo#..#ooo#ooo#..#ooo#ooo",
"o#ooo#o....o#ooo#o....o#ooo#o.",
"oo#ooo#..#ooo#ooo#..#ooo#ooo#.",
"ooo#o....o#ooo#o....o#ooo#o...",
"#ooo#..#ooo#ooo#..#ooo#ooo#..#",
"o#o....o#ooo#o....o#ooo#o....o",
"oo#..#ooo#ooo#..#ooo#ooo#..#oo",
"ooo..o#ooo#o....o#ooo#o....o#o",
"ooo#ooo#ooo#..#ooo#ooo#..#ooo#",
"oooo#ooo#o....o#ooo#o....o#ooo",
"ooooo#ooo#..#ooo#ooo#..#ooo#oo"}};

const Map map4{{
"ooo#ooo#...#ooooo#....o#ooo#..",
"#ooo#o.....o#ooo#...#ooo#o....",
"o#ooo#...#ooo#o.....o#ooo#...#",
"oo#o.....o#ooo#...#ooo#o.....o",
"ooo#...#ooo#o.....o#ooo#...#oo",
"oo.....o#ooo#...#ooo#o.....o#o",
"o#...#ooo#o.....o#ooo#...#ooo#",
"#....o#ooo#...#ooo#o.....o#ooo",
"...#ooo#o.....o#ooo#...#ooo#o.",
"...o#ooo#...#ooo#o.....o#ooo#.",
"o#ooo#o.....o#ooo#...#ooo#o...",
"oo#ooo#...#ooo#o.....o#ooo#...",
"ooo#o.....o#ooo#...#ooo#o....#",
"#ooo#...#ooo#o.....o#ooo#...#o",
"o#o.....o#ooo#...#ooo#o.....oo",
"oo#...#ooo#o.....o#ooo#...#ooo",
"o.....o#ooo#...#ooo#o.....o#oo",
"#...#ooo#o.....o#ooo#...#ooo#o",
"....o#ooo#...#ooo#o.....o#ooo#",
"..#ooo#o.....o#ooo#...#ooo#o..",
"..o#ooo#...#ooo#o.....o#ooo#..",
"#ooo#o.....o#ooo#...#ooo#o....",
"o#ooo#...#ooo#o.....o#ooo#...#",
"oo#o.....o#ooo#...#ooo#o.....o",
"ooo#...#ooo#o.....o#ooo#...#oo",
"#o.....o#ooo#...#ooo#o.....o#o",
"o#...#ooo#o.....o#ooo#...#ooo#",
"oo...o#ooo#...#ooo#o.....o#ooo",
"ooo#ooo#o.....o#ooo#...#ooo#o.",
"oooo#ooo#...#ooo#o....#ooooo#."
}};

struct Constructing
{
    i32 lane; // 0 ... 4
    bool up;
};
struct Free
{
};
struct Human
{
    variant<Constructing, Free> state;
};
struct Column
{
    i32 x;
    bool up;
};
struct Lane
{
    bool complete = false;
    bool left;
    vector<Column> columns;
    i32 state0;
    i32 state1;
    i32 x_last;
    Lane(const Map &map, i32 lane, bool left)
    {
        this->left = left;
        i32 x0 = left ? 0 : S - 1;
        i32 x1 = left ? S : -1;
        i32 ddx = left ? 1 : -1;
        i32 x_last = [&]
        {
            for (i32 x = x1 - ddx; x != x0 - ddx; x -= ddx)
            {
                for (i32 yy = 0; yy < 6; yy++)
                {
                    if (map(lane * 6 + yy, x) == '#')
                    {
                        return x;
                    }
                }
            }
            return x0 - ddx;
        }();
        this->x_last = x_last;
        // cerr << "x_last = " << x_last << endl;

        for (i32 x = x0; x != x_last + ddx; x += ddx)
        {
            columns.push_back({x, true});
        }
        for (i32 x = x_last; x != x0 - ddx; x -= ddx)
        {
            columns.push_back({x, false});
        }
        if (columns.size() == 0)
        {
            complete = true;
            return;
        }
        for (i32 i = 0; i < i32(columns.size()); i++)
        {
            auto [x, col_up] = columns[i];
            i32 downy = col_up ? 0 : 3;

            auto remains = [&, x = x](i32 i){return map(6 * lane + downy + i, x) == '#';};

            if (remains(0) || remains(1) || remains(2))
            {
                state0 = i;
                break;
            }
        }
        for (i32 i = i32(columns.size()) - 1; i >= 0; i--)
        {           
            auto [x, col_up] = columns[i];
            i32 downy = col_up ? 0 : 3;

            auto remains = [&, x = x](i32 i){return map(6 * lane + downy + i, x) == '#';};

            if (remains(0) || remains(1) || remains(2))
            {
                state1 = i;
                break;
            }
        }
    }
};

char move_toward(i32 y0, i32 x0, const array<array<i32, S>, S> &distmap)
{

    i32 best_dist = distmap[y0][x0];
    i32 best_dir = -1;
    for (i32 dir = 0; dir < 4; dir++)
    {
        i32 y = y0 + dy[dir];
        i32 x = x0 + dx[dir];
        if (y < 0 || y >= S || x < 0 || x >= S)
        {
            continue;
        }
        i32 dist = distmap[y][x];
        if (dist < best_dist)
        {
            best_dist = dist;
            best_dir = dir;
        }
    }
    if (best_dir == -1)
    {
        return '.';
    }
    return dirs[best_dir];
}

struct CatTargetEstimation{
    array<array<double, S>, S> probs = {};
    void estimate(const Map& walls, i32 y, i32 x, i32 dir, i32 dir2/*const array<array<i32, S>, S>& distmap, const array<optional<array<array<i32, S>, S>>, 4>& distmap_dirs*/){
        array<array<i32, S>, S> distmap = walls.distmap(y, x);

        i32 reachable = 0;
        for (i32 yy = 0; yy < S; yy++)
        {
            for (i32 xx = 0; xx < S; xx++)
            {
                if (distmap[yy][xx] < INF)
                {
                    reachable++;
                }
            }
        }

        array<array<i32, S>, S> denom = {};
        array<array<i32, S>, S> numer = {};
        for(i32 i = 0; i < 4; i++){
            if(y + dy[i] < 0 || y + dy[i] >= S || x + dx[i] < 0 || x + dx[i] >= S || walls(y + dy[i], x + dx[i]) == '#')
                continue;
            auto new_distmap = walls.distmap(y + dy[i], x + dx[i]);
            for(i32 y = 0; y < S; y++){
                for(i32 x = 0; x < S; x++){
                    if(distmap[y][x] - 1 == new_distmap[y][x]){
                        denom[y][x]++;
                        numer[y][x] += (dir == i ? 1 : 0);
                    }
                }
            }
        }
        for(i32 y = 0; y < S; y++){
            for(i32 x = 0; x < S; x++){
                if(numer[y][x] == 0){
                    probs[y][x] = 0.0;
                }else{
                    probs[y][x] *= double(numer[y][x]) / denom[y][x];
                }
            }
        }
    
        auto reach = [&](i32 y, i32 x){
            double p = probs[y][x];
            probs[y][x] = 0.0;
                
            for (i32 yy = 0; yy < S; yy++)
            {
                for (i32 xx = 0; xx < S; xx++)
                {
                    if (distmap[yy][xx] < INF)
                    {
                        probs[yy][xx] += p / reachable;
                    }
                }
            }
        };
        i32 y1 = y + dy[dir], x1 = x + dx[dir];
        reach(y1, x1);
        i32 y2 = y1 + dy[dir2], x2 = x1 + dx[dir2];
        reach(y2, x2);

        auto normalize = [&]{
            double psum = 0.0;
            for (i32 y = 0; y < S; y++)
            {
                for (i32 x = 0; x < S; x++)
                {
                    psum += probs[y][x];
                }
            }
            for (i32 y = 0; y < S; y++)
            {
                for (i32 x = 0; x < S; x++)
                {
                    probs[y][x] /= psum;
                }
            }
        };
        normalize();
    }
    CatTargetEstimation(){
        for(i32 y = 0; y < S; y++){
            for(i32 x = 0; x < S; x++){
                probs[y][x] = 1.0 / (S * S);
            }
        }
    }
};

struct DogTargetEstimation{
    vector<double> probs;

    void estimate(const Map& walls, i32 y, i32 x, i32 dir, i32 dir2, const vector<i32>& hy, const vector<i32>& hx){
        i32 M = i32(probs.size());
        
        array<array<i32, S>, S> distmap = walls.distmap(y, x);

        i32 reachable = 0;
        for(i32 i = 0; i <M; i++){
            if (distmap[hy[i]][hx[i]] < INF)
            {
                reachable++;
            }
        }

        vector<i32> denom(M);
        vector<i32> numer(M);
        for(i32 d = 0; d < 4; d++){
            if(y + dy[d] < 0 || y + dy[d] >= S || x + dx[d] < 0 || x + dx[d] >= S || walls(y + dy[d], x + dx[d]) == '#')
                continue;
            auto new_distmap = walls.distmap(y + dy[d], x + dx[d]);
            for(i32 i = 0; i < M; i++){
                if(distmap[hy[i]][hx[i]] - 1 == new_distmap[hy[i]][hx[i]]){
                    denom[i]++;
                    numer[i] += (dir == d ? 1 : 0);
                }
            }
        }
        for(i32 i = 0; i < M; i++){
            if(numer[i] == 0){
                probs[i] = 0;
            }else{
                probs[i] *= double(numer[i]) / denom[i];
            }
        }

        auto reach = [&](i32 y, i32 x){

            double p = 0.0;
            for(i32 i = 0; i < M; i++){
                if(hy[i] == y && hx[i] == x){
                    p += probs[i];
                    probs[i] = 0.0;
                }
            }
            for (i32 j = 0; j < M; j++){
                if (distmap[hy[j]][hx[j]] < INF){
                    probs[j] += p / reachable;
                }
            }
        };
        i32 y1 = y + dy[dir], x1 = x + dx[dir];
        reach(y1, x1);
        i32 y2 = y1 + dy[dir2], x2 = x1 + dx[dir2];
        reach(y2, x2);

        auto normalize = [&]{
            double psum = 0.0;
            for (i32 i = 0; i < M; i++)
            {
                psum += probs[i];
            }
            if(psum >= 1e-10){
                for (i32 i = 0; i < M; i++)
                {
                    probs[i] /= psum;
                }
            }
        };
        normalize();
    }
    DogTargetEstimation(i32 M): probs(M){
        for(i32 i = 0; i < M; i++){
            probs[i] = 1.0 / M;
        }
    }
};



int main()
{
#ifndef ONLINE_JUDGE

// _MM_SET_EXCEPTION_MASK(_MM_GET_EXCEPTION_MASK() & ~_MM_MASK_INVALID & ~_MM_MASK_OVERFLOW);
#endif
#ifndef NDEBUG
    this_thread::sleep_for(chrono::milliseconds(1500));
#endif

    i32 N;
    cin >> N;
    vector<i32> px(N), py(N), pt(N);
    for (i32 i = 0; i < N; i++)
    {
        cin >> py[i] >> px[i] >> pt[i];
        px[i]--;
        py[i]--;
    }
    i32 M;
    cin >> M;
    vector<i32> hx(M), hy(M);
    for (i32 i = 0; i < M; i++)
    {
        cin >> hy[i] >> hx[i];
        hx[i]--;
        hy[i]--;
    }
    constexpr array<i32, 11> thresholds = {0, 0, 0, 0, 0, 0, 15, 17, 21, 21, 21};
    // const Map& map = N >= thresholds[M] ? map3: map2;
    constexpr array<i32, 11> thresholds2 = {0, 0, 0, 0, 0, 0, 0, 14, 21, 21, 21};
    // const Map &map = map3; //N >= thresholds[M] ? map0 : map2;
    const Map& map = N >= thresholds[M] ? map3: N >= thresholds2[M] ? map2 : map4;
    // const Map& map = map4;
    Map augmented_map = map;
    vector<Chamber> chambers = decompose_chambers(map);

    array<array<i32, S>, S> chamber_id;

    for(i32 y = 0; y < S; y++){
        for(i32 x = 0; x < S; x++){
            chamber_id[y][x] = -1;
            if(map(y, x) == 'o'){
                for(i32 ch = 0; ch < chambers.size(); ch++){
                    if(chambers[ch].map(y, x) == 'o'){
                        chamber_id[y][x] = ch;
                        // cerr << "chamber_id[" << y << "][" << x << "] = " << ch << endl;
                        break;
                    }
                }
            }
            if(map(y, x) == '.'){
                for(i32 ch = 0; ch < chambers.size(); ch++){
                    if(chambers[ch].map(y, x) != '.'){
                        augmented_map(y, x) = chambers[ch].map(y, x);
                        break;
                    }
                }
            }
        }
    }
    

    Map walls = {};

    vector<CatTargetEstimation> cat_estimations(N);
    vector<DogTargetEstimation> dog_estimations(N, DogTargetEstimation{M});
    vector<char> alive(N, true);

    vector<Human> humans(M);
    vector<Lane> lanes = {Lane(map, 0, true), Lane(map, 1, false), Lane(map, 2, true), Lane(map, 3, false), Lane(map, 4, true)};

    array<Constructing, 10> start_points{{{0, true}, {1, true}, {2, true}, {3, true}, {4, true}, {2, false}, {3, false}, {1, false}, {4, false}, {0, false}}}; 
    auto [match, cost] = minimax_match(M, M, [&](i32 i, i32 j){
        auto [lane, up] = start_points[j];
        i32 y = 6 * lane + (up ? 1 : 4);
        i32 x = lanes[lane].columns[up ? lanes[lane].state0 : lanes[lane].state1].x;
        return abs(hy[i] - y) + abs(hx[i] - x);
    });


    for(i32 i = 0; i < M; i++){
        humans[i].state = start_points[match[i]];
    }
    for(i32 i = M; i < 10; i++){
        auto [lane, up] = start_points[i];
        if(up){
            lanes[lane].state0--;
        }else{
            lanes[lane].state1++;
        }
    }



    for (i32 a = 0; a < 300; a++)
    {

        // Set cannot_put
        array<array<bool, 30>, 30> cannot_put_ = {};
        for (i32 i = 0; i < M; i++)
            cannot_put_[hy[i]][hx[i]] = true;
        for (i32 i = 0; i < N; i++)
        {
            cannot_put_[py[i]][px[i]] = true;
            if (py[i] > 0) cannot_put_[py[i] - 1][px[i]] = true;
            if (py[i] < 29)cannot_put_[py[i] + 1][px[i]] = true;
            if (px[i] > 0) cannot_put_[py[i]][px[i] - 1] = true;
            if (px[i] < 29)cannot_put_[py[i]][px[i] + 1] = true;
        }
        auto cannot_put = [&](i32 y, i32 x){
            return y < 0 || y >= 30 || x < 0 || x >= 30 || cannot_put_[y][x];
        };
        auto is_open = [&](i32 y, i32 x)
        {
            if (y < 0 || y >= S || x < 0 || x >= S)
                return false;
            return walls(y, x) != '#';
        };

        string action(M, '?');

        auto exec_action = [&](i32 i, char c){
            assert(action[i] == '?');
            if(c >= 'a' && c <= 'z'){
                i32 dir = dir_i[c - 'a' + 'A'];
                if(cannot_put(hy[i] + dy[dir], hx[i] + dx[dir])){
                    cout << "# Cannot put at (" << hy[i] + dy[dir] << ", " << hx[i] + dx[dir] << ")!\n";
                    c = '.';
                }else{
                    walls(hy[i] + dy[dir], hx[i] + dx[dir]) = '#';
                }
            }else if(c >= 'A' && c <= 'Z'){
                i32 dir = dir_i[c];
                assert(is_open(hy[i] + dy[dir], hx[i] + dx[dir]));
                hy[i] += dy[dir];
                hx[i] += dx[dir];
            }
            action[i] = c;
        };

        // Move constructing humans

        vector<pair<i32, i32>> move_target(M, {-1, -1});
        for (i32 i = 0; i < M; i++)
        {
            
            visit(Overload{[&](Constructing st)
                           {
                               i32 lane = st.lane;
                               bool up = st.up;

                               i32 ddx = lanes[lane].left ? 1 : -1;
                               if (lanes[lane].complete)
                               {
                                   humans[i].state = Free{};
                                   return;
                               }
                               else
                               {

                                   char c = [&]
                                   {
                                       beginning:;
                                           auto [x, col_up] = lanes[lane].columns[up ? lanes[lane].state0 : lanes[lane].state1];
                                           i32 downy = col_up ? 0 : 3;


                                            auto remains = [&, x = x](i32 i){return map(6 * lane + downy + i, x) == '#' && walls(6 * lane + downy + i, x) != '#';};

                                           if (!remains(0) && !remains(1) && !remains(2))
                                           {
                                               if (lanes[lane].state1 - lanes[lane].state0 == 1)
                                               {
                                                   humans[i].state = Free{};
                                                   return '?';
                                               }
                                               if (up)
                                               {
                                                   lanes[lane].state0++;
                                               }
                                               else
                                               {
                                                   lanes[lane].state1--;
                                               }
                                               goto beginning;
                                           }

                                           i32 target_y;
                                           i32 target_x;

                                           if (lanes[lane].state1 - lanes[lane].state0 == 1)
                                           {

                                               cout << "#a " << x << " " << up << " " << i << "\n";
                                               i32 first_open_yy = -1;
                                               i32 first_open_xx = 0;
                                               for (i32 yy : {1, 0, 2})
                                               {
                                                   for (i32 xx : {-1, 1})
                                                   {
                                                       if (is_open(lane * 6 + downy + yy, x + xx))
                                                       {
                                                           first_open_yy = yy;
                                                           first_open_xx = xx;
                                                           break;
                                                       }
                                                   }
                                               }
                                               if (first_open_yy == -1)
                                               {
                                                   cout << "# Locked inside!\n";
                                                   return '.';
                                               }
                                               if((first_open_yy != 0 && remains(0)) || (first_open_yy != 2 && remains(2))){
                                                   target_x = x;
                                                   target_y = lane * 6 + downy + 1;
                                                   if(hx[i] == target_x && hy[i] == target_y){
                                                       if ((first_open_yy != 0 && remains(0)) &&  !cannot_put(target_y - 1, target_x))
                                                           return 'u';
                                                       else if ((first_open_yy != 2 && remains(2)) && !cannot_put(target_y + 1, target_x))
                                                           return 'd';
                                                       else
                                                           return '.';
                                                   }
                                               }else if(remains(1)){
                                                   target_x = x;
                                                   target_y = lane * 6 + downy + first_open_yy;
                                                   if(hx[i] == target_x && hy[i] == target_y){
                                                       return first_open_yy == 0 ? 'd' : 'u';
                                                   }
                                               }else{
                                                   // remains(first_open_yy) == true
                                                   target_x = x + first_open_xx;
                                                   target_y = lane * 6 + downy + first_open_yy;
                                                   if(hx[i] == target_x && hy[i] == target_y){
                                                       return first_open_xx == 1 ? 'l' : 'r';
                                                   }
                                               }
                                           }
                                           else if (x == lanes[lane].x_last && up == col_up)
                                           {
                                               cout << "#b " << x << " " << up << " " << i << "\n";
                                                target_x = x;
                                                i32 ddy = col_up ? 1 : -1;
                                                i32 yy0 = col_up ? 0 : 2;
                                                i32 yy1 = col_up ? 3 : -1;
                                                for(i32 yy = yy0; yy != yy1; yy+=ddy){
                                                    if(remains(yy)){
                                                        target_y = lane * 6 + downy + yy + ddy;
                                                        if(hx[i] == target_x && hy[i] == target_y){
                                                            return ddy == -1 ? 'd' : 'u';
                                                        }
                                                    }
                                                }
                                           }
                                           else
                                           {
                                               cout << "#c " << x << " " << up << " " << i << "\n";
                                               target_y = col_up ? lane * 6 + 1 : lane * 6 + 4;

                                               if (remains(0) || remains(2))
                                               {
                                                   target_x = x;
                                                   if (hy[i] == target_y && hx[i] == target_x)
                                                   {
                                                       if (remains(0) && !cannot_put(target_y - 1, target_x))
                                                           return 'u';
                                                       else if (remains(2) && !cannot_put(target_y + 1, target_x))
                                                           return 'd';
                                                       else
                                                           return '.';
                                                   }
                                               }
                                               else
                                               {
                                                   i32 inc = (up == col_up ? ddx : -ddx);
                                                   target_x = x + (up == col_up ? ddx : -ddx);
                                                   if (hy[i] == target_y && hx[i] == target_x)
                                                   {
                                                       if (!cannot_put(target_y, target_x - inc))
                                                           return inc == 1 ? 'l' : 'r';
                                                       else
                                                           return '.';
                                                   }
                                               }
                                            }
                                            move_target[i] = {target_y, target_x};
                                            cout << "#move_target " << i << " " << target_y << " " << target_x << "\n";
                                            return '?';
                                            // return move_toward(hy[i], hx[i], walls.distmap(target_y, target_x));
                                    }();
                                    if(c != '?'){
                                        cout << "#construction " << i << c << "\n";
                                        exec_action(i, c);
                                    }
                                }
                           },
                           [&](Free) {}},
                  humans[i].state);
        }

        for (i32 i = 0; i < M; i++)
        {
            visit(Overload{[&](const Constructing&)
                           {
                               if(action[i] == '?'){
                                   auto [target_y, target_x] = move_target[i];
                                   if(target_y != -1){
                                       exec_action(i, move_toward(hy[i], hx[i], walls.distmap(target_y, target_x)));
                                   }
                               }
                           },
                           [&](Free) {}}, 
                    humans[i].state);
        }

        vector<char> is_built(chambers.size());
        for(i32 i = 0; i < chambers.size(); i++){
            is_built[i] = true;
            for(auto[y, x]: chambers[i].dependency){
                if(walls(y, x) != '#'){
                    is_built[i] = false;
                    break;
                }
            }
        }
        // Test for putting

        // vector<char> used(M);
        // for(i32 i=0; i<M; i++){
        //     used[i] = (action[i] != '?');
        // }
        for(i32 ch = 0; ch < chambers.size(); ch++){
            if(!is_built[ch]){
                continue;
            }
            if(chambers[ch].locked){
                continue;
            }
            bool has_pet = false;
            for(i32 p = 0; p < N; p++){
                if(chambers[ch].map(py[p], px[p]) == 'o'){
                    has_pet = true;
                    break;
                }
            }

            if(!has_pet){
                continue;
            }
            

            vector<tuple<i32, i32>> open_gates;
            for(i32 i=0; i<chambers[ch].gates.size(); i++){
                auto[y,x] = chambers[ch].gates[i];
                if(walls(y,x) != '#'){
                    open_gates.push_back({y,x});
                }
            }

            if(open_gates.size() == 0){
                cout << "# Already shut!!\n";
                continue;
            }

            for(i32 h = 0; h < M; h++){
                if(chambers[ch].map(hy[h], hx[h]) == 'o' || chambers[ch].map(hy[h], hx[h]) == 'y'){
                    cout << "# has_human " << ch << " " << h << "\n";
                    continue; // has_human
                }
            }

            bool bad = false;
            for(auto[gy, gx]: open_gates){
                if(cannot_put(gy, gx)){bad=true; break;}
            }
            if(bad){continue;}

            vector<pair<i32, char>> actions;
            for(auto[gy, gx]: open_gates){
                for(i32 i = 0; i<M; i++){
                    if(action[i] != '?'){continue;}
                    i32 y = hy[i];
                    i32 x = hx[i];
                    if(chambers[ch].map(y, x) != 'y' && abs(y-gy) + abs(x-gx) == 1){
                        if(y - gy == -1) actions.push_back({i, 'd'});
                        else if(y - gy == 1) actions.push_back({i, 'u'});
                        else if(x - gx == -1) actions.push_back({i, 'r'});
                        else if(x - gx == 1) actions.push_back({i, 'l'});
                        goto ok;
                    }
                }
                break;
                ok:;
            }
            if(actions.size() != open_gates.size()){
                continue;
            }
            chambers[ch].locked = true;
            for(auto[i, a]: actions){
                // used[i] = true;
                exec_action(i, a);
            }
            
        }



        // auto delay = calc_delay();

        // Test for moving


        auto simulate = [&](const Map& walls, i32 p, i32 T){

            auto is_open = [&](i32 y, i32 x)
            {
                if (y < 0 || y >= S || x < 0 || x >= S)
                    return false;
                return walls(y, x) != '#';
            };
            array<array<double, S>, S> prob_buf1, prob_buf2{};
            prob_buf1 = {};
            prob_buf2 = {};
            array<array<double, S>, S>* prob1 = &prob_buf1;
            array<array<double, S>, S>* prob2 = &prob_buf2;           
            (*prob1)[py[p]][px[p]] = 1.0;


            auto basic_move = [&]{
                *prob2 = {};
                for(i32 y = 0; y < S; y++){
                    for(i32 x = 0; x < S; x++){
                        i32 open_count = 0;
                        for(i32 d = 0; d < 4; d++){
                            if(is_open(y + dy[d], x + dx[d])){
                                open_count++;
                            }
                        }
                        for(i32 d = 0; d < 4; d++){
                            if(is_open(y + dy[d], x + dx[d])){
                                // if(open_count == 0) cerr << "!!" << endl;
                                (*prob2)[y + dy[d]][x + dx[d]] += (*prob1)[y][x] / open_count;
                            }
                        }
                    }
                }
                swap(prob1, prob2);
            };


            auto track = [&](const auto& distmap){
                *prob2 = {};
                for(i32 y = 0; y < S; y++){
                    for(i32 x = 0; x < S; x++){
                        i32 open_count = 0;
                        i32 possible_count = 0;
                        for(i32 d = 0; d < 4; d++){
                            if(is_open(y + dy[d], x + dx[d])){
                                open_count++;
                                if(distmap[y + dy[d]][x + dx[d]] < distmap[y][x]){
                                    possible_count++;
                                }
                            }
                        }
                        if(possible_count > 0){
                            for(i32 d = 0; d < 4; d++){
                                if(is_open(y + dy[d], x + dx[d]) && distmap[y + dy[d]][x + dx[d]] < distmap[y][x]){
                                    (*prob2)[y + dy[d]][x + dx[d]] += (*prob1)[y][x] / possible_count;
                                }
                            }
                        }else{
                            for(i32 d = 0; d < 4; d++){
                                if(is_open(y + dy[d], x + dx[d]) ){
                                    (*prob2)[y + dy[d]][x + dx[d]] += (*prob1)[y][x] / open_count;
                                }
                            }
                        }
                    }
                }
                swap(prob1, prob2);
            };
            // if(pt[p] == 4){

            //     vector<double> probs = dog_estimations[p].probs;
            //     double sum = [&]{
            //         double s = 0;
            //         for(auto p: probs){
            //             s += p;
            //         }
            //         return s;
            //     }();

            //     discrete_distribution<> distr;
            //     if(sum == 0){
            //         probs = vector<double>(M, 1.0/M);
            //     }
            //     distr = discrete_distribution<>(probs.begin(), probs.end());
                
            //     array<array<double, S>, S> ret{};
            //     constexpr i32 N_SAMPLE = 5;
            //     for(i32 sample = 0; sample < N_SAMPLE; sample++){
            //         i32 ty = distr(rng);
            //         i32 tx = distr(rng);
            //         auto distmap = walls.distmap(ty, tx);
            //         for(i32 t = 0; t < min(T, 300 - a); t++){
            //             track(distmap);
            //             basic_move();
            //         }

            //         for(i32 y = 0; y < S; y++){
            //             for(i32 x = 0; x < S; x++){
            //                 ret[y][x] += (*prob1)[y][x] / N_SAMPLE;
            //             }
            //         }
            //     }
            // }
            // else{

                for(i32 t = 0; t < min(T, 300 - a); t++){
                    if(pt[p] == 1){
                        basic_move();
                    }else if(pt[p] == 2){
                        basic_move();
                        basic_move();
                    }else if(pt[p] == 3){
                        basic_move();
                        basic_move();
                        basic_move();
                    }else if(pt[p] == 4 || pt[p] == 5){
                        basic_move();
                        basic_move();
                        basic_move();
                        basic_move();
                    }
                }
            // }

            // ret += min(T, 300 - a) * (1 - p_absorbed);
            return *prob1;
        };

        constexpr i32 T = 3;
        vector<array<array<double, S>, S>> prob_maps(N);
        vector<double> score(chambers.size());

        for(i32 i = 0; i < N; i++){
            if(!alive[i]){continue;}
            prob_maps[i] = simulate(walls, i, T);
            vector<double> new_score(chambers.size());
            for(i32 y = 0; y < S; y++){
                for(i32 x = 0; x < S; x++){
                    i32 ch = chamber_id[y][x];
                    if(ch == -1){
                        continue;
                    }
                    if(!is_built[ch]){
                        continue;
                    }
                    new_score[ch] += prob_maps[i][y][x];
                }
            }
            for(i32 ch = 0; ch < chambers.size(); ch++){
                if(!is_built[ch]){
                    continue;
                }
                score[ch] = max(score[ch], new_score[ch]);
            }
        }

        auto min_argmin = [&](const auto& x, const auto& y){
            const auto& [m1, argmin1] = x;
            const auto& [m2, argmin2] = y;
            if(m2 < m1){
                return y;
            }else{
                return x;
            }
        };


        auto locktime2 = [&](i32 ch, const Map& walls, const vector<array<array<i32, S>, S>> & distmaps) -> pair<i32, vector<tuple<i32, i32, i32>>>{

            auto is_open = [&](i32 y, i32 x)
            {
                if (y < 0 || y >= S || x < 0 || x >= S)
                    return false;
                return walls(y, x) != '#';
            };

            vector<tuple<i32, i32, i32>> ret;
            if(!is_built[ch]){
                return {INF, {}};
            }

            vector<tuple<i32, i32>> open_gates;
            for(i32 i=0; i<chambers[ch].gates.size(); i++){
                auto[y,x] = chambers[ch].gates[i];
                if(walls(y,x) != '#'){
                    open_gates.push_back({y,x});
                }
            }
            
            if(open_gates.size() == 0){
                return {-1, {}};
            } else if(open_gates.size() == 1){
                pair<i32, tuple<i32, i32, i32>> min_time = {INF, {-1, -1, -1}};
                for(i32 i = 0; i < M; i++){
                    for(i32 d = 0; d < 4; d++){
                        auto [y,x] = open_gates[0];
                        y += dy[d];
                        x += dx[d];
                        if(is_open(y, x) && chambers[ch].map(y, x) != 'y'){
                            min_time = min_argmin(min_time, pair<i32, tuple<i32, i32, i32>>{distmaps[i][y][x], {i, y, x}});
                        }
                    }
                }
                return {min_time.first, {min_time.second}};
            }else if(open_gates.size() == 2){
                array<vector<pair<i32, tuple<i32, i32, i32>>>, 2> min_time_raw{vector<pair<i32, tuple<i32, i32, i32>>>(M), vector<pair<i32, tuple<i32, i32, i32>>>(M)};
                // array<vector<tuple<i32, i32, i32>>, 2> argmin_raw{vector<tuple<i32, i32, i32>>(M), vector<tuple<i32, i32, i32>>(M)};
                for(i32 g = 0; g < 2; g++){
                    for(i32 i = 0; i < M; i++){
                        pair<i32, tuple<i32, i32, i32>> min_time = {INF, {-1, -1, -1}};
                        for(i32 d = 0; d < 4; d++){
                            auto [y,x] = open_gates[g];
                            y += dy[d];
                            x += dx[d];
                            if(is_open(y, x) && chambers[ch].map(y, x) != 'y'){
                                min_time = min_argmin(min_time, pair<i32, tuple<i32, i32, i32>>{distmaps[i][y][x], {i, y, x}});
                            }
                        }
                        min_time_raw[g][i] = min_time;
                    }
                }
                
                vector<pair<i32, tuple<i32, i32, i32>>> min_time_a(M), min_time_b(M);

                min_time_a[0] = {INF, {-1, -1, -1}};
                for(i32 i=1; i<M; i++){
                    min_time_a[i] = min_argmin(min_time_a[i-1], min_time_raw[1][i-1]);
                }
                min_time_b[M-1] = {INF, {-1, -1, -1}};
                for(i32 i = M-2; i>=0; i--){
                    min_time_b[i] = min_argmin(min_time_b[i+1], min_time_raw[1][i+1]);
                }

                pair<i32, vector<tuple<i32, i32, i32>>> ret = {INF, {}};
                for(i32 i=0; i<M; i++){
                    auto [m, arg] = min_argmin(min_time_a[i], min_time_b[i]);
                    auto [m2, arg2] = min_time_raw[0][i];
                    ret = min_argmin(ret, pair<i32, vector<tuple<i32, i32, i32>>>{m+m2, {arg, arg2}});
                }
                return ret;

            }else{
                assert(false);
                return {INF, {}};
            }
        };

        vector<array<array<i32, S>, S>> distmaps(M);
        // vector<char> available(M);
        // vector<i32> delay(M);
        for(i32 i = 0; i < M; i++){
            if(!std::holds_alternative<Free>(humans[i].state)){
                for(i32 y = 0; y < S; y++){
                    for(i32 x = 0; x < S; x++){
                        distmaps[i][y][x] = INF;
                    }
                }
                // delay[i] = INF;
            }else{
                distmaps[i] = walls.distmap(hy[i], hx[i]);
                for(i32 y = 0; y < S; y++){
                    for(i32 x = 0; x < S; x++){
                        distmaps[i][y][x] += (action[i] != '?' ? 1 : 0);
                    }
                }
            }
        }
        vector<char> chamber_used(chambers.size());
        while(true){
            i32 available_humans = 0;
            for(i32 i = 0; i < M; i++){
                if(action[i] == '?'){
                    available_humans++;
                }
            }

            if(available_humans == 0){
                break;
            }
            pair<double, tuple<i32, i32, vector<tuple<i32, i32, i32>>>> min_cost = {INF, {-1, INF, {}}};
            for(i32 i = 0; i < chambers.size(); i++){
                if(chamber_used[i] || !is_built[i] || chambers[i].locked){
                    continue;
                }
                auto [lt, actions] = locktime2(i, walls, distmaps);
                if(lt == INF || lt == -1){
                    continue;
                }
                double s = score[i];
                double cost = -s / (lt + 3.0);

                min_cost = min_argmin(min_cost, pair<double, tuple<i32, i32, vector<tuple<i32, i32, i32>>>>{cost, {i, lt, actions}});

                // cout << "# cost " << i << " " << cost << " - min: " << min_cost.first << endl;
            }
            if(min_cost.first == INF){
                break;
            }

            auto [ch, lt, actions] = min_cost.second;
                chamber_used[ch] = true;
                for(auto [h, y, x] : actions){
                    if(action[h] == '?'){
                        cout << "# move_toward " << h << " " << y << " " << x << "  - " << ch << " " << lt << endl;
                        exec_action(h, move_toward(hy[h], hx[h], walls.distmap(y, x)));
                    }
                    distmaps[h] = walls.distmap(y, x);
                    for(i32 y = 0; y < S; y++){
                        for(i32 x = 0; x < S; x++){
                            distmaps[h][y][x] += lt + 1;
                        }
                    }
                }

        }
        

        
        // for(i32 i = 0; i < M; i++){
        //     if(action[i] != '?' || delay[i] != 0){continue;}
        //     delay[i] = 1;
        //     auto test = [&](i32 ddy, i32 ddx){
        //         auto nhy = hy;
        //         auto nhx = hx;
        //         nhy[i] += ddy;
        //         nhx[i] += ddx;
        //         double ret = score(walls, nhx, nhy, delay);
        //         cout << "#" << i << " " << ddy << " " << ddx << " " << ret << "\n";
        //         return ret;
        //     };

        //     double best = test(0, 0);
        //     char best_move = '.';
        //     for(i32 d = 0; d < 4; d++){

        //         if(!is_open(hy[i] + dy[d], hx[i] + dx[d])){
        //             continue;
        //         }
        //         double score = test(dy[d], dx[d]);
        //         if(score > best){
        //             best = score;
        //             best_move = dirs[d];
        //         }
        //     }
        //     exec_action(i, best_move);
        // }


        for(i32 i = 0; i < M; i++){
            if(action[i] == '?'){
                cout << "# Unknown action!\n";
                action[i] = '.';
            }
        }

        // update alive

        for(auto& chamber: chambers){
            bool locked = true;
            for(auto[y,x]: chamber.gates){
                if(is_open(y, x)){
                    locked = false;
                    break;
                }
            }
            chamber.locked = locked;
        }
        for(i32 i = 0; i < M; i++){
            if(!alive[i]) continue;
            for(const auto& chamber: chambers){
                if((chamber.map(py[i], px[i]) == 'o' || chamber.map(py[i], px[i]) == 'y') && chamber.locked){
                    alive[i] = false;
                    break;
                }
            }
        }

        cout << action << endl;
        // cerr << action << endl;
        for (i32 i = 0; i < N; i++)
        {
            std::string pet_movement;
            cin >> pet_movement;

            
            // if(alive[i]){
            //     if(pt[i] == 4){
            //         dog_estimations[i].estimate(walls, py[i], px[i], dir_i[pet_movement[0]], dir_i[pet_movement[1]], hy, hx);

            //         double max_p = 0.0;
            //         i32 max_j = -1;
            //         for(i32 j = 0; j < M; j++){
            //             if(dog_estimations[i].probs[j] > max_p){
            //                 max_p = dog_estimations[i].probs[j];
            //                 max_j = j;
            //             }
            //         }
            //         cout << "# estimate_dog " << i << ": " << max_j << " " << max_p << endl;
            //     }
            //     if(pt[i] == 5){
            //         cat_estimations[i].estimate(walls, py[i], px[i], dir_i[pet_movement[0]], dir_i[pet_movement[1]]);
            //         double max_p = 0.0;
            //         i32 max_y = -1, max_x = -1;
            //         for(i32 y = 0; y < S; y++){
            //             for(i32 x = 0; x < S; x++){
            //                 if(cat_estimations[i].probs[y][x] > max_p){
            //                     max_p = cat_estimations[i].probs[y][x];
            //                     max_y = y;
            //                     max_x = x;
            //                 }
            //             }
            //         }
            //         cout << "# estimate_cat " << i << ": " << max_y << " " << max_x << " " << max_p << endl;
            //     }
            // }
            

            for (char c : pet_movement)
            {
                if (c == 'L')
                    px[i]--;
                else if (c == 'R')
                    px[i]++;
                else if (c == 'U')
                    py[i]--;
                else if (c == 'D')
                    py[i]++;
            }

        }
    }
}
