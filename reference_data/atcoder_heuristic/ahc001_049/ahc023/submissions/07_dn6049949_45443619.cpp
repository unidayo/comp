#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,tune=native")
#pragma GCC target("avx2")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("O3")


#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cmath>
#include <chrono>


#define FOR(i, s, e) for (int i = int(s); i < int(e); ++i)

using namespace std;

constexpr int DYX[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};

constexpr int MAX_TIME = 1980;

double start = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();


unsigned int randxor() {
    static unsigned int x=123456789,y=362436069,z=521288629,w=88675123;
    unsigned int t;
    t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
}


int T = 100;
int H = 20;
int W = 20;
int i0,K;
int n = 400;
unsigned long long int full_mask = (1LL<<51)-1;
unsigned long long int sup_masks[102][2], inf_masks[102][2], bits[102][2], reaches[800], temp_reaches[800], cannot_reaches[800];
int ss[40000], ss_[40000], ds[40000], q[10000], is_blocked[400], plant_list[101][400], harvest_list[101][400], plant_list_size[101], harvest_list_size[101];
vector<int> points, best_combination, planted_crops_list[400], temp_planted_crops_list[400];
void calculate_reaches(const unsigned long long int graph[], const int entrance, const int blocked_point) {
    FOR(i,0,2*n) {
        reaches[i] = 0;
    }
    reaches[2*entrance] = full_mask;
    reaches[2*entrance+1] = full_mask;
    q[0] = entrance;
    int q_size = 1;
    int index = 0;
    while (index < q_size) {
        const auto i = q[index++];
        if (i == blocked_point) {
            continue;
        }
        const auto ri0 = reaches[2*i];
        const auto ri1 = reaches[2*i+1];
        const auto index = 8*i;
        FOR(k,0,4) {
            int j = i+DYX[k][0]*W+DYX[k][1];
            const auto rj0 = reaches[2*j];
            const auto rj1 = reaches[2*j+1];
            const auto nrj0 = rj0|(ri0&graph[index+2*k]);
            const auto nrj1 = rj1|(ri1&graph[index+2*k+1]);
            if (rj0 == nrj0 && rj1 == nrj1) {
                continue;
            }
            reaches[2*j] = nrj0;
            reaches[2*j+1] = nrj1;
            q[q_size++] = j;
        }
    }
}


void calculate_reaches(const unsigned long long int graph[], const int entrance) {
    FOR(i,0,2*n) {
        temp_reaches[i] = 0;
    }
    temp_reaches[2*entrance] = inf_masks[1][0];
    temp_reaches[2*entrance+1] = sup_masks[T][1];
    q[0] = entrance;
    int q_size = 1;
    int index = 0;
    while (index < q_size) {
        const auto i = q[index++];
        if (is_blocked[i]) {
            continue;
        }
        const auto ri0 = temp_reaches[2*i];
        const auto ri1 = temp_reaches[2*i+1];
        const auto index = 8*i;
        FOR(k,0,4) {
            int j = i+DYX[k][0]*W+DYX[k][1];
            const auto rj0 = temp_reaches[2*j];
            const auto rj1 = temp_reaches[2*j+1];
            const auto nrj0 = rj0|(ri0&graph[index+2*k]);
            const auto nrj1 = rj1|(ri1&graph[index+2*k+1]);
            if (rj0 == nrj0 && rj1 == nrj1) {
                continue;
            }
            temp_reaches[2*j] = nrj0;
            temp_reaches[2*j+1] = nrj1;
            q[q_size++] = j;
        }
    }
}


void add_points_to_reaches(const unsigned long long int graph[]) {
    int q_size = 0;
    for (const auto point: points) {
        if (is_blocked[point]) {
            continue;
        }
        q[q_size++] = point;
    }
    int index = 0;
    while (index < q_size) {
        const auto i = q[index++];
        if (is_blocked[i]) {
            continue;
        }
        const auto ri0 = reaches[2*i];
        const auto ri1 = reaches[2*i+1];
        const auto index = 8*i;
        FOR(k,0,4) {
            int j = i+DYX[k][0]*W+DYX[k][1];
            const auto rj0 = reaches[2*j];
            const auto rj1 = reaches[2*j+1];
            const auto nrj0 = rj0|(ri0&graph[index+2*k]);
            const auto nrj1 = rj1|(ri1&graph[index+2*k+1]);
            if (rj0 == nrj0 && rj1 == nrj1) {
                continue;
            }
            reaches[2*j] = nrj0;
            reaches[2*j+1] = nrj1;
            q[q_size++] = j;
        }
    }
}


int can_reach(const int target, const int t) {
    if (t < 51) {
        if (t == 0) {
            return 1;
        }
        if (bits[t][0]&cannot_reaches[2*target]) {
            return 0;
        }
    } else {
        if (t == T) {
            return 1;
        }
        if (bits[t][1]&cannot_reaches[2*target+1]) {
            return 0;
        }
    }
    return 1;
}


void remove_node(unsigned long long int graph[], const int p, const int adjs[], const int s, const int d) {
    unsigned long long int masks[2] = {inf_masks[s][0]&sup_masks[d][0], inf_masks[s][1]&sup_masks[d][1]};
    masks[0] ^= full_mask;
    masks[1] ^= full_mask;
    FOR(k,0,4) {
        if (!((adjs[p]>>k)&1)) {
            continue;
        }
        int i = 8*p+2*k;
        int j = 8*(p+DYX[k][0]*W+DYX[k][1])+2*(k^2);
        graph[i] &= masks[0];
        graph[i+1] &= masks[1];
        graph[j] &= masks[0];
        graph[j+1] &= masks[1];
    }
}


void add_node(unsigned long long int graph[], const int p, const int adjs[], const unsigned long long int can_plant[], const int s, const int d) {
    unsigned long long int masks[2] = {inf_masks[s][0]&sup_masks[d][0], inf_masks[s][1]&sup_masks[d][1]};
    FOR(k,0,4) {
        if (!((adjs[p]>>k)&1)) {
            continue;
        }
        int q = p+DYX[k][0]*W+DYX[k][1];
        unsigned long long int masks_[2] = {can_plant[2*q]&masks[0], can_plant[2*q+1]&masks[1]};
        int i = 8*p+2*k;
        int j = 8*q+2*(k^2);
        graph[i] |= masks_[0];
        graph[i+1] |= masks_[1];
        graph[j] |= masks_[0];
        graph[j+1] |= masks_[1];
    }
}


int dp[101], pre[101];
unsigned long long int graph[3200], temp_graph[3200], can_plant[800], temp_can_plant[800], cannot_plant[2];
int entrance;
int ans[40000], adjs[400], cannot_reaches_[101], cannot_plants[100], crops_list[10100][100], crops_list_size[10100], crops_list_size_of_each_d[101];
int T_ = T+1;
void calculate_best_combination(const int target, const int score_offset) {
    FOR(t,0,T_) {
        dp[t] = 0;
        pre[t] = -1;
        cannot_reaches_[t] = !can_reach(target, t);
        cannot_plants[t] = (cannot_plant[0]&bits[t][0]) || (cannot_plant[1]&bits[t][1]);
    }
    auto dp_d = dp[1];
    auto pre_d = pre[1];
    FOR(d,2,T_) {
        if (dp_d < dp[d]) {
            dp_d = dp[d];
            pre_d = pre[d];
        }
        if (cannot_reaches_[d]) {
            dp[d] = dp_d;
            pre[d] = pre_d;
            continue;
        }
        auto crop_list_size_of_each_d_ = crops_list_size_of_each_d[d];
        for(int s = d-1; s >= 1; --s) {
            if (crop_list_size_of_each_d_ == 0) {
                break;
            }
            int sd_key = d*T+s;
            crop_list_size_of_each_d_ -= crops_list_size[sd_key];
            if (cannot_plants[s]) {
                break;
            }
            if (cannot_reaches_[s-1] || crops_list_size[sd_key] == 0) {
                continue;
            }
            auto score = dp[s-1]+50*(d-s)+score_offset;
            if (dp_d < score) {
                dp_d = score;
                pre_d = sd_key;
            }
        }
        dp[d] = dp_d;
        pre[d] = pre_d;
    }
    best_combination.clear();
    int t = T;
    while (pre[t] >= 0) {
        const auto sd_key = pre[t];
        t = sd_key%T-1;
        const auto crop = crops_list[sd_key][--crops_list_size[sd_key]];
        --crops_list_size_of_each_d[sd_key/T];
        best_combination.push_back(crop);
    }
}


deque<int> q2;
void calculate_distances(const int adjs[], const int entrance, int distances[]) {
    FOR(i,0,n) {
        distances[i] = -1;
    }
    distances[entrance] = 0;
    q[0] = entrance;
    int q_size = 1;
    int index = 0;
    while (index < q_size) {
        const auto i = q[index++];
        q2.push_back(i);
        const auto nd = distances[i]+1;
        FOR(k,0,4) {
            if (!((adjs[i]>>k)&1)) {
                continue;
            }
            int j = i+DYX[k][0]*W+DYX[k][1];
            if (distances[j] < 0) {
                distances[j] = nd;
                q[q_size++] = j;
            }
        }
    }
}


void print_ans(const int ans[]) {
    vector<vector<int>> res;
    FOR(i,0,K) {
        const auto point = ans[i];
        if (point < 0) {
            continue;
        }
        const auto y = point/W;
        const auto x = point%W;
        res.push_back({i+1,y,x,ss[i]});
    }
    cout << res.size() << endl;
    for (const auto& item: res) {
        cout << item[0] << " " << item[1] << " " << item[2] << " " << item[3] << endl;
    }
}


long long int calculate_score(const int ans[]) {
    long long int score = 0;
    FOR(i,0,K) {
        if (ans[i] < 0) {
            continue;
        }
        score += ds[i]-ss_[i]+1;
    }
    return score;
}

long long int calculate_score() {
    long long int score = 0;
    for (const auto& point: points) {
        for (const auto& i: planted_crops_list[point]) {
            score += ds[i]-ss_[i]+1;
        }
    }
    return score;
}


void greedy(const int score_offset) {
    for(const auto& point: points) {
        calculate_reaches(graph, entrance, point);

        FOR(i,0,2*n) {
            cannot_reaches[i] = full_mask^reaches[i];
        }
        cannot_plant[0] = 0;
        cannot_plant[1] = 0;
        FOR(t,0,51) {
            const auto bit = bits[t][0];
            int cannot_plant_ = 0;
            const auto t_harvest_list_size = harvest_list_size[t];
            const auto t_harvest_list = harvest_list[t];
            FOR(i,0,t_harvest_list_size) {
                const auto point = t_harvest_list[i];
                if (bit&cannot_reaches[2*point]) {
                    cannot_plant_ = 1;
                    break;
                }
            }
            if (!cannot_plant_) {
                const auto t_plant_list_size = plant_list_size[t+1];
                const auto t_plant_list = plant_list[t+1];
                FOR(i,0,t_plant_list_size) {
                    const auto point = t_plant_list[i];
                    if (bit&cannot_reaches[2*point]) {
                        cannot_plant_ = 1;
                        break;
                    }
                }
            }
            if (cannot_plant_) {
                cannot_plant[0] |= bit;
            }
        }
        FOR(t,51,T) {
            const auto bit = bits[t][1];
            int cannot_plant_ = 0;
            const auto t_harvest_list_size = harvest_list_size[t];
            const auto t_harvest_list = harvest_list[t];
            FOR(i,0,t_harvest_list_size) {
                const auto point = t_harvest_list[i];
                if (bit&cannot_reaches[2*point+1]) {
                    cannot_plant_ = 1;
                    break;
                }
            }
            if (!cannot_plant_) {
                const auto t_plant_list_size = plant_list_size[t+1];
                const auto t_plant_list = plant_list[t+1];
                FOR(i,0,t_plant_list_size) {
                    const auto point = t_plant_list[i];
                    if (bit&cannot_reaches[2*point+1]) {
                        cannot_plant_ = 1;
                        break;
                    }
                }
            }
            if (cannot_plant_) {
                cannot_plant[1] |= bit;
            }
        }

        calculate_best_combination(point, score_offset);
        for(const auto& i: best_combination) {
            const auto s = ss[i];
            const auto d = ds[i];
            ans[i] = point;
            can_plant[2*point] ^= inf_masks[s][0]&sup_masks[d][0];
            can_plant[2*point+1] ^= inf_masks[s][1]&sup_masks[d][1];
            plant_list[s][plant_list_size[s]++] = point;
            harvest_list[d][harvest_list_size[d]++] = point;
            planted_crops_list[point].push_back(i);
            remove_node(graph, point, adjs, s, d);
        }
    }
}


void partial_greedy(const int score_offset) {
    for (const auto& point: points) {
        is_blocked[point] = 1;
    }
    calculate_reaches(graph, entrance);
    for (const auto& other_point: points) {
        is_blocked[other_point] = 0;
    }
    for (const auto& point: points) {
        FOR(i,0,2*n) {
            reaches[i] = temp_reaches[i];
        }
        is_blocked[point] = 1;
        add_points_to_reaches(graph);
        is_blocked[point] = 0;
        FOR(i,0,2*n) {
            cannot_reaches[i] = full_mask^reaches[i];
        }
        cannot_plant[0] = 0;
        cannot_plant[1] = 0;
        FOR(t,0,51) {
            const auto bit = bits[t][0];
            int cannot_plant_ = 0;
            const auto t_harvest_list_size = harvest_list_size[t];
            const auto t_harvest_list = harvest_list[t];
            FOR(i,0,t_harvest_list_size) {
                const auto point = t_harvest_list[i];
                if (bit&cannot_reaches[2*point]) {
                    cannot_plant_ = 1;
                    break;
                }
            }
            if (!cannot_plant_) {
                const auto t_plant_list_size = plant_list_size[t+1];
                const auto t_plant_list = plant_list[t+1];
                FOR(i,0,t_plant_list_size) {
                    const auto point = t_plant_list[i];
                    if (bit&cannot_reaches[2*point]) {
                        cannot_plant_ = 1;
                        break;
                    }
                }
            }
            if (cannot_plant_) {
                cannot_plant[0] |= bit;
            }
        }
        FOR(t,51,T) {
            const auto bit = bits[t][1];
            int cannot_plant_ = 0;
            const auto t_harvest_list_size = harvest_list_size[t];
            const auto t_harvest_list = harvest_list[t];
            FOR(i,0,t_harvest_list_size) {
                const auto point = t_harvest_list[i];
                if (bit&cannot_reaches[2*point+1]) {
                    cannot_plant_ = 1;
                    break;
                }
            }
            if (!cannot_plant_) {
                const auto t_plant_list_size = plant_list_size[t+1];
                const auto t_plant_list = plant_list[t+1];
                FOR(i,0,t_plant_list_size) {
                    const auto point = t_plant_list[i];
                    if (bit&cannot_reaches[2*point+1]) {
                        cannot_plant_ = 1;
                        break;
                    }
                }
            }
            if (cannot_plant_) {
                cannot_plant[1] |= bit;
            }
        }

        calculate_best_combination(point, score_offset);
        for(const auto& i: best_combination) {
            const auto s = ss[i];
            const auto d = ds[i];
            ans[i] = point;
            can_plant[2*point] ^= inf_masks[s][0]&sup_masks[d][0];
            can_plant[2*point+1] ^= inf_masks[s][1]&sup_masks[d][1];
            plant_list[s][plant_list_size[s]++] = point;
            harvest_list[d][harvest_list_size[d]++] = point;
            planted_crops_list[point].push_back(i);
            remove_node(graph, point, adjs, s, d);
        }
        temp_reaches[2*point] &= can_plant[2*point];
        temp_reaches[2*point+1] &= can_plant[2*point+1];
    }
}


void destruct_ans() {
    for (const auto& point: points) {
        temp_can_plant[2*point] = can_plant[2*point];
        temp_can_plant[2*point+1] = can_plant[2*point+1];
        can_plant[2*point] = full_mask;
        can_plant[2*point+1] = full_mask;
        auto planted_crops_list_ = &planted_crops_list[point];
        auto temp_planted_crops_list_ = &temp_planted_crops_list[point];
        for (const auto& i: *planted_crops_list_) {
            ans[i] = -1;
            const auto s = ss[i];
            const auto d = ds[i];
            const auto t_plant_list_size = plant_list_size[s];
            auto t_plant_list = plant_list[s];
            FOR(i,0,t_plant_list_size) {
                if (t_plant_list[i] == point) {
                    t_plant_list[i] = t_plant_list[t_plant_list_size-1];
                    break;
                }
            }
            --plant_list_size[s];
            const auto t_harvest_list_size = harvest_list_size[d];
            auto t_harvest_list = harvest_list[d];
            FOR(i,0,t_harvest_list_size) {
                if (t_harvest_list[i] == point) {
                    t_harvest_list[i] = t_harvest_list[t_harvest_list_size-1];
                    break;
                }
            }
            --harvest_list_size[d];
            int sd_key = d*T+s;
            crops_list[sd_key][crops_list_size[sd_key]++] = i;
            ++crops_list_size_of_each_d[d];
        }
        temp_planted_crops_list_->clear();
        copy(planted_crops_list_->begin(), planted_crops_list_->end(), back_inserter(*temp_planted_crops_list_));
        planted_crops_list_->clear();
    }
    FOR(i,0,8*n) {
        temp_graph[i] = graph[i];
    }
    for (const auto& p: points) {
        add_node(graph, p, adjs, can_plant, 0, T+2);
    }
}


void restore() {
    for (const auto& point: points) {
        can_plant[2*point] = temp_can_plant[2*point];
        can_plant[2*point+1] = temp_can_plant[2*point+1];
        auto planted_crops_list_ = &planted_crops_list[point];
        for (const auto& i: *planted_crops_list_) {
            ans[i] = -1;
            const auto s = ss[i];
            const auto d = ds[i];
            const auto t_plant_list_size = plant_list_size[s];
            auto t_plant_list = plant_list[s];
            FOR(i,0,t_plant_list_size) {
                if (t_plant_list[i] == point) {
                    t_plant_list[i] = t_plant_list[t_plant_list_size-1];
                    break;
                }
            }
            --plant_list_size[s];
            const auto t_harvest_list_size = harvest_list_size[d];
            auto t_harvest_list = harvest_list[d];
            FOR(i,0,t_harvest_list_size) {
                if (t_harvest_list[i] == point) {
                    t_harvest_list[i] = t_harvest_list[t_harvest_list_size-1];
                    break;
                }
            }
            --harvest_list_size[d];
            int sd_key = d*T+s;
            crops_list[sd_key][crops_list_size[sd_key]++] = i;
            ++crops_list_size_of_each_d[d];
        }
    }
    FOR(i,0,8*n) {
        graph[i] = temp_graph[i];
    }
    for (const auto& point: points) {
        auto planted_crops_list_ = &planted_crops_list[point];
        auto temp_planted_crops_list_ = &temp_planted_crops_list[point];
        for (const auto& i: *temp_planted_crops_list_) {
            ans[i] = point;
            const auto s = ss[i];
            const auto d = ds[i];
            plant_list[s][plant_list_size[s]++] = point;
            harvest_list[d][harvest_list_size[d]++] = point;
            const auto sd_key = d*T+s;
            auto sd_crops_list = crops_list[sd_key];
            const auto sd_crops_list_size = crops_list_size[sd_key];
            FOR(j,0,sd_crops_list_size) {
                if (sd_crops_list[j] == i) {
                    sd_crops_list[j] = sd_crops_list[sd_crops_list_size-1];
                    break;
                }
            }
            --crops_list_size[sd_key];
            --crops_list_size_of_each_d[d];
        }
        planted_crops_list_->clear();
        copy(temp_planted_crops_list_->begin(), temp_planted_crops_list_->end(), back_inserter(*planted_crops_list_));
    }
}


int main() {
    cin >> T >> H >> W >> i0;
    n = H*W;
    int hs[H-1][W], vs[H][W-1];
    FOR(i,0,H-1) {
        FOR(j,0,W) {
            char x;
            cin >> x;
            hs[i][j] = (x-'0');
        }
    }
    FOR(i,0,H) {
        FOR(j,0,W-1) {
            char x;
            cin >> x;
            vs[i][j] = (x-'0');
        }
    }
    cin >> K;
    FOR(i,0,K) {
        cin >> ss_[i] >> ds[i];
        ss[i] = ss_[i];
        if (ss[i] <= 2) {
            ss[i] = 1;
        }
    }
    FOR(i,0,51) {
        unsigned long long int sup_mask = (1LL<<i)-1;
        inf_masks[i][0] = full_mask^sup_mask;
        inf_masks[i][1] = full_mask;
        sup_masks[i][0] = sup_mask;
        sup_masks[i][1] = 0;
        bits[i][0] = 1LL<<i;
        bits[i][1] = 0;
    }
    FOR(i,51,102) {
        unsigned long long int sup_mask = (1LL<<(i-51))-1;
        inf_masks[i][0] = 0;
        inf_masks[i][1] = full_mask^sup_mask;
        sup_masks[i][0] = full_mask;
        sup_masks[i][1] = sup_mask;
        bits[i][0] = 0;
        bits[i][1] = 1LL<<(i-51);
    }
    entrance = i0*W;
    int blocked[n][4];
    FOR(y,0,H) {
        FOR(x,0,W) {
            int i = y*W+x;
            FOR(d,0,4) {
                blocked[i][d] = 0;
            }
        }
    }
    FOR(y,0,H) {
        FOR(x,0,W) {
            int i = y*W+x;
            if (y == 0) {
                blocked[i][2] = 1;
            }
            if (y+1 == H) {
                blocked[i][0] = 1;
            }
            if (x == 0) {
                blocked[i][3] = 1;
            }
            if (x+1 == W) {
                blocked[i][1] = 1;
            }
            if (y+1 < H && hs[y][x]) {
                blocked[i][0] = 1;
                blocked[i+W][2] = 1;
            }
            if (x+1 < W && vs[y][x]) {
                blocked[i][1] = 1;
                blocked[i+1][3] = 1;
            }
        }
    }

    FOR(i,0,n) {
        adjs[i] = 0;
    }
    FOR(y,0,H) {
        FOR(x,0,W) {
            int i = y*W+x;
            FOR(k,0,4) {
                if (blocked[i][k]) {
                    continue;
                }
                int j = i+DYX[k][0]*W+DYX[k][1];
                adjs[i] |= (1<<k);
                adjs[j] |= (1<<(k^2));
            }
        }
    }

    FOR(t,0,T_) {
        plant_list_size[t] = 0;
        harvest_list_size[t] = 0;
    }

    FOR(d,2,T_) {
        crops_list_size_of_each_d[d] = 0;
        FOR(s,1,d) {
            crops_list_size[d*T+s] = 0;
        }
    }

    vector<int> crops;
    FOR(i,0,K) {
        crops.push_back(i);
    }
    sort(crops.begin(), crops.end(), [](int a, int b){
        return ss_[a]-ss[a] > ss_[b]-ss[b];
    });

    for (const auto& i: crops) {
        const auto s = ss[i];
        const auto d = ds[i];
        int sd_key = d*T+s;
        crops_list[sd_key][crops_list_size[sd_key]++] = i;
        ++crops_list_size_of_each_d[d];
    }

    int best_ans[K], distances[n];

    FOR(i,0,n) {
        points.push_back(i);
    }

    calculate_distances(adjs, entrance, distances);
    int key[n];
    FOR(i,0,n) {
        const auto di = distances[i];
        int found = 0;
        FOR(k,0,4) {
            if (!((adjs[i]>>k)&1)) {
                continue;
            }
            int j = i+DYX[k][0]*W+DYX[k][1];
            if (di <= distances[j]) {
                found = 1;
                break;
            }
        }
        if (found) {
            key[i] = -1;
        } else {
            key[i] = 0;
        }
    }
    while (!q2.empty()) {
        const auto i = q2.back();
        q2.pop_back();
        const auto nk = key[i]+1;
        const auto di = distances[i];
        FOR(k,0,4) {
            if (!((adjs[i]>>k)&1)) {
                continue;
            }
            int j = i+DYX[k][0]*W+DYX[k][1];
            if (di <= distances[j]) {
                continue;
            }
            if (key[j] < nk) {
                key[j] = nk;
            }
        }
    }

    stable_sort(points.begin(), points.end(), [&key, &distances](int a, int b) {
        return key[a]*20-distances[a] < key[b]*20-distances[b];
    });

    FOR(i,0,K) {
        ans[i] = -1;
    }

    FOR(i,0,n) {
        FOR(k,0,4) {
            if ((adjs[i]>>k)&1) {
                graph[8*i+2*k] = full_mask;
                graph[8*i+2*k+1] = full_mask;
            } else {
                graph[8*i+2*k] = 0;
                graph[8*i+2*k+1] = 0;
            }
        }
    }
    FOR(i,0,2*n) {
        can_plant[i] = full_mask;
    }

    greedy(-25);

    FOR(i,0,K) {
        best_ans[i] = ans[i];
    }
    auto best_score = calculate_score(best_ans);

    int width = 2;

    vector<int> delete_start_points;
    FOR(y,0,H-width+1) {
        FOR(x,0,W-width+1) {
            delete_start_points.push_back(y*W+x);
        }
    }

    stable_sort(delete_start_points.begin(), delete_start_points.end(), [&key, &distances](int a, int b) {
        return key[a]*20-distances[a] < key[b]*20-distances[b];
    });

    FOR(i,0,n) {
        is_blocked[i] = 0;
    }

    unsigned int randoms[K];
    auto temp_score = best_score;
    double time = 0;
    int iter = 0;
    int delete_start_point_index = 0;
    int skip_index = 0;
    while (time < MAX_TIME) {
        time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - start;
        points.clear();
        const auto delete_start_point = delete_start_points[delete_start_point_index];
        const int up = delete_start_point/W;
        const int left = delete_start_point%W;
        const int down = up+width;
        const int right = left+width;
        int index = 0;
        FOR(y,up,down) {
            FOR(x,left,right) {
                if (index++ == skip_index) {
                    continue;
                }
                points.push_back(y*W+x);
            }
        }
        delete_start_point_index = (delete_start_point_index+1)%delete_start_points.size();
        if (delete_start_point_index == 0) {
            skip_index = (skip_index+1)%4;
        }
        unsigned int count = 0;
        for (const auto& p: points) {
            FOR(k,0,4) {
                if (!((adjs[p]>>k)&1)) {
                    continue;
                }
                const auto np = p+DYX[k][0]*W+DYX[k][1];
                if (find(points.begin(), points.end(), np) != points.end()) {
                    ++count;
                }
            }
        }
        if (count < 2*(points.size()-1)) {
            continue;
        }
        ++iter;
        const auto prev_score = calculate_score();
        destruct_ans();
        for (const auto& point: points) {
            randoms[point] = randxor();
        }
        stable_sort(points.begin(), points.end(), [&randoms](int a, int b) {
            return randoms[a] > randoms[b];
        });
        const auto score_offset = (int)(randxor()%51*time/MAX_TIME)-25;
        partial_greedy(score_offset);
        const auto diff_score = calculate_score() - prev_score;
        // cerr << time << " " << iter << " " << 1000000*(temp_score+diff_score)/(H*W*T) << " " << 1000000*temp_score/(H*W*T) << " " << 1000000*best_score/(H*W*T) << endl;
        if (diff_score > 0) {
            temp_score += diff_score;
            if (best_score < temp_score) {
                best_score = temp_score;
                FOR(i,0,K) {
                    best_ans[i] = ans[i];
                }
            }
        } else {
            double temperature = best_score*(MAX_TIME-time)/MAX_TIME/2000;
            double prob = exp(diff_score / temperature);
            if (randxor() < prob * 4294967296) {
                temp_score += diff_score;
            } else {
                restore();
            }
        }
    }
    cerr << iter << " " << 1000000*best_score/(H*W*T) << endl;

    print_ans(best_ans);
    return 0;
}