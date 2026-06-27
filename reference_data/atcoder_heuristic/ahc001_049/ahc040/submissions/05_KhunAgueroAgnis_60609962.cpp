#include <bits/stdc++.h>
using namespace std;
using ll = long long;
using P = pair<ll, ll>;
#define rep(i, a, b) for(int i = a; i < b; ++i)
#define rrep(i, a, b) for(int i = a; i >= b; --i)
constexpr ll inf = 4e18;

class TimeKeeperDouble {
 private:
  std::chrono::high_resolution_clock::time_point start_time_;
  double time_threshold_;

  double now_time_ = 0;

 public:
  // 時間制限をミリ秒単位で指定してインスタンスをつくる。
  TimeKeeperDouble(const double time_threshold)
      : start_time_(std::chrono::high_resolution_clock::now()),
        time_threshold_(time_threshold) {}

  // 経過時間をnow_time_に格納する。
  void setNowTime() {
    auto diff = std::chrono::high_resolution_clock::now() - this->start_time_;
    this->now_time_ =
        std::chrono::duration_cast<std::chrono::microseconds>(diff).count() *
        1e-3;  // ms
  }

  // 経過時間をnow_time_に取得する。
  double getNowTime() const { return this->now_time_; }

  // インスタンス生成した時から指定した時間制限を超過したか判定する。
  bool isTimeOver() const { return now_time_ >= time_threshold_; }
};


struct SetupIO {
    SetupIO() {
        ios::sync_with_stdio(0);
        cin.tie(0);
        cout << fixed << setprecision(30);
    }
} setup_io;

// 構造体の定義
struct Box {
    int x, y;
    Box(int x, int y) : x(x), y(y){}
};

struct PRDB {
    int p, r;
    int b;
    PRDB(int p, int r, int b) : p(p), r(r), b(b){}

    bool operator==(const PRDB& other) const {
        return p == other.p and r == other.r and b == other.b;
    }

    PRDB() {p = 0; r = 0; b = 0;}
};

struct Beam {
    vector<int> widths;
    vector<int> heights;
    int width;
    int height;
    PRDB prdbs;
    int parent;
    vector<int> arrangement;
    vector<int> num;
    ll score;

    Beam(vector<int> widths, vector<int> heights, int width, int height, PRDB prdbs, int parent, vector<int> arrangement, vector<int> num, ll score) : widths(widths), heights(heights), width(width), height(height), prdbs(prdbs), parent(parent), arrangement(arrangement), num(num), score(score) {}
    Beam() {widths = {}; heights = {}; width = 0; height = 0; prdbs = PRDB(); parent = -1; arrangement = {}; num = {}; score = 0;}
};

struct Action {
    int parent_beam;
    int row;
    int w;
    int h;
    bool new_row;
    PRDB prdb;
    ll score;
    Action(int parent_beam, int row, int w, int h, bool new_row, PRDB prdb, ll score) : parent_beam(parent_beam), row(row), w(w), h(h), new_row(new_row), prdb(prdb), score(score) {}
};

// 変数の定義
int n, t, sigma;
int num_query = 0;

vector<Box> boxes;
vector<vector<int>> bigger(101, vector<int>(101, -1));
vector<vector<int>> bigger_max_edge(101, vector<int>(101, -1));

vector<vector<Beam>> beams;

float sigma_ratio = 1.55;
int beam_width = 300;
float gap_ratio = 1;
float height_tolerance = 1.1;
float width_remain_ratio = 1000000;
int min_t = 7;
int time_threshold = 2850;
int time_deadline = 2850;


vector<PRDB> attempt(Beam beam) {
    vector<PRDB> tmp = {beam.prdbs};
    int parent =  beam.parent;
    rrep(i, n-2, 0)  {
        tmp.push_back(beams[i][parent].prdbs);
        parent = beams[i][parent].parent;
    }

    return tmp;
}



int main(void) {
    auto time_keeper = TimeKeeperDouble(time_threshold);
    auto deadline_keeper = TimeKeeperDouble(time_deadline);

    // 入力を行う
    cin >> n >> t >> sigma;
    rep(i, 0, n) {
        int w, h;
        cin >> w >> h;
        boxes.push_back(Box(w, h));
    }

    // 箱の大きさの標準偏差を小さくする
    int original_sigma = sigma;
    int precise_i = n;
    if(t >= 3 * n + min_t) {
        rep(i, 0, n) {
            cout << 1 << endl;
            cout << i << ' ' << 0 << ' ' << 'U' << ' ' << -1 << endl;

            int w, h;
            cin >> w >> h;

            t--;

            boxes[i].x += w;
            boxes[i].y += h;

            cout << 1 << endl;
            cout << i << ' ' << 0 << ' ' << 'U' << ' ' << -1 << endl;

            cin >> w >> h;

            t--;

            boxes[i].x += w;
            boxes[i].y += h;

            cout << 1 << endl;
            cout << i << ' ' << 0 << ' ' << 'U' << ' ' << -1 << endl;

            cin >> w >> h;

            t--;

            boxes[i].x += w;
            boxes[i].y += h;

            boxes[i].x /= 4;
            boxes[i].y /= 4;
        }
        sigma /= 2;
    } else if(t >= 2 * n + min_t) {
        rep(i, 0, n) {
            cout << 1 << endl;
            cout << i << ' ' << 0 << ' ' << 'U' << ' ' << -1 << endl;

            int w, h;
            cin >> w >> h;

            t--;

            boxes[i].x += w;
            boxes[i].y += h;

            cout << 1 << endl;
            cout << i << ' ' << 0 << ' ' << 'U' << ' ' << -1 << endl;

            cin >> w >> h;

            t--;

            boxes[i].x += w;
            boxes[i].y += h;

            boxes[i].x /= 3;
            boxes[i].y /= 3;
        }
        sigma /= sqrt(3);
    }
    else if(t >= n + min_t) {
        rep(i, 0, n) {
            cout << 1 << endl;
            cout << i << ' ' << 0 << ' ' << 'U' << ' ' << -1 << endl;

            int w, h;
            cin >> w >> h;

            t--;

            boxes[i].x = (boxes[i].x + w) / 2;
            boxes[i].y = (boxes[i].y + h) / 2;
        }
        sigma /= sqrt(2);
    }  else {
        precise_i = t - min_t;
        rep(i, 0, t - min_t) {
            cout << 1 << endl;
            cout << i << ' ' << 0 << ' ' << 'U' << ' ' << -1 << endl;

            int w, h;
            cin >> w >> h;

            t--;

            boxes[i].x = (boxes[i].x + w) / 2;
            boxes[i].y = (boxes[i].y + h) / 2;
        }
        sigma /= sqrt(2);
    }

    rep(i, 0, n) {
        if(boxes[i].x > 1e5) boxes[i].x = 1e5;
        if(boxes[i].y > 1e5) boxes[i].y = 1e5;
        if(boxes[i].x < 1e4) boxes[i].x = 1e4;
        if(boxes[i].y < 1e4) boxes[i].y = 1e4;
    }

    beam_width = beam_width * 100 * 100 / t / n;
    
    bool slow_down = false;

    // 面積を計算する
    ll area = 0;
    rep(i, 0, n) area += (ll) boxes[i].x * boxes[i].y;

    vector<pair<int, vector<PRDB>>> cands;

    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    normal_distribution<> dist(0, 0.03);

    while(1) {
        beams = {{Beam({boxes[0].x}, {boxes[0].y}, boxes[0].x, boxes[0].y, PRDB(0, 0, -1), -1, {0}, {1}, 0), Beam({boxes[0].y}, {boxes[0].x}, boxes[0].y, boxes[0].x, PRDB(0, 1, -1), -1, {0}, {1}, 0)}};
        rep(i, 0, n-1) {
            beams.push_back({});
        }

        // 許容幅を計算
        int width = sqrt(area) * (1.04 + dist(engine));

        rep(i, 1, n) {
            if(i == precise_i) swap(sigma, original_sigma);
            time_keeper.setNowTime();
            if (time_keeper.isTimeOver() and !slow_down) {
                beam_width /= 10;
                slow_down = true;
            }

            deadline_keeper.setNowTime();
            if (deadline_keeper.isTimeOver()) {
                break;
            }

            // 考えられる置き方を全探索
            vector<Action> actions;
            rep(b, 0, (int) beams[i-1].size()) {
                rep(r, 0, 2) {
                    int w, h;
                    if (r & 1) {
                        w = boxes[i].y;
                        h = boxes[i].x;
                    } else {
                        w = boxes[i].x;
                        h = boxes[i].y;
                    }
                    // if(w > h) continue;
                    // 新しい行を作る
                    if (beams[i-1][b].widths.back() - sigma * sigma_ratio > w + sigma * sigma_ratio) {
                        actions.push_back(Action(b, beams[i-1][b].heights.size(), w, h, true, PRDB(i, r, -1), beams[i-1][b].score - 100000000000000*max(beams[i-1][b].height + h - height_tolerance * (area / width), (float)0.0) - 1000));
                        // cerr << beams[i-1][b].score - 100000000000000*max(beams[i-1][b].height + h - height_tolerance * (area / width), (float)0.0) - 1000 << endl;
                    }

                    // 既存の行に追加
                    rep(row, 0, beams[i-1][b].heights.size()) {
                        // 上の列より長くなってはいけない
                        ll burden = -1000000000000000;
                        if(row == 0 or (width - beams[i-1][b].widths[row-1]) < width / sqrt(n) / 2.25 or beams[i-1][b].widths[row-1] - sigma * sigma_ratio * sqrt(beams[i-1][b].num[row-1]) > beams[i-1][b].widths[row] + w + sigma * sigma_ratio * sqrt(beams[i-1][b].num[row])) {
                            burden = 0;
                        }


                        if(width < beams[i-1][b].widths[row]+w) continue;

                        // 上の列に置くほど加点
                        ll higher = (-1) * row * 50000;

                        // 残りの幅が狭いほど加点
                        // ll narrow = (width - beams[i-1][b].widths[row] < width / 6) ? pow(-(width - beams[i-1][b].widths[row] - w) + width, 2) : 0;
                        ll narrow = (width - beams[i-1][b].widths[row] - w < w) ? pow(1 - (width - beams[i-1][b].widths[row] - w) * 1.0 / w, 2) * width_remain_ratio : 0;

                        if(beams[i-1][b].heights[row] > h) {
                            // cerr << "a " << higher << ' ' << narrow << ' ' << pow(beams[i-1][b].heights[row] * 1.0 / h, 6) * gap_ratio << beams[i-1][b].score - pow(beams[i-1][b].heights[row] * 1.0 / h, 6) * gap_ratio  + burden + higher + narrow << endl;
                            actions.push_back(Action(b, row, w, h, false, PRDB(i, r, beams[i-1][b].arrangement[row]), beams[i-1][b].score - (ll)(beams[i-1][b].heights[row] - h) * w + burden + higher + narrow));
                        }
                        else {
                            // cerr << "b " << higher << ' ' << narrow << ' ' << pow(h * 1.0 / beams[i-1][b].heights[row], 6) * gap_ratio << ' ' <<  beams[i-1][b].score - pow(h * 1.0 / beams[i-1][b].heights[row], 6) * gap_ratio + burden + higher + narrow << endl;
                            actions.push_back(Action(b, row, w, h, false, PRDB(i, r, beams[i-1][b].arrangement[row]), beams[i-1][b].score - (ll)(h - beams[i-1][b].heights[row]) * beams[i-1][b].widths[row] + burden + higher + narrow));
                        }
                    }
                }
            }
            // スコアが上位を保存
            vector<Action> sorted_actions;
            bool sorted = false;

            rep(a, 0, actions.size()) {
                ll score = actions[a].score;
                if(sorted && sorted_actions[beam_width-1].score >= score) continue;

                sorted_actions.push_back(actions[a]);

                if(sorted_actions.size() >= 2 * beam_width) {
                    sort(sorted_actions.begin(), sorted_actions.end(), [](Action a, Action b) { return a.score > b.score; });
                    rep(j, 0, 2*beam_width - beam_width) {
                        sorted_actions.pop_back();
                    }
                    sorted = true;
                }
            }

       

            swap(actions, sorted_actions);
            sort(actions.begin(), actions.end(), [](Action a, Action b) { return a.score > b.score; });

            vector<Beam> new_beams(min((int) actions.size(), beam_width));

            int skip = 0;
            rep(k, 0, min((int) actions.size(), beam_width)) {
                int j = k;
                if(actions.size() > beam_width + skip) j = k + skip;
                // ランダムでスキップ
                if(rand() % 10 == 0) {
                    skip++;
                }
                Beam new_beam = beams[i-1][actions[j].parent_beam];
                new_beam.parent = actions[j].parent_beam;
                new_beam.prdbs = actions[j].prdb;
                new_beam.score = actions[j].score;

                if(actions[j].new_row) {
                    new_beam.arrangement.push_back((int)i);
                    new_beam.num.push_back(1);
                    new_beam.widths.push_back(actions[j].w);
                    new_beam.heights.push_back(actions[j].h);
                    new_beam.height += actions[j].h;
                    new_beam.width = max(new_beam.width, actions[j].w);
                }
                else {
                    new_beam.arrangement[actions[j].row] = i;
                    new_beam.num[actions[j].row]++;
                    new_beam.widths[actions[j].row] += actions[j].w;
                    new_beam.height += (max(max(new_beam.heights[actions[j].row], actions[j].h) - new_beam.heights[actions[j].row], 0));
                    new_beam.width = max(new_beam.width, new_beam.widths[actions[j].row]);
                    new_beam.heights[actions[j].row] = max(new_beam.heights[actions[j].row], actions[j].h);
                }
                new_beams[k] = new_beam;
            }
            swap(beams[i], new_beams);
        }

        swap(sigma, original_sigma);

        time_keeper.setNowTime();
        if (time_keeper.isTimeOver() and !slow_down) {
            beam_width /= 10;
            slow_down = true;
        }

        deadline_keeper.setNowTime();
        if (deadline_keeper.isTimeOver()) {
            break;
        }

        int num = 0;
        int min_score = 1001001001;

        rep(i, 0, (int) beams[n-1].size()) {
            if(beams[n-1][i].height + beams[n-1][i].width < min_score) {
                min_score = beams[n-1][i].height + beams[n-1][i].width;
                num = i;
            }
        }

        auto prdbs = attempt(beams[n-1][num]);
        cands.push_back(make_pair(beams[n-1][num].width + beams[n-1][num].height, prdbs));
    }

    sort(cands.begin(), cands.end(), [](pair<int, vector<PRDB>> a, pair<int, vector<PRDB>> b) { return a.first < b.first; });
    rep(i, 0, min((int)cands.size(), t)) {
        cout << n << endl;
        rrep(j, n-1, 0) {
            cout << cands[i].second[j].p << ' ' << cands[i].second[j].r << ' ' << 'U' << ' ' << cands[i].second[j].b << endl;
        }
        num_query++;
    }

    assert(num_query <= t);
    cerr << num_query << endl;
    if(num_query < t) {
        rep(i, num_query, t) {
            cout << 0 << endl;
        }
    }
}
