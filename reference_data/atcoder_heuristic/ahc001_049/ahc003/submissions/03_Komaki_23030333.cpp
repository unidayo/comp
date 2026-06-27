#ifndef KOMAKI_LOCAL
#define NDEBUG
#endif

#include <bits/stdc++.h>
#include <sys/time.h>
#include <unistd.h>
using namespace std;
#define rep(i, n)   for(int i = 0; (i) < ((int)(n)); ++(i))
#define bit(n)      (((int)1)<<((int)(n)))
#define sz(v)       ((int)((v).size()))

std::string dbgDelim(int &i){ return (i++ == 0 ? "" : ", "); }
#define dbgEmbrace(exp) { int i = 0; os << "{"; { exp; } os << "}"; return os; }
template <class T> std::ostream& operator<<(std::ostream &os, std::vector<T> v);
template <class T> std::ostream& operator<<(std::ostream &os, std::set<T> v);
template <class T> std::ostream& operator<<(std::ostream &os, std::queue<T> q);
template <class T> std::ostream& operator<<(std::ostream &os, std::priority_queue<T> q);
template <class T, class K> std::ostream& operator<<(std::ostream &os, std::pair<T, K> p);
template <class T, class K> std::ostream& operator<<(std::ostream &os, std::map<T, K> mp);
template <class T, class K> std::ostream& operator<<(std::ostream &os, std::unordered_map<T, K> mp);
template <int INDEX, class TUPLE> void dbgDeploy(std::ostream &os, TUPLE _){}
template <int INDEX, class TUPLE, class H, class ...Ts> void dbgDeploy(std::ostream &os, TUPLE t){ os << (INDEX == 0 ? "" : ", ") << get<INDEX>(t); dbgDeploy<INDEX + 1, TUPLE, Ts...>(os, t); }
template <class T, class K> void dbgDeploy(std::ostream &os, std::pair<T, K> p, std::string delim){ os << "(" << p.first << delim << p.second << ")"; }
template <class ...Ts> std::ostream& operator<<(std::ostream &os, std::tuple<Ts...> t){ os << "("; dbgDeploy<0, std::tuple<Ts...>, Ts...>(os, t); os << ")"; return os; }
template <class T, class K> std::ostream& operator<<(std::ostream &os, std::pair<T, K> p){ dbgDeploy(os, p, ", "); return os; }
template <class T> std::ostream& operator<<(std::ostream &os, std::vector<T> v){ dbgEmbrace( for(T t: v){ os << dbgDelim(i) << t; }); }
template <class T> std::ostream& operator<<(std::ostream &os, std::set<T> s){ dbgEmbrace( for(T t: s){ os << dbgDelim(i) << t; }); }
template <class T> std::ostream& operator<<(std::ostream &os, std::queue<T> q){ dbgEmbrace( for(; q.size(); q.pop()){ os << dbgDelim(i) << q.front(); }); }
template <class T> std::ostream& operator<<(std::ostream &os, std::priority_queue<T> q){ dbgEmbrace( for(; q.size(); q.pop()){ os << dbgDelim(i) << (T) q.top();   }); }
template <class T, class K> std::ostream& operator<<(std::ostream &os, std::map<T, K> mp){ dbgEmbrace( for(auto p: mp){ os << dbgDelim(i); dbgDeploy(os, p, "->"); }); }
template <class T, class K> std::ostream& operator<<(std::ostream &os, std::unordered_map<T, K> mp){ dbgEmbrace( for(auto p: mp){ os << dbgDelim(i); dbgDeploy(os, p, "->"); }); }
#define DBG_OUT std::cerr
#define DBG_OVERLOAD(_1, _2, _3, _4, _5, _6, _7, _8, _9, macro_name, ...) macro_name
#define DBG_LINE() { char s[99]; sprintf(s, "line:%3d | ", __LINE__); DBG_OUT << s; }
#define DBG_OUTPUT(v) { DBG_OUT << (#v) << "=" << (v); }
#define DBG1(v, ...) { DBG_OUTPUT(v); }
#define DBG2(v, ...) { DBG_OUTPUT(v); DBG_OUT << ", "; DBG1(__VA_ARGS__); }
#define DBG3(v, ...) { DBG_OUTPUT(v); DBG_OUT << ", "; DBG2(__VA_ARGS__); }
#define DBG4(v, ...) { DBG_OUTPUT(v); DBG_OUT << ", "; DBG3(__VA_ARGS__); }
#define DBG5(v, ...) { DBG_OUTPUT(v); DBG_OUT << ", "; DBG4(__VA_ARGS__); }
#define DBG6(v, ...) { DBG_OUTPUT(v); DBG_OUT << ", "; DBG5(__VA_ARGS__); }
#define DBG7(v, ...) { DBG_OUTPUT(v); DBG_OUT << ", "; DBG6(__VA_ARGS__); }
#define DBG8(v, ...) { DBG_OUTPUT(v); DBG_OUT << ", "; DBG7(__VA_ARGS__); }
#define DBG9(v, ...) { DBG_OUTPUT(v); DBG_OUT << ", "; DBG8(__VA_ARGS__); }

#define DEBUG0() { DBG_LINE(); DBG_OUT << std::endl; }
#define DEBUG(...)                                                      \
  {                                                                     \
    DBG_LINE();                                                         \
    DBG_OVERLOAD(__VA_ARGS__, DBG9, DBG8, DBG7, DBG6, DBG5, DBG4, DBG3, DBG2, DBG1)(__VA_ARGS__); \
    DBG_OUT << std::endl;                                               \
  }


class XorShift{
public:
    int rand()
    {
      t = x ^ (x << 11);
      x = y;
      y = z;
      z = w;
      w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
      return w & 0x7fffffff;
    }

    int rand(int high)
    {
      return rand() % high;
    }

    int rand(int low, int high)
    {
      int r = rand() % (high - low);
      return low + r;
    }

    void reset(int step)
    {
      x = 123456789;
      y = 362436069;
      z = 521288629;
      w = 88675123;
      t = 1;

      rep(i, step) rand();
    }

private:
    unsigned int x = 123456789;
    unsigned int y = 362436069;
    unsigned int z = 521288629;
    unsigned int w = 88675123;
    unsigned int t = 1;
};



const int DXS[] = {1, 0, -1, 0};
const int DYS[] = {0, 1, 0, -1};
const string DIR_STR = "DRUL";

string dirs_to_str(vector<int> &dirs)
{
  stringstream ss;
  for(int d: dirs) ss << DIR_STR[d];
  return ss.str();
}

class Q
{
public:
    int sx;
    int sy;
    int tx;
    int ty;

    static Q gen_random(XorShift &xor_shift)
    {
      while(true){
        int sx = xor_shift.rand(30);
        int sy = xor_shift.rand(30);
        int tx = xor_shift.rand(30);
        int ty = xor_shift.rand(30);
        if(abs(sx - tx) + abs(sy - ty) < 10) continue;
        return Q{sx, sy, tx, ty};
      }
    }

    int dist() const
    {
      return abs(sx - tx) + abs(sy - ty);
    }
};

class QA
{
public:
    Q q;
    vector<int> dirs;
    int score;
};

template <typename T> class Dijkstra
{
public:
    Dijkstra<T>& addEdge(int from, int dest, T cost);
    void update(int source);
    T getDistance(int vertex);

    ~Dijkstra();
    explicit Dijkstra(int vertex_num);

private:
    const int vertex_num;
    T *distances;

    class Edge
    {
    public:
        int target;
        T distance;
        Edge(int target, T distance) : target(target), distance(distance) {}
    };
    std::vector<Edge> *edges;

    class QueueElement
    {
    public:
        T distance;
        int vertex;
        inline QueueElement(T distance, int vertex) : distance(distance), vertex(vertex){}
        inline bool operator>(const QueueElement &e)const{ return distance > e.distance; }
    };
};

template <typename T>
inline Dijkstra<T>::~Dijkstra()
{
  delete [] edges;
  delete [] distances;
}

template <typename T>
inline Dijkstra<T>::Dijkstra(int vertex_num) : vertex_num(vertex_num)
{
  edges = new std::vector<Edge>[vertex_num];
  distances = new T[vertex_num];
}

template <typename T>
inline T Dijkstra<T>::getDistance(int vertex)
{
  return distances[vertex];
}

template <typename T>
inline void Dijkstra<T>::update(int source)
{
  memset(distances, -1, sizeof(T) * vertex_num);

  std::priority_queue<QueueElement, std::vector<QueueElement>, std::greater<QueueElement> > q;
  distances[source] = 0;
  q.push(QueueElement(0, source));

  for(int total_size = 1; 0 < total_size;){
    int vertex = q.top().vertex;
    T distance = q.top().distance;
    q.pop();
    if(distances[vertex] < distance) continue;
    --total_size;

    for(Edge e : edges[vertex]){
      int next_vertex = e.target;
      T next_distance = e.distance + distance;
      if(distances[next_vertex] == -1){
        ++total_size;
        distances[next_vertex] = next_distance;
        q.push(QueueElement(next_distance, next_vertex));
      }else if(next_distance < distances[next_vertex]){
        distances[next_vertex] = next_distance;
        q.push(QueueElement(next_distance, next_vertex));
      }
    }
  }
}

template <typename T>
inline Dijkstra<T>& Dijkstra<T>::addEdge(int from, int dest, T cost)
{
  edges[from].push_back(Edge(dest, cost));
  return *this;
}

class Env
{
public:
    virtual Q get_question() = 0;
    virtual int compute_score(Q q, vector<int> &dirs) = 0;
    virtual double compute_relative_score(Q &q, vector<int> &dirs) = 0;
};

class RustEnv : public Env
{
public:

    Q get_question() override
    {
      int sx, sy, tx, ty;
      cin >> sx >> sy >> tx >> ty;
      return Q{sx, sy, tx, ty};
    }

    int compute_score(Q q, vector<int> &dirs) override
    {
      cout << dirs_to_str(dirs) << endl;
      int score;
      cin >> score;
      return score;
    }


    double compute_relative_score(Q &q, vector<int> &dirs) override
    {
      return 1.0;
    }
};

class LocalEnv : public Env
{
public:
    XorShift xor_shift;
    int m;

    explicit LocalEnv(int seed)
    {
      xor_shift.reset(seed);
      int d = xor_shift.rand(100, 2001);
      this->m = seed % 2 + 1; // xor_shift.rand(1, 3);
      cout << "m: " << m << ", d: " << d << endl;
      init_d = d;

      rep(xy, 30){
        h_bases0.push_back(xor_shift.rand(1000 + d, 9001 - d));
        v_bases0.push_back(xor_shift.rand(1000 + d, 9001 - d));
        h_bases1.push_back(xor_shift.rand(1000 + d, 9001 - d));
        v_bases1.push_back(xor_shift.rand(1000 + d, 9001 - d));
        h_thresholds.push_back(xor_shift.rand(1, 29));
        v_thresholds.push_back(xor_shift.rand(1, 29));
      }

      rep(x, 30){
        vector<int> v(29);
        rep(y, 29) v[y] = xor_shift.rand(-d, d + 1) +
                          ((m == 1 || y < h_thresholds[x]) ? h_bases0[x] : h_bases1[x]);
        h_dists.push_back(v);
      }

      rep(x, 29){
        vector<int> v(30);
        rep(y, 30) v[y] = xor_shift.rand(-d, d + 1) +
                          ((m == 1 || x < v_thresholds[y]) ? v_bases0[y] : v_bases1[y]);
        v_dists.push_back(v);
      }
    }

    Q get_question() override
    {
      return Q::gen_random(xor_shift);
    }

    int compute_score(Q q, vector<int> &dirs) override
    {
      return compute_score(q, dirs, true);
    }

    int compute_score(Q q, vector<int> &dirs, bool add_noise)
    {
      int score = 0;
      int x = q.sx;
      int y = q.sy;
      // "DRUL"
      for(int dir: dirs){
        if(dir == 0){
          score += v_dists[x][y];
          x += 1;
        }else if(dir == 1){
          score += h_dists[x][y];
          y += 1;
        }else if(dir == 2){
          score += v_dists[x - 1][y];
          x -= 1;
        }else{
          score += h_dists[x][y - 1];
          y -= 1;
        }
      }
      if(add_noise) score = (int) (score * (0.9 + 0.2 * xor_shift.rand(1000001) / 1000000.0));
      return score;
    }

    double compute_relative_score(Q &q, vector<int> &dirs) override
    {
      Dijkstra<int> graph(30 * 30);
      rep(x, 30)rep(y, 29) graph.addEdge(x * 30 + y, x * 30 + (y + 1), h_dists[x][y]);
      rep(x, 30)rep(y, 29) graph.addEdge(x * 30 + (y + 1), x * 30 + y, h_dists[x][y]);
      rep(x, 29)rep(y, 30) graph.addEdge(x * 30 + y, (x + 1) * 30 + y, v_dists[x][y]);
      rep(x, 29)rep(y, 30) graph.addEdge((x + 1) * 30 + y, x * 30 + y, v_dists[x][y]);
      graph.update(q.sx * 30 + q.sy);
      int best = graph.getDistance(q.tx * 30 + q.ty);
      return best / ((double) compute_score(q, dirs, false));
    }

    vector<vector<int>>& get_h_dists()
    {
      return h_dists;
    }

    vector<vector<int>>& get_v_dists()
    {
      return v_dists;
    }

// private:
    int init_d;
    vector<int> h_bases0;
    vector<int> v_bases0;
    vector<int> h_bases1;
    vector<int> v_bases1;
    vector<int> h_thresholds;
    vector<int> v_thresholds;
    vector<vector<int>> h_dists;
    vector<vector<int>> v_dists;
};

class VisitCounter
{
public:
    VisitCounter()
    {
      rep(i, 30) h_counts_i.emplace_back(29, 0);
      rep(i, 29) v_counts_i.emplace_back(30, 0);
      rep(i, 30) h_counts_d.emplace_back(29, 0);
      rep(i, 29) v_counts_d.emplace_back(30, 0);

      h_lines_d = vector<double>(30, 0);
      v_lines_d = vector<double>(30, 0);
      h_lines_i = vector<int>(30, 0);
      v_lines_i = vector<int>(30, 0);
    }

    void travel(QA &qa)
    {
      int x = qa.q.sx;
      int y = qa.q.sy;
      double d = 1.0 / sz(qa.dirs);

      unordered_set<int> hs;
      unordered_set<int> vs;
      for(int dir: qa.dirs){
        if(dir == 0 || dir == 2){
          if(dir == 2) --x;
          v_counts_i[x][y] += 1;
          v_counts_d[x][y] += d;
          vs.insert(y);
          v_lines_d[y] += d;
          if(dir == 0) ++x;
        }else{
          if(dir == 3) --y;
          h_counts_i[x][y] += 1;
          h_counts_d[x][y] += d;
          hs.insert(x);
          h_lines_d[x] += d;
          if(dir == 1) ++y;
        }
      }
      for(int i: vs) ++v_lines_i[i];
      for(int i: hs) ++h_lines_i[i];
    }

    vector<vector<double>> h_counts_d;
    vector<vector<double>> v_counts_d;
    vector<vector<int>> h_counts_i;
    vector<vector<int>> v_counts_i;
    vector<double> h_lines_d;
    vector<double> v_lines_d;
    vector<int> h_lines_i;
    vector<int> v_lines_i;

};

class PathSolver
{
private:
    class Entry
    {
    public:
        double d;
        int x, y, prev_dir;

        inline bool operator<(const Entry &e)const
        {
          return d > e.d;
        }
    };

    class Entry2
    {
    public:
        double d;
        int x, y, prev_dir;

        inline bool operator<(const Entry2 &e)const
        {
          return d > e.d;
        }
    };

public:

    PathSolver()
    {
      rep(i, 30) h_dists.emplace_back(29, -1);
      rep(i, 29) v_dists.emplace_back(30, -1);
      rep(x, 30) dists.emplace_back(vector<double>(30, -1));
      rep(x, 30) prev_dirs.emplace_back(vector<int>(30, -1));
    }

    /*
    vector<int> find_shortest(vector<vector<int>> &_h_dists, vector<vector<int>> &_v_dists, Q &q)
    {
      rep(x, 30)rep(y, 29) this->h_dists[x][y] = _h_dists[x][y];
      rep(x, 29)rep(y, 30) this->v_dists[x][y] = _v_dists[x][y];
      return find_shortest(q, 1.0);
    }

    vector<int> find_shortest(vector<vector<double>> &_h_dists, vector<vector<double>> &_v_dists, Q &q)
    {
      return find_shortest(_h_dists, _v_dists, q, 1.0);
    }
     */

    vector<int> find_shortest(vector<vector<double>> &_h_dists, vector<vector<double>> &_v_dists, Q &q, double dir_discount)
    {
      rep(x, 30)rep(y, 29) this->h_dists[x][y] = _h_dists[x][y];
      rep(x, 29)rep(y, 30) this->v_dists[x][y] = _v_dists[x][y];

      rep(x, 30)rep(y, 30)rep(dir, 4){
            int nx = x + DXS[dir];
            int ny = y + DYS[dir];
            if (nx < 0 || 30 <= nx) continue;
            if (ny < 0 || 30 <= ny) continue;

            double &d = dir_dists[x][y][dir];
            if (dir == 0) d = v_dists[x][y];
            else if (dir == 2) d = v_dists[x - 1][y];
            else if (dir == 1) d = h_dists[x][y];
            else if (dir == 3) d = h_dists[x][y - 1];
          }

      return find_shortest(q, dir_discount);
    }

    /*
    vector<int> find_shortest2(vector<vector<double>> &_h_dists, vector<vector<double>> &_v_dists, Q &q, double penalty)
    {
      rep(x, 30)rep(y, 29) this->h_dists[x][y] = _h_dists[x][y];
      rep(x, 29)rep(y, 30) this->v_dists[x][y] = _v_dists[x][y];
      rep(x, 30)rep(y, 30)rep(dir, 4){
            int nx = x + DXS[dir];
            int ny = y + DYS[dir];
            if (nx < 0 || 30 <= nx) continue;
            if (ny < 0 || 30 <= ny) continue;

            double &d = dir_dists[x][y][dir];
            if (dir == 0) d = v_dists[x][y];
            else if (dir == 2) d = v_dists[x - 1][y];
            else if (dir == 1) d = h_dists[x][y];
            else if (dir == 3) d = h_dists[x][y - 1];
      }
      return find_shortest2(q, penalty);
    }
     */

private:

    /*
    static inline int idx(int x, int y, int dir)
    {
      return (((x << 5) | y) << 2) | dir;
    }

    vector<int> find_shortest2(Q &q, double penalty)
    {
      const double NEG = -1;
      rep(turn_count, MAX_TURN + 1)rep(i, T24) dists2[turn_count][i] = NEG;
      rep(i, T24) best_dists2[i] = 1e100;

      rep(prev_dir, 4) dists2[0][idx(q.sx, q.sy, prev_dir)] = best_dists2[idx(q.sx, q.sy, prev_dir)] = 0;
      rep(prev_dir, 4) queue2[0].push(Entry2{0, q.sx, q.sy, prev_dir});

      rep(q_dist, MAX_TURN + 1) {
        auto &qu = queue2[q_dist];
        while (sz(qu)) {
          Entry2 e = qu.top();
          qu.pop();

          int id = idx(e.x, e.y, e.prev_dir);
          if (best_dists2[id] < e.d) continue;
          best_dists2[id] = e.d;

          rep(dir, 4) {
            if(dir == (dir ^ 2)) continue;
            int nx = e.x + DXS[dir];
            int ny = e.y + DYS[dir];
            if (nx < 0 || 30 <= nx) continue;
            if (ny < 0 || 30 <= ny) continue;
            int next_turn_count = min(MAX_TURN, q_dist + ((e.prev_dir == dir) ? 0 : 1));
            double nd = e.d + dir_dists[e.x][e.y][dir];

            int nid = idx(nx, ny, dir);
            if(best_dists2[nid] <= nd) continue;
            double &d_ref = dists2[next_turn_count][nid];
            if (d_ref != NEG && d_ref <= nd) continue;
            d_ref = nd;
            if(next_turn_count == q_dist) best_dists2[nid] = nd;
            prev_prev_dirs[next_turn_count][nid] = e.prev_dir;
            queue2[next_turn_count].push(Entry2{nd, nx, ny, dir});
          }
        }
      }

      int x = q.tx;
      int y = q.ty;
      int turn_count = 0;
      int prev_dir = -1;
      auto penalty_dist = [&](int _x, int _y, int _turn_count, int _prev_dir){
        return dists2[_turn_count][idx(_x, _y, _prev_dir)] * (1.0 + penalty * _turn_count);
      };
      rep(turn_count_, MAX_TURN + 1)rep(prev_dir_, 4){
        if(dists2[turn_count_][idx(x, y, prev_dir_)] == NEG) continue;
        if(prev_dir == -1 || penalty_dist(x, y, turn_count_, prev_dir_) < penalty_dist(x, y, turn_count, prev_dir)){
          turn_count = turn_count_;
          prev_dir = prev_dir_;
        }
      }

      vector<int> dirs;
      while(x != q.sx || y != q.sy){
        int prev_prev_dir = prev_prev_dirs[turn_count][idx(x, y, prev_dir)];
        int prev_turn_count = (prev_prev_dir == prev_dir) ? turn_count : (turn_count - 1);
        dirs.push_back(prev_dir);
        int nx = x + DXS[prev_dir ^ 2];
        int ny = y + DYS[prev_dir ^ 2];
        x = nx;
        y = ny;
        prev_dir = prev_prev_dir;
        turn_count = prev_turn_count;
      }
      reverse(dirs.begin(), dirs.end());
      return dirs;
    }
     */

    vector<int> find_shortest(Q &q, double dir_discount)
    {
      const double NEG = -1;
      rep(x, 30)rep(y, 30) dists[x][y] = NEG;

      dists[q.sx][q.sy] = 0;
      queue.push(Entry{0, q.sx, q.sy, -1});

      while(sz(queue)){
        double d = queue.top().d;
        int x = queue.top().x;
        int y = queue.top().y;
        int prev_dir = queue.top().prev_dir;
        queue.pop();

        if(dists[x][y] != d) continue;
        prev_dirs[x][y] = prev_dir;

        if(x == q.tx && y == q.ty) break;

        rep(dir, 4){
          int nx = x + DXS[dir];
          int ny = y + DYS[dir];
          if(nx < 0 || 30 <= nx) continue;
          if(ny < 0 || 30 <= ny) continue;
          double discount = (dir == prev_dir) ? dir_discount : 1.0;
          double nd = d + dir_dists[x][y][dir] * discount;
          double &d_ref = dists[nx][ny];
          if(d_ref != NEG && d_ref <= nd) continue;
          d_ref = nd;
          queue.push(Entry{nd, nx, ny, dir});
        }
      }
      while(sz(queue)) queue.pop();

      vector<int> dirs;
      int x = q.tx;
      int y = q.ty;
      while(x != q.sx || y != q.sy){
        int prev_dir = prev_dirs[x][y];
        dirs.push_back(prev_dir);
        int nx = x + DXS[prev_dir ^ 2];
        int ny = y + DYS[prev_dir ^ 2];
        x = nx;
        y = ny;
      }
      reverse(dirs.begin(), dirs.end());
      return dirs;
    }

    //static const int T = 32;
    //static const int T24 = T * T * 4;
    //static const int MAX_TURN = 20;
    //double dists2[MAX_TURN + 1][T24];
    //double best_dists2[T24];
    //int prev_prev_dirs[MAX_TURN + 1][T24];
    //priority_queue<Entry2> queue2[MAX_TURN + 1];

    double dir_dists[30][30][4];
    vector<vector<double>> h_dists;
    vector<vector<double>> v_dists;
    vector<vector<double>> dists;
    vector<vector<int>> prev_dirs;
    priority_queue<Entry> queue;
};

const int SIGMOID_T = 500000;
const int SIGMOID_T2 = SIGMOID_T * 2;
vector<double> sigmoids;
vector<double> exps;

inline double sigmoid_inv(double d)
{
  return d * 100 / SIGMOID_T;
}

inline double sigmoid_scale(double d)
{
  return d * SIGMOID_T / 100;
}

inline void init_sigmoid()
{
  sigmoids.resize(SIGMOID_T2);
  exps.resize(SIGMOID_T2);
  rep(i, SIGMOID_T) sigmoids[SIGMOID_T + i] = 1.0 / (1.0 + exp(-sigmoid_inv(i)));
  rep(i, SIGMOID_T) sigmoids[SIGMOID_T - i] = 1.0 / (1.0 + exp(sigmoid_inv(i)));
  rep(i, SIGMOID_T) exps[SIGMOID_T + i] = exp(sigmoid_inv(i));
  rep(i, SIGMOID_T) exps[SIGMOID_T - i] = exp(-sigmoid_inv(i));
}

inline double sigmoid(double d)
{
  int index = int(sigmoid_scale(d)) + SIGMOID_T;
  if(0 < index && index < SIGMOID_T2) return sigmoids[index];
  return 1.0 / (1.0 + exp(-d));
}

inline double my_exp(double d)
{
  int index = int(sigmoid_scale(d)) + SIGMOID_T;
  if(0 < index && index < SIGMOID_T2) return exps[index];
  return exp(-d);
}

inline int idx(const int x, const int y)
{
  return (x << 5) | y;
}

class NnModel
{
public:
    static const int T = 32;
    static const int T2 = T * T;
    constexpr static const double R_SCALE_COEFF = 0.25;

    double hes[T2], hEs[T2], hRs[T2], sig_hes[T2];
    double ves[T2], vEs[T2], vRs[T2], sig_ves[T2];
    double sig_hrs[T], hrs[T], hr_scales[T], exp_hr_scales[T];
    double sig_vrs[T], vrs[T], vr_scales[T], exp_vr_scales[T];
    double hb0s[T], hB0s[T], sig_hb0s[T];
    double vb0s[T], vB0s[T], sig_vb0s[T];
    double hb1s[T], hB1s[T], sig_hb1s[T];
    double vb1s[T], vB1s[T], sig_vb1s[T];

    double grad_hes[T2];
    double grad_ves[T2];

    double sig_d = 0;
    double d = 0.0;

    bool is_m1 = false;
    bool debug = false;

    double learning_rate;
    double e_dev_e;
    double e_dev_b;

    int lower_type_e;
    int lower_type_b;
    int reg_type;
    XorShift xor_shift;

    vector<vector<double>> h_dists;
    vector<vector<double>> v_dists;

    NnModel(bool is_m1, double learning_rate, double e_dev_e, int lower_type_e, double e_dev_b, int lower_type_b, int reg_type)
    {
      rep(i, T2) hes[i] = hEs[i] = hRs[i] = sig_hes[i] = 0.0;
      rep(i, T2) ves[i] = vEs[i] = vRs[i] = sig_ves[i] = 0.0;
      rep(i, T) sig_hrs[i] = hrs[i] = hr_scales[i] = 0;
      rep(i, T) sig_vrs[i] = vrs[i] = vr_scales[i] = 0;
      rep(i, T) hb0s[i] = hB0s[i] = sig_hb0s[i] = 0;
      rep(i, T) vb0s[i] = vB0s[i] = sig_vb0s[i] = 0;
      rep(i, T) hb1s[i] = hB1s[i] = sig_hb1s[i] = 0;
      rep(i, T) vb1s[i] = vB1s[i] = sig_vb1s[i] = 0;

      rep(i, 30) h_dists.emplace_back(vector<double>(29, 0));
      rep(i, 29) v_dists.emplace_back(vector<double>(30, 0));

      this->is_m1 = is_m1;
      this->learning_rate = learning_rate;
      this->e_dev_b = e_dev_b;
      this->lower_type_b = lower_type_b;
      this->e_dev_e = e_dev_e;
      this->lower_type_e = lower_type_e;
      this->reg_type = reg_type;
    }

    void clear_all()
    {
      rep(i, T2) hes[i] = hEs[i] = hRs[i] = sig_hes[i] = 0.0;
      rep(i, T2) ves[i] = vEs[i] = vRs[i] = sig_ves[i] = 0.0;
      rep(i, T) sig_hrs[i] = hrs[i] = hr_scales[i] = 0;
      rep(i, T) sig_vrs[i] = vrs[i] = vr_scales[i] = 0;
      rep(i, T) hb0s[i] = hB0s[i] = sig_hb0s[i] = 0;
      rep(i, T) vb0s[i] = vB0s[i] = sig_vb0s[i] = 0;
      rep(i, T) hb1s[i] = hB1s[i] = sig_hb1s[i] = 0;
      rep(i, T) vb1s[i] = vB1s[i] = sig_vb1s[i] = 0;

      sig_d = 0;
      d = 0.0;
    }

    double compute_D() const
    {
      return 1900 * sigmoid(d) + 100;
    }

    static double compute_b(double loc_D, double b)
    {
      return 1000 + loc_D + (8000 - 2 * loc_D) * sigmoid(b);
    }

    double compute_lower_b(double loc_D, double b, double count, double lower_sigmoid_r, int lower_type, double e_dev)
    {
      return 1000 + loc_D + (8000 - 2 * loc_D) * get_lower_sigmoid(b, count, lower_sigmoid_r, lower_type, e_dev);
    }

    vector<vector<double>>& get_h_dists()
    {
      double D = compute_D();
      rep(x, 30){
        double b0 = compute_b(D, hb0s[x]);
        double b1 = compute_b(D, hb1s[x]);
        double threshold = (0.5 + 27 * sigmoid(hrs[x]));
        rep(y, 29){
          double r = sigmoid((y - threshold) * R_SCALE_COEFF * my_exp(hr_scales[x]));
          h_dists[x][y] = (2 * D * sigmoid(hes[idx(x, y)]) - D) + (b0 * r + b1 * (1 - r));
        }
      }
      return h_dists;
    }

    double get_lower_sigmoid(double e, double count, double lower_sigmoid_r, int lower_type, double e_dev)
    {
      double mul = lower_sigmoid_r * e_dev;
      double low = -1;
      if(lower_type == 0) low = (count == 0) ? mul : 0.0;
      else if(lower_type == 1) low = mul / (count + 1);
      else if(lower_type == 2) low = mul / (count + 1) / (count + 1);
      else if(lower_type == 3) low = mul / sqrt(count + 1);
      else if(lower_type == 4) low = (count == 0) ? sigmoid(e) : mul / sqrt(count);
      else if(lower_type == 5) low = (count == 0) ? sigmoid(e) : mul / count;
      else if(lower_type == 6) low = mul / (count + 1) / (count + 1) / (count + 1);
      else if(lower_type == 7) low = (count == 0) ? sigmoid(e) : mul / (count + 1) / (count + 1);
      else if(lower_type == 8) low = (count == 0) ? sigmoid(e) : mul / (count + 1) / (count + 1) / (count + 1);
      else assert(false);
      return max(0.0, sigmoid(e) - low);
    }

    vector<vector<double>>& get_h_dists(vector<vector<int>> h_counts, vector<double> h_lines, double lower_sigmoid_r)
    {
      double D = compute_D();
      rep(x, 30){
        double b0 = compute_lower_b(D, hb0s[x], h_lines[x], lower_sigmoid_r, lower_type_b, e_dev_b);
        double b1 = compute_lower_b(D, hb1s[x], h_lines[x], lower_sigmoid_r, lower_type_b, e_dev_b);
        double threshold = (0.5 + 27 * sigmoid(hrs[x]));
        rep(y, 29){
          double r = sigmoid((y - threshold) * R_SCALE_COEFF * my_exp(hr_scales[x]));
          double e = get_lower_sigmoid(hes[idx(x, y)], h_counts[x][y], lower_sigmoid_r, lower_type_e, e_dev_e);
          h_dists[x][y] = (2 * D * e - D) + (b0 * r + b1 * (1 - r));
        }
      }
      return h_dists;
    }

    vector<vector<double>>& get_v_dists()
    {
      double D = compute_D();
      rep(y, 30){
        double b0 = compute_b(D, vb0s[y]);
        double b1 = compute_b(D, vb1s[y]);
        double threshold = (0.5 + 27 * sigmoid(vrs[y]));
        rep(x, 29){
          double r = sigmoid((x - threshold) * R_SCALE_COEFF * my_exp(vr_scales[y]));
          v_dists[x][y] = (2 * D * sigmoid(ves[idx(x, y)]) - D) + (b0 * r + b1 * (1 - r));
        }
      }
      return v_dists;
    }

    vector<vector<double>>& get_v_dists(vector<vector<int>> v_counts, vector<double> v_lines, double lower_sigmoid_r)
    {
      double D = compute_D();
      rep(y, 30){
        double b0 = compute_lower_b(D, vb0s[y], v_lines[y], lower_sigmoid_r, lower_type_b, e_dev_b);
        double b1 = compute_lower_b(D, vb1s[y], v_lines[y], lower_sigmoid_r, lower_type_b, e_dev_b);
        double threshold = (0.5 + 27 * sigmoid(vrs[y]));
        rep(x, 29){
          double r = sigmoid((x - threshold) * R_SCALE_COEFF * my_exp(vr_scales[y]));
          double e = get_lower_sigmoid(ves[idx(x, y)], v_counts[x][y], lower_sigmoid_r, lower_type_e, e_dev_e);
          v_dists[x][y] = (2 * D * e - D) + (b0 * r + b1 * (1 - r));
        }
      }
      return v_dists;
    }

    static double inv_sig(double sig_v)
    {
      return sig_v * (1 - sig_v);
    }

    void train(vector<QA> &qas)
    {
      sig_d = sigmoid(d);
      double D = 1900 * sig_d + 100;
      rep(y, 30) sig_vrs[y] = sigmoid(vrs[y]);
      rep(x, 30) sig_hrs[x] = sigmoid(hrs[x]);
      rep(y, 30) sig_vb0s[y] = sigmoid(vb0s[y]);
      rep(x, 30) sig_hb0s[x] = sigmoid(hb0s[x]);
      rep(y, 30) sig_vb1s[y] = sigmoid(vb1s[y]);
      rep(x, 30) sig_hb1s[x] = sigmoid(hb1s[x]);
      rep(y, 30) vB0s[y] = 1000 + D + (8000 - 2 * D) * sig_vb0s[y];
      rep(x, 30) hB0s[x] = 1000 + D + (8000 - 2 * D) * sig_hb0s[x];
      rep(y, 30) vB1s[y] = 1000 + D + (8000 - 2 * D) * sig_vb1s[y];
      rep(x, 30) hB1s[x] = 1000 + D + (8000 - 2 * D) * sig_hb1s[x];
      rep(y, 30) exp_vr_scales[y] = my_exp(vr_scales[y]);
      rep(x, 30) exp_hr_scales[x] = my_exp(hr_scales[x]);
      rep(x, 29)rep(y, 30) vRs[idx(x, y)] = sigmoid(R_SCALE_COEFF * exp_vr_scales[y] * (x - (0.5 + 27 * sig_vrs[y])));
      rep(x, 30)rep(y, 29) hRs[idx(x, y)] = sigmoid(R_SCALE_COEFF * exp_hr_scales[x] * (y - (0.5 + 27 * sig_hrs[x])));
      rep(x, 29)rep(y, 30) sig_ves[idx(x, y)] = sigmoid(ves[idx(x, y)]);
      rep(x, 30)rep(y, 29) sig_hes[idx(x, y)] = sigmoid(hes[idx(x, y)]);
      rep(x, 29)rep(y, 30) vEs[idx(x, y)] = (2 * D * sig_ves[idx(x, y)] - D) + (vB0s[y] * vRs[idx(x, y)] + vB1s[y] * (1 - vRs[idx(x, y)]));
      rep(x, 30)rep(y, 29) hEs[idx(x, y)] = (2 * D * sig_hes[idx(x, y)] - D) + (hB0s[x] * hRs[idx(x, y)] + hB1s[x] * (1 - hRs[idx(x, y)]));

      double grad_hrs[T];
      double grad_vrs[T];
      double grad_hr_scales[T];
      double grad_vr_scales[T];
      double grad_hb0s[T];
      double grad_vb0s[T];
      double grad_hb1s[T];
      double grad_vb1s[T];
      rep(i, T) grad_hrs[i] = grad_hr_scales[i] = grad_hb0s[i] = grad_hb1s[i] = 0;
      rep(i, T) grad_vrs[i] = grad_vr_scales[i] = grad_vb0s[i] = grad_vb1s[i] = 0;
      rep(i, T2) grad_hes[i] = grad_ves[i] = 0;

      if(debug){
        vector<vector<double>> vEs_valid = get_v_dists();
        vector<vector<double>> hEs_valid = get_h_dists();
        rep(x, 29)rep(y, 30) assert(abs(vEs[idx(x, y)] - vEs_valid[x][y]) < 1e-3);
        rep(x, 30)rep(y, 29) assert(abs(hEs[idx(x, y)] - hEs_valid[x][y]) < 1e-3);
      }

      double grad_d = 0.0;
      for(QA &qa: qas){
        double prediction = 0.0;
        {
          int x = qa.q.sx;
          int y = qa.q.sy;

          for (int dir: qa.dirs) {
            if (dir == 0) prediction += vEs[idx(x++, y)];
            else if (dir == 1) prediction += hEs[idx(x, y++)];
            else if (dir == 2) prediction += vEs[idx(--x, y)];
            else prediction += hEs[idx(x, --y)];
          }
        }

        {
          int x = qa.q.sx;
          int y = qa.q.sy;

          int prev_is_v = -1;
          double B0_minus_B1;
          double sig_b0_8000_minus_2D;
          double sig_b1_8000_minus_2D;
          double sig_b0_1_minus_2;
          double sig_b1_1_minus_2;
          double inv_sig_r_neg_27;
          double exp_r_scale;
          double inv_sig_d_1900 = 1900 * inv_sig(sig_d);
          double *grad_b0s = nullptr;
          double *grad_b1s = nullptr;
          double *grad_rs = nullptr;
          double *grad_r_scales = nullptr;

          double l_der_E = 2 * ((prediction - qa.score) / qa.score) * (1.0 / qa.score);
          double prediction_validation = 0.0;
          for (int dir: qa.dirs){
            if(dir == 2) --x;
            if(dir == 3) --y;

            int is_v = (dir == 0) || (dir == 2);
            double *sig_es = is_v ? sig_ves : sig_hes;
            double *Rs = is_v ? vRs : hRs;
            double *grad_es = is_v ? grad_ves : grad_hes;
            double *r_scales = is_v ? vr_scales : hr_scales;
            int key_i = is_v ? y : x;
            int xy = idx(x, y);
            double sig_r = (is_v ? sig_vrs : sig_hrs)[key_i];
            double r_inner = ((is_v ? x : y) - (0.5 + 27 * sig_r));
            if(prev_is_v != is_v) {
              prev_is_v = is_v;
              double B0 = (is_v ? vB0s : hB0s)[key_i];
              double B1 = (is_v ? vB1s : hB1s)[key_i];
              B0_minus_B1 = B0 - B1;
              double sig_b0 = (is_v ? sig_vb0s : sig_hb0s)[key_i];
              double sig_b1 = (is_v ? sig_vb1s : sig_hb1s)[key_i];
              sig_b0_8000_minus_2D = (8000 - 2 * D) * inv_sig(sig_b0);
              sig_b1_8000_minus_2D = (8000 - 2 * D) * inv_sig(sig_b1);
              sig_b0_1_minus_2 = 1 - 2 * sig_b0;
              sig_b1_1_minus_2 = 1 - 2 * sig_b1;
              exp_r_scale = my_exp(r_scales[key_i]);
              inv_sig_r_neg_27 = inv_sig(sig_r) * -27 * R_SCALE_COEFF * exp_r_scale;
              grad_b0s = (is_v ? grad_vb0s : grad_hb0s) + key_i;
              grad_b1s = (is_v ? grad_vb1s : grad_hb1s) + key_i;
              grad_rs = (is_v ? grad_vrs : grad_hrs) + key_i;
              grad_r_scales = (is_v ? grad_vr_scales : grad_hr_scales) + key_i;
            }

            double l_der_e = l_der_E * 2 * D * inv_sig(sig_es[xy]);
            double l_der_R = l_der_E * B0_minus_B1;
            double l_der_r = l_der_R * inv_sig(Rs[xy]) * inv_sig_r_neg_27;
            double l_der_r_scale = l_der_R * inv_sig(Rs[xy]) * R_SCALE_COEFF * exp_r_scale * r_inner;
            double l_der_B0 = l_der_E * Rs[xy];
            double l_der_B1 = l_der_E * (1 - Rs[xy]);
            double l_der_b0 = l_der_B0 * sig_b0_8000_minus_2D;
            double l_der_b1 = l_der_B1 * sig_b1_8000_minus_2D;
            double l_der_D = l_der_E * (
                    (2 * sig_es[xy] - 1)
                    + Rs[xy] * sig_b0_1_minus_2
                    + (1 - Rs[xy]) * sig_b1_1_minus_2);
            double l_der_d = l_der_D * inv_sig_d_1900;

            grad_es[xy] += l_der_e;
            *grad_rs += l_der_r;
            *grad_r_scales += l_der_r_scale;
            *grad_b0s += l_der_b0;
            *grad_b1s += l_der_b1;
            grad_d += l_der_d;

            if (dir == 0 || dir == 2) prediction_validation += vEs[xy];
            else prediction_validation += hEs[xy];

            if(dir == 0) ++x;
            if(dir == 1) ++y;
          }
          // assert(abs(prediction - prediction_validation) < 1e-3);
        }
      }

      double mul = learning_rate / sz(qas);



      /*
      if(reg_type == 0 || reg_type == 1 || reg_type == 2 || reg_type == 3) {

        double reg = 0.25 / 2.0;
        if(reg_type == 1) reg = 0.25;
        if(reg_type == 2) reg = 0.25 / 3.0;
        if(reg_type == 3) reg = 0.25 * 3.0;

        reg *= learning_rate;
        rep(x, 30) {
          double m1 = 0.0;
          double m20 = 0.0;
          double m21 = 0.0;
          rep(y, 29) {
            double s = sig_hes[idx(x, y)];
            m1 += s;
            m20 += s * s;
            m21 += (1 - s) * (1 - s);
          }
          m1 /= 29.0;
          m20 /= 29.0;
          m21 /= 29.0;
          rep(y, 29) {
            int id = idx(x, y);
            double s = sig_hes[idx(x, y)];
            double inv = sigmoid_inv(s);
            grad_hes[id] += reg * 2 * (m1 - 0.5) * (inv / 29.0);
            grad_hes[id] += reg * 2 * (m20 - 1 / 3.0) * (2 * s * inv / 29.0);
            grad_hes[id] += reg * 2 * (m21 - 1 / 3.0) * (2 * (s - 1) * inv / 29.0);
          }
        }

        rep(y, 30) {
          double m1 = 0.0;
          double m20 = 0.0;
          double m21 = 0.0;
          rep(x, 29) {
            double s = sig_ves[idx(x, y)];
            m1 += s;
            m20 += s * s;
            m21 += (1 - s) * (1 - s);
          }
          m1 /= 29.0;
          m20 /= 29.0;
          m21 /= 29.0;
          rep(x, 29) {
            int id = idx(x, y);
            double s = sig_ves[idx(x, y)];
            double inv = sigmoid_inv(s);
            grad_ves[id] += reg * 2 * (m1 - 0.5) * (inv / 29.0);
            grad_ves[id] += reg * 2 * (m20 - 1 / 3.0) * (2 * s * inv / 29.0);
            grad_ves[id] += reg * 2 * (m21 - 1 / 3.0) * (2 * (s - 1) * inv / 29.0);
          }
        }
      }
       */


      rep(i, T2) hes[i] -= grad_hes[i] * mul;
      rep(i, T2) ves[i] -= grad_ves[i] * mul;
      rep(x, 30) hrs[x] -= grad_hrs[x] * mul;
      rep(y, 30) vrs[y] -= grad_vrs[y] * mul;
      rep(x, 30) hr_scales[x] -= grad_hr_scales[x] * mul;
      rep(y, 30) vr_scales[y] -= grad_vr_scales[y] * mul;
      rep(x, 30) hb0s[x] -= (is_m1 ? (grad_hb0s[x] + grad_hb1s[x]) : grad_hb0s[x]) * mul;
      rep(y, 30) vb0s[y] -= (is_m1 ? (grad_vb0s[y] + grad_vb1s[y]) : grad_vb0s[y]) * mul;
      rep(x, 30) hb1s[x] -= (is_m1 ? (grad_hb0s[x] + grad_hb1s[x]) : grad_hb1s[x]) * mul;
      rep(y, 30) vb1s[y] -= (is_m1 ? (grad_vb0s[y] + grad_vb1s[y]) : grad_vb1s[y]) * mul;
      d -= grad_d * mul;

      ++epoch;
    }

    void add_val_loss(double prediction, double score)
    {
      double loss = pow(abs(prediction - score) / score, 2);
      moving_avg_val_loss = moving_avg_val_loss * moving_r + loss * (1 - moving_r);
    }

    double moving_avg_val_loss = 0.0;
    double moving_r = 0.99;

    int epoch = 0;
};

/*
class Evaluator
{
public:
    Evaluator(int n, Env *env)
    {
      this->env = env;
      rep(i, n) qs.push_back(env->get_question());
    }

    double evaluate(NnModel &model, PathSolver &solver)
    {
      if(sz(qs) == 0) return 1.0;

      vector<vector<double>> h_dists = model.get_h_dists();
      vector<vector<double>> v_dists = model.get_v_dists();
      double total = 0.0;
      for(Q &q: qs){
        vector<int> dirs = solver.find_shortest(h_dists, v_dists, q);
        total += env->compute_relative_score(q, dirs);
      }
      return total / sz(qs);
    }

private:
    Env *env;
    vector<Q> qs;
};
 */

double compute_leaderboard_score(double score)
{
  return score / (973998783.69 / 2312311.0);
}

double compute_path_dist(vector<vector<double>> &h_dists, vector<vector<double>> &v_dists, Q &q, vector<int> &dirs)
{
  double total = 0.0;
  int x = q.sx;
  int y = q.sy;
  for(int dir: dirs){
    if(dir == 0){
      total += v_dists[x][y];
      ++x;
    }else if(dir == 1){
      total += h_dists[x][y];
      ++y;
    }else if(dir == 2){
      --x;
      total += v_dists[x][y];
    }else{
      --y;
      total += h_dists[x][y];
    }
  }
  assert(x == q.tx && y == q.ty);
  return total;
}

void add_loss(vector<NnModel> &models, QA &qa)
{
  for(NnModel &model: models) {
    double dist = compute_path_dist(model.get_h_dists(), model.get_v_dists(), qa.q, qa.dirs);
    model.add_val_loss(dist, qa.score);
  }
}

vector<QA> create_batch(vector<QA> &qas, vector<int> &indices, int batch, int must_index, XorShift &xor_shift)
{
  vector<QA> res;
  rep(i, sz(indices)) swap(indices[i], indices[xor_shift.rand(i + 1)]);

  bool used = false;
  rep(i, batch){
    int index = indices[i % sz(indices)];
    if(index == must_index) used = true;
    res.push_back(qas[index]);
  }
  if(!used && 0 <= must_index) res.push_back(qas[must_index]);
  return res;
}

/*
vector<int> find_edge_path(Q q, bool is_h, XorShift &xor_shift)
{
  if(is_h){
    if(q.sx == q.tx) return vector<int>();
    vector<int> dirs;
    int x = q.sx;
    int y = q.sy;
    if(xor_shift.rand(2) == 0){
      for(; 0 < y; --y) dirs.push_back(3);
      for(; x < q.tx; ++x) dirs.push_back(0);
      for(; x > q.tx; --x) dirs.push_back(2);
      for(; y < q.ty; ++y) dirs.push_back(1);
      return dirs;
    }else{
      for(; y < 29; ++y) dirs.push_back(1);
      for(; x < q.tx; ++x) dirs.push_back(0);
      for(; x > q.tx; --x) dirs.push_back(2);
      for(; y > q.ty; --y) dirs.push_back(3);
      return dirs;
    }
  }else{
    if(q.sy == q.ty) return vector<int>();
    vector<int> dirs;
    int x = q.sx;
    int y = q.sy;
    if(xor_shift.rand(2) == 0){
      for(; 0 < x; --x) dirs.push_back(2);
      for(; y < q.ty; ++y) dirs.push_back(1);
      for(; y > q.ty; --y) dirs.push_back(3);
      for(; x < q.tx; ++x) dirs.push_back(0);
      return dirs;
    }else{
      for(; x < 29; ++x) dirs.push_back(0);
      for(; y < q.ty; ++y) dirs.push_back(1);
      for(; y > q.ty; --y) dirs.push_back(3);
      for(; x > q.tx; --x) dirs.push_back(2);
      return dirs;
    }
  }
}
 */

double run(Env *env, bool is_local, int min_epochs, int batch, double lr, int n_epoch, double e_dev_e, double e_dev_b, int count_begin, int count_end)
{
  XorShift xor_shift;
  int lower_type_e = 2;
  int lower_type_b = 2;

  double total_score = 0.0;
  unordered_set<int> retrain_steps = {0, 5, 10, 15, 20, 25, 30, 40, 50, 60, 80, 100, 150, 200, 300, 500, 600, 700, 800, 900};

  PathSolver path_solver;
  vector<QA> qas;

  VisitCounter counter;

  vector<NnModel> models = {
          NnModel(true, lr, e_dev_e, lower_type_e, e_dev_b, lower_type_b, -1),
          NnModel(false, lr, e_dev_e, lower_type_e, e_dev_b, lower_type_b, -1)
  };
  vector<int> batch_indices;

  vector<int> model_counts(sz(models), 0);
  rep(step, 1000){
    if(step == count_end){
      DEBUG(model_counts);
      if(model_counts[0] < model_counts[1] * 3) swap(models[0], models[1]);
      while(1 < sz(models)) models.pop_back();
    }

    Q q = env->get_question();

    vector<int> dirs;
    {
      int model_i = 0;
      rep(i, sz(models))if(models[i].moving_avg_val_loss < models[model_i].moving_avg_val_loss) model_i = i;
      if(count_begin <= step) ++model_counts[model_i];

      NnModel &model = models[model_i];
      vector<vector<double>> h_dists = model.get_h_dists(counter.h_counts_i, counter.h_lines_d, 1.0 / pow(0.998, 1000 - step));
      vector<vector<double>> v_dists = model.get_v_dists(counter.v_counts_i, counter.v_lines_d, 1.0 / pow(0.998, 1000 - step));
      dirs = path_solver.find_shortest(h_dists, v_dists, q, 0.9999);
    }

    int score = env->compute_score(q, dirs);
    QA qa = QA{q, dirs, score};
    batch_indices.push_back(sz(qas));
    qas.push_back(qa);
    counter.travel(qa);
    add_loss(models, qa);

    total_score = total_score * 0.998 + env->compute_relative_score(q, dirs);

    bool retrain = retrain_steps.count(step);
    for(NnModel &model: models) {
      if (retrain) {
        int max_pass = max(n_epoch * step, min_epochs);

        model.clear_all();
        rep(epoch, max_pass) {
          vector<QA> v = create_batch(qas, batch_indices, batch, -1, xor_shift);
          model.train(v);
        }
      } else {
        rep(epoch, n_epoch) {
          vector<QA> v = create_batch(qas, batch_indices, batch, sz(qas) - 1 - epoch, xor_shift);
          model.train(v);
        }
      }
    }
  }
  if(false){
    NnModel &m = models[0];
    LocalEnv *e = (LocalEnv*) env;
    DEBUG(e->init_d, m.compute_D());
    rep(i, 30) DEBUG(e->v_bases1[i], m.compute_b(m.compute_D(), m.vb0s[i]), e->v_bases1[i] - m.compute_b(m.compute_D(), m.vb0s[i]));
    rep(i, 30) DEBUG(e->v_bases0[i], m.compute_b(m.compute_D(), m.vb1s[i]), e->v_bases0[i] - m.compute_b(m.compute_D(), m.vb1s[i]));
    rep(i, 30) DEBUG(e->h_bases1[i], m.compute_b(m.compute_D(), m.hb0s[i]), e->h_bases1[i] - m.compute_b(m.compute_D(), m.hb0s[i]));
    rep(i, 30) DEBUG(e->h_bases0[i], m.compute_b(m.compute_D(), m.hb1s[i]), e->h_bases0[i] - m.compute_b(m.compute_D(), m.hb1s[i]));
    rep(i, 30) DEBUG(e->v_thresholds[i], (0.5 + 27 * sigmoid(m.vrs[i])), e->v_thresholds[i] - (0.5 + 27 * sigmoid(m.vrs[i])));
    rep(i, 30) DEBUG(e->h_thresholds[i], (0.5 + 27 * sigmoid(m.hrs[i])), e->h_thresholds[i] - (0.5 + 27 * sigmoid(m.hrs[i])));
  }
  // DEBUG(model_counts);
  return compute_leaderboard_score(total_score);
}

class RunResult
{
public:
    int min_epochs;
    int batch;
    double lr;
    double score;
    int epoch;
    double e_dev_e;
    double e_dev_b;
    int count_begin;
    int count_end;
};

int main()
{
  init_sigmoid();
  vector<RunResult> results;

  {
    int batch = 64;
    double lr = 40;
    int min_epochs = 50;
    int epoch = 4;
    double e_dev_e = 0.1;
    double e_dev_b = 0.03;
    int count_begin = 99;
    int count_end = 500;

#ifndef KOMAKI_LOCAL
    if (true) {
      RustEnv env;
      run(&env, false, min_epochs, batch, lr, epoch, e_dev_e, e_dev_b, count_begin, count_end);
      return 0;
    }
#endif

/*
    for (int batch_overwrite: {64}) {
      for (double lr_overwrite: {40}) {
        for (int min_epochs_overwrite: {50}) {
          for (int epoch_overwrite: {4}) {
            for (int count_begin_overwrite: {99}) {
              for (int count_end_overwrite: {500}) {
                for(double e_dev_e_overwrite: {0.05, 0.1}) {
                  for (double e_dev_b_overwrite: {0.12, 0.03}) {
                    if (true) {
                      batch = batch_overwrite;
                      lr = lr_overwrite;
                      min_epochs = min_epochs_overwrite;
                      epoch = epoch_overwrite;
                      count_begin = count_begin_overwrite;
                      count_end = count_end_overwrite;
                      e_dev_e = e_dev_e_overwrite;
                      e_dev_b = e_dev_b_overwrite;
                    }
                    */
                    results.push_back(RunResult{
                            min_epochs,
                            batch,
                            lr,
                            -1,
                            epoch,
                            e_dev_e,
                            e_dev_b,
                            count_begin,
                            count_end,
                    });
                    /*
                  }
                }
              }
            }
          }
        }
      }
    }
                     */
  }

#pragma omp parallel for
  for (int result_i = 0; result_i < sz(results); ++result_i) {
    RunResult &r = results[result_i];
    double score = 0;
    rep(i, 10) {
      LocalEnv env(i + 235324);
      double s = run(&env, true, r.min_epochs, r.batch, r.lr, r.epoch, r.e_dev_e, r.e_dev_b, r.count_begin, r.count_end);
      score = (score * i + s) / (i + 1);
      DEBUG(i, score);
    }
    r.score = score;
    DEBUG(r.score, r.min_epochs, r.batch, r.lr, r.epoch, r.e_dev_e, r.e_dev_b, r.count_begin, r.count_end);
  }

  sort(results.begin(), results.end(), [&](RunResult &r0, RunResult &r1) { return r0.score < r1.score; });
  for (RunResult r: results) {
    DEBUG(r.score, r.min_epochs, r.batch, r.lr, r.epoch, r.e_dev_e, r.e_dev_b);
  }
}







