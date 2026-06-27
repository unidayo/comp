#ifndef LOCAL
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#define NDEBUG
#define dbg(...)
#else
#define DBG_MACRO_NO_WARNING
#include "dbg.h"
#endif

// clang-format off
#include <bits/stdc++.h>
using namespace std;
template <typename T> using vec = vector<T>;
template <typename T> using vvec = vector<vec<T>>;
template <typename T> using vvvec = vector<vvec<T>>;
using ll = long long; using ld = long double;
using pii = pair<int, int>; using pll = pair<ll, ll>;
using tiii = tuple<int, int, int>; using tlll = tuple<ll, ll, ll>;
using vi = vec<int>; using vvi = vvec<int>; using vvvi = vvvec<int>;
using vll = vec<ll>; using vvll = vvec<ll>; using vvvll = vvvec<ll>;
#define ALL(x) begin(x), end(x)
#define RALL(x) (x).rbegin(), (x).rend()
constexpr int INF = 1001001001; constexpr long long INFll = 1001001001001001001;
template <class T> bool chmax(T& a, const T& b) { return a < b ? a = b, 1 : 0; }
template <class T> bool chmaxEq(T& a, const T &b) { return a <= b ? a = b, 1 : 0; }
template <class T> bool chmin(T& a, const T& b) { return a > b ? a = b, 1 : 0; }
template <class T> bool chminEq(T& a, const T &b) { return a >= b ? a = b, 1 : 0; }

struct Timer{
  using C=std::chrono::system_clock;
  Timer(){}
  void start(){s=C::now();assert(!b);b=1;}
  void stop(){sum+=duration();assert(b);b=0;}
  void reset(){sum=0;}
  double sec(){return sum+(b?duration():0);}
  private:C::time_point s;bool b=0;double sum=0;
  double duration(){return std::chrono::duration<double>(C::now()-s).count();}
}timer,timer1,timer2,timer3;

struct Xor128{// period 2^128 - 1
  uint32_t x,y,z,w;
  static constexpr uint32_t min(){return 0;}
  static constexpr uint32_t max(){return UINT32_MAX;}
  Xor128(uint32_t seed=0):x(123456789),y(362436069),z(521288629),w(88675123+seed){}
  uint32_t operator()(){uint32_t t=x^(x<<11);x=y;y=z;z=w;return w=(w^(w>>19))^(t^(t>>8));}
  uint32_t operator()(uint32_t l,uint32_t r){return((*this)()%(r-l))+l;}
  uint32_t operator()(uint32_t r){return(*this)()%r;}
};

struct Rand{// https://docs.python.org/ja/3/library/random.html
  Rand():gen(0),mt(0){}
  void set_seed(int seed){Xor128 _gen(seed);gen=_gen;}
  /* prob%  */ bool randBool(double prob){return gen()<prob*gen.max();}
  /* [0, 1) */ double random(){return(double)gen()/gen.max();}
  /* [a, b] */ double uniform(double a,double b){if(b<a)swap(a,b);return a+(b-a)*double(gen())/gen.max();}
  /* [0, r) */ uint32_t randRange(uint32_t r){return gen(r);}
  /* [l, r) */ uint32_t randRange(uint32_t l,uint32_t r){return gen(l,r);}
  /* [l, r] */ uint32_t randInt(uint32_t l,uint32_t r){return gen(l,r+1);}
  /* Fisher */ template<class T>void shuffle(vector<T>&x){for(int i=x.size(),j;i>1;){j=gen(i);swap(x[j],x[--i]);}}
  /* Normal */ double normalvariate(double mu=0.0,double sigma=1.0){return std::normal_distribution(mu,sigma)(mt);}
  private: Xor128 gen; std::mt19937 mt;
}myrand;

namespace esc {
const vi colors{196,208,226,46,77,14,12,13,5,136,195,245};
template <class T>void color(int c,T s,bool doEndl=true){cerr<<"\e[38;5;"<<colors[c]<<"m"<<s<<"\e[0m";if(doEndl)cerr<<endl;}
template <class T>void RED       (T s,bool doEndl=true){color( 0,s,doEndl);}
template <class T>void ORANGE    (T s,bool doEndl=true){color( 1,s,doEndl);}
template <class T>void YELLOW    (T s,bool doEndl=true){color( 2,s,doEndl);}
template <class T>void LIGHTGREEN(T s,bool doEndl=true){color( 3,s,doEndl);}
template <class T>void GREEN     (T s,bool doEndl=true){color( 4,s,doEndl);}
template <class T>void AQUA      (T s,bool doEndl=true){color( 5,s,doEndl);}
template <class T>void BLUE      (T s,bool doEndl=true){color( 6,s,doEndl);}
template <class T>void PINK      (T s,bool doEndl=true){color( 7,s,doEndl);}
template <class T>void PURPLE    (T s,bool doEndl=true){color( 8,s,doEndl);}
template <class T>void BROWN     (T s,bool doEndl=true){color( 9,s,doEndl);}
template <class T>void WHITE     (T s,bool doEndl=true){color(10,s,doEndl);}
template <class T>void GRAY      (T s,bool doEndl=true){color(11,s,doEndl);}
string withSep(int n,char sep=','){string ret="",s=to_string(n);reverse(ALL(s));for(int i=0,len=s.length();i<=len;)
{ret+=s.substr(i,3);if((i+=3)>=len)break;ret+=sep;}reverse(ALL(ret));return ret;}
string withFill(int n,int num=3,char space=' '){string s=to_string(n);return string(max(0,num-int(s.size())),space)+s;}}

template<int sz>
struct IndexSet{
  vector<int>vec;
  IndexSet(){for(int i=0;i<sz;i++)_pos[i]=-1;}
  void insert(int v){assert(0<=v&&v<sz);if(contains(v))return;_pos[v]=vec.size();vec.push_back(v);}
  void remove(int v){assert(contains(v));int p=_pos[v],b=vec.back();vec[p]=b;vec.pop_back();_pos[b]=p;_pos[v]=-1;}
  bool contains(int v)const{return _pos[v]!=-1;}
  bool empty()const{return vec.empty();}
  size_t size()const{return vec.size();}
  friend ostream&operator<<(ostream&os,const IndexSet&is){vi v=is.vec;sort(ALL(v));for(auto&e:v)os<<e<<" ";return os<<endl;}
 private:
  int _pos[sz];
};
using IdxSet = IndexSet<100>;
// clang-format on

#ifdef VIS
#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;
#define vis(...) _vis(__LINE__, #__VA_ARGS__, __VA_ARGS__)
void _vis(int lineNum, string name, vector<int> As) {
  cerr << "\033[1;32m" << lineNum << "\033[0m: now plotting '" << name << "'..."
       << endl;
  plt::title(name);
  plt::plot(As);
  plt::show();
  // plt::save("img/imshow.png");
}
#else
#define vis(...)
#endif

constexpr int N = 20;

enum CellState { EMPTY = 0, ONI = 1, FUKU = 2 };
using Board = array<char, N * N>;

Board BOARD;

// 2D index to 1D index
inline int index(int i, int j) { return i * N + j; }

template <bool upd>
void addOperations(vector<pair<char, int>>& operations, char direction,
                   int index, int count) {
  if constexpr (!upd) return;
  for (int k = 0; k < count; ++k) {
    operations.emplace_back(direction, index);
  }
}

array<int, 4 * N * N> scores;

template <bool upd>
int removeOni(const Board& board, vector<pair<char, int>>& operations) {
  vvi moves(4, vi(N, 0));

  // d方向に対して、FUKUが存在しないかを確認
  for (int j = 0; j < N; j++) {
    for (int i = 0; i < N; i++) {
      if (board[index(i, j)] == FUKU) {
        break;
      } else if (board[index(i, j)] == ONI) {
        scores[4 * index(i, j) + 0] = i + 1;
      }
    }
    for (int i = N - 1; i >= 0; i--) {
      if (board[index(i, j)] == FUKU) {
        break;
      } else if (board[index(i, j)] == ONI) {
        scores[4 * index(i, j) + 1] = N - i;
      }
    }
  }
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (board[index(i, j)] == FUKU) {
        break;
      } else if (board[index(i, j)] == ONI) {
        scores[4 * index(i, j) + 2] = j + 1;
      }
    }
    for (int j = N - 1; j >= 0; j--) {
      if (board[index(i, j)] == FUKU) {
        break;
      } else if (board[index(i, j)] == ONI) {
        scores[4 * index(i, j) + 3] = N - j;
      }
    }
  }

  bool returnINF = false;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (board[index(i, j)] != ONI) continue;
      if (!returnINF) {
        int bestD = -1;
        int bestScore = INF;
        for (int d = 0; d < 4; d++) {
          if (scores[4 * index(i, j) + d] < bestScore) {
            bestScore = scores[4 * index(i, j) + d];
            bestD = d;
          }
        }
        if (bestScore == INF)
          returnINF = true;
        else
          chmax(moves[bestD][bestD < 2 ? j : i], bestScore);
      }
      scores[4 * index(i, j) + 0] = INF;
      scores[4 * index(i, j) + 1] = INF;
      scores[4 * index(i, j) + 2] = INF;
      scores[4 * index(i, j) + 3] = INF;
    }
  }
  if (returnINF) return INF;

  int moveCount = 0;
  for (int j = 0; j < N; j++) {
    bool isAllClear = true;
    for (int i = moves[0][j]; i < N - moves[1][j]; i++) {
      if (board[index(i, j)] != EMPTY) {
        isAllClear = false;
        break;
      }
    }
    if (isAllClear) {
      if (moves[0][j] < moves[1][j]) {
        addOperations<upd>(operations, 'U', j, moves[0][j]);
        addOperations<upd>(operations, 'D', j, moves[0][j]);
        addOperations<upd>(operations, 'D', j, moves[1][j]);
        moveCount += moves[0][j] * 2 + moves[1][j];
      } else {
        addOperations<upd>(operations, 'D', j, moves[1][j]);
        addOperations<upd>(operations, 'U', j, moves[1][j]);
        addOperations<upd>(operations, 'U', j, moves[0][j]);
        moveCount += moves[0][j] + moves[1][j] * 2;
      }
    } else {
      addOperations<upd>(operations, 'U', j, moves[0][j]);
      addOperations<upd>(operations, 'D', j, moves[0][j]);
      addOperations<upd>(operations, 'D', j, moves[1][j]);
      addOperations<upd>(operations, 'U', j, moves[1][j]);
      moveCount += moves[0][j] * 2 + moves[1][j] * 2;
    }
  }
  for (int i = 0; i < N; i++) {
    if (moves[2][i] < moves[3][i]) {
      addOperations<upd>(operations, 'L', i, moves[2][i]);
      addOperations<upd>(operations, 'R', i, moves[2][i]);
      addOperations<upd>(operations, 'R', i, moves[3][i]);
      moveCount += moves[2][i] * 2 + moves[3][i];
    } else {
      addOperations<upd>(operations, 'R', i, moves[3][i]);
      addOperations<upd>(operations, 'L', i, moves[3][i]);
      addOperations<upd>(operations, 'L', i, moves[2][i]);
      moveCount += moves[2][i] + moves[3][i] * 2;
    }
  }
  return moveCount;
}

// 評価関数
int evaluate(const Board& board, const vector<pair<int, int>>& actions) {
  int score = actions.size();
  vector<pair<char, int>> dummy = {};
  int res = removeOni<false>(board, dummy);
  return score + res;
}

bool updateBoard(Board& board, const vector<pair<int, int>>& actions) {
  // inline int index(int i, int j) { return i * N + j; }
  for (auto [d, p] : actions) {
    // dはUDLRを表す pは列か行を表すindex
    // boardに対して、一回分のshiftをそれぞれ適用する
    if (d == 0) {
      int j = p;
      int IDX = index(0, j);
      if (board[IDX] == FUKU) return false;
      for (int i = 0; i + 1 < N; i++) {
        board[IDX] = board[IDX + N];
        IDX += N;
      }
      board[IDX] = EMPTY;
    } else if (d == 1) {
      int j = p;
      int IDX = index(N - 1, j);
      if (board[IDX] == FUKU) return false;
      for (int i = N - 1; i - 1 >= 0; i--) {
        board[IDX] = board[IDX - N];
        IDX -= N;
      }
      board[IDX] = EMPTY;
    } else if (d == 2) {
      int i = p;
      int IDX = index(i, 0);
      if (board[IDX] == FUKU) return false;
      for (int j = 0; j + 1 < N; j++) {
        board[IDX] = board[IDX + 1];
        IDX++;
      }
      board[IDX] = EMPTY;
    } else {
      int i = p;
      int IDX = index(i, N - 1);
      if (board[IDX] == FUKU) return false;
      for (int j = N - 1; j - 1 >= 0; j--) {
        board[IDX] = board[IDX - 1];
        IDX--;
      }
      board[IDX] = EMPTY;
    }
  }
  return true;
}

// 焼き鈍し法
pair<Board, vector<pair<int, int>>> optimizeShift() {
  Board bestBoard = BOARD;
  vector<pair<int, int>> bestActions;
  int bestScore = evaluate(bestBoard, bestActions);
  assert(bestScore < INF);
  const double TL = 1.95;
  const double initialTemp = 4.0;
  const double finalTemp = -2.0;
  vector<pair<int, int>> actions;
  while (true) {
    double now = timer.sec();
    if (now > TL) break;
    double temp = initialTemp + (finalTemp - initialTemp) * now / TL;
    double t = pow(2, temp);

    // actionsに適当な操作を追加
    double strategy = myrand.random();
    if (strategy < 0.55) {
      actions = bestActions;
      int idx = myrand.randInt(0, actions.size());
      int d = myrand.randInt(0, 3);
      int p = myrand.randInt(0, N - 1);
      actions.insert(actions.begin() + idx, {d, p});
    } else if (strategy < 0.7) {
      // 隣接する2つの操作をswap
      if (bestActions.size() < 2) continue;
      actions = bestActions;
      int idx = myrand.randInt(0, bestActions.size() - 2);
      swap(actions[idx], actions[idx + 1]);
    } else {
      // 適当に削除
      if (bestActions.empty()) continue;
      actions.clear();
      int idx = myrand.randRange(bestActions.size());
      for (int i = 0; i < int(bestActions.size()); i++) {
        if (i == idx) continue;
        actions.push_back(bestActions[i]);
      }
    }

    Board board = BOARD;
    bool success = updateBoard(board, actions);
    if (!success) continue;

    int score = evaluate(board, actions);
    if (score < bestScore || ((bestScore - score / t < 10) &&
                              exp((bestScore - score) / t) > myrand.random())) {
      bestScore = score;
      bestBoard = board;
      bestActions = actions;
    }
  }
  return {bestBoard, bestActions};
}

void readInput() {
  int _N;
  cin >> _N;
  assert(_N == N);
  for (int i = 0; i < N; ++i) {
    string row;
    cin >> row;
    for (int j = 0; j < N; ++j) {
      if (row[j] == 'x')
        BOARD[index(i, j)] = ONI;
      else if (row[j] == 'o')
        BOARD[index(i, j)] = FUKU;
      else
        BOARD[index(i, j)] = EMPTY;
    }
  }
  scores.fill(INF);
}

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  readInput();
  timer.start();

  auto [bestBoard, bestActions] = optimizeShift();
  for (auto [d, p] : bestActions) {
    cout << "UDLR"[d] << " " << p << "\n";
  }

  vector<pair<char, int>> operations;
  removeOni<true>(bestBoard, operations);
  for (auto [d, p] : operations) {
    cout << d << " " << p << "\n";
  }

  timer.stop();

  std::cerr << bestActions.size() + operations.size() << endl;

  return 0;
}
