


namespace atcoder {}

#ifdef LOCAL
#define dbg(x) cerr << __LINE__ << " : " << #x << " = " << (x) << endl;
#else
#define NDEBUG
#define dbg(x) true;
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#endif

#ifdef GTEST
#include <gtest/gtest.h>
#endif

#include <math.h>

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#ifdef PERF
#include <gperftools/profiler.h>
#endif

using namespace std;
using namespace atcoder;
#define fast_io                     \
  ios_base::sync_with_stdio(false); \
  cin.tie(0);                       \
  cout.tie(0);
#define ll long long int
#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#define reps(i, n) for (int i = 1; i <= (int)(n); i++)
#define REP(i, n) for (int i = n - 1; i >= 0; i--)
#define REPS(i, n) for (int i = n; i > 0; i--)
// #define MOD (long long int)(1e9 + 7)
#define INF (int)(1e9)
#define LINF (long long int)(1e18)
#define all(v) v.begin(), v.end()
typedef pair<int, int> Pii;
typedef pair<ll, ll> Pll;
const double PI = acos(-1);

#ifdef NDEBUG
#define CHECK(v1, op, v2)
#else
#define CHECK(v1, op, v2)                            \
  if (!((v1)op(v2))) {                               \
    cerr << "ERROR:" << (v1) << " " << (v2) << endl; \
    assert((v1)op(v2));                              \
  }
#endif

long double nCr(const int n, const int r) {
  long double ret = 1;
  rep(t, r) {
    ret *= (n - t);
    ret /= (r - t);
  }
  return ret;
}

template <typename T>
string to_string(const vector<T>& vec) {
  string ret = "";
  rep(i, vec.size()) {
    ret += vec[i].to_string();
    if (i + 1 != vec.size()) {
      ret += ",";
    }
  }
  return ret;
}

template <typename T>
ostream& operator<<(ostream& os, const vector<T>& vec) {
  os << to_string(vec);
  return os;
}

uint32_t xorshift() {
  static uint32_t x = 12345789;
  static uint32_t y = 362436069;
  static uint32_t z = 521288629;
  static uint32_t w = 38675123;
  uint32_t t;
  t = x ^ (x << 11);
  x = y;
  y = z;
  z = w;
  w ^= t ^ (t >> 8) ^ (w >> 19);

  return w;
}

int rand(const int l, const int r) {
  return xorshift() % (r - l) + l;
}

int rand_other_than(const int l, const int r,
                         const int other) {
  const int num = rand(l, r - 1);
  return num + (num >= other);
}

template <typename T>
const T& rand_vec(const vector<T>& vec) {
  assert(vec.size() > 0);
  return vec[rand(0, vec.size())];
}

template <typename T>
void shuffle(vector<T>& vec) {
  rep(l, (int)vec.size() - 1) {
    const int idx = rand(l, vec.size());
    swap(vec[idx], vec[l]);
  }
}

template <class T, class U = T>
bool chmin(T& x, U&& y) {
  return y < x && (x = std::forward<U>(y), true);
}

template <class T, class U = T>
bool chmax(T& x, U&& y) {
  return x < y && (x = std::forward<U>(y), true);
}

template <typename Ret, typename T>
Ret Sum(const vector<T>& vec) {
  return std::accumulate(all(vec), (Ret)0);
}

template <typename Ret, typename T>
Ret Mean(const vector<T>& vec) {
  assert((int)vec.size() > 0);
  return Sum<T, Ret>(vec) / vec.size();
}

template <typename Ret, typename T>
Ret Std(const vector<T>& vec) {
  assert((int)vec.size() > 0);
  const auto mean =  Mean<Ret>(vec);
  const auto sum2 = std::accumulate(
      all(vec), (Ret)0,
      [](const Ret acc, const T val) { return acc + (Ret)val * val; });
  return (Ret)sum2 / vec.size() - mean * mean;
}

template<typename T, typename U>
T Ceil(const T a, const U b){
  assert(a >= 0);
  assert(b > 0);
  return (a + b - 1) / b;
}

template<typename T, typename U>
T Mod(const T a, const U b){
  return (a + b) % b;
}

class Timer {
  chrono::system_clock::time_point _start, _end;
  ll _sum = 0, _count = 0;

 public:
  void start() { _start = chrono::system_clock::now(); }

  void stop() { _end = chrono::system_clock::now(); }

  void add() {
    const chrono::system_clock::time_point now = chrono::system_clock::now();
    _sum += static_cast<double>(
        chrono::duration_cast<chrono::nanoseconds>(now - _start).count());
    _count++;
  }

  ll sum() const { return _sum / 1000; }

  int count() const { return _count; }

  string average() const {
    if (_count == 0) {
      return "NaN";
    }
    return to_string(_sum / 1000 / _count);
  }

  void reset() {
    _start = chrono::system_clock::now();
    _sum = 0;
    _count = 0;
  }

  inline int ms() const {
    const chrono::system_clock::time_point now = chrono::system_clock::now();
    return static_cast<double>(
        chrono::duration_cast<chrono::microseconds>(now - _start).count() /
        1000);
  }

  inline int ns() const {
    const chrono::system_clock::time_point now = chrono::system_clock::now();
    return static_cast<double>(
        chrono::duration_cast<chrono::microseconds>(now - _start).count());
  }
};

#ifdef LOCAL
struct Timers : unordered_map<string, Timer> {
  friend ostream& operator<<(ostream& os, const Timers& timers) {
    for (const auto& pa : timers) {
      os << pa.first << " time: " << pa.second.sum() / 1000
         << " count: " << pa.second.count() << endl;
    }
    return os;
  }
};
#else
struct Timers {
  struct Dummy {
    void start() const {}
    void add() const {}
  };
  Dummy dummy;
  const Dummy& operator[](const std::string& str) { return dummy; }
  friend ostream& operator<<(ostream& os, const Timers& timers) { return os; }
};
#endif

Timers global_timers;




/* start */

struct Pos{
  int x,y;
  Pos(int x,int y):x(x),y(y){}
  Pos(){}
  bool operator==(const Pos& p) const {
    return x == p.x && y == p.y;
  }
  bool operator!=(const Pos& p) const{
    return !(*this == p);
  }
  int Dist(const Pos& p) const {
    return sqrt(pow(x-p.x,2)+pow(y-p.y,2));
  }
};

struct MyRange{
  int lx,rx,ly,ry;
  MyRange(int lx,int rx,int ly,int ry):lx(lx),rx(rx),ly(ly),ry(ry){}
  MyRange(){}
  Pos Sample() const {
    return Pos(rand(lx,rx + 1),rand(ly,ry + 1));
  }
  Pos Clip(const Pos& p) const {
    return Pos(max(lx,min(rx,p.x)),max(ly,min(ry,p.y)));
  }
  ll Area() const {
    return (ll)(rx - lx) * (ry - ly);
  }
  float MaxDist(const MyRange& other) const {
    float maxDist = 0;
    for(const auto p : {Pos(lx,ly),Pos(lx,ry),Pos(rx,ly),Pos(rx,ry)}){
      for(const auto q : {Pos(other.lx,other.ly),Pos(other.lx,other.ry),
                           Pos(other.rx,other.ly),Pos(other.rx,other.ry)}){
        maxDist = max(maxDist, (float)p.Dist(q));
      }
    }
    return maxDist;
  }
  float MinDist(const MyRange& other) const {
    if(lx <= other.rx && other.lx <= rx && ly <= other.ry && other.ly <= ry){
      return 0;
    }
    float minDist = INF;
    for(const auto& p : {Pos(lx,ly),Pos(lx,ry),Pos(rx,ly),Pos(rx,ry)}){
      for(const auto& q : {Pos(other.lx,other.ly),Pos(other.lx,other.ry),
                           Pos(other.rx,other.ly),Pos(other.rx,other.ry)}){
        minDist = min(minDist, (float)p.Dist(q));
      }
    }
    return minDist;
  }
  // 境界にへばりついている場合は、逆の境界に移動する
  Pos WallFlip(Pos p) const{
    int x = p.x;
    int y = p.y;
    if (x == lx) {
      x = rx;
    } else if (x == rx) {
      x = lx;
    }
    if (y == ly) {
      y = ry;
    } else if (y == ry) {
      y = ly;
    }
    return Pos(x, y);
  }
};

struct Edge{
  int u,v;
  int dist;
  Edge(int u,int v, int dist):u(u),v(v),dist(dist){}
  Edge(){}
};
#include <atcoder/dsu>
#include <sstream>

/* start */

Timer global_timer;
int N, M, Q, L, W;
int q = 0;
vector<int> G;
bool useJudge = false;
bool shouldDebug = false;

struct Judge {
  vector<Pos> truePoints;
  void Init(istream &is) {
    rep(i, N) {
      int x, y;
      is >> x >> y;
      truePoints.push_back(Pos(x, y));
    }
  }

  stringstream Query(istream &is) {
    char kind;
    is >> kind;
    if (kind == '?') {
      int l;
      is >> l;
      vector<int> ids(l);
      rep(i, l) { is >> ids[i]; }

      // 最小全域木を構築
      struct Edge {
        int u, v;
        int dist;
        Edge(int u, int v, int dist) : u(min(u, v)), v(max(u, v)), dist(dist) {}
        bool operator<(const Edge &other) const {
          if (dist == other.dist) {
            if (u != other.u)
              return u < other.u;
            return v < other.v;
          }
          return dist < other.dist;
        }
      };

      vector<Edge> edges;
      rep(i, l) {
        rep(j, i) {
          int dist = truePoints[ids[i]].Dist(truePoints[ids[j]]);
          edges.push_back(Edge(ids[i], ids[j], dist));
        }
      }
      sort(edges.begin(), edges.end());
      atcoder::dsu dsu(N);
      vector<Edge> mstEdges;
      for (const auto &edge : edges) {
        if (dsu.same(edge.u, edge.v))
          continue;
        dsu.merge(edge.u, edge.v);
        mstEdges.push_back(edge);
      }

      // (u,v)の順にソートして返す
      vector<pair<int, int>> result;
      for (const auto &edge : mstEdges) {
        result.push_back({edge.u, edge.v});
      }
      sort(result.begin(), result.end());

      stringstream ss;
      for (const auto &edge : result) {
        ss << edge.first << " " << edge.second << endl;
      }
      return ss;
    } else {
      assert(kind == '!');
      ll distSum = 0;
      rep(i, M) {
        int g = G[i];
        rep(j, g) {
          int _;
          is >> _;
        }

        rep(j, g - 1) {
          int a, b;
          is >> a >> b;
          distSum += truePoints[a].Dist(truePoints[b]);
        }
      }

      stringstream ss;
      ss << distSum << endl;
      return ss;
    }
  }
};

Judge judge;

struct CityPair {
  int smallId, bigId;
  CityPair(int a, int b) : smallId(min(a, b)), bigId(max(a, b)) {}
  int Id() const { return smallId * N + bigId; }
};

vector<int> globalG2Counts;
vector<MyRange> centerRanges;
vector<Pos> centers;
vector<vector<int>> id2NearCenterIds;
vector<int> id2RangeAreaRanks;
vector<int> id2QueryCounts;
vector<vector<CityPair>> resultABCDGraph;
vector<float> globalMaxDists;
vector<float> globalMinDists;
vector<int> cityPair2QueryCounts;

// dist(a, b) < dist(c, d) の形式で結果を保持する
struct ResultABCD {
  int a, b, c, d;
  ResultABCD(int a, int b, int c, int d)
      : a(min(a, b)), b(max(a, b)), c(min(c, d)), d(max(c, d)) {}
  ResultABCD() {}
  bool operator<(const ResultABCD &other) const {
    if (a != other.a)
      return a < other.a;
    if (b != other.b)
      return b < other.b;
    if (c != other.c)
      return c < other.c;
    return d < other.d;
  }
};
set<ResultABCD> globalResultABCDs;
vector<ResultABCD> globalResultABCDsVec;

// N個のintからintへのマッピングを格納するための辞書
// 初期化が遅いので使いまわす
vector<int> id2Indexes;

// N要素のbool配列
// 使いまわす用
vector<bool> vectorNBools;

// プログラムの一番最初に呼び出す。入力を受け取ったり、前計算すべき要素の初期化を行ったりする。
void Initialize(istream &is) {
  global_timer.start();
  is >> N >> M >> Q >> L >> W;
  cityPair2QueryCounts.resize(N * N);
  resultABCDGraph.resize(N * N);
  // rep(i, N*N){
  //   resultABCDGraph[i].reserve(N);
  // }
  id2QueryCounts.resize(N);
  G.resize(M);
  globalG2Counts.resize(1000);
  centerRanges.resize(N);
  centers.resize(N);
  for (int i = 0; i < M; i++) {
    int g;
    is >> g;
    G[i] = g;
    globalG2Counts[g]++;
  }
  for (int i = 0; i < N; i++) {
    int lx, rx, ly, ry;
    is >> lx >> rx >> ly >> ry;
    centerRanges[i] = MyRange(lx, rx, ly, ry);
    centers[i] = Pos((lx + rx) / 2, (ly + ry) / 2);
  }
  globalMaxDists.resize(N*N);
  globalMinDists.resize(N*N);
  rep(i, N){
    rep(j, N){
      const auto range_i = centerRanges[i];
      const auto range_j = centerRanges[j];
      globalMaxDists[CityPair(i, j).Id()] = range_i.MaxDist(range_j);
      globalMinDists[CityPair(i, j).Id()] = range_i.MinDist(range_j);
    }
  }

  id2NearCenterIds.resize(N);
  for (int i = 0; i < N; i++) {
    vector<int> nearCenterIds;
    for (int j = 0; j < N; j++) {
      if (i == j)
        continue;
      nearCenterIds.push_back(j);
    }
    sort(nearCenterIds.begin(), nearCenterIds.end(), [&](int a, int b) {
      return centers[i].Dist(centers[a]) < centers[i].Dist(centers[b]);
    });
    id2NearCenterIds[i] = nearCenterIds;
  }

  if (useJudge) {
    judge.Init(is);
  }

  id2Indexes.resize(N);
  vectorNBools.resize(N);

  // rangeの面積の降順にソート
  vector<int> sortedIds(N);
  iota(sortedIds.begin(), sortedIds.end(), 0);
  sort(sortedIds.begin(), sortedIds.end(), [&](int a, int b) {
    return centerRanges[a].Area() > centerRanges[b].Area();
  });
  id2RangeAreaRanks.resize(N);
  for (int i = 0; i < N; i++) {
    id2RangeAreaRanks[sortedIds[i]] = i;
  }
}

int sameQueryCount = 0;

vector<pair<int, int>> Query(vector<int> ids,
                             vector<ResultABCD> *resultABCDsPtr = nullptr) {
  assert(q < Q);

  sort(ids.begin(), ids.end());

  // 既に投げたクエリをメモしておく
  static vector<pair<vector<int>, vector<pair<int, int>>>> pastQueries;
  // 既に投げたクエリと完全に一致しているなら、その結果をそのまま返す
  {
    for (const auto &[queryIds, result] : pastQueries) {
      if (queryIds.size() != ids.size()) {
        continue;
      }
      bool isSame = true;
      for (int i = 0; i < (int)queryIds.size(); i++) {
        if (queryIds[i] != ids[i]) {
          isSame = false;
          break;
        }
      }
      if (isSame) {
        sameQueryCount++;
        return result;
      }
    }
  }

  q++;

  stringstream sout;
  if (useJudge) {
    stringstream sin;
    sin << "? " << ids.size();
    for (int i = 0; i < (int)ids.size(); i++) {
      sin << " " << ids[i];
      id2QueryCounts[ids[i]]++;
    }
    sin << endl;

    cout << sin.str();
    cout.flush();

    sout = judge.Query(sin);
  } else {
    cout << "? " << ids.size();
    for (int i = 0; i < (int)ids.size(); i++) {
      cout << " " << ids[i];
      id2QueryCounts[ids[i]]++;
    }
    cout << endl;
    cout.flush();
    rep(idx, ids.size()) {
      rep(jIdx, idx) {
        int a = ids[idx];
        int b = ids[jIdx];
        cityPair2QueryCounts[CityPair(a, b).Id()]++;
      }
    }
  }

  // 自作Judgeを使う場合は、judge.Query()の結果を受け取る
  auto &is = useJudge ? sout : cin;

  vector<pair<int, int>> ret;
  map<int, vector<int>> resultGraph;
  for (int i = 0; i < (int)ids.size() - 1; i++) {
    int a, b;
    is >> a >> b;
    ret.push_back({a, b});
    resultGraph[a].push_back(b);
    resultGraph[b].push_back(a);
  }
  // 結果をメモしておく
  pastQueries.push_back({ids, ret});

  // 辺i-jが存在する場合、辺i-jを削除してできる2つの連結成分間の全ての辺は、辺i-jよりも長い
  for (const auto [a, b] : ret) {
    // a-bを削除してできる2つの連結成分
    vector<int> componentA;
    vector<int> componentB;
    // current, parent
    rep(t, 2) {
      queue<pair<int, int>> que;
      auto &component = (t == 0 ? componentA : componentB);
      if (t == 0) {
        que.push({a, b});
      } else {
        que.push({b, a});
      }
      while (que.size()) {
        const int current = que.front().first;
        const int parent = que.front().second;
        que.pop();
        component.push_back(current);
        for (const auto next : resultGraph[current]) {
          if (next == parent)
            continue;
          que.push({next, current});
        }
      }
    }
    // componentA~componentBの辺は、a-bよりも長い
    for (const auto id_a : componentA) {
      for (const auto id_b : componentB) {
        if (id_a == a && id_b == b) {
          continue;
        }
        assert(a != b);
        assert(id_a != id_b);
        assert(a != id_a || b != id_b);
        assert(a != id_b || b != id_a);
        // 結果が自明な場合は追加しない
        if(globalMaxDists[CityPair(a, b).Id()] <= globalMinDists[CityPair(id_a, id_b).Id()]){
          continue;
        }
        ResultABCD resultABCD(a, b, id_a, id_b);
        const auto [_, success] = globalResultABCDs.insert(resultABCD);
        if (success) {
          globalResultABCDsVec.push_back(resultABCD);
          resultABCDGraph[CityPair(a, b).Id()].push_back(CityPair(id_a, id_b));
        }
        if (resultABCDsPtr != nullptr) {
          resultABCDsPtr->push_back(resultABCD);
        }
      }
    }
  }
  return ret;
}





/* start */

vector<double> PARAMS = {10, 0.1, 0.1,1.0, 0.1, 3, 1};



/* start */




#include <atcoder/dsu>

/* start */

struct Cluster {
  vector<int> ids;
  vector<Edge> edges;
  int sumDist = 0;

  static vector<vector<int>> estimatedDists;
  // 各IDから近い順にIDを並べたリスト
  static vector<vector<int>> nearIds;

  Cluster() {}
  Cluster(vector<int> ids) : ids(ids) { BuildMST(); }

  Cluster(vector<int> ids, vector<pair<int, int>> _edges) : ids(ids) {
    for (const auto edge : _edges) {
      const int dist = estimatedDists[edge.first][edge.second];
      edges.push_back(Edge(edge.first, edge.second, dist));
      sumDist += dist;
    }
  }

  void ChangeId(int j, int newId) {
    // ids[j]をnewIdに変更する
    ids[j] = newId;
  }

  void BuildMST() {
    sumDist = 0;
    edges.clear();
    if (ids.size() <= 1u) {
      return;
    }
    atcoder::dsu dsu(ids.size());
    vector<Edge> edgeCandidates;
    for (int i = 0; i < (int)ids.size(); i++) {
      id2Indexes[ids[i]] = i;
      for (int j = i + 1; j < (int)ids.size(); j++) {
        int dist = estimatedDists[ids[i]][ids[j]];
        edgeCandidates.push_back(Edge(ids[i], ids[j], dist));
      }
    }
    sort(edgeCandidates.begin(), edgeCandidates.end(),
         [](const Edge &a, const Edge &b) { return a.dist < b.dist; });

    for (const auto &edge : edgeCandidates) {
      const int u = id2Indexes[edge.u];
      const int v = id2Indexes[edge.v];
      if (dsu.same(u, v))
        continue;
      dsu.merge(u, v);
      edges.push_back(edge);
      sumDist += edge.dist;
      if ((int)edges.size() >= (int)ids.size() - 1) {
        break;
      }
    }
    assert((int)edges.size() == (int)ids.size() - 1);
  }

  static void SetEstimatedPoses(const vector<vector<int>> &_estimatedDists) {
    estimatedDists = _estimatedDists;

    nearIds.clear();
    nearIds.resize(N);
    for (int i = 0; i < N; i++) {
      nearIds[i].clear();
      for (int j = 0; j < N; j++) {
        if (i != j) {
          nearIds[i].push_back(j);
        }
      }
      sort(nearIds[i].begin(), nearIds[i].end(), [&](int a, int b) {
        return estimatedDists[i][a] < estimatedDists[i][b];
      });
    }
  }
};

vector<vector<int>> Cluster::estimatedDists;
vector<vector<int>> Cluster::nearIds;

void PrintAnswer(vector<Cluster> clusters) {
  if (useJudge) {
    stringstream sin;
    sin << "!" << endl;
    rep(ci, M) {
      const auto &cluster = clusters[ci];
      assert(cluster.ids.size() > 0u);
      rep(j, cluster.ids.size()) { sin << cluster.ids[j] << " "; }
      sin << endl;
      rep(j, cluster.edges.size()) {
        const auto &edge = cluster.edges[j];
        sin << edge.u << " " << edge.v << endl;
      }
    }

    cout << sin.str();
    cout.flush();

    stringstream sout = judge.Query(sin);
    ll score;
    sout >> score;
    cerr << "score: " << score << endl;
  } else {
    cout << "!" << endl;
    rep(ci, M) {
      const auto &cluster = clusters[ci];
      assert(cluster.ids.size() > 0u);
      rep(j, cluster.ids.size()) { cout << cluster.ids[j] << " "; }
      cout << endl;
      rep(j, cluster.edges.size()) {
        const auto &edge = cluster.edges[j];
        cout << edge.u << " " << edge.v << endl;
      }
    }
    cout.flush();
  }
}

// 指定サイズのクラスタに分割する
vector<Cluster> SplitClusters(const vector<int> &ids,
                              const vector<int> &clusterSizes) {
  assert(clusterSizes.size() >= 2u);
  vector<bool> isUsed(ids.size(), false);
  vector<int> baseIds;
  vector<vector<int>> clusterIds(clusterSizes.size());
  // 各クラスタの基準とするIDを決める
  rep(ci, clusterSizes.size()) {
    if (ci >= 1) {
      // 最初に選んだ点から最も遠い点を選ぶ
      int maxDist = -1;
      int maxIndex = -1;
      for (int i = 0; i < (int)ids.size(); i++) {
        if (isUsed[i]) {
          continue;
        }
        int dist = Cluster::estimatedDists[ids[i]][baseIds[0]];
        if (dist > maxDist) {
          maxDist = dist;
          maxIndex = i;
        }
      }
      assert(maxIndex != -1);
      isUsed[maxIndex] = true;
      baseIds.push_back(ids[maxIndex]);
      clusterIds[ci].push_back(ids[maxIndex]);
    } else {
      while (true) {
        int index = rand(0, ids.size());
        if (isUsed[index]) {
          continue;
        }
        isUsed[index] = true;
        baseIds.push_back(ids[index]);
        clusterIds[ci].push_back(ids[index]);
        break;
      }
    }
  }

  // 残りのIDについて、最も良いクラスタに割り当てた場合と二番目に良いクラスタに割り当てた場合の距離の差を求める
  // diff, index
  vector<pair<int, int>> diffAndIndexes;
  rep(i, ids.size()) {
    if (isUsed[i]) {
      continue;
    }
    int bestDist = INF;
    int secondBestDist = INF;
    for (int ci = 0; ci < (int)clusterSizes.size(); ci++) {
      int dist = Cluster::estimatedDists[ids[i]][baseIds[ci]];
      if (dist < bestDist) {
        secondBestDist = bestDist;
        bestDist = dist;
      } else if (dist < secondBestDist) {
        secondBestDist = dist;
      }
    }
    // このdiffが大きいほど、優先的に割り当てたほうが良い
    const int diff = secondBestDist - bestDist;
    diffAndIndexes.push_back({diff, i});
  }

  // diffの大きい順にソート
  sort(diffAndIndexes.begin(), diffAndIndexes.end(), greater<pair<int, int>>());
  // diffの大きい順にクラスタに割り当てる
  for (const auto [_, index] : diffAndIndexes) {
    int bestCi = -1;
    int bestDist = INF;
    for (int ci = 0; ci < (int)clusterSizes.size(); ci++) {
      if ((int)clusterIds[ci].size() >= clusterSizes[ci]) {
        // これ以上追加できない
        continue;
      }
      int dist = Cluster::estimatedDists[ids[index]][baseIds[ci]];
      if (dist < bestDist) {
        bestDist = dist;
        bestCi = ci;
      }
    }
    assert(bestCi != -1);
    // 一番良いクラスタに割り当てる
    clusterIds[bestCi].push_back(ids[index]);
  }

  // クラスタを作成
  vector<Cluster> clusters;
  for (int ci = 0; ci < (int)clusterIds.size(); ci++) {
    clusters.push_back(Cluster(clusterIds[ci]));
  }
  return clusters;
}










/* start */

class Estimator {
public:
  vector<Pos> estimatedPoses;
  vector<ResultABCD> flippedResultABCDs;
  int g = 0;
  int acc = 0;
  double swapProb = 1.0;
  Estimator(bool random) {
    estimatedPoses.resize(N);
    for (int i = 0; i < N; i++) {
      if (random)
        estimatedPoses[i] = centerRanges[i].Sample();
      else
        estimatedPoses[i] = centers[i];
    }
  }

  void UpdateRandomOne(const double gamma,
                       const vector<ResultABCD> &resultABCDs,
                       const bool priorityFlippedResultABCDs) {
    g++;
    if (priorityFlippedResultABCDs && rand(0,100) < PARAMS[0] &&
        (int)flippedResultABCDs.size() > 0) {
      const int index = rand(0, flippedResultABCDs.size());
      const auto resultABCD = flippedResultABCDs[index];
      const auto flipped = UpdateOne(gamma, resultABCD);
      if (!flipped) {
        swap(flippedResultABCDs[index], flippedResultABCDs.back());
        flippedResultABCDs.pop_back();
        return;
      }
      int count = 0;
      while (count < 5) {
        count++;
        const auto nextFlipped = UpdateOne(gamma, resultABCD);
        if (nextFlipped) {
#if 1
          if (count >= 1000) {
            cerr << count << endl;
            cerr << resultABCD.a << " " << resultABCD.b << " " << resultABCD.c
                 << " " << resultABCD.d << endl;
            cerr << centerRanges[resultABCD.a].lx
                 << " <= " << estimatedPoses[resultABCD.a].x
                 << " <= " << centerRanges[resultABCD.a].rx << ", ";
            cerr << centerRanges[resultABCD.a].ly
                 << " <= " << estimatedPoses[resultABCD.a].y
                 << " <= " << centerRanges[resultABCD.a].ry << endl;
            cerr << centerRanges[resultABCD.b].lx
                 << " <= " << estimatedPoses[resultABCD.b].x
                 << " <= " << centerRanges[resultABCD.b].rx << ", ";
            cerr << centerRanges[resultABCD.b].ly
                 << " <= " << estimatedPoses[resultABCD.b].y
                 << " <= " << centerRanges[resultABCD.b].ry << endl;
            cerr << centerRanges[resultABCD.c].lx
                 << " <= " << estimatedPoses[resultABCD.c].x
                 << " <= " << centerRanges[resultABCD.c].rx << ", ";
            cerr << centerRanges[resultABCD.c].ly
                 << " <= " << estimatedPoses[resultABCD.c].y
                 << " <= " << centerRanges[resultABCD.c].ry << endl;
            cerr << centerRanges[resultABCD.d].lx
                 << " <= " << estimatedPoses[resultABCD.d].x
                 << " <= " << centerRanges[resultABCD.d].rx << ", ";
            cerr << centerRanges[resultABCD.d].ly
                 << " <= " << estimatedPoses[resultABCD.d].y
                 << " <= " << centerRanges[resultABCD.d].ry << endl;
            cerr << estimatedPoses[resultABCD.a].Dist(
                        estimatedPoses[resultABCD.b])
                 << " "
                 << estimatedPoses[resultABCD.c].Dist(
                        estimatedPoses[resultABCD.d])
                 << endl;
          }
#endif
        } else {
          break;
        }
      }
      return;
    }
    assert(resultABCDs.size() > 0u);
    const int index = rand(0, resultABCDs.size());
    const auto &resultABCD = resultABCDs[index];
    const auto flipped = UpdateOne(gamma, resultABCD);
    if (flipped) {
      flippedResultABCDs.push_back(resultABCD);
      int count = 0;
      while (count < 5) {
        count++;
        const auto nextFlipped = UpdateOne(gamma, resultABCD);
        if (nextFlipped) {
#if 1
          if (count >= 1000) {
            cerr << count << endl;
            cerr << resultABCD.a << " " << resultABCD.b << " " << resultABCD.c
                 << " " << resultABCD.d << endl;
            cerr << centerRanges[resultABCD.a].lx
                 << " <= " << estimatedPoses[resultABCD.a].x
                 << " <= " << centerRanges[resultABCD.a].rx << ", ";
            cerr << centerRanges[resultABCD.a].ly
                 << " <= " << estimatedPoses[resultABCD.a].y
                 << " <= " << centerRanges[resultABCD.a].ry << endl;
            cerr << centerRanges[resultABCD.b].lx
                 << " <= " << estimatedPoses[resultABCD.b].x
                 << " <= " << centerRanges[resultABCD.b].rx << ", ";
            cerr << centerRanges[resultABCD.b].ly
                 << " <= " << estimatedPoses[resultABCD.b].y
                 << " <= " << centerRanges[resultABCD.b].ry << endl;
            cerr << centerRanges[resultABCD.c].lx
                 << " <= " << estimatedPoses[resultABCD.c].x
                 << " <= " << centerRanges[resultABCD.c].rx << ", ";
            cerr << centerRanges[resultABCD.c].ly
                 << " <= " << estimatedPoses[resultABCD.c].y
                 << " <= " << centerRanges[resultABCD.c].ry << endl;
            cerr << centerRanges[resultABCD.d].lx
                 << " <= " << estimatedPoses[resultABCD.d].x
                 << " <= " << centerRanges[resultABCD.d].rx << ", ";
            cerr << centerRanges[resultABCD.d].ly
                 << " <= " << estimatedPoses[resultABCD.d].y
                 << " <= " << centerRanges[resultABCD.d].ry << endl;
            cerr << estimatedPoses[resultABCD.a].Dist(
                        estimatedPoses[resultABCD.b])
                 << " "
                 << estimatedPoses[resultABCD.c].Dist(
                        estimatedPoses[resultABCD.d])
                 << endl;
          }
#endif
        } else {
          break;
        }
      }
    }
  }

  void DebugFlipCount() {
    int flipCount = 0;
    for (const auto [a, b, c, d] : globalResultABCDs) {
      const Pos pos_a = estimatedPoses[a];
      const Pos pos_b = estimatedPoses[b];
      const Pos pos_c = estimatedPoses[c];
      const Pos pos_d = estimatedPoses[d];
      const int dist_ab = pos_a.Dist(pos_b);
      const int dist_cd = pos_c.Dist(pos_d);
      if (!IsCorrect(dist_ab, dist_cd)) {
        flipCount++;
      }
    }
    cerr << "flipCount: " << flipCount << " / " << globalResultABCDs.size()
         << " " << (float)flipCount / globalResultABCDs.size() * 100 << "%"
         << endl;
  }

  // 各IDについて、逆転している結果の個数を返す
  vector<int> FlipCounts() {
    vector<int> flipCounts(N, 0);
    for (const auto [a, b, c, d] : globalResultABCDs) {
      const Pos pos_a = estimatedPoses[a];
      const Pos pos_b = estimatedPoses[b];
      const Pos pos_c = estimatedPoses[c];
      const Pos pos_d = estimatedPoses[d];
      const int dist_ab = pos_a.Dist(pos_b);
      const int dist_cd = pos_c.Dist(pos_d);
      if (!IsCorrect(dist_ab, dist_cd)) {
        // 同じ頂点を2回足している場合があるが、仕様としている
        flipCounts[a]++;
        flipCounts[b]++;
        flipCounts[c]++;
        flipCounts[d]++;
      }
    }
    return flipCounts;
  }

private:
  bool UpdateOne(const double gamma, const ResultABCD &resultABCD,
                 const float specifiedSwapProb = -1.0f) {
    const float swapProb =
        specifiedSwapProb == -1.0f ? this->swapProb : specifiedSwapProb;
    const auto [a, b, c, d] = resultABCD;
    vector<int> ids = {a, b, c, d};
    vector<float> deltaXs(4);
    vector<float> deltaYs(4);
    Pos pos_a = estimatedPoses[a];
    Pos pos_b = estimatedPoses[b];
    Pos pos_c = estimatedPoses[c];
    Pos pos_d = estimatedPoses[d];
    int dist_ab = pos_a.Dist(pos_b);
    int dist_cd = pos_c.Dist(pos_d);
    if (IsCorrect(dist_ab, dist_cd)) {
      return false;
    }
    acc++;

    if (true && (a == c || a == d || b == c || b == d)) {
      // (i,j) < (i,k)
      int i = -1;
      int j = -1;
      int k = -1;
      if (a == c) {
        i = a;
        j = b;
        k = d;
      } else if (a == d) {
        i = a;
        j = b;
        k = c;
      } else if (b == c) {
        i = b;
        j = a;
        k = d;
      } else {
        i = b;
        j = a;
        k = c;
      }

      // iはjに近づきたくて、kから遠ざかりたい
      // jはiに近づきたい
      // kはiから遠ざかりたい
      Pos pos_i = estimatedPoses[i];
      Pos pos_j = estimatedPoses[j];
      Pos pos_k = estimatedPoses[k];

      // iを動かしてから、j,kを動かす
      // まず、iを動かす量を求める
      float distSqrtratio = (float)dist_ab / dist_cd;
      float dx_i = (pos_i.x - pos_j.x) / distSqrtratio + pos_j.x - pos_i.x;
      float dy_i = (pos_i.y - pos_j.y) / distSqrtratio + pos_j.y - pos_i.y;
      dx_i += (pos_i.x - pos_k.x) * distSqrtratio + pos_k.x - pos_i.x;
      dy_i += (pos_i.y - pos_k.y) * distSqrtratio + pos_k.y - pos_i.y;

      // iを動かす
      pos_i.x += std::round(dx_i * gamma);
      pos_i.y += std::round(dy_i * gamma);
      pos_i = centerRanges[i].Clip(pos_i);

      estimatedPoses[i] = pos_i;

#if 1
      // 改めて距離を算出する
      auto dist_ij = pos_i.Dist(pos_j);
      auto dist_ik = pos_i.Dist(pos_k);
      // cerr << distSqrtratio << " ";
      distSqrtratio = (float)dist_ij / dist_ik;
      // cerr << distSqrtratio << endl;

      if (IsCorrect(dist_ij, dist_ik)) {
        // 解消されたら終わり
        return true;
      }

      // j,kの移動後のiからの距離を決める
      float target_dist_ij = dist_ij / distSqrtratio * gamma * 1.0;
      float target_dist_ik = dist_ik * distSqrtratio * gamma * 1.0;
      if (target_dist_ij + 20 > target_dist_ik) {
        target_dist_ij = target_dist_ik - 10;
        target_dist_ik = target_dist_ij + 20;
      }

      // 動かす
      pos_j = MovePos(pos_j, pos_i, target_dist_ij, centerRanges[j]);
      pos_k = MovePos(pos_k, pos_i, target_dist_ik, centerRanges[k]);
      // cerr << pos_i.Dist(pos_j) << " "
      //      << pos_i.Dist(pos_k) << endl;
      estimatedPoses[j] = pos_j;
      estimatedPoses[k] = pos_k;

      dist_ij = pos_i.Dist(pos_j);
      dist_ik = pos_i.Dist(pos_k);
      if (!IsCorrect(dist_ij, dist_ik) && rand(0, (int)1e7) < swapProb * 1e7) {
        // 解決しない場合は、スワップする
        if ((pos_i.x < pos_k.x) == (pos_k.x < pos_j.x)) {
          int randomValue = rand(0, 2);
          if (randomValue == 0) {
            swap(pos_i.x, pos_k.x);
            if (pos_i.x == pos_k.x) {
              const int diff = 1 - 2 * rand(0, 2);
              pos_i.x -= diff;
              pos_k.x += diff;
            }
          } else if (randomValue == 1) {
            swap(pos_j.x, pos_k.x);
            if (pos_j.x == pos_k.x) {
              const int diff = 1 - 2 * rand(0, 2);
              pos_j.x -= diff;
              pos_k.x += diff;
            }
          }
        } else if (pos_k.x == centerRanges[k].lx ||
                   pos_k.x == centerRanges[k].rx) {
          // kが範囲の端にいる場合は、iとスワップする
          swap(pos_i.x, pos_k.x);
        }
        if ((pos_i.y < pos_k.y) == (pos_k.y < pos_j.y)) {
          int randomValue = rand(0, 2);
          if (randomValue == 0) {
            swap(pos_i.y, pos_k.y);
            if (pos_i.y == pos_k.y) {
              const int diff = 1 - 2 * rand(0, 2);
              pos_i.y -= diff;
              pos_k.y += diff;
            }
          } else if (randomValue == 1) {
            swap(pos_j.y, pos_k.y);
            if (pos_j.y == pos_k.y) {
              const int diff = 1 - 2 * rand(0, 2);
              pos_j.y -= diff;
              pos_k.y += diff;
            }
          }
        } else if (pos_k.y == centerRanges[k].ly ||
                   pos_k.y == centerRanges[k].ry) {
          // kが範囲の端にいる場合は、iとスワップする
          swap(pos_i.y, pos_k.y);
        }
        pos_i = centerRanges[i].Clip(pos_i);
        estimatedPoses[i] = pos_i;
        pos_j = centerRanges[j].Clip(pos_j);
        estimatedPoses[j] = pos_j;
        pos_k = centerRanges[k].Clip(pos_k);
        estimatedPoses[k] = pos_k;
        return true;
      }
      // cerr << (float)pos_i.Dist(pos_j) / pos_i.Dist(pos_k) << endl;
#endif
    } else {
      // const float distSqrtRatio = sqrt((float)dist_ab / dist_cd);
      const float distSqrtRatio = (float)dist_ab / dist_cd;
      const float dx_a =
          (pos_a.x - pos_b.x) / distSqrtRatio + pos_b.x - pos_a.x;
      const float dy_a =
          (pos_a.y - pos_b.y) / distSqrtRatio + pos_b.y - pos_a.y;
      const float dx_b =
          (pos_b.x - pos_a.x) / distSqrtRatio + pos_a.x - pos_b.x;
      const float dy_b =
          (pos_b.y - pos_a.y) / distSqrtRatio + pos_a.y - pos_b.y;
      const float dx_c =
          (pos_c.x - pos_d.x) * distSqrtRatio + pos_d.x - pos_c.x;
      const float dy_c =
          (pos_c.y - pos_d.y) * distSqrtRatio + pos_d.y - pos_c.y;
      const float dx_d =
          (pos_d.x - pos_c.x) * distSqrtRatio + pos_c.x - pos_d.x;
      const float dy_d =
          (pos_d.y - pos_c.y) * distSqrtRatio + pos_c.y - pos_d.y;
      deltaXs[0] += dx_a;
      deltaYs[0] += dy_a;
      deltaXs[1] += dx_b;
      deltaYs[1] += dy_b;
      deltaXs[2] += dx_c;
      deltaYs[2] += dy_c;
      deltaXs[3] += dx_d;
      deltaYs[3] += dy_d;

      for (int i = 0; i < 4; i++) {
        const float dx = deltaXs[i] * gamma * 0.5;
        const float dy = deltaYs[i] * gamma * 0.5;
        const int id = ids[i];
        estimatedPoses[id].x += std::round(dx);
        estimatedPoses[id].y += std::round(dy);
      }
      rep(i, 4) {
        // 矩形範囲を超えないようにする
        const int id = ids[i];
        estimatedPoses[id] = centerRanges[id].Clip(estimatedPoses[id]);
      }
    }

    // 解消されていない場合、c,dが領域にへばりついている場合がある。
    // これは局所解にハマってしまっているので、c,dの位置をスワップする
    dist_ab = estimatedPoses[a].Dist(estimatedPoses[b]);
    dist_cd = estimatedPoses[c].Dist(estimatedPoses[d]);
    if (!IsCorrect(dist_ab, dist_cd)) {
      if (a != c && a != d && b != c && b != d) {
        if (rand(0, (int)1e7) < swapProb * 1e7) {
          auto pos_c = estimatedPoses[c];
          auto pos_d = estimatedPoses[d];
          if (pos_c.x == centerRanges[c].lx || pos_c.x == centerRanges[c].rx ||
              pos_d.x == centerRanges[d].lx || pos_d.x == centerRanges[d].rx) {
            swap(pos_c.x, pos_d.x);
          }
          if (pos_c.y == centerRanges[c].ly || pos_c.y == centerRanges[c].ry ||
              pos_d.y == centerRanges[d].ly || pos_d.y == centerRanges[d].ry) {
            swap(pos_c.y, pos_d.y);
          }
          // 矩形範囲を超えないようにする
          estimatedPoses[c] = centerRanges[c].Clip(pos_c);
          estimatedPoses[d] = centerRanges[d].Clip(pos_d);
        }
      } else {
        int i, k;
        if (a == c) {
          i = a;
          k = d;
        } else if (a == d) {
          i = a;
          k = c;
        } else if (b == c) {
          i = b;
          k = d;
        } else {
          i = b;
          k = c;
        }

        // kをiから最大限遠ざけるためには、iよりも左右、上下どちらにいるべきか
        Pos pos_i = estimatedPoses[i];
        Pos pos_k = estimatedPoses[k];
        const bool shouldLeft = abs(pos_i.x - centerRanges[k].lx) >
                                abs(pos_i.x - centerRanges[k].rx);
        const bool shouldUp = abs(pos_i.y - centerRanges[k].ly) >
                              abs(pos_i.y - centerRanges[k].ry);

        // 左にいるべきなのに右にいるなら、反転する
        if (shouldLeft && pos_k.x > pos_i.x) {
          pos_k.x = pos_i.x + (pos_i.x - pos_k.x);
        } else if (!shouldLeft && pos_k.x < pos_i.x) {
          pos_k.x = pos_i.x + (pos_i.x - pos_k.x);
        }

        // 上にいるべきなのに下にいるなら、反転する
        if (shouldUp && pos_k.y > pos_i.y) {
          pos_k.y = pos_i.y + (pos_i.y - pos_k.y);
        } else if (!shouldUp && pos_k.y < pos_i.y) {
          pos_k.y = pos_i.y + (pos_i.y - pos_k.y);
        }
        // 反転した位置を矩形範囲にクリップする
        pos_k = centerRanges[k].Clip(pos_k);
        // 反転した位置を代入する
        estimatedPoses[k] = pos_k;
      }
    }

    return true;
  }

  // currentPosをbasePosからdistの距離に移動する
  // ただし、rangeの範囲内に収める
  Pos MovePos(const Pos currentPos, const Pos basePos, const float dist,
              const MyRange &range) {
    Pos newPos = currentPos;
    const float originalDist = currentPos.Dist(basePos);
    if (originalDist == 0) {
      return newPos;
    }
    const float dx = (currentPos.x - basePos.x) / originalDist * dist +
                     basePos.x - currentPos.x;
    const float dy = (currentPos.y - basePos.y) / originalDist * dist +
                     basePos.y - currentPos.y;
    newPos.x += std::round(dx * 2.0);
    newPos.y += std::round(dy * 2.0);
    newPos = range.Clip(newPos);

    float currentDist = currentPos.Dist(basePos);
    // 相対誤差が十分小さければOK
    if (abs(currentDist - dist) / max(dist, currentDist) < 0.01) {
      return newPos;
    }
    // もし、rangeの壁にぶつかった場合は、壁沿いに移動する
    // 移動位置を二分探索で求める
    const bool leftWall = range.lx == newPos.x;
    const bool rightWall = range.rx == newPos.x;
    const bool upWall = range.ry == newPos.y;
    const bool downWall = range.ly == newPos.y;

    // 2つの壁にぶつかった場合は、ここでは諦めて、上段で何とかする
    const int wallCount = leftWall + rightWall + upWall + downWall;
    if (wallCount == 2) {
      return newPos;
    }

    if (wallCount == 0) {
      return newPos;
    }

    // 1つの壁にぶつかった場合は、そこから移動する
    // 移動距離を二分探索で求める
    bool moveX;
    // low = basePosに近いほう とする
    float low, high;
    if (leftWall || rightWall) {
      // 上下
      moveX = false;

      // 自分の方が上なら、上を探索する
      if (currentPos.y < basePos.y) {
        low = basePos.y;
        high = range.ly;
      } else {
        low = basePos.y;
        high = range.ry;
      }
    } else {
      // 左右
      moveX = true;

      // 自分の方が左なら、左を探索する
      if (currentPos.x < basePos.x) {
        low = basePos.x;
        high = range.lx;
      } else {
        low = basePos.x;
        high = range.rx;
      }
    }

    // 二分探索で移動距離を求める
    rep(t, 10) {
      const float mid = (low + high) / 2;
      if (moveX) {
        newPos.x = mid;
      } else {
        newPos.y = mid;
      }
      currentDist = newPos.Dist(basePos);
      if (currentDist < dist) {
        low = mid;
      } else {
        high = mid;
      }
    }
    // ここで求めた位置をrangeの範囲にクリップする
    newPos = range.Clip(newPos);
    return newPos;
  }

  bool IsCorrect(const int dist_ab, const int dist_cd) {
    return dist_ab <= dist_cd + 10;
  }
};


#include <atcoder/dsu>

/* start */

namespace Greedy1 {
// 貪欲に木を構築する
struct GreedySolver {
  // 失敗したら、要素数0のvectorを返す
  vector<Cluster> TrySolve(vector<vector<int>> estimatedDists) {
    Timer timer;
    timer.start();
    vector<int> sortedGs = G;
    sort(sortedGs.begin(), sortedGs.end());
    reverse(sortedGs.begin(), sortedGs.end());
    atcoder::dsu dsu(N);
    vector<Edge> sortedEdges;
    rep(i, N) {
      for (int j = i + 1; j < N; j++) {
        int dist = estimatedDists[i][j];
        sortedEdges.push_back(Edge(i, j, dist));
      }
    }
    sort(sortedEdges.begin(), sortedEdges.end(),
         [](const Edge &a, const Edge &b) { return a.dist < b.dist; });
    reverse(sortedEdges.begin(), sortedEdges.end());
    auto memoSortedEdges = sortedEdges;

    bool splitRetry = false;
    int remainMergeCount = N - M;
    multiset<int, greater<int>> sizes;
    for (int i = 0; i < N; i++) {
      sizes.insert(1);
    }
    vector<Edge> retryEdges;
    while (remainMergeCount) {
      bool updated = false;
      vector<Edge> nextRetryEdges;
      rep(t, 2) {
        if (updated)
          break;
        auto &edges = t == 0 ? retryEdges : sortedEdges;
        REP(i, edges.size()) {
          if (remainMergeCount <= 0) {
            break;
          }
          const auto edge = edges[i];
          edges.pop_back();
          const int u = edge.u;
          const int v = edge.v;
          if (dsu.same(u, v)) {
            continue;
          }
          const int uSize = dsu.size(u);
          const int vSize = dsu.size(v);
          const int newSize = uSize + vSize;
          sizes.erase(sizes.find(uSize));
          sizes.erase(sizes.find(vSize));
          sizes.insert(newSize);
          int index = 0;
          bool ok = true;
          for (const auto size : sizes) {
            if (index >= (int)sortedGs.size()) {
              if (splitRetry) {
                // 2回目の試行の場合は、サイズ調整を確実に行うため、端数は1とする
                if (size != 1) {
                  ok = false;
                  break;
                }
              } else {
                ok = true;
                break;
              }
            } else if (sortedGs[index] < size) {
              ok = false;
              break;
            }
            index++;
          }
          if (!ok) {
            sizes.erase(sizes.find(newSize));
            sizes.insert(uSize);
            sizes.insert(vSize);
            nextRetryEdges.push_back(edge);
            continue;
          }
          dsu.merge(u, v);
          remainMergeCount--;
          updated = true;
          break;
        }
      }

      if (!updated) {
        // 失敗
        cerr << "greedy failed: " << timer.ms() << "[ms]" << endl;
        splitRetry = true;
        // 要素数の少ないクラスタを分解して、やり直し
        auto groups = dsu.groups();
        sort(groups.begin(), groups.end(),
             [](const auto &a, const auto &b) { return a.size() > b.size(); });
        groups.resize(M);
        dsu = atcoder::dsu(N);
        sizes.clear();
        int remainCount = N;
        for (const auto &group : groups) {
          sizes.insert(group.size());
          remainCount -= group.size();
          rep(i, group.size()) {
            if(i == 0) continue;
            dsu.merge(group[0], group[i]);
          }
        }
        for (int i = 0; i < remainCount; i++) {
          sizes.insert(1);
        }
        nextRetryEdges.clear();
        retryEdges.clear();
        sortedEdges = memoSortedEdges;
        remainMergeCount = (int)dsu.groups().size() - M;
      }

      while (!nextRetryEdges.empty()) {
        const auto edge = nextRetryEdges.back();
        nextRetryEdges.pop_back();
        retryEdges.push_back(edge);
      }
      reverse(retryEdges.begin(), retryEdges.end());
    }

    // 成功したので、Gに合わせてグループを並び替える
    Cluster::SetEstimatedPoses(estimatedDists);
    vector<Cluster> clusters;
    auto groups = dsu.groups();
    sort(groups.begin(), groups.end(),
         [](const auto &a, const auto &b) { return a.size() > b.size(); });

    map<int, vector<int>> g2Indexes;
    rep(gi, G.size()) { g2Indexes[G[gi]].push_back(gi); }

    auto newGroups = vector<vector<int>>(groups.size());
    for (int i = 0; i < (int)groups.size(); i++) {
      const auto &group = groups[i];
      vector<int> ids;
      for (const auto id : group) {
        ids.push_back(id);
      }
      int index = g2Indexes[ids.size()].back();
      g2Indexes[ids.size()].pop_back();
      newGroups[index] = ids;
    }
    groups = newGroups;

    for (const auto &group : groups) {
      vector<int> ids;
      for (const auto id : group) {
        ids.push_back(id);
      }
      clusters.emplace_back(ids);
    }
    cerr << "greedy success: " << timer.ms() << "[ms]" << endl;

    return clusters;
  }
};
} // namespace Greedy1




// NBD = neighborhood
template <class Solution, class NBDGenerator> struct SimulatedAnnealing {
public:
  // kは0以下 (k = 0のときは線形, k = -∞のときは指数)
  SimulatedAnnealing(double end_time, double start_temp, double end_temp,
                     double k = 0.0)
      : end_time_(end_time * 1000), // ns
        inv_time_(1.0 / end_time_), cur_time_(0.0), start_temp_(start_temp),
        end_temp_(end_temp), diff_temp_(end_temp_ / start_temp_),
        cur_temp_(start_temp), k_(k) {
    assert(start_temp >= end_temp);
    timer_.start();
  }

  Solution run(const Solution &initial_sol, NBDGenerator &nbd_generator) {
    Solution sol(initial_sol);

    int acc = 0;
    int g = 0;
    for (;; ++g) {
      if ((g & 0x1) == 0) {
#ifdef SA_MAX_G
        if (g >= SA_MAX_G) {
          break;
        }
#endif
        UpdateTime();
        UpdateTemp();
        if (cur_time_ > end_time_) {
          break;
        }
      }

      const auto nbd = nbd_generator.Generate(&sol, g, cur_time_ * inv_time_);
      constexpr int mod = 1'000'000'000;
      const int r = rand(0, mod);
      if (static_cast<double>(r) / mod < exp(-nbd->Diff() / cur_temp_)) {
        acc++;
        nbd->Update(&sol);
      } else {
        nbd->Restore(&sol);
      }
    }
    cerr << "g,acc: " << g << " " << acc << endl;
    return sol;
  }

private:
  double end_time_, inv_time_, cur_time_;
  double start_temp_, end_temp_, diff_temp_, cur_temp_;
  double k_;
  Timer timer_;

  void UpdateTime() { cur_time_ = timer_.ns(); }
  void UpdateTemp() {
    // 指数
    cur_temp_ = start_temp_ * pow(diff_temp_, cur_time_ * inv_time_);
    // 線形
    // cur_temp_ = max(0.0, start_temp_ - cur_time_ * diff_temp_ * inv_time_);
    // 一般化
    // cur_temp_ = pow(start_temp_ - k_, 1 - cur_time_ * inv_time_) *
    //                 pow(end_temp_ - k_, cur_time_ * inv_time_) +
    //             k_;
  }
};



#include <atcoder/dsu>
#include <cstdlib>

/*

実装手順
0. Output::StaticInit(istream&)を実装
1. Solutionの初期解生成を書く
2. Solutionに対する操作とEvalの更新を書く
3. Evalの初期値計算を書く
4. Eval::Sum()とSolution::Score()を書く
5. 近傍操作と復元操作を書く
6. 近傍の各確率を調整

*/

namespace SA1 {

struct Evaluation {
  // 最大化問題の場合は符号を反転すること
  int distSum = 0;
  double Sum() const { return distSum; }
};

struct Solution {
  Evaluation eval;
  vector<Cluster> clusters;
  vector<pair<int, int>> id2CiJs;

  Solution() {
    int nextId = 0;
    id2CiJs.resize(N, {-1, -1});
    rep(ci, M) {
      vector<int> ids;
      while (nextId < N && id2CiJs[nextId].first != -1) {
        nextId++;
      }
      const int baseId = nextId;

      vector<int> remainIds;
      for (int i = 0; i < N; i++) {
        if (id2CiJs[i].first == -1) {
          remainIds.push_back(i);
        }
      }
      const auto &dists = Cluster::estimatedDists[baseId];
      sort(remainIds.begin(), remainIds.end(),
           [&](int a, int b) { return dists[a] < dists[b]; });

      rep(j, G[ci]) {
        ids.push_back(remainIds[j]);
        id2CiJs[remainIds[j]] = {ci, j};
      }
      clusters.push_back(Cluster(ids));
    }
    InitEval();
  }
  Solution(const vector<Cluster> &_clusters) : clusters(_clusters) {
    id2CiJs.resize(N, {-1, -1});
    for (int ci = 0; ci < (int)clusters.size(); ci++) {
      for (int j = 0; j < (int)clusters[ci].ids.size(); j++) {
        id2CiJs[clusters[ci].ids[j]] = {ci, j};
      }
    }
    InitEval();
  }
  void InitEval() {
    eval = Evaluation();
    rep(ci, M) {
      eval.distSum += clusters[ci].sumDist;
    }
  }
  pair<int, int> GetCiJ(int id) const { return id2CiJs[id]; }
  void ReflectClusterEvaluation(int ci, bool increase) {
    if (increase) {
      eval.distSum += clusters[ci].sumDist;
    } else {
      eval.distSum -= clusters[ci].sumDist;
    }
  }
  void BuildMST(int ci) { clusters[ci].BuildMST(); }
  void SwapCluster(int ci1, int j1, int ci2, int j2) {
    if (ci1 == ci2)
      return;
    int id1 = clusters[ci1].ids[j1];
    int id2 = clusters[ci2].ids[j2];
    clusters[ci1].ChangeId(j1, id2);
    clusters[ci2].ChangeId(j2, id1);
    id2CiJs[id1] = {ci2, j2};
    id2CiJs[id2] = {ci1, j1};
  }
  void SetNewCluster(int ci, const Cluster &cluster) {
    clusters[ci] = cluster;
    for (int i = 0; i < (int)cluster.ids.size(); i++) {
      id2CiJs[cluster.ids[i]] = {ci, i};
    }
  }
  static Solution CreateInitialSol() {
    // 貪欲構築による解生成を試みる
    Greedy1::GreedySolver greedySolver;
    auto greedyClusters = greedySolver.TrySolve(Cluster::estimatedDists);
    return Solution(greedyClusters);
  }
  double Eval() const { return eval.Sum(); }
  double Score() const { return -Eval(); }
  friend ostream &operator<<(ostream &os, const Solution &sol) { return os; }
};

struct NBD {
  double Diff() const { return diff; }
  void Update(Solution *) const {};
  virtual void Restore(Solution *) const = 0;
  double diff;
};

// 2点スワップ
struct NBD1 : public NBD {
  NBD1() {}
  NBD1(Solution *const sol) {
    diff = -sol->Eval();
    while (true) {
      ci1 = rand(0, M);
      j1 = rand(0, sol->clusters[ci1].ids.size());

      // 近い要素が所属しているクラスタを選ぶ
      const int id2 =
          Cluster::nearIds[ci1][rand(0, max(100, min(N - 1, G[ci1] * 5)))];
      const auto cij2 = sol->GetCiJ(id2);
      ci2 = cij2.first;
      if (rand(0, 100) < 100) {
        ci2 = rand(0, M);
      }

      // 同じクラスタは選ばない
      if (ci1 == ci2)
        continue;

      // スワップ対象の要素はランダムに選ぶ
      j2 = rand(0, sol->clusters[ci2].ids.size());

      sol->ReflectClusterEvaluation(ci1, false);
      sol->ReflectClusterEvaluation(ci2, false);
      sol->SwapCluster(ci1, j1, ci2, j2);
      sol->BuildMST(ci1);
      sol->BuildMST(ci2);
      sol->ReflectClusterEvaluation(ci1, true);
      sol->ReflectClusterEvaluation(ci2, true);
      diff += sol->Eval();
      return;
    }
  }
  void Restore(Solution *sol) const override {
    sol->ReflectClusterEvaluation(ci1, false);
    sol->ReflectClusterEvaluation(ci2, false);
    sol->SwapCluster(ci1, j1, ci2, j2);
    sol->BuildMST(ci1);
    sol->BuildMST(ci2);
    sol->ReflectClusterEvaluation(ci1, true);
    sol->ReflectClusterEvaluation(ci2, true);
  }

  int ci1, j1, ci2, j2;
};

// 2つのクラスタをkickして再構築
struct NBD2 : public NBD {
  NBD2() {}
  NBD2(Solution *const sol) {
    diff = -sol->Eval();
    int count = 0;
    while (true) {
      count++;
      // 近い要素2つを選んで、属する2つのクラスタを破壊再構築する
      const int id1 = rand(0, N);
      ci1 = sol->GetCiJ(id1).first;
      const int id2 = rand_other_than(0, N, id1);
      ci2 = sol->GetCiJ(id2).first;
      if (ci1 == ci2) {
        // 同じクラスタは選ばない
        continue;
      }

      // 距離が遠い場合は選び直し
      if(Cluster::estimatedDists[id1][id2] >= 1000 && count < 30) {
        continue;
      }

      // 2つのクラスタを破壊再構築する
      oldCluster1 = sol->clusters[ci1];
      oldCluster2 = sol->clusters[ci2];
      vector<int> ids;
      ids.reserve(oldCluster1.ids.size() + oldCluster2.ids.size());
      ids.insert(ids.end(), oldCluster1.ids.begin(), oldCluster1.ids.end());
      ids.insert(ids.end(), oldCluster2.ids.begin(), oldCluster2.ids.end());
      vector<int> clusterSizes;
      clusterSizes.push_back(oldCluster1.ids.size());
      clusterSizes.push_back(oldCluster2.ids.size());
      const auto newClusters = SplitClusters(ids, clusterSizes);
      // 評価を更新する
      sol->ReflectClusterEvaluation(ci1, false);
      sol->ReflectClusterEvaluation(ci2, false);
      // 新しいクラスタをセットする
      sol->SetNewCluster(ci1, newClusters[0]);
      sol->SetNewCluster(ci2, newClusters[1]);
      // 評価を更新する
      sol->ReflectClusterEvaluation(ci1, true);
      sol->ReflectClusterEvaluation(ci2, true);
      diff += sol->Eval();
      return;
    }
  }
  void Restore(Solution *sol) const override {
    // 評価を更新する
    sol->ReflectClusterEvaluation(ci1, false);
    sol->ReflectClusterEvaluation(ci2, false);
    // 古いクラスタをセットする
    sol->SetNewCluster(ci1, oldCluster1);
    sol->SetNewCluster(ci2, oldCluster2);
    // 評価を更新する
    sol->ReflectClusterEvaluation(ci1, true);
    sol->ReflectClusterEvaluation(ci2, true);
  }

  int ci1, ci2;
  Cluster oldCluster1, oldCluster2;
};

struct NBDGenerator {
  NBDGenerator(const Solution& sol){
    best_sol = sol;
  }
  NBD *Generate(Solution *const sol, const int g, const double progress) {
    if (g == 0 || sol->Score() > best_sol.Score()) {
      best_sol = *sol;
    }
    const int random = rand(0, 100);
    if (random < PARAMS[6]) {
      nbd1_ = NBD1(sol);
      return &nbd1_;
    } else{
      nbd2_ = NBD2(sol);
      return &nbd2_;
    }
  }
  const Solution &GetBestSol() const { return best_sol; }

private:
  NBD1 nbd1_;
  NBD2 nbd2_;
  Solution best_sol;
};

vector<Cluster> Run(const vector<vector<int>> &estimatedDists, const int timeLimit) {
  Cluster::SetEstimatedPoses(estimatedDists);
  Solution first_sol = Solution::CreateInitialSol();
  Solution best_sol;
  cerr << "start SA: " << global_timer.ms() << "[ms]" << endl;
  if (M == 1) {
    best_sol = first_sol;
  } else {
    NBDGenerator gen(first_sol);
#ifdef LOCAL
    SimulatedAnnealing<Solution, NBDGenerator> SA(timeLimit, 1e-3, 1e-3);
#else
    SimulatedAnnealing<Solution, NBDGenerator> SA(1900 - global_timer.ms(), 1e-3, 1e-3);
#endif
    const auto final_sol = SA.run(first_sol, gen);
    best_sol = gen.GetBestSol();
  }
  cerr << "end SA: " << global_timer.ms() << endl;
  cerr << "score: " << best_sol.Score() << "[ms]" << endl;
  cerr << "distSum: " << best_sol.eval.distSum << endl;

  auto bestClusters = best_sol.clusters;
  int minDistSum = 0;
  rep(i, M) { minDistSum += bestClusters[i].sumDist; }

  // return bestClusters;

  PrintAnswer(bestClusters);
  // cerr << "elapsed time: " << global_timer.ms() << endl;
  std::quick_exit(0);
}

} // namespace SA1






/* start */

namespace Estimator2 {

// 位置推定を行わずに、辺の大小関係から長さを推定する
struct Estimator {
  // 各cityPairの長さの推定値
  vector<int> estimatedDists;

  Estimator(const vector<vector<int>>& _estimatedDists) {
    estimatedDists.resize(N * N, INF);
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        CityPair cityPair(i, j);
        estimatedDists[cityPair.Id()] = _estimatedDists[i][j];
      }
    }

    // 各辺の値を順伝播で更新する
    UpdateForward();
  }

  int Dist(int a, int b) const { return estimatedDists[CityPair(a, b).Id()]; }

  const int kAddingDist = 1;

  void UpdateForward() {
    // 各辺の最小値を順伝播で更新する
    // 未評価の親の個数をカウント
    vector<int> parentCount;
    parentCount.resize(N * N, 0);
    for (int i = 0; i < N * N; i++) {
      for (const auto &child : resultABCDGraph[i]) {
        parentCount[child.Id()]++;
      }
    }

    // 未評価の親がいない辺をキューに追加
    vector<int> que;
    que.reserve(N * N);
    for (int i = 0; i < N * N; i++) {
      if (parentCount[i] == 0) {
        que.push_back(i);
      }
    }

    // 順に評価していく
    while (que.size()) {
      const int parent = que.back();
      que.pop_back();
      const auto &parentDist = estimatedDists[parent];
      for (const auto &child : resultABCDGraph[parent]) {
        // 現在の辺の長さの範囲を更新
        auto &childDist = estimatedDists[child.Id()];
        childDist = max(childDist, parentDist + kAddingDist);
        parentCount[child.Id()]--;
        // 未評価の親がいなくなったらキューに追加
        if (parentCount[child.Id()] == 0) {
          que.push_back(child.Id());
        }
      }
    }
  }
};

} // namespace Estimator2



/* start */

class Solver {
public:
  Solver() {}
  int kEstimatorCount = 20;
  vector<Estimator> estimators;
  void Solve() {
    kEstimatorCount = 50;
    constexpr double kGamma = 1.0;
    // W==0はローカル実験用
    const int firstQ = W == 0 ? 0 : Q;
    rep(i, kEstimatorCount) { estimators.emplace_back(i != 0); }
    auto basePoses = DecideBasePoses(10000 / 20, 10000 / 20);

    // vector<vector<int>> bestQueries = SA_Query::Run(firstQ);
    vector<bool> chosen(N, false);

    while (q < firstQ) {
      Pos basePos = Pos(rand(0, 10000), rand(0, 10000));
      if (L <= PARAMS[5]) {
        const int maxId = DecideMaxVarianceId(chosen);
        basePos = centers[maxId];
        chosen[maxId] = true;
      } else if (basePoses.size() > 0) {
        basePos = basePoses.back();
        basePoses.pop_back();
      } else {
        // 推定対象になっていない点があるなら、選ぶ
        rep(i, N) {
          if (id2QueryCounts[i] == 0) {
            basePos = centers[i];
            break;
          }
        }
      }

      // basePosを基にして、L個の点を選ぶ
      const auto ids = DecideNearIds(basePos);

      vector<ResultABCD> localResultABCDs;
      Query(ids, &localResultABCDs);

      if (localResultABCDs.size() > 0u) {
        const int kTimeLimitNs = 300 * 1000 / Q / kEstimatorCount;
        rep(i, kEstimatorCount) {
          Timer timer;
          timer.start();
          int g = 0;
          while (true) {
            g++;
            if (g % 10 == 0 && timer.ns() > kTimeLimitNs) {
              break;
            }
            estimators[i].UpdateRandomOne(kGamma, localResultABCDs, true);
          }
        }
      }
    }

    cerr << "finish query: " << global_timer.ms() << "[ms]" << endl;

    const int beginTime = 0;
    const int endTime = 500;
    // constexpr float kBeginGamma = kGamma;
    // const float kEndGamma = kGamma * PARAMS[3];
    int updateCount = 0;
    if (q > 0) {
      Timer timer;
      timer.start();
      while (timer.ms() < endTime) {
        if (shouldDebug && updateCount % 10000 == 0) {
          PrintEstimationError();
        }
        if (updateCount % 100 == 0) {
          const float swapProb =
              1.0 * pow(PARAMS[4] / 1.0, (float)(timer.ms() - beginTime) /
                                             (endTime - beginTime));
          rep(k, kEstimatorCount) { estimators[k].swapProb = swapProb; }
        }
        updateCount++;
        // const float gamma = kBeginGamma * pow(kEndGamma / kBeginGamma,
        //                                       (float)(timer.ms() - beginTime) /
        //                                           (endTime - beginTime));
        const float gamma = kGamma;
        rep(i, kEstimatorCount) {
          estimators[i].UpdateRandomOne(gamma, globalResultABCDsVec, true);
        }
      }
    }

    cerr << "finish update: " << global_timer.ms() << "[ms]" << endl;

    if (shouldDebug) {
      PrintEstimationError();
      vector<int> flipCountSums(N);
      rep(k, kEstimatorCount) {
        const auto flipCounts = estimators[k].FlipCounts();
        rep(i, N) { flipCountSums[i] += flipCounts[i]; }
      }
      rep(i, N) {
        if (flipCountSums[i] > 0) {
          if (flipCountSums[i] == 0)
            continue;
        }
      }
      // exit(0);
    }

    cerr << "updateCount: " << updateCount << endl;
    cerr << "sameQueryCount: " << sameQueryCount << endl;

    vector<vector<int>> ensembleDists(N, vector<int>(N));
    rep(k, kEstimatorCount) {
      vector<vector<int>> estimatedDists(N, vector<int>(N));
      rep(i, N) {
        rep(j, N) {
          ensembleDists[i][j] += estimators[k].estimatedPoses[i].Dist(
              estimators[k].estimatedPoses[j]);
        }
      }
      // Estimator2::Estimator estimator2(estimatedDists);
      // rep(i, N) {
      //   rep(j, N) { ensembleDists[i][j] += estimator2.Dist(i, j); }
      // }
    }

    rep(i, N) {
      rep(j, N) { ensembleDists[i][j] /= kEstimatorCount; }
    }

    Estimator2::Estimator estimator2(ensembleDists);
    rep(i, N) {
      rep(j, N) { ensembleDists[i][j] = estimator2.Dist(i, j); }
    }

    cerr << "finish estimation: " << global_timer.ms() << "[ms]" << endl;
    cerr << "g: " << estimators[0].g << " " << "acc: " << estimators[0].acc
         << endl;

    SA1::Run(ensembleDists, 800);
    return;
  }

  vector<Pos> DecideBasePoses(const int width, const int height) {
    vector<Pos> basePoses;
    for (int lx = 0; lx < 10000; lx += width) {
      for (int ly = 0; ly < 10000; ly += height) {
        basePoses.push_back(Pos(lx + width / 2, ly + height / 2));
      }
    }

    return basePoses;
  }

  int DecideMaxVarianceId(const vector<bool> &chosen) {
    // 分散が最も大きい点を選ぶ
    vector<float> variances(N);
    rep(i, N) {
      if (chosen[i]) {
        continue;
      }
      int sumX = 0;
      int sumY = 0;
      ll squareSumX = 0;
      ll squareSumY = 0;
      rep(j, kEstimatorCount) {
        sumX += estimators[j].estimatedPoses[i].x;
        sumY += estimators[j].estimatedPoses[i].y;
        squareSumX += (ll)estimators[j].estimatedPoses[i].x *
                      (ll)estimators[j].estimatedPoses[i].x;
        squareSumY += (ll)estimators[j].estimatedPoses[i].y *
                      (ll)estimators[j].estimatedPoses[i].y;
      }
      sumX /= kEstimatorCount;
      sumY /= kEstimatorCount;
      squareSumX /= kEstimatorCount;
      squareSumY /= kEstimatorCount;
      variances[i] = sqrt(squareSumX - (ll)sumX * (ll)sumX) +
                     sqrt(squareSumY - (ll)sumY * (ll)sumY);
    }

    float maxVariance = -INF;
    int maxId = -1;
    rep(i, N) {
      if (maxVariance < variances[i]) {
        maxVariance = variances[i];
        maxId = i;
      }
    }
    cerr << maxId << " " << maxVariance << endl;
    return maxId;
  }

  vector<int> DecideNearIds(Pos basePos) {
    vector<int> ids(N);
    std::iota(ids.begin(), ids.end(), 0);
    vector<int> dists(N);
    rep(i, N) {
      int minDist = INF;
      rep(j, kEstimatorCount) {
        minDist = min(minDist, estimators[j].estimatedPoses[i].Dist(basePos));
      }
      dists[i] = minDist * (id2QueryCounts[i] * PARAMS[1] -
                            sqrt(centerRanges[i].Area()) / W * PARAMS[2] + 1.0);
    }
    sort(ids.begin(), ids.end(),
         [&](int a, int b) { return dists[a] < dists[b]; });
    ids.resize(L);
    return ids;
  }

  void PrintEstimationError() {
    rep(k, kEstimatorCount) { estimators[k].DebugFlipCount(); }
    // 推定誤差を出力
    vector<Pos> estimatedPoses(N);
    vector<float> variances(N);
    vector<pair<float, int>> diffs;
    float diffSum = 0;
    rep(i, N) {
      int sumX = 0;
      int sumY = 0;
      ll squareSumX = 0;
      ll squareSumY = 0;
      rep(j, kEstimatorCount) {
        sumX += estimators[j].estimatedPoses[i].x;
        sumY += estimators[j].estimatedPoses[i].y;
        squareSumX += (ll)estimators[j].estimatedPoses[i].x *
                      (ll)estimators[j].estimatedPoses[i].x;
        squareSumY += (ll)estimators[j].estimatedPoses[i].y *
                      (ll)estimators[j].estimatedPoses[i].y;
      }
      sumX /= kEstimatorCount;
      sumY /= kEstimatorCount;
      squareSumX /= kEstimatorCount;
      squareSumY /= kEstimatorCount;
      variances[i] = sqrt(squareSumX - (ll)sumX * (ll)sumX) +
                     sqrt(squareSumY - (ll)sumY * (ll)sumY);
      Pos estimatedPos(sumX, sumY);
      diffs.push_back({estimatedPos.Dist(judge.truePoints[i]), i});
      diffSum += estimatedPos.Dist(judge.truePoints[i]);
    }
    diffSum /= N;
    sort(diffs.begin(), diffs.end());
    cerr << "min diff: " << diffs[0].first << " " << diffs[0].second << " "
         << variances[diffs[0].second] << endl;
    cerr << "max diff: " << diffs[N - 1].first << " " << diffs[N - 1].second
         << " " << variances[diffs[N - 1].second] << endl;
    cerr << "mean diff: " << diffSum << endl;
  }

private:
};

int main(int argc, char *argv[]) {
  fast_io;

  if (argc >= 2) {
    if ((string)(argv[1]) == "MyJudge") {
      useJudge = true;
      shouldDebug = true;
    } else {
      int idx = 0;
      for (int i = 1; i < argc; ++i) {
        PARAMS[idx++] = std::stod(argv[i]);
      }
    }
  }

  Timer timer;
  timer.start();
  Initialize(cin);

  Solver solver;
  solver.Solve();
  return 0;
}