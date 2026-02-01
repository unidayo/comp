/**
 * ヨット（Yacht）期待値計算プログラム
 *
 * 概要:
 *   動的計画法を用いて、ヨットの最適戦略における期待得点を計算する。
 *   計算結果は C++ のヘッダファイルとして出力され、AIの実装に利用できる。
 *
 * 出力:
 *   yacht_dp_table.hpp - dp[upper_sum][used_hands] の二重vector
 *
 * ビルド:
 *   g++ -std=c++20 -O3 -march=native yacht_precompute.cpp -o yacht_precompute
 *
 * 依存:
 *   ankerl/unordered_dense.h （高速ハッシュマップ）
 */

#include <ankerl/unordered_dense.h>
#include <bits/stdc++.h>

using namespace std;

// =============================================================================
// 型定義
// =============================================================================

/// サイコロの出目分布を表す型
/// dice[i] = 目が (i+1) のサイコロの個数 (i = 0..5)
using Dice = array<int, 6>;

// =============================================================================
// 定数
// =============================================================================

namespace Constants {
constexpr int NUM_FACES = 6;                    // サイコロの面数
constexpr int NUM_DICE = 5;                     // サイコロの個数
constexpr int NUM_HANDS = 12;                   // 役の種類数
constexpr int NUM_ROLLS = 3;                    // 1ターンの振り回数
constexpr int UPPER_BONUS_THRESHOLD = 63;       // 上段ボーナスの閾値
constexpr int UPPER_BONUS_POINTS = 35;          // 上段ボーナスの得点
constexpr int UPPER_SUM_MAX = 64;               // 上段合計の状態数 (0-63)
constexpr int USED_HANDS_MAX = 1 << NUM_HANDS;  // 使用済み役の状態数 (4096)
}  // namespace Constants

/// 役のインデックス
namespace Hand {
constexpr int ONES = 0;
constexpr int TWOS = 1;
constexpr int THREES = 2;
constexpr int FOURS = 3;
constexpr int FIVES = 4;
constexpr int SIXES = 5;
constexpr int FULL_HOUSE = 6;
constexpr int FOUR_OF_A_KIND = 7;
constexpr int LITTLE_STRAIGHT = 8;
constexpr int BIG_STRAIGHT = 9;
constexpr int CHOICE = 10;
constexpr int YACHT = 11;

/// 役名の配列（デバッグ用）
const array<string, 12> NAMES = {"Ones",       "Twos",           "Threes",          "Fours",        "Fives",  "Sixes",
                                 "Full House", "Four of a Kind", "Little Straight", "Big Straight", "Choice", "Yacht"};

/// 上段の役かどうか
inline bool is_upper(int hand_index) { return hand_index < 6; }
}  // namespace Hand

// =============================================================================
// Dice ユーティリティ
// =============================================================================

namespace DiceUtil {

/// サイコロの合計個数を計算
inline int count(const Dice& dice) {
    int sum = 0;
    for (int i = 0; i < Constants::NUM_FACES; i++) {
        sum += dice[i];
    }
    return sum;
}

/// サイコロの出目の合計を計算（得点計算用）
inline int total_pips(const Dice& dice) {
    int sum = 0;
    for (int i = 0; i < Constants::NUM_FACES; i++) {
        sum += dice[i] * (i + 1);
    }
    return sum;
}

/// Dice を整数にエンコード（各目は0-5なので3ビットずつ）
inline int encode(const Dice& dice) { return dice[0] | (dice[1] << 3) | (dice[2] << 6) | (dice[3] << 9) | (dice[4] << 12) | (dice[5] << 15); }

/// 整数から Dice をデコード
inline Dice decode(int code) { return {code & 7, (code >> 3) & 7, (code >> 6) & 7, (code >> 9) & 7, (code >> 12) & 7, (code >> 15) & 7}; }

/// エンコードされた Dice の合計個数を計算
inline int count_from_code(int code) {
    return (code & 7) + ((code >> 3) & 7) + ((code >> 6) & 7) + ((code >> 9) & 7) + ((code >> 12) & 7) + ((code >> 15) & 7);
}

/// 2つの Dice を合成（足し算）
inline Dice add(const Dice& a, const Dice& b) { return {a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3], a[4] + b[4], a[5] + b[5]}; }

/// 空の Dice を作成
inline Dice empty() { return {0, 0, 0, 0, 0, 0}; }
}  // namespace DiceUtil

// =============================================================================
// 得点計算
// =============================================================================

namespace Scoring {

/// 上段の役（指定した目の合計）
inline int upper_section(const Dice& dice, int face_index) { return dice[face_index] * (face_index + 1); }

/// フルハウス（3つ同じ + 2つ同じ）
inline int full_house(const Dice& dice) {
    bool has_three = false;
    bool has_two = false;
    for (int i = 0; i < Constants::NUM_FACES; i++) {
        if (dice[i] == 3) has_three = true;
        if (dice[i] == 2) has_two = true;
    }
    return (has_three && has_two) ? DiceUtil::total_pips(dice) : 0;
}

/// フォーオブアカインド（4つ以上同じ）
inline int four_of_a_kind(const Dice& dice) {
    for (int i = 0; i < Constants::NUM_FACES; i++) {
        if (dice[i] >= 4) {
            return DiceUtil::total_pips(dice);
        }
    }
    return 0;
}

/// リトルストレート（4連続）
inline int little_straight(const Dice& dice) {
    // 1-2-3-4, 2-3-4-5, 3-4-5-6 のいずれか
    for (int start = 0; start <= 2; start++) {
        if (dice[start] >= 1 && dice[start + 1] >= 1 && dice[start + 2] >= 1 && dice[start + 3] >= 1) {
            return 15;
        }
    }
    return 0;
}

/// ビッグストレート（5連続）
inline int big_straight(const Dice& dice) {
    // 1-2-3-4-5 または 2-3-4-5-6
    for (int start = 0; start <= 1; start++) {
        if (dice[start] >= 1 && dice[start + 1] >= 1 && dice[start + 2] >= 1 && dice[start + 3] >= 1 && dice[start + 4] >= 1) {
            return 30;
        }
    }
    return 0;
}

/// チョイス（全サイコロの合計）
inline int choice(const Dice& dice) { return DiceUtil::total_pips(dice); }

/// ヨット（5つ全て同じ）
inline int yacht(const Dice& dice) {
    for (int i = 0; i < Constants::NUM_FACES; i++) {
        if (dice[i] == 5) {
            return 50;
        }
    }
    return 0;
}

/// 指定した役の得点を計算
inline int calculate(int hand_index, const Dice& dice) {
    switch (hand_index) {
        case Hand::ONES:
            return upper_section(dice, 0);
        case Hand::TWOS:
            return upper_section(dice, 1);
        case Hand::THREES:
            return upper_section(dice, 2);
        case Hand::FOURS:
            return upper_section(dice, 3);
        case Hand::FIVES:
            return upper_section(dice, 4);
        case Hand::SIXES:
            return upper_section(dice, 5);
        case Hand::FULL_HOUSE:
            return full_house(dice);
        case Hand::FOUR_OF_A_KIND:
            return four_of_a_kind(dice);
        case Hand::LITTLE_STRAIGHT:
            return little_straight(dice);
        case Hand::BIG_STRAIGHT:
            return big_straight(dice);
        case Hand::CHOICE:
            return choice(dice);
        case Hand::YACHT:
            return yacht(dice);
        default:
            return 0;
    }
}
}  // namespace Scoring

// =============================================================================
// 振り出しパターンの事前計算
// =============================================================================

class RollPatterns {
   public:
    /// roll_patterns[n] = n個のサイコロを振ったときの全出目パターン（エンコード済み）
    vector<vector<int>> patterns;

    RollPatterns() {
        patterns.resize(Constants::NUM_DICE + 1);

        for (int num_dice = 0; num_dice <= Constants::NUM_DICE; num_dice++) {
            int total_patterns = static_cast<int>(pow(Constants::NUM_FACES, num_dice));
            patterns[num_dice].reserve(total_patterns);

            for (int pattern_id = 0; pattern_id < total_patterns; pattern_id++) {
                Dice dice = DiceUtil::empty();
                int temp = pattern_id;
                for (int i = 0; i < num_dice; i++) {
                    dice[temp % Constants::NUM_FACES]++;
                    temp /= Constants::NUM_FACES;
                }
                patterns[num_dice].push_back(DiceUtil::encode(dice));
            }
        }
    }

    /// n個のサイコロを振ったときのパターン数
    int count(int num_dice) const { return patterns[num_dice].size(); }

    /// n個のサイコロを振ったときの全パターンを取得
    const vector<int>& get(int num_dice) const { return patterns[num_dice]; }
};

// =============================================================================
// キープパターンの事前計算
// =============================================================================

class KeepPatterns {
   public:
    /// keep_patterns[roll_code] = その振り出しに対する全キープパターン
    ankerl::unordered_dense::map<int, vector<int>> patterns;

    void compute(const RollPatterns& roll_patterns) {
        ankerl::unordered_dense::set<int> seen;

        for (int num_dice = 1; num_dice <= Constants::NUM_DICE; num_dice++) {
            for (int roll_code : roll_patterns.get(num_dice)) {
                if (seen.contains(roll_code)) continue;
                seen.insert(roll_code);

                Dice roll = DiceUtil::decode(roll_code);
                vector<int>& keep_list = patterns[roll_code];

                // 6重ループで全キープパターンを列挙
                for (int k0 = 0; k0 <= roll[0]; k0++) {
                    for (int k1 = 0; k1 <= roll[1]; k1++) {
                        for (int k2 = 0; k2 <= roll[2]; k2++) {
                            for (int k3 = 0; k3 <= roll[3]; k3++) {
                                for (int k4 = 0; k4 <= roll[4]; k4++) {
                                    for (int k5 = 0; k5 <= roll[5]; k5++) {
                                        Dice keep = {k0, k1, k2, k3, k4, k5};
                                        keep_list.push_back(DiceUtil::encode(keep));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /// 指定した振り出しに対する全キープパターンを取得
    const vector<int>& get(int roll_code) const { return patterns.at(roll_code); }
};

// =============================================================================
// DP計算エンジン
// =============================================================================

class YachtDPSolver {
   private:
    const RollPatterns& roll_patterns_;
    const KeepPatterns& keep_patterns_;

    /// dp[upper_sum][used_hands] = ターン開始時の期待値
    vector<vector<double>> dp_;

    /// 1ターン内の計算用メモ
    /// key = (stage << 20) | dice_code
    ankerl::unordered_dense::map<int, double> memo_;

    /// 現在計算中の状態
    int current_upper_sum_;
    int current_used_hands_;

   public:
    YachtDPSolver(const RollPatterns& roll_patterns, const KeepPatterns& keep_patterns) : roll_patterns_(roll_patterns), keep_patterns_(keep_patterns) {
        dp_.assign(Constants::UPPER_SUM_MAX, vector<double>(Constants::USED_HANDS_MAX, 0.0));
    }

    /// DPテーブルを計算
    void solve() {
        using namespace Constants;

        int total_hands_states = USED_HANDS_MAX;
        int processed = 0;
        auto start_time = chrono::steady_clock::now();

        // used_hands のビット数が多い順（残り役が少ない順）に計算
        for (int used_hands = total_hands_states - 2; used_hands >= 0; used_hands--) {
            for (int upper_sum = 0; upper_sum < UPPER_SUM_MAX; upper_sum++) {
                dp_[upper_sum][used_hands] = compute_turn_value(upper_sum, used_hands);
            }

            processed++;
            if (processed % 100 == 0) {
                print_progress(processed, total_hands_states, start_time);
            }
        }
    }

    /// 計算結果を取得
    const vector<vector<double>>& get_dp() const { return dp_; }

    /// 初期状態からの期待値を取得
    double get_initial_expected_score() const { return dp_[0][0]; }

   private:
    /// 1ターンの期待値を計算
    double compute_turn_value(int upper_sum, int used_hands) {
        current_upper_sum_ = upper_sum;
        current_used_hands_ = used_hands;
        memo_.clear();

        int initial_state = DiceUtil::encode(DiceUtil::empty());
        return compute_stage_value(0, initial_state);
    }

    /// 指定したステージからの期待値を計算（メモ化再帰）
    /// stage: 0=1回目振り前, 1=2回目振り前, 2=3回目振り前, 3=役選択
    double compute_stage_value(int stage, int state_code) {
        // メモをチェック
        int memo_key = (stage << 20) | state_code;
        auto it = memo_.find(memo_key);
        if (it != memo_.end()) {
            return it->second;
        }

        int num_kept = DiceUtil::count_from_code(state_code);
        int num_to_roll = Constants::NUM_DICE - num_kept;

        double result;

        if (stage == 3) {
            // 役選択ステージ
            result = compute_best_hand_value(state_code);
        } else if (stage == 2) {
            // 最終振り（キープ選択なし）
            result = compute_final_roll_value(state_code, num_to_roll);
        } else {
            // 通常の振り（キープ選択あり）
            result = compute_roll_with_keep_value(stage, state_code, num_to_roll);
        }

        memo_[memo_key] = result;
        return result;
    }

    /// 役選択の最大期待値を計算
    double compute_best_hand_value(int dice_code) {
        Dice dice = DiceUtil::decode(dice_code);
        double best_value = -1e18;

        for (int hand = 0; hand < Constants::NUM_HANDS; hand++) {
            if (is_hand_used(hand)) continue;

            double value = compute_hand_value(hand, dice);
            best_value = max(best_value, value);
        }

        return best_value;
    }

    /// 指定した役を選んだときの期待値
    double compute_hand_value(int hand, const Dice& dice) {
        int points = Scoring::calculate(hand, dice);
        int new_used_hands = current_used_hands_ | (1 << hand);

        if (Hand::is_upper(hand)) {
            int new_upper_sum = min(Constants::UPPER_BONUS_THRESHOLD, current_upper_sum_ + points);
            double bonus = (current_upper_sum_ < Constants::UPPER_BONUS_THRESHOLD && new_upper_sum >= Constants::UPPER_BONUS_THRESHOLD)
                               ? Constants::UPPER_BONUS_POINTS
                               : 0.0;
            return dp_[new_upper_sum][new_used_hands] + points + bonus;
        } else {
            return dp_[current_upper_sum_][new_used_hands] + points;
        }
    }

    /// 最終振りの期待値を計算
    double compute_final_roll_value(int state_code, int num_to_roll) {
        if (num_to_roll == 0) {
            return compute_stage_value(3, state_code);
        }

        Dice current_state = DiceUtil::decode(state_code);
        double total = 0.0;

        for (int roll_code : roll_patterns_.get(num_to_roll)) {
            Dice rolled = DiceUtil::decode(roll_code);
            Dice final_dice = DiceUtil::add(current_state, rolled);
            total += compute_stage_value(3, DiceUtil::encode(final_dice));
        }

        return total / roll_patterns_.count(num_to_roll);
    }

    /// キープ選択ありの振りの期待値を計算
    double compute_roll_with_keep_value(int stage, int state_code, int num_to_roll) {
        if (num_to_roll == 0) {
            return compute_stage_value(stage + 1, state_code);
        }

        Dice current_state = DiceUtil::decode(state_code);
        double total = 0.0;

        for (int roll_code : roll_patterns_.get(num_to_roll)) {
            double best_keep_value = -1e18;

            // 全キープパターンを試して最良を選ぶ
            for (int keep_code : keep_patterns_.get(roll_code)) {
                Dice keep = DiceUtil::decode(keep_code);
                Dice new_state = DiceUtil::add(current_state, keep);
                double value = compute_stage_value(stage + 1, DiceUtil::encode(new_state));
                best_keep_value = max(best_keep_value, value);
            }

            total += best_keep_value;
        }

        return total / roll_patterns_.count(num_to_roll);
    }

    /// 役が使用済みかどうか
    bool is_hand_used(int hand) const { return (current_used_hands_ >> hand) & 1; }

    /// 進捗を表示
    void print_progress(int processed, int total, chrono::steady_clock::time_point start_time) {
        auto now = chrono::steady_clock::now();
        double elapsed = chrono::duration<double>(now - start_time).count();
        double progress = 100.0 * processed / total;
        double eta = (elapsed / processed) * (total - processed);

        cerr << "Progress: " << processed << "/" << total << " (" << fixed << setprecision(1) << progress << "%), " << "ETA: " << static_cast<int>(eta / 60)
             << "m " << static_cast<int>(fmod(eta, 60)) << "s" << endl;
    }
};

// =============================================================================
// ファイル出力
// =============================================================================

class OutputWriter {
   public:
    static void write_dp_table(const string& filename, const vector<vector<double>>& dp) {
        ofstream ofs(filename);
        ofs << fixed << setprecision(15);

        write_header(ofs);
        write_table(ofs, dp);
        write_accessor(ofs);

        ofs.close();
    }

   private:
    static void write_header(ofstream& ofs) {
        ofs << R"(/**
 * ヨット DP テーブル（自動生成）
 * 
 * dp_table[upper_sum][used_hands] = その状態からの期待得点
 * 
 * upper_sum: 0-63 (上段合計、63でキャップ)
 * used_hands: 12ビットのビットマスク
 *   bit 0: Ones        bit 1: Twos         bit 2: Threes
 *   bit 3: Fours       bit 4: Fives        bit 5: Sixes
 *   bit 6: Full House  bit 7: Four of a Kind
 *   bit 8: Little Straight (4連続)
 *   bit 9: Big Straight (5連続)
 *   bit 10: Choice     bit 11: Yacht
 * 
 * 上段ボーナス: upper_sum >= 63 のとき +35点
 */

#pragma once
#include <vector>

namespace yacht {

)";
    }

    static void write_table(ofstream& ofs, const vector<vector<double>>& dp) {
        ofs << "const std::vector<std::vector<double>> dp_table = {\n";

        for (size_t x = 0; x < dp.size(); x++) {
            ofs << "    {";
            for (size_t hand = 0; hand < dp[x].size(); hand++) {
                if (hand > 0) ofs << ", ";
                ofs << dp[x][hand];
            }
            ofs << "}";
            if (x < dp.size() - 1) ofs << ",";
            ofs << "\n";
        }

        ofs << "};\n\n";
    }

    static void write_accessor(ofstream& ofs) {
        ofs << R"(/// 指定した状態からの期待得点を取得
inline double get_expected_score(int upper_sum, int used_hands) {
    return dp_table[upper_sum][used_hands];
}

/// 初期状態からの期待得点を取得
inline double get_initial_expected_score() {
    return dp_table[0][0];
}

} // namespace yacht
)";
    }
};

// =============================================================================
// メイン
// =============================================================================

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cerr << "=== ヨット期待値計算 ===" << endl;

    // 振り出しパターンの事前計算
    cerr << "振り出しパターンを計算中..." << endl;
    RollPatterns roll_patterns;

    // キープパターンの事前計算
    cerr << "キープパターンを計算中..." << endl;
    KeepPatterns keep_patterns;
    keep_patterns.compute(roll_patterns);
    cerr << "キープパターン数: " << keep_patterns.patterns.size() << endl;

    // DP計算
    cerr << "DP計算を開始..." << endl;
    auto start_time = chrono::steady_clock::now();

    YachtDPSolver solver(roll_patterns, keep_patterns);
    solver.solve();

    auto end_time = chrono::steady_clock::now();
    double total_time = chrono::duration<double>(end_time - start_time).count();

    // 結果出力
    cerr << "\n=== 計算完了 ===" << endl;
    cerr << "計算時間: " << total_time << " 秒" << endl;
    cerr << "初期状態からの期待得点: " << solver.get_initial_expected_score() << endl;

    // ファイル出力
    const string output_filename = "yacht_dp_table.hpp";
    cerr << "ファイル出力: " << output_filename << endl;
    OutputWriter::write_dp_table(output_filename, solver.get_dp());

    // 標準出力にも結果を表示
    cout << fixed << setprecision(15);
    cout << "Expected score: " << solver.get_initial_expected_score() << endl;

    return 0;
}