#ifdef __GNUC__
#pragma GCC optimize ("O3")
#pragma GCC optimize ("unroll-loops")
#pragma GCC target ("avx2")
#endif

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <array>
#include <algorithm>
#include <utility>
#include <string>
#include <queue>
#include <stack>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <bitset>
#include <random>
#include <functional>
#include <memory>
#include <cmath>
#include <cassert>

#include <atcoder/all>

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

struct xorshift64 {
  unsigned long long int x = 88172645463325252ULL;
  inline unsigned short nextUShort() {
    x = x ^ (x << 7);
    return x = x ^ (x >> 9);
  }
  inline unsigned int nextUShortMod(unsigned long long int mod) {
    x = x ^ (x << 7);
    x = x ^ (x >> 9);
    return ((x & 0x0000ffffffffffff) * mod) >> 48;
  }
  inline unsigned int nextUInt() {
    x = x ^ (x << 7);
    return x = x ^ (x >> 9);
  }
  inline unsigned int nextUIntMod(unsigned long long int mod) {
    x = x ^ (x << 7);
    x = x ^ (x >> 9);
    return ((x & 0x00000000ffffffff) * mod) >> 32;
  }
  inline unsigned long long int nextULL() {
    x = x ^ (x << 7);
    return x = x ^ (x >> 9);
  }
  inline double nextDouble() {
    x = x ^ (x << 7);
    x = x ^ (x >> 9);
    return (double)x * 5.42101086242752217e-20;
  }
};

struct timer {
  double t = 0.0;
  double lastStop = 0.0;
  bool stopped = false;
  timer() {
    restart();
  }
  inline void restart() {
    t = now();
    stopped = false;
  }
  inline void start() {
    if (stopped) {
      t += now() - lastStop;
      stopped = false;
    }
  }
  inline void stop() {
    if (!stopped) {
      lastStop = now();
      stopped = true;
    }
  }
  inline double time() {
    if (stopped) return lastStop - t;
    else return now() - t;
  }
  inline double now() {
    #ifdef _MSC_VER
      #ifdef LOCAL
        return __rdtsc() * 2.857142857142857e-10; // 1 / 3.5e9, for local (Ryzen 9 3950X)
      #else
        //return __rdtsc() * 3.5714285714285715e-10; // 1 / 2.8e9, for AWS EC2 C3 (Xeon E5-2680 v2)
        //return __rdtsc() * 3.4482758620689656e-10; // 1 / 2.9e9, for AWS EC2 C4 (Xeon E5-2666 v3)
        //return __rdtsc() * 3.333333333333333e-10; // 1 / 3.0e9, for AWS EC2 C5 (Xeon Platinum 8124M / Xeon Platinum 8275CL)
        return __rdtsc() * 3.4482758620689656e-10; // 1 / 2.9e9, for AWS EC2 C6i (Xeon Platinum 8375C)
      #endif
    #else
      unsigned long long l, h;
        __asm__ ("rdtsc" : "=a"(l), "=d"(h));
      #ifdef LOCAL
        return (double)(l | h << 32) * 2.857142857142857e-10; // 1 / 3.5e9, for local (Ryzen 9 3950X)
      #else
        //return (double)(l | h << 32) * 3.5714285714285715e-10; // 1 / 2.8e9, for AWS EC2 C3 (Xeon E5-2680 v2)
        //return (double)(l | h << 32) * 3.4482758620689656e-10; // 1 / 2.9e9, for AWS EC2 C4 (Xeon E5-2666 v3)
        //return (double)(l | h << 32) * 3.333333333333333e-10; // 1 / 3.0e9, for AWS EC2 C5 (Xeon Platinum 8124M / Xeon Platinum 8275CL)
        return (double)(l | h << 32) * 3.4482758620689656e-10; // 1 / 2.9e9, for AWS EC2 C6i (Xeon Platinum 8375C)
      #endif
    #endif
  }
};

using namespace std;

typedef long long int ll;
typedef unsigned long long int ull;
typedef pair<int, int> Pii;
typedef unsigned char uchar;

const ll mod = 998244353;

timer theTimer;
xorshift64 theRandom;
mt19937_64 theMersenne(1);

// libraries
namespace Lib {
}

// hyper parameters
namespace HyperParameter {
  void load_hyper_parameter(int argc, char *argv[]) {
  }
}

// enums

// structs

// constants
struct Consts {
  constexpr static int board_size = 20;
};

struct Adj4 {
  static constexpr array<int, 4>  dx = { -1,   0,   1,   0};
  static constexpr array<int, 4>  dy = {  0,  -1,   0,   1};
  static constexpr array<char, 4> ch = {'U', 'L', 'D', 'R'};
  static int reverse(const int dir) {
    assert(0 <= dir && dir < 4);
    return dir ^ 2;
  }
};

// inputs
struct Input {
  int board_size;
  vector<vector<char>> init_board;
};

// outputs
struct Output {
  vector<pair<char, int>> actions;
};

// internals
struct Environment {
  int board_size;
  vector<vector<char>> init_board;

  int max_turn_limit;
  int init_oni_count;
};

Input get_input() {
  Input input;

  cin >> input.board_size;
  input.init_board = vector<vector<char>>(input.board_size, vector<char>(input.board_size));
  for (int i = 0; i < input.board_size; i++) {
    for (int j = 0; j < input.board_size; j++) {
      cin >> input.init_board[i][j];
    }
  }

  return input;
}

Environment create_environment(const Input &input) {
  Environment env;

  env.board_size = input.board_size;
  env.init_board = input.init_board;

  env.max_turn_limit = env.board_size * env.board_size * 4;
  env.init_oni_count = 0;
  for (int i = 0; i < env.board_size; i++) {
    for (int j = 0; j < env.board_size; j++) {
      if (env.init_board[i][j] == 'x') env.init_oni_count++;
    }
  }

  return env;
}

void print_output(const Output &output) {
  for (auto &a: output.actions) {
    cout << a.first << " " << a.second << endl;
  }
}

// source: https://eijirou-kyopro.hatenablog.com/entry/2024/02/01/115639
namespace beam_search {

// メモリの再利用を行いつつ集合を管理するクラス
template<class T>
class ObjectPool {
    public:
        // 配列と同じようにアクセスできる
        T& operator[](int i) {
            return data_[i];
        }

        // 配列の長さを変更せずにメモリを確保する
        void reserve(size_t capacity) {
            data_.reserve(capacity);
        }

        // 要素を追加し、追加されたインデックスを返す
        int push(const T& x) {
            if (garbage_.empty()) {
                data_.push_back(x);
                return data_.size() - 1;
            } else {
                int i = garbage_.top();
                garbage_.pop();
                data_[i] = x;
                return i;
            }
        }

        // 要素を（見かけ上）削除する
        void pop(int i) {
            garbage_.push(i);
        }

        // 使用した最大のインデックス(+1)を得る
        // この値より少し大きい値をreserveすることでメモリの再割り当てがなくなる
        size_t size() {
            return data_.size();
        }

    private:
        vector<T> data_;
        stack<int> garbage_;
};

// 連想配列
// Keyにハッシュ関数を適用しない
// open addressing with linear probing
// unordered_mapよりも速い
// nは格納する要素数よりも4~16倍ほど大きくする
template <class Key, class T>
struct HashMap {
    public:
        explicit HashMap(uint32_t n) {
            n_ = n;
            valid_.resize(n_, false);
            data_.resize(n_);
        }

        // 戻り値
        // - 存在するならtrue、存在しないならfalse
        // - index
        pair<bool,int> get_index(Key key) const {
            Key i = key % n_;
            while (valid_[i]) {
                if (data_[i].first == key) {
                    return {true, i};
                }
                if (++i == n_) {
                    i = 0;
                }
            }
            return {false, i};
        }

        // 指定したindexにkeyとvalueを格納する
        void set(int i, Key key, T value) {
            valid_[i] = true;
            data_[i] = {key, value};
        }

        // 指定したindexのvalueを返す
        T get(int i) const {
            assert(valid_[i]);
            return data_[i].second;
        }

        void clear() {
            fill(valid_.begin(), valid_.end(), false);
        }

    private:
        uint32_t n_;
        vector<bool> valid_;
        vector<pair<Key,T>> data_;
};

using Hash = uint32_t; // TODO

vector<vector<Hash>> hash_table;

inline void init_hash(const Environment &env) {
  hash_table = vector<vector<Hash>>(2, vector<Hash>(env.board_size));
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < env.board_size; j++) {
      hash_table[i][j] = theRandom.nextUInt();
    }
  }
}

inline Hash get_hash(const char dir, const char lane) {
  return (dir & 2) ? hash_table[dir & 1][lane] : -hash_table[dir & 1][lane];
}

// 状態遷移を行うために必要な情報
// メモリ使用量をできるだけ小さくしてください
struct Action {
    char dir;
    char lane;

    Action(const char dir, const char lane) :
        dir(dir),
        lane(lane) {}
};

using Cost = int;

// 状態のコストを評価するための構造体
// メモリ使用量をできるだけ小さくしてください
struct Evaluator {
    Cost score;

    Evaluator(const Cost score) : score(score) {}

    // 低いほどよい
    Cost evaluate() const {
      return score;
    }
};

// 展開するノードの候補を表す構造体
struct Candidate {
    Action action;
    Evaluator evaluator;
    Hash hash;
    int parent;
    Cost cost;

    Candidate(Action action, Evaluator evaluator, Hash hash, int parent, Cost cost) :
        action(action),
        evaluator(evaluator),
        hash(hash),
        parent(parent),
        cost(cost) {}
};

// ビームサーチの設定
struct Config {
    int max_turn;
    size_t beam_width;
    size_t nodes_capacity;
    uint32_t hash_map_capacity;
};

// 削除可能な優先度付きキュー
using MaxSegtree = atcoder::segtree<
    pair<Cost,int>,
    [](pair<Cost,int> a, pair<Cost,int> b){
        if (a.first >= b.first) {
            return a;
        } else {
            return b;
        }
    },
    []() { return make_pair(numeric_limits<Cost>::min(), -1); }
>;

// ノードの候補から実際に追加するものを選ぶクラス
// ビーム幅の個数だけ、評価がよいものを選ぶ
// ハッシュ値が一致したものについては、評価がよいほうのみを残す
class Selector {
    public:
        explicit Selector(const Config& config) :
            hash_to_index_(config.hash_map_capacity)
        {
            beam_width = config.beam_width;
            candidates_.reserve(beam_width);
            full_ = false;
            st_original_.resize(beam_width);
        }

        // 候補を追加する
        // ターン数最小化型の問題で、candidateによって実行可能解が得られる場合にのみ finished = true とする
        // ビーム幅分の候補をCandidateを追加したときにsegment treeを構築する
        void push(Action action, const Evaluator& evaluator, Hash hash, int parent, bool finished) {
            Cost cost = evaluator.evaluate();
            if (finished) {
                finished_candidates_.emplace_back(Candidate(action, evaluator, hash, parent, cost));
                return;
            }
            if (full_ && cost >= st_.all_prod().first) {
                // 保持しているどの候補よりもコストが小さくないとき
                return;
            }
            auto [valid, i] = hash_to_index_.get_index(hash);

            if (valid) {
                int j = hash_to_index_.get(i);
                if (hash == candidates_[j].hash) {
                    // ハッシュ値が等しいものが存在しているとき
                    if (cost < candidates_[j].cost) {
                        // 更新する場合
                        candidates_[j] = Candidate(action, evaluator, hash, parent, cost);
                        if (full_) {
                            st_.set(j, {cost, j});
                        }
                    }
                    return;
                }
            }
            if (full_) {
                // segment treeが構築されている場合
                int j = st_.all_prod().second;
                hash_to_index_.set(i, hash, j);
                candidates_[j] = Candidate(action, evaluator, hash, parent, cost);
                st_.set(j, {cost, j});
            } else {
                // segment treeが構築されていない場合
                hash_to_index_.set(i, hash, candidates_.size());
                candidates_.emplace_back(Candidate(action, evaluator, hash, parent, cost));

                if (candidates_.size() == beam_width) {
                    // 保持している候補がビーム幅分になったとき
                    construct_segment_tree();
                }
            }
        }

        // 選んだ候補を返す
        const vector<Candidate>& select() const {
            return candidates_;
        }

        // 実行可能解が見つかったか
        bool have_finished() const {
            return !finished_candidates_.empty();
        }

        // 実行可能解に到達する「候補」を返す
        vector<Candidate> get_finished_candidates() const {
            return finished_candidates_;
        }

        void clear() {
            candidates_.clear();
            hash_to_index_.clear();
            full_ = false;
        }

    private:
        size_t beam_width;
        vector<Candidate> candidates_;
        HashMap<Hash,int> hash_to_index_;
        bool full_;
        vector<pair<Cost,int>> st_original_;
        MaxSegtree st_;
        vector<Candidate> finished_candidates_;

        void construct_segment_tree() {
            full_ = true;
            for (size_t i = 0; i < beam_width; ++i) {
                st_original_[i] = {candidates_[i].cost, i};
            }
            st_ = MaxSegtree(st_original_);
        }
};

// 深さ優先探索に沿って更新する情報をまとめたクラス
class State {
    public:
        explicit State(const Environment& env) : env(env) {
            turn_num = 0;
            board_oni_row = vector<bitset<Consts::board_size>>(Consts::board_size);
            board_oni_col = vector<bitset<Consts::board_size>>(Consts::board_size);
            board_fuku_row = vector<bitset<Consts::board_size>>(Consts::board_size);
            board_fuku_col = vector<bitset<Consts::board_size>>(Consts::board_size);
            for (int i = 0; i < Consts::board_size; i++) {
              for (int j = 0; j < Consts::board_size; j++) {
                if (env.init_board[i][j] == 'x') {
                  board_oni_row[i].set(j);
                  board_oni_col[j].set(i);
                }
                else if (env.init_board[i][j] == 'o') {
                  board_fuku_row[i].set(j);
                  board_fuku_col[j].set(i);
                }
              }
            }
        }

        // 次の状態候補を全てselectorに追加する
        // 引数
        //   evaluator : 今の評価器
        //   hash      : 今のハッシュ値
        //   parent    : 今のノードID（次のノードにとって親となる）
        void expand(const Evaluator& evaluator, Hash hash, int parent, Selector& selector) {
          // cerr << turn_num << endl;
          // for (int i = 0; i < env.board_size; i++) {
          //   for (int j = 0; j < env.board_size; j++) {
          //     cerr << (board_oni_row[i][j] ? 'x' : board_fuku_row[i][j] ? 'o' : '.');
          //   }
          //   cerr << endl;
          //   for (int j = 0; j < env.board_size; j++) {
          //     cerr << (board_oni_col[j][i] ? 'x' : board_fuku_col[j][i] ? 'o' : '.');
          //   }
          //   cerr << endl;
          // }
          for (char dir = 0; dir < 4; dir++) {
            for (char lane = 0; lane < env.board_size; lane++) {
              int score_delta = 0;
              if (dir == 0) {
                if (board_fuku_row[0][lane]) continue;
                for (int i = 0; i < env.board_size; i++) {
                  if (board_oni_row[i][lane]) score_delta -= get_oni_score(i, lane);
                }
              }
              else if (dir == 1) {
                if (board_fuku_row[lane][0]) continue;
                for (int j = 0; j < env.board_size; j++) {
                  if (board_oni_row[lane][j]) score_delta -= get_oni_score(lane, j);
                }
              }
              else if (dir == 2) {
                if (board_fuku_row[env.board_size - 1][lane]) continue;
                for (int i = 0; i < env.board_size; i++) {
                  if (board_oni_row[i][lane]) score_delta -= get_oni_score(i, lane);
                }
              }
              else if (dir == 3) {
                if (board_fuku_row[lane][env.board_size - 1]) continue;
                for (int j = 0; j < env.board_size; j++) {
                  if (board_oni_row[lane][j]) score_delta -= get_oni_score(lane, j);
                }
              }

              update_board_forward(dir, lane);

              if (dir == 0) {
                for (int i = 0; i < env.board_size; i++) {
                  if (board_oni_row[i][lane]) score_delta += get_oni_score(i, lane);
                }
              }
              else if (dir == 1) {
                for (int j = 0; j < env.board_size; j++) {
                  if (board_oni_row[lane][j]) score_delta += get_oni_score(lane, j);
                }
              }
              else if (dir == 2) {
                for (int i = 0; i < env.board_size; i++) {
                  if (board_oni_row[i][lane]) score_delta += get_oni_score(i, lane);
                }
              }
              else if (dir == 3) {
                for (int j = 0; j < env.board_size; j++) {
                  if (board_oni_row[lane][j]) score_delta += get_oni_score(lane, j);
                }
              }

              Hash next_hash = hash + get_hash(dir, lane);
              Action next_action = Action(dir, lane);
              Evaluator next_evaluator = Evaluator(evaluator.score + score_delta);
              selector.push(next_action, next_evaluator, next_hash, parent, (int) removed_oni_turn.size() == env.init_oni_count);

              update_board_backward(dir, lane);
            }
          }
        }

        // actionを実行して次の状態に遷移する
        void move_forward(Action action) {
          update_board_forward(action.dir, action.lane);
        }

        // actionを実行する前の状態に遷移する
        // 今の状態は、親からactionを実行して遷移した状態である
        void move_backward(Action action) {
          update_board_backward(action.dir, action.lane);
        }

    private:
        const Environment &env;

        int turn_num;
        vector<bitset<Consts::board_size>> board_oni_row;
        vector<bitset<Consts::board_size>> board_oni_col;
        vector<bitset<Consts::board_size>> board_fuku_row;
        vector<bitset<Consts::board_size>> board_fuku_col;
        stack<short> removed_oni_turn;

        void update_board_forward(const char dir, const char lane) {
          if (dir == 0) {
            if (board_oni_row[0][lane]) {
              removed_oni_turn.push(turn_num);
            }
            for (int i = 1; i < env.board_size; i++) {
              board_oni_row[i - 1][lane] = board_oni_row[i][lane];
              board_fuku_row[i - 1][lane] = board_fuku_row[i][lane];
            }
            board_oni_row[env.board_size - 1][lane] = false;
            board_fuku_row[env.board_size - 1][lane] = false;
            board_oni_col[lane] >>= 1;
            board_fuku_col[lane] >>= 1;
          }
          else if (dir == 1) {
            if (board_oni_row[lane][0]) {
              removed_oni_turn.push(turn_num);
            }
            board_oni_row[lane] >>= 1;
            board_fuku_row[lane] >>= 1;
            for (int j = 1; j < env.board_size; j++) {
              board_oni_col[j - 1][lane] = board_oni_col[j][lane];
              board_fuku_col[j - 1][lane] = board_fuku_col[j][lane];
            }
            board_oni_col[env.board_size - 1][lane] = false;
            board_fuku_col[env.board_size - 1][lane] = false;
          }
          else if (dir == 2) {
            if (board_oni_row[env.board_size - 1][lane]) {
              removed_oni_turn.push(turn_num);
            }
            for (int i = env.board_size - 1; i > 0; i--) {
              board_oni_row[i][lane] = board_oni_row[i - 1][lane];
              board_fuku_row[i][lane] = board_fuku_row[i - 1][lane];
            }
            board_oni_row[0][lane] = false;
            board_fuku_row[0][lane] = false;
            board_oni_col[lane] <<= 1;
            board_fuku_col[lane] <<= 1;
          }
          else if (dir == 3) {
            if (board_oni_row[lane][env.board_size - 1]) {
              removed_oni_turn.push(turn_num);
            }
            board_oni_row[lane] <<= 1;
            board_fuku_row[lane] <<= 1;
            for (int j = env.board_size - 1; j > 0; j--) {
              board_oni_col[j][lane] = board_oni_col[j - 1][lane];
              board_fuku_col[j][lane] = board_fuku_col[j - 1][lane];
            }
            board_oni_col[0][lane] = false;
            board_fuku_col[0][lane] = false;
          }
          turn_num++;
        }

        void update_board_backward(const char dir, const char lane) {
          turn_num--;
          if (dir == 0) {
            for (int i = env.board_size - 1; i > 0; i--) {
              board_oni_row[i][lane] = board_oni_row[i - 1][lane];
              board_fuku_row[i][lane] = board_fuku_row[i - 1][lane];
            }
            board_oni_row[0][lane] = false;
            board_fuku_row[0][lane] = false;
            board_oni_col[lane] <<= 1;
            board_fuku_col[lane] <<= 1;
            if (!removed_oni_turn.empty() && removed_oni_turn.top() == turn_num) {
              board_oni_row[0][lane] = true;
              board_oni_col[lane][0] = true;
              removed_oni_turn.pop();
            }
          }
          else if (dir == 1) {
            board_oni_row[lane] <<= 1;
            board_fuku_row[lane] <<= 1;
            for (int j = env.board_size - 1; j > 0; j--) {
              board_oni_col[j][lane] = board_oni_col[j - 1][lane];
              board_fuku_col[j][lane] = board_fuku_col[j - 1][lane];
            }
            board_oni_col[0][lane] = false;
            board_fuku_col[0][lane] = false;
            if (!removed_oni_turn.empty() && removed_oni_turn.top() == turn_num) {
              board_oni_row[lane][0] = true;
              board_oni_col[0][lane] = true;
              removed_oni_turn.pop();
            }
          }
          else if (dir == 2) {
            for (int i = 1; i < env.board_size; i++) {
              board_oni_row[i - 1][lane] = board_oni_row[i][lane];
              board_fuku_row[i - 1][lane] = board_fuku_row[i][lane];
            }
            board_oni_row[env.board_size - 1][lane] = false;
            board_fuku_row[env.board_size - 1][lane] = false;
            board_oni_col[lane] >>= 1;
            board_fuku_col[lane] >>= 1;
            if (!removed_oni_turn.empty() && removed_oni_turn.top() == turn_num) {
              board_oni_row[env.board_size - 1][lane] = true;
              board_oni_col[lane][env.board_size - 1] = true;
              removed_oni_turn.pop();
            }
          }
          else if (dir == 3) {
            board_oni_row[lane] >>= 1;
            board_fuku_row[lane] >>= 1;
            for (int j = 1; j < env.board_size; j++) {
              board_oni_col[j - 1][lane] = board_oni_col[j][lane];
              board_fuku_col[j - 1][lane] = board_fuku_col[j][lane];
            }
            board_oni_col[env.board_size - 1][lane] = false;
            board_fuku_col[env.board_size - 1][lane] = false;
            if (!removed_oni_turn.empty() && removed_oni_turn.top() == turn_num) {
              board_oni_row[lane][env.board_size - 1] = true;
              board_oni_col[env.board_size - 1][lane] = true;
              removed_oni_turn.pop();
            }
          }
        }

        int get_oni_score(const char x, const char y) {
          static array<int, 4> scores;
          // up
          {
            scores[0] = (x + 1) * 2 + ((board_fuku_col[y] & bitset<Consts::board_size>((1 << x) - 1)).count() ? 1 : 0);
          }
          // left
          {
            scores[1] = (y + 1) * 2 + ((board_fuku_row[x] & bitset<Consts::board_size>((1 << y) - 1)).count() ? 1 : 0);
          }
          // down
          {
            scores[2] = (env.board_size - x) * 2 + ((board_fuku_col[y] & bitset<Consts::board_size>(~((1 << x) - 1))).count() ? 1 : 0);
          }
          // right
          {
            scores[3] = (env.board_size - y) * 2 + ((board_fuku_row[x] & bitset<Consts::board_size>(~((1 << y) - 1))).count() ? 1 : 0);
          }
          return *min_element(scores.begin(), scores.end());
        }
};

// 探索木（二重連鎖木）のノード
struct Node {
    Action action;
    Evaluator evaluator;
    Hash hash;
    int parent, child, left, right;

    // 根のコンストラクタ
    Node(Action action, const Evaluator& evaluator, Hash hash) :
        action(action),
        evaluator(evaluator),
        hash(hash),
        parent(-1),
        child(-1),
        left(-1),
        right(-1) {}

    // 通常のコンストラクタ
    Node(const Candidate& candidate, int right) :
        action(candidate.action),
        evaluator(candidate.evaluator),
        hash(candidate.hash),
        parent(candidate.parent),
        child(-1),
        left(-1),
        right(right) {}
};

// 二重連鎖木に対する操作をまとめたクラス
class Tree {
    public:
        explicit Tree(const State& state, size_t nodes_capacity, const Node& root) :
            state_(state)
        {
            nodes_.reserve(nodes_capacity);
            root_ = nodes_.push(root);
        }

        // 状態を更新しながら深さ優先探索を行い、次のノードの候補を全てselectorに追加する
        void dfs(Selector& selector) {
            update_root();

            int v = root_;
            while (true) {
                v = move_to_leaf(v);
                state_.expand(nodes_[v].evaluator, nodes_[v].hash, v, selector);
                v = move_to_ancestor(v);
                if (v == root_) {
                    break;
                }
                v = move_to_right(v);
            }
        }

        // 根からノードvまでのパスを取得する
        vector<Action> get_path(int v) {
            // cerr << nodes_.size() << endl;

            vector<Action> path;
            while (nodes_[v].parent != -1) {
                path.push_back(nodes_[v].action);
                v = nodes_[v].parent;
            }
            reverse(path.begin(), path.end());
            return path;
        }

        // 新しいノードを追加する
        int add_leaf(const Candidate& candidate) {
            int parent = candidate.parent;
            int sibling = nodes_[parent].child;
            int v = nodes_.push(Node(candidate, sibling));

            nodes_[parent].child = v;

            if (sibling != -1) {
                nodes_[sibling].left = v;
            }
            return v;
        }

        // ノードvに子がいなかった場合、vと不要な先祖を削除する
        void remove_if_leaf(int v) {
            if (nodes_[v].child == -1) {
                remove_leaf(v);
            }
        }

        // 最も評価がよいノードを返す
        int get_best_leaf(const vector<int>& last_nodes) {
            assert(!last_nodes.empty());
            int ret = last_nodes[0];
            for (int v : last_nodes) {
                if (nodes_[v].evaluator.evaluate() < nodes_[ret].evaluator.evaluate()) {
                    ret = v;
                }
            }
            return ret;
        }

    private:
        State state_;
        ObjectPool<Node> nodes_;
        int root_;

        // 根から一本道の部分は往復しないようにする
        void update_root() {
            int child = nodes_[root_].child;
            while (child != -1 && nodes_[child].right == -1) {
                root_ = child;
                state_.move_forward(nodes_[child].action);
                child = nodes_[child].child;
            }
        }

        // ノードvの子孫で、最も左にある葉に移動する
        int move_to_leaf(int v) {
            int child = nodes_[v].child;
            while (child != -1) {
                v = child;
                state_.move_forward(nodes_[child].action);
                child = nodes_[child].child;
            }
            return v;
        }

        // ノードvの先祖で、右への分岐があるところまで移動する
        int move_to_ancestor(int v) {
            while (v != root_ && nodes_[v].right == -1) {
                state_.move_backward(nodes_[v].action);
                v = nodes_[v].parent;
            }
            return v;
        }

        // ノードvの右のノードに移動する
        int move_to_right(int v) {
            state_.move_backward(nodes_[v].action);
            v = nodes_[v].right;
            state_.move_forward(nodes_[v].action);
            return v;
        }

        // 不要になった葉を再帰的に削除する
        void remove_leaf(int v) {
            while (true) {
                int left = nodes_[v].left;
                int right = nodes_[v].right;
                if (left == -1) {
                    int parent = nodes_[v].parent;

                    if (parent == -1) {
                        cerr << "ERROR: root is removed" << endl;
                        exit(-1);
                    }
                    nodes_.pop(v);
                    nodes_[parent].child = right;
                    if (right != -1) {
                        nodes_[right].left = -1;
                        return;
                    }
                    v = parent;
                } else {
                    nodes_.pop(v);
                    nodes_[left].right = right;
                    if (right != -1) {
                        nodes_[right].left = left;
                    }
                    return;
                }
            }
        }
};

// ビームサーチを行う関数
vector<Action> beam_search(const Config& config, State state, Node root) {
    Tree tree(state, config.nodes_capacity, root);

    // 探索中のノード集合
    vector<int> curr_nodes;
    curr_nodes.reserve(config.beam_width);
    // 本来は curr_nodes = {state.root_} とすべきだが, 省略しても問題ない

    // 新しいノードの集合
    vector<int> next_nodes;
    next_nodes.reserve(config.beam_width);

    // 新しいノード候補の集合
    Selector selector(config);

    for (int turn = 0; turn < config.max_turn; ++turn) {
        // Euler Tour で selector に候補を追加する
        tree.dfs(selector);

        if (selector.have_finished()) {
            // ターン数最小化型の問題で実行可能解が見つかったとき
            Candidate candidate = selector.get_finished_candidates()[0];
            vector<Action> ret = tree.get_path(candidate.parent);
            ret.push_back(candidate.action);
            return ret;
        }
        // 新しいノードを追加する
        for (const Candidate& candidate : selector.select()) {
            next_nodes.push_back(tree.add_leaf(candidate));
        }
        if (next_nodes.empty()) {
            // 新しいノードがないとき
            cerr << "ERROR: Failed to find any valid solution" << endl;
            return {};
        }
        // 不要なノードを再帰的に削除する
        for (int v : curr_nodes) {
            tree.remove_if_leaf(v);
        }
        // ダブルバッファリングで配列を使い回す
        swap(curr_nodes, next_nodes);
        next_nodes.clear();

        selector.clear();
    }
    // ターン数固定型の問題で全ターンが終了したとき
    int best_leaf = tree.get_best_leaf(curr_nodes);
    return tree.get_path(best_leaf);
}

} // namespace beam_search

Output solve_beam(const Environment &env) {
  const int max_turn = env.max_turn_limit;
  const size_t beam_width = 2048;
  const size_t nodes_capacity = 256 * beam_width;
  const uint32_t hash_map_capacity = 512 * beam_width;
  beam_search::Config config = {
    max_turn,
    beam_width,
    nodes_capacity,
    hash_map_capacity
  };
  beam_search::init_hash(env);
  beam_search::State state(env);
  beam_search::Node root(beam_search::Action(0, 0), beam_search::Evaluator(0), 0);
  auto actions = beam_search::beam_search(config, state, root);

  Output output;
  for (const auto &action: actions) {
    output.actions.emplace_back(Adj4::ch[action.dir], action.lane);
  }
  return output;
}

Output solve_hc(const Environment &env, const Output &prev_output) {
  auto evaluate = [&](const vector<vector<char>> &init_board, const vector<pair<char, int>> &actions) {
    auto board = init_board;
    for (const auto &[dir, lane]: actions) {
      if (dir == 'U') {
        if (board[0][lane] == 'o') return false;
        for (int i = 1; i < env.board_size; i++) {
          board[i - 1][lane] = board[i][lane];
        }
        board[env.board_size - 1][lane] = '.';
      } else if (dir == 'L') {
        if (board[lane][0] == 'o') return false;
        for (int j = 1; j < env.board_size; j++) {
          board[lane][j - 1] = board[lane][j];
        }
        board[lane][env.board_size - 1] = '.';
      } else if (dir == 'D') {
        if (board[env.board_size - 1][lane] == 'o') return false;
        for (int i = env.board_size - 1; i > 0; i--) {
          board[i][lane] = board[i - 1][lane];
        }
        board[0][lane] = '.';
      } else if (dir == 'R') {
        if (board[lane][env.board_size - 1] == 'o') return false;
        for (int j = env.board_size - 1; j > 0; j--) {
          board[lane][j] = board[lane][j - 1];
        }
        board[lane][0] = '.';
      }
    }
    for (int i = 0; i < env.board_size; i++) {
      for (int j = 0; j < env.board_size; j++) {
        if (board[i][j] == 'x') return false;
      }
    }
    return true;
  };

  auto actions = prev_output.actions;

  while (theTimer.time() < 1.980) {
    double roll = theRandom.nextDouble();
    if (roll < 0.25) {
      int i1 = theRandom.nextUIntMod(actions.size());
      int i2 = theRandom.nextUIntMod(actions.size());
      swap(actions[i1], actions[i2]);
      if (!evaluate(env.init_board, actions)) {
        swap(actions[i1], actions[i2]);
      }
    }
    else if (roll < 0.50) {
      int i1 = theRandom.nextUIntMod(actions.size());
      int i2 = theRandom.nextUIntMod(actions.size());
      auto action_to_move = actions[i1];
      actions.erase(actions.begin() + i1);
      actions.insert(actions.begin() + i2, action_to_move);
      if (!evaluate(env.init_board, actions)) {
        actions.erase(actions.begin() + i2);
        actions.insert(actions.begin() + i1, action_to_move);
      }
    }
    else if (roll < 0.75) {
      int i = theRandom.nextUIntMod(actions.size());
      int d = theRandom.nextUIntMod(4);
      int l = theRandom.nextUIntMod(env.board_size);
      auto prev_action = actions[i];
      actions[i] = make_pair(Adj4::ch[d], l);
      if (!evaluate(env.init_board, actions)) {
        actions[i] = prev_action;
      }
    }
    else if (roll < 0.9) {
      int i1 = theRandom.nextUIntMod(actions.size());
      int i2 = theRandom.nextUIntMod(actions.size());
      int d = theRandom.nextUIntMod(4);
      int l = theRandom.nextUIntMod(env.board_size);
      auto action_to_erase = actions[i1];
      actions.erase(actions.begin() + i1);
      actions.insert(actions.begin() + i2, make_pair(Adj4::ch[d], l));
      if (!evaluate(env.init_board, actions)) {
        actions.erase(actions.begin() + i2);
        actions.insert(actions.begin() + i1, action_to_erase);
      }
    }
    else if (roll < 1.0) {
      int i = theRandom.nextUIntMod(actions.size());
      auto action_to_erase = actions[i];
      actions.erase(actions.begin() + i);
      if (!evaluate(env.init_board, actions)) {
        actions.insert(actions.begin() + i, action_to_erase);
      }
    }
  }

  Output output;
  output.actions = actions;
  return output;
}

Output solve(const Environment &env) {
  Output output1 = solve_beam(env);
  Output output2 = solve_hc(env, output1);
  return output2;
}

int main(int argc, char *argv[]) {
  cin.tie(0);
  ios::sync_with_stdio(false);

  HyperParameter::load_hyper_parameter(argc, argv);

  const auto input = get_input();
  const auto env = create_environment(input);
  const auto output = solve(env);
  print_output(output);

  return 0;
}
