#include <bits/stdc++.h>

using namespace std;
using ll = long long;
using uint = unsigned int;


constexpr int N = 5;
constexpr int NN = N*N;
int A[NN];
// int dx[4] = {1,-1,0,0};
// int dy[4] = {0,0,1,-1};
int POW2[NN+1];
int W[NN];
int SUM[N][N+1];
constexpr int P = 100;
constexpr int MAX_DEPTH = 90;

// 入力受け取り
void input() {
    int n;
    cin >> n;

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) cin >> A[i*N+j];
    }
}


void initPOW(int p, bool flag) {
    POW2[0] = 1;
    for (int i = 0; i < NN; ++i) {
        POW2[i+1] = POW2[i]*p;
    }

    for (int i = 0; i < NN; ++i) {
        // W[i] = POW2[N-i%N-1];
        W[i] = P*(N-i%N);
        // if (i%N == 0) W[i] += P;
    }
    // int tmp[NN];
    // for (int i = 0; i < NN; ++i) {
    //     tmp[A[i]] = i%N;
    // }
    // for (int i = 0; i < N; ++i) {
    //     for (int j = 0; j < N; ++j) {
    //         for (int k = j+1; k < N; ++k) {
    //             if (tmp[i*N+j] <= tmp[i*N+k]) {
    //                 // W[i*N+k] -= P/10;
    //                 W[i*N+j] -= P*(N-j)/10;
    //             }
    //             else {
    //                 W[i*N+k] -= P*(N-k)/10;
    //             }
    //         }
    //     }
    // }

    for (int i = 0; i < N; ++i) {
        SUM[i][N] = 0;
        for (int j = N-1; j >= 0; --j) {
            // SUM[i][j] = SUM[i][j+1] + POW2[N-A[i*N+j]%N];
            SUM[i][j] = SUM[i][j+1] + W[A[i*N+j]];
        }
    }
}

// ゾブリストハッシュ
uint H_AREA[NN*(NN+1)];
uint H_ARM[N*NN];
uint H_OUT[N*(N+1)];
uint H_HAVE[N*(NN+1)];
uint H_HAVE_AREA[NN*(NN+1)];

void init_zobrist() {
    mt19937 mt_random(0);
    for (int i = 0; i < NN; ++i) {
        for (int j = 0; j <= NN; ++j) {
            H_AREA[i*(NN+1)+j] = mt_random();
            H_HAVE_AREA[i*(NN+1)+j] = mt_random();
        }
        for (int j = 0; j < N; ++j) {
            H_ARM[j*NN+i] = mt_random();
        }
    }

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j <= N; ++j) {
            H_OUT[i*(N+1)+j] = mt_random();
        }
    }

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j <= NN; ++j) {
            H_HAVE[i*(NN+1)+j] = mt_random();
        }
    }
}


// 行動履歴用BIT
// constexpr ull BIT_L = 0b001;
// constexpr ull BIT_R = 0b110;
// constexpr ull BIT_U = 0b010;
// constexpr ull BIT_D = 0b101;
// constexpr ull BIT_P = 0b100;
// constexpr ull BIT_Q = 0b011;
// constexpr ull BIT_DOT = 0b111;

// ull action_to_bit(const char op) {
//     switch (op) {
//     case 'L':
//         return BIT_L;
//     case 'R':
//         return BIT_R;
//     case 'U':
//         return BIT_U;
//     case 'D':
//         return BIT_D;
//     case 'P':
//         return BIT_P;
//     case 'Q':
//         return BIT_Q;
//     case '.':
//         return BIT_DOT;
//     default:
//         break;
//     }

//     return 0;
// }

// 行動履歴用BIT(2bitで表す)
constexpr int BIT_LR = 0b00;
constexpr int BIT_U = 0b01;
constexpr int BIT_D = 0b10;
constexpr int BIT_PQ = 0b11;

int action_to_bit2(const char op) {
    switch (op) {
    case 'H':
        return BIT_LR;
    // case 'L':
    //     return BIT_LR;
    // case 'R':
    //     return BIT_LR;
    case 'U':
        return BIT_U;
    case 'D':
        return BIT_D;
    // case 'P':
    //     return BIT_PQ;
    // case 'Q':
    //     return BIT_PQ;
    case 'Z':
        return BIT_PQ;
    case '.':
        return -1;
    }

    return 0;
}


// BIT
constexpr int BIT_MASK = 0b11111;
constexpr int BIT_SHIFT = 5;

// ll time1 = 0;
// ll time2 = 0;
// ll time3 = 0;
// ll time4 = 0;
// using std::chrono::duration_cast;
// using std::chrono::microseconds;

struct State {
    // 変数
    int now_Area[N];    // 現在の盤面の状態
    int now_pos;        // それぞれのクレーンに位置
    int bef_pos;        // 直前のクレーンの位置
    int have;           // 所持しているコンテナの番号(所持していない場合はNN)
    int cnt_in;         // コンテナの何番目まで出したか
    int cnt_out;        // 何番目のコンテナを搬出したか
    int now_score;      // 現在のスコア
    int turn;           // 現在のターン数
    uint hash;           // ハッシュ
    int one_catched;    // i番目のコンテナを1回以上出したか
    int all_bit_pos;    // それぞれのクレーンの位置
    int all_bit_bef_pos;    // 直前のクレーンの位置
    int carry_out;      // 運び出したコンテナの個数
    bitset<2*MAX_DEPTH*N> move_history;   // 行動履歴
    int depth;          // 探索した深さ
    int now_container_dis[N];   // それぞれのコンテナの搬出口までの距離


    // 初期化とか
    State() {

    }

    State(const State& state) {
        for (int i = 0; i < N; ++i) now_Area[i] = state.now_Area[i];
        now_pos = state.now_pos;
        bef_pos = state.bef_pos;
        have = state.have;
        cnt_in = state.cnt_in;
        cnt_out = state.cnt_out;

        now_score = state.now_score;
        turn = state.turn;
        hash = state.hash;
        one_catched = state.one_catched;
        all_bit_pos = state.all_bit_pos;
        all_bit_bef_pos = state.all_bit_bef_pos;
        carry_out = state.carry_out;
        move_history = state.move_history;
        depth = state.depth;
        for (int i = 0; i < N; ++i) now_container_dis[i] = state.now_container_dis[i];
    }

    void init() {
        for (int i = 0; i < N; ++i) now_Area[i] = 0;
        for (int i = 0; i < NN; ++i) set_now_Area(i,NN);
        now_pos = 0;
        for (int i = 0; i < N; ++i) set_now_pos(i,i*N);
        bef_pos = 0;
        for (int i = 0; i < N; ++i) set_bef_pos(i,i*N);
        have = 0;
        for (int i = 0; i < N; ++i) set_have(i,NN);
        cnt_in = 0;
        for (int i = 0; i < N; ++i) add_cnt_in(i);
        cnt_out = 0;
        for (int i = 0; i < N; ++i) set_now_Area(i*N,A[i*N]);
        now_score = 0;
        hash = 0;
        one_catched = 0;
        all_bit_pos = 0;
        all_bit_bef_pos = 0;
        for (int i = 0; i < N; ++i) all_bit_bef_pos |= 1<<get_now_pos(i);
        carry_out = 0;
        move_history.reset();
        depth = 0;

        for (int i = 0; i < N; ++i) now_container_dis[i] = 0;
        for (int i = 0; i < NN; ++i) {
            set_now_container_dis(A[i],abs(i/N-A[i]/N)+N);
        }

        for (int i = 0; i < NN; ++i) {
            if (i%N) hash ^= H_AREA[i*(NN+1)];
            else hash ^= H_AREA[i*(NN+1) + get_now_Area(i) + 1];
        }
        for (int i = 0; i < N; ++i) {
            hash ^= H_ARM[(i>0)*NN+get_now_pos(i)];
        }
        // hash ^= H_ARM[get_now_pos(0)];
        for (int i = 0; i < N; ++i) {
            hash ^= H_OUT[i*(N+1)];
        }
        // for (int i = 0; i < N; ++i) {
        //     hash ^= H_HAVE[i*(NN+1)];
        // }
        for (int i = 0; i < NN; ++i) {
            hash ^= H_HAVE_AREA[i*(NN+1)];
        }

        turn = 0;
    }

    // 一手進めた場合のスコアとハッシュ値を返す。更新はしない
    pair<int,uint> try_move(const char op) const {
        int ret = now_score;
        uint ret_hash = hash;

        // スコアの計算
        int pos = get_now_pos(turn);
        int hv = get_have(turn);

        switch (op) {
        // コンテナを所持している場合スコアが減少する
        case 'H':
            // L
            if (hv == NN) {
                if (pos%N != 0) {
                    if (turn == 0) ret += 2*P;
                    else ret += P;
                }
                else {
                    // if (depth >= N) ret -= POW2[N]*P;
                    // else ret -= 2*P;
                    ret -= POW2[2]*P;
                }
            }
            // R
            else {
                // ret += W[hv];
                if (pos%N != N-2 || hv%N == get_cnt_out(hv/N)) ret += W[hv];
                else ret -= 2*POW2[hv%N]*P;

                // ゴールまでの距離から転倒数を計算する
                int tmp = now_container_dis[hv/N];
                int tmp_dis = get_now_container_dis(hv);
                for (int i = 0; i < hv%N; ++i) {
                    if ((tmp&BIT_MASK) == tmp_dis) ret -= 2*P;
                    tmp >>= BIT_SHIFT;
                }
                tmp >>= BIT_SHIFT;
                for (int i = hv%N+1; i < N; ++i) {
                    if ((tmp&BIT_MASK) == tmp_dis-1) ret += P;
                    tmp >>= BIT_SHIFT;
                }
            }

            break;
        // コンテナを所持している場合、ゴールに近づくかどうかで変わる
        case 'U':
            if (hv != NN) {
                if (hv/N >= pos/N) {
                    ret -= 2*W[hv];
                    // ゴールまでの距離から転倒数を計算する
                    int tmp = now_container_dis[hv/N];
                    int tmp_dis = get_now_container_dis(hv);
                    for (int i = 0; i < hv%N; ++i) {
                        if ((tmp&BIT_MASK) == tmp_dis+1) ret += P;
                        tmp >>= BIT_SHIFT;
                    }
                    tmp >>= BIT_SHIFT;
                    for (int i = hv%N+1; i < N; ++i) {
                        if ((tmp&BIT_MASK) == tmp_dis) ret -= 2*P;
                        tmp >>= BIT_SHIFT;
                    }
                }
                else {
                    ret += W[hv];
                    // ゴールまでの距離から転倒数を計算する
                    int tmp = now_container_dis[hv/N];
                    int tmp_dis = get_now_container_dis(hv);
                    for (int i = 0; i < hv%N; ++i) {
                        if ((tmp&BIT_MASK) == tmp_dis) ret -= 2*P;
                        tmp >>= BIT_SHIFT;
                    }
                    tmp >>= BIT_SHIFT;
                    for (int i = hv%N+1; i < N; ++i) {
                        if ((tmp&BIT_MASK) == tmp_dis-1) ret += P;
                        tmp >>= BIT_SHIFT;
                    }
                }
            }
            else {
                ret -= P;
            };
            break;
        // コンテナを所持している場合、ゴールに近づくかどうかで変わる
        case 'D':
            if (hv != NN) {
                if (hv/N <= pos/N) {
                    ret -= 2*W[hv];
                    // ゴールまでの距離から転倒数を計算する
                    int tmp = now_container_dis[hv/N];
                    int tmp_dis = get_now_container_dis(hv);
                    for (int i = 0; i < hv%N; ++i) {
                        if ((tmp&BIT_MASK) == tmp_dis+1) ret += P;
                        tmp >>= BIT_SHIFT;
                    }
                    tmp >>= BIT_SHIFT;
                    for (int i = hv%N+1; i < N; ++i) {
                        if ((tmp&BIT_MASK) == tmp_dis) ret -= 2*P;
                        tmp >>= BIT_SHIFT;
                    }
                }
                else {
                    ret += W[hv];
                    // ゴールまでの距離から転倒数を計算する
                    int tmp = now_container_dis[hv/N];
                    int tmp_dis = get_now_container_dis(hv);
                    for (int i = 0; i < hv%N; ++i) {
                        if ((tmp&BIT_MASK) == tmp_dis) ret -= 2*P;
                        tmp >>= BIT_SHIFT;
                    }
                    tmp >>= BIT_SHIFT;
                    for (int i = hv%N+1; i < N; ++i) {
                        if ((tmp&BIT_MASK) == tmp_dis-1) ret += P;
                        tmp >>= BIT_SHIFT;
                    }
                }
            }
            else {
                ret -= P;
            }
            break;
        // 増加(後で条件を変えてみる)
        case 'Z':
            // P
            if (hv == NN) {
                if ((one_catched&(1<<get_now_Area(pos))) == 0) {
                    ret += SUM[pos/N][get_cnt_in(pos/N)-1];// - POW2[(get_now_Area(pos)%N) - get_cnt_out(get_now_Area(pos)/N)];
                }
                else ret += P;
            }
            // Q
            else {
                if (pos%N == N-1) {
                    ret += W[hv];
                    if (carry_out == NN-1) ret += INT_MAX/2;
                }
                // else if (((pos/N)&1)) {
                //     ret -= 2*POW2[hv%N]*(N-pos%N)*P;
                //     // if (get_now_Area(pos+1) == NN) ret -= 2*POW2[hv%N]*(N-pos%N)*P;
                //     // else ret -= (N-pos%N)*P;
                // }
                else if (pos/N == N/2) ret -= 2*POW2[hv%N]*(N-pos%N)*P;
                else ret -= (N-pos%N)*(POW2[hv%N-get_cnt_out(hv/N)]-1);
                // else ret -= POW2[N-pos%N]*P/POW2[2];
                // else ret += POW2[have[turn] - cnt_out[have[turn]/N]];

                if (pos%N != N-1) {
                    if (turn == 0) ret -= W[hv];
                    else ret -= W[hv];
                    // ret -= W[hv];
                }

                if (pos%N == 1) {
                    if (get_cnt_in(pos/N) != N) {
                        bool ng = true;
                        if (pos/N == 0) {
                            if (get_now_Area(pos+1) != NN && get_cnt_in(pos/N+1) == N && (get_now_Area(pos+N) == NN || get_cnt_in(pos/N+2) == N)) ng = false;
                        }
                        else if (pos/N == 1 || pos/N == N-2) {
                            if (get_cnt_in(pos/N-1) == N && (get_now_Area(pos-N) == NN)) ng = false;
                            if (get_cnt_in(pos/N+1) == N && (get_now_Area(pos+N) == NN)) ng = false;
                        }
                        else if (pos/N == N-1) {
                            if (get_now_Area(pos+1) != NN && get_cnt_in(pos/N-1) == N && (get_now_Area(pos-N) == NN || get_cnt_in(pos/N-2) == N)) ng = false;
                        }
                        

                        if (ng) ret -= SUM[pos/N][get_cnt_in(pos/N)-1];
                    }
                }
            }
            
            break;
        
        default:
            break;
        }

        // if (turn == N-1) {
        //     for (int i = 0; i < N; ++i) {
        //         ret -= SUM[i][get_cnt_in(i)]/P;
        //     }
        // }

        // ハッシュの計算
        switch (op) {
        case 'H':
            // L
            if (hv == NN) {
                if (pos%N != 0) {
                    ret_hash ^= H_ARM[(turn>0)*NN+pos];
                    ret_hash ^= H_ARM[(turn>0)*NN+pos-1];
                }
            }
            // R
            else {
                ret_hash ^= H_ARM[(turn>0)*NN+pos];
                ret_hash ^= H_ARM[(turn>0)*NN+pos+1];                

                ret_hash ^= H_HAVE_AREA[pos*(NN+1)+hv+1];
                ret_hash ^= H_HAVE_AREA[pos*(NN+1)];
                ret_hash ^= H_HAVE_AREA[(pos+1)*(NN+1)+hv+1];
                ret_hash ^= H_HAVE_AREA[(pos+1)*(NN+1)];
            }
            
            break;
        case 'U':
            ret_hash ^= H_ARM[(turn>0)*NN+pos];
            ret_hash ^= H_ARM[(turn>0)*NN+pos-N];
            
            if (hv != NN) {
                ret_hash ^= H_HAVE_AREA[pos*(NN+1)+hv+1];
                ret_hash ^= H_HAVE_AREA[pos*(NN+1)];
                ret_hash ^= H_HAVE_AREA[(pos-N)*(NN+1)+hv+1];
                ret_hash ^= H_HAVE_AREA[(pos-N)*(NN+1)];
            }
            break;
        case 'D':
            ret_hash ^= H_ARM[(turn>0)*NN+pos];
            ret_hash ^= H_ARM[(turn>0)*NN+pos+N];

            if (hv != NN) {
                ret_hash ^= H_HAVE_AREA[pos*(NN+1)+hv+1];
                ret_hash ^= H_HAVE_AREA[pos*(NN+1)];
                ret_hash ^= H_HAVE_AREA[(pos+N)*(NN+1)+hv+1];
                ret_hash ^= H_HAVE_AREA[(pos+N)*(NN+1)];
            }
            break;
        case 'Z':
            // P
            if (hv == NN) {
                ret_hash ^= H_AREA[pos*(NN+1)];
                ret_hash ^= H_AREA[pos*(NN+1)+get_now_Area(pos)+1];

                // ret_hash ^= H_HAVE[turn*(NN+1)+get_now_Area(pos)+1];
                // ret_hash ^= H_HAVE[turn*(NN+1)];
                ret_hash ^= H_HAVE_AREA[pos*(NN+1)+get_now_Area(pos)+1];
                ret_hash ^= H_HAVE_AREA[pos*(NN+1)];
            }
            // Q
            else {
                if (pos%N == N-1) {
                    ret_hash ^= H_OUT[(pos/N)*(N+1)+get_cnt_out(pos/N)];
                    ret_hash ^= H_OUT[(pos/N)*(N+1)+get_cnt_out(pos/N)+1];
                }
                else {
                    ret_hash ^= H_AREA[pos*(NN+1)+hv+1];
                    ret_hash ^= H_AREA[pos*(NN+1)];
                }
                // ret_hash ^= H_HAVE[turn*(NN+1)];
                // ret_hash ^= H_HAVE[turn*(NN+1)+hv+1];
                ret_hash ^= H_HAVE_AREA[pos*(NN+1)+hv+1];
                ret_hash ^= H_HAVE_AREA[pos*(NN+1)];
            }
            

            break;
        
        default:
            break;
        }

        return make_pair(ret,ret_hash);
    }


    // 更新する
    void apply_move(const char op) {
        all_bit_bef_pos ^= 1<<get_now_pos(turn);
        auto [ret,ret_hash] = try_move(op);

        now_score = ret;
        hash = ret_hash;


        int op_bit = action_to_bit2(op);
        for (int i = 0; i < 2; ++i) {
            if (op_bit&(1<<i)) move_history.set(i+2*depth);
        }

        set_bef_pos(turn,get_now_pos(turn));
        int pos = get_now_pos(turn);
        int hv = get_have(turn);

        switch (op) {
        case 'H':
            if (hv == NN) {
                if (pos%N != 0) --pos;
            }
            else {
                ++pos;
                set_now_container_dis(hv,get_now_container_dis(hv)-1);
            }
            break;
        case 'U':
            if (hv != NN) {
                if (hv/N >= pos/N) set_now_container_dis(hv,get_now_container_dis(hv)+1);
                else set_now_container_dis(hv,get_now_container_dis(hv)-1);
            }
            pos -= N;
            
            break;
        case 'D':
            if (hv != NN) {
                if (hv/N <= pos/N) set_now_container_dis(hv,get_now_container_dis(hv)+1);
                else set_now_container_dis(hv,get_now_container_dis(hv)-1);
            }
            pos += N;
            break;
        case 'Z':
            if (hv == NN) {
                set_have(turn, get_now_Area(pos));
                set_now_Area(pos,NN);
                one_catched |= 1<<get_have(turn);
            }
            else {
                if (pos%N == N-1) {
                    add_cnt_out(pos/N);
                    ++carry_out;
                }
                else {
                    set_now_Area(pos,get_have(turn));
                }
                set_have(turn,NN);
            }
            
            
            break;
        
        default:
            break;
        }

        set_now_pos(turn,pos);
        all_bit_pos |= 1<<pos;

        ++turn;
        ++depth;

        if (turn == N) {
            turn = 0;
            int not_c = 0;
            all_bit_bef_pos = all_bit_pos;
            all_bit_pos = 0;

            for (int i = 0; i < N; ++i) {
                if (get_now_pos(i)%N == 0 && get_have(i) != NN) not_c |= 1<<(get_now_pos(i)/N);
            }
            for (int i = 0; i < N; ++i) {
                if (get_cnt_in(i) < N && get_now_Area(i*N) == NN && !(not_c&(1<<i))) {
                    set_now_Area(i*N,A[i*N + get_cnt_in(i)]);
                    add_cnt_in(i);
                    hash ^= H_AREA[(i*N)*(NN+1)];
                    hash ^= H_AREA[(i*N)*(NN+1)+get_now_Area(i*N)+1];
                }
            }
        }
    }


    // 現在のクレーンiに対して、j<iのクレーンに衝突しているか判定する(-1なら衝突しておらず、そうでないとき衝突したクレーンの番号)
    inline int overlap() const {
        int pos = get_now_pos(turn);
        if (all_bit_pos&(1<<pos)) {
            for (int i = 0; i < turn; ++i) {
                if (get_now_pos(i) == pos) return i;
            }
            return -1;
        }
        else return -1;
    }

    inline char get_before_operation(int d) const {
        if (d < 0) return '.';
        int tmp = 0;
        for (int i = 1; i >= 0; --i) {
            tmp <<= 1;
            if (move_history[i+2*d]) tmp |= 1;
        }

        switch (tmp) {
        case BIT_LR:
            return 'H';
            break;
        
        case BIT_U:
            return 'U';
            break;

        case BIT_D:
            return 'D';
            break;

        case BIT_PQ:
            return 'Z';
            break;
        
        default:
            return '.';
            break;
        }
    }

    // 有効な手かどうか
    bool is_valid(const char op, const int ov) const {
        int pos = get_now_pos(turn);
        int hv = get_have(turn);

        switch (op) {
        case 'H':
            // L
            if (hv == NN) {
                // if (pos%N == 0) return false;   // 範囲外
                if (pos%N == 0) return ov == -1 && turn != 0;    // 例外、移動しない
                
                if (all_bit_pos&(1<<(pos-1))) return false;
                if (ov != -1) {
                    if (get_bef_pos(ov) == pos-1) return false;
                }

                // 小クレーンのとき、地面のコンテナにぶつかる
                if (hv != NN && turn != 0 && get_now_Area(pos-1) != NN) return false;
            }
            // R
            else {
                if (pos%N == N-1) return false;   // 範囲外

                if (all_bit_pos&(1<<(pos+1))) return false;
                if (ov != -1) {
                    if (get_bef_pos(ov) == pos+1) return false;
                }
                
                // 小クレーンのとき、地面のコンテナにぶつかる
                if (hv != NN && turn != 0 && get_now_Area(pos+1) != NN) return false;
            }
            break;

        case 'U':
            if (pos/N == 0) return false;   // 範囲外

            // if (turn == 0 && hv != NN && hv/N >= pos/N) return false;

            if (all_bit_pos&(1<<(pos-N))) return false;
            if (ov != -1) {
                if (get_bef_pos(ov) == pos-N) return false;
            }

            // 小クレーンのとき、地面のコンテナにぶつかる
            if (hv != NN && turn != 0 && get_now_Area(pos-N) != NN) return false;
            
            // if (hv == NN && bef_op == 'D') return false;
            // if (hv == NN && (bef_op == 'U' || bef_op == 'D') && (bef2_op == 'U' || bef2_op == 'D')) return false;
            // if (hv != NN && bef_op == 'D' && bef2_op == 'U') return false;

            break;
        case 'D':
            if (pos/N == N-1) return false;   // 範囲外
            
            // if (turn == 0 && hv != NN && hv/N <= pos/N) return false;

            if (all_bit_pos&(1<<(pos+N))) return false;
            if (ov != -1) {
                if (get_bef_pos(ov) == pos+N) return false;
            }

            // 小クレーンのとき、地面のコンテナにぶつかる
            if (hv != NN && turn != 0 && get_now_Area(pos+N) != NN) return false;

            // if (hv == NN && bef_op == 'U') return false;
            // if (hv == NN && (bef_op == 'U' || bef_op == 'D') && (bef2_op == 'U' || bef2_op == 'D')) return false;
            // if (hv != NN && bef_op == 'U' && bef2_op == 'D') return false;

            break;
        case 'Z':
            if (ov != -1) return false;
            // if (bef_op == 'Z' && bef2_op == 'Z') return false;
            // コンテナを掴めるか
            // P
            if (hv == NN) {
                if (get_now_Area(pos) == NN) return false;

                if (turn != 0) {
                    bool ng = true;
                    if ((pos%N > 0 && (get_now_Area(pos-1) == NN) || (all_bit_bef_pos&(1<<(pos-1))))) ng = false;
                    if (ng && pos%N < N-1 && (get_now_Area(pos+1) == NN || (all_bit_bef_pos&(1<<(pos+1))))) ng = false;
                    if (ng && pos/N > 0 && (get_now_Area(pos-N) == NN || (all_bit_bef_pos&(1<<(pos-N))))) ng = false;
                    if (ng && pos/N < N-1 && (get_now_Area(pos+N) == NN || (all_bit_bef_pos&(1<<(pos+N))))) ng = false;
                    if (ng) return false;
                }
                else {
                    // if (carry_out < N*N/2 && pos%N != 0) return false; 
                }
            }
            // Q
            else {
                if (get_now_Area(pos) != NN) return false;

                if (pos/N == N/2 && 0 < pos%N && pos%N < N-1) return false;  

                if (pos%N == 0 && get_cnt_in(pos/N) != N) return false;

                if (pos%N == N-1) {
                    if (hv/N != pos/N) return false;
                    if (get_cnt_out(hv/N) != hv%N) return false;
                }
                else {
                    if (get_cnt_out(hv/N) == hv%N) return false;
                }
            }
            
            break;
        
        default:
            return false;
            break;
        }

        return true;
    }


    inline bool is_done() const {
        // return cnt_out == 0b0010100101001010010100101;
        return carry_out == NN;
    }

    inline int get_now_Area(int pos) const {return (now_Area[pos/N]>>(BIT_SHIFT*(pos%N)))&BIT_MASK;}
    inline void set_now_Area(int pos, int item) {now_Area[pos/N] &= (~(BIT_MASK<<(BIT_SHIFT*(pos%N)))), now_Area[pos/N] |= (item<<(BIT_SHIFT*(pos%N)));}
    inline int get_now_pos(int i) const {return (now_pos>>(BIT_SHIFT*i))&BIT_MASK;}
    inline void set_now_pos(int i, int pos) {now_pos &= (~(BIT_MASK<<(BIT_SHIFT*i))), now_pos |= pos<<(BIT_SHIFT*i);}
    inline int get_bef_pos(int i) const {return (bef_pos>>(BIT_SHIFT*i))&BIT_MASK;}
    inline void set_bef_pos(int i, int pos) {bef_pos &= (~(BIT_MASK<<(BIT_SHIFT*i))), bef_pos |= pos<<(BIT_SHIFT*i);}
    inline int get_have(int i) const {return (have>>(BIT_SHIFT*i))&BIT_MASK;}
    inline void set_have(int i, int item) {have &= (~(BIT_MASK<<(BIT_SHIFT*i))), have |= item<<(BIT_SHIFT*i);}
    inline int get_cnt_in(int i) const {return (cnt_in>>(BIT_SHIFT*i))&BIT_MASK;}
    inline void add_cnt_in(int i) {cnt_in += 1<<(BIT_SHIFT*i);}
    inline int get_cnt_out(int i) const {return (cnt_out>>(BIT_SHIFT*i))&BIT_MASK;}
    inline void add_cnt_out(int i) {cnt_out += 1<<(BIT_SHIFT*i);}
    inline int get_now_container_dis(int item) const {return (now_container_dis[item/N]>>(BIT_SHIFT*(item%N)))&BIT_MASK;}
    inline void set_now_container_dis(int item, int dis) {now_container_dis[item/N] &= (~(BIT_MASK<<(BIT_SHIFT*(item%N)))), now_container_dis[item/N] |= (dis<<(BIT_SHIFT*(item%N)));}

};

// struct History {
//     char op;
//     shared_ptr<History> parent;
//     History(const char op, shared_ptr<History>& parent) :op(op), parent(parent) {}
// };

// struct Stack {
//     shared_ptr<History> head;

//     char top() { return head->op;}
//     Stack push(const char& op) { return Stack({make_shared<History>(op,head)}); }
//     Stack pop() { return Stack({head->parent}); }
// };

struct Node {
    State state;
    // Stack move_history;

    Node() {}

    Node(State& state) : state(state) {

    }

    pair<int,uint> calculate(const char op) const {
        return state.try_move(op);
    }

    void advance(const char op) {
        state.apply_move(op);
    }

    int overlap() const {
        return state.overlap();
    }

    char get_before_operation(int d) const {
        return state.get_before_operation(d);
    }

    bool is_valid(const char op, const int ov) const {
        return state.is_valid(op, ov);
    }

    bool is_done() const {
        return state.is_done();
    }
};

struct TemporaryNode {
    int score;
    uint hash;
    int node_index;
    char op;
    
    // TemporaryNode(int score, ull hash, int node_index, char op) :
    //     score(score), hash(hash), node_index(node_index), op(op) {}
};


// constexpr int mod[] = {10000001,1000033};//,1000037};//,1000039,1000081};
// constexpr int mod_size = 1;
constexpr int mod = 10000019;
// constexpr int mod = 20000003;

Node beamSearch(State init_state, const int max_depth, const int beam_width, bool flag) {
    vector<Node> nodes, next_nodes;
    // Node nodes[2][beam_width];
    // nodes.reserve(beam_width), next_nodes.reserve(beam_width);
    nodes.resize(beam_width), next_nodes.resize(beam_width);
    int nodes_size = 0;
    // nodes.emplace_back(init_state);
    // nodes.back().move_history = Stack{nullptr};
    nodes[nodes_size].state = init_state;
    // nodes[nodes_size].move_history = Stack{nullptr};
    // nodes[0][nodes_size].state = init_state;
    // nodes[0][nodes_size].move_history = Stack{nullptr};
    ++nodes_size;


    vector<TemporaryNode> temp_nodes;
    // unordered_set<ull> fields;

    // char all_operations[] = {'L','R','U','D','P','Q','.'};
    // if (flag) all_operations[6] = '?';
    // char all_operations[] = {'L','R','U','D','P','Q'};
    char all_operations[] = {'H','U','D','Z'};  // H:左右移動、U:上、D:下、Z:物を掴むか掴まないか
    // temp_nodes.reserve(beam_width*4);
    temp_nodes.resize(beam_width*4);

    // vector<bitset<mod[mod_size-1]>> fields(mod_size);
    // bitset<mod[0]> fields0;
    // bitset<mod[1]> fields1;
    bitset<mod> fields;

    for (int turn = 1; turn <= max_depth; ++turn) {
        // cerr << turn << ' ' << nodes_size << endl;

        // temp_nodes.clear();
        // for (int i = 0; i < mod_size; ++i) fields[i].reset();
        fields.reset();
        // fields1.reset();
        // fields.clear();

        // auto s1 = std::chrono::high_resolution_clock::now();

        int node_size = 0;
        for (int i = 0; i < nodes_size; ++i) {
            int ov = nodes[i].overlap();
            // char bef_op = nodes[i].get_before_operation(turn-1-N);
            // char bef2_op = nodes[i].get_before_operation(turn-1-2*N);
            for (char op : all_operations) {
                // if (!nodes[i].is_valid(op,ov,bef_op,bef2_op)) continue;
                if (!nodes[i].is_valid(op,ov)) continue;
                
                auto&& [next_score,hash] = nodes[i].calculate(op);
                // if (fields0[hash%mod[0]] && fields1[hash%mod[1]]) continue;
                if (fields[hash%mod]) continue;
                // if (fields.count(hash)) continue;
                // temp_nodes.emplace_back(next_score, hash, i, op);
                temp_nodes[node_size] = {next_score, hash, i, op};
                ++node_size;
                // for (int i = 0; i < mod_size; ++i) fields[i].set(hash%mod[i]);
                // fields0.set(hash%mod[0]);
                // fields1.set(hash%mod[1]);
                fields.set(hash%mod);
                // fields.insert(hash);
            }
        }

        
        // auto s2 = std::chrono::high_resolution_clock::now();

        // int node_size = temp_nodes.size();
        // int node_size = temp_node_size;
        // cerr << node_size << endl;

        if (node_size > beam_width) {
            nth_element(temp_nodes.begin(),temp_nodes.begin() + beam_width, temp_nodes.begin()+node_size,
                [](TemporaryNode& n1, TemporaryNode& n2) {
                    if (n1.score == n2.score) n1.hash > n2.hash;
                    return n1.score > n2.score;
                });
            // sort(temp_nodes.begin(),temp_nodes.end(),
            //     [](TemporaryNode& n1, TemporaryNode& n2) {
            //         if (n1.score == n2.score) n1.hash < n2.hash;
            //         return n1.score > n2.score;
            //     });
        }

        
        // auto s3 = std::chrono::high_resolution_clock::now();

        for (int i = 0, s = min(beam_width, node_size); i < s; ++i) {
            int index = temp_nodes[i].node_index;
            // next_nodes.emplace_back(nodes[index]);
            // next_nodes.back().advance(temp_nodes[i].op);

            // next_nodes.back().move_history = nodes[index].move_history.push(temp_nodes[i].op);
            next_nodes[i] = nodes[index];
            
            next_nodes[i].advance(temp_nodes[i].op);
            // next_nodes[next_nodes_size].move_history = nodes[index].move_history.push(temp_nodes[i].op);
        }


        swap(nodes, next_nodes);
        nodes_size = min(beam_width, node_size);

        // auto s4 = std::chrono::high_resolution_clock::now();
        // next_nodes.clear();


        // 処理が終了したやつがあればそれを取り出す
        if (turn%N == 0 && turn >= 40*N) {
            for (int i = 0, s = min(beam_width, node_size); i < s; ++i) {
                if (nodes[i].is_done()) {
                    // cerr << time1 << ' ' << time2 << ' ' << time3 << ' ' << time4 << endl;
                    return nodes[i];
                }
            }
        }

        // auto s5 = std::chrono::high_resolution_clock::now();

        // time1 += duration_cast<microseconds>(s2-s1).count();
        // time2 += duration_cast<microseconds>(s3-s2).count();
        // time3 += duration_cast<microseconds>(s4-s3).count();
        // time4 += duration_cast<microseconds>(s5-s4).count();
    }

    int arg_best = -1;
    int best_score = INT_MIN;
    for (int i = 0; i < (int)nodes.size(); ++i) {
        if (nodes[i].state.now_score > best_score) {
            arg_best = i;
            best_score = nodes[i].state.now_score;
        }
    }

    if (arg_best == -1) {
        Node node(init_state);
        // node.move_history = Stack{nullptr};
        return node;
    }

    return nodes[arg_best];
}


// vector<string> get_answer(const Node& result) {
//     vector<char> ans;
//     vector<string> ret(N);

//     Stack move_history = result.move_history;
//     while (move_history.head) {
//         char tmp = move_history.top();
//         ans.emplace_back(tmp);
//         move_history = move_history.pop();
//     }
//     reverse(ans.begin(),ans.end());

//     for (int i = 0; i < ans.size(); ++i) {
//         ret[i%N].push_back(ans[i]);
//     }

//     return ret;
// }

vector<string> get_answer(const Node& result) {
    // auto s1 = std::chrono::high_resolution_clock::now();
    // vector<char> ans;
    vector<string> ret(N);
    vector<bool> is_have(N);
    vector<int> x_pos(N);

    // vector<char> all_operations = {'L','R','U','D','P','Q'};

    // bitset<3*N*MAX_DEPTH> bit_mask(7);
    auto move_history = result.state.move_history;

    // cerr << "test" << endl;

    int depth = result.state.depth;

    int now = 0;
    
    for (int t = 0; t < depth; ++t) {
        int tmp = 0;
        for (int i = 1; i >= 0; --i) {
            tmp <<= 1;
            if (move_history[now+i]) tmp |= 1;
        }

        // int x = N-1-t%N;
        int x = t%N;
        switch (tmp) {
        case BIT_LR:
            if (is_have[x]) {
                ret[x].push_back('R');
                ++x_pos[x];
            }
            else {
                if (x_pos[x] != 0) {
                    ret[x].push_back('L');
                    --x_pos[x];
                }
                else ret[x].push_back('.');
            }
            break;
        case BIT_U:
            ret[x].push_back('U');
            break;
        case BIT_D:
            ret[x].push_back('D');
            break;
        case BIT_PQ:
            if (is_have[x]) ret[x].push_back('Q');
            else ret[x].push_back('P');
            is_have[x] = !is_have[x];
        
        default:
            break;
        }

        // move_history >>= 3;
        now += 2;
    }

    // auto s2 = std::chrono::high_resolution_clock::now();
    // cerr << "test" << endl;
    // cerr << duration_cast<microseconds>(s2-s1).count() << endl;

    // reverse(ans.begin(),ans.end());
    // for (int i = 0; i < ans.size(); ++i) {
    //     ret[i%N].push_back(ans[i]);
    // }

    return ret;
}


void output(const vector<string>&  ans) {
    for (int i = 0; i < N; ++i) cout << ans[i] << endl;
}

int main(void) {
    input();
    init_zobrist();

    // 与えられた数列の難しさを求める
    // int diff1 = 0, diff2 = 0;
    // for (int i = 0; i < NN; ++i) {
    //     diff1 += abs(i/N - A[i]/N);
    //     diff2 += abs(i%N - A[i]%N);
    // }
    // cerr << diff1 << ' ' << diff2 << endl;

    // vector<int> cnt(N);
    // for (int i = 0; i < NN; ++i) {
    //     if (A[i]/N < N/2) {
    //         if (i/N > N/2) cnt[A[i]%N]++;
    //     }
    //     else if (A[i]/N == N/2) {
    //         if (i/N == N/2) cnt[A[i]%N]++;
    //     }
    //     else {
    //         if (i/N < N/2) cnt[A[i]%N]++;
    //     }
    // }
    // for (int i = 0; i < N; ++i) cerr << cnt[i] << ' ';
    // cerr << endl;

    State state;
    state.init();

    // Node node = beamSearch(state,N*150,10000);

    
    
    initPOW(2,true);
    // vector<string> ans1;
    // vector<string> ans2;
    // if (diff2 >= 48) ans1 = get_answer(beamSearch(state,N*MAX_DEPTH,55000,true));
    // else {
    //     ans1 = get_answer(beamSearch(state,N*MAX_DEPTH,30000,true));
    //     test = true;
    //     ans2 = get_answer(beamSearch(state,N*MAX_DEPTH,30000,true));
    //     if (ans1[0].size() > ans2[0].size()) swap(ans1,ans2);
    //     if (ans1[0].size() == 0) swap(ans1,ans2);
    //     // cerr << ans1[0].size() << ' ' << ans2[0].size() << endl;
    // }
    
    auto ans1 = get_answer(beamSearch(state,N*MAX_DEPTH,55000,true));
    // cerr << ans1[0].size() << endl;

    output(ans1);
    // cerr << ans1[0].size() << endl;
    
    return 0;
}