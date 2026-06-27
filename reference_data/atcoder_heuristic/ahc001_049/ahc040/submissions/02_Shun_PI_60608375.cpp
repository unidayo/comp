#include <string>
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#include <bits/stdc++.h>
using namespace std;
using lint = long long int;
using P = pair<int, int>;
using PL = pair<lint, lint>;
#define FOR(i, begin, end) for(int i=(begin),i##_end_=(end);i<i##_end_;i++)
#define IFOR(i, begin, end) for(int i=(end)-1,i##_begin_=(begin);i>=i##_begin_;i--)
#define REP(i, n) FOR(i,0,n)
#define IREP(i, n) IFOR(i,0,n)
#define ALL(a)  (a).begin(),(a).end()
constexpr int MOD = 1000000007;
vector<lint> RH_B = {1532834020, 1388622299};
vector<lint> RH_M = {2147482409, 2147478017};
constexpr int INF = 2147483647;
void yes(bool expr) {cout << (expr ? "Yes" : "No") << "\n";}
template<class T>void chmax(T &a, const T &b) { if (a<b) a=b; }
template<class T>void chmin(T &a, const T &b) { if (b<a) a=b; }

//確率計算で使用するsigmaの和を状態に保持しておく（今は最後に置く長方形のsigmaのみを使用している）
//管理がかなり難しそうなので平均値で諦めた
//TODO: ソルバー選択をN,sigma_mean,辺の最大÷最小 の3特徴量から線形回帰したい（optuna回した後、最終日にやる？）
//両方のソルバーを回し、true_scoreが低い方のIDを（0, 1）で返す→ロジスティック回帰で予測

int N, T, sigma;
vector<int> H, W;
vector<int> sigma_H, sigma_W;
vector<int> true_H, true_W;

constexpr bool DEBUG = false;
constexpr bool TEST_OUTPUT = false;
constexpr bool OPTUNA = false; //optunaはDEBUG=trueで実行する

constexpr int BEAM_WIDTH = 1000; //MLE対策のため多くて1000~2000程度
constexpr int BEAM_WIDTH_2 = 10000;
constexpr double TARGET_SCORE_MAX_LIM = 0.5;
constexpr bool DO_CUT_DISTRIBUTION = true;

//以下optunaで最適化したい
//SOLVE_TURN_RATIOはN, T, sigmaに対する線形回帰で求めたい（最終日にやる）
//どちらのソルバーを使用するかも線形回帰でできるかも？
//NNもやってみる

//{'SOLVE_TURN_RATIO': 0.21694810022771444, 'COMPLEXITY_THRESHOLD': 240377.74384908524, 'PROB_THRESHOLD': 0.952837093119258, 'RECT_SIZE_FACTOR': 0.0037665916293429364, 'RANDOM_FACTOR': 0.10827900861738542, 'RANDOM_FACTOR_2': 0.30209702775976177, 'RANDOM_FACTOR_FINAL': 0.018249938882897884, 'RANDOM_FACTOR_FINAL_2': 0.029863466651743057, 'TARGET_SCORE_UP': 1.0086577751213723, 'TARGET_SCORE_DOWN': 0.9167202516240829, 'LANE_ADD_PENALTY': 3190492514.1912827, 'RECT_CNT_CHECK_MAX': 8, 'SA_START_TEMP': 3.161961626505845, 'SA_END_TEMP': 0.5354202962174639, 'BEAM_TIME_LIMIT': 936.179847696001, 'SA_MODE0_PROB': 29, 'SA_MODE1_PROB': 16, 'MARGIN_THRESHOLD': 1.6809565683617582, 'PROB_THRESHOLD_DECREASE_RATE': 0.9998259195708081, 'PROB_THRESHOLD_2': 0.8145032295812871}
double SOLVE_TURN_RATIO = 0.21694810022771444;
double PROB_THRESHOLD = 0.952837093119258;
double RECT_SIZE_FACTOR = 0.0037665916293429364;
double RANDOM_FACTOR = 0.10827900861738542;
double RANDOM_FACTOR_2 = 0.30209702775976177;
double RANDOM_FACTOR_FINAL = 0.018249938882897884;
double RANDOM_FACTOR_FINAL_2 = 0.029863466651743057;
double TARGET_SCORE_UP = 1.0086577751213723;
double TARGET_SCORE_DOWN = 0.9167202516240829;
double LANE_ADD_PENALTY = 3190492514.1912827;
int RECT_CNT_CHECK_MAX = 8;
double SA_START_TEMP = 3.161961626505845;
double SA_END_TEMP = 0.5354202962174639;
double BEAM_TIME_LIMIT = 936.179847696001;
int SA_MODE0_PROB = 29;
int SA_MODE1_PROB = 16;
double MARGIN_THRESHOLD = 1.6809565683617582;
double PROB_THRESHOLD_DECREASE_RATE = 0.9998259195708081;
double PROB_THRESHOLD_2 = 0.8145032295812871;

//ソルバー選択用のロジスティック回帰パラメータ
vector<double> X_MEAN = {6.52156667e+01, 2.37286249e+03, 1.59997269e+00, 4.67674989e+03, 1.51505432e+05, 1.04244944e+02, 8.68833555e+06, 3.80214269e+03, 2.63059145e+00};
vector<double> X_STD = {2.05831657e+01, 1.74867356e+03, 2.65854794e-01, 2.70278763e+03, 1.25054270e+05, 3.72604865e+01, 1.17785826e+07, 2.90933046e+03, 9.11828997e-01};
vector<double> COEF = {0.86312575, -1.51043796, -3.5528194, -0.86064456, 1.22179495, 0.37381407, 0.11901816, 1.94295153, 2.40980389};
double INTERCEPT = -1.17967546;

//search_num推定用の線形回帰パラメータ
vector<double> X_MEAN_SEARCH = {6.46228514e+01, 1.10158746e+02, 5.28344793e+03, 1.64727015e+00};
vector<double> X_STD_SEARCH = {2.06564739e+01, 7.45384455e+01, 2.59041384e+03, 3.16078088e-01};
vector<double> COEF_SEARCH = {-0.84717547, 5.89039492, -1.80607343, -0.26540474};
double INTERCEPT_SEARCH = 19.53993933265925;







random_device rnd;
mt19937 mt(rnd());

int true_score = INF;
vector<int> sqrt_list(100);

struct Rect {
  int id = -1;
  int x, y; //右端・下端　
  //その長方形と衝突する可能性のある長方形の右端や下端を記録
  int next_x, next_y;
  //自分より左・上に積み重なる長方形の個数
  int left_cnt = 0, up_cnt = 0;
  bool operator < (const Rect &other) const {return (x == other.x ? y > other.y : x > other.x); }
};
struct Beam;
vector<Beam> beam_states;
struct Beam {
  //階段状に置いていき、凸包を成す長方形のみを管理する
  double value = 0;
  double prob_mult = 1;
  int id = -1;
  int last_id = -1;
  int height = 0, width = 0;
  lint area = 0;
  set<Rect> rects;

  //そのターンの操作情報
  int rect_id = -1;
  bool rect_rotate = false;
  bool is_put_with_y = false;
  int ref_id = -1;

  bool operator < (const Beam &other) const { return value < other.value; }

  Beam get_copy() {
    Beam s;
    s.prob_mult = prob_mult;
    s.last_id = id;
    s.height = height;
    s.width = width;
    s.area = area;
    s.rects = set<Rect>(rects);
    return s;
  }
  void push(){
    id = beam_states.size();
    beam_states.emplace_back(*this);
  }
  int get_cand_cnt() {
    return rects.size() - 1;
  }

  //候補点および回転方向を選択し遷移する
  bool apply(int rect_id, int cand_id, bool rect_rotate, int target_score) {
    this->rect_id = rect_id;
    this->rect_rotate = rect_rotate;
    Rect rect_d = *next(rects.begin(), cand_id); //下端を合わせる長方形
    Rect rect_r = *next(rects.begin(), cand_id+1); //右端を合わせる長方形
    int left_x = rect_r.x;
    int up_y = rect_d.y;
    int h = rect_rotate ? W[rect_id] : H[rect_id];
    int w = rect_rotate ? H[rect_id] : W[rect_id];
    int right_x = left_x + w;
    int down_y = up_y + h;
    area += (lint)h * w;

    chmax(height, down_y);
    chmax(width, right_x);
    if(max(height, width) > target_score * TARGET_SCORE_MAX_LIM) return false;
    if(height + width > target_score) return false;

    //操作方向の決定
    //最下辺がrect_dである確率・最右辺がrect_rである確率を計算
    //確率の高い方を選択する
    //計算の簡単のため、次点となる長方形の右端・下端を参照し、上下関係が逆転する確率を使用する
    //ただし、x=0またはy=0に接する場合はペナルティ0とする
    int down_y_1 = 0, down_y_2 = 0; //rect_r到達までの間における最大の下端（と次点）
    for(Rect r : rects) {
      if(r.id == -1) continue;
      if(r.id == rect_r.id) break;
      if(r.y > down_y_1) {
        down_y_2 = down_y_1;
        down_y_1 = r.y;
      } else if(r.y > down_y_2) {
        down_y_2 = r.y;
      }
      if(r.next_y > down_y_1) {
        down_y_2 = down_y_1;
        down_y_1 = r.next_y;
      } else if(r.next_y > down_y_2) {
        down_y_2 = r.next_y;
      }
    }

    assert(down_y_1 == rect_d.y);
    assert(down_y_1 >= down_y_2);
    int right_x_1 = 0, right_x_2 = 0; //（setを逆方向に見て）rect_d到達までの間における最大の右端（と次点）
    for(auto it = rects.rbegin(); it != rects.rend(); it++) {
      Rect r = *it;
      if(r.id == -1) continue;
      if(r.id == rect_d.id) break;
      if(r.x > right_x_1) {
        right_x_2 = right_x_1;
        right_x_1 = r.x;
      } else if(r.x > right_x_2) {
        right_x_2 = r.x;
      }
      if(r.next_x > right_x_1) {
        right_x_2 = right_x_1;
        right_x_1 = r.next_x;
      } else if(r.next_x > right_x_2) {
        right_x_2 = r.next_x;
      }
    }
    assert(right_x_1 == rect_r.x);
    assert(right_x_1 >= right_x_2);
    //√積み重なる長方形の個数 だけ分散が増えると考える
    double prob_accept_down = 1;
    if(rect_d.up_cnt > 0) {
      prob_accept_down = 0.5 * (1 + erf((down_y_1 - down_y_2) / (sigma * sqrt_list[2 * min(RECT_CNT_CHECK_MAX, rect_d.up_cnt)])));
      //prob_accept_down = 0.5 * (1 + erf((down_y_1 - down_y_2) / (sigma * sqrt(2))));
    }
    double prob_accept_right = 1;
    if(rect_r.left_cnt > 0) {
      prob_accept_right = 0.5 * (1 + erf((right_x_1 - right_x_2) / (sigma * sqrt_list[2 * min(RECT_CNT_CHECK_MAX, rect_r.left_cnt)])));
      //prob_accept_right = 0.5 * (1 + erf((right_x_1 - right_x_2) / (sigma * sqrt(2))));
    }

    if(prob_accept_down > prob_accept_right) {
      if(prob_accept_down * prob_mult < PROB_THRESHOLD) return false;
      prob_mult *= prob_accept_down;
      //下端の方が確度が高いので右から挿入
      is_put_with_y = false;
      ref_id = rect_d.id;
    } else {
      //左辺の方が短いので、下から挿入
      if(prob_accept_right * prob_mult < PROB_THRESHOLD) return false;
      prob_mult *= prob_accept_right;
      is_put_with_y = true;
      ref_id = rect_r.id;
    }

    //長方形を凸包に追加
    //次点長方形の右端・下端の計算をする
    //凸包から削除する長方形を決定
    //削除する長方形の右端・下端のうち最大のものを次点とする
    int next_down_y = 0, next_right_x = 0;
    vector<Rect> erase_rects;
    for(Rect r : rects) {
      if(r.x <= right_x && r.y <= down_y) {
        erase_rects.push_back(r);
        chmax(next_down_y, r.y);
        chmax(next_right_x, r.x);
      }
    }
    for(Rect r : erase_rects) rects.erase(r);
    Rect rect = {rect_id, right_x, down_y, next_right_x, next_down_y, rect_r.left_cnt + 1, rect_d.up_cnt + 1};
    rects.insert(rect);

    //充填率の計算
    //凸包の面積を求める
    lint space = 0;
    int last_x = INF;
    int last_y = 0;
    for(Rect r : rects) {
      space += (lint)(last_x - r.x) * last_y;
      last_x = r.x;
      last_y = r.y;
    }
    double fill_rate = (double)area / (double)space;

    value = (1.0 - fill_rate) + RECT_SIZE_FACTOR * rects.size();
    value *= (mt() % (int)(RANDOM_FACTOR * 100000) + ((1.0 - RANDOM_FACTOR) * 100000)) / 100000.0;

    return true;
  }
};

struct State {
  double score, value;
  //各長方形について、回転方向・配置向き・配置参照IDを持つ
  vector<int> idx;
  vector<bool> is_rotated;
  vector<bool> put_with_y; //false: x軸方向、true: y軸方向
  vector<int> ref_id;

  //実際の配置情報
  //現在の長方形の大きさを仮定して配置を実行する
  //長方形の大きさを変えたら1からやり直す必要があることに注意
  //各長方形の下端のx座標
  vector<double> x;
  //各長方形の右端のy座標
  vector<double> y;

  bool operator < (const State &other) const { return value < other.value; }

  State get_copy() {
    State res;
    res.idx = vector<int>(idx);
    res.is_rotated = vector<bool>(is_rotated);
    res.put_with_y = vector<bool>(put_with_y);
    res.ref_id = vector<int>(ref_id);
    res.x = vector<double>(x);
    res.y = vector<double>(y);
    return res;
  }

  void put_rectangle(int id, vector<int> &H, vector<int> &W) {

    //id番目の長方形を配置する
    int idx_ptr = idx.size();
    int h = is_rotated[idx_ptr] ? W[id] : H[id];
    int w = is_rotated[idx_ptr] ? H[id] : W[id];
    if(!put_with_y[idx_ptr]) {
      //x軸方向に配置
      int yup, ydown;
      if(ref_id[idx_ptr] == -1) {
        yup = 0;
      } else {
        int ref_id_ptr = 0;
        REP(i, idx.size()) if(idx[i] == ref_id[idx_ptr]) ref_id_ptr = i;
        yup = y[ref_id_ptr];
      }
      ydown = yup + h;
      int xmax = 0;
      REP(k, idx.size()) {
        int i = idx[k];
        int hh = is_rotated[k] ? W[i] : H[i];
        if(x[k] > xmax && !(ydown <= y[k] - hh || yup >= y[k])) xmax = x[k];
      }
      x.push_back(xmax + w);
      y.push_back(ydown);
    } else {
      //y軸方向に配置
      int xleft, xright;
      if(ref_id[idx_ptr] == -1) {
        xleft = 0;
      } else {
        int ref_id_ptr = 0;
        REP(i, idx.size()) if(idx[i] == ref_id[idx_ptr]) ref_id_ptr = i;
        xleft = x[ref_id_ptr];
      }
      xright = xleft + w;
      int ymax = 0;
      REP(k, idx.size()) {
        int i = idx[k];
        int ww = is_rotated[k] ? H[i] : W[i];
        if(y[k] > ymax && !(xright <= x[k] - ww || xleft >= x[k])) ymax = y[k];
      }
      y.push_back(ymax + h);
      x.push_back(xright);
    }
    idx.push_back(id);
  }

  double calc_xmax() {
    double xmax = 0;
    for(double xx : x) {
      chmax(xmax, xx);
    }
    return xmax;
  }
  double calc_ymax() {
    double ymax = 0;
    for(double yy : y) {
      chmax(ymax, yy);
    }
    return ymax;
  }

  void calc_score() {
    //最大右端＋最大下端を返す（一番簡単な評価値）
    score = calc_xmax() + calc_ymax();
  }
};

State beam_to_state(Beam b) {
  //ビームサーチ状態からidをさかのぼって操作を復元し、Stateに変換する
  //idxは含まない（put_rectangleを実施する想定）
  State s;
  s.is_rotated = vector<bool>(N);
  s.put_with_y = vector<bool>(N);
  s.ref_id = vector<int>(N);

  while(true) {
    s.is_rotated[b.rect_id] = b.rect_rotate;
    s.put_with_y[b.rect_id] = b.is_put_with_y;
    s.ref_id[b.rect_id] = b.ref_id;
    if(b.last_id == -1) break;
    b = beam_states[b.last_id];
  }
  return s;
}


//正規分布の平均と分散に対し、特定の値が出現する確率を計算
double calc_prob(double x, double mean, double sigma) {
  return exp(-0.5 * (double)(x - mean) * (x - mean) / (double)(sigma * sigma)) / (sigma * sqrt(2 * 3.14159265358979323846));
}
double normal_prob(double x) {
  return calc_prob(x, 0, 1);
}

P do_query(State s) {
  //長方形の配置を出力
  cout << s.idx.size() << "\n";
  REP(i, s.idx.size()) {
    cout << s.idx[i] << " " << s.is_rotated[i] << " ";
    cout << (s.put_with_y[i] ? "U" : "L") << " ";
    cout << s.ref_id[i] << "\n";
  }
  cout.flush();

  int Wret, Hret;
  if(!DEBUG) {
    cin >> Wret >> Hret;
  } else {
    //真の値を計算
    State true_state;
    true_state.is_rotated = s.is_rotated;
    true_state.put_with_y = s.put_with_y;
    true_state.ref_id = s.ref_id;
    true_state.x = true_state.y = vector<double>();
    REP(i, s.idx.size()) true_state.put_rectangle(s.idx[i], true_H, true_W);
    int dW, dH;
    cin >> dW >> dH;
    int true_x = true_state.calc_xmax();
    int true_y = true_state.calc_ymax();
    Wret = true_x + dW;
    Hret = true_y + dH;
    chmin(true_score, true_x + true_y);
  }

  return P(Wret, Hret);
}

void solve_beam_rd(int TURN_LIMIT, int TIME_LIMIT)
{

  auto start_time = std::chrono::system_clock::now();

  double target_score = 1500000;
  beam_states.clear();
  vector<set<Beam>> beam(N);
  //最初はレーン0に回転せずに長方形を置くに固定
  //転置しているだけの重複を避けるため
  Beam init;
  Rect r1({-1, INF, 0, INF, 0});
  Rect r2({-1, 0, INF, 0, INF});
  init.rects.insert(r1);
  init.rects.insert(r2);
  init.apply(0, 0, false, target_score);
  init.push();
  beam[0].insert(init);
  //chokudaiサーチ
  //MLEが激しいので、動的幅ビームサーチに変更
  auto turn_start_time = std::chrono::system_clock::now();
  double time_from_start_to_turn = chrono::duration_cast<chrono::milliseconds>(turn_start_time - start_time).count();
  int loop = 0;
  vector<Beam> final_states;
  double target_score_up = TARGET_SCORE_UP;
  double target_score_down = TARGET_SCORE_DOWN;
  while(true) {
    if(loop%10 == 0) {
      //cerr << "loop: " << loop << " " << (final_states.empty() ? 0 : final_states[0].value) << " " << target_score << "\n";
      double time_overall = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
      double time_turn_end = TIME_LIMIT;
      if(time_overall > time_turn_end) break;
      target_score_up = TARGET_SCORE_UP - (TARGET_SCORE_UP - 1.0) * time_overall / time_turn_end;
      target_score_down = TARGET_SCORE_DOWN + (1.0 - TARGET_SCORE_DOWN) * time_overall / time_turn_end;
    }
    loop++;
    bool ok = false;
    FOR(i, 1, N) if(!beam[i-1].empty()) {
      ok = true;
      Beam b = *beam[i-1].begin();
      beam[i-1].erase(beam[i-1].begin());
      int cand_cnt = b.get_cand_cnt();
      REP(c, cand_cnt) REP(is_rotate, 2) {
        Beam nb = b.get_copy();
        if(!nb.apply(i, c, is_rotate, target_score)) continue;
        if(beam[i].size() >= BEAM_WIDTH && nb.value > beam[i].rbegin()->value) {
          //cerr << beam_states.size() << " " << nb.value << " " << beam[i].rbegin()->value << "\n";
          continue;
        }
        nb.push();
        beam[i].insert(nb);
        if(beam[i].size() > BEAM_WIDTH) beam[i].erase(prev(beam[i].end()));
      }
    }
    if(!ok) break;
    if(!beam[N-1].empty()) {
      for(Beam b : beam[N-1]) {
        //より正確にスコアを計算
        State s = beam_to_state(b);
        REP(i, N) s.put_rectangle(i, H, W);
        s.calc_score();
        b.value = s.score;
        //乱数を掛ける？
        b.value *= (mt() % (int)(RANDOM_FACTOR_FINAL * 100000 + 1) + ((1.0 - RANDOM_FACTOR_FINAL) * 100000)) / 100000.0;
        final_states.push_back(b);
      }
      beam[N-1].clear();
      target_score *= target_score_down;
    } else target_score *= target_score_up;
  }
  sort(ALL(final_states));

  int final_score = INF;
  REP(turn, TURN_LIMIT) {
    cerr << "turn: " << turn << " " << final_score << "\n";

    int state_idx = min(turn, (int)final_states.size()-1);
    if(TEST_OUTPUT) state_idx = 0;
    Beam b = final_states[state_idx];
    State s = beam_to_state(b);
    REP(i, N) s.put_rectangle(i, H, W);

    if(TEST_OUTPUT) {
      //テスト出力
      REP(len, s.idx.size()) {
        cout << len+1 << "\n";
        REP(i, len+1) {
          cout << s.idx[i] << " " << s.is_rotated[i] << " ";
          cout << (s.put_with_y[i] ? "U" : "L") << " ";
          cout << s.ref_id[i] << "\n";
        }
        cout.flush();
      }
      return;
    }

    P ret = do_query(s);
    int Wret = ret.first;
    int Hret = ret.second;
    chmin(final_score, Wret + Hret);
  }
}







struct Beam_2;
vector<Beam_2> beam_states_2;
struct Beam_2 {
  //ビームサーチの状態として、各レーンの高さ・幅・専有面積を管理
  //ターゲットスコアを超えるような遷移は禁止（または大ペナルティ）としつつ、充填率を最大化する
  //レーンを追加すると充填率を減らさず遷移できてしまうので、レーン追加に対して小ペナルティがあるとよさそう（充填率9割くらいがボーダー？）
  //充填率の計算の際、maxを担当している長方形を対象から除外するとよさそう！
  //ターゲットスコアは徐々に下げていくのが良さそう（chokudaiサーチ？）
  //そのターンでの操作は各状態に記録し、後で復元できるようにする
  double value = 0;
  double prob_mult = 1;
  int id = -1;
  int last_id = -1;
  int height_max = 0;
  int width_sum = 0;
  vector<int> height;
  vector<int> width;
  vector<int> height_of_max_width;
  vector<int> cnt;
  vector<lint> area;

  //そのターンの操作情報
  int rect_id = -1;
  int rect_lane = -1;
  bool rect_rotate = false;

  bool operator < (const Beam_2 &other) const { return value < other.value; }

  Beam_2 get_copy() {
    Beam_2 s;
    s.last_id = id;
    s.prob_mult = prob_mult;
    s.height_max = height_max;
    s.width_sum = width_sum;
    s.height = vector<int>(height);
    s.width = vector<int>(width);
    s.height_of_max_width = vector<int>(height_of_max_width);
    s.cnt = vector<int>(cnt);
    s.area = vector<lint>(area);
    return s;
  }
  void push(){
    id = beam_states_2.size();
    beam_states_2.emplace_back(*this);
  }
  int get_lane_num() {
    return height.size();
  }

  //単調減少チェックを行い、適用可能かどうかを返す
  //衝突確率の積を保持し、閾値を下回るかどうかで判断したい
  bool can_apply(int rect_id, int rect_lane, bool rect_rotate) {
    //一つ右のレーンをチェックし、高さを比較する
    int lane_num = height.size();
    FOR(i, rect_lane+1, lane_num) {
      if(cnt[rect_lane] + cnt[i] == 0) continue;
      double prob_accept = 0.5 * (1 + erf((height[rect_lane] - height[i]) / (sigma * sqrt_list[cnt[rect_lane] + cnt[i]])));
      if(prob_mult * prob_accept < PROB_THRESHOLD_2) return false;
      prob_mult *= prob_accept;
    }
    /*
    int h = rect_rotate ? W[rect_id] : H[rect_id];
    if(rect_lane > 0) {
      if(rect_lane == lane_num) {
        if(height[rect_lane-1] - h < sigma_H[rect_id] * threshold_list[cnt[rect_lane-1]]) return false;
      }
    }
    */
    return true;
  }

  //操作の実施
  bool apply(int rect_id, int rect_lane, bool rect_rotate, int target_score) {
    this->rect_id = rect_id;
    this->rect_lane = rect_lane;
    this->rect_rotate = rect_rotate;
    if(height.size() == rect_lane) {
      height.push_back(0);
      width.push_back(0);
      height_of_max_width.push_back(0);
      cnt.push_back(0);
      area.push_back(0);
    }
    width_sum -= width[rect_lane];
    int h = rect_rotate ? W[rect_id] : H[rect_id];
    int w = rect_rotate ? H[rect_id] : W[rect_id];
    height[rect_lane] += h;
    if(w > width[rect_lane]) {
      height_of_max_width[rect_lane] = h;
      width[rect_lane] = w;
    }
    area[rect_lane] += (lint)H[rect_id] * W[rect_id];
    cnt[rect_lane]++;
    height_max = max(height_max, height[rect_lane]);
    width_sum += width[rect_lane];

    if(max(height_max, width_sum) > target_score * TARGET_SCORE_MAX_LIM) return false;
    if(height_max + width_sum > target_score) return false;

    //充填率の計算
    lint area_sum = 0;
    lint space_sum = 0;
    REP(i, height.size()) {
      area_sum += area[i] - (lint)height_of_max_width[i] * width[i];
      space_sum += (lint)(height[i] - height_of_max_width[i]) * width[i];
    }
    value = space_sum - area_sum + (lint)height.size() * LANE_ADD_PENALTY;
    value *= (mt() % (int)(RANDOM_FACTOR_2 * 100000) + ((1.0 - RANDOM_FACTOR_2) * 100000)) / 100000.0;
    if(rect_id == N-1) value = height_max + width_sum;
    return true;
  }
};

struct State_2 {
  double score, value;
  //各長方形について、回転方向・配置向き・配置参照IDを持つ
  vector<int> idx;
  vector<bool> is_rotated;
  vector<bool> put_with_y; //false: x軸方向、true: y軸方向
  vector<int> ref_id;
  vector<int> lane;

  //実際の配置情報
  //現在の長方形の大きさを仮定して配置を実行する
  //長方形の大きさを変えたら1からやり直す必要があることに注意
  //各長方形の下端のx座標
  vector<int> x;
  //各長方形の右端のy座標
  vector<int> y;

  bool operator < (const State_2 &other) const { return value < other.value; }

  State_2 get_copy() {
    State_2 res;
    res.idx = vector<int>(idx);
    res.is_rotated = vector<bool>(is_rotated);
    res.put_with_y = vector<bool>(put_with_y);
    res.ref_id = vector<int>(ref_id);
    res.x = vector<int>(x);
    res.y = vector<int>(y);
    return res;
  }

  void put_rectangle(int id, vector<int> &H, vector<int> &W) {

    //id番目の長方形を配置する
    int idx_ptr = idx.size();
    int h = is_rotated[idx_ptr] ? W[id] : H[id];
    int w = is_rotated[idx_ptr] ? H[id] : W[id];
    if(!put_with_y[idx_ptr]) {
      //x軸方向に配置
      int yup, ydown;
      if(ref_id[idx_ptr] == -1) {
        yup = 0;
      } else {
        int ref_id_ptr = 0;
        REP(i, idx.size()) if(idx[i] == ref_id[idx_ptr]) ref_id_ptr = i;
        yup = y[ref_id_ptr];
      }
      ydown = yup + h;
      int xmax = 0;
      REP(k, idx.size()) {
        int i = idx[k];
        int hh = is_rotated[k] ? W[i] : H[i];
        if(x[k] > xmax && !(ydown <= y[k] - hh || yup >= y[k])) xmax = x[k];
      }
      x.push_back(xmax + w);
      y.push_back(ydown);
    } else {
      //y軸方向に配置
      int xleft, xright;
      if(ref_id[idx_ptr] == -1) {
        xleft = 0;
      } else {
        int ref_id_ptr = 0;
        REP(i, idx.size()) if(idx[i] == ref_id[idx_ptr]) ref_id_ptr = i;
        xleft = x[ref_id_ptr];
      }
      xright = xleft + w;
      int ymax = 0;
      REP(k, idx.size()) {
        int i = idx[k];
        int ww = is_rotated[k] ? H[i] : W[i];
        if(y[k] > ymax && !(xright <= x[k] - ww || xleft >= x[k])) ymax = y[k];
      }
      y.push_back(ymax + h);
      x.push_back(xright);
    }
    idx.push_back(id);
  }

  int calc_xmax() {
    int xmax = 0;
    for(int xx : x) {
      chmax(xmax, xx);
    }
    return xmax;
  }
  int calc_ymax() {
    int ymax = 0;
    for(int yy : y) {
      chmax(ymax, yy);
    }
    return ymax;
  }

  void calc_score() {
    //最大右端＋最大下端を返す（一番簡単な評価値）
    score = calc_xmax() + calc_ymax();
  }
};

State_2 beam_to_state(Beam_2 b) {
  //ビームサーチ状態からidをさかのぼって操作を復元し、Stateに変換する
  vector<int> lane(N);
  vector<bool> is_rotated(N);
  while(true) {
    lane[b.rect_id] = b.rect_lane;
    is_rotated[b.rect_id] = b.rect_rotate;
    if(b.last_id == -1) break;
    b = beam_states_2[b.last_id];
  }
  State_2 s;
  s.idx = vector<int>();
  REP(i, N) s.idx.push_back(i);
  s.is_rotated = is_rotated;
  s.lane = lane;
  s.put_with_y = vector<bool>(N, false);
  s.ref_id = vector<int>(N);
  int lane_num = *max_element(ALL(lane)) + 1;
  vector<int> lane_last(lane_num, -1);
  REP(i, N) {
    s.ref_id[i] = lane_last[lane[i]];
    lane_last[lane[i]] = i;
  }
  return s;
}

int calc_lane_score(int lane_num, vector<int> &lane, vector<bool> &is_rotated, vector<int> &H, vector<int> &W) {
  vector<int> lane_height(lane_num, 0);
  vector<int> lane_max_width(lane_num, 0);
  vector<int> lane_cnt(lane_num, 0);
  double prob_mult = 1;
  REP(i, N) {
    //単調減少チェック
    //本来は自分より右のレーン全部チェックする必要があるが、隣だけ見たほうがスコアは良かった
    if(lane[i] + 1 < lane_num) {
      if(lane_cnt[lane[i]] + lane_cnt[lane[i]+1] > 0) {
        double prob_accept = 0.5 * (1 + erf((lane_height[lane[i]] - lane_height[lane[i]+1]) / (sigma * sqrt_list[lane_cnt[lane[i]] + lane_cnt[lane[i]+1]])));
        if(prob_mult * prob_accept < PROB_THRESHOLD_2) return INF;
        prob_mult *= prob_accept;
      }
    }
    lane_height[lane[i]] += is_rotated[i] ? W[i] : H[i];
    chmax(lane_max_width[lane[i]], is_rotated[i] ? (int)H[i] : (int)W[i]);
    lane_cnt[lane[i]]++;
  }
  int max_height = 0;
  REP(i, lane_num) chmax(max_height, lane_height[i]);
  int sum_width = 0;
  REP(i, lane_num) sum_width += lane_max_width[i];
  return max_height + sum_width;
}

P do_query_2(State_2 s) {
  //長方形の配置を出力
  cout << s.idx.size() << "\n";
  REP(i, s.idx.size()) {
    cout << s.idx[i] << " " << s.is_rotated[i] << " ";
    cout << (s.put_with_y[i] ? "U" : "L") << " ";
    cout << s.ref_id[i] << "\n";
  }
  cout.flush();

  int Wret, Hret;
  if(!DEBUG) {
    cin >> Wret >> Hret;
  } else {
    //真の値を計算
    State_2 true_state;
    true_state.is_rotated = s.is_rotated;
    true_state.put_with_y = s.put_with_y;
    true_state.ref_id = s.ref_id;
    true_state.x = true_state.y = vector<int>();
    REP(i, s.idx.size()) true_state.put_rectangle(s.idx[i], true_H, true_W);
    int dW, dH;
    cin >> dW >> dH;
    int true_x = true_state.calc_xmax();
    int true_y = true_state.calc_ymax();
    Wret = true_x + dW;
    Hret = true_y + dH;
    chmin(true_score, true_x + true_y);
  }

  return P(Wret, Hret);
}

void solve_beam_sa(int TURN_LIMIT, int TIME_LIMIT_BEAM, int TIME_LIMIT_SA)
{

  auto start_time = std::chrono::system_clock::now();

  //いくつかのレーンに分けて長方形を置いていく
  //長方形は全て左に向かっておく
  //左のレーンほど高さが高くなるような単調減少配置に制限する

  //ビームサーチにより初期解を生成
  double target_score = 1500000;
  beam_states_2.clear();
  vector<set<Beam_2>> beam(N);
  //最初はレーン0に回転せずに長方形を置くに固定
  //転置しているだけの重複を避けるため
  Beam_2 init;
  init.apply(0, 0, false, target_score);
  init.push();
  beam[0].insert(init);
  //chokudaiサーチ
  auto turn_start_time = std::chrono::system_clock::now();
  double time_from_start_to_turn = chrono::duration_cast<chrono::milliseconds>(turn_start_time - start_time).count();
  int loop = 0;
  vector<Beam_2> final_states;
  double target_score_up = TARGET_SCORE_UP;
  double target_score_down = TARGET_SCORE_DOWN;
  while(true) {
    if(loop%10 == 0) {
      //cerr << "loop: " << loop << " " << (final_states.empty() ? 0 : final_states[0].value) << " " << target_score << "\n";
      double time_overall = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
      double time_turn_end = TIME_LIMIT_BEAM;
      if(time_overall > time_turn_end) break;
      target_score_up = TARGET_SCORE_UP - (TARGET_SCORE_UP - 1.0) * time_overall / time_turn_end;
      target_score_down = TARGET_SCORE_DOWN + (1.0 - TARGET_SCORE_DOWN) * time_overall / time_turn_end;
    }
    loop++;
    FOR(i, 1, N) if(!beam[i-1].empty()) {
      Beam_2 b = *beam[i-1].begin();
      beam[i-1].erase(beam[i-1].begin());
      int lane_num = b.get_lane_num();
      REP(l, lane_num+1) REP(is_rotate, 2) {
        Beam_2 nb = b.get_copy();
        if(!nb.can_apply(i, l, is_rotate)) continue;
        if(!nb.apply(i, l, is_rotate, target_score)) continue;
        if(beam[i].size() >= BEAM_WIDTH_2 && nb.value > beam[i].rbegin()->value) continue;
        nb.push();
        beam[i].insert(nb);
        if(beam[i].size() > BEAM_WIDTH_2) beam[i].erase(prev(beam[i].end()));
      }
    }
    if(!beam[N-1].empty()) {
      for(Beam_2 b : beam[N-1]) {
        //より正確にスコアを計算
        State_2 s = beam_to_state(b);
        int lane_score = calc_lane_score(*max_element(ALL(s.lane)) + 1, s.lane, s.is_rotated, H, W);
        b.value = lane_score;
        b.value *= (mt() % (int)(RANDOM_FACTOR_FINAL_2 * 100000 + 1) + ((1.0 - RANDOM_FACTOR_FINAL_2) * 100000)) / 100000.0;
        final_states.push_back(b);
      }
      sort(ALL(final_states));
      beam[N-1].clear();
      target_score *= target_score_down;
    } else target_score *= target_score_up;
  }

  int final_score = INF;
  REP(turn, TURN_LIMIT) {

    cerr << "turn: " << turn << " " << final_score << "\n";
    vector<int> Huse(N), Wuse(N);
    REP(i, N) {
      //Huse[i] = normal_distribution<double>(H[i], 0)(mt);
      //Wuse[i] = normal_distribution<double>(W[i], 0)(mt);
      Huse[i] = H[i];
      Wuse[i] = W[i];
    }

    Beam_2 b = final_states[min(TURN_LIMIT-1, (int)final_states.size()-1) - min(turn, (int)final_states.size()-1)];
    State_2 init = beam_to_state(b);

    vector<int> lane = init.lane;
    vector<bool> is_rotated = init.is_rotated;
    int lane_num = *max_element(ALL(lane)) + 1;

    //1つの長方形の担当レーンを変える焼きなまし
    //レーンの高さが単調減少になるような制約を付ける
    int best_score = calc_lane_score(lane_num, lane, is_rotated, Huse, Wuse);
    double start_temp = SA_START_TEMP;
    double end_temp = SA_END_TEMP;
    double temp = start_temp;
    auto turn_start_time = std::chrono::system_clock::now();
    double time_from_start_to_turn = chrono::duration_cast<chrono::milliseconds>(turn_start_time - start_time).count();
    int loop = 0;
    int last_time = 0;
    int loop_interval = 1000;
    int next_check = 0;
    while(true) {

      if(loop >= next_check) {
        //cerr << "loop: " << loop << " " << best_score << "\n";
        double time_from_turn_start = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - turn_start_time).count();
        double time_turn = TIME_LIMIT_SA / TURN_LIMIT;
        temp = start_temp + (end_temp - start_temp) * time_from_turn_start / time_turn;
        if(time_from_turn_start > time_turn) break;
        if(time_from_turn_start - last_time < 1) loop_interval *= 2;
        else if(time_from_turn_start - last_time > 2 && loop_interval > 1) loop_interval /= 2;
        last_time = time_from_turn_start;
        next_check += loop_interval;
      }
      loop++;

      vector<int> lane_last(lane);
      vector<bool> is_rotated_last(is_rotated);
      int mode = mt() % 100;
      if(mode < SA_MODE0_PROB) {
        //その場回転
        int idx = mt() % N;
        is_rotated[idx] = !is_rotated[idx];
      } else if(mode < SA_MODE0_PROB + SA_MODE1_PROB) {
        //1点変更
        int idx = mt() % N;
        int last_lane = lane[idx];
        lane[idx] = mt() % lane_num;
        if(last_lane == lane[idx]) continue;
        is_rotated[idx] = mt() % 2;
      } else {
        //2点変更
        int idx1 = mt() % N;
        int idx2 = mt() % N;
        if(lane[idx1] == lane[idx2]) continue;
        swap(lane[idx1], lane[idx2]);
        is_rotated[idx1] = mt() % 2;
        is_rotated[idx2] = mt() % 2;
      }
      int score = calc_lane_score(lane_num, lane, is_rotated, H, W);
      double diff = best_score - score;
      double prob = exp(diff * pow(0.1, temp));
      if(diff >= 0 || prob*(double)INF > (rand()%INF)) {
        best_score = score;
      } else {
        lane = lane_last;
        is_rotated = is_rotated_last;
      }
    }

    State s;
    s.is_rotated = is_rotated;
    vector<int> lane_last(lane_num, -1);
    REP(i, N) {
      s.idx.push_back(i);
      s.put_with_y.push_back(false);
      s.ref_id.push_back(lane_last[lane[i]]);
      lane_last[lane[i]] = i;
    }
    P ret = do_query(s);
    int Wret = ret.first;
    int Hret = ret.second;
    chmin(final_score, Wret + Hret);
  }
}

//転置行列を求める
vector<vector<double>> trans_mat(vector<vector<double>> a) {
  vector<vector<double>> ret(a[0].size(), vector<double>(a.size()));
  REP(i, a.size()) REP(j, a[0].size()) ret[j][i] = a[i][j];
  return ret;
}

//逆行列を求める
bool inv_mat(vector<vector<double>> a, vector<vector<double>> &inv) {
  assert(a.size() == a[0].size() && inv.size() == inv[0].size());
  int n = a.size();
  REP(i, n) REP(j, n) inv[i][j] = (i == j ? 1.0 : 0.0);
  REP(i, n) {
    double ma = 0.0;
    int pivot;
    FOR(j, i, n) {
      if(a[j][i] > ma) {
        ma = a[j][i];
        pivot = j;
      }
    }
    if(ma == 0.0) return false;
    if(pivot != i) {
      REP(j, n) {
        swap(a[i][j], a[pivot][j]);
        swap(inv[i][j], inv[pivot][j]);
      }
    }
    double tmp = 1.0 / a[i][i];
    REP(j, n) {
      a[i][j] *= tmp;
      inv[i][j] *= tmp;
    }
    REP(j, n) {
      if(i != j) {
        double tmp2 = a[j][i];
        REP(k, n) {
          a[j][k] -= a[i][k] * tmp2;
          inv[j][k] -= inv[i][k] * tmp2;
        }
      }
    }
  }
  return true;
}

vector<vector<double>> mxm(vector<vector<double>> a, vector<vector<double>> b) {
  vector<vector<double>> ret(a.size(), vector<double>(a.size()));
  REP(i, a.size()) REP(j, a.size()) REP(k, a[0].size()) ret[i][j] += a[i][k]*b[k][j];
  return ret;
}
vector<double> mxv(vector<vector<double>> a, vector<double> b) {
  vector<double> ret(a.size());
  REP(i, a.size()) REP(j, b.size()) ret[i] += a[i][j]*b[j];
  return ret;
}

double normal_cumulative_distribution(double x) {
  return 0.5 * (1 + erf(x / sqrt(2)));
}

void main_query_rectangle(int TURN_LIMIT) {
  //長方形をN/2個ずつに分けて、縦と横につなげる
  //繋げた長方形の長さの合計が精度よく求まる
  //ランダムに回転方向と挿入方向を決める
  //実際にシミュレートしつつ、衝突先が確実に決められないような配置であればやり直す
  //衝突先の決め方は、まず衝突候補を広めにカバーしつつ、最大高さと2番目の高さの差を比較して確率を決める
  //衝突先を保持した上でx,y最大の長方形から復元を行い、WとHに対応する辺の集合を決める
  vector<int> bef_H(H), bef_W(W);

  //正規方程式を用いて期待値（最小二乗解）およびその分散を求める
  //方程式数が未知数より大きく、過剰方程式である場合、正規方程式を用いて最小二乗解が求まる（すごい）
  vector<vector<double>> A(2*N+2*TURN_LIMIT, vector<double>(2*N, 0));
  vector<double> B(2*N+2*TURN_LIMIT, 0);
  //Wの値
  REP(i, N) {
    A[i][i] = 1;
    B[i] = W[i];
  }
  //Hの値
  REP(i, N) {
    A[N+i][N+i] = 1;
    B[N+i] = H[i];
  }

  int turn = 0;
  double prob_threshold = 0.999;
  while(turn < TURN_LIMIT) {
    //cerr << "turn: " << turn << " " << prob_threshold << "\n";
    vector<bool> put_with_y(N);
    REP(i, N) put_with_y[i] = mt() % 2;
    //put_with_yの分布が均等になるようにする
    int put_with_y_cnt = 0;
    REP(i, N) put_with_y_cnt += put_with_y[i];
    if((N%2 == 0 && abs(put_with_y_cnt - N/2) > 0) || (N%2 == 1 && abs(put_with_y_cnt - N/2) > 1)) continue;
    vector<bool> rot(N);
    REP(i, N) rot[i] = mt() % 2;

    //シミュレーションを行い、衝突先の決定および衝突確率判定を行う
    vector<int> right_x(N), down_y(N);
    vector<int> parent_x(N, -1), parent_y(N, -1);
    bool ok = true;
    REP(i, N) {
      int h = rot[i] ? W[i] : H[i];
      int w = rot[i] ? H[i] : W[i];
      if(!put_with_y[i]) {
        //右から挿入
        down_y[i] = h;
        int cover_check_y = down_y[i] + sigma_H[i] * MARGIN_THRESHOLD;
        int max_x = 0, max_x_2 = 0, max_id = -1;
        REP(j, i) {
          int target_up_y = down_y[j] - (rot[j] ? W[j] : H[j]);
          if(target_up_y < cover_check_y) {
            if(right_x[j] > max_x) {
              max_x_2 = max_x;
              max_x = right_x[j];
              max_id = j;
            } else {
              chmax(max_x_2, right_x[j]);
            }
          }
        }
        right_x[i] = max_x + w;
        parent_x[i] = max_id;
        //十分左上から離れていれば処理をスキップ
        if(max_x > 200000 || max_x == 0) continue;
        //max_idが衝突する可能性が低いケースを除外
        //max_idのup_yがdown_yより十分上にあることが条件
        int max_id_up_y = down_y[max_id] - (rot[max_id] ? W[max_id] : H[max_id]);
        double prob_accept_2 = 0.5 * (1 + erf((down_y[i] - max_id_up_y) / sqrt(sigma_H[i] + sigma_H[max_id])));
        if(prob_accept_2 < prob_threshold) {
          ok = false;
          break;
        }
        //確率計算
        double prob_accept = 0.5 * (1 + erf((max_x - max_x_2) / sqrt(sigma_H[i] + sigma_H[max_id])));
        if(prob_accept < prob_threshold) {
          ok = false;
          break;
        }
      } else {
        //下から挿入
        right_x[i] = w;
        int cover_check_x = right_x[i] + sigma_W[i] * MARGIN_THRESHOLD;
        int max_y = 0, max_y_2 = 0, max_id = -1;
        REP(j, i) {
          int target_left_x = right_x[j] - (rot[j] ? H[j] : W[j]);
          if(target_left_x < cover_check_x) {
            if(down_y[j] > max_y) {
              max_y_2 = max_y;
              max_y = down_y[j];
              max_id = j;
            } else {
              chmax(max_y_2, down_y[j]);
            }
          }
        }
        down_y[i] = max_y + h;
        parent_y[i] = max_id;
        //十分左上から離れていれば処理をスキップ
        if(max_y > 200000 || max_y == 0) continue;
        //max_idが衝突する可能性が低いケースを除外
        //max_idのleft_xがright_xより十分左にあることが条件
        int max_id_left_x = right_x[max_id] - (rot[max_id] ? H[max_id] : W[max_id]);
        double prob_accept_2 = 0.5 * (1 + erf((right_x[i] - max_id_left_x) / sqrt(sigma_W[i] + sigma_W[max_id])));
        if(prob_accept_2 < prob_threshold) {
          ok = false;
          break;
        }
        //確率計算
        double prob_accept = 0.5 * (1 + erf((max_y - max_y_2) / sqrt(sigma_W[i] + sigma_W[max_id])));
        if(prob_accept < prob_threshold) {
          ok = false;
          break;
        }
      }
      if(!ok) break;
    }
    if(!ok) {
      prob_threshold *= PROB_THRESHOLD_DECREASE_RATE;
      continue;
    }

    //衝突先の復元
    //x方向最大の長方形からスタート
    int x_max_id = 0;
    REP(i, N) if(right_x[i] > right_x[x_max_id]) x_max_id = i;
    //cout << "# x query" << endl;
    while(x_max_id != -1) {
      if(!rot[x_max_id]) {
        A[2*N+2*turn][x_max_id] = 1;
        //cout << "# W " << x_max_id << endl;
      } else {
        A[2*N+2*turn][N+x_max_id] = 1;
        //cout << "# H " << x_max_id << endl;
      }
      x_max_id = parent_x[x_max_id];
    }
    //y方向最大の長方形からスタート
    int y_max_id = 0;
    REP(i, N) if(down_y[i] > down_y[y_max_id]) y_max_id = i;
    //cout << "# y query" << endl;
    while(y_max_id != -1) {
      if(!rot[y_max_id]) {
        A[2*N+2*turn+1][N+y_max_id] = 1;
        //cout << "# H " << y_max_id << endl;
      } else {
        A[2*N+2*turn+1][y_max_id] = 1;
        //cout << "# W " << y_max_id << endl;
      }
      y_max_id = parent_y[y_max_id];
    }

    State s;
    s.idx = vector<int>(N);
    REP(i, N) s.idx[i] = i;
    s.is_rotated = rot;
    s.put_with_y = put_with_y;
    s.ref_id = vector<int>(N, -1);
    P ret = do_query(s);
    //Bに情報を追加
    B[2*N+2*turn] = ret.first;
    B[2*N+2*turn+1] = ret.second;

    turn++;
  }

  //正規方程式を解く
  vector<vector<double>> AT = trans_mat(A);
  vector<vector<double>> ATA = mxm(AT, A);
  assert(ATA.size() == 2*N && ATA[0].size() == 2*N);
  vector<vector<double>> ATA_inv(2*N, vector<double>(2*N));
  if(!inv_mat(ATA, ATA_inv)) {
    cerr << "Inverse matrix does not exist\n";
  }
  vector<double> X = mxv(ATA_inv, mxv(AT, B));

  assert(X.size() == 2*N);
  vector<vector<double>> cov(2*N, vector<double>(2*N));
  REP(i, 2*N) REP(j, 2*N) {
    cov[i][j] = ATA_inv[i][j] * sigma * sigma;
  }

  //推定結果を格納
  REP(i, N) {
    W[i] = round(X[i]);
    H[i] = round(X[N+i]);
    sigma_W[i] = round(sqrt(cov[i][i]));
    sigma_H[i] = round(sqrt(cov[N+i][N+i]));
  }

  //値が10000~100000の範囲に収まることを利用し、切断正規分布の公式を使って期待値と分散を補正する
  //lowerは10000ではなく与えられた値から類推できるかも（無理そう）
  bef_W = vector<int>(W), bef_H = vector<int>(H);
  vector<int> bef_sigma_W(sigma_W), bef_sigma_H(sigma_H);
  if(DO_CUT_DISTRIBUTION) {
    double upper = 100000;
    double lower = 10000;
    REP(i, N) {
      double prob_lower_W = normal_prob((double)(lower - W[i]) / sigma_W[i]);
      double prob_upper_W = normal_prob((double)(upper - W[i]) / sigma_W[i]);
      double erf_lower_W = normal_cumulative_distribution((double)(lower - W[i]) / sigma_W[i]);
      double erf_upper_W = normal_cumulative_distribution((double)(upper - W[i]) / sigma_W[i]);
      double E_W = W[i] + sigma_W[i] * (prob_lower_W - prob_upper_W) / (erf_upper_W - erf_lower_W);
      double var_W = sigma_W[i] * sigma_W[i] * (1 + ((lower - W[i]) / sigma_W[i] * prob_lower_W - (upper - W[i]) / sigma_W[i] * prob_upper_W) / (erf_upper_W - erf_lower_W) - pow((prob_lower_W - prob_upper_W) / (erf_upper_W - erf_lower_W), 2));
      double prob_lower_H = normal_prob((double)(lower - H[i]) / sigma_H[i]);
      double prob_upper_H = normal_prob((double)(upper - H[i]) / sigma_H[i]);
      double erf_lower_H = normal_cumulative_distribution((double)(lower - H[i]) / sigma_H[i]);
      double erf_upper_H = normal_cumulative_distribution((double)(upper - H[i]) / sigma_H[i]);
      double E_H = H[i] + sigma_H[i] * (prob_lower_H - prob_upper_H) / (erf_upper_H - erf_lower_H);
      double var_H = sigma_H[i] * sigma_H[i] * (1 + ((lower - H[i]) / sigma_H[i] * prob_lower_H - (upper - H[i]) / sigma_H[i] * prob_upper_H) / (erf_upper_H - erf_lower_H) - pow((prob_lower_H - prob_upper_H) / (erf_upper_H - erf_lower_H), 2));
      W[i] = round(E_W);
      H[i] = round(E_H);
      sigma_W[i] = round(sqrt(var_W));
      sigma_H[i] = round(sqrt(var_H));
    }

    //たまにsigmaが壊れるケースがあるので（計算誤差）、その場合は元に戻す
    bool ok = true;
    REP(i, N) if(sigma_W[i] <= 0 || sigma_H[i] <= 0 || sigma_W[i] >= 1e5 || sigma_H[i] >= 1e5) ok = false;
    if(!ok) {
      W = bef_W;
      H = bef_H;
      sigma_W = bef_sigma_W;
      sigma_H = bef_sigma_H;
    }
  }

};

int main(int argc, char *argv[])
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  cin >> N >> T >> sigma;
  W.resize(N);
  H.resize(N);
  REP(i, N) cin >> W[i] >> H[i];
  if(DEBUG) {
    true_H.resize(N);
    true_W.resize(N);
    REP(i, N) cin >> true_W[i] >> true_H[i];
  }
  sigma_H.resize(N, sigma);
  sigma_W.resize(N, sigma);
  REP(i, 100) sqrt_list[i] = sqrt(i);

  if(OPTUNA) {
    if(argc == 0) {
      cerr << "Usage: ./a.out <input_file> <output_file>" << endl;
      return 1;
    } else {
      SOLVE_TURN_RATIO = atof(argv[1]);
      PROB_THRESHOLD = atof(argv[2]);
      RECT_SIZE_FACTOR = atof(argv[3]);
      RANDOM_FACTOR = atof(argv[4]);
      RANDOM_FACTOR_2 = atof(argv[5]);
      RANDOM_FACTOR_FINAL = atof(argv[6]);
      RANDOM_FACTOR_FINAL_2 = atof(argv[7]);
      TARGET_SCORE_UP = atof(argv[8]);
      TARGET_SCORE_DOWN = atof(argv[9]);
      LANE_ADD_PENALTY = atof(argv[10]);
      RECT_CNT_CHECK_MAX = atoi(argv[11]);
      SA_START_TEMP = atof(argv[12]);
      SA_END_TEMP = atof(argv[13]);
      BEAM_TIME_LIMIT = atof(argv[14]);
      SA_MODE0_PROB = atoi(argv[15]);
      SA_MODE1_PROB = atoi(argv[16]);
      MARGIN_THRESHOLD = atof(argv[17]);
      PROB_THRESHOLD_DECREASE_RATE = atof(argv[18]);
      PROB_THRESHOLD_2 = atof(argv[19]);
    }
  }

  //長方形の「細長さ」の平均を特徴量として利用
  double mean_ratio = 0;
  REP(i, N) {
    double ratio = (double)max(10000, max(W[i], H[i])) / max(10000, min(W[i], H[i]));
    mean_ratio += ratio;
  }
  mean_ratio /= N;

  vector<double> features_query_num = {(double)N, (double)T, (double)sigma, mean_ratio};
  REP(i, 4) features_query_num[i] = (features_query_num[i] - X_MEAN_SEARCH[i]) / X_STD_SEARCH[i];

  //線形回帰を用いてquery_turnを決定する
  double search_turn = INTERCEPT_SEARCH;
  REP(i, 4) search_turn += features_query_num[i] * COEF_SEARCH[i];
  int query_turn = T - round(search_turn);
  chmin(query_turn, T-8);
  chmax(query_turn, 0);
  cerr << "Query turn: " << query_turn << " Search turn: " << round(search_turn) << "\n";

  //int query_turn = T-N*SOLVE_TURN_RATIO;
  if(query_turn > 0) main_query_rectangle(query_turn);
  double sigma_mean = 0;
  REP(i, N) sigma_mean += sigma_W[i] + sigma_H[i];
  sigma_mean /= 2*N;
  cerr << "Sigma mean: " << sigma << " -> " << sigma_mean << "\n";
  sigma = sigma_mean;
  double complexity = N * sigma_mean;
  cerr << "Complexity: " << complexity << "\n";
  cout << "# Complexity: " << complexity << "\n";

  //推定後の値で再計算
  mean_ratio = 0;
  REP(i, N) {
    double ratio = (double)max(10000, max(W[i], H[i])) / max(10000, min(W[i], H[i]));
    mean_ratio += ratio;
  }
  mean_ratio /= N;

  vector<double> features = {(double)N, sigma_mean, mean_ratio};
  //2乗の項を追加
  REP(i, 3) FOR(j, i, 3) {
    features.push_back(features[i] * features[j]);
  }
  //特徴量の正規化
  REP(i, features.size()) {
    features[i] = (features[i] - X_MEAN[i]) / X_STD[i];
  }

  //ソルバー選択をロジスティック回帰で行う
  double pred = INTERCEPT;
  REP(i, features.size()) {
    pred += features[i] * COEF[i];
  }
  cerr << "Logistic Prediction: " << pred << "\n";

  if(pred < 0) {
    cerr << "Use solve_beam_rd\n";
    solve_beam_rd(T-query_turn, 2300);
  } else {
    cerr << "Use solve_beam_sa\n";
    solve_beam_sa(T-query_turn, BEAM_TIME_LIMIT, 2500 - BEAM_TIME_LIMIT);
  }
  if(DEBUG) {
    cerr << true_score << "\n";
  }

  /*
  //特徴量を出力
  for(double f: features) {
    cerr << f << " ";
  }
  cerr << "\n";
  cerr << true_score << "\n";
  */
}
