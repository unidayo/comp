#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <algorithm>

using namespace std;

const int MAX_D = 14;
const int MAX_D2 = MAX_D * MAX_D;
const int MAX_D3 = MAX_D * MAX_D * MAX_D;

short dx[24] = {3, -2, -3, 2, 3, 2, -3, -2, 3, 1, -3, -1, 3, -1, -3, 1, 2, -1, -2, 1, 2, 1, -2, -1};
short dy[24] = {2, 3, -2, -3, -2, 3, 2, -3, -1, 3, 1, -3, 1, 3, -1, -3, 1, 2, -1, -2, -1, 2, 1, -2};
short dz[24] = {1, 1, 1, 1, -1, -1, -1, -1, 2, 2, 2, 2, -2, -2, -2, -2, 3, 3, 3, 3, -3, -3, -3, -3};

class timer {
    public:
    void start() {
        origin = rdtsc();
    }
    
    inline double get_time() {
        return (rdtsc() - origin) * SECONDS_PER_CLOCK;
    }
    
    private:
    constexpr static double SECONDS_PER_CLOCK = 1 / 3.0e9;
    unsigned long long origin;
    
    inline static unsigned long long rdtsc() {
        unsigned long long lo, hi;
        __asm__ volatile ("rdtsc" : "=a" (lo), "=d" (hi));
        return (hi << 32) | lo;
    }
};

class random {
    public:
    // [0, x)
    inline static unsigned get(unsigned x) {
        return ((unsigned long long)xorshift() * x) >> 32;
    }
    
    // [x, y]
    inline static unsigned get(unsigned x, unsigned y) {
        return get(y - x + 1) + x;
    }
    
    // [0, x] (x = 2^c - 1)
    inline static unsigned get_fast(unsigned x) {
        return xorshift() & x;
    }
    
    // [0.0, 1.0]
    inline static double probability() {
        return xorshift() * INV_MAX;
    }
    
    inline static bool toss() {
        return xorshift() & 1;
    }
    
    private:
    constexpr static double INV_MAX = 1.0 / 0xFFFFFFFF;
    
    inline static unsigned xorshift() {
        static unsigned x = 123456789, y = 362436039, z = 521288629, w = 88675123;
        unsigned t = x ^ (x << 11);
        x = y, y = z, z = w;
        return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
    }
};

class simulated_annealing {
    public:
    simulated_annealing();
    void set_time_limit(double t);
    void set_start_temp(double t1, double t2);
    inline bool end1();
    inline bool end2();
    inline bool accept1(double current_score, double next_score);
    inline bool accept2(double current_score, double next_score);
    void print() const;
    
    private:
    constexpr static bool MAXIMIZE = false;
    constexpr static int LOG_SIZE = 0xFFFF;
    constexpr static int UPDATE_INTERVAL = 0xFF;
    constexpr static double TIME_LIMIT2 = 5.9;
    constexpr static double END_TEMP = 1e-9;
    double TIME_LIMIT1;
    double START_TEMP1;
    double START_TEMP2;
    double TEMP_RATIO1;
    double TEMP_RATIO2;
    double log_probability[LOG_SIZE + 1];
    long long iteration = 0;
    long long accepted = 0;
    long long rejected = 0;
    double time = 0;
    double temp1 = START_TEMP1;
    double temp2 = START_TEMP2;
    timer sa_timer;
};

simulated_annealing::simulated_annealing() {
    sa_timer.start();
    for (int i = 0; i <= LOG_SIZE; i++) log_probability[i] = log(random::probability());
}

void simulated_annealing::set_time_limit(double t) {
    TIME_LIMIT1 = t;
}

void simulated_annealing::set_start_temp(double t1, double t2) {
    START_TEMP1 = t1;
    TEMP_RATIO1 = (END_TEMP - START_TEMP1) / TIME_LIMIT2;
    temp1 = t1;
    START_TEMP2 = t2;
    TEMP_RATIO2 = (END_TEMP - START_TEMP2) / TIME_LIMIT2;
    temp2 = t2;
}

inline bool simulated_annealing::end1() {
    iteration++;
    if ((iteration & UPDATE_INTERVAL) == 0) {
        time = sa_timer.get_time();
        temp1 = START_TEMP1 + TEMP_RATIO1 * time;
        temp2 = START_TEMP2 + TEMP_RATIO2 * time;
        return time >= TIME_LIMIT1;
    } else {
        return false;
    }
}

inline bool simulated_annealing::end2() {
    iteration++;
    if ((iteration & UPDATE_INTERVAL) == 0) {
        time = sa_timer.get_time();
        temp1 = START_TEMP1 + TEMP_RATIO1 * time;
        temp2 = START_TEMP2 + TEMP_RATIO2 * time;
        return time >= TIME_LIMIT2;
    } else {
        return false;
    }
}

inline bool simulated_annealing::accept1(double current_score, double next_score) {
    double diff = (MAXIMIZE ? next_score - current_score : current_score - next_score);
    if (diff >= 0 || diff > log_probability[random::get_fast(LOG_SIZE)] * temp1) {
        accepted++;
        return true;
    } else {
        rejected++;
        return false;
    }
}

inline bool simulated_annealing::accept2(double current_score, double next_score) {
    double diff = (MAXIMIZE ? next_score - current_score : current_score - next_score);
    if (diff >= 0 || diff > log_probability[random::get_fast(LOG_SIZE)] * temp2) {
        accepted++;
        return true;
    } else {
        rejected++;
        return false;
    }
}

void simulated_annealing::print() const {
    fprintf(stderr, "iteration: %lld\n", iteration);
    fprintf(stderr, "accepted: %lld\n", accepted);
    fprintf(stderr, "rejected: %lld\n", rejected);
}

template<int N> class random_array {
    public:
    random_array() : num(0) {}
    
    inline bool empty() const {
        return num == 0;
    }
    
    inline short size() const {
        return num;
    }
    
    inline void add(short value) {
        rdata[value] = num;
        data[num++] = value;
    }
    
    inline void remove(short value) {
        short pos = rdata[value];
        data[pos] = data[--num];
        rdata[data[pos]] = pos;
    }
    
    inline short get() const {
        return data[random::get(num)];
    }
    
    inline void save() {
        save_num = num;
        memcpy(save_data, data, sizeof(short) * num);
        memcpy(save_rdata, rdata, sizeof(short) * N);
    }
    
    inline void load() {
        num = save_num;
        memcpy(data, save_data, sizeof(short) * num);
        memcpy(rdata, save_rdata, sizeof(short) * N);
    }
    
    private:
    short num;
    short data[N];
    short rdata[N];
    short save_num;
    short save_data[N];
    short save_rdata[N];
};

struct block {
    short d;
    short di;
    char cnt1;
    char cnt2;
    vector<short> index1;
    
    block() {}
    block(short d, short di) : d(d), di(di) {}
    
    inline short size() const {
        return index1.size();
    }
    
    inline void add(short i1) {
        index1.push_back(i1);
    }
    
    inline void remove(short i1) {
        for (short i = 0; ; i++) {
            if (index1[i] == i1) {
                index1[i] = index1.back();
                index1.pop_back();
                return;
            }
        }
    }
    
    inline void init(const block& b) {
        d = b.d;
        di = b.di;
        index1.clear();
    }
    
    bool operator<(const block& b) const {
        if (cnt1 + cnt2 != b.cnt1 + b.cnt2) return cnt1 + cnt2 > b.cnt1 + b.cnt2;
        if (cnt1 * cnt2 != b.cnt1 * b.cnt2) return cnt1 * cnt2 < b.cnt1 * b.cnt2;
        return size() > b.size();
    }
};

struct record {
    unsigned char num;
    short d;
    short di;
    
    record() {}
    record(unsigned char num, short d, short di) : num(num), d(d), di(di) {}
};

int D, D2, D3;
int f1[MAX_D][MAX_D];
int r1[MAX_D][MAX_D];
int f2[MAX_D][MAX_D];
int r2[MAX_D][MAX_D];
unsigned char best_b1[MAX_D3];
unsigned char best_b2[MAX_D3];
unsigned char b1[MAX_D3];
unsigned char b2[MAX_D3];
vector<record> records;
int tmp_num;
int tmp_b[MAX_D3];
char cnt1[MAX_D2 * 2];
char cnt2[MAX_D2 * 2];
short front[MAX_D3];
short right[MAX_D3];
short coord[MAX_D3][3];
short mapping[24][MAX_D3];
short rmapping[24][MAX_D3];
bool ok1[MAX_D3];
bool ok2[MAX_D3];
vector<short> possible1;
vector<short> possible2;
vector<short> possible_side1[MAX_D2 * 2];
vector<short> possible_side2[MAX_D2 * 2];
vector<short> possible[MAX_D * 2][MAX_D * 2][MAX_D * 2];
random_array<MAX_D3> empty1;
random_array<MAX_D3> empty2;
random_array<MAX_D2 * 2> unfilled1;
random_array<MAX_D2 * 2> unfilled2;
vector<short> neighbors1[MAX_D3];
vector<short> neighbors2[MAX_D3];
vector<block> all_blocks;
vector<block> small_blocks;
vector<int> blocks1[MAX_D3];
vector<int> blocks2[MAX_D3];
block pool[MAX_D3];
vector<block*> available;
vector<unsigned char> numbers;
double inv[MAX_D3];

void input() {
    scanf("%d", &D);
    for (int z = 0; z < D; z++) {
        for (int x = 0; x < D; x++) {
            scanf("%1d", &f1[z][x]);
        }
    }
    for (int z = 0; z < D; z++) {
        for (int y = 0; y < D; y++) {
            scanf("%1d", &r1[z][y]);
        }
    }
    for (int z = 0; z < D; z++) {
        for (int x = 0; x < D; x++) {
            scanf("%1d", &f2[z][x]);
        }
    }
    for (int z = 0; z < D; z++) {
        for (int y = 0; y < D; y++) {
            scanf("%1d", &r2[z][y]);
        }
    }
}

void output(int n) {
    printf("%d\n", n);
    for (int i = 0; i < D3; i++) {
        if (i > 0) putchar(' ');
        printf("%d", b1[i]);
    }
    puts("");
    for (int i = 0; i < D3; i++) {
        if (i > 0) putchar(' ');
        printf("%d", b2[i]);
    }
    puts("");
    fflush(stdout);
}

inline short index(short x, short y, short z) {
    return x * D2 + y * D + z;
}

inline bool valid1(const block& b, short index1) {
    short index0 = b.index1[0], index2 = mapping[b.d][index0 - b.di];
    short diff_x = coord[index1][0] - coord[index0][0];
    short diff_y = coord[index1][1] - coord[index0][1];
    short diff_z = coord[index1][2] - coord[index0][2];
    short x = coord[index2][0], y = coord[index2][1], z = coord[index2][2];
    if (dx[b.d] == D2) {
        x += diff_x;
    } else if (dx[b.d] == -D2) {
        x -= diff_x;
    } else if (dx[b.d] == D) {
        y += diff_x;
    } else if (dx[b.d] == -D) {
        y -= diff_x;
    } else if (dx[b.d] == 1) {
        z += diff_x;
    } else {
        z -= diff_x;
    }
    if (dy[b.d] == D2) {
        x += diff_y;
    } else if (dy[b.d] == -D2) {
        x -= diff_y;
    } else if (dy[b.d] == D) {
        y += diff_y;
    } else if (dy[b.d] == -D) {
        y -= diff_y;
    } else if (dy[b.d] == 1) {
        z += diff_y;
    } else {
        z -= diff_y;
    }
    if (dz[b.d] == D2) {
        x += diff_z;
    } else if (dz[b.d] == -D2) {
        x -= diff_z;
    } else if (dz[b.d] == D) {
        y += diff_z;
    } else if (dz[b.d] == -D) {
        y -= diff_z;
    } else if (dz[b.d] == 1) {
        z += diff_z;
    } else {
        z -= diff_z;
    }
    if (x < 0 || x >= D || y < 0 || y >= D || z < 0 || z >= D) return false;
    return ok2[mapping[b.d][index1 - b.di]];
}

inline bool valid2(const block& b, short index2) {
    short index0 = mapping[b.d][b.index1[0] - b.di], index1 = b.index1[0];
    short diff_x = coord[index2][0] - coord[index0][0];
    short diff_y = coord[index2][1] - coord[index0][1];
    short diff_z = coord[index2][2] - coord[index0][2];
    short x = coord[index1][0], y = coord[index1][1], z = coord[index1][2];
    if (dx[b.d] == D2) {
        x += diff_x;
    } else if (dx[b.d] == -D2) {
        x -= diff_x;
    } else if (dx[b.d] == D) {
        x += diff_y;
    } else if (dx[b.d] == -D) {
        x -= diff_y;
    } else if (dx[b.d] == 1) {
        x += diff_z;
    } else {
        x -= diff_z;
    }
    if (dy[b.d] == D2) {
        y += diff_x;
    } else if (dy[b.d] == -D2) {
        y -= diff_x;
    } else if (dy[b.d] == D) {
        y += diff_y;
    } else if (dy[b.d] == -D) {
        y -= diff_y;
    } else if (dy[b.d] == 1) {
        y += diff_z;
    } else {
        y -= diff_z;
    }
    if (dz[b.d] == D2) {
        z += diff_x;
    } else if (dz[b.d] == -D2) {
        z -= diff_x;
    } else if (dz[b.d] == D) {
        z += diff_y;
    } else if (dz[b.d] == -D) {
        z -= diff_y;
    } else if (dz[b.d] == 1) {
        z += diff_z;
    } else {
        z -= diff_z;
    }
    if (x < 0 || x >= D || y < 0 || y >= D || z < 0 || z >= D) return false;
    return ok1[rmapping[b.d][index2] + b.di];
}

inline short get_unfilled1() {
    short i = unfilled1.get();
    return possible_side1[i][random::get(possible_side1[i].size())];
}

inline short get_unfilled2() {
    short i = unfilled2.get();
    return possible_side2[i][random::get(possible_side2[i].size())];
}

inline void use1(short index, unsigned char num) {
    b1[index] = num;
    if (cnt1[front[index]]++ == 0) unfilled1.remove(front[index]);
    if (cnt1[right[index]]++ == 0) unfilled1.remove(right[index]);
    empty1.remove(index);
}

inline void use2(short index, unsigned char num) {
    b2[index] = num;
    if (cnt2[front[index]]++ == 0) unfilled2.remove(front[index]);
    if (cnt2[right[index]]++ == 0) unfilled2.remove(right[index]);
    empty2.remove(index);
}

inline void unuse1(short index) {
    b1[index] = 0;
    if (--cnt1[front[index]] == 0) unfilled1.add(front[index]);
    if (--cnt1[right[index]] == 0) unfilled1.add(right[index]);
    empty1.add(index);
}

inline void unuse2(short index) {
    b2[index] = 0;
    if (--cnt2[front[index]] == 0) unfilled2.add(front[index]);
    if (--cnt2[right[index]] == 0) unfilled2.add(right[index]);
    empty2.add(index);
}

inline unsigned char get_number() {
    unsigned char num = numbers.back();
    numbers.pop_back();
    return num;
}

inline void return_number(unsigned char num) {
    numbers.push_back(num);
}

inline block* get_block(const block& origin) {
    block* b = available.back();
    available.pop_back();
    b->init(origin);
    return b;
}

inline void return_block(block* b) {
    available.push_back(b);
}

void precalc() {
    D2 = D * D;
    D3 = D2 * D;
    
    for (short d = 0; d < 24; d++) {
        if (dx[d] == 3) {
            dx[d] = D2;
        } else if (dx[d] == -3) {
            dx[d] = -D2;
        } else if (dx[d] == 2) {
            dx[d] = D;
        } else if (dx[d] == -2) {
            dx[d] = -D;
        }
    }
    for (short d = 0; d < 24; d++) {
        if (dy[d] == 3) {
            dy[d] = D2;
        } else if (dy[d] == -3) {
            dy[d] = -D2;
        } else if (dy[d] == 2) {
            dy[d] = D;
        } else if (dy[d] == -2) {
            dy[d] = -D;
        }
    }
    for (short d = 0; d < 24; d++) {
        if (dz[d] == 3) {
            dz[d] = D2;
        } else if (dz[d] == -3) {
            dz[d] = -D2;
        } else if (dz[d] == 2) {
            dz[d] = D;
        } else if (dz[d] == -2) {
            dz[d] = -D;
        }
    }
    
    for (short x = 0; x < D; x++) {
        for (short y = 0; y < D; y++) {
            for (short z = 0; z < D; z++) {
                short i = index(x, y, z);
                front[i] = x * D + z;
                right[i] = D2 + y * D + z;
                coord[i][0] = x;
                coord[i][1] = y;
                coord[i][2] = z;
            }
        }
    }
    
    for (short d = 0; d < 24; d++) {
        for (short x = 0; x < D; x++) {
            for (short y = 0; y < D; y++) {
                for (short z = 0; z < D; z++) {
                    short i = 0;
                    if (dx[d] > 0) {
                        i += x * dx[d];
                    } else {
                        i -= (D - 1 - x) * dx[d];
                    }
                    if (dy[d] > 0) {
                        i += y * dy[d];
                    } else {
                        i -= (D - 1 - y) * dy[d];
                    }
                    if (dz[d] > 0) {
                        i += z * dz[d];
                    } else {
                        i -= (D - 1 - z) * dz[d];
                    }
                    mapping[d][index(x, y, z)] = i;
                    rmapping[d][i] = index(x, y, z);
                }
            }
        }
    }
    
    for (short x = 0; x < D; x++) {
        for (short y = 0; y < D; y++) {
            for (short z = 0; z < D; z++) {
                if (f1[z][x] > 0 && r1[z][y] > 0) {
                    short i = index(x, y, z);
                    ok1[i] = true;
                    possible1.push_back(i);
                    possible_side1[x * D + z].push_back(i);
                    possible_side1[D2 + y * D + z].push_back(i);
                    empty1.add(i);
                    for (short dx = -(D - 1 - x); dx <= x; dx++) {
                        for (short dy = -(D - 1 - y); dy <= y; dy++) {
                            for (short dz = -(D - 1 - z); dz <= z; dz++) {
                                possible[dx + D][dy + D][dz + D].push_back(i);
                            }
                        }
                    }
                }
            }
        }
    }
    for (short x = 0; x < D; x++) {
        for (short y = 0; y < D; y++) {
            for (short z = 0; z < D; z++) {
                if (f2[z][x] > 0 && r2[z][y] > 0) {
                    short i = index(x, y, z);
                    ok2[i] = true;
                    possible2.push_back(i);
                    possible_side2[x * D + z].push_back(i);
                    possible_side2[D2 + y * D + z].push_back(i);
                    empty2.add(i);
                }
            }
        }
    }
    empty1.save();
    empty2.save();
    
    for (short z = 0; z < D; z++) {
        for (short x = 0; x < D; x++) {
            if (f1[z][x] > 0) unfilled1.add(x * D + z);
        }
    }
    for (short z = 0; z < D; z++) {
        for (short y = 0; y < D; y++) {
            if (r1[z][y] > 0) unfilled1.add(D2 + y * D + z);
        }
    }
    for (short z = 0; z < D; z++) {
        for (short x = 0; x < D; x++) {
            if (f2[z][x] > 0) unfilled2.add(x * D + z);
        }
    }
    for (short z = 0; z < D; z++) {
        for (short y = 0; y < D; y++) {
            if (r2[z][y] > 0) unfilled2.add(D2 + y * D + z);
        }
    }
    unfilled1.save();
    unfilled2.save();
    
    for (short x = 0; x < D; x++) {
        for (short y = 0; y < D; y++) {
            for (short z = 0; z < D; z++) {
                short i = index(x, y, z);
                if (!ok1[i]) continue;
                if (x + 1 < D && ok1[index(x + 1, y, z)]) neighbors1[i].push_back(index(x + 1, y, z));
                if (x > 0 && ok1[index(x - 1, y, z)]) neighbors1[i].push_back(index(x - 1, y, z));
                if (y + 1 < D && ok1[index(x, y + 1, z)]) neighbors1[i].push_back(index(x, y + 1, z));
                if (y > 0 && ok1[index(x, y - 1, z)]) neighbors1[i].push_back(index(x, y - 1, z));
                if (z + 1 < D && ok1[index(x, y, z + 1)]) neighbors1[i].push_back(index(x, y, z + 1));
                if (z > 0 && ok1[index(x, y, z - 1)]) neighbors1[i].push_back(index(x, y, z - 1));
            }
        }
    }
    for (short x = 0; x < D; x++) {
        for (short y = 0; y < D; y++) {
            for (short z = 0; z < D; z++) {
                short i = index(x, y, z);
                if (!ok2[i]) continue;
                if (x + 1 < D && ok2[index(x + 1, y, z)]) neighbors2[i].push_back(index(x + 1, y, z));
                if (x > 0 && ok2[index(x - 1, y, z)]) neighbors2[i].push_back(index(x - 1, y, z));
                if (y + 1 < D && ok2[index(x, y + 1, z)]) neighbors2[i].push_back(index(x, y + 1, z));
                if (y > 0 && ok2[index(x, y - 1, z)]) neighbors2[i].push_back(index(x, y - 1, z));
                if (z + 1 < D && ok2[index(x, y, z + 1)]) neighbors2[i].push_back(index(x, y, z + 1));
                if (z > 0 && ok2[index(x, y, z - 1)]) neighbors2[i].push_back(index(x, y, z - 1));
            }
        }
    }
    
    for (short i = 0; i < D3; i++) available.push_back(&pool[i]);
    
    for (unsigned char i = 255; i > 0; i--) numbers.push_back(i);
    
    for (short i = 1; i < D3; i++) inv[i] = 1.0 / i;
}

void count_filled(block& b) {
    b.cnt1 = b.cnt2 = 0;
    for (short i1 : b.index1) {
        if (cnt1[front[i1]]++ == 0) b.cnt1++;
        if (cnt1[right[i1]]++ == 0) b.cnt1++;
        short i2 = mapping[b.d][i1 - b.di];
        if (cnt2[front[i2]]++ == 0) b.cnt2++;
        if (cnt2[right[i2]]++ == 0) b.cnt2++;
    }
    for (short i1 : b.index1) {
        cnt1[front[i1]]--;
        cnt1[right[i1]]--;
        short i2 = mapping[b.d][i1 - b.di];
        cnt2[front[i2]]--;
        cnt2[right[i2]]--;
    }
}

bool is_filled(const block& b) {
    for (short i1 : b.index1) {
        if (cnt1[front[i1]] == 0) return true;
        if (cnt1[right[i1]] == 0) return true;
        short i2 = mapping[b.d][i1 - b.di];
        if (cnt2[front[i2]] == 0) return true;
        if (cnt2[right[i2]] == 0) return true;
    }
    return false;
}

void get_connected(block& b, short i, int start) {
    static short stack[MAX_D3];
    short pos = 1;
    tmp_b[i] = tmp_num;
    b.add(i);
    stack[0] = i;
    while (pos > 0) {
        i = stack[--pos];
        for (short j : neighbors1[i]) {
            if (tmp_b[j] == start) {
                tmp_b[j] = tmp_num;
                b.add(j);
                stack[pos++] = j;
            }
        }
    }
}

bool separate_blocks(const block& b, short i, block& deleted, vector<block*>& new_blocks, short limit) {
    if (b.size() <= limit) return false;
    tmp_num++;
    for (short i1 : b.index1) tmp_b[i1] = tmp_num;
    tmp_b[i] = 0;
    deleted.add(i);
    int start = tmp_num, degree = 0;
    for (short j : neighbors1[i]) {
        if (tmp_b[j] == start) degree++;
    }
    if (degree == 1) return true;
    for (short j : neighbors1[i]) {
        if (tmp_b[j] == start) {
            tmp_num++;
            block* nb = get_block(b);
            get_connected(*nb, j, start);
            if (nb->size() < limit) {
                for (short i1 : nb->index1) deleted.add(i1);
                return_block(nb);
            } else {
                new_blocks.push_back(nb);
            }
        }
    }
    if (deleted.size() == 1 && new_blocks.size() == 1) {
        return_block(new_blocks[0]);
        new_blocks.clear();
    }
    return deleted.size() != b.size();
}

map<unsigned char, block*> calc_greedy(short limit) {
    vector<short> v;
    for (short d = 0; d < 24; d++) {
        for (short dx = -D + 1; dx < D; dx++) {
            for (short dy = -D + 1; dy < D; dy++) {
                for (short dz = -D + 1; dz < D; dz++) {
                    short di = index(dx, dy, dz);
                    tmp_num++;
                    v.clear();
                    for (short i1 : possible[dx + D][dy + D][dz + D]) {
                        short i2 = mapping[d][i1 - di];
                        if (!ok2[i2]) continue;
                        tmp_b[i1] = tmp_num;
                        v.push_back(i1);
                    }
                    if (v.size() <= 2) continue;
                    int start = tmp_num;
                    for (short i : v) {
                        if (tmp_b[i] == start) {
                            block b(d, di);
                            tmp_num++;
                            get_connected(b, i, start);
                            if (b.size() <= 2) continue;
                            if (b.size() < limit) {
                                small_blocks.push_back(b);
                            } else {
                                count_filled(b);
                                all_blocks.push_back(b);
                            }
                        }
                    }
                }
            }
        }
    }
    
    sort(all_blocks.begin(), all_blocks.end());
    for (int i = 0; i < all_blocks.size(); i++) {
        for (short i1 : all_blocks[i].index1) {
            blocks1[i1].push_back(i);
            blocks2[mapping[all_blocks[i].d][i1 - all_blocks[i].di]].push_back(i);
        }
    }
    
    map<unsigned char, block*> state;
    for (const block& b : all_blocks) {
        tmp_num++;
        short cnt = 0;
        for (short i1 : b.index1) {
            if (b1[i1] == 0 && b2[mapping[b.d][i1 - b.di]] == 0) {
                tmp_b[i1] = tmp_num;
                cnt++;
            }
        }
        if (cnt < limit) continue;
        int start = tmp_num;
        for (short i : b.index1) {
            if (tmp_b[i] == start) {
                block* nb = get_block(b);
                tmp_num++;
                get_connected(*nb, i, start);
                if (nb->size() < limit) {
                    return_block(nb);
                    continue;
                }
                if (is_filled(*nb)) {
                    unsigned char num = get_number();
                    for (short i1 : nb->index1) {
                        use1(i1, num);
                        use2(mapping[nb->d][i1 - nb->di], num);
                    }
                    state[num] = nb;
                } else {
                    return_block(nb);
                }
            }
        }
    }
    return state;
}

void reset() {
    memcpy(b1, best_b1, sizeof(unsigned char) * D3);
    memset(cnt1, 0, sizeof(char) * D2 * 2);
    empty1.load();
    unfilled1.load();
    for (short i1 : possible1) {
        if (b1[i1] > 0) use1(i1, b1[i1]);
    }
    memcpy(b2, best_b2, sizeof(unsigned char) * D3);
    memset(cnt2, 0, sizeof(char) * D2 * 2);
    empty2.load();
    unfilled2.load();
    for (short i2 : possible2) {
        if (b2[i2] > 0) use2(i2, b2[i2]);
    }
    set<unsigned char> s;
    for (const record& r : records) s.insert(r.num);
    numbers.clear();
    for (unsigned char i = 255; i > 0; i--) {
        if (!s.count(i)) numbers.push_back(i);
    }
}

void save_best(const map<unsigned char, block*>& state) {
    memcpy(best_b1, b1, sizeof(unsigned char) * D3);
    memcpy(best_b2, b2, sizeof(unsigned char) * D3);
    records.clear();
    for (const pair<unsigned char, block*>& p : state) records.emplace_back(p.first, p.second->d, p.second->di);
}

void reset_best(map<unsigned char, block*>& state) {
    reset();
    for (const pair<unsigned char, block*>& p : state) return_block(p.second);
    state.clear();
    for (const record& r : records) {
        block* b = get_block(all_blocks[0]);
        b->d = r.d;
        b->di = r.di;
        state[r.num] = b;
    }
    for (short i1 : possible1) {
        if (b1[i1] > 0) state[b1[i1]]->add(i1);
    }
}

void fill_two() {
    struct block_two {
        char cnt;
        short i1;
        short i2;
        
        block_two() {}
        block_two(char cnt, short i1, short i2) : cnt(cnt), i1(i1), i2(i2) {}
        
        bool operator<(const block_two& b) const {
            return cnt < b.cnt;
        }
    };
    
    priority_queue<block_two> pq1;
    for (short i1 : possible1) {
        if (b1[i1] > 0) continue;
        for (short i2 : neighbors1[i1]) {
            if (i1 > i2 || b1[i2] > 0) continue;
            char cnt = 0;
            if (cnt1[front[i1]] == 0) cnt++;
            if (cnt1[right[i1]] == 0) cnt++;
            if (front[i1] != front[i2] && cnt1[front[i2]] == 0) cnt++;
            if (right[i1] != right[i2] && cnt1[right[i2]] == 0) cnt++;
            pq1.emplace(cnt, i1, i2);
        }
    }
    priority_queue<block_two> pq2;
    for (short i1 : possible2) {
        if (b2[i1] > 0) continue;
        for (short i2 : neighbors2[i1]) {
            if (i1 > i2 || b2[i2] > 0) continue;
            char cnt = 0;
            if (cnt2[front[i1]] == 0) cnt++;
            if (cnt2[right[i1]] == 0) cnt++;
            if (front[i1] != front[i2] && cnt2[front[i2]] == 0) cnt++;
            if (right[i1] != right[i2] && cnt2[right[i2]] == 0) cnt++;
            pq2.emplace(cnt, i1, i2);
        }
    }
    
    while (true) {
        bool f1 = false;
        block_two bl1;
        while (!pq1.empty()) {
            bl1 = pq1.top();
            pq1.pop();
            if (b1[bl1.i1] > 0 || b1[bl1.i2] > 0) continue;
            char cnt = 0;
            if (cnt1[front[bl1.i1]] == 0) cnt++;
            if (cnt1[right[bl1.i1]] == 0) cnt++;
            if (front[bl1.i1] != front[bl1.i2] && cnt1[front[bl1.i2]] == 0) cnt++;
            if (right[bl1.i1] != right[bl1.i2] && cnt1[right[bl1.i2]] == 0) cnt++;
            if (cnt == bl1.cnt) {
                f1 = true;
                break;
            } else {
                pq1.emplace(cnt, bl1.i1, bl1.i2);
            }
        }
        if (!f1) break;
        
        bool f2 = false;
        block_two bl2;
        while (!pq2.empty()) {
            bl2 = pq2.top();
            pq2.pop();
            if (b2[bl2.i1] > 0 || b2[bl2.i2] > 0) continue;
            char cnt = 0;
            if (cnt2[front[bl2.i1]] == 0) cnt++;
            if (cnt2[right[bl2.i1]] == 0) cnt++;
            if (front[bl2.i1] != front[bl2.i2] && cnt2[front[bl2.i2]] == 0) cnt++;
            if (right[bl2.i1] != right[bl2.i2] && cnt2[right[bl2.i2]] == 0) cnt++;
            if (cnt == bl2.cnt) {
                f2 = true;
                break;
            } else {
                pq2.emplace(cnt, bl2.i1, bl2.i2);
            }
        }
        if (!f2) break;
        
        if (bl1.cnt + bl2.cnt == 0) break;
        unsigned char num = get_number();
        use1(bl1.i1, num);
        use1(bl1.i2, num);
        use2(bl2.i1, num);
        use2(bl2.i2, num);
    }
}

void fill_one() {
    struct block_one {
        char cnt;
        short i;
        
        block_one() {}
        block_one(char cnt, short i) : cnt(cnt), i(i) {}
        
        bool operator<(const block_one& b) const {
            return cnt < b.cnt;
        }
    };
    
    vector<unsigned char> numbers;
    priority_queue<block_one> pq1;
    for (short i : possible1) {
        if (b1[i] > 0) continue;
        char cnt = 0;
        if (cnt1[front[i]] == 0) cnt++;
        if (cnt1[right[i]] == 0) cnt++;
        if (cnt > 0) pq1.emplace(cnt, i);
    }
    while (!pq1.empty()) {
        block_one b = pq1.top();
        pq1.pop();
        char cnt = 0;
        if (cnt1[front[b.i]] == 0) cnt++;
        if (cnt1[right[b.i]] == 0) cnt++;
        if (cnt == b.cnt) {
            unsigned char num = get_number();
            use1(b.i, num);
            numbers.push_back(num);
        } else if (cnt > 0) {
            pq1.emplace(cnt, b.i);
        }
        
    }
    
    priority_queue<block_one> pq2;
    for (short i : possible2) {
        if (b2[i] > 0) continue;
        char cnt = 0;
        if (cnt2[front[i]] == 0) cnt++;
        if (cnt2[right[i]] == 0) cnt++;
        if (cnt > 0) pq2.emplace(cnt, i);
    }
    while (!pq2.empty()) {
        block_one b = pq2.top();
        pq2.pop();
        char cnt = 0;
        if (cnt2[front[b.i]] == 0) cnt++;
        if (cnt2[right[b.i]] == 0) cnt++;
        if (cnt == b.cnt) {
            unsigned char num;
            if (!numbers.empty()) {
                num = numbers.back();
                numbers.pop_back();
            } else {
                num = get_number();
            }
            use2(b.i, num);
        } else if (cnt > 0) {
            pq2.emplace(cnt, b.i);
        }
    }
}

short rename() {
    unsigned char n = 0;
    map<unsigned char, unsigned char> mp;
    for (short i = 0; i < D3; i++) {
        if (b1[i] == 0) continue;
        if (!mp.count(b1[i])) mp[b1[i]] = ++n;
        b1[i] = mp[b1[i]];
    }
    for (short i = 0; i < D3; i++) {
        if (b2[i] == 0) continue;
        if (!mp.count(b2[i])) mp[b2[i]] = ++n;
        b2[i] = mp[b2[i]];
    }
    return n;
}

long long calc_score(short n) {
    map<unsigned char, short> count1, count2;
    for (short i = 0; i < D3; i++) {
        if (b1[i] > 0) count1[b1[i]]++;
    }
    for (short i = 0; i < D3; i++) {
        if (b2[i] > 0) count2[b2[i]]++;
    }
    double sum = 0;
    for (unsigned char i = 1; i <= n; i++) {
        if (count1.count(i) && count2.count(i)) {
            sum += inv[count1[i]];
        } else if (count1.count(i)) {
            sum += count1[i];
        } else {
            sum += count2[i];
        }
    }
    return round(sum * 1e9);
}

int main() {
    simulated_annealing sa;
    
    input();
    
    precalc();
    
    short limit = min(D, 9);
    map<unsigned char, block*> state = calc_greedy(limit);
    double best_score = unfilled1.size() + unfilled2.size();
    for (const pair<unsigned char, block*>& p : state) best_score += inv[p.second->size()];
    save_best(state);
    double current_score = best_score;
    unsigned short unchanged = 0;
    vector<unsigned char> numbers;
    vector<block*> new_blocks;
    sa.set_time_limit(pow(D, 0.63));
    sa.set_start_temp(pow(D, 1.5) / 200.0, pow(D, 1.7) / 15.0);
    while (!sa.end1()) {
        if (++unchanged == 0 && current_score != best_score) {
            current_score = best_score;
            reset_best(state);
        }
        if (random::get(100) < 70) {
            short index1, index2;
            unsigned char num, deleted_num = 0;
            bool delete_all;
            double next_score = current_score;
            block* target;
            block* now;
            block* deleted;
            int r = random::get(100);
            if (r < 50) {
                if (r < 30) {
                    if (unfilled1.empty()) continue;
                    index1 = get_unfilled1();
                } else {
                    if (empty1.empty()) continue;
                    index1 = empty1.get();
                }
                for (short i : neighbors1[index1]) {
                    if (b1[i] > 0) {
                        block* b = state[b1[i]];
                        if (valid1(*b, index1)) numbers.push_back(b1[i]);
                    }
                }
                if (numbers.empty()) continue;
                num = numbers[random::get(numbers.size())];
                target = state[num];
                index2 = mapping[target->d][index1 - target->di];
                if (b2[index2] > 0) {
                    deleted_num = b2[index2];
                    now = state[deleted_num];
                    deleted = get_block(*now);
                    delete_all = !separate_blocks(*now, rmapping[now->d][index2] + now->di, *deleted, new_blocks, limit);
                    if (delete_all) {
                        return_block(deleted);
                        deleted = now;
                    }
                }
            } else {
                if (r < 80) {
                    if (unfilled2.empty()) continue;
                    index2 = get_unfilled2();
                } else {
                    if (empty2.empty()) continue;
                    index2 = empty2.get();
                }
                for (short i : neighbors2[index2]) {
                    if (b2[i] > 0) {
                        block* b = state[b2[i]];
                        if (valid2(*b, index2)) numbers.push_back(b2[i]);
                    }
                }
                if (numbers.empty()) continue;
                num = numbers[random::get(numbers.size())];
                target = state[num];
                index1 = rmapping[target->d][index2] + target->di;
                if (b1[index1] > 0) {
                    deleted_num = b1[index1];
                    now = state[deleted_num];
                    deleted = get_block(*now);
                    delete_all = !separate_blocks(*now, index1, *deleted, new_blocks, limit);
                    if (delete_all) {
                        return_block(deleted);
                        deleted = now;
                    }
                }
            }
            numbers.clear();
            if (deleted_num > 0) {
                next_score -= inv[now->size()] + unfilled1.size() + unfilled2.size();
                for (short i1 : deleted->index1) {
                    unuse1(i1);
                    unuse2(mapping[deleted->d][i1 - deleted->di]);
                }
                next_score += unfilled1.size() + unfilled2.size();
                if (!delete_all) {
                    if (new_blocks.empty()) {
                        next_score += inv[now->size() - 1];
                    } else {
                        for (const block* nb : new_blocks) next_score += inv[nb->size()];
                    }
                }
            }
            next_score += inv[target->size() + 1] - inv[target->size()];
            if (cnt1[front[index1]] == 0) next_score -= 1;
            if (cnt1[right[index1]] == 0) next_score -= 1;
            if (cnt2[front[index2]] == 0) next_score -= 1;
            if (cnt2[right[index2]] == 0) next_score -= 1;
            if (sa.accept1(current_score, next_score)) {
                current_score = next_score;
                target->add(index1);
                use1(index1, num);
                use2(index2, num);
                if (deleted_num > 0) {
                    if (delete_all) {
                        return_block(now);
                        return_number(deleted_num);
                        state.erase(deleted_num);
                    } else if (new_blocks.empty()) {
                        now->remove(deleted->index1[0]);
                    } else {
                        return_block(now);
                        return_number(deleted_num);
                        state.erase(deleted_num);
                        for (block* nb : new_blocks) {
                            unsigned char num = get_number();
                            for (short i1 : nb->index1) {
                                b1[i1] = num;
                                b2[mapping[nb->d][i1 - nb->di]] = num;
                            }
                            state[num] = nb;
                        }
                        new_blocks.clear();
                    }
                }
            } else {
                if (deleted_num > 0) {
                    for (short i1 : deleted->index1) {
                        use1(i1, deleted_num);
                        use2(mapping[deleted->d][i1 - deleted->di], deleted_num);
                    }
                }
            }
            if (deleted_num > 0 && !delete_all) return_block(deleted);
            if (!new_blocks.empty()) {
                for (block* nb : new_blocks) return_block(nb);
                new_blocks.clear();
            }
        } else {
            short index1, index2;
            unsigned char num1, num2;
            block* target;
            int r = random::get(100);
            if (r < 5) {
                if (unfilled1.empty()) continue;
                index1 = get_unfilled1();
                if (blocks1[index1].empty()) continue;
                num1 = b1[index1];
                target = &all_blocks[blocks1[index1][random::get(blocks1[index1].size())]];
                index2 = mapping[target->d][index1 - target->di];
                num2 = b2[index2];
            } else if (r < 10) {
                if (unfilled2.empty()) continue;
                index2 = get_unfilled2();
                if (blocks2[index2].empty()) continue;
                num2 = b2[index2];
                target = &all_blocks[blocks2[index2][random::get(blocks2[index2].size())]];
                index1 = rmapping[target->d][index2] + target->di;
                num1 = b1[index1];
            } else {
                index1 = possible1[random::get(possible1.size())];
                if (blocks1[index1].empty()) continue;
                num1 = b1[index1];
                target = &all_blocks[blocks1[index1][random::get(blocks1[index1].size())]];
                index2 = mapping[target->d][index1 - target->di];
                num2 = b2[index2];
            }
            if (num1 > 0 && num2 > 0 && num1 != num2) continue;
            short num = max(num1, num2);
            double next_score = current_score;
            block* deleted;
            if (num > 0) {
                deleted = state[num];
                next_score -= inv[deleted->size()] + unfilled1.size() + unfilled2.size();
                for (short i1 : deleted->index1) {
                    unuse1(i1);
                    unuse2(mapping[deleted->d][i1 - deleted->di]);
                }
                next_score += unfilled1.size() + unfilled2.size();
            }
            tmp_num++;
            short cnt = 0;
            for (short i1 : target->index1) {
                if (b1[i1] == 0 && b2[mapping[target->d][i1 - target->di]] == 0) {
                    tmp_b[i1] = tmp_num;
                    cnt++;
                }
            }
            block* added = get_block(*target);
            bool add = false;
            if (cnt >= limit) {
                tmp_num++;
                get_connected(*added, index1, tmp_num - 1);
                if (added->size() >= limit) {
                    count_filled(*added);
                    if (added->cnt1 + added->cnt2 > 0) add = true;
                }
            }
            if (add) {
                next_score += inv[added->size()];
                next_score -= added->cnt1;
                next_score -= added->cnt2;
            } else if (num == 0) {
                return_block(added);
                continue;
            }
            if (sa.accept2(current_score, next_score)) {
                current_score = next_score;
                if (add) {
                    unsigned char num = get_number();
                    for (short i1 : added->index1) {
                        use1(i1, num);
                        use2(mapping[added->d][i1 - added->di], num);
                    }
                    state[num] = added;
                } else {
                    return_block(added);
                }
                if (num > 0) {
                    return_block(deleted);
                    return_number(num);
                    state.erase(num);
                }
            } else {
                return_block(added);
                if (num > 0) {
                    for (short i1 : deleted->index1) {
                        use1(i1, num);
                        use2(mapping[deleted->d][i1 - deleted->di], num);
                    }
                }
            }
        }
        if (current_score < best_score) {
            unchanged = 0;
            best_score = current_score;
            save_best(state);
        }
    }
    
    limit = 3;
    for (const block& b : small_blocks) {
        int pos = all_blocks.size();
        all_blocks.push_back(b);
        for (short i1 : b.index1) {
            blocks1[i1].push_back(pos);
            blocks2[mapping[b.d][i1 - b.di]].push_back(pos);
        }
    }
    while (!sa.end2()) {
        if (++unchanged == 0 && current_score != best_score) {
            current_score = best_score;
            reset_best(state);
        }
        if (random::get(100) < 70) {
            short index1, index2;
            unsigned char num, deleted_num = 0;
            bool delete_all;
            double next_score = current_score;
            block* target;
            block* now;
            block* deleted;
            int r = random::get(100);
            if (r < 50) {
                if (r < 30) {
                    if (unfilled1.empty()) continue;
                    index1 = get_unfilled1();
                } else {
                    if (empty1.empty()) continue;
                    index1 = empty1.get();
                }
                for (short i : neighbors1[index1]) {
                    if (b1[i] > 0) {
                        block* b = state[b1[i]];
                        if (valid1(*b, index1)) numbers.push_back(b1[i]);
                    }
                }
                if (numbers.empty()) continue;
                num = numbers[random::get(numbers.size())];
                target = state[num];
                index2 = mapping[target->d][index1 - target->di];
                if (b2[index2] > 0) {
                    deleted_num = b2[index2];
                    now = state[deleted_num];
                    deleted = get_block(*now);
                    delete_all = !separate_blocks(*now, rmapping[now->d][index2] + now->di, *deleted, new_blocks, limit);
                    if (delete_all) {
                        return_block(deleted);
                        deleted = now;
                    }
                }
            } else {
                if (r < 80) {
                    if (unfilled2.empty()) continue;
                    index2 = get_unfilled2();
                } else {
                    if (empty2.empty()) continue;
                    index2 = empty2.get();
                }
                for (short i : neighbors2[index2]) {
                    if (b2[i] > 0) {
                        block* b = state[b2[i]];
                        if (valid2(*b, index2)) numbers.push_back(b2[i]);
                    }
                }
                if (numbers.empty()) continue;
                num = numbers[random::get(numbers.size())];
                target = state[num];
                index1 = rmapping[target->d][index2] + target->di;
                if (b1[index1] > 0) {
                    deleted_num = b1[index1];
                    now = state[deleted_num];
                    deleted = get_block(*now);
                    delete_all = !separate_blocks(*now, index1, *deleted, new_blocks, limit);
                    if (delete_all) {
                        return_block(deleted);
                        deleted = now;
                    }
                }
            }
            numbers.clear();
            if (deleted_num > 0) {
                next_score -= inv[now->size()] + unfilled1.size() + unfilled2.size();
                for (short i1 : deleted->index1) {
                    unuse1(i1);
                    unuse2(mapping[deleted->d][i1 - deleted->di]);
                }
                next_score += unfilled1.size() + unfilled2.size();
                if (!delete_all) {
                    if (new_blocks.empty()) {
                        next_score += inv[now->size() - 1];
                    } else {
                        for (const block* nb : new_blocks) next_score += inv[nb->size()];
                    }
                }
            }
            next_score += inv[target->size() + 1] - inv[target->size()];
            if (cnt1[front[index1]] == 0) next_score -= 1;
            if (cnt1[right[index1]] == 0) next_score -= 1;
            if (cnt2[front[index2]] == 0) next_score -= 1;
            if (cnt2[right[index2]] == 0) next_score -= 1;
            if (sa.accept1(current_score, next_score)) {
                current_score = next_score;
                target->add(index1);
                use1(index1, num);
                use2(index2, num);
                if (deleted_num > 0) {
                    if (delete_all) {
                        return_block(now);
                        return_number(deleted_num);
                        state.erase(deleted_num);
                    } else if (new_blocks.empty()) {
                        now->remove(deleted->index1[0]);
                    } else {
                        return_block(now);
                        return_number(deleted_num);
                        state.erase(deleted_num);
                        for (block* nb : new_blocks) {
                            unsigned char num = get_number();
                            for (short i1 : nb->index1) {
                                b1[i1] = num;
                                b2[mapping[nb->d][i1 - nb->di]] = num;
                            }
                            state[num] = nb;
                        }
                        new_blocks.clear();
                    }
                }
            } else {
                if (deleted_num > 0) {
                    for (short i1 : deleted->index1) {
                        use1(i1, deleted_num);
                        use2(mapping[deleted->d][i1 - deleted->di], deleted_num);
                    }
                }
            }
            if (deleted_num > 0 && !delete_all) return_block(deleted);
            if (!new_blocks.empty()) {
                for (block* nb : new_blocks) return_block(nb);
                new_blocks.clear();
            }
        } else {
            short index1, index2;
            unsigned char num1, num2;
            block* target;
            int r = random::get(100);
            if (r < 5) {
                if (unfilled1.empty()) continue;
                index1 = get_unfilled1();
                if (blocks1[index1].empty()) continue;
                num1 = b1[index1];
                target = &all_blocks[blocks1[index1][random::get(blocks1[index1].size())]];
                index2 = mapping[target->d][index1 - target->di];
                num2 = b2[index2];
            } else if (r < 10) {
                if (unfilled2.empty()) continue;
                index2 = get_unfilled2();
                if (blocks2[index2].empty()) continue;
                num2 = b2[index2];
                target = &all_blocks[blocks2[index2][random::get(blocks2[index2].size())]];
                index1 = rmapping[target->d][index2] + target->di;
                num1 = b1[index1];
            } else {
                index1 = possible1[random::get(possible1.size())];
                if (blocks1[index1].empty()) continue;
                num1 = b1[index1];
                target = &all_blocks[blocks1[index1][random::get(blocks1[index1].size())]];
                index2 = mapping[target->d][index1 - target->di];
                num2 = b2[index2];
            }
            if (num1 > 0 && num2 > 0 && num1 != num2) continue;
            short num = max(num1, num2);
            double next_score = current_score;
            block* deleted;
            if (num > 0) {
                deleted = state[num];
                next_score -= inv[deleted->size()] + unfilled1.size() + unfilled2.size();
                for (short i1 : deleted->index1) {
                    unuse1(i1);
                    unuse2(mapping[deleted->d][i1 - deleted->di]);
                }
                next_score += unfilled1.size() + unfilled2.size();
            }
            tmp_num++;
            short cnt = 0;
            for (short i1 : target->index1) {
                if (b1[i1] == 0 && b2[mapping[target->d][i1 - target->di]] == 0) {
                    tmp_b[i1] = tmp_num;
                    cnt++;
                }
            }
            block* added = get_block(*target);
            bool add = false;
            if (cnt >= limit) {
                tmp_num++;
                get_connected(*added, index1, tmp_num - 1);
                if (added->size() >= limit) {
                    count_filled(*added);
                    if (added->cnt1 + added->cnt2 > 0) add = true;
                }
            }
            if (add) {
                next_score += inv[added->size()];
                next_score -= added->cnt1;
                next_score -= added->cnt2;
            } else if (num == 0) {
                return_block(added);
                continue;
            }
            if (sa.accept2(current_score, next_score)) {
                current_score = next_score;
                if (add) {
                    unsigned char num = get_number();
                    for (short i1 : added->index1) {
                        use1(i1, num);
                        use2(mapping[added->d][i1 - added->di], num);
                    }
                    state[num] = added;
                } else {
                    return_block(added);
                }
                if (num > 0) {
                    return_block(deleted);
                    return_number(num);
                    state.erase(num);
                }
            } else {
                return_block(added);
                if (num > 0) {
                    for (short i1 : deleted->index1) {
                        use1(i1, num);
                        use2(mapping[deleted->d][i1 - deleted->di], num);
                    }
                }
            }
        }
        if (current_score < best_score) {
            unchanged = 0;
            best_score = current_score;
            save_best(state);
        }
    }
    
    reset();
    fill_two();
    fill_one();
    short n = rename();
    output(n);
    
    fprintf(stderr, "%lld\n", calc_score(n));
    fflush(stderr);
    
    quick_exit(0);
    
    return 0;
}
