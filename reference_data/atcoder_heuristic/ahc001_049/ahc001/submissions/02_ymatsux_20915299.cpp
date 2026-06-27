#include <bits/stdc++.h>

using namespace std;
using int64 = long long;

constexpr int DEBUG = 1;

constexpr int X_MAX = 10000;
constexpr int Y_MAX = 10000;

struct Rectangle {
  int x1, y1, x2, y2;
  Rectangle(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}
  Rectangle() {}
  int Area() const {
    return (x2 - x1) * (y2 - y1);
  }
  string ToString() const {
    return "((" + to_string(x1) + ", " + to_string(y1) + "), " +
           "(" + to_string(x2) + ", " + to_string(y2) + "))";
  };
};

bool HasIntersection(const Rectangle& r1, const Rectangle& r2) {
  int x1 = max(r1.x1, r2.x1);
  int x2 = min(r1.x2, r2.x2);
  int y1 = max(r1.y1, r2.y1);
  int y2 = min(r1.y2, r2.y2);
  return x1 < x2 && y1 < y2;
}

bool HasIntersectionWithCenter(const Rectangle& r, int x, int y) {
  return HasIntersection(r, Rectangle(x, y, x + 1, y + 1));
}

vector<double> ComputeScores(
    const vector<int>& requests,
    const vector<Rectangle>& rectangles) {
  int n = rectangles.size();
  vector<double> scores(n);
  for (int i = 0; i < n; i++) {
    double q = double(min(requests[i], rectangles[i].Area())) /
              double(max(requests[i], rectangles[i].Area()));
    double p = 1.0 - (1.0 - q) * (1.0 - q);
    scores[i] = p * 1.0E9;
  }
  return scores;
}

double Sum(const vector<double>& xs) {
  double sum = 0;
  for (double x : xs) {
    sum += x;
  }
  return sum;
}

double OfficialScore(const vector<double>& xs) {
  double sum = 0;
  for (double x : xs) {
    sum += x;
  }
  return sum / xs.size();
}

double ToMilliseconds(chrono::system_clock::duration t) {
  return double(chrono::duration_cast<chrono::microseconds>(t).count()) / 1E3;
}

int main() {
  ios::sync_with_stdio(false);
  cin.tie(0);

  auto time_start = chrono::system_clock::now();

  default_random_engine generator;

  int n;
  cin >> n;
  vector<int> xs(n), ys(n), requests(n);
  for (int i = 0; i < n; i++) {
    cin >> xs[i] >> ys[i] >> requests[i];
  }

  vector<Rectangle> rectangles(n);
  vector<double> scores(n);
  auto reset_fn = [&]() {
    for (int i = 0; i < n; i++) {
      rectangles[i] = Rectangle(xs[i], ys[i], xs[i] + 1, ys[i] + 1);
    }
    scores = ComputeScores(requests, rectangles);
  };
  reset_fn();

  double best_score = -1.0;
  vector<Rectangle> best_rectangles = rectangles;

  // Simulated Annealing phase.
  constexpr double MAX_PSEUDO_TIME = 1E9;
  constexpr double INITIAL_PSUEDO_TIME_STEP = 1;
  double START_T = 1E9;
  // const double START_T = 1;
  double END_T = 1E7;
  int pseudo_time_step_size = INITIAL_PSUEDO_TIME_STEP;
  int effective_count = 0;
  float TIME_LIMIT = 4500;
  auto time_sa_start = chrono::system_clock::now();

  auto step_fn = [&](int id, int max_step_size, double temperature) -> bool {
    int n = rectangles.size();
    Rectangle r = rectangles[id];
    int dir = uniform_int_distribution<int>(0, 3)(generator);
    if (dir == 0) {
      int step_cap = (requests[id] - r.Area()) / (r.y2 - r.y1);
      int step = min(max_step_size, step_cap);
      r.x2 = min(r.x2 + step, X_MAX);
    } else if (dir == 1) {
      int step_cap = (requests[id] - r.Area()) / (r.x2 - r.x1);
      int step = min(max_step_size, step_cap);
      r.y2 = min(r.y2 + step, Y_MAX);
    } else if (dir == 2) {
      int step_cap = (requests[id] - r.Area()) / (r.y2 - r.y1);
      int step = min(max_step_size, step_cap);
      r.x1 = max(r.x1 - step, 0);
    } else {
      int step_cap = (requests[id] - r.Area()) / (r.x2 - r.x1);
      int step = min(max_step_size, step_cap);
      r.y1 = max(r.y1 - step, 0);
    }

    for (int i = 0; i < n; i++) {
      if (i == id) continue;
      if (HasIntersectionWithCenter(r, xs[i], ys[i])) {
        if (dir == 0 || dir == 2) {
          if (ys[id] == ys[i]) return false;
          if (ys[id] > ys[i]) {
            r.y1 = ys[i] + 1;
          } else {  // ys[id] < ys[i]
            r.y2 = ys[i];
          }
        }
        if (dir == 1 || dir == 3) {
          if (xs[id] == xs[i]) return false;
          if (xs[id] > xs[i]) {
            r.x1 = xs[i] + 1;
          } else {
            r.x2 = xs[i];
          }
        }
      }
    }

    vector<int> changed_rectangles;
    changed_rectangles.push_back(id);

    double current_score = Sum(scores);

    vector<Rectangle> new_rectangles = rectangles;
    new_rectangles[id] = r;
    for (int i = 0; i < n; i++) {
      if (i == id) continue;
      if (HasIntersection(rectangles[i], r)) {
        vector<Rectangle> candidates;
        if (rectangles[i].x1 < r.x1) {
          Rectangle candidate = rectangles[i];
          candidate.x2 = r.x1;
          candidates.push_back(candidate);
        }
        if (rectangles[i].x2 > r.x2) {
          Rectangle candidate = rectangles[i];
          candidate.x1 = r.x2;
          if (candidate.x1 < candidate.x2) {
            candidates.push_back(candidate);
          }
        }
        if (rectangles[i].y1 < r.y1) {
          Rectangle candidate = rectangles[i];
          candidate.y2 = r.y1;
          if (candidate.y1 < candidate.y2) {
            candidates.push_back(candidate);
          }
        }
        if (rectangles[i].y2 > r.y2) {
          Rectangle candidate = rectangles[i];
          candidate.y1 = r.y2;
          if (candidate.y1 < candidate.y2) {
            candidates.push_back(candidate);
          }
        }

        if (candidates.empty()) {
          cerr << "Unexpected!" << endl;
          exit(1);
        }

        double max_area = 0;
        // int max_area_index = -1;
        for (int c_index = 0; c_index < candidates.size(); c_index++) {
          if (!HasIntersectionWithCenter(candidates[c_index], xs[i], ys[i])) {
            continue;
          }
          if (max_area < candidates[c_index].Area()) {
            max_area = candidates[c_index].Area();
            // max_area_index = c_index;
          }
        }

        vector<pair<int, double>> candidate_scores;
        double candidate_score_sum = 0;
        for (int c_index = 0; c_index < candidates.size(); c_index++) {
          if (!HasIntersectionWithCenter(candidates[c_index], xs[i], ys[i])) {
            continue;
          }
          double area = candidates[c_index].Area();
          // double candidate_score = exp((area - max_area) / temperature);
          double candidate_score = 1.0;  // Use pure-random instead of soft-max.
          candidate_scores.push_back({c_index, candidate_score});
          candidate_score_sum += candidate_score;
        }

        double random = uniform_real_distribution<double>(
            0, candidate_score_sum)(generator);

        double a = 0;
        int candidate_index = -1;
        for (int i = 0; i < candidate_scores.size(); i++) {
          a += candidate_scores[i].second;
          if (a >= random) {
            candidate_index = candidate_scores[i].first;
            break;
          }
        }

        if (candidate_index < 0) {
          cerr << "Unexpected!" << endl;
          exit(1);
        }

        new_rectangles[i] = candidates[candidate_index];

        changed_rectangles.push_back(i);
      }
    }

    // TODO: Consider specifying a generator.
    int expand_index_size = changed_rectangles.size() * 4;
    vector<int> expand_indexes;
    for (int i = 0; i < expand_index_size; i++) {
      expand_indexes.push_back(i);
    }
    random_shuffle(expand_indexes.begin(), expand_indexes.end());
    for (int expand_index : expand_indexes) {
      int changed_rectangle_id = changed_rectangles[expand_index / 4];
      int d = expand_index % 4;
      // vector<int> ds({0, 1, 2, 3});
      // random_shuffle(ds.begin(), ds.end());
      // for (int d : ds) {
        Rectangle s = new_rectangles[changed_rectangle_id];
        int h = s.y2 - s.y1;
        int w = s.x2 - s.x1;
        if (d == 0) {
          int step_cap = (requests[changed_rectangle_id] - s.Area()) / h;
          int step = step_cap;
          s.x2 = min(s.x2 + step, X_MAX);
          for (int i = 0; i < n; i++) {
            if (i == changed_rectangle_id) continue;
            if (HasIntersection(s, new_rectangles[i])) {
              s.x2 = new_rectangles[i].x1;
            }
          }
        } else if (d == 1) {
          int step_cap = (requests[changed_rectangle_id] - s.Area()) / w;
          int step = step_cap;
          s.y2 = min(s.y2 + step, Y_MAX);
          for (int i = 0; i < n; i++) {
            if (i == changed_rectangle_id) continue;
            if (HasIntersection(s, new_rectangles[i])) {
              s.y2 = new_rectangles[i].y1;
            }
          }
        } else if (d == 2) {
          int step_cap = (requests[changed_rectangle_id] - s.Area()) / h;
          int step = step_cap;
          s.x1 = max(s.x1 - step, 0);
          for (int i = 0; i < n; i++) {
            if (i == changed_rectangle_id) continue;
            if (HasIntersection(s, new_rectangles[i])) {
              s.x1 = new_rectangles[i].x2;
            }
          }
        } else {
          int step_cap = (requests[changed_rectangle_id] - s.Area()) / w;
          int step = step_cap;
          s.y1 = max(s.y1 - step, 0);
          for (int i = 0; i < n; i++) {
            if (i == changed_rectangle_id) continue;
            if (HasIntersection(s, new_rectangles[i])) {
              s.y1 = new_rectangles[i].y2;
            }
          }
        }
        if (s.Area() <= 0) {
          cerr << "s has non-positive area!" << endl;
          exit(1);
        }
        new_rectangles[changed_rectangle_id] = s;
      // }
    }

    vector<double> new_scores = ComputeScores(requests, new_rectangles);

    double new_score = Sum(new_scores);
    double probability = exp((new_score - current_score) / temperature);
    bool force_next =
        probability > uniform_real_distribution<double>(0, 1)(generator);
    if (new_score > current_score || force_next) {
      rectangles = new_rectangles;
      scores = new_scores;
      if (new_score > best_score) {
        best_score = new_score;
        best_rectangles = rectangles;
      }
      return true;
    } else {
      return false;
    }
  };

  auto step_type_2_fn = [&](int id, int max_step_size, double temperature) -> bool {
    int n = rectangles.size();
    Rectangle r = rectangles[id];
    // double random_1 = uniform_real_distribution<double>(0, 1)(generator);
    double random_1 = 1;
    int target_area = int(r.Area() + (requests[id] - r.Area()) * random_1);
    double random_2 = uniform_real_distribution<double>(0, 1)(generator);
    double h_w_ratio = (1.0 - random_2) / random_2;
    int w = sqrt(target_area / h_w_ratio);
    int h = sqrt(target_area / h_w_ratio) * h_w_ratio;
    if (w == 0 || h == 0) return false;

    // TODO: Refine this part.
    int x1_min = xs[id] + 1 - w;
    int x1_max = xs[id];
    r.x1 = uniform_int_distribution<int>(x1_min, x1_max)(generator);
    r.x2 = r.x1 + w;
    int y1_min = ys[id] + 1 - h;
    int y1_max = ys[id];
    r.y1 = uniform_int_distribution<int>(y1_min, y1_max)(generator);
    r.y2 = r.y1 + h;

    r.x1 = max(r.x1, 0);
    r.x2 = min(r.x2, 10000);
    r.y1 = max(r.y1, 0);
    r.y2 = min(r.y2, 10000);

    if (!HasIntersectionWithCenter(r, xs[id], ys[id])) {
      cerr << "Unexpected! @" << __LINE__ << endl;
      exit(1);
    }

    for (int i = 0; i < n; i++) {
      if (i == id) continue;
      if (HasIntersectionWithCenter(r, xs[i], ys[i])) {
        // cerr << "id: " << id << " i: " << i << endl;
        // TODO: Is this really a good approach?
        int random_flag = uniform_int_distribution<int>(0, 1)(generator);
        if (random_flag) {
          if (ys[id] == ys[i]) return false;
          if (ys[id] > ys[i]) {
            r.y1 = ys[i] + 1;
          } else {  // ys[id] < ys[i]
            r.y2 = ys[i];
          }
        } else {
          if (xs[id] == xs[i]) return false;
          if (xs[id] > xs[i]) {
            r.x1 = xs[i] + 1;
          } else {
            r.x2 = xs[i];
          }
        }
      }
    }

    if (!HasIntersectionWithCenter(r, xs[id], ys[id])) {
      cerr << "Unexpected! @" << __LINE__ << endl;
      exit(1);
    }

    if (r.Area() < rectangles[id].Area()) {
      return false;
    }

    vector<int> changed_rectangles;
    changed_rectangles.push_back(id);

    double current_score = Sum(scores);

    vector<Rectangle> new_rectangles = rectangles;
    new_rectangles[id] = r;
    for (int i = 0; i < n; i++) {
      if (i == id) continue;
      if (HasIntersection(rectangles[i], r)) {
        if (HasIntersectionWithCenter(r, xs[i], ys[i])) {
          cerr << "Unexpected! @" << __LINE__ << endl;
          exit(1);
        }
        vector<Rectangle> candidates;
        if (rectangles[i].x1 < r.x1) {
          Rectangle candidate = rectangles[i];
          candidate.x2 = r.x1;
          if (candidate.x1 < candidate.x2 &&
              HasIntersectionWithCenter(candidate, xs[i], ys[i])) {
            candidates.push_back(candidate);
          }
        }
        if (rectangles[i].x2 > r.x2) {
          Rectangle candidate = rectangles[i];
          candidate.x1 = r.x2;
          if (candidate.x1 < candidate.x2 &&
              HasIntersectionWithCenter(candidate, xs[i], ys[i])) {
            candidates.push_back(candidate);
          }
        }
        if (rectangles[i].y1 < r.y1) {
          Rectangle candidate = rectangles[i];
          candidate.y2 = r.y1;
          if (candidate.y1 < candidate.y2 &&
              HasIntersectionWithCenter(candidate, xs[i], ys[i])) {
            candidates.push_back(candidate);
          }
        }
        if (rectangles[i].y2 > r.y2) {
          Rectangle candidate = rectangles[i];
          candidate.y1 = r.y2;
          if (candidate.y1 < candidate.y2 &&
              HasIntersectionWithCenter(candidate, xs[i], ys[i])) {
            candidates.push_back(candidate);
          }
        }

        if (candidates.empty()) {
          cerr << "Unexpected! @" << __LINE__ << endl;
          cerr << r.ToString() << endl;
          cerr << rectangles[i].ToString() << endl;
          cerr << xs[i] << " " << ys[i] << endl;
          exit(1);
        }

        vector<pair<int, double>> candidate_scores;
        double candidate_score_sum = 0;
        for (int c_index = 0; c_index < candidates.size(); c_index++) {
          if (!HasIntersectionWithCenter(candidates[c_index], xs[i], ys[i])) {
            continue;
          }
          double area = candidates[c_index].Area();
          // double candidate_score = exp((area - max_area) / temperature);
          double candidate_score = 1.0;  // Use pure-random instead of soft-max.
          candidate_scores.push_back({c_index, candidate_score});
          candidate_score_sum += candidate_score;
        }

        if (candidate_scores.empty()) {
          cerr << "Unexpected! @" << __LINE__ << endl;
          exit(1);
        }

        double random = uniform_real_distribution<double>(
            0, candidate_score_sum)(generator);

        double a = 0;
        int candidate_index = -1;
        for (int i = 0; i < candidate_scores.size(); i++) {
          a += candidate_scores[i].second;
          if (a >= random) {
            candidate_index = candidate_scores[i].first;
            break;
          }
        }

        if (candidate_index < 0) {
          cerr << "Unexpected! @" << __LINE__ << endl;
          exit(1);
        }

        new_rectangles[i] = candidates[candidate_index];

        changed_rectangles.push_back(i);
      }
    }

    // TODO: Consider specifying a generator.
    int expand_index_size = changed_rectangles.size() * 4;
    vector<int> expand_indexes;
    for (int i = 0; i < expand_index_size; i++) {
      expand_indexes.push_back(i);
    }
    random_shuffle(expand_indexes.begin(), expand_indexes.end());
    for (int expand_index : expand_indexes) {
      int changed_rectangle_id = changed_rectangles[expand_index / 4];
      int d = expand_index % 4;
      // vector<int> ds({0, 1, 2, 3});
      // random_shuffle(ds.begin(), ds.end());
      // for (int d : ds) {
        Rectangle s = new_rectangles[changed_rectangle_id];
        int h = s.y2 - s.y1;
        int w = s.x2 - s.x1;
        if (d == 0) {
          int step_cap = (requests[changed_rectangle_id] - s.Area()) / h;
          int step = step_cap;
          s.x2 = min(s.x2 + step, X_MAX);
          for (int i = 0; i < n; i++) {
            if (i == changed_rectangle_id) continue;
            if (HasIntersection(s, new_rectangles[i])) {
              s.x2 = new_rectangles[i].x1;
            }
          }
        } else if (d == 1) {
          int step_cap = (requests[changed_rectangle_id] - s.Area()) / w;
          int step = step_cap;
          s.y2 = min(s.y2 + step, Y_MAX);
          for (int i = 0; i < n; i++) {
            if (i == changed_rectangle_id) continue;
            if (HasIntersection(s, new_rectangles[i])) {
              s.y2 = new_rectangles[i].y1;
            }
          }
        } else if (d == 2) {
          int step_cap = (requests[changed_rectangle_id] - s.Area()) / h;
          int step = step_cap;
          s.x1 = max(s.x1 - step, 0);
          for (int i = 0; i < n; i++) {
            if (i == changed_rectangle_id) continue;
            if (HasIntersection(s, new_rectangles[i])) {
              s.x1 = new_rectangles[i].x2;
            }
          }
        } else if (d == 3) {
          int step_cap = (requests[changed_rectangle_id] - s.Area()) / w;
          int step = step_cap;
          s.y1 = max(s.y1 - step, 0);
          for (int i = 0; i < n; i++) {
            if (i == changed_rectangle_id) continue;
            if (HasIntersection(s, new_rectangles[i])) {
              s.y1 = new_rectangles[i].y2;
            }
          }
        }
        if (s.Area() <= 0) {
          cerr << "s has non-positive area!" << endl;
          exit(1);
        }
        new_rectangles[changed_rectangle_id] = s;
      // }
    }

    vector<double> new_scores = ComputeScores(requests, new_rectangles);

    double new_score = Sum(new_scores);
    double probability = exp((new_score - current_score) / temperature);
    bool force_next =
        probability > uniform_real_distribution<double>(0, 1)(generator);
    if (new_score > current_score || force_next) {
      rectangles = new_rectangles;
      scores = new_scores;
      if (new_score > best_score) {
        best_score = new_score;
        best_rectangles = rectangles;
      }
      return true;
    } else {
      return false;
    }
  };

  int64 step_suceess_count = 0;
  int64 step_failure_count = 0;

  auto last_measured_time = chrono::system_clock::now();

  vector<int> candidates;
  for (int i = 0; i < n; i++) {
    candidates.push_back(i);
  }

  auto update_candidates_fn = [&](double area_threshold) {
    candidates.clear();
    for (int i = 0; i < n; i++) {
      if (rectangles[i].Area() < requests[i] * area_threshold) {
        candidates.push_back(i);
      }
    }
    if (!candidates.empty()) {
      return;
    }

    for (int i = 0; i < 10; i++) {
      area_threshold += (1.0 - area_threshold) / 2.0;
      for (int i = 0; i < n; i++) {
        if (rectangles[i].Area() < requests[i] * area_threshold) {
          candidates.push_back(i);
        }
      }
      if (!candidates.empty()) {
        return;
      }
    }

    for (int i = 0; i < n; i++) {
      candidates.push_back(i);
    }
  };

  auto run_sa_fn = [&]() {
    for (double iter = 0; iter < MAX_PSEUDO_TIME;
         iter += pseudo_time_step_size) {
      double relative_time = double(iter) / double(MAX_PSEUDO_TIME);
      double area_threshold_min = 0.75;
      double are_threshold_max = 0.975;
      double area_threshold =
          area_threshold_min +
          relative_time * (are_threshold_max - area_threshold_min);

      if (effective_count % 10000 == 0) {
        // cerr << "Revert!" << endl;
        rectangles = best_rectangles;
        update_candidates_fn(area_threshold);
      }

      constexpr int DEBUG = 0;
      if (DEBUG && effective_count % 10000 == 0) {
        cerr << "effective_count: " << effective_count << endl;
        cerr << "relative_time: "
             << double(iter) / double(MAX_PSEUDO_TIME) << endl;
        cerr << "step_success: " << step_suceess_count << endl;
        cerr << "step_failure: " << step_failure_count << endl;
        double relative_time = double(iter) / double(MAX_PSEUDO_TIME);
        double temperature = exp(
            log(START_T) + relative_time * (log(END_T) - log(START_T)));
        cerr << "temperature: " << temperature << endl;
        cerr << int64(best_score / n) << endl;
      }

      const int MEASURE_TIME_PER_COUNT = 10000;
      if (effective_count > 0 &&
          effective_count % MEASURE_TIME_PER_COUNT == 0) {
        auto time = chrono::system_clock::now();
        if (ToMilliseconds(time - time_start) > TIME_LIMIT) {
          // cerr << "Exit SA at: " << iter << endl;
          break;
        }

        double last_duration = ToMilliseconds(time - last_measured_time);
        double remaining_time =
            TIME_LIMIT - ToMilliseconds(time - time_sa_start);
        double remaining_count =
            (remaining_time / last_duration) * MEASURE_TIME_PER_COUNT;
        pseudo_time_step_size =
            (MAX_PSEUDO_TIME - iter) / remaining_count;

        last_measured_time = time;
      }
      effective_count++;
      double temperature = exp(
          log(START_T) + relative_time * (log(END_T) - log(START_T)));

      int id = candidates[uniform_int_distribution<int>(
          0, candidates.size() - 1)(generator)];
      Rectangle rectangle = rectangles[id];
      double random = uniform_real_distribution<double>(0, 1)(generator);
      if (rectangle.Area() < requests[id] * area_threshold) {
        double r = uniform_real_distribution<double>(0, 1)(generator);
        int step_size = max(int(exp(r * log(10000))), 1);

        // int64 current_best_score = int64(best_score / n);
        bool step_succeeded = false;
        if (uniform_real_distribution<double>(0, 1)(generator) < 0.75) {
          step_succeeded = step_fn(id, step_size, temperature);
        } else {
          step_succeeded = step_type_2_fn(id, step_size, temperature);
        }
        if (step_succeeded) {
          step_suceess_count++;

          update_candidates_fn(area_threshold);
        } else {
          step_failure_count++;
        }

        // int64 new_best_score = int64(best_score / n);
        // if (new_best_score > current_best_score && new_best_score > 9.5E8) {
        //   cerr << "effective_count: " << effective_count << endl;
        //   cerr << new_best_score << endl;
        // }
        continue;
      } else {
        continue;
      }
    }
  };

  START_T = 3E8;
  END_T = 1E7;
  int ITER = 1;
  int UNIT_TIME = 4800;
  for (int i = 0; i < ITER; i++) {
    TIME_LIMIT = (1 + i) * UNIT_TIME;
    reset_fn();
    run_sa_fn();
  }

  cerr << "effective_count: " << effective_count << endl;
  cerr << "step_success: " << step_suceess_count << endl;
  cerr << "step_failure: " << step_failure_count << endl;

  // Choose the final output.
  if (best_score > Sum(scores)) {
    rectangles = best_rectangles;
  }

  cerr << "final_score: " << int64(best_score / n) << endl;

  for (int i = 0; i < n; i++) {
    cout << rectangles[i].x1 << " " << rectangles[i].y1 << " "
         << rectangles[i].x2 << " " << rectangles[i].y2 << endl;
  }
}