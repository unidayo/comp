#ifndef KOMAKI_LOCAL
#define NDEBUG
#endif

#include <bits/stdc++.h>
#include <sys/time.h>
#include <unistd.h>
using namespace std;
#define i64         int64_t
#define rep(i, n)   for(int i = 0; (i) < ((int)(n)); ++(i))
#define bit(n)      (((i64)1)<<((i64)(n)))
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
#define DBG_OVERLOAD(_1, _2, _3, _4, _5, _6, macro_name, ...) macro_name
#define DBG_LINE() { char s[99]; sprintf(s, "line:%3d | ", __LINE__); DBG_OUT << s; }
#define DBG_OUTPUT(v) { DBG_OUT << (#v) << "=" << (v); }
#define DBG1(v, ...) { DBG_OUTPUT(v); }
#define DBG2(v, ...) { DBG_OUTPUT(v); DBG_OUT << ", "; DBG1(__VA_ARGS__); }
#define DBG3(v, ...) { DBG_OUTPUT(v); DBG_OUT << ", "; DBG2(__VA_ARGS__); }
#define DBG4(v, ...) { DBG_OUTPUT(v); DBG_OUT << ", "; DBG3(__VA_ARGS__); }
#define DBG5(v, ...) { DBG_OUTPUT(v); DBG_OUT << ", "; DBG4(__VA_ARGS__); }
#define DBG6(v, ...) { DBG_OUTPUT(v); DBG_OUT << ", "; DBG5(__VA_ARGS__); }

#define DEBUG0() { DBG_LINE(); DBG_OUT << std::endl; }
#define DEBUG(...)                                                      \
  {                                                                     \
    DBG_LINE();                                                         \
    DBG_OVERLOAD(__VA_ARGS__, DBG6, DBG5, DBG4, DBG3, DBG2, DBG1)(__VA_ARGS__); \
    DBG_OUT << std::endl;                                               \
  }


class Timer
{
public:
    void restart();
    double getElapsed();

    Timer();
private:
    static double rdtsc_per_sec_inv;

    double getTimeOfDay();
    unsigned long long int getCycle();

    double start_time;
    unsigned long long int start_clock;
};
double Timer::rdtsc_per_sec_inv = -1;


inline double Timer::getElapsed()
{
  if(rdtsc_per_sec_inv != -1) return (double)(getCycle() - start_clock) * rdtsc_per_sec_inv;

  const double RDTSC_MEASUREMENT_INTERVAL = 0.1;
  double res = getTimeOfDay() - start_time;
  if(res <= RDTSC_MEASUREMENT_INTERVAL) return res;

  rdtsc_per_sec_inv = 1.0 / (getCycle() - start_clock);
  rdtsc_per_sec_inv *= getTimeOfDay() - start_time;
  return getElapsed();
}

inline void Timer::restart()
{
  start_time = getTimeOfDay();
  start_clock = getCycle();
}

Timer::Timer()
{
  restart();
}

inline double Timer::getTimeOfDay()
{
  timeval tv;
  gettimeofday(&tv,0);
  return tv.tv_sec + tv.tv_usec * 0.000001;
}

inline unsigned long long int Timer::getCycle()
{
  unsigned int low, high;
  __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
  return ((unsigned long long int)low) | ((unsigned long long int)high << 32);
}

class XorShift{
public:
    static int rand();
    static double rand_double();
private:
    static unsigned int x;
    static unsigned int y;
    static unsigned int z;
    static unsigned int w;
    static unsigned int t;
};
unsigned int XorShift::x = 123456789;
unsigned int XorShift::y = 362436069;
unsigned int XorShift::z = 521288629;
unsigned int XorShift::w = 88675123;
unsigned int XorShift::t = 1;

int XorShift::rand()
{
  t = x ^ (x << 11);
  x = y;
  y = z;
  z = w;
  w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
  return w & 0x7fffffff;
}

double XorShift::rand_double()
{
  return rand() % 1000000000 / (1000000000.0 - 1);
}




class Line
{
public:
    Line(string s, vector<int> ids) : s(s), ids(ids) {}

    string s;
    vector<int> ids;
};

class Result
{
public:
    double score;
    vector<Line> lines;
};

i64 get_rand(vector<int> &v)
{
  return v[XorShift::rand() % sz(v)];
}

class FastString
{
public:
    FastString(string s)
    {
      hash = 0;
      rep(i, sz(s)) hash = (hash << 3) | (s[i] - 'A');
      len = sz(s);
    }

    i64 get_suffix_hash(int l)
    {
      return hash & (bit(3 * l) - 1);
    }

    i64 get_prefix_hash(int l)
    {
      return hash >> (3 * (len - l));
    }

    void append(int overlap, string s)
    {
      rep(i, sz(s))if(overlap <= i){
        if(len < 20) {
          ++len;
          hash = (hash << 3) + (s[i] - 'A');
        }
      }
    }

    bool can_add(string s)
    {
      return can_add(0, s);
    }

    bool can_add(int overlap, string s)
    {
      rep(i, sz(s)){
        int pos = len - overlap + i;
        if(pos < 20) continue;
        if(s[i] != 'A' + get_char(pos % 20)) return false;
      }
      return true;
    }

    int get_char(int pos)
    {
      return (hash >> ((len - (pos + 1)) * 3)) & 7;
    }

    string to_string()
    {
      stringstream ss;
      rep(i, len) ss << (char) (get_char(i) + 'A');
      return ss.str();
    }

    string to_string_20()
    {
      stringstream ss;
      ss << to_string();
      rep(i, 20 - len) ss << (char) ('A' + XorShift::rand() % 8);
      return ss.str();
    }

    i64 hash;
    int len;
};

bool contains(string &s, string &query)
{
  rep(begin, sz(s)){
    bool ok = true;
    rep(i, sz(query)){
      if(query[i] == s[(begin + i) % sz(s)]) continue;
      ok = false;
      break;
    }
    if(ok) return true;
  }
  return false;
}

bool contains_test(string s, string q)
{
  return contains(s, q);
}

void test_fast_string()
{
  {
    FastString s(string(18, 'A'));
    assert(s.can_add("A"));
    assert(s.can_add("AA"));
    assert(s.can_add("AAA"));
    assert(s.can_add("AAAA"));
    assert(s.can_add("AAAAA"));
    assert(s.can_add("B"));
    assert(s.can_add("BB"));
    assert(s.can_add(1, "ABB"));
    assert(!s.can_add("BBB"));
    assert(!s.can_add("BBBB"));
    assert(!s.can_add("BBBBB"));
    // DEBUG(s.to_string());
  }
  {
    FastString s(string(18, 'B'));
    assert(s.can_add("B"));
    assert(s.can_add("BB"));
    assert(s.can_add("BBB"));
    assert(s.can_add("BBBB"));
    assert(s.can_add("BBBBB"));
    assert(s.can_add("A"));
    assert(s.can_add("AA"));
    assert(s.can_add(1, "BAA"));
    assert(!s.can_add("AAA"));
    assert(!s.can_add("AAAA"));
    assert(!s.can_add("AAAAA"));
    // DEBUG(s.to_string());
  }
  {
    FastString s("");
    s.append(0, "AA");
    s.append(0, "BB");
    s.append(0, "CC");
    rep(step, 12) s.append(2, "CCC");
    assert(s.can_add("A"));
    assert(s.can_add("AA"));
    assert(s.can_add("AAA"));
    assert(s.can_add("AAAA"));
    assert(s.can_add("AAAAB"));
    assert(s.can_add("AAAABBC"));
    assert(s.can_add("AAAABBCC"));
    assert(s.can_add("B"));
    assert(s.can_add("BB"));
    assert(!s.can_add("BBB"));
    assert(!s.can_add("BBBB"));
    // DEBUG(s.to_string());
  }
  {
    FastString s("");
    s.append(0, "AA");
    s.append(0, "BB");
    s.append(0, "CC");
    rep(step, 12) s.append(2, "CCC");
    s.append(1, "CAAAABBC");

    FastString s2("");
    s2.append(0, "AA");
    s2.append(0, "BB");
    s2.append(0, "CC");
    rep(step, 12) s2.append(2, "CCC");
    s2.append(0, "AA");
    // DEBUG(s.to_string());
    // DEBUG(s2.to_string());
    assert(s.len == s2.len);
    assert(s.hash == s2.hash);
  }
  {
    assert(contains_test("ABC", "A"));
    assert(contains_test("ABC", "B"));
    assert(contains_test("ABC", "C"));
    assert(!contains_test("ABC", "CC"));
    assert(contains_test("ABC", "CA"));
    assert(!contains_test("ABC", "CB"));
    assert(contains_test("AABAAC", "AA"));
    assert(contains_test("AABAAC", "AAB"));
    assert(contains_test("AABAAC", "AAC"));
    assert(!contains_test("AABAAC", "AABB"));
    assert(!contains_test("AABAAC", "AACC"));
    assert(contains_test("AABAAC", "AACAAB"));
    assert(contains_test("AABAAC", "ACAABA"));
    assert(!contains_test("AABAAC", "AACAB"));
    assert(!contains_test("AABAAC", "ACAAA"));
  }
}

vector<Line> gen_lines(vector<string> &terms, vector<int> is_term_used)
{
  test_fast_string();

  const int L = 20;
  unordered_map<i64, vector<int>> prefixes[L];
  rep(term_i, sz(terms)){//if(!is_term_used[term_i]){
    FastString s(terms[term_i]);
    for(int len = 0; len <= s.len; ++len){
      prefixes[len][s.get_prefix_hash(len)].push_back(term_i);
    }
  }

  vector<Line> lines;
  rep(step, 3) {
    rep(term_i, sz(terms)) {
      if (!is_term_used[term_i]) {
        FastString s(terms[term_i]);
        unordered_set<int> ids({term_i});
        while (s.len < 20) {
          int best_score = -1;
          int best_len = -1;
          int best_id = -1;
          string best_term;
          for (int len = s.len; 0 <= len; --len) {
            if (prefixes[len].count(s.get_suffix_hash(len)) == 0) continue;
            rep(loop, 2) {
              int cand = get_rand(prefixes[len][s.get_suffix_hash(len)]);
              if (ids.count(cand)) continue;
              if (sz(terms[cand]) <= len) continue;
              if (!s.can_add(len, terms[cand])) continue;
              int score = len;
              if(step == 1) score = 100 - (sz(terms[cand]) - len);
              if(step == 2) score = 100 - sz(terms[cand]);
              if(best_score < score) {
                best_score = score;
                best_id = cand;
                best_len = len;
                best_term = terms[cand];
              }
            }
          }
          if(best_score == -1) break;
          ids.insert(best_id);
          s.append(best_len, best_term);
        }

        string line = s.to_string_20();
        vector<int> ids_vec;
        rep(i, sz(terms))if (contains(line, terms[i])) ids_vec.push_back(i);
        lines.emplace_back(Line(line, ids_vec));
      }
    }
  }
  return lines;
}

Result simulated_annealing(vector<Line> &lines, Timer &timer, double time_limit, vector<string> &terms)
{
  int score = 0;
  vector<int> counts(sz(terms), 0);
  vector<int> term_scores;
  rep(i, sz(terms)) term_scores.push_back(1000 + sz(terms[i]));

  auto add_ids = [&](int line_id){
      for(int id: lines[line_id].ids){
        if(counts[id] == 0) score += term_scores[id];
        ++counts[id];
      }
  };

  auto remove_ids = [&](int line_id){
      for(int id: lines[line_id].ids){
        --counts[id];
        if(counts[id] == 0) score -= term_scores[id];
      }
  };

  vector<int> line_ids;
  rep(i, sz(lines)) line_ids.push_back(i);
  rep(i, 20) add_ids(line_ids[i]);

  double begin_time = timer.getElapsed();
  int cnt = 0;
  while(true){
    ++cnt;
    double rem_time = (time_limit - timer.getElapsed());
    if(rem_time <= 0) break;
    double temperature = rem_time / (time_limit - begin_time);

    int pos0 = XorShift::rand() % 20;
    int pos1 = XorShift::rand() % (sz(lines) - 20) + 20;
    double prev_score = score;
    remove_ids(line_ids[pos0]);
    add_ids(line_ids[pos1]);
    double score_diff = score - prev_score;
    if(0 <= score_diff || XorShift::rand_double() < exp(score_diff / temperature)){
      swap(line_ids[pos0], line_ids[pos1]);
    }else{
      add_ids(line_ids[pos0]);
      remove_ids(line_ids[pos1]);
    }
  }
  DEBUG(cnt);
  //DEBUG(score, num_terms);
  //rep(i, 100) DEBUG(i, counts[i]);
  DEBUG((double) score / sz(terms));

  Result result;
  result.score = score;
  rep(i, 20) result.lines.push_back(lines[line_ids[i]]);
  return result;
}

vector<string> read_inputs()
{
  int k;
  cin >> k >> k;
  vector<string> inputs(k);
  rep(i, k) cin >> inputs[i];
  DEBUG(k, sz(inputs[0]));
  return inputs;
}

class FinalResult
{
public:
    int score;
    vector<string> lines;
};

FinalResult vertical_optimization(vector<string> &lines, vector<string> &terms, Timer timer, double time_limit)
{
  int board[20][20];
  int board_trans[20][20];
  vector<int> line_indexes;
  rep(i, 20) line_indexes.push_back(i);
  rep(i, 20) swap(line_indexes[i], line_indexes[XorShift::rand() % (i + 1)]);
  vector<int> begin_positions;
  rep(i, 20) begin_positions.push_back(i);

  int max_term_length = 0;
  vector<i64> term_hashes;
  for(string &term: terms){
    i64 hash = 0;
    rep(i, sz(term)) hash = (hash << 3) | (term[i] - 'A');
    term_hashes.push_back(hash);
    max_term_length = max(max_term_length, sz(term));
  }

  auto populate_board = [&](){
      rep(i, 20){
        string &line = lines[line_indexes[i]];
        int y = begin_positions[i];
        rep(j, 20){
          board[i][j] = board_trans[j][i] = line[y] - 'A';
          y = (y + 1) % 20;
        }
      }
  };

  unordered_set<i64> hashes[20];
  auto score_board = [&](){
    rep(i, 20) hashes[i].clear();
    rep(x, 20)rep(y, 20){
        {
          i64 hash = 0;
          int cy = y;
          rep(len, max_term_length) {
            hash = (hash << 3) | board[x][cy];
            hashes[len].insert(hash);
            cy = (cy + 1) % 20;
          }
        }
        {
          i64 hash = 0;
          int cx = x;
          rep(len, max_term_length){
            hash = (hash << 3) | board_trans[y][cx];
            hashes[len].insert(hash);
            cx = (cx + 1) % 20;
          }
        }
    }
    int count = 0;
    rep(term_i, sz(terms)){
      if(hashes[sz(terms[term_i]) - 1].count(term_hashes[term_i])) ++count;
    }
    return count;
  };

  populate_board();
  int curr_score = score_board();
  DEBUG(curr_score);

  int cnt = 0;
  while(timer.getElapsed() < time_limit){
    ++cnt;
    int op = XorShift::rand() % 2;
    if(op == 0){
      int line_index = XorShift::rand() % 20;
      int prev_position = begin_positions[line_index];
      int next_position = XorShift::rand() % 20;
      if(prev_position == next_position) continue;
      begin_positions[line_index] = next_position;
      populate_board();
      int score = score_board();
      if(curr_score <= score){
        // if(curr_score < score) DEBUG(score);
        curr_score = score;
      }else{
        begin_positions[line_index] = prev_position;
      }
    }else{

    }
  }
  DEBUG(cnt, curr_score);

  populate_board();
  vector<string> results;
  rep(i, 20){
    stringstream ss;
    rep(j, 20) ss << (char) (board[i][j] + 'A');
    results.push_back(ss.str());
  }
  return FinalResult{curr_score, results};
}

void run()
{
  Timer timer;
  vector<string> inputs = read_inputs();
  vector<Result> results;
  {
    vector<Line> lines;
    vector<double> time_limits = {0.5, 1.0, 1.5};
    for (double time_limit: time_limits) {
      vector<int> is_term_used(sz(inputs), false);
      if (sz(results)) {
        rep(i, 20)for (int id: results.back().lines[i].ids) is_term_used[id] = true;
      }

      rep(step, 2) {
        for (Line &line: gen_lines(inputs, is_term_used)) {
          lines.push_back(line);
        }
        vector<Line> next_lines;
        unordered_set<string> s;
        for(Line &line: lines){
          if(s.count(line.s)) continue;
          s.insert(line.s);
          next_lines.push_back(line);
        }
        DEBUG(sz(next_lines), sz(lines));
        // lines = next_lines;
      }
      results.push_back(simulated_annealing(lines, timer, time_limit, inputs));
    }
  }

  {
    vector<string> lines;
    int best_result_i = 0;
    rep(i, sz(results))if (results[best_result_i].score < results[i].score) best_result_i = i;
    rep(i, 20) lines.push_back(results[best_result_i].lines[i].s);

    vector<FinalResult> final_results;
    vector<double> time_limits = {1.8, 2.1, 2.4, 2.7};
    for(double time_limit: time_limits){
      final_results.push_back(vertical_optimization(lines, inputs, timer, time_limit));
    }

    int best_result = 0;
    rep(i, sz(final_results))if(final_results[best_result].score < final_results[i].score) best_result = i;
    FinalResult &final_result = final_results[best_result];
    DEBUG((double) final_result.score / sz(inputs));
    rep(i, 20) cout << final_result.lines[i] << endl;
  }
}


int main()
{
  run();
}