#include <algorithm>
#include <bitset>
#include <cassert>
#include <cmath>
#include <sstream>
#include <fstream>
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <numeric>
#include <vector>
#pragma GCC optimize("Ofast")
#pragma GCC target("avx2")
#define REP(i, N) for (int i = 0; i < (int)N; i++)
#define FOR(i, a, b) for (int i = a; i < (int)b; i++)
#define ALL(v) v.begin(), v.end()

using namespace std;
using Pii = pair<int, int>;

struct Timer {
  static constexpr double ONE_CLOCK = 0.000000333;
  unsigned long long get_cycle() {
    unsigned int low, high;
    __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
    return ((unsigned long long int)low) | ((unsigned long long int)high << 32);
  }
  unsigned long long start_time, last_time;
  Timer() { start_time = last_time = get_cycle(); }
  int get_time() { return get_cycle() * ONE_CLOCK - start_time * ONE_CLOCK; }
  int split_time() { return get_cycle() * ONE_CLOCK - last_time * ONE_CLOCK; }
  void split() { last_time = get_cycle(); }
} timer;

class RandInt {
public:
  int x = 8753, y = 239017, z = 1000000123;
  int next(int d) {
    int t = x ^ (x << 11);
    x = y;
    y = z;
    z ^= (z >> 19) ^ t ^ (t >> 8);
    return z % d;
  }
} ri;

template <class T> string to_string(T s);
template <class S, class T> string to_string(pair<S, T> p);
string to_string(char c) { return string(1, c); }
string to_string(string s) { return s; }
string to_string(const char s[]) { return string(s); }

template <class T> string to_string(T v) {
  if (v.empty())
    return "{}";
  string ret = "{";
  for (auto x : v)
    ret += to_string(x) + ",";
  ret.back() = '}';
  return ret;
}

template <class S, class T> string to_string(pair<S, T> p) {
  return "{" + to_string(p.first) + ":" + to_string(p.second) + "}";
}
void debug() { cerr << endl; }

template <class Head, class... Tail> void debug(Head head, Tail... tail) {
  cerr << to_string(head) << " ";
  debug(tail...);
}

inline bool bit(long long x, int b) { return (x >> b) & 1; }

template <class T> bool chmax(T &a, const T &b) {
  if (a < b) {
    a = b;
    return 1;
  }
  return 0;
}
template <class T> bool chmin(T &a, const T &b) {
  if (b < a) {
    a = b;
    return 1;
  }
  return 0;
}

int inf = 1 << 25;
int dy[4] = {-1, 1, 0, 0}, dx[4] = {0, 0, -1, 1};
string moves = "UDLR", places = "udlr";

int block[30][30], humann[30][30], petn[30][30];

int N, M;

int getas[6][11] = {
    {6, 6, 6, 6, 6, 6, 5, 5, 6, 6, 6}, {6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5},
    {6, 6, 6, 5, 5, 5, 7, 7, 6, 6, 6}, {6, 6, 6, 6, 6, 6, 8, 8, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6}, {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6}};

double bias1s[6][11] = {
    {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 7.6, 7.6, 1.88, 1.88, 1.88},
    {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 3.73, 3.73, 6.76, 6.76, 6.76},
    {2.0, 2.0, 2.0, 5.17, 5.17, 5.17, 2.9, 2.9, 2.83, 2.83, 2.83},
    {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 1.83, 1.83, 4.73, 4.73, 4.73},
    {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0},
    {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 5.3, 5.3, 5.3}};
double bias2s[6][11] = {
    {4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 7.5, 7.5, 6.87, 6.87, 6.87},
    {4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 1.42, 1.42, 7.08, 7.08, 7.08},
    {4.0, 4.0, 4.0, 4.87, 4.87, 4.0, 4.5, 4.5, 4.92, 4.92, 4.92},
    {4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 3.77, 3.77, 7.73, 7.73, 7.73},
    {4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0},
    {4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 3.58, 3.58, 3.58}};
double cuts[6][11] = {
    {15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 18.9, 18.9, 16.35, 16.35, 16.35},
    {15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 21.22, 21.22, 7.67, 7.67, 7.67},
    {15.0, 15.0, 15.0, 21.4, 21.4, 21.4, 10.84, 10.84, 23.14, 23.14, 23.14},
    {15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 23, 23, 27.43, 27.43, 27.43},
    {15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0},
    {15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 22.58, 22.58, 22.58}};

int geta = 6, canmove = -1;
double bias1 = 5.0, bias2 = 4.0, cut = 20.0;

struct Point {
  int y, x;
  Point() {}
  Point(int _y, int _x) : y(_y), x(_x) {}
  bool valid() { return 0 <= y && y < 30 && 0 <= x && x < 30; }
  Point next(int dir) { return Point(y + dy[dir], x + dx[dir]); }
  int dist(Point p) { return abs(y - p.y) + abs(x - p.x); }
  int id() { return y * 30 + x; }
};

struct Order {
  vector<Point> pos;
  int type;
  Order() {}
  Order(Point _pos, int _type) : pos({_pos}), type(_type) {}
  Order(Point _pos1, Point _pos2, int _type)
      : pos({_pos1, _pos2}), type(_type) {}
};

struct Pet {
  Point pos, tpos;
  int thuman = -1;
  int t;
  int last = -1;
  Pet() {
    tpos.y = -1;
    thuman = -1;
  }
  void move(char c) {
    if (c == 'U')
      pos = pos.next(0);
    else if (c == 'D')
      pos = pos.next(1);
    else if (c == 'L')
      pos = pos.next(2);
    else if (c == 'R')
      pos = pos.next(3);
  }
};
Pet pets[20];

bool can_place(Point p) {
  if (petn[p.y][p.x] > 0 || humann[p.y][p.x] > 0 || block[p.y][p.x])
    return 0;
  REP(dir, 4) {
    Point n = p.next(dir);
    if (n.valid()) {
      if (petn[n.y][n.x] > 0)
        return 0;
    }
  }
  return 1;
}

struct Human {
  Point pos;
  int idx;
  bool achieved(Order order) {
    if (order.type == 0) {
      return pos.id() == order.pos[0].id();
    } else {
      for (auto p : order.pos)
        if (block[p.y][p.x] == 0)
          return false;
      return true;
    }
  }
  char move(int dir) {
    humann[pos.y][pos.x]--;
    pos = pos.next(dir);
    humann[pos.y][pos.x]++;
    return moves[dir];
  }
  char do_order(Order order) {
    if (order.type == 0) {
      if (pos.id() == order.pos[0].id())
        return '.';
      vector<Pii> ls;
      REP(dir, 4) {
        Point p = pos.next(dir);
        if (p.valid() && block[p.y][p.x] == 0) {
          vector<vector<int>> dists(30, vector<int>(30, inf));
          queue<Point> q;
          dists[p.y][p.x] = 0;
          q.push(p);
          while (!q.empty()) {
            Point u = q.front();
            q.pop();
            if (u.id() == order.pos[0].id()) {
              ls.emplace_back(dists[u.y][u.x], dir);
              break;
            }
            REP(d, 4) {
              Point v = u.next(d);
              if (v.valid() && block[v.y][v.x] == 0 && dists[v.y][v.x] == inf) {
                dists[v.y][v.x] = dists[u.y][u.x] + 1;
                q.push(v);
              }
            }
          }
        }
      }
      sort(ALL(ls));
      if (ls.empty())
        return '.';
      else {
        humann[pos.y][pos.x]--;
        pos = pos.next(ls[0].second);
        humann[pos.y][pos.x]++;
        return moves[ls[0].second];
      }
    } else {
      for (auto p : order.pos) {
        if (can_place(p)) {
          block[p.y][p.x] = 1;
          if (p.y < pos.y)
            return 'u';
          else if (p.y > pos.y)
            return 'd';
          else if (p.x < pos.x)
            return 'l';
          else
            return 'r';
        }
      }
      return '.';
    }
  };
};

Human humans[10];

#include <random>

namespace judge {
int local = 0;
int seed = 0;
int Nmin = 10, Nmax = 20, Mmin = 5, Mmax = 10, Gmin = 0, Gmax = 20, N, M, G = 0;
Pet pets[20];
Human humans[10];
int petn[30][30], humann[30][30], block[30][30];
ofstream out;
void make_problem(int _N = -1, int _M = -1) {
  if (!local) {
    cin >> N;
    REP(i, N) {
      cin >> pets[i].pos.y >> pets[i].pos.x >> pets[i].t;
      pets[i].pos.y--;
      pets[i].pos.x--;
      petn[pets[i].pos.y][pets[i].pos.x]++;
    }
    cin >> M;
    REP(i, M) {
      cin >> humans[i].pos.y >> humans[i].pos.x;
      humans[i].pos.y--;
      humans[i].pos.x--;
      humann[humans[i].pos.y][humans[i].pos.x]++;
    }
  } else {
    ofstream in("input.txt");
    ri.x += seed;
    random_device seed_gen;
    mt19937 engine(seed);
    vector<Point> points;
    REP(i, 30) REP(j, 30) points.emplace_back(i, j);
    shuffle(ALL(points), engine);
    N = ri.next(Nmax - Nmin + 1) + Nmin;
    in << N << endl;
    REP(i, N) { pets[i].t = ri.next(5) + 1; }
    G = 0;
    REP(i, N) if (pets[i].t < 4) G++;
    while (G < Gmin || Gmax < G) {
      REP(i, N) { pets[i].t = ri.next(5) + 1; }
      G = 0;
      REP(i, N) if (pets[i].t < 4) G++;
    }
    REP(i, N) {
      pets[i].pos = points[i];
      in << (pets[i].pos.y + 1) << " " << (pets[i].pos.x + 1) << " "
         << pets[i].t << endl;
      petn[pets[i].pos.y][pets[i].pos.x]++;
    }
    M = ri.next(Mmax - Mmin + 1) + Mmin;
    in << M << endl;
    REP(i, M) {
      humans[i].pos = points[N + i];
      in << (humans[i].pos.y + 1) << " " << (humans[i].pos.x + 1) << " "
         << endl;

      humann[humans[i].pos.y][humans[i].pos.x]++;
    }
    in << 0 << endl;
    out = ofstream("output.txt");
  }
}
char basic_move(Pet &pet) {
  vector<int> cand;
  REP(dir, 4) {
    Point p = pet.pos.next(dir);
    if (p.valid() && !block[p.y][p.x]) {
      cand.push_back(dir);
    }
  }
  int dir = cand[ri.next(cand.size())];
  petn[pet.pos.y][pet.pos.x]--;
  pet.pos = pet.pos.next(dir);
  petn[pet.pos.y][pet.pos.x]--;
  return moves[dir];
}
void set_thuman(Pet &pet) {
  int dists[30][30];
  REP(i, 30) REP(j, 30) dists[i][j] = inf;
  queue<Point> q;
  dists[pet.pos.y][pet.pos.x] = 0;
  q.push(pet.pos);
  vector<int> cand;
  while (!q.empty()) {
    Point u = q.front();
    q.pop();
    if (pet.pos.id() != u.id())
      REP(i, M) if (humans[i].pos.id() == u.id()) cand.push_back(i);
    REP(d, 4) {
      Point v = u.next(d);
      if (v.valid() && block[v.y][v.x] == 0 && dists[v.y][v.x] == inf) {
        dists[v.y][v.x] = dists[u.y][u.x] + 1;
        q.push(v);
      }
    }
  }
  if (cand.size() >= 1)
    pet.thuman = cand[ri.next(cand.size())];
  else
    pet.thuman = -1;
}
void set_tpos(Pet &pet) {
  int dists[30][30];
  REP(i, 30) REP(j, 30) dists[i][j] = inf;
  queue<Point> q;
  dists[pet.pos.y][pet.pos.x] = 0;
  q.push(pet.pos);
  vector<Point> cand;
  while (!q.empty()) {
    Point u = q.front();
    q.pop();
    if (pet.pos.id() != u.id())
      cand.push_back(u);
    REP(d, 4) {
      Point v = u.next(d);
      if (v.valid() && block[v.y][v.x] == 0 && dists[v.y][v.x] == inf) {
        dists[v.y][v.x] = dists[u.y][u.x] + 1;
        q.push(v);
      }
    }
  }
  if (cand.size() >= 1)
    pet.tpos = cand[ri.next(cand.size())];
  else
    pet.tpos.y = -1;
}
vector<string> interact(string ans) {
  vector<string> acts(N);
  if (!local) {
    cout << ans << endl;
    REP(i, N)
    cin >> acts[i];
  } else {
    out << ans << endl;
    REP(i, M) {
      char c = ans[i];
      REP(j, 4) if (c == moves[j]) {
        humann[humans[i].pos.y][humans[i].pos.x]--;
        humans[i].pos = humans[i].pos.next(j);
        humann[humans[i].pos.y][humans[i].pos.x]++;
      }
      REP(j, 4) if (c == places[j]) {
        Point pos = humans[i].pos.next(j);
        block[pos.y][pos.x] = 1;
      }
    }
    REP(i, N) {
      if (pets[i].t <= 3) {
        REP(j, pets[i].t) acts[i] += basic_move(pets[i]);
      } else if (pets[i].t == 4) {
        if (pets[i].pos.id() == humans[pets[i].thuman].pos.id())
          pets[i].thuman = -1;
        vector<Pii> ls;
        if (pets[i].thuman != -1) {
          REP(dir, 4) {
            Point p = pets[i].pos.next(dir);
            if (p.valid() && block[p.y][p.x] == 0) {
              int dists[30][30];
              REP(i, 30) REP(j, 30) dists[i][j] = inf;
              queue<Point> q;
              dists[p.y][p.x] = 0;
              q.push(p);
              while (!q.empty()) {
                Point u = q.front();
                q.pop();
                if (u.id() == humans[pets[i].thuman].pos.id()) {
                  ls.emplace_back(dists[u.y][u.x], dir);
                  break;
                }
                REP(d, 4) {
                  Point v = u.next(d);
                  if (v.valid() && block[v.y][v.x] == 0 &&
                      dists[v.y][v.x] == inf) {
                    dists[v.y][v.x] = dists[u.y][u.x] + 1;
                    q.push(v);
                  }
                }
              }
            }
          }
          sort(ALL(ls));
        }
        if (ls.empty())
          pets[i].thuman = -1;
        if (pets[i].thuman == -1) {
          set_thuman(pets[i]);
          if (pets[i].thuman != -1) {
            REP(dir, 4) {
              Point p = pets[i].pos.next(dir);
              if (p.valid() && block[p.y][p.x] == 0) {
                int dists[30][30];
                REP(i, 30) REP(j, 30) dists[i][j] = inf;
                queue<Point> q;
                dists[p.y][p.x] = 0;
                q.push(p);
                while (!q.empty()) {
                  Point u = q.front();
                  q.pop();
                  if (u.id() == humans[pets[i].thuman].pos.id()) {
                    ls.emplace_back(dists[u.y][u.x], dir);
                    break;
                  }
                  REP(d, 4) {
                    Point v = u.next(d);
                    if (v.valid() && block[v.y][v.x] == 0 &&
                        dists[v.y][v.x] == inf) {
                      dists[v.y][v.x] = dists[u.y][u.x] + 1;
                      q.push(v);
                    }
                  }
                }
              }
            }
            sort(ALL(ls));
          }
        }
        if (!ls.empty()) {
          while (ls.back().first > ls[0].first)
            ls.pop_back();
          int dir = ls[ri.next(ls.size())].second;
          petn[pets[i].pos.y][pets[i].pos.x]--;
          pets[i].move(moves[dir]);
          acts[i] += moves[dir];
          petn[pets[i].pos.y][pets[i].pos.x]++;
          if (pets[i].pos.id() == humans[pets[i].thuman].pos.id())
            pets[i].thuman = -1;
        }
        acts[i] += basic_move(pets[i]);
        if (pets[i].pos.id() == humans[pets[i].thuman].pos.id())
          pets[i].thuman = -1;
      } else if (pets[i].t == 5) {
        if (pets[i].pos.id() == pets[i].tpos.id())
          pets[i].tpos.y = -1;
        vector<Pii> ls;
        if (pets[i].tpos.y != -1) {
          REP(dir, 4) {
            Point p = pets[i].pos.next(dir);
            if (p.valid() && block[p.y][p.x] == 0) {
              int dists[30][30];
              REP(i, 30) REP(j, 30) dists[i][j] = inf;
              queue<Point> q;
              dists[p.y][p.x] = 0;
              q.push(p);
              while (!q.empty()) {
                Point u = q.front();
                q.pop();
                if (u.id() == pets[i].tpos.id()) {
                  ls.emplace_back(dists[u.y][u.x], dir);
                  break;
                }
                REP(d, 4) {
                  Point v = u.next(d);
                  if (v.valid() && block[v.y][v.x] == 0 &&
                      dists[v.y][v.x] == inf) {
                    dists[v.y][v.x] = dists[u.y][u.x] + 1;
                    q.push(v);
                  }
                }
              }
            }
          }
          sort(ALL(ls));
        }
        if (ls.empty())
          pets[i].tpos.y = -1;
        if (pets[i].tpos.y == -1) {
          set_tpos(pets[i]);
          if (pets[i].tpos.y != -1) {
            REP(dir, 4) {
              Point p = pets[i].pos.next(dir);
              if (p.valid() && block[p.y][p.x] == 0) {
                int dists[30][30];
                REP(i, 30) REP(j, 30) dists[i][j] = inf;
                queue<Point> q;
                dists[p.y][p.x] = 0;
                q.push(p);
                while (!q.empty()) {
                  Point u = q.front();
                  q.pop();
                  if (u.id() == pets[i].tpos.id()) {
                    ls.emplace_back(dists[u.y][u.x], dir);
                    break;
                  }
                  REP(d, 4) {
                    Point v = u.next(d);
                    if (v.valid() && block[v.y][v.x] == 0 &&
                        dists[v.y][v.x] == inf) {
                      dists[v.y][v.x] = dists[u.y][u.x] + 1;
                      q.push(v);
                    }
                  }
                }
              }
            }
            sort(ALL(ls));
          }
        }
        if (!ls.empty()) {
          while (ls.back().first > ls[0].first)
            ls.pop_back();
          int dir = ls[ri.next(ls.size())].second;
          petn[pets[i].pos.y][pets[i].pos.x]--;
          pets[i].move(moves[dir]);
          acts[i] += moves[dir];
          petn[pets[i].pos.y][pets[i].pos.x]++;
          if (pets[i].pos.id() == pets[i].tpos.id())
            pets[i].tpos.y = -1;
        }
        acts[i] += basic_move(pets[i]);
        if (pets[i].pos.id() == pets[i].tpos.id())
          pets[i].tpos.y = -1;
      }
      out << acts[i] << (i + 1 < N ? " " : "");
    }
    out << endl;
  }
  return acts;
}
int get_score() {
  int score = 0;
  REP(i, M) {
    int dists[30][30];
    REP(i, 30) REP(j, 30) dists[i][j] = inf;
    queue<Point> q;
    dists[humans[i].pos.y][humans[i].pos.x] = 0;
    q.push(humans[i].pos);
    int cnt = 0, pcnt = 0;
    while (!q.empty()) {
      Point u = q.front();
      q.pop();
      cnt++;
      REP(j, N) if (pets[j].pos.id() == u.id()) pcnt++;
      REP(d, 4) {
        Point v = u.next(d);
        if (v.valid() && block[v.y][v.x] == 0 && dists[v.y][v.x] == inf) {
          dists[v.y][v.x] = dists[u.y][u.x] + 1;
          q.push(v);
        }
      }
    }
    score += 1e8 * cnt / pow(2, pcnt) / 900 / M;
  }
  return score;
}
}; // namespace judge

namespace solver_prev {
namespace solver_cols {

int C = 5;
int K = 5;
int built[5][30], row_used[5][30];
int row_l[5] = {0, 6, 13, 20, 27}, row_r[5] = {2, 9, 16, 23, 29};
int row_c1[5] = {0, 7, 14, 21, 29}, row_c2[5] = {0, 8, 15, 22, 29};
int col_type[5];
int pet_depth[5][30];

int get_dist_closest_pet(int col, int row) {
  int dists[30][30];
  REP(i, 30) REP(j, 30) dists[i][j] = inf;
  queue<Point> q;
  dists[row][row_c1[col]] = dists[row][row_c2[col]] = 0;
  q.push(Point(row, row_c1[col]));
  q.push(Point(row, row_c2[col]));
  int cnt = 0;
  int mn = inf;
  while (!q.empty()) {
    Point u = q.front();
    q.pop();
    cnt++;
    if (petn[u.y][u.x] > 0) {
      chmin(mn, dists[u.y][u.x]);
    }
    REP(d, 4) {
      Point v = u.next(d);
      if (v.valid() && block[v.y][v.x] == 0 && dists[v.y][v.x] == inf) {
        dists[v.y][v.x] = dists[u.y][u.x] + 1;
        q.push(v);
      }
    }
  }
  return cnt < 17 ? inf : mn;
}

void set_pet_depth() {
  REP(i, C) REP(r, 30) pet_depth[i][r] = -inf;
  for (int col = 0; col < C; ++col) {
    for (int r = 0; r <= 28; r += 2) {
      if (!row_used[col][r] && built[col][r]) {
        int dist = get_dist_closest_pet(col, r);
        if (dist == inf) {
          row_used[col][r] = 1;
          pet_depth[col][r] = -inf;
        } else {
          if (col_type[col] == 1)
            pet_depth[col][r] = row_r[col] - row_l[col] - dist + geta;
          else if (col_type[col] == 2)
            pet_depth[col][r] = row_r[col] - row_l[col] - dist + geta;
          else
            pet_depth[col][r] = row_r[col] - row_c2[col] - dist + geta;

          if (pet_depth[col][r] >= geta + 1)
            pet_depth[col][r]++;
          chmax(pet_depth[col][r], 0);
        }
      } else {
        pet_depth[col][r] = -inf;
      }
    }
  }
}

double eval_state() {
  double ret = 0;

  REP(col, C) for (int row = 0; row <= 28; row += 2) {
    if (pet_depth[col][row] >= 0) {
      int lmin1 = inf, lmin2 = inf, rmin1 = inf, rmin2 = inf;

      REP(i, M) {
        if (col_type[col] != 1 && humans[i].pos.x == row_l[col] - 2) {
          int d = abs(row - humans[i].pos.y);
          if (d <= lmin1) {
            lmin2 = lmin1;
            lmin1 = d;
          } else if (d < lmin2) {
            lmin2 = d;
          }
        }
        if (col_type[col] != 2 && humans[i].pos.x == row_r[col] + 2) {
          int d = abs(row - humans[i].pos.y);
          if (d <= rmin1) {
            rmin2 = rmin1;
            rmin1 = d;
          } else if (d < rmin2) {
            rmin2 = d;
          }
        }
      }
      int dist = col_type[col] == 1   ? rmin1
                 : col_type[col] == 2 ? lmin1
                                      : max(lmin1, rmin1);
      ret += max(1.0 * pet_depth[col][row], 0.0) / (bias1 + dist);
      if (col_type[col] == 1) {
        ret += pet_depth[col][row] / (bias2 + rmin2) / cut;
      } else if (col_type[col] == 2) {
        ret += pet_depth[col][row] / (bias2 + lmin2) / cut;
      } else {
        ret += pet_depth[col][row] / (bias2 + (lmin2 + rmin2) / 2.0) / cut;
      }
    }
  }
  return ret;
}

queue<Order> qs[10];
string ans;

void build_row(int i, int col, int row, int ori, int hole = -1) {
  if (ori == 0) {
    for (int x = row_l[col]; x <= row_r[col]; ++x) {
      qs[i].push(Order(Point(row, x), 0));
      if (x == row_r[col]) {
        if (hole == 0) {
          qs[i].push(Order(Point(row + 1, x), 1));
          qs[i].push(Order(Point(row - 2, x), 0));
          qs[i].push(Order(Point(row - 1, x), 1));
        } else if (hole == 1) {
          qs[i].push(Order(Point(row - 1, x), 1));
          qs[i].push(Order(Point(row + 2, x), 0));
          qs[i].push(Order(Point(row + 1, x), 1));
        } else
          qs[i].push(Order(Point(row - 1, x), Point(row + 1, x), 1));
      } else {
        qs[i].push(Order(Point(row - 1, x), Point(row + 1, x), 1));
      }
    }
  } else {
    for (int x = row_r[col]; x >= row_l[col]; --x) {
      qs[i].push(Order(Point(row, x), 0));
      if (x == row_l[col]) {
        if (hole == 0) {
          qs[i].push(Order(Point(row + 1, x), 1));
          qs[i].push(Order(Point(row - 2, x), 0));
          qs[i].push(Order(Point(row - 1, x), 1));
        } else if (hole == 1) {
          qs[i].push(Order(Point(row - 1, x), 1));
          qs[i].push(Order(Point(row + 2, x), 0));
          qs[i].push(Order(Point(row + 1, x), 1));
        } else
          qs[i].push(Order(Point(row - 1, x), Point(row + 1, x), 1));
      } else {
        qs[i].push(Order(Point(row - 1, x), Point(row + 1, x), 1));
      }
    }
  }
}

void block_single() {
  REP(i, M) {
    for (int dir : {2, 3}) {
      Point p = humans[i].pos.next(dir);
      Point s = p.next(dir);
      if (p.valid() && s.valid() && !block[s.y][s.x] && can_place(p)) {
        block[p.y][p.x] = 1;
        int dists[30][30];
        REP(i, 30) REP(j, 30) dists[i][j] = inf;
        queue<Point> q;
        dists[s.y][s.x] = 0;
        q.push(s);
        int cnt = 0, pet_cnt = 0;
        while (!q.empty()) {
          Point u = q.front();
          q.pop();
          cnt++;
          if (petn[u.y][u.x] > 0) {
            pet_cnt++;
          }
          if (humann[u.y][u.x] > 0) {
            pet_cnt = 0;
            break;
          }
          REP(d, 4) {
            Point v = u.next(d);
            if (v.valid() && block[v.y][v.x] == 0 && dists[v.y][v.x] == inf) {
              dists[v.y][v.x] = dists[u.y][u.x] + 1;
              q.push(v);
            }
          }
        }
        block[p.y][p.x] = 0;
        if (pet_cnt && cnt < 16) {
          ans[i] = humans[i].do_order(Order(p, 1));
        }
      }
    }
  }
}

void block_double(int col) {
  for (int row = 0; row <= 28; row += 2) {
    int l = -1, r = -1;
    REP(i, M) {
      if (ans[i] == '.' && qs[i].empty() && humans[i].pos.y == row &&
          humans[i].pos.x == row_l[col] - 2) {
        l = i;
      } else if (ans[i] == '.' && qs[i].empty() && humans[i].pos.y == row &&
                 humans[i].pos.x == row_r[col] + 2) {
        r = i;
      }
    }
    if (l >= 0 && r >= 0) {
      Point p1 = humans[l].pos.next(3), p2 = humans[r].pos.next(2);
      Point s = p1.next(3);
      if (p1.valid() && p2.valid() && s.valid() && !block[s.y][s.x] &&
          can_place(p1) && can_place(p2)) {
        block[p1.y][p1.x] = block[p2.y][p2.x] = 1;
        int dists[30][30];
        REP(i, 30) REP(j, 30) dists[i][j] = inf;
        queue<Point> q;
        dists[s.y][s.x] = 0;
        q.push(s);
        int cnt = 0, pet_cnt = 0;
        while (!q.empty()) {
          Point u = q.front();
          q.pop();
          cnt++;
          if (petn[u.y][u.x] > 0) {
            pet_cnt++;
          }
          if (humann[u.y][u.x] > 0) {
            pet_cnt = 0;
            break;
          }
          REP(d, 4) {
            Point v = u.next(d);
            if (v.valid() && block[v.y][v.x] == 0 && dists[v.y][v.x] == inf) {
              dists[v.y][v.x] = dists[u.y][u.x] + 1;
              q.push(v);
            }
          }
        }
        block[p1.y][p1.x] = block[p2.y][p2.x] = 0;
        if (pet_cnt && cnt < 16) {
          ans[l] = humans[l].do_order(Order(p1, 1));
          ans[r] = humans[r].do_order(Order(p2, 1));
        }
      }
    }
  }
}

void solve() {
  REP(i, C) {
    if (i == 0 && row_l[i] == row_c1[i])
      col_type[i] = 1;
    else if (i == C - 1 && row_r[i] == row_c2[i])
      col_type[i] = 2;
    else
      col_type[i] = 0;
  }
  vector<int> human_conv(M), ids(M);
  iota(ALL(ids), 0);
  int best = inf;
  do {
    int mx = 0;
    REP(i, K) { mx = max(mx, humans[ids[i]].pos.dist(qs[i].front().pos[0])); }
    if (mx < best) {
      human_conv = ids;
      best = mx;
    }
  } while (next_permutation(ALL(ids)));
  vector<Human> new_humans(M);
  REP(i, M) new_humans[i] = humans[human_conv[i]];
  REP(i, M) humans[i] = new_humans[i];
  REP(t, 300) {
    if (0 && t % 1 == 0)
      debug(t, timer.get_time());
    ans = string(M, '.');
    vector<int> moved(N);
    REP(i, N) if (pets[i].t >= 4 && pets[i].last != -1) {
      pets[i].pos = pets[i].pos.next(pets[i].last);
      if (pets[i].pos.valid() && !block[pets[i].pos.y][pets[i].pos.x]) {
        moved[i] = 1;
      } else {
        pets[i].pos = pets[i].pos.next(pets[i].last <= 1 ? 1 - pets[i].last
                                                         : 5 - pets[i].last);
      }
    }
    set_pet_depth();
    REP(i, N) if (moved[i]) {
      pets[i].pos = pets[i].pos.next(pets[i].last <= 1 ? 1 - pets[i].last
                                                       : 5 - pets[i].last);
    }
    block_single();
    REP(col, C)
    if (col_type[col] == 0)
      block_double(col);
    REP(i, M)
    while (!qs[i].empty() && humans[i].achieved(qs[i].front()))
      qs[i].pop();
    vector<int> free_humans;
    set<int> exist;
    REP(i, M) if (ans[i] == '.' && qs[i].empty()) {
      int stay = 0;
      REP(j, C) {
        if (humans[i].pos.y % 2 == 0) {
          if (humans[i].pos.x == row_r[j] + 2 &&
              pet_depth[j][humans[i].pos.y] >= geta + canmove) {
            stay = 1;
          }
          if (humans[i].pos.x == row_l[j] - 2 &&
              pet_depth[j][humans[i].pos.y] >= geta + canmove) {
            stay = 1;
          }
        }
      }
      if (!stay || exist.count(humans[i].pos.id())) {
        free_humans.push_back(i);
      }
      exist.insert(humans[i].pos.id());
    }
    int X = free_humans.size();
    double bestv = -1;
    int best_move = -1;
    REP(b, 1 << X) {
      int out = 0;
      REP(i, X) {
        humans[free_humans[i]].move(bit(b, i));
        if (!humans[free_humans[i]].pos.valid())
          out = 1;
      }
      if (!out) {
        double score = eval_state();
        if (bestv < score) {
          bestv = score;
          best_move = b;
        }
      }
      REP(i, X) humans[free_humans[i]].move(1 - bit(b, i));
    }
    if (bestv >= 0) {
      REP(i, X)
      ans[free_humans[i]] = humans[free_humans[i]].move(bit(best_move, i));
    }

    REP(i, M)
    if (ans[i] == '.' && !qs[i].empty() && qs[i].front().type == 1)
      ans[i] = humans[i].do_order(qs[i].front());
    REP(i, M)
    if (ans[i] == '.' && !qs[i].empty() && qs[i].front().type == 0)
      ans[i] = humans[i].do_order(qs[i].front());
    REP(col, C) {
      for (int row = 0; row <= 28; row += 2)
        if (!built[col][row]) {
          built[col][row] = 1;
          for (int x = row_l[col]; x <= row_r[col]; ++x) {
            Point p1(row - 1, x), p2(row + 1, x);
            if (p1.valid() && !block[p1.y][p1.x]) {
              built[col][row] = 0;
              break;
            }
            if (p2.valid() && row != 28 && !block[p2.y][p2.x]) {
              built[col][row] = 0;
              break;
            }
          }
        }
    }
    string ans2(M, '.');
    REP(i, M) ans2[human_conv[i]] = ans[i];
    auto acts = judge::interact(ans2);
    REP(i, N) {
      petn[pets[i].pos.y][pets[i].pos.x]--;
      pets[i].last = -1;
      for (char c : acts[i]) {
        if (pets[i].last == -1) {
          REP(j, 4) if (moves[j] == c) pets[i].last = j;
        }
        pets[i].move(c);
      }
      petn[pets[i].pos.y][pets[i].pos.x]++;
    }
  }
}
} // namespace solver_cols

namespace solver_C3 {
int row_l[3] = {0, 11, 22}, row_r[3] = {7, 18, 29};
int row_c1[3] = {0, 14, 29}, row_c2[3] = {0, 15, 29};

void solve() {
  solver_cols::C = 3;
  solver_cols::K = 5;
  REP(i, 3) {
    solver_cols::row_l[i] = row_l[i];
    solver_cols::row_r[i] = row_r[i];
    solver_cols::row_c1[i] = row_c1[i];
    solver_cols::row_c2[i] = row_c2[i];
  }
  using solver_cols::build_row;
  using solver_cols::qs;
  REP(i, 5) {
    if (i == 0) {
      build_row(i, 0, 2, 0);
      build_row(i, 1, 2, 0);
      build_row(i, 2, 2, 0, 1);
      build_row(i, 2, 6, 1);
      qs[i].push(Order(Point(6, row_r[1] + 2), 0));

    } else if (i == 1) {
      build_row(i, 2, 10, 1);
      build_row(i, 1, 10, 1);
      build_row(i, 0, 10, 1, 0);
      build_row(i, 0, 6, 0);
      build_row(i, 1, 6, 0);
      qs[i].push(Order(Point(6, row_r[1] + 2), 0));

    } else if (i == 2) {
      build_row(i, 1, 18, 0);
      build_row(i, 2, 18, 0, 0);
      build_row(i, 2, 14, 1);
      build_row(i, 1, 14, 1);
      qs[i].push(Order(Point(14, row_r[0] + 2), 0));

    } else if (i == 3) {
      build_row(i, 2, 22, 1);
      build_row(i, 1, 22, 1);
      build_row(i, 0, 18, 1, 0);
      build_row(i, 0, 14, 0);
      qs[i].push(Order(Point(14, row_r[0] + 2), 0));

    } else if (i == 4) {
      build_row(i, 2, 26, 1);
      qs[i].push(Order(Point(29, row_l[2] - 1), 0));
      qs[i].push(Order(Point(29, row_l[2]), 1));
      qs[i].push(Order(Point(29, row_r[1] + 1), 0));
      qs[i].push(Order(Point(29, row_r[1]), 1));
      build_row(i, 1, 26, 1);
      qs[i].push(Order(Point(29, row_l[1] - 1), 0));
      qs[i].push(Order(Point(29, row_l[1]), 1));
      qs[i].push(Order(Point(29, row_r[0] + 1), 0));
      qs[i].push(Order(Point(29, row_r[0]), 1));
      build_row(i, 0, 26, 1, 0);
      build_row(i, 0, 22, 0);
      qs[i].push(Order(Point(22, row_r[0] + 2), 0));
    }
  }
  FOR(i, 5, M)
  qs[i].push(Order(
      Point(humans[i].pos.y, i % 2 == 0 ? row_r[1] + 2 : row_r[0] + 2), 0));
  solver_cols::solve();
}
} // namespace solver_C3

namespace solver_C4 {
int row_l[4] = {0, 8, 16, 24}, row_r[4] = {4, 12, 20, 29};
int row_c1[4] = {0, 10, 18, 29}, row_c2[4] = {0, 10, 18, 29};

void solve() {
  using solver_cols::build_row;
  using solver_cols::qs;
  solver_cols::C = 4;
  REP(i, 4) {
    solver_cols::row_l[i] = row_l[i];
    solver_cols::row_r[i] = row_r[i];
    solver_cols::row_c1[i] = row_c1[i];
    solver_cols::row_c2[i] = row_c2[i];
  }
  if (M >= 6) {
    solver_cols::K = 6;

    REP(i, 6) {
      if (i == 0) {
        build_row(i, 0, 2, 0);
        build_row(i, 1, 2, 0);
        build_row(i, 2, 2, 0);
        build_row(i, 3, 2, 0, 1);
        build_row(i, 3, 6, 1);
        qs[i].push(Order(Point(6, row_r[2] + 2), 0));
      } else if (i == 1) {
        build_row(i, 1, 10, 1);
        build_row(i, 0, 10, 1, 0);
        build_row(i, 0, 6, 0);
        build_row(i, 1, 6, 0);
        build_row(i, 2, 6, 0);

        qs[i].push(Order(Point(6, row_r[2] + 2), 0));

      } else if (i == 2) {
        build_row(i, 2, 14, 0);
        build_row(i, 3, 14, 0, 0);
        build_row(i, 3, 10, 1);
        build_row(i, 2, 10, 1);
        build_row(i, 1, 10, 1);
        qs[i].push(Order(Point(10, row_r[0] + 2), 0));
      } else if (i == 3) {
        build_row(i, 1, 18, 1);
        build_row(i, 0, 18, 1, 0);
        build_row(i, 0, 14, 0);
        build_row(i, 1, 14, 0);
        qs[i].push(Order(Point(14, row_r[1] + 2), 0));
      } else if (i == 4) {
        build_row(i, 1, 22, 0);
        build_row(i, 2, 22, 0);
        build_row(i, 3, 22, 0, 0);
        build_row(i, 3, 18, 1);
        build_row(i, 2, 18, 1);
        qs[i].push(Order(Point(18, row_r[1] + 2), 0));
      } else if (i == 5) {
        build_row(i, 3, 26, 1);
        qs[i].push(Order(Point(29, row_l[3] - 1), 0));
        qs[i].push(Order(Point(29, row_l[3]), 1));
        qs[i].push(Order(Point(29, row_r[2] + 1), 0));
        qs[i].push(Order(Point(29, row_r[2]), 1));
        build_row(i, 2, 26, 1);
        qs[i].push(Order(Point(29, row_l[2] - 1), 0));
        qs[i].push(Order(Point(29, row_l[2]), 1));
        qs[i].push(Order(Point(29, row_r[1] + 1), 0));
        qs[i].push(Order(Point(29, row_r[1]), 1));
        build_row(i, 1, 26, 1);
        qs[i].push(Order(Point(29, row_l[1] - 1), 0));
        qs[i].push(Order(Point(29, row_l[1]), 1));
        qs[i].push(Order(Point(29, row_r[0] + 1), 0));
        qs[i].push(Order(Point(29, row_r[0]), 1));
        build_row(i, 0, 26, 1, 0);
        build_row(i, 0, 22, 0);
        qs[i].push(Order(Point(22, row_r[0] + 2), 0));
      }
    }
    FOR(i, 6, M)
    qs[i].push(Order(Point(humans[i].pos.y, row_r[i % 3] + 2), 0));
  } else {
    solver_cols::K = 5;

    REP(i, 6) {
      if (i == 0) {
        build_row(i, 0, 2, 0);
        build_row(i, 1, 2, 0);
        build_row(i, 2, 2, 0);
        build_row(i, 3, 2, 0, 1);
        build_row(i, 3, 6, 1);
        qs[i].push(Order(Point(6, row_r[2] + 2), 0));
      } else if (i == 1) {
        build_row(i, 2, 10, 1);
        build_row(i, 1, 10, 1);
        build_row(i, 0, 10, 1, 0);
        build_row(i, 0, 6, 0);
        build_row(i, 1, 6, 0);
        build_row(i, 2, 6, 0);

        qs[i].push(Order(Point(6, row_r[2] + 2), 0));

      } else if (i == 2) {
        build_row(i, 3, 10, 0, 1);
        build_row(i, 3, 14, 1);
        build_row(i, 2, 14, 1);
        build_row(i, 1, 14, 1);
        build_row(i, 0, 14, 1, 1);
        build_row(i, 0, 18, 0);
        qs[i].push(Order(Point(18, row_r[0] + 2), 0));
      } else if (i == 3) {
        build_row(i, 1, 18, 0);
        build_row(i, 2, 18, 0);
        build_row(i, 3, 18, 0, 1);
        build_row(i, 3, 22, 1);
        build_row(i, 2, 22, 1);
        qs[i].push(Order(Point(22, row_r[1] + 2), 0));
      } else if (i == 4) {
        build_row(i, 3, 26, 1);
        qs[i].push(Order(Point(29, row_l[3] - 1), 0));
        qs[i].push(Order(Point(29, row_l[3]), 1));
        qs[i].push(Order(Point(29, row_r[2] + 1), 0));
        qs[i].push(Order(Point(29, row_r[2]), 1));
        build_row(i, 2, 26, 1);
        qs[i].push(Order(Point(29, row_l[2] - 1), 0));
        qs[i].push(Order(Point(29, row_l[2]), 1));
        qs[i].push(Order(Point(29, row_r[1] + 1), 0));
        qs[i].push(Order(Point(29, row_r[1]), 1));
        build_row(i, 1, 26, 1);
        qs[i].push(Order(Point(29, row_l[1] - 1), 0));
        qs[i].push(Order(Point(29, row_l[1]), 1));
        qs[i].push(Order(Point(29, row_r[0] + 1), 0));
        qs[i].push(Order(Point(29, row_r[0]), 1));
        build_row(i, 0, 26, 1, 0);
        build_row(i, 0, 22, 0);
        build_row(i, 1, 22, 0);
        qs[i].push(Order(Point(22, row_r[1] + 2), 0));
      }
    }
    FOR(i, 5, M)
    qs[i].push(Order(Point(humans[i].pos.y, row_r[i % 3] + 2), 0));
  }
  solver_cols::solve();
}
} // namespace solver_C4

namespace solver_C4M {
int row_l[4] = {2, 9, 16, 23}, row_r[4] = {5, 12, 19, 26};
int row_c1[4] = {3, 10, 17, 24}, row_c2[4] = {4, 11, 18, 25};

void solve() {
  using solver_cols::build_row;
  using solver_cols::qs;
  solver_cols::C = 4;
  REP(i, 4) {
    solver_cols::row_l[i] = row_l[i];
    solver_cols::row_r[i] = row_r[i];
    solver_cols::row_c1[i] = row_c1[i];
    solver_cols::row_c2[i] = row_c2[i];
  }
  if (M >= 9) {
    solver_cols::K = 9;

    REP(i, 9) {
      if (i == 0) {
        build_row(i, 0, 2, 0);
        build_row(i, 1, 2, 0);
        build_row(i, 2, 2, 0);
        qs[i].push(Order(Point(2, row_r[2] + 2), 0));
      } else if (i == 1) {
        build_row(i, 3, 2, 0);
        build_row(i, 3, 6, 1);
        build_row(i, 2, 6, 1);
        qs[i].push(Order(Point(6, row_r[1] + 2), 0));
      } else if (i == 2) {
        build_row(i, 1, 6, 1);
        build_row(i, 0, 6, 1);
        build_row(i, 0, 10, 0);
        qs[i].push(Order(Point(10, row_r[0] + 2), 0));
      } else if (i == 3) {
        build_row(i, 0, 10, 0);
        build_row(i, 1, 10, 0);
        build_row(i, 2, 10, 0);
        build_row(i, 3, 10, 0);

        qs[i].push(Order(Point(10, row_r[3] + 2), 0));
      } else if (i == 4) {
        build_row(i, 3, 14, 1);
        build_row(i, 2, 14, 1);
        build_row(i, 1, 14, 1);
        build_row(i, 0, 14, 1);
        qs[i].push(Order(Point(14, row_l[0] - 2), 0));
      } else if (i == 5) {
        build_row(i, 2, 18, 1);
        build_row(i, 1, 18, 1);
        build_row(i, 0, 18, 1);
        qs[i].push(Order(Point(18, row_l[0] - 2), 0));
      } else if (i == 6) {
        build_row(i, 2, 22, 0);
        build_row(i, 3, 22, 0);
        build_row(i, 3, 18, 1);
        qs[i].push(Order(Point(18, row_r[2] + 2), 0));
      } else if (i == 7) {
        qs[i].push(Order(Point(29, row_r[0] + 1), 0));
        qs[i].push(Order(Point(29, row_r[0]), 1));
        build_row(i, 0, 26, 1);
        qs[i].push(Order(Point(29, row_l[0] - 1), 0));
        qs[i].push(Order(Point(29, row_l[0]), 1));
        build_row(i, 0, 22, 0);
        build_row(i, 1, 22, 0);
        qs[i].push(Order(Point(22, row_r[1] + 2), 0));
      } else if (i == 8) {
        qs[i].push(Order(Point(29, row_r[3] + 1), 0));
        qs[i].push(Order(Point(29, row_r[3]), 1));
        build_row(i, 3, 26, 1);
        qs[i].push(Order(Point(29, row_l[3] - 1), 0));
        qs[i].push(Order(Point(29, row_l[3]), 1));
        qs[i].push(Order(Point(29, row_r[2] + 1), 0));
        qs[i].push(Order(Point(29, row_r[2]), 1));
        build_row(i, 2, 26, 1);
        qs[i].push(Order(Point(29, row_l[2] - 1), 0));
        qs[i].push(Order(Point(29, row_l[2]), 1));
        qs[i].push(Order(Point(29, row_r[1] + 1), 0));
        qs[i].push(Order(Point(29, row_r[1]), 1));
        build_row(i, 1, 26, 1);
        qs[i].push(Order(Point(29, row_l[1] - 1), 0));
        qs[i].push(Order(Point(29, row_l[1]), 1));
        qs[i].push(Order(Point(29, row_r[0] + 2), 0));
      }
    }
    FOR(i, 9, M)
    qs[i].push(Order(Point(humans[i].pos.y, row_r[i % 5 - 1] + 2), 0));
  }
  solver_cols::solve();
}
} // namespace solver_C4M

namespace solver_C5 {
int row_l[5] = {0, 6, 13, 20, 27}, row_r[5] = {2, 9, 16, 23, 29};
int row_c1[5] = {0, 7, 14, 21, 29}, row_c2[5] = {0, 8, 15, 22, 29};

void solve() {
  using solver_cols::build_row;
  using solver_cols::qs;
  solver_cols::C = 5;
  REP(i, 5) {
    solver_cols::row_l[i] = row_l[i];
    solver_cols::row_r[i] = row_r[i];
    solver_cols::row_c1[i] = row_c1[i];
    solver_cols::row_c2[i] = row_c2[i];
  }
  if (M >= 8) {
    solver_cols::K = 8;

    REP(i, 8) {
      if (i == 0) {
        build_row(i, 0, 2, 0);
        build_row(i, 1, 2, 0);
        build_row(i, 2, 2, 0);
        build_row(i, 3, 2, 0);
        qs[i].push(Order(Point(2, row_r[3] + 2), 0));
      } else if (i == 1) {
        build_row(i, 1, 6, 0);
        build_row(i, 2, 6, 0);
        build_row(i, 3, 6, 0);
        build_row(i, 4, 6, 0, 0);
        build_row(i, 4, 2, 1);

        qs[i].push(Order(Point(2, row_r[3] + 2), 0));

      } else if (i == 2) {
        build_row(i, 2, 10, 1);
        build_row(i, 1, 10, 1);
        build_row(i, 0, 10, 1, 0);
        build_row(i, 0, 6, 0);
        qs[i].push(Order(Point(6, row_r[0] + 2), 0));
      } else if (i == 3) {
        build_row(i, 2, 14, 0);
        build_row(i, 3, 14, 0);
        build_row(i, 4, 14, 0, 0);
        build_row(i, 4, 10, 1);
        build_row(i, 3, 10, 1);
        qs[i].push(Order(Point(10, row_r[2] + 2), 0));
      } else if (i == 4) {
        build_row(i, 1, 18, 1);
        build_row(i, 0, 18, 1, 0);
        build_row(i, 0, 14, 0);
        build_row(i, 1, 14, 0);
        qs[i].push(Order(Point(14, row_r[1] + 2), 0));
      } else if (i == 5) {
        build_row(i, 3, 22, 0);
        build_row(i, 4, 22, 0, 0);
        build_row(i, 4, 18, 1);
        build_row(i, 3, 18, 1);
        build_row(i, 2, 18, 1);
        qs[i].push(Order(Point(18, row_r[1] + 2), 0));
      } else if (i == 6) {
        qs[i].push(Order(Point(28, row_r[0]), 0));
        qs[i].push(Order(Point(29, row_r[0]), 1));
        build_row(i, 0, 26, 1, 0);
        build_row(i, 0, 22, 0);
        build_row(i, 1, 22, 0);
        build_row(i, 2, 22, 0);
        qs[i].push(Order(Point(22, row_r[2] + 2), 0));
      } else if (i == 7) {
        build_row(i, 4, 26, 1);
        qs[i].push(Order(Point(29, row_l[4] - 1), 0));
        qs[i].push(Order(Point(29, row_l[4]), 1));
        qs[i].push(Order(Point(29, row_r[3] + 1), 0));
        qs[i].push(Order(Point(29, row_r[3]), 1));
        build_row(i, 3, 26, 1);
        qs[i].push(Order(Point(29, row_l[3] - 1), 0));
        qs[i].push(Order(Point(29, row_l[3]), 1));
        qs[i].push(Order(Point(29, row_r[2] + 1), 0));
        qs[i].push(Order(Point(29, row_r[2]), 1));
        build_row(i, 2, 26, 1);
        qs[i].push(Order(Point(29, row_l[2] - 1), 0));
        qs[i].push(Order(Point(29, row_l[2]), 1));
        qs[i].push(Order(Point(29, row_r[1] + 1), 0));
        qs[i].push(Order(Point(29, row_r[1]), 1));
        build_row(i, 1, 26, 1);
        qs[i].push(Order(Point(29, row_l[1] - 1), 0));
        qs[i].push(Order(Point(29, row_l[1]), 1));
        qs[i].push(Order(Point(26, row_r[0] + 2), 0));
      }
    }
    FOR(i, 8, M)
    qs[i].push(Order(Point(humans[i].pos.y, row_r[i % 4] + 2), 0));
  } else if (M == 7) {
    solver_cols::K = 7;

    REP(i, 7) {
      if (i == 0) {
        build_row(i, 4, 2, 1);
        build_row(i, 3, 2, 1);
        build_row(i, 2, 2, 1);
        build_row(i, 1, 2, 1);
        build_row(i, 0, 2, 1);
        qs[i].push(Order(Point(2, row_r[0] + 2), 0));
      } else if (i == 1) {
        build_row(i, 0, 6, 0);
        build_row(i, 1, 6, 0);
        build_row(i, 2, 6, 0);
        build_row(i, 3, 6, 0);
        build_row(i, 4, 6, 0);
        qs[i].push(Order(Point(6, row_r[3] + 2), 0));

      } else if (i == 2) {
        build_row(i, 4, 10, 1);
        build_row(i, 3, 10, 1);
        build_row(i, 2, 10, 1);
        build_row(i, 1, 10, 1);
        build_row(i, 0, 10, 1);
        qs[i].push(Order(Point(10, row_r[0] + 2), 0));
      } else if (i == 3) {
        build_row(i, 0, 14, 0);
        build_row(i, 1, 14, 0);
        build_row(i, 2, 14, 0);
        build_row(i, 3, 14, 0);
        build_row(i, 4, 14, 0);
        qs[i].push(Order(Point(14, row_r[2] + 2), 0));
      } else if (i == 4) {
        build_row(i, 4, 18, 1);
        build_row(i, 3, 18, 1);
        build_row(i, 2, 18, 1);
        build_row(i, 1, 18, 1);
        build_row(i, 0, 18, 1);
        qs[i].push(Order(Point(18, row_r[1] + 2), 0));
      } else if (i == 5) {
        build_row(i, 0, 22, 0);
        build_row(i, 1, 22, 0);
        build_row(i, 2, 22, 0);
        build_row(i, 3, 22, 0);
        build_row(i, 4, 22, 0);
        qs[i].push(Order(Point(22, row_r[2] + 2), 0));
      } else if (i == 6) {
        build_row(i, 4, 26, 1);
        qs[i].push(Order(Point(29, row_l[4] - 1), 0));
        qs[i].push(Order(Point(29, row_l[4]), 1));
        qs[i].push(Order(Point(29, row_r[3] + 1), 0));
        qs[i].push(Order(Point(29, row_r[3]), 1));
        build_row(i, 3, 26, 1);
        qs[i].push(Order(Point(29, row_l[3] - 1), 0));
        qs[i].push(Order(Point(29, row_l[3]), 1));
        qs[i].push(Order(Point(29, row_r[2] + 1), 0));
        qs[i].push(Order(Point(29, row_r[2]), 1));
        build_row(i, 2, 26, 1);
        qs[i].push(Order(Point(29, row_l[2] - 1), 0));
        qs[i].push(Order(Point(29, row_l[2]), 1));
        qs[i].push(Order(Point(29, row_r[1] + 1), 0));
        qs[i].push(Order(Point(29, row_r[1]), 1));
        build_row(i, 1, 26, 1);
        qs[i].push(Order(Point(29, row_l[1] - 1), 0));
        qs[i].push(Order(Point(29, row_l[1]), 1));
        qs[i].push(Order(Point(29, row_r[0] + 1), 0));
        qs[i].push(Order(Point(29, row_r[0]), 1));
        build_row(i, 0, 26, 1);
        qs[i].push(Order(Point(26, row_r[1] + 2), 0));
      }
    }
    FOR(i, 7, M)
    qs[i].push(Order(Point(humans[i].pos.y, row_r[i % 4] + 2), 0));
  } else if (M == 6) {
    solver_cols::K = 6;

    REP(i, 6) {
      if (i == 0) {
        build_row(i, 0, 2, 0);
        build_row(i, 1, 2, 0);
        build_row(i, 2, 2, 0);
        build_row(i, 3, 2, 0);
        build_row(i, 4, 2, 0, 1);
        build_row(i, 4, 6, 1);
        qs[i].push(Order(Point(6, row_r[3] + 2), 0));
      } else if (i == 1) {
        build_row(i, 3, 6, 1);
        build_row(i, 2, 6, 1);
        build_row(i, 1, 6, 1);
        build_row(i, 0, 6, 1, 1);
        build_row(i, 0, 10, 0);
        build_row(i, 1, 10, 0);
        qs[i].push(Order(Point(10, row_r[1] + 2), 0));
      } else if (i == 2) {
        build_row(i, 3, 14, 0);
        build_row(i, 4, 14, 0, 0);
        build_row(i, 4, 10, 1);
        build_row(i, 3, 10, 1);
        build_row(i, 2, 10, 1);
        qs[i].push(Order(Point(10, row_r[1] + 2), 0));
      } else if (i == 3) {
        build_row(i, 2, 18, 1);
        build_row(i, 1, 18, 1);
        build_row(i, 0, 18, 1, 0);
        build_row(i, 0, 14, 0);
        build_row(i, 1, 14, 0);
        build_row(i, 2, 14, 0);

        qs[i].push(Order(Point(14, row_r[2] + 2), 0));
      } else if (i == 4) {
        build_row(i, 1, 22, 0);
        build_row(i, 2, 22, 0);
        build_row(i, 3, 22, 0);
        build_row(i, 4, 22, 0, 0);
        build_row(i, 4, 18, 1);
        build_row(i, 3, 18, 1);
        qs[i].push(Order(Point(18, row_r[2] + 2), 0));
      } else if (i == 5) {
        build_row(i, 4, 26, 1);
        qs[i].push(Order(Point(29, row_l[4] - 1), 0));
        qs[i].push(Order(Point(29, row_l[4]), 1));
        qs[i].push(Order(Point(29, row_r[3] + 1), 0));
        qs[i].push(Order(Point(29, row_r[3]), 1));
        build_row(i, 3, 26, 1);
        qs[i].push(Order(Point(29, row_l[3] - 1), 0));
        qs[i].push(Order(Point(29, row_l[3]), 1));
        qs[i].push(Order(Point(29, row_r[2] + 1), 0));
        qs[i].push(Order(Point(29, row_r[2]), 1));
        build_row(i, 2, 26, 1);
        qs[i].push(Order(Point(29, row_l[2] - 1), 0));
        qs[i].push(Order(Point(29, row_l[2]), 1));
        qs[i].push(Order(Point(29, row_r[1] + 1), 0));
        qs[i].push(Order(Point(29, row_r[1]), 1));
        build_row(i, 1, 26, 1);
        qs[i].push(Order(Point(29, row_l[1] - 1), 0));
        qs[i].push(Order(Point(29, row_l[1]), 1));
        qs[i].push(Order(Point(29, row_r[0] + 1), 0));
        qs[i].push(Order(Point(29, row_r[0]), 1));
        build_row(i, 0, 26, 1, 0);
        build_row(i, 0, 22, 0);
        qs[i].push(Order(Point(22, row_r[0] + 2), 0));
      }
    }
    FOR(i, 6, M)
    qs[i].push(Order(Point(humans[i].pos.y, row_r[i % 4] + 2), 0));
  } else if (M == 5) {
    solver_cols::K = 5;

    REP(i, 5) {
      if (i == 0) {
        build_row(i, 1, 6, 1);
        build_row(i, 0, 6, 1, 0);
        build_row(i, 0, 2, 0);
        build_row(i, 1, 2, 0);
        build_row(i, 2, 2, 0);
        build_row(i, 3, 2, 0);
        build_row(i, 4, 2, 0, 1);
        build_row(i, 4, 6, 0);

        qs[i].push(Order(Point(6, row_r[3] + 2), 0));
      } else if (i == 1) {
        build_row(i, 3, 6, 1);
        build_row(i, 2, 6, 1);
        build_row(i, 2, 10, 0);
        build_row(i, 3, 10, 0);
        build_row(i, 4, 10, 0, 1);
        build_row(i, 4, 14, 1);
        build_row(i, 3, 14, 1);

        qs[i].push(Order(Point(14, row_r[2] + 2), 0));
      } else if (i == 2) {
        build_row(i, 2, 14, 1);
        build_row(i, 1, 14, 1, 1);
        build_row(i, 1, 18, 1);
        build_row(i, 0, 18, 1, 0);
        build_row(i, 0, 14, 0);
        build_row(i, 0, 10, 0);
        build_row(i, 1, 10, 0);
        qs[i].push(Order(Point(10, row_r[1] + 2), 0));
      } else if (i == 3) {
        build_row(i, 1, 22, 0);
        build_row(i, 2, 22, 0);
        build_row(i, 3, 22, 0);
        build_row(i, 4, 22, 0, 0);
        build_row(i, 4, 18, 1);
        build_row(i, 3, 18, 1);
        build_row(i, 2, 18, 1);

        qs[i].push(Order(Point(18, row_r[1] + 2), 0));
      } else if (i == 4) {
        build_row(i, 4, 26, 1, 1);
        qs[i].push(Order(Point(28, row_l[4]), 0));
        qs[i].push(Order(Point(29, row_l[4]), 1));
        qs[i].push(Order(Point(28, row_r[3]), 0));
        qs[i].push(Order(Point(29, row_r[3]), 1));
        build_row(i, 3, 26, 1, 1);
        qs[i].push(Order(Point(28, row_l[3]), 0));
        qs[i].push(Order(Point(29, row_l[3]), 1));
        qs[i].push(Order(Point(28, row_r[2]), 0));
        qs[i].push(Order(Point(29, row_r[2]), 1));
        build_row(i, 2, 26, 1, 1);
        qs[i].push(Order(Point(28, row_l[2]), 0));
        qs[i].push(Order(Point(29, row_l[2]), 1));
        qs[i].push(Order(Point(28, row_r[1]), 0));
        qs[i].push(Order(Point(29, row_r[1]), 1));
        build_row(i, 1, 26, 1, 1);
        qs[i].push(Order(Point(28, row_l[1]), 0));
        qs[i].push(Order(Point(29, row_l[1]), 1));
        qs[i].push(Order(Point(28, row_r[0]), 0));
        qs[i].push(Order(Point(29, row_r[0]), 1));
        build_row(i, 0, 26, 1, 0);
        build_row(i, 0, 22, 0);
        qs[i].push(Order(Point(22, row_r[0] + 2), 0));
      }
    }
    FOR(i, 5, M)
    qs[i].push(Order(Point(humans[i].pos.y, row_r[i % 4] + 2), 0));
  }
  solver_cols::solve();
}
} // namespace solver_C5

void solve() {
  if (M == 10 && N <= 16 || M == 9 && N <= 13)
    solver_C4M::solve();
  else if (M >= 7 || (M == 6 && N <= 17) || (M == 5 && N <= 13))
    solver_C5::solve();
  else if (M >= 6 || (M == 5 && N <= 17))
    solver_C4::solve();
  else
    solver_C3::solve();
  if (judge::local)
    debug(judge::get_score());
}

} // namespace solver_prev

namespace solver_cols {

int C = 5;
int K = 5;
int built[5][30], row_used[5][30];
int row_l[5] = {0, 6, 13, 20, 27}, row_r[5] = {2, 9, 16, 23, 29};
int row_c1[5] = {0, 7, 14, 21, 29}, row_c2[5] = {0, 8, 15, 22, 29};
int col_type[5];
int pet_depth[5][30];

int get_dist_closest_pet(int col, int row) {
  int dists[30][30];
  REP(i, 30) REP(j, 30) dists[i][j] = inf;
  queue<Point> q;
  dists[row][row_c1[col]] = dists[row][row_c2[col]] = 0;
  q.push(Point(row, row_c1[col]));
  q.push(Point(row, row_c2[col]));
  int cnt = 0;
  int mn = inf;
  while (!q.empty()) {
    Point u = q.front();
    q.pop();
    cnt++;
    if (petn[u.y][u.x] > 0) {
      chmin(mn, dists[u.y][u.x]);
    }
    REP(d, 4) {
      Point v = u.next(d);
      if (v.valid() && block[v.y][v.x] == 0 && dists[v.y][v.x] == inf) {
        dists[v.y][v.x] = dists[u.y][u.x] + 1;
        q.push(v);
      }
    }
  }
  return cnt < 17 ? inf : mn;
}

void set_pet_depth() {
  REP(i, C) REP(r, 30) pet_depth[i][r] = -inf;
  for (int col = 0; col < C; ++col) {
    for (int r = 0; r <= 28; r += 2) {
      if (!row_used[col][r] && built[col][r]) {
        int dist = get_dist_closest_pet(col, r);
        if (dist == inf) {
          row_used[col][r] = 1;
          pet_depth[col][r] = -inf;
        } else {
          if (col_type[col] == 1)
            pet_depth[col][r] = row_r[col] - row_l[col] - dist + geta;
          else if (col_type[col] == 2)
            pet_depth[col][r] = row_r[col] - row_l[col] - dist + geta;
          else
            pet_depth[col][r] = row_r[col] - row_c2[col] - dist + geta;

          if (pet_depth[col][r] >= geta + 1)
            pet_depth[col][r]++;
          chmax(pet_depth[col][r], 0);
        }
      } else {
        pet_depth[col][r] = -inf;
      }
    }
  }
}

double eval_state() {
  double ret = 0;

  REP(col, C) for (int row = 0; row <= 28; row += 2) {
    if (pet_depth[col][row] >= 0) {
      int lmin1 = inf, lmin2 = inf, rmin1 = inf, rmin2 = inf;

      REP(i, M) {
        if (col_type[col] != 1 && humans[i].pos.x == row_l[col] - 2) {
          int d = abs(row - humans[i].pos.y);
          if (d <= lmin1) {
            lmin2 = lmin1;
            lmin1 = d;
          } else if (d < lmin2) {
            lmin2 = d;
          }
        }
        if (col_type[col] != 2 && humans[i].pos.x == row_r[col] + 2) {
          int d = abs(row - humans[i].pos.y);
          if (d <= rmin1) {
            rmin2 = rmin1;
            rmin1 = d;
          } else if (d < rmin2) {
            rmin2 = d;
          }
        }
      }
      int dist = col_type[col] == 1   ? rmin1
                 : col_type[col] == 2 ? lmin1
                                      : max(lmin1, rmin1);
      ret += max(1.0 * pet_depth[col][row], 0.08) / (bias1 + dist);
      if (col_type[col] == 1) {
        ret += pet_depth[col][row] / (bias2 + rmin2) / cut;
      } else if (col_type[col] == 2) {
        ret += pet_depth[col][row] / (bias2 + lmin2) / cut;
      } else {
        ret += pet_depth[col][row] / (bias2 + (lmin2 + rmin2) / 2.0) / cut;
      }
    }
  }
  return ret;
}

queue<Order> qs[10];
string ans;

void build_row(int i, int col, int row, int ori, int hole = -1) {
  if (ori == 0) {
    if (row == 26) {
      qs[i].push(Order(Point(28, row_l[col]), 0));
      qs[i].push(Order(Point(29, row_l[col]), 1));

      for (int x = row_l[col]; x <= row_r[col]; ++x) {
        qs[i].push(Order(Point(row, x), 0));
        if (x == row_r[col] && col != C - 1) {
          qs[i].push(Order(Point(row - 1, x), 1));
          qs[i].push(Order(Point(row + 2, x), 0));
          qs[i].push(Order(Point(row + 1, x), Point(row + 3, x), 1));
        } else {
          qs[i].push(Order(Point(row - 1, x), Point(row + 1, x), 1));
        }
      }
    } else
      for (int x = row_l[col]; x <= row_r[col]; ++x) {
        qs[i].push(Order(Point(row, x), 0));
        if (x == row_r[col]) {
          if (hole == 0) {
            qs[i].push(Order(Point(row + 1, x), 1));
            qs[i].push(Order(Point(row - 2, x), 0));
            qs[i].push(Order(Point(row - 1, x), 1));
          } else if (hole == 1) {
            qs[i].push(Order(Point(row - 1, x), 1));
            qs[i].push(Order(Point(row + 2, x), 0));
            qs[i].push(Order(Point(row + 1, x), 1));
          } else
            qs[i].push(Order(Point(row - 1, x), Point(row + 1, x), 1));
        } else {
          qs[i].push(Order(Point(row - 1, x), Point(row + 1, x), 1));
        }
      }
  } else {
    if (row == 26) {
      qs[i].push(Order(Point(28, row_r[col]), 0));
      qs[i].push(Order(Point(29, row_r[col]), 1));
      for (int x = row_r[col]; x >= row_l[col]; --x) {
        qs[i].push(Order(Point(row, x), 0));
        if (x == row_l[col] && col != 0) {
          qs[i].push(Order(Point(row - 1, x), 1));
          qs[i].push(Order(Point(row + 2, x), 0));
          qs[i].push(Order(Point(row + 1, x), Point(row + 3, x), 1));
        } else {
          qs[i].push(Order(Point(row - 1, x), Point(row + 1, x), 1));
        }
      }
    } else
      for (int x = row_r[col]; x >= row_l[col]; --x) {
        qs[i].push(Order(Point(row, x), 0));
        if (x == row_l[col]) {
          if (hole == 0) {
            qs[i].push(Order(Point(row + 1, x), 1));
            qs[i].push(Order(Point(row - 2, x), 0));
            qs[i].push(Order(Point(row - 1, x), 1));
          } else if (hole == 1) {
            qs[i].push(Order(Point(row - 1, x), 1));
            qs[i].push(Order(Point(row + 2, x), 0));
            qs[i].push(Order(Point(row + 1, x), 1));
          } else
            qs[i].push(Order(Point(row - 1, x), Point(row + 1, x), 1));
        } else {
          qs[i].push(Order(Point(row - 1, x), Point(row + 1, x), 1));
        }
      }
  }
}

void block_single() {
  REP(i, M) {
    for (int dir : {2, 3}) {
      Point p = humans[i].pos.next(dir);
      Point s = p.next(dir);
      if (p.valid() && s.valid() && !block[s.y][s.x] && can_place(p)) {
        block[p.y][p.x] = 1;
        int dists[30][30];
        REP(i, 30) REP(j, 30) dists[i][j] = inf;
        queue<Point> q;
        dists[s.y][s.x] = 0;
        q.push(s);
        int cnt = 0, pet_cnt = 0;
        while (!q.empty()) {
          Point u = q.front();
          q.pop();
          cnt++;
          if (petn[u.y][u.x] > 0) {
            pet_cnt++;
          }
          if (humann[u.y][u.x] > 0) {
            pet_cnt = 0;
            break;
          }
          REP(d, 4) {
            Point v = u.next(d);
            if (v.valid() && block[v.y][v.x] == 0 && dists[v.y][v.x] == inf) {
              dists[v.y][v.x] = dists[u.y][u.x] + 1;
              q.push(v);
            }
          }
        }
        block[p.y][p.x] = 0;
        if (pet_cnt && cnt < 16) {
          ans[i] = humans[i].do_order(Order(p, 1));
        }
      }
    }
  }
}

void block_double(int col) {
  for (int row = 0; row <= 28; row += 2) {
    int l = -1, r = -1;
    REP(i, M) {
      if (ans[i] == '.' && qs[i].empty() && humans[i].pos.y == row &&
          humans[i].pos.x == row_l[col] - 2) {
        l = i;
      } else if (ans[i] == '.' && qs[i].empty() && humans[i].pos.y == row &&
                 humans[i].pos.x == row_r[col] + 2) {
        r = i;
      }
    }
    if (l >= 0 && r >= 0) {
      Point p1 = humans[l].pos.next(3), p2 = humans[r].pos.next(2);
      Point s = p1.next(3);
      if (p1.valid() && p2.valid() && s.valid() && !block[s.y][s.x] &&
          can_place(p1) && can_place(p2)) {
        block[p1.y][p1.x] = block[p2.y][p2.x] = 1;
        int dists[30][30];
        REP(i, 30) REP(j, 30) dists[i][j] = inf;
        queue<Point> q;
        dists[s.y][s.x] = 0;
        q.push(s);
        int cnt = 0, pet_cnt = 0;
        while (!q.empty()) {
          Point u = q.front();
          q.pop();
          cnt++;
          if (petn[u.y][u.x] > 0) {
            pet_cnt++;
          }
          if (humann[u.y][u.x] > 0) {
            pet_cnt = 0;
            break;
          }
          REP(d, 4) {
            Point v = u.next(d);
            if (v.valid() && block[v.y][v.x] == 0 && dists[v.y][v.x] == inf) {
              dists[v.y][v.x] = dists[u.y][u.x] + 1;
              q.push(v);
            }
          }
        }
        block[p1.y][p1.x] = block[p2.y][p2.x] = 0;
        if (pet_cnt && cnt < 16) {
          ans[l] = humans[l].do_order(Order(p1, 1));
          ans[r] = humans[r].do_order(Order(p2, 1));
        }
      }
    }
  }
}

void solve() {
  REP(i, C) {
    if (i == 0 && row_l[i] == row_c1[i])
      col_type[i] = 1;
    else if (i == C - 1 && row_r[i] == row_c2[i])
      col_type[i] = 2;
    else
      col_type[i] = 0;
  }
  vector<int> human_conv(M), ids(M);
  iota(ALL(ids), 0);
  int best = inf;
  do {
    int mx = 0;
    REP(i, K) { mx = max(mx, humans[ids[i]].pos.dist(qs[i].front().pos[0])); }
    if (mx < best) {
      human_conv = ids;
      best = mx;
    }
  } while (next_permutation(ALL(ids)));
  vector<Human> new_humans(M);
  REP(i, M) new_humans[i] = humans[human_conv[i]];
  REP(i, M) humans[i] = new_humans[i];
  vector<vector<int>> building(5, vector<int>(30));
  set<Pii> buildq;
  REP(t, 300) {
    if (0 && t % 1 == 0)
      debug(t, timer.get_time());
    ans = string(M, '.');
    vector<int> moved(N);
    REP(i, N) if (pets[i].t >= 4 && pets[i].last != -1) {
      pets[i].pos = pets[i].pos.next(pets[i].last);
      if (pets[i].pos.valid() && !block[pets[i].pos.y][pets[i].pos.x]) {
        moved[i] = 1;
      } else {
        pets[i].pos = pets[i].pos.next(pets[i].last <= 1 ? 1 - pets[i].last
                                                         : 5 - pets[i].last);
      }
    }
    set_pet_depth();
    REP(i, N) if (moved[i]) {
      pets[i].pos = pets[i].pos.next(pets[i].last <= 1 ? 1 - pets[i].last
                                                       : 5 - pets[i].last);
    }
    block_single();
    REP(col, C)
    if (col_type[col] == 0)
      block_double(col);

    int dist[30][30];
    REP(i, 30) REP(j, 30) dist[i][j] = inf;
    queue<Point> q;
    q.push(humans[0].pos);
    dist[humans[0].pos.y][humans[0].pos.x] = 0;
    while (!q.empty()) {
      auto u = q.front();
      q.pop();
      REP(dir, 4) {
        auto v = u.next(dir);
        if (v.valid() && dist[v.y][v.x] == inf && !block[v.y][v.x]) {
          dist[v.y][v.x] = dist[u.y][u.x] + 1;
          q.push(v);
        }
      }
    }
    int gdead = 1;
    REP(i, N)
    if (dist[pets[i].pos.y][pets[i].pos.x] != inf && pets[i].t >= 4)
      gdead = 0;
    if (gdead && 0) {
      REP(i, M) if (qs[i].empty()) {
        int col = 0;
        REP(j, C) if (row_r[j] + 2 == humans[i].pos.x) col = j;
        int ok = 0;
        for (int y = 0; y <= 28; y += 2)
          if (pet_depth[col][y] > 0 || pet_depth[col + 1][y] > 0)
            ok = 1;
        if (!ok) {
          // debug(t, "OUT", i);
          if (col == 0) {
            qs[i].push(Order(Point(humans[i].pos.y, row_r[col + 1] + 2), 0));
          } else if (col == C - 2) {
            qs[i].push(Order(Point(humans[i].pos.y, row_r[col - 1] + 2), 0));
          }
        }
      }
    }
    REP(i, M)
    while (!qs[i].empty() && humans[i].achieved(qs[i].front()))
      qs[i].pop();
    if (t >= 50 + M * 10) {
      int dist[30][30];
      REP(i, 30) REP(j, 30) dist[i][j] = inf;
      queue<Point> q;
      q.push(humans[0].pos);
      dist[humans[0].pos.y][humans[0].pos.x] = 0;
      while (!q.empty()) {
        auto u = q.front();
        q.pop();
        REP(dir, 4) {
          auto v = u.next(dir);
          if (v.valid() && dist[v.y][v.x] == inf && !block[v.y][v.x]) {
            dist[v.y][v.x] = dist[u.y][u.x] + 1;
            q.push(v);
          }
        }
      }
      REP(i, N) {
        auto &pet = pets[i];
        if (dist[pet.pos.y][pet.pos.x] != inf) {
          if (pet.pos.x <= 5) {
            for (int y = 2; y <= 26; y += 4) {
              if (abs(y - pet.pos.y) <= 3) {
                if (!building[0][y]) {
                  buildq.insert({0, y});
                }
              }
            }
          }
          if (pet.pos.x >= 24) {
            for (int y = 2; y <= 26; y += 4) {
              if (abs(y - pet.pos.y) <= 3) {
                if (!building[4][y]) {
                  buildq.insert({4, y});
                }
              }
            }
          }
        }
      }
      while (!buildq.empty()) {
        int upd = 0;
        for (auto p : buildq) {
          int ok = 0;
          REP(i, N) if (dist[pets[i].pos.y][pets[i].pos.x] != inf) {
            if (p.first == 0) {
              if (pets[i].pos.x <= 5 && abs(p.second - pets[i].pos.y) <= 3) {
                ok = 1;
                break;
              }
            } else {
              if (pets[i].pos.x >= 24 && abs(p.second - pets[i].pos.y) <= 3) {
                ok = 1;
                break;
              }
            }
          }
          if (!ok) {
            buildq.erase(p);
            upd = 1;
            break;
          }
        }
        if (!upd)
          break;
      }
      auto readys = [&](int x) {
        int ret = 0;
        REP(i, M) if (humans[i].pos.x == x && qs[i].empty()) ret++;
        return ret;
      };
      while (!buildq.empty()) {
        int upd = 0;
        for (auto p : buildq) {
          if (p.first == 0) {
            if (readys(4) > 1) {
              REP(i, M) if (humans[i].pos.x == 4 && qs[i].empty()) {
                build_row(i, 0, p.second, 0);
                qs[i].push(Order(Point(p.second, 4), 0));
                building[p.first][p.second] = 1;
                buildq.erase(p);
                upd = 1;
                break;
              }
              if (upd)
                break;
            }
          } else {
            if (readys(25) > 1) {
              REP(i, M) if (humans[i].pos.x == 25 && qs[i].empty()) {
                build_row(i, 4, p.second, 0);
                qs[i].push(Order(Point(p.second, 25), 0));

                building[p.first][p.second] = 1;
                buildq.erase(p);
                upd = 1;
                break;
              }
              if (upd)
                break;
            }
          }
        }
        if (!upd)
          break;
      }
    }
    vector<int> free_humans;
    set<int> exist;
    REP(i, M) if (ans[i] == '.' && qs[i].empty()) {
      int stay = 0;
      REP(j, C) {
        if (humans[i].pos.y % 2 == 0) {
          if (humans[i].pos.x == row_r[j] + 2 &&
              pet_depth[j][humans[i].pos.y] >= geta + canmove) {
            stay = 1;
          }
          if (humans[i].pos.x == row_l[j] - 2 &&
              pet_depth[j][humans[i].pos.y] >= geta + canmove) {
            stay = 1;
          }
        }
      }
      if (!stay || exist.count(humans[i].pos.id())) {
        free_humans.push_back(i);
      }
      exist.insert(humans[i].pos.id());
    }
    int X = free_humans.size();
    double bestv = -1;
    int best_move = -1;
    REP(b, 1 << X) {
      int out = 0;
      REP(i, X) {
        humans[free_humans[i]].move(bit(b, i));
        if (!humans[free_humans[i]].pos.valid())
          out = 1;
      }
      if (!out) {
        double score = eval_state();
        if (bestv < score) {
          bestv = score;
          best_move = b;
        }
      }
      REP(i, X) humans[free_humans[i]].move(1 - bit(b, i));
    }
    if (bestv >= 0) {
      REP(i, X)
      ans[free_humans[i]] = humans[free_humans[i]].move(bit(best_move, i));
    }

    REP(i, M)
    if (ans[i] == '.' && !qs[i].empty() && qs[i].front().type == 1)
      ans[i] = humans[i].do_order(qs[i].front());
    REP(i, M)
    if (ans[i] == '.' && !qs[i].empty() && qs[i].front().type == 0)
      ans[i] = humans[i].do_order(qs[i].front());
    REP(col, C) {
      for (int row = 0; row <= 28; row += 2)
        if (!built[col][row]) {
          built[col][row] = 1;
          for (int x = row_l[col]; x <= row_r[col]; ++x) {
            Point p1(row - 1, x), p2(row + 1, x);
            if (p1.valid() && !block[p1.y][p1.x]) {
              built[col][row] = 0;
              break;
            }
            if (p2.valid() && row != 28 && !block[p2.y][p2.x]) {
              built[col][row] = 0;
              break;
            }
          }
        }
    }
    string ans2(M, '.');
    REP(i, M) ans2[human_conv[i]] = ans[i];
    auto acts = judge::interact(ans2);
    REP(i, N) {
      petn[pets[i].pos.y][pets[i].pos.x]--;
      pets[i].last = -1;
      for (char c : acts[i]) {
        if (pets[i].last == -1) {
          REP(j, 4) if (moves[j] == c) pets[i].last = j;
        }
        pets[i].move(c);
      }
      petn[pets[i].pos.y][pets[i].pos.x]++;
    }
  }
}
} // namespace solver_cols

namespace solver_C3 {
int row_l[3] = {0, 11, 22}, row_r[3] = {7, 18, 29};
int row_c1[3] = {0, 14, 29}, row_c2[3] = {0, 15, 29};

void solve() {
  solver_cols::C = 3;
  solver_cols::K = 5;
  REP(i, 3) {
    solver_cols::row_l[i] = row_l[i];
    solver_cols::row_r[i] = row_r[i];
    solver_cols::row_c1[i] = row_c1[i];
    solver_cols::row_c2[i] = row_c2[i];
  }
  using solver_cols::build_row;
  using solver_cols::qs;
  REP(i, 5) {
    if (i == 0) {
      build_row(i, 0, 2, 0);
      build_row(i, 1, 2, 0);
      build_row(i, 2, 2, 0, 1);
      build_row(i, 2, 6, 1);
      qs[i].push(Order(Point(6, row_r[1] + 2), 0));

    } else if (i == 1) {
      build_row(i, 2, 10, 1);
      build_row(i, 1, 10, 1);
      build_row(i, 0, 10, 1, 0);
      build_row(i, 0, 6, 0);
      build_row(i, 1, 6, 0);
      qs[i].push(Order(Point(6, row_r[1] + 2), 0));

    } else if (i == 2) {
      build_row(i, 1, 18, 0);
      build_row(i, 2, 18, 0, 0);
      build_row(i, 2, 14, 1);
      build_row(i, 1, 14, 1);
      qs[i].push(Order(Point(14, row_r[0] + 2), 0));

    } else if (i == 3) {
      build_row(i, 2, 22, 1);
      build_row(i, 1, 22, 1);
      build_row(i, 0, 18, 1, 0);
      build_row(i, 0, 14, 0);
      qs[i].push(Order(Point(14, row_r[0] + 2), 0));

    } else if (i == 4) {
      build_row(i, 2, 26, 1);
      qs[i].push(Order(Point(29, row_l[2] - 1), 0));
      qs[i].push(Order(Point(29, row_l[2]), 1));
      qs[i].push(Order(Point(29, row_r[1] + 1), 0));
      qs[i].push(Order(Point(29, row_r[1]), 1));
      build_row(i, 1, 26, 1);
      qs[i].push(Order(Point(29, row_l[1] - 1), 0));
      qs[i].push(Order(Point(29, row_l[1]), 1));
      qs[i].push(Order(Point(29, row_r[0] + 1), 0));
      qs[i].push(Order(Point(29, row_r[0]), 1));
      build_row(i, 0, 26, 1, 0);
      build_row(i, 0, 22, 0);
      qs[i].push(Order(Point(22, row_r[0] + 2), 0));
    }
  }
  FOR(i, 5, M)
  qs[i].push(Order(
      Point(humans[i].pos.y, i % 2 == 0 ? row_r[1] + 2 : row_r[0] + 2), 0));
  solver_cols::solve();
}
} // namespace solver_C3

namespace solver_C4 {
int row_l[4] = {0, 8, 16, 24}, row_r[4] = {4, 12, 20, 29};
int row_c1[4] = {0, 10, 18, 29}, row_c2[4] = {0, 10, 18, 29};

void solve() {
  using solver_cols::build_row;
  using solver_cols::qs;
  solver_cols::C = 4;
  REP(i, 4) {
    solver_cols::row_l[i] = row_l[i];
    solver_cols::row_r[i] = row_r[i];
    solver_cols::row_c1[i] = row_c1[i];
    solver_cols::row_c2[i] = row_c2[i];
  }
  if (M >= 6) {
    solver_cols::K = 6;

    REP(i, 6) {
      if (i == 0) {
        build_row(i, 0, 2, 0);
        build_row(i, 1, 2, 0);
        build_row(i, 2, 2, 0);
        build_row(i, 3, 2, 0, 1);
        build_row(i, 3, 6, 1);
        qs[i].push(Order(Point(6, row_r[2] + 2), 0));
      } else if (i == 1) {
        build_row(i, 1, 10, 1);
        build_row(i, 0, 10, 1, 0);
        build_row(i, 0, 6, 0);
        build_row(i, 1, 6, 0);
        build_row(i, 2, 6, 0);

        qs[i].push(Order(Point(6, row_r[2] + 2), 0));

      } else if (i == 2) {
        build_row(i, 2, 14, 0);
        build_row(i, 3, 14, 0, 0);
        build_row(i, 3, 10, 1);
        build_row(i, 2, 10, 1);
        build_row(i, 1, 10, 1);
        qs[i].push(Order(Point(10, row_r[0] + 2), 0));
      } else if (i == 3) {
        build_row(i, 1, 18, 1);
        build_row(i, 0, 18, 1, 0);
        build_row(i, 0, 14, 0);
        build_row(i, 1, 14, 0);
        qs[i].push(Order(Point(14, row_r[1] + 2), 0));
      } else if (i == 4) {
        build_row(i, 1, 22, 0);
        build_row(i, 2, 22, 0);
        build_row(i, 3, 22, 0, 0);
        build_row(i, 3, 18, 1);
        build_row(i, 2, 18, 1);
        qs[i].push(Order(Point(18, row_r[1] + 2), 0));
      } else if (i == 5) {
        build_row(i, 3, 26, 1);
        qs[i].push(Order(Point(29, row_l[3] - 1), 0));
        qs[i].push(Order(Point(29, row_l[3]), 1));
        qs[i].push(Order(Point(29, row_r[2] + 1), 0));
        qs[i].push(Order(Point(29, row_r[2]), 1));
        build_row(i, 2, 26, 1);
        qs[i].push(Order(Point(29, row_l[2] - 1), 0));
        qs[i].push(Order(Point(29, row_l[2]), 1));
        qs[i].push(Order(Point(29, row_r[1] + 1), 0));
        qs[i].push(Order(Point(29, row_r[1]), 1));
        build_row(i, 1, 26, 1);
        qs[i].push(Order(Point(29, row_l[1] - 1), 0));
        qs[i].push(Order(Point(29, row_l[1]), 1));
        qs[i].push(Order(Point(29, row_r[0] + 1), 0));
        qs[i].push(Order(Point(29, row_r[0]), 1));
        build_row(i, 0, 26, 1, 0);
        build_row(i, 0, 22, 0);
        qs[i].push(Order(Point(22, row_r[0] + 2), 0));
      }
    }
    FOR(i, 6, M)
    qs[i].push(Order(Point(humans[i].pos.y, row_r[i % 3] + 2), 0));
  } else {
    solver_cols::K = 5;

    REP(i, 6) {
      if (i == 0) {
        build_row(i, 0, 6, 0);
        build_row(i, 1, 6, 0);
        build_row(i, 2, 6, 0, 0);
        build_row(i, 2, 2, 1);
        build_row(i, 1, 2, 1);
        qs[i].push(Order(Point(2, row_r[0] + 2), 0));
      } else if (i == 1) {
        build_row(i, 1, 10, 0);
        build_row(i, 2, 10, 0);
        build_row(i, 3, 10, 0, 0);
        build_row(i, 3, 6, 1);
        qs[i].push(Order(Point(6, row_r[2] + 2), 0));
      } else if (i == 2) {
        build_row(i, 3, 14, 1);
        build_row(i, 2, 14, 1);
        build_row(i, 1, 14, 1);
        build_row(i, 0, 14, 1, 0);
        build_row(i, 0, 10, 0);

        qs[i].push(Order(Point(10, row_r[0] + 2), 0));
      } else if (i == 3) {
        build_row(i, 1, 26, 0, 0);
        build_row(i, 1, 22, 1);
        build_row(i, 0, 22, 1, 0);
        build_row(i, 0, 18, 0);
        build_row(i, 1, 18, 0);

        qs[i].push(Order(Point(18, row_r[1] + 2), 0));
      } else if (i == 4) {
        build_row(i, 2, 26, 1, 0);
        build_row(i, 2, 22, 0);
        build_row(i, 3, 22, 0, 0);
        build_row(i, 3, 18, 1);
        build_row(i, 2, 18, 1);
        qs[i].push(Order(Point(18, row_r[1] + 2), 0));
      }
    }
    FOR(i, 5, M)
    qs[i].push(Order(Point(humans[i].pos.y, row_r[i % 3] + 2), 0));
  }
  solver_cols::solve();
}
} // namespace solver_C4

namespace solver_C4M {
int row_l[4] = {2, 9, 16, 23}, row_r[4] = {5, 12, 19, 26};
int row_c1[4] = {3, 10, 17, 24}, row_c2[4] = {4, 11, 18, 25};

void solve() {
  using solver_cols::build_row;
  using solver_cols::qs;
  solver_cols::C = 4;
  REP(i, 4) {
    solver_cols::row_l[i] = row_l[i];
    solver_cols::row_r[i] = row_r[i];
    solver_cols::row_c1[i] = row_c1[i];
    solver_cols::row_c2[i] = row_c2[i];
  }
  if (M >= 9) {
    solver_cols::K = 9;

    REP(i, 9) {
      if (i == 0) {
        build_row(i, 0, 2, 0);
        build_row(i, 1, 2, 0);
        build_row(i, 2, 2, 0);
        qs[i].push(Order(Point(2, row_r[2] + 2), 0));
      } else if (i == 1) {
        build_row(i, 3, 2, 0);
        build_row(i, 3, 6, 1);
        build_row(i, 2, 6, 1);
        qs[i].push(Order(Point(6, row_r[1] + 2), 0));
      } else if (i == 2) {
        build_row(i, 1, 6, 1);
        build_row(i, 0, 6, 1);
        build_row(i, 0, 10, 0);
        qs[i].push(Order(Point(10, row_r[0] + 2), 0));
      } else if (i == 3) {
        build_row(i, 0, 10, 0);
        build_row(i, 1, 10, 0);
        build_row(i, 2, 10, 0);
        build_row(i, 3, 10, 0);

        qs[i].push(Order(Point(10, row_r[3] + 2), 0));
      } else if (i == 4) {
        build_row(i, 3, 14, 1);
        build_row(i, 2, 14, 1);
        build_row(i, 1, 14, 1);
        build_row(i, 0, 14, 1);
        qs[i].push(Order(Point(14, row_l[0] - 2), 0));
      } else if (i == 5) {
        build_row(i, 2, 18, 1);
        build_row(i, 1, 18, 1);
        build_row(i, 0, 18, 1);
        qs[i].push(Order(Point(18, row_l[0] - 2), 0));
      } else if (i == 6) {
        build_row(i, 2, 22, 0);
        build_row(i, 3, 22, 0);
        build_row(i, 3, 18, 1);
        qs[i].push(Order(Point(18, row_r[2] + 2), 0));
      } else if (i == 7) {
        qs[i].push(Order(Point(29, row_r[0] + 1), 0));
        qs[i].push(Order(Point(29, row_r[0]), 1));
        build_row(i, 0, 26, 1);
        qs[i].push(Order(Point(29, row_l[0] - 1), 0));
        qs[i].push(Order(Point(29, row_l[0]), 1));
        build_row(i, 0, 22, 0);
        build_row(i, 1, 22, 0);
        qs[i].push(Order(Point(22, row_r[1] + 2), 0));
      } else if (i == 8) {
        qs[i].push(Order(Point(29, row_r[3] + 1), 0));
        qs[i].push(Order(Point(29, row_r[3]), 1));
        build_row(i, 3, 26, 1);
        qs[i].push(Order(Point(29, row_l[3] - 1), 0));
        qs[i].push(Order(Point(29, row_l[3]), 1));
        qs[i].push(Order(Point(29, row_r[2] + 1), 0));
        qs[i].push(Order(Point(29, row_r[2]), 1));
        build_row(i, 2, 26, 1);
        qs[i].push(Order(Point(29, row_l[2] - 1), 0));
        qs[i].push(Order(Point(29, row_l[2]), 1));
        qs[i].push(Order(Point(29, row_r[1] + 1), 0));
        qs[i].push(Order(Point(29, row_r[1]), 1));
        build_row(i, 1, 26, 1);
        qs[i].push(Order(Point(29, row_l[1] - 1), 0));
        qs[i].push(Order(Point(29, row_l[1]), 1));
        qs[i].push(Order(Point(29, row_r[0] + 2), 0));
      }
    }
    FOR(i, 9, M)
    qs[i].push(Order(Point(humans[i].pos.y, row_r[i % 5 - 1] + 2), 0));
  }
  solver_cols::solve();
}
} // namespace solver_C4M

namespace solver_C5 {
int row_l[5] = {0, 6, 13, 20, 27}, row_r[5] = {2, 9, 16, 23, 29};
int row_c1[5] = {0, 7, 14, 21, 29}, row_c2[5] = {0, 8, 15, 22, 29};

void solve() {
  using solver_cols::build_row;
  using solver_cols::qs;
  solver_cols::C = 5;
  REP(i, 5) {
    solver_cols::row_l[i] = row_l[i];
    solver_cols::row_r[i] = row_r[i];
    solver_cols::row_c1[i] = row_c1[i];
    solver_cols::row_c2[i] = row_c2[i];
  }
  if (M >= 8) {
    solver_cols::K = 8;

    REP(i, 8) {
      if (i == 0) {
        build_row(i, 1, 2, 0);
        build_row(i, 2, 2, 0);
        build_row(i, 3, 2, 0);
        qs[i].push(Order(Point(2, row_r[3] + 2), 0));
      } else if (i == 1) {
        build_row(i, 3, 6, 1);
        build_row(i, 2, 6, 1);
        build_row(i, 1, 6, 1);
        qs[i].push(Order(Point(6, row_r[0] + 2), 0));
      } else if (i == 2) {
        build_row(i, 1, 10, 0);
        build_row(i, 2, 10, 0);
        build_row(i, 3, 10, 0);
        qs[i].push(Order(Point(10, row_r[3] + 2), 0));
      } else if (i == 3) {
        build_row(i, 3, 14, 1);
        build_row(i, 2, 14, 1);
        qs[i].push(Order(Point(14, row_r[1] + 2), 0));
      } else if (i == 4) {
        build_row(i, 1, 14, 1, 1);
        build_row(i, 1, 18, 0);
        build_row(i, 2, 18, 0);
        qs[i].push(Order(Point(18, row_r[2] + 2), 0));
      } else if (i == 5) {
        build_row(i, 2, 22, 0);
        build_row(i, 3, 22, 0, 1);
        build_row(i, 3, 18, 1);
        qs[i].push(Order(Point(18, row_r[2] + 2), 0));
      } else if (i == 6) {
        build_row(i, 1, 22, 1, 1);
        build_row(i, 1, 26, 0, 1);
        qs[i].push(Order(Point(28, row_r[1]), 0));
        qs[i].push(Order(Point(29, row_r[1]), 1));
        qs[i].push(Order(Point(28, row_l[1]), 0));
        qs[i].push(Order(Point(29, row_l[1]), 1));
        qs[i].push(Order(Point(28, row_r[0] + 2), 0));

      } else if (i == 7) {
        qs[i].push(Order(Point(28, row_r[3]), 0));
        qs[i].push(Order(Point(29, row_r[3]), 1));
        build_row(i, 3, 26, 1, 1);
        qs[i].push(Order(Point(28, row_l[3]), 0));
        qs[i].push(Order(Point(29, row_l[3]), 1));
        qs[i].push(Order(Point(28, row_r[2]), 0));
        qs[i].push(Order(Point(29, row_r[2]), 1));
        build_row(i, 2, 26, 1);
        qs[i].push(Order(Point(28, row_l[2]), 0));
        qs[i].push(Order(Point(29, row_l[2]), 1));
        qs[i].push(Order(Point(28, row_r[1] + 2), 0));
      }
    }
    FOR(i, 8, M)
    qs[i].push(Order(Point(humans[i].pos.y, row_r[i % 4] + 2), 0));
  } else if (M == 7) {
    solver_cols::K = 7;

    REP(i, 7) {
      if (i == 0) {
        build_row(i, 1, 2, 0);
        build_row(i, 2, 2, 0);
        build_row(i, 3, 2, 0);
        qs[i].push(Order(Point(2, row_r[3] + 2), 0));
      } else if (i == 1) {
        build_row(i, 2, 6, 0);
        build_row(i, 3, 6, 0, 10);
        build_row(i, 3, 10, 1);
        qs[i].push(Order(Point(6, row_r[2] + 2), 0));

      } else if (i == 2) {
        build_row(i, 2, 10, 1);
        build_row(i, 1, 10, 1, 0);
        build_row(i, 1, 6, 0);
        qs[i].push(Order(Point(6, row_r[1] + 2), 0));
      } else if (i == 3) {
        build_row(i, 1, 14, 0);
        build_row(i, 2, 14, 0);
        build_row(i, 3, 14, 0);
        qs[i].push(Order(Point(14, row_r[3] + 2), 0));
      } else if (i == 4) {
        build_row(i, 3, 22, 0, 0);
        build_row(i, 3, 18, 1);
        build_row(i, 2, 18, 1);
        qs[i].push(Order(Point(18, row_r[1] + 2), 0));
      } else if (i == 5) {
        build_row(i, 3, 22, 1);
        build_row(i, 2, 22, 1);
        build_row(i, 1, 22, 1, 0);
        build_row(i, 1, 18, 0);
        qs[i].push(Order(Point(18, row_r[0] + 2), 0));
      } else if (i == 6) {
        qs[i].push(Order(Point(28, row_r[3]), 0));
        qs[i].push(Order(Point(29, row_r[3]), 1));
        build_row(i, 3, 26, 1, 1);
        qs[i].push(Order(Point(28, row_l[3]), 0));
        qs[i].push(Order(Point(29, row_l[3]), 1));
        qs[i].push(Order(Point(28, row_r[2]), 0));
        qs[i].push(Order(Point(29, row_r[2]), 1));
        build_row(i, 2, 26, 1, 1);
        qs[i].push(Order(Point(28, row_l[2]), 0));
        qs[i].push(Order(Point(29, row_l[2]), 1));
        qs[i].push(Order(Point(28, row_r[1]), 0));
        qs[i].push(Order(Point(29, row_r[1]), 1));
        build_row(i, 1, 26, 1, 1);
        qs[i].push(Order(Point(28, row_l[1]), 0));
        qs[i].push(Order(Point(29, row_l[1]), 1));
        qs[i].push(Order(Point(28, row_r[0] + 2), 0));
      }
    }
    FOR(i, 7, M)
    qs[i].push(Order(Point(humans[i].pos.y, row_r[i % 4] + 2), 0));
  } else if (M == 6) {
    solver_cols::K = 6;

    REP(i, 6) {
      if (i == 0) {
        build_row(i, 0, 2, 0);
        build_row(i, 1, 2, 0);
        build_row(i, 2, 2, 0);
        build_row(i, 3, 2, 0);
        build_row(i, 4, 2, 0, 1);
        build_row(i, 4, 6, 1);
        qs[i].push(Order(Point(6, row_r[3] + 2), 0));
      } else if (i == 1) {
        build_row(i, 3, 6, 1);
        build_row(i, 2, 6, 1);
        build_row(i, 1, 6, 1);
        build_row(i, 0, 6, 1, 1);
        build_row(i, 0, 10, 0);
        build_row(i, 1, 10, 0);
        qs[i].push(Order(Point(10, row_r[1] + 2), 0));
      } else if (i == 2) {
        build_row(i, 3, 14, 0);
        build_row(i, 4, 14, 0, 0);
        build_row(i, 4, 10, 1);
        build_row(i, 3, 10, 1);
        build_row(i, 2, 10, 1);
        qs[i].push(Order(Point(10, row_r[1] + 2), 0));
      } else if (i == 3) {
        build_row(i, 2, 18, 1);
        build_row(i, 1, 18, 1);
        build_row(i, 0, 18, 1, 0);
        build_row(i, 0, 14, 0);
        build_row(i, 1, 14, 0);
        build_row(i, 2, 14, 0);

        qs[i].push(Order(Point(14, row_r[2] + 2), 0));
      } else if (i == 4) {
        build_row(i, 1, 22, 0);
        build_row(i, 2, 22, 0);
        build_row(i, 3, 22, 0);
        build_row(i, 4, 22, 0, 0);
        build_row(i, 4, 18, 1);
        build_row(i, 3, 18, 1);
        qs[i].push(Order(Point(18, row_r[2] + 2), 0));
      } else if (i == 5) {
        build_row(i, 4, 26, 1);
        qs[i].push(Order(Point(29, row_l[4] - 1), 0));
        qs[i].push(Order(Point(29, row_l[4]), 1));
        qs[i].push(Order(Point(29, row_r[3] + 1), 0));
        qs[i].push(Order(Point(29, row_r[3]), 1));
        build_row(i, 3, 26, 1);
        qs[i].push(Order(Point(29, row_l[3] - 1), 0));
        qs[i].push(Order(Point(29, row_l[3]), 1));
        qs[i].push(Order(Point(29, row_r[2] + 1), 0));
        qs[i].push(Order(Point(29, row_r[2]), 1));
        build_row(i, 2, 26, 1);
        qs[i].push(Order(Point(29, row_l[2] - 1), 0));
        qs[i].push(Order(Point(29, row_l[2]), 1));
        qs[i].push(Order(Point(29, row_r[1] + 1), 0));
        qs[i].push(Order(Point(29, row_r[1]), 1));
        build_row(i, 1, 26, 1);
        qs[i].push(Order(Point(29, row_l[1] - 1), 0));
        qs[i].push(Order(Point(29, row_l[1]), 1));
        qs[i].push(Order(Point(29, row_r[0] + 1), 0));
        qs[i].push(Order(Point(29, row_r[0]), 1));
        build_row(i, 0, 26, 1, 0);
        build_row(i, 0, 22, 0);
        qs[i].push(Order(Point(22, row_r[0] + 2), 0));
      }
    }
    FOR(i, 6, M)
    qs[i].push(Order(Point(humans[i].pos.y, row_r[i % 4] + 2), 0));
  } else if (M == 5) {
  }
  solver_cols::solve();
}
} // namespace solver_C5

void read_problem() {
  N = judge::N;
  REP(i, N) {
    pets[i] = judge::pets[i];
    petn[pets[i].pos.y][pets[i].pos.x]++;
  }
  M = judge::M;
  REP(i, M) {
    humans[i] = judge::humans[i];
    humann[humans[i].pos.y][humans[i].pos.x]++;
  }
}

int main(int argc, char **argv) {
  if (argc >= 2 && string(argv[1]) == "local") {
    judge::local = 1;
    judge::seed = stoi(argv[2]);
    judge::Nmin = stoi(argv[3]);
    judge::Nmax = stoi(argv[4]);
    judge::Mmin = stoi(argv[5]);
    judge::Mmax = stoi(argv[6]);
    judge::Gmin = stoi(argv[7]);
    judge::Gmax = stoi(argv[8]);
  }
  judge::make_problem();
  read_problem();
  /*geta = getas[M - 5][N - 10];
  bias1 = bias1s[M - 5][N - 10];
  bias2 = bias2s[M - 5][N - 10];
  cut = cuts[M - 5][N - 10];*/

  if (argc >= 10) {
    geta = stoi(argv[9]);
    canmove = stoi(argv[10]);
    bias1 = stof(argv[11]);
    bias2 = stof(argv[12]);
    cut = stof(argv[13]);
    if (argc >= 15) {
    }
  }
  if (M == 8 && N <= 14 && judge::G <= 10 ||
      M >= 9 && N <= 15 && judge::G <= 10 || M == 7 && N == 10 && judge::G <= 8)
    solver_C5::solve();
  else if (M == 5 && 0) {
    solver_C3::solve();
  } else if (M == 9 && 0) {
    solver_prev::solver_C4M::solve();
  } else {
    solver_prev::solve();
  }
  if (judge::local)
    debug(judge::get_score());
}
