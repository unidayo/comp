/*

bonus param
投げたら下がるかも

100 cases mean : 914325.98

*/

#ifdef LOCAL
    #define _GLIBCXX_DEBUG
    #define __clock__
#else
    #pragma GCC optimize("Ofast")
#endif
#include<bits/stdc++.h>
using namespace std;
using ll = long long;
using VI = vector<ll>;
using VV = vector<VI>;
using VS = vector<string>;
using PII = pair<ll, ll>;

// #define INT128 // 必要なら有効化してください
#ifdef INT128
  using LL = __int128;
#endif

// tourist set
template <typename A, typename B>
string to_string(pair<A, B> p);

template <typename A, typename B, typename C>
string to_string(tuple<A, B, C> p);

template <typename A, typename B, typename C, typename D>
string to_string(tuple<A, B, C, D> p);

string to_string(const string& s) {
  return '"' + s + '"';
}

string to_string(const char* s) {
  return to_string((string) s);
}

string to_string(bool b) {
  return (b ? "true" : "false");
}

string to_string(char c){
  string s = {c};
  return s;
}

// LL
#ifdef INT128
// input
istream& operator>>(istream& is, LL& v)
{
    string s;
    is >> s;
    v = 0;
    for (int i = 0; i < (int)s.size(); i++) {
        if (isdigit(s[i])) { v = v * 10 + s[i] - '0'; }
    }
    if (s[0] == '-') { v *= -1; }
    return is;
}
std::ostream &operator<<(std::ostream &dest, LL value) {
  std::ostream::sentry s(dest);
  if (s) {
    LL tmp = value < 0 ? -value : value;
    char buffer[128];
    char *d = std::end(buffer);
    do {
      --d;
      *d = "0123456789"[tmp % 10];
      tmp /= 10;
    } while (tmp != 0);
    if (value < 0) {
      --d;
      *d = '-';
    }
    int len = std::end(buffer) - d;
    if (dest.rdbuf()->sputn(d, len) != len) {
      dest.setstate(std::ios_base::badbit);
    }
  }
  return dest;
}
string to_string(LL v){
  stringstream ss;
  ss << v;
  return ss.str();
}
#endif // LL

string to_string(vector<bool> v) {
  bool first = true;
  string res = "{";
  for (int i = 0; i < static_cast<int>(v.size()); i++) {
    if (!first) {
      res += ", ";
    }
    first = false;
    res += to_string(v[i]);
  }
  res += "}";
  return res;
}

template <size_t N>
string to_string(bitset<N> v) {
  string res = "";
  for (size_t i = 0; i < N; i++) {
    res += static_cast<char>('0' + v[i]);
  }
  return res;
}

template <typename A>
string to_string(A v) {
  bool first = true;
  string res = "{";
  for (const auto &x : v) {
    if (!first) {
      res += ", ";
    }
    first = false;
    res += to_string(x);
  }
  res += "}";
  return res;
}

template <typename A, typename B>
string to_string(pair<A, B> p) {
  return "(" + to_string(p.first) + ", " + to_string(p.second) + ")";
}

template <typename A, typename B, typename C>
string to_string(tuple<A, B, C> p) {
  return "(" + to_string(get<0>(p)) + ", " + to_string(get<1>(p)) + ", " + to_string(get<2>(p)) + ")";
}

template <typename A, typename B, typename C, typename D>
string to_string(tuple<A, B, C, D> p) {
  return "(" + to_string(get<0>(p)) + ", " + to_string(get<1>(p)) + ", " + to_string(get<2>(p)) + ", " + to_string(get<3>(p)) + ")";
}

void debug_out() { cerr << '\n'; }

template <typename Head, typename... Tail>
void debug_out(Head H, Tail... T) {
  cerr << " " << to_string(H);
  debug_out(T...);
}

#ifdef LOCAL
#define debug(...) cerr << "[" << #__VA_ARGS__ << "]:", debug_out(__VA_ARGS__)
#else
#define debug(...) 42
#endif
// tourist set end

template<class T>bool chmax(T &a, const T &b) { if (a<b) { a=b; return 1; } return 0; }
template<class T>bool chmin(T &a, const T &b) { if (b<a) { a=b; return 1; } return 0; }

#define FOR(i,a,b) for(ll i=(a);i<(b);++i)
#define rep(i,b) FOR(i, 0, b)
#define ALL(v) (v).begin(), (v).end()
#define p(s) cout<<(s)<<'\n'
#define p2(s, t) cout << (s) << " " << (t) << '\n'
#define SZ(x) ((int)(x).size())
#define SORT(A) sort(ALL(A))
#define RSORT(A) sort(ALL(A), greater<ll>())

ll SUM(VI& V){
  return accumulate(ALL(V), 0LL);
}

ll MIN(VI& V){return *min_element(ALL(V));}
ll MAX(VI& V){return *max_element(ALL(V));}

void print_vector(VI& V, ll offset=0){
  ll n = V.size();
  rep(i, n){
    if(i) cout << ' ';
    cout << V[i]+offset;
  }
  cout << endl;
}

ll gcd(ll a,ll b){
    if(b == 0) return a;
    return gcd(b,a%b);
}

ll lcm(ll a,ll b){
    ll g = gcd(a,b);
    return a / g * b;
}

// long double
using ld = long double;
// #define EPS (1e-14)
constexpr ld EPS = 1e-14;
// #define equals(a,b) (fabs((a)-(b)) < EPS)
constexpr bool equals(ld a, ld b){return fabs((a)-(b)) < EPS;}

// 小さい順に取り出すpriority queue
using inverse_priority_queue = priority_queue<ll, vector<ll>, greater<ll> >;

int popcount(ll t){
    return __builtin_popcountll(t);
}

const ll mod = 1e9 + 7;
// const ll mod = 998244353;
// const ll inf = 4e18; // LLONG_MAX = 9223372036854775807 (atcoder, codeforces)
const ll inf = 1e9;
const double PI = acos(-1);

// [a/b] (繰り上げ)
ll ceil_div(ll a, ll b){
  return (a+b-1)/b;
}

// 累乗※modなし
ll ll_pow(ll a, ll n){
  ll ans = 1;
  rep(i,n){
    ans *= a;
  }
  return ans;
}

// --- Vec2
struct Vec2{
  ll y,x;
  bool operator<(const Vec2 &another) const{ // 適当比較関数
    PII a = {y,x};
    PII b = {another.y, another.x};
    // return x+y < another.x+another.y; // これだとsetに入れた時に消える(違うのに同一視される)
    return a<b;
  }
  bool operator==(const Vec2 &another) const{
    return x==another.x && y==another.y;
  }
  bool operator!=(const Vec2 &another) const{
    return x!=another.x or y!=another.y;
  }
};
string to_string(Vec2 v) {
  stringstream ss;
  ss << '(' << v.y << ',' << v.x << ')';
  return ss.str();
}
ll L1(Vec2 a, Vec2 b){
  return abs(a.y-b.y) + abs(a.x-b.x);
}
// --- Vec2

// random ---
// [a,b]
ll rand_range(ll a, ll b){
  assert(a<=b);
  ll d = b-a+1;
  ll r = rand()%d;
  return a+r;
}
// [0,1)
double random_double(){
  return (double)rand() / RAND_MAX;
};

#define SHUFFLE(A) std::random_device rd;std::mt19937 g(rd());shuffle(ALL(A),g);

VI random_indexes(ll L){
  VI I;
  rep(i,L)I.push_back(i);
  SHUFFLE(I);
  return I;  
}
// --- random

// time ---
#include<chrono>
std::chrono::_V2::system_clock::time_point get_time(){
  return std::chrono::system_clock::now();
}
ll past_milliseconds(std::chrono::_V2::system_clock::time_point _start_time){
  auto current_time = get_time();
  auto spent_ms = std::chrono::duration_cast<std::chrono::milliseconds>(current_time-_start_time).count();
  return (ll)spent_ms;  
}
std::chrono::_V2::system_clock::time_point start_time;
ll TIME_LIMIT_MS=2000;
ll BUFFER_TIME=100;
// --- time

VI loadV(ll N){
  VI A(N);
  rep(i,N)cin>>A[i];
  return A;
}

int dx[4] = {-1, 0, 1,  0};
int dy[4] = { 0, 1, 0, -1};

bool in_range(ll i, ll j){
  if(0<=i && i<6 && 0<=j && j<6){
    return true;
  }else{
    return false;
  }
}

ll N; // 6 x 6
ll M; // 15 種類
ll T; // turn

VV load_seeds(){
  ll L = 2*N*(N-1);
  VV ret;
  rep(i,L){
    VI A = loadV(M);
    for(ll a : A){
      if(a<0 or a>100){
        debug(i, a); exit(0);
      }
    }
    for(ll a : A)assert(0<=a && a<=100);
    ret.push_back(A);
  }
  return ret;
}

vector<Vec2> get_order(){
  vector<Vec2> ret;
  // 4
  ret.push_back({2,2});
  ret.push_back({3,3});
  ret.push_back({2,3});
  ret.push_back({3,2});

  ret.push_back({1,2});
  ret.push_back({4,3});
  ret.push_back({2,4});
  ret.push_back({3,1});
  
  ret.push_back({1,3});
  ret.push_back({4,2});
  ret.push_back({3,4});
  ret.push_back({2,1});

  // corner
  ret.push_back({1,1});
  ret.push_back({4,4});
  ret.push_back({1,4});
  ret.push_back({4,1});

  ret.push_back({0,2});
  ret.push_back({5,3});
  ret.push_back({2,5});
  ret.push_back({3,0});
  ret.push_back({0,3});
  ret.push_back({5,2});
  ret.push_back({3,5});
  ret.push_back({2,0});
  
  ret.push_back({0,1});
  ret.push_back({5,4});
  ret.push_back({1,5});
  ret.push_back({4,0});
  ret.push_back({0,4});
  ret.push_back({5,1});
  ret.push_back({4,5});
  ret.push_back({1,0});

  ret.push_back({0,0});
  ret.push_back({5,5});
  ret.push_back({5,0});
  ret.push_back({0,5});
  
  set<Vec2> se;
  for(auto a : ret)se.insert(a);
  assert(se.size()==36);
  return ret;
}

// (2,2)の近くに最強を集める // まさか悪化するとは
vector<Vec2> get_order2(){
  Vec2 center = {2,2};

  vector<pair<ll,Vec2>> temp;
  rep(i,6){
    rep(j,6){
      Vec2 now = {i,j};
      ll dist = L1(now,center);
      temp.push_back({dist, now});
    }
  }
  sort(ALL(temp));
  vector<Vec2> ret;
  for(auto pa : temp){
    ret.push_back(pa.second);
  }
  return ret;
}

// うずまき
vector<Vec2> get_order3(){
  vector<Vec2> ret;
  ret.push_back({2,2});
  ret.push_back({3,2});
  ret.push_back({3,3});
  ret.push_back({2,3});
  ret.push_back({1,3});
  ret.push_back({1,2});
  ret.push_back({1,1});
  ret.push_back({2,1});
  ret.push_back({3,1});
  ret.push_back({4,1});
  ret.push_back({4,2});
  ret.push_back({4,3});
  ret.push_back({4,4});
  ret.push_back({3,4});
  ret.push_back({2,4});
  ret.push_back({1,4});
  ret.push_back({0,4});
  ret.push_back({0,3});
  ret.push_back({0,2});
  ret.push_back({0,1});
  ret.push_back({0,0});
  ret.push_back({1,0});
  ret.push_back({2,0});
  ret.push_back({3,0});
  ret.push_back({4,0});
  ret.push_back({5,0});
  ret.push_back({5,1});
  ret.push_back({5,2});
  ret.push_back({5,3});
  ret.push_back({5,4});
  ret.push_back({5,5});
  ret.push_back({4,5});
  ret.push_back({3,5});
  ret.push_back({2,5});
  ret.push_back({1,5});
  ret.push_back({0,5});
  return ret;
}

// VV load_rin(){
//   VV ret;
//   ret.push_back({2,3,3,3,3,2});
//   ret.push_back({3,4,4,4,4,3});
//   ret.push_back({3,4,4,4,4,3});
//   ret.push_back({3,4,4,4,4,3});
//   ret.push_back({3,4,4,4,4,3});
//   ret.push_back({2,3,3,3,3,2});
//   return ret;
// }

int main(){
    cin.tie(0);
    ios::sync_with_stdio(false);
    srand(777);
    start_time = get_time();

    cin>>N>>M>>T;
    debug(N,M,T);

    // VV RIN = load_rin();

    auto order = get_order();

    rep(t,T){
      debug(t);
      // どこにどれを植えますか
      auto seeds = load_seeds();
      ll seed_num = SZ(seeds);
      debug(seed_num);

      // 各要素のMaxを求めておく
      VI maxes(M);
      for(auto seed : seeds){
        rep(i,M){
          chmax(maxes[i], seed[i]);
        }
      }

      // 各要素のMaxを持っている種の数
      VI max_count(M);
      for(auto seed : seeds){
        rep(i,M){
          if(seed[i]==maxes[i])max_count[i]++;
        }
      }
      for(ll v : max_count){
        assert(v>0);
      }

      // 評価値を前計算
      VI sums(seed_num);
      rep(i,seed_num){
        sums[i] = SUM(seeds[i]);

        // ここの2つのパラメータは片方上げたら、片方下げる
        ll bonus = 32; // マックスを持っている種は高評価したい
        rep(j,M){
          // j番目の要素のrarity
          ll rarity = 150/max_count[j];
          if(seeds[i][j]==maxes[j]){
            sums[i] += bonus + rarity;
          }
        }
      }
      debug("precalc done");

      // 種間の違い
      auto calc_diff_between_seeds = [&](ll idx0, ll idx1){
        VI A;
        rep(i,M){
          ll diff = abs(seeds[idx0][i] - seeds[idx1][i]);
          A.push_back(diff);
        }
        // ll param=M;
        ll param=M/2 - 1; // 上位いくつかだけ取ればいいのでは？
        RSORT(A);
        A.resize(param);
        return SUM(A);
      };

      VI is_seed_used(seed_num);

      VV Ans(N,VI(N,-1));
      for(auto seed_pos : order){
        // 位置は固定とする
        vector<pair<double,ll>> temp; // 評価値・種Index
        rep(seed_index,60){
          // どの種にしようかな？
          if(is_seed_used[seed_index])continue;
          double evaluation = sums[seed_index]; // 合計が評価値のベース

          // 隣接見る
          rep(dir,4){
            ll ny = seed_pos.y + dy[dir];
            ll nx = seed_pos.x + dx[dir];
            if(!in_range(ny,nx))continue;
            if(Ans[ny][nx]==-1)continue;
            // 隣接がある場合
            ll ajacent_seed_index = Ans[ny][nx];
            ll diff = calc_diff_between_seeds(seed_index, ajacent_seed_index);
            double param = 1.2; // 種間の違いをどれだけ重視するか？のパラメータ
            // if(RIN[seed_pos.y][seed_pos.x]==3)param=1.2*1.1; // 端っこは強いやつを残すために使いたくない？
            // if(RIN[seed_pos.y][seed_pos.x]==2)param=1.2*1.2; // 端っこは
            evaluation += param * diff; // 評価値
          }
          temp.push_back({evaluation,seed_index});
        }
        assert(temp.size()>0);
        sort(ALL(temp));
        ll best_seed_index = temp.back().second;
        is_seed_used[best_seed_index]=1;
        Ans[seed_pos.y][seed_pos.x] = best_seed_index;
      }

      // print
      rep(i,N){
        print_vector(Ans[i]);
        debug(Ans[i]);
      }
      cout<<flush;
    }
    
    return 0;
}