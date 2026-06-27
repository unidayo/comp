#pragma GCC optimize("unroll-loops")

#pragma GCC optimize("O3,inline")  // Ofast = O3,fast-math,allow-store-data-races,no-protect-parens


#pragma GCC target("bmi,bmi2,lzcnt,popcnt")                       // bit manipulation
#pragma GCC target("movbe")                                       // byte swap
#pragma GCC target("aes,pclmul,rdrnd")                            // encryption
#pragma GCC target("avx,avx2,f16c,fma,sse3,ssse3,sse4.1,sse4.2")  // SIMD

#undef _GLIBCXX_DEBUG  // disable run-time bound checking, etc */
#define NDEBUG

#define FLAG_PRINT_SCORE
#define FLAG_PRINT_SCORE_END

// ---------------------------------------- template ----------------------------------------

// #define _GLIBCXX_DEBUG
#include <bits/stdc++.h>
#include <unistd.h>
#include <x86intrin.h>

/* #include "atcoder/all" */
#include "atcoder/mincostflow.hpp"
using namespace atcoder;

// #define FLAG_CERR

using namespace std;
using std::cin;
using std::cout;
using ll = long long;
using ull = unsigned long long;

#define bit(n, k) (((n) >> (k)) & 1) /*nのk bit目*/

#ifdef ONLINE_JUDGE
const double TIME_RATIO = 1.0;
#else
const double TIME_RATIO = 2.0;
#endif

constexpr int INF = 1000000000;
constexpr ll INFLL = 1ll << 60;
/* constexpr int TIME_LIMIT = 100; */
constexpr int TIME_LIMIT = 3900;
// constexpr int TIME_LIMIT = 39000000;
constexpr int TIME_USE_KL = 300;
constexpr double PI = 3.14159265359;

template <class T>
bool chmin(T& a, const T& b) {
    if (a > b) {
        a = b;
        return 1;
    } else
        return 0;
}
template <class T>
bool chmax(T& a, const T& b) {
    if (a < b) {
        a = b;
        return 1;
    } else
        return 0;
}

template <typename A, size_t N, typename T>
void Fill(A (&array)[N], const T& val) {
    std::fill((T*)array, (T*)(array + N), val);
}

auto start = chrono::system_clock::now();
int get_time() {
    int elapsed = (int)(TIME_RATIO * double(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start).count()));
    return elapsed;
}
int get_time_remaining() {
    int time_remaining = TIME_LIMIT - get_time();
    return time_remaining;
}
void print_time() {
    int elapsed = get_time();
    cerr << "[DATA] totaltime = " << elapsed << endl;
}

static uint32_t randxor() {
    static uint32_t x = 123456789;
    static uint32_t y = 362436069;
    static uint32_t z = 521288629;
    static uint32_t w = 88675123;
    uint32_t t;

    t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}

int randrange(int n) {
    assert(0 <= n);
    // return (randxor() / 123) % n;
    return randxor() % n;
}

int randrange(int l, int r) {
    return l + randxor() % (r - l);
}

// ---------------------------------------- embedded ----------------------------------------
bool use_embedded = bool(1);
int argc_memo[41][30] = {{8, 8, 8, 11, 8, 8, 8, 5, 5, 17, 5, 8, 11, 8, 5, 1, 5, 5, 5, 6, 14, 6, 6, 6, 6, 6, 6, 6, 6, 8}, {8, 11, 11, 8, 8, 11, 8, 5, 8, 11, 5, 5, 11, 5, 5, 1, 5, 8, 5, 6, 6, 17, 4, 6, 6, 11, 6, 6, 8, 8}, {8, 8, 11, 8, 11, 8, 5, 8, 8, 8, 5, 14, 5, 8, 1, 8, 11, 6, 6, 11, 4, 6, 1, 6, 6, 6, 8, 20, 17, 14}, {8, 14, 8, 8, 17, 8, 8, 11, 5, 8, 5, 5, 8, 5, 5, 8, 14, 6, 6, 4, 6, 1, 6, 11, 4, 8, 6, 20, 8, 8}, {1, 11, 14, 11, 11, 8, 8, 8, 8, 8, 14, 14, 8, 8, 1, 1, 6, 6, 6, 1, 14, 8, 1, 6, 6, 6, 6, 8, 6, 10}, {11, 8, 8, 5, 8, 11, 8, 8, 8, 5, 1, 8, 11, 5, 8, 1, 8, 8, 8, 6, 6, 8, 11, 5, 17, 6, 11, 6, 14, 17}, {8, 8, 8, 8, 8, 11, 8, 8, 11, 8, 8, 8, 14, 11, 11, 6, 1, 6, 6, 6, 5, 8, 6, 6, 6, 6, 6, 14, 29, 10}, {8, 8, 8, 8, 8, 11, 8, 5, 8, 8, 8, 8, 8, 8, 5, 6, 6, 8, 11, 6, 5, 1, 6, 6, 1, 6, 1, 6, 8, 10}, {1, 5, 8, 11, 14, 8, 8, 8, 8, 11, 5, 1, 5, 8, 6, 1, 6, 11, 6, 14, 4, 4, 6, 1, 1, 17, 14, 6, 8, 12}, {1, 8, 8, 8, 8, 8, 14, 8, 8, 14, 11, 1, 8, 8, 8, 6, 1, 8, 5, 11, 1, 1, 4, 6, 1, 26, 26, 23, 23, 12}, {11, 8, 8, 14, 8, 11, 1, 11, 8, 5, 5, 8, 8, 5, 6, 6, 1, 11, 8, 5, 14, 20, 6, 6, 1, 8, 10, 8, 8, 10}, {11, 8, 11, 14, 11, 5, 11, 8, 8, 17, 11, 8, 8, 1, 1, 6, 6, 6, 6, 6, 8, 6, 4, 1, 8, 8, 8, 8, 10, 10}, {1, 11, 8, 8, 8, 8, 11, 8, 8, 8, 8, 11, 8, 14, 1, 8, 6, 6, 8, 4, 6, 6, 6, 6, 6, 8, 17, 10, 10, 10}, {1, 8, 8, 14, 11, 8, 11, 8, 8, 8, 8, 14, 5, 8, 8, 5, 1, 11, 8, 4, 6, 4, 6, 1, 6, 6, 10, 8, 8, 10}, {1, 8, 8, 14, 8, 11, 11, 8, 8, 8, 1, 5, 11, 8, 8, 11, 1, 5, 6, 23, 1, 1, 6, 6, 14, 6, 8, 23, 12, 12}, {1, 8, 11, 11, 14, 8, 11, 8, 14, 14, 8, 8, 5, 11, 8, 8, 1, 6, 11, 17, 1, 6, 6, 6, 8, 6, 10, 20, 10, 29}, {1, 1, 11, 11, 8, 11, 11, 8, 11, 8, 11, 8, 6, 8, 6, 1, 14, 6, 4, 4, 6, 4, 6, 6, 6, 6, 17, 8, 10, 10}, {1, 1, 8, 8, 5, 8, 11, 8, 8, 11, 1, 5, 5, 1, 11, 5, 4, 6, 6, 6, 8, 6, 17, 8, 8, 6, 10, 10, 12, 12}, {1, 1, 11, 8, 8, 11, 8, 14, 14, 11, 8, 8, 11, 11, 1, 5, 1, 11, 6, 6, 8, 6, 6, 6, 17, 8, 8, 8, 10, 12}, {1, 1, 5, 14, 8, 5, 1, 11, 8, 8, 8, 1, 11, 11, 14, 4, 6, 11, 6, 6, 1, 6, 6, 6, 8, 10, 17, 29, 12, 12}, {1, 14, 5, 11, 14, 14, 17, 8, 11, 8, 8, 8, 1, 1, 1, 11, 11, 17, 8, 6, 6, 20, 1, 6, 6, 8, 26, 10, 10, 26}, {1, 8, 8, 8, 11, 8, 8, 8, 11, 14, 1, 1, 6, 6, 1, 1, 4, 6, 1, 1, 6, 11, 1, 1, 8, 10, 10, 10, 12, 10}, {1, 17, 14, 8, 11, 8, 11, 8, 14, 11, 11, 11, 8, 8, 1, 8, 8, 11, 8, 23, 1, 6, 6, 1, 6, 10, 10, 12, 12, 12}, {1, 1, 11, 8, 8, 11, 8, 11, 11, 11, 14, 11, 8, 11, 6, 14, 8, 6, 8, 1, 6, 8, 8, 6, 20, 20, 10, 8, 10, 10}, {1, 1, 8, 14, 8, 8, 11, 8, 11, 11, 8, 8, 11, 8, 8, 11, 11, 8, 6, 8, 6, 6, 6, 6, 8, 10, 8, 10, 12, 20}, {1, 11, 14, 1, 8, 11, 11, 8, 14, 8, 8, 14, 14, 1, 11, 6, 6, 8, 8, 6, 6, 1, 6, 23, 8, 8, 12, 26, 12, 12}, {1, 1, 8, 11, 14, 11, 17, 14, 8, 11, 8, 11, 14, 8, 6, 11, 1, 6, 6, 6, 23, 6, 8, 8, 12, 8, 10, 12, 12, 12}, {1, 1, 11, 5, 8, 5, 8, 11, 11, 8, 8, 6, 8, 11, 11, 6, 6, 6, 6, 20, 6, 6, 6, 8, 8, 26, 14, 10, 8, 12}, {1, 1, 8, 8, 11, 8, 8, 11, 11, 8, 8, 8, 14, 14, 14, 11, 11, 8, 4, 17, 6, 6, 17, 8, 1, 10, 10, 10, 12, 12}, {1, 1, 8, 11, 8, 11, 8, 14, 8, 8, 11, 11, 11, 17, 1, 11, 6, 11, 14, 6, 6, 6, 6, 20, 8, 10, 10, 10, 12, 26}, {1, 1, 8, 8, 8, 8, 11, 8, 14, 11, 8, 14, 11, 11, 6, 1, 11, 4, 6, 6, 6, 17, 10, 6, 23, 10, 10, 12, 12, 29}, {1, 1, 8, 8, 8, 8, 11, 17, 14, 11, 11, 8, 11, 5, 8, 1, 1, 5, 6, 1, 8, 6, 11, 10, 8, 26, 8, 26, 26, 12}, {1, 1, 14, 8, 11, 8, 11, 14, 8, 14, 8, 11, 8, 8, 14, 14, 11, 6, 14, 6, 1, 6, 8, 8, 8, 29, 26, 10, 12, 12}, {1, 1, 11, 14, 8, 11, 14, 8, 11, 14, 8, 8, 14, 6, 6, 6, 17, 17, 6, 20, 5, 11, 8, 6, 8, 10, 10, 10, 12, 12}, {1, 1, 14, 17, 11, 5, 11, 8, 8, 14, 8, 8, 1, 8, 8, 6, 4, 4, 8, 14, 6, 14, 10, 14, 26, 26, 20, 12, 23, 12}, {1, 1, 8, 11, 8, 11, 8, 11, 11, 17, 14, 8, 17, 11, 11, 11, 6, 6, 6, 6, 6, 8, 6, 14, 10, 10, 8, 29, 12, 29}, {1, 14, 11, 14, 14, 8, 11, 11, 8, 8, 14, 8, 8, 17, 8, 6, 14, 14, 6, 10, 6, 6, 10, 8, 10, 32, 10, 12, 23, 12}, {1, 1, 8, 8, 8, 11, 8, 11, 8, 14, 11, 8, 11, 8, 11, 8, 8, 8, 8, 17, 8, 8, 6, 8, 8, 12, 10, 12, 12, 12}, {1, 11, 11, 8, 11, 11, 8, 8, 11, 11, 8, 8, 14, 8, 11, 6, 6, 6, 6, 11, 1, 8, 6, 8, 10, 14, 12, 10, 23, 26}, {1, 14, 11, 8, 14, 14, 8, 17, 14, 8, 8, 14, 8, 8, 6, 6, 6, 6, 6, 11, 8, 6, 8, 26, 29, 12, 10, 12, 12, 12}, {1, 1, 8, 14, 11, 8, 8, 8, 8, 8, 11, 11, 11, 8, 6, 11, 14, 6, 14, 6, 6, 6, 8, 17, 20, 10, 10, 12, 12, 12}};
string argv_memo[41][30] = {{"tmp,mountains_surround0,5,5,89,7,4,82", "tmp,mountains_surround0,5,5,143,4,6,115", "tmp,mountains_surround0,4,6,203,5,6,153", "tmp,mountains_surround0,2,5,120,4,3,231,9,6,68", "tmp,mountains_surround0,4,4,214,5,3,284", "tmp,mountains_surround0,5,4,349,4,6,193", "tmp,mountains_surround0,4,4,381,4,7,151", "tmp,mountains_surround0,6,5,397", "tmp,mountains_surround0,5,4,500", "tmp,mountains_surround0,4,5,532,8,9,85,6,5,275,5,7,241,1,9,56", "tmp,mountains_surround0,5,5,638", "tmp,mountains_surround0,5,3,628,4,5,270", "tmp,mountains_surround0,5,3,222,6,5,688,5,3,72", "tmp,mountains_surround0,7,9,62,6,4,761", "tmp,mountains_surround0,5,5,852", "tmp", "tmp,mountains_surround0,5,6,864", "tmp,mountains_surround0,5,5,969", "tmp,mountains_surround0,5,5,997", "tmp,1000mountains,5,3,5,3", "tmp,mountains_surround0,7,3,999,9,8,103,6,6,321,4,2,101", "tmp,1000mountains,7,3,7,3", "tmp,1000mountains,5,3,5,3", "tmp,1000mountains,3,6,2,6", "tmp,1000mountains,7,2,7,3", "tmp,1000mountains,6,9,6,8", "tmp,1000mountains,4,1,5,2", "tmp,1000mountains,9,4,8,4", "tmp,1000mountains,1,6,2,6", "tmp,1000mountains,2,8,3,7,2,9"}, {"tmp,mountains_surround0,6,7,104,3,3,73", "tmp,mountains_surround0,4,5,164,4,7,122,4,3,62", "tmp,mountains_surround0,8,5,201,9,9,81,5,6,158", "tmp,mountains_surround0,4,4,204,5,6,178", "tmp,mountains_surround0,5,5,299,2,6,109", "tmp,mountains_surround0,5,6,241,3,9,132,4,5,296", "tmp,mountains_surround0,2,8,86,5,6,407", "tmp,mountains_surround0,5,6,476", "tmp,mountains_surround0,6,4,507,7,6,275", "tmp,mountains_surround0,5,8,350,4,7,538,1,4,20", "tmp,mountains_surround0,5,6,614", "tmp,mountains_surround0,5,6,683", "tmp,mountains_surround0,1,6,79,9,5,51,4,7,686", "tmp,mountains_surround0,6,7,741", "tmp,mountains_surround0,6,4,805", "tmp", "tmp,mountains_surround0,5,6,943", "tmp,mountains_surround0,4,2,188,5,5,972", "tmp,mountains_surround0,4,6,983", "tmp,1000mountains,6,7,6,7", "tmp,1000mountains,3,6,3,6", "tmp,mountains_surround0,4,8,1000,4,1,0,8,8,0,10,6,0,8,8,0", "tmp,1000mountains,5,8", "tmp,1000mountains,8,4,8,3", "tmp,1000mountains,6,2,6,3", "tmp,mountains_surround0,8,9,1000,9,8,1000,5,4,0", "tmp,1000mountains,7,8,9,8", "tmp,1000mountains,8,9,8,8", "tmp,1000mountains,8,9,8,10,7,7", "tmp,1000mountains,8,9,8,10,7,8"}, {"tmp,mountains_surround0,6,7,120,2,6,44", "tmp,mountains_surround0,7,2,89,7,6,143", "tmp,mountains_surround0,6,5,193,7,7,160,8,3,121", "tmp,mountains_surround0,6,5,221,7,5,173", "tmp,mountains_surround0,6,6,265,7,8,143,8,7,195", "tmp,mountains_surround0,7,8,328,7,6,208", "tmp,mountains_surround0,5,6,400", "tmp,mountains_surround0,5,7,417,6,6,295", "tmp,mountains_surround0,9,9,91,6,5,512", "tmp,mountains_surround0,7,6,597,8,7,337", "tmp,mountains_surround0,5,6,613", "tmp,mountains_surround0,7,7,633,5,10,240,10,7,200,3,4,70", "tmp,mountains_surround0,5,6,679", "tmp,mountains_surround0,6,5,752,6,7,460", "tmp", "tmp,mountains_surround0,6,5,897,2,1,41", "tmp,mountains_surround0,11,6,187,6,3,315,8,5,952", "tmp,1000mountains,6,6,6,6", "tmp,1000mountains,3,7,3,7", "tmp,mountains_surround0,5,5,1000,9,11,0,9,2,0", "tmp,1000mountains,5,4", "tmp,1000mountains,8,7,8,7", "tmp", "tmp,1000mountains,5,3,5,2", "tmp,1000mountains,6,4,6,2", "tmp,1000mountains,10,4,8,4", "tmp,1000mountains,4,11,4,8,4,9", "tmp,mountains_surround0,8,11,1000,7,10,1000,8,10,1000,1,2,0,11,8,0,1,11,0", "tmp,mountains_surround0,9,3,1000,11,3,1000,1,3,0,10,7,0,7,10,0", "tmp,mountains_surround0,5,8,1000,6,11,1000,6,10,1000,7,6,0"}, {"tmp,mountains_surround0,7,9,82,5,6,96", "tmp,mountains_surround0,5,1,41,7,8,149,1,11,56,8,6,124", "tmp,mountains_surround0,6,6,203,7,4,147", "tmp,mountains_surround0,7,6,232,5,8,160", "tmp,mountains_surround0,8,4,178,8,6,198,8,8,313,11,7,107,12,4,64", "tmp,mountains_surround0,7,7,360,6,5,259", "tmp,mountains_surround0,5,6,205,7,6,401", "tmp,mountains_surround0,7,7,448,4,5,136,7,8,351", "tmp,mountains_surround0,6,6,532", "tmp,mountains_surround0,3,5,218,6,5,591", "tmp,mountains_surround0,6,7,626", "tmp,mountains_surround0,7,6,707", "tmp,mountains_surround0,6,6,812,6,9,298", "tmp,mountains_surround0,7,7,795", "tmp,mountains_surround0,8,6,787", "tmp,mountains_surround0,9,8,458,8,7,892", "tmp,mountains_surround0,2,7,166,2,3,0,7,8,952,6,9,548", "tmp,1000mountains,7,4,7,4", "tmp,1000mountains,5,6,5,6", "tmp,1000mountains,7,8", "tmp,1000mountains,8,9,8,9", "tmp", "tmp,1000mountains,4,9,4,10", "tmp,mountains_surround0,4,4,673,3,2,956,4,3,805", "tmp,1000mountains,7,4", "tmp,1000mountains,4,8,4,8,2,9", "tmp,1000mountains,8,10,9,11", "tmp,mountains_surround0,6,11,1000,6,9,1000,2,6,0,7,4,0,3,9,0,8,6,0", "tmp,1000mountains,10,6,9,9,12,7", "tmp,1000mountains,7,3,5,2,7,1"}, {"tmp", "tmp,mountains_surround0,10,13,51,8,9,125,7,5,151", "tmp,mountains_surround0,8,4,160,3,12,25,5,12,90,8,6,173", "tmp,mountains_surround0,6,9,106,7,5,200,8,6,239", "tmp,mountains_surround0,7,5,217,5,5,300,3,4,174", "tmp,mountains_surround0,7,7,309,6,6,365", "tmp,mountains_surround0,5,6,379,7,5,158", "tmp,mountains_surround0,6,6,476,3,4,165", "tmp,mountains_surround0,3,8,188,7,8,501", "tmp,mountains_surround0,5,8,365,6,8,548", "tmp,mountains_surround0,8,9,399,7,7,656,1,1,37,6,9,335", "tmp,mountains_surround0,5,5,674,11,6,151,13,5,26,5,8,279", "tmp,mountains_surround0,12,9,39,5,7,737", "tmp,mountains_surround0,12,5,72,7,7,831", "tmp", "tmp", "tmp,1000mountains,7,5,7,5", "tmp,1000mountains,6,6,6,6", "tmp,1000mountains,9,8,9,8", "tmp", "tmp,mountains_surround0,8,9,1000,10,1,0,4,12,0,1,12,0", "tmp,mountains_surround0,8,9,970,13,5,21", "tmp", "tmp,1000mountains,8,12,7,11", "tmp,1000mountains,12,8,10,8", "tmp,1000mountains,10,2,9,4", "tmp,1000mountains,3,9,3,12", "tmp,1000mountains,6,13,7,12,8,10", "tmp,1000mountains,2,12,4,12", "tmp,1000mountains,5,11,3,12,4,10,3,13"}, {"tmp,mountains_surround0,9,6,106,5,13,61,5,11,135", "tmp,mountains_surround0,9,10,95,7,7,152", "tmp,mountains_surround0,7,9,130,7,7,190", "tmp,mountains_surround0,8,8,257", "tmp,mountains_surround0,8,8,316,10,3,56", "tmp,mountains_surround0,8,8,379,3,2,0,3,11,64", "tmp,mountains_surround0,5,9,341,6,9,422", "tmp,mountains_surround0,4,8,191,8,9,516", "tmp,mountains_surround0,12,5,112,8,9,546", "tmp,mountains_surround0,8,8,592", "tmp", "tmp,mountains_surround0,8,8,699,10,10,286", "tmp,mountains_surround0,13,8,195,6,7,342,8,8,838", "tmp,mountains_surround0,7,7,801", "tmp,mountains_surround0,6,10,310,8,8,848", "tmp", "tmp,mountains_surround0,7,9,973,14,10,154", "tmp,mountains_surround0,7,8,992,1,8,0", "tmp,mountains_surround0,13,5,67,6,9,965", "tmp,1000mountains,8,9,8,9", "tmp,1000mountains,9,10,9,10", "tmp,mountains_surround0,6,9,1000,9,2,0", "tmp,mountains_surround0,6,6,157,4,3,337,3,7,1000", "tmp,mountains_surround0,10,9,999", "tmp,mountains_surround0,7,11,1000,2,1,0,14,2,0,10,7,0,10,4,0", "tmp,1000mountains,9,13,9,11", "tmp,mountains_surround0,8,11,1000,7,13,1000,8,6,0", "tmp,1000mountains,10,4,9,2", "tmp,mountains_surround0,3,12,1000,2,13,1000,4,10,1000,7,7,0", "tmp,mountains_surround0,11,13,1000,7,9,1000,9,10,1000,9,3,0,2,2,0"}, {"tmp,mountains_surround0,7,8,92,8,5,113", "tmp,mountains_surround0,9,6,163,7,9,147", "tmp,mountains_surround0,9,9,225,4,10,114", "tmp,mountains_surround0,3,6,141,7,9,270", "tmp,mountains_surround0,6,9,301,9,7,137", "tmp,mountains_surround0,1,11,22,6,7,340,8,6,267", "tmp,mountains_surround0,7,8,283,8,6,416", "tmp,mountains_surround0,9,8,330,8,6,469", "tmp,mountains_surround0,1,2,33,9,7,541,7,10,146", "tmp,mountains_surround0,9,9,598,9,7,365", "tmp,mountains_surround0,8,8,254,8,5,648", "tmp,mountains_surround0,12,11,161,7,7,729", "tmp,mountains_surround0,9,6,552,8,3,99,8,7,739,10,3,19", "tmp,mountains_surround0,11,10,401,9,9,840,2,15,33", "tmp,mountains_surround0,8,7,1000,15,2,0,1,10,0", "tmp,1000mountains,5,9,5,9", "tmp", "tmp,1000mountains,5,9,5,9", "tmp,1000mountains,7,6,7,6", "tmp,1000mountains,7,6,7,6", "tmp,mountains_surround0,5,6,998", "tmp,mountains_surround0,6,5,1000,6,6,668", "tmp,1000mountains,12,8,13,8", "tmp,1000mountains,14,9,11,8", "tmp,1000mountains,5,8,2,7", "tmp,1000mountains,11,5,11,2", "tmp,1000mountains,6,4,6,2", "tmp,mountains_surround0,11,14,1000,10,12,1000,6,7,71,8,8,724", "tmp,mountains_surround0,9,14,1000,11,13,1000,9,15,1000,9,13,1000,2,14,0,15,8,0,3,5,0,6,6,0,2,14,0", "tmp,1000mountains,6,3,5,1,6,7,6,5"}, {"tmp,mountains_surround0,8,9,114,8,12,100", "tmp,mountains_surround0,8,10,173,13,10,121", "tmp,mountains_surround0,9,10,163,10,8,200", "tmp,mountains_surround0,8,8,197,9,10,258", "tmp,mountains_surround0,12,11,210,10,11,321", "tmp,mountains_surround0,13,6,82,10,12,270,10,11,372", "tmp,mountains_surround0,11,8,271,9,9,436", "tmp,mountains_surround0,8,9,520", "tmp,mountains_surround0,8,7,350,9,9,547", "tmp,mountains_surround0,10,8,627,14,5,212", "tmp,mountains_surround0,9,11,341,10,7,660", "tmp,mountains_surround0,9,7,710,15,16,42", "tmp,mountains_surround0,10,3,288,9,6,772", "tmp,mountains_surround0,10,7,861,6,13,127", "tmp,mountains_surround0,8,10,823", "tmp,1000mountains,6,9,6,9", "tmp,1000mountains,7,13,7,13", "tmp,mountains_surround0,7,8,1000,5,16,0", "tmp,mountains_surround0,5,16,29,14,4,302,10,7,963", "tmp,1000mountains,11,7,11,7", "tmp,mountains_surround0,10,5,989", "tmp", "tmp,1000mountains,12,2,12,4", "tmp,1000mountains,8,3,5,4", "tmp", "tmp,1000mountains,9,13,10,15", "tmp", "tmp,1000mountains,13,5,15,3", "tmp,1000mountains,8,2,8,3,9,5", "tmp,1000mountains,16,12,14,13,11,13,13,14"}, {"tmp", "tmp,mountains_surround0,9,8,214", "tmp,mountains_surround0,10,10,159,8,9,209", "tmp,mountains_surround0,7,10,250,9,9,213,12,8,150", "tmp,mountains_surround0,10,13,213,9,11,356,7,6,140,4,7,44", "tmp,mountains_surround0,8,5,202,8,8,392", "tmp,mountains_surround0,11,9,310,10,8,414", "tmp,mountains_surround0,5,12,227,7,10,469", "tmp,mountains_surround0,7,10,524,10,8,243", "tmp,mountains_surround0,8,8,596,8,6,363,17,4,40", "tmp,mountains_surround0,8,10,636", "tmp", "tmp,mountains_surround0,7,8,829", "tmp,mountains_surround0,9,9,876,17,10,33", "tmp,1000mountains,12,10,12,10", "tmp", "tmp,1000mountains,7,9,7,9", "tmp,mountains_surround0,15,11,211,8,12,1000,11,7,189", "tmp,1000mountains,11,10,11,10", "tmp,mountains_surround0,10,7,1000,16,9,0,3,12,0,3,14,0", "tmp,1000mountains,7,13", "tmp,1000mountains,8,8", "tmp,1000mountains,3,14,5,13", "tmp", "tmp", "tmp,mountains_surround0,12,14,1000,13,14,1000,4,15,0,5,13,0,1,10,0", "tmp,mountains_surround0,13,2,1000,11,6,1000,14,16,0,8,11,0", "tmp,1000mountains,13,10,16,9", "tmp,1000mountains,1,8,5,7,2,6", "tmp,1000mountains,15,3,7,8,14,5,17,4,10,6"}, {"tmp", "tmp,mountains_surround0,13,11,139,9,7,201", "tmp,mountains_surround0,12,9,230,8,10,172", "tmp,mountains_surround0,12,6,194,10,7,264", "tmp,mountains_surround0,8,8,319,12,5,166", "tmp,mountains_surround0,9,14,125,10,9,391", "tmp,mountains_surround0,18,10,57,13,11,197,9,8,468,14,7,229", "tmp,mountains_surround0,11,10,478,7,10,259", "tmp,mountains_surround0,9,8,288,11,11,620", "tmp,mountains_surround0,11,12,615,18,16,25,9,14,356,3,16,55", "tmp,mountains_surround0,2,8,46,12,11,489,11,9,695", "tmp", "tmp,mountains_surround0,10,15,92,9,9,785", "tmp,mountains_surround0,9,9,892,10,11,500", "tmp,mountains_surround0,13,4,178,9,11,954", "tmp,1000mountains,12,9,12,9", "tmp", "tmp,mountains_surround0,12,9,1000,7,12,0", "tmp,mountains_surround0,8,11,1000", "tmp,mountains_surround0,9,8,1000,16,12,0,4,10,0", "tmp", "tmp", "tmp,1000mountains,9,8", "tmp,1000mountains,3,11,4,12", "tmp", "tmp,mountains_surround0,15,9,1000,15,8,1000,17,8,1000,17,18,0,14,18,0,7,10,0,6,5,0,18,4,0", "tmp,mountains_surround0,12,6,1000,9,9,1000,12,8,1000,10,3,0,15,4,0,13,5,0,12,13,0,16,15,0", "tmp,mountains_surround0,5,13,1000,6,15,1000,6,17,1000,13,14,0,15,7,0,4,18,0,8,7,0", "tmp,mountains_surround0,6,1,1000,7,5,1000,5,3,1000,3,6,1000,12,6,0,11,8,0,7,17,0", "tmp,1000mountains,12,8,17,4,15,7,13,9,14,5"}, {"tmp,mountains_surround0,13,13,125,10,9,85,14,10,81", "tmp,mountains_surround0,9,5,145,10,9,189", "tmp,mountains_surround0,11,12,209,9,8,189", "tmp,mountains_surround0,4,8,112,9,3,108,9,11,263,11,12,185", "tmp,mountains_surround0,10,9,354,15,8,98", "tmp,mountains_surround0,9,10,233,3,3,25,11,11,410", "tmp", "tmp,mountains_surround0,10,8,424,11,9,586,2,1,3", "tmp,mountains_surround0,9,9,394,12,9,558", "tmp,mountains_surround0,11,10,621", "tmp,mountains_surround0,10,11,757", "tmp,mountains_surround0,9,11,733,9,6,330", "tmp,mountains_surround0,9,10,819,8,8,442", "tmp,mountains_surround0,13,11,809", "tmp,1000mountains,12,10,12,10", "tmp,1000mountains,8,11,8,11", "tmp", "tmp,mountains_surround0,18,19,32,7,8,265,12,11,1000", "tmp,mountains_surround0,10,8,1000,18,16,0", "tmp,mountains_surround0,10,13,999", "tmp,mountains_surround0,11,7,1000,11,7,1000,16,3,0,3,6,0", "tmp,mountains_surround0,12,6,1000,14,2,0,3,11,0,17,15,0,13,14,0,4,5,0", "tmp,1000mountains,9,17,8,16", "tmp,1000mountains,17,7,15,7", "tmp", "tmp,1000mountains,16,18,13,15,15,14", "tmp,1000mountains,15,2,14,6,15,9,13,6", "tmp,1000mountains,4,9,6,7,2,8", "tmp,1000mountains,18,11,17,11,16,13", "tmp,1000mountains,7,3,6,7,9,1,9,4"}, {"tmp,mountains_surround0,13,9,68,12,6,92,12,13,118", "tmp,mountains_surround0,14,4,114,9,9,203", "tmp,mountains_surround0,8,12,245,11,8,107,4,6,52", "tmp,mountains_surround0,11,13,276,18,13,72,12,19,94,12,4,121", "tmp,mountains_surround0,12,8,154,9,11,336,8,12,260", "tmp,mountains_surround0,9,11,404", "tmp,mountains_surround0,13,14,307,11,14,434,12,10,262", "tmp,mountains_surround0,8,12,500,11,13,355", "tmp,mountains_surround0,8,8,403,8,10,582", "tmp,mountains_surround0,12,9,138,13,13,343,10,15,611,20,2,112,18,8,58", "tmp,mountains_surround0,16,2,41,10,12,725,6,8,230", "tmp,mountains_surround0,13,12,743,13,17,268", "tmp,mountains_surround0,9,10,846,7,11,507", "tmp", "tmp", "tmp,1000mountains,11,11,11,11", "tmp,1000mountains,8,7,8,7", "tmp,1000mountains,10,14,10,14", "tmp,1000mountains,13,12,13,12", "tmp,1000mountains,14,13,14,13", "tmp,mountains_surround0,12,12,1000,13,2,0", "tmp,1000mountains,10,7,8,3", "tmp,1000mountains,15,9", "tmp", "tmp,1000mountains,12,14,13,19,14,16", "tmp,1000mountains,6,15,4,18,7,16", "tmp,1000mountains,12,1,11,3,12,7", "tmp,1000mountains,3,19,5,15,3,16", "tmp,1000mountains,13,11,16,10,15,11,19,9", "tmp,1000mountains,17,1,18,7,15,8,17,3"}, {"tmp", "tmp,mountains_surround0,14,9,207,2,14,42,6,12,116", "tmp,mountains_surround0,8,9,136,14,9,269", "tmp,mountains_surround0,9,18,104,14,13,302", "tmp,mountains_surround0,11,12,279,13,12,361", "tmp,mountains_surround0,10,9,392,13,13,251", "tmp,mountains_surround0,12,13,436,11,11,285,14,9,230", "tmp,mountains_surround0,10,12,527,11,10,379", "tmp,mountains_surround0,11,12,432,10,10,600", "tmp,mountains_surround0,15,11,272,10,13,652", "tmp,mountains_surround0,10,12,693,8,12,408", "tmp,mountains_surround0,12,13,726,5,10,177,12,16,450", "tmp,mountains_surround0,11,11,570,12,13,773", "tmp,mountains_surround0,8,15,512,11,20,116,10,14,927,10,13,668", "tmp", "tmp,mountains_surround0,10,9,447,10,12,935", "tmp,1000mountains,13,15,13,15", "tmp,1000mountains,9,11,9,11", "tmp,mountains_surround0,11,12,1000,21,19,0", "tmp,1000mountains,11,10", "tmp,1000mountains,9,13,9,13", "tmp,1000mountains,9,19,7,15", "tmp,1000mountains,14,4,15,6", "tmp,1000mountains,19,17,14,15", "tmp,1000mountains,9,20,10,16", "tmp,1000mountains,13,20,15,16,16,16", "tmp,mountains_surround0,16,19,962,12,18,547,16,21,1000,16,15,1000,10,13,346", "tmp,1000mountains,14,20,14,14,18,12,16,14", "tmp,1000mountains,17,12,14,17,19,19,17,16", "tmp,1000mountains,18,14,15,10,21,11,20,13"}, {"tmp", "tmp,mountains_surround0,9,9,193,16,11,140", "tmp,mountains_surround0,14,13,161,11,9,225", "tmp,mountains_surround0,15,8,274,12,9,225,14,16,50,18,7,148", "tmp,mountains_surround0,21,10,65,14,9,231,12,10,284", "tmp,mountains_surround0,12,12,287,10,11,405", "tmp,mountains_surround0,9,10,386,10,11,497,13,4,109", "tmp,mountains_surround0,10,11,257,15,11,548", "tmp,mountains_surround0,7,6,303,10,8,576", "tmp,mountains_surround0,10,10,631,5,4,114", "tmp,mountains_surround0,16,13,395,13,11,749", "tmp,mountains_surround0,7,8,161,16,19,232,13,13,741,15,17,361", "tmp,mountains_surround0,10,9,816", "tmp,mountains_surround0,14,14,792,12,13,518", "tmp,mountains_surround0,6,5,292,9,9,927", "tmp,mountains_surround0,10,9,996", "tmp", "tmp,mountains_surround0,12,8,441,11,18,117,14,11,967", "tmp,mountains_surround0,10,8,997,5,21,39", "tmp,1000mountains,10,10", "tmp,1000mountains,18,11,14,12", "tmp,1000mountains,16,16", "tmp,1000mountains,17,3,16,7", "tmp", "tmp,1000mountains,6,15,3,15", "tmp,1000mountains,15,3,15,7", "tmp,1000mountains,14,8,17,7,21,10,17,9", "tmp,1000mountains,12,19,15,21,15,16", "tmp,1000mountains,21,5,16,7,18,5", "tmp,1000mountains,16,14,15,16,19,15,22,13"}, {"tmp", "tmp,mountains_surround0,8,13,163,14,10,195", "tmp,mountains_surround0,9,12,199,14,10,264", "tmp,mountains_surround0,13,15,231,7,15,180,15,12,145,11,17,283", "tmp,mountains_surround0,11,10,358,5,9,134", "tmp,mountains_surround0,18,7,122,13,12,398,11,11,251", "tmp,mountains_surround0,8,14,357,2,2,58,11,12,487", "tmp,mountains_surround0,12,10,339,14,12,540", "tmp,mountains_surround0,10,11,390,13,10,602", "tmp,mountains_surround0,11,17,368,13,15,677", "tmp", "tmp,mountains_surround0,11,11,821", "tmp,mountains_surround0,3,14,126,16,11,398,13,8,825", "tmp,mountains_surround0,15,13,911,14,8,388", "tmp,mountains_surround0,17,12,381,14,14,968", "tmp,mountains_surround0,12,11,1000,16,22,0,10,22,0", "tmp", "tmp,mountains_surround0,13,10,1000", "tmp,1000mountains,11,9,11,9", "tmp,mountains_surround0,15,16,1000,15,16,1000,21,1,0,22,23,0,3,6,0,1,12,0,5,3,0", "tmp", "tmp", "tmp,1000mountains,3,8,7,9", "tmp,1000mountains,18,14,21,14", "tmp,mountains_surround0,6,7,1000,9,8,1000,3,8,1000,22,1,0", "tmp,1000mountains,18,6,21,6", "tmp,1000mountains,16,2,16,7,14,4", "tmp,mountains_surround0,22,7,1000,17,7,1000,20,10,1000,19,18,0,3,2,0,2,13,0,4,15,0", "tmp,1000mountains,2,5,8,7,12,8,9,9,4,7", "tmp,1000mountains,13,20,11,18,9,17,14,17,15,22"}, {"tmp", "tmp,mountains_surround0,9,13,186,13,12,183", "tmp,mountains_surround0,14,10,182,8,12,105,15,15,258", "tmp,mountains_surround0,11,14,329,21,12,59,11,9,231", "tmp,mountains_surround0,14,14,359,12,10,232,6,3,73,20,10,139", "tmp,mountains_surround0,13,11,292,11,13,417", "tmp,mountains_surround0,13,11,497,11,21,143,11,10,347", "tmp,mountains_surround0,15,10,277,13,9,536", "tmp,mountains_surround0,17,13,615,13,17,319,6,6,32,4,13,114", "tmp,mountains_surround0,2,9,0,11,14,648,15,11,289,22,17,80", "tmp,mountains_surround0,16,10,337,11,11,684", "tmp,mountains_surround0,13,14,437,15,17,794", "tmp,mountains_surround0,12,12,867", "tmp,mountains_surround0,10,10,884,8,8,450,5,14,160", "tmp,mountains_surround0,15,11,641,16,9,950", "tmp,mountains_surround0,18,11,336,16,16,1000", "tmp", "tmp,1000mountains,17,16,17,16", "tmp,mountains_surround0,9,11,1000,5,3,26,9,11,153", "tmp,mountains_surround0,15,21,1000,16,19,1000,5,7,0,7,21,0,24,11,0", "tmp", "tmp,1000mountains,8,7,11,10", "tmp,1000mountains,14,4,14,9", "tmp,1000mountains,7,17,3,16", "tmp,1000mountains,5,12,7,11,2,13", "tmp,1000mountains,2,15,8,14", "tmp,1000mountains,18,17,15,17,22,21,21,21", "tmp,mountains_surround0,5,10,1000,2,8,1000,11,11,1000,7,8,1000,3,21,0,3,2,0", "tmp,1000mountains,17,2,18,5,22,5,17,5", "tmp,mountains_surround0,11,13,1000,14,12,1000,14,2,1000,14,7,1000,15,9,1000,22,8,0,23,20,0,7,4,0,11,21,0"}, {"tmp", "tmp", "tmp,mountains_surround0,16,7,152,17,18,138,15,13,251", "tmp,mountains_surround0,13,17,260,10,11,161,12,16,253", "tmp,mountains_surround0,11,9,298,14,9,337", "tmp,mountains_surround0,17,11,427,14,19,145,15,12,340", "tmp,mountains_surround0,12,11,376,13,9,494,13,14,266", "tmp,mountains_surround0,9,8,404,10,10,550", "tmp,mountains_surround0,15,9,241,12,16,608,22,11,164", "tmp,mountains_surround0,10,13,594,14,11,320", "tmp,mountains_surround0,6,2,155,8,12,339,11,15,757", "tmp,mountains_surround0,18,13,387,15,11,773", "tmp,1000mountains,14,10,14,10", "tmp,mountains_surround0,13,15,1000,21,24,0", "tmp,1000mountains,11,13,11,13", "tmp", "tmp,mountains_surround0,24,3,0,12,14,967,9,22,15,9,21,26", "tmp,1000mountains,11,16,12,15", "tmp,1000mountains,9,13", "tmp,1000mountains,14,10", "tmp,1000mountains,8,20,9,20", "tmp,1000mountains,11,14", "tmp,1000mountains,13,5,12,10", "tmp,1000mountains,15,7,14,3", "tmp,1000mountains,7,12,14,20", "tmp,1000mountains,8,16,4,15", "tmp,mountains_surround0,8,24,1000,8,19,1000,6,21,1000,17,15,0,11,9,0", "tmp,1000mountains,3,3,11,6,10,8", "tmp,1000mountains,8,8,8,5,2,5,4,9", "tmp,1000mountains,7,3,8,6,9,8,9,6"}, {"tmp", "tmp", "tmp,mountains_surround0,10,13,192,13,9,230", "tmp,mountains_surround0,14,9,205,15,14,276", "tmp,mountains_surround0,13,15,398", "tmp,mountains_surround0,11,16,339,15,15,349", "tmp,mountains_surround0,14,24,81,11,15,474,8,14,281", "tmp,mountains_surround0,12,14,445,13,12,534", "tmp,mountains_surround0,16,15,432,13,16,561", "tmp,mountains_surround0,16,15,655,11,14,455,11,21,122", "tmp", "tmp,mountains_surround0,11,14,825", "tmp,mountains_surround0,12,13,864", "tmp", "tmp,mountains_surround0,14,13,483,10,14,1000,18,18,249", "tmp,mountains_surround0,11,14,980", "tmp,1000mountains,14,13", "tmp,1000mountains,13,11,12,11", "tmp,1000mountains,10,20,9,21", "tmp,1000mountains,13,16,15,15", "tmp,mountains_surround0,10,11,1000,17,24,0", "tmp,1000mountains,9,13,6,14", "tmp,mountains_surround0,8,16,1000,11,3,0,2,6,0,16,3,0,4,12,0", "tmp,1000mountains,4,7,11,8,12,9", "tmp,1000mountains,14,21,14,20,12,24", "tmp,1000mountains,5,17,8,20", "tmp,1000mountains,7,8,12,6,7,12,10,2", "tmp,1000mountains,14,8,12,6,10,7,13,2", "tmp,1000mountains,11,4,15,11,14,2,10,10,13,10", "tmp,1000mountains,16,21,14,22,20,25,18,18,15,16"}, {"tmp", "tmp", "tmp,mountains_surround0,14,17,181,19,20,181,10,17,220", "tmp,mountains_surround0,19,17,237,15,16,313", "tmp,mountains_surround0,14,15,357,16,15,292", "tmp,mountains_surround0,17,18,455,21,21,212,16,11,235", "tmp,mountains_surround0,14,16,519,12,14,406", "tmp,mountains_surround0,10,16,248,13,9,325,21,22,16,9,9,521", "tmp,mountains_surround0,12,11,683,12,2,124,12,2,45,10,10,494", "tmp,mountains_surround0,12,13,420,2,8,84,17,11,679", "tmp,mountains_surround0,16,15,512,17,13,736", "tmp,mountains_surround0,16,8,441,18,12,788", "tmp,mountains_surround0,13,11,761,13,14,497,8,10,260", "tmp,mountains_surround0,16,18,802,7,9,24,17,13,470", "tmp", "tmp,mountains_surround0,14,14,997", "tmp", "tmp,mountains_surround0,10,13,960,16,12,405,13,27,127", "tmp,1000mountains,10,12,12,9", "tmp,1000mountains,12,8,12,7", "tmp,mountains_surround0,9,13,677,8,20,999", "tmp,1000mountains,18,24,18,19", "tmp,1000mountains,17,19,15,24", "tmp,1000mountains,19,12,20,5", "tmp,mountains_surround0,15,8,999,13,3,980,16,12,624,10,7,380,13,24,71", "tmp,1000mountains,19,11,25,13,21,11", "tmp,1000mountains,16,10,17,5,17,2", "tmp,1000mountains,21,14,26,10,24,12", "tmp,1000mountains,25,21,26,24,20,23,21,19", "tmp,1000mountains,24,7,20,8,26,3,20,9,22,7"}, {"tmp", "tmp", "tmp,mountains_surround0,13,15,307", "tmp,mountains_surround0,10,14,194,13,18,259,16,19,336,27,12,34", "tmp,mountains_surround0,13,18,366,9,18,260", "tmp,mountains_surround0,17,12,432", "tmp", "tmp,mountains_surround0,18,11,400,15,12,529,8,23,86", "tmp,mountains_surround0,14,13,551,16,16,372", "tmp,mountains_surround0,16,9,283,15,14,701", "tmp,mountains_surround0,15,14,814,12,19,375", "tmp", "tmp,mountains_surround0,14,14,943,16,14,631,25,4,25", "tmp,mountains_surround0,17,16,947,20,17,525,14,22,345", "tmp,mountains_surround0,19,8,201,24,27,57,11,14,978,12,20,444", "tmp,1000mountains,14,12", "tmp,1000mountains,17,15,17,16", "tmp,mountains_surround0,17,18,986,19,19,624,12,15,377", "tmp,1000mountains,10,11,10,11", "tmp,1000mountains,15,22,16,21", "tmp", "tmp,1000mountains,14,9,16,5", "tmp,1000mountains,21,21,24,20", "tmp,1000mountains,18,25,16,22", "tmp,1000mountains,8,18,3,17,11,22", "tmp,1000mountains,12,12,18,8,17,12,18,2", "tmp,mountains_surround0,23,25,1000,22,20,1000,19,22,1000,2,3,0,7,18,0", "tmp,mountains_surround0,13,7,1000,12,7,1000,7,11,1000,9,2,1000,23,20,0,2,15,0,21,16,0,5,26,0,27,19,0", "tmp,1000mountains,6,10,10,1,7,8,7,4,11,7", "tmp,1000mountains,7,25,5,24,9,17,11,18,2,21"}, {"tmp", "tmp,mountains_surround0,11,20,170,11,5,73,6,23,115,22,23,121", "tmp,mountains_surround0,15,15,302", "tmp,mountains_surround0,14,18,200,12,13,254,17,16,214", "tmp,mountains_surround0,17,15,173,10,16,353,15,20,195,9,13,248", "tmp,mountains_surround0,8,15,128,15,16,306,18,10,210,16,19,474", "tmp,mountains_surround0,21,20,136,11,23,17,22,14,279,8,7,62,17,12,479", "tmp,mountains_surround0,19,15,541,17,9,222", "tmp,mountains_surround0,13,14,313,18,16,604,17,17,454", "tmp,mountains_surround0,13,13,657,9,15,408", "tmp,mountains_surround0,12,20,525,15,19,758", "tmp,mountains_surround0,15,17,848,14,20,399", "tmp", "tmp", "tmp", "tmp,mountains_surround0,15,8,137,19,18,971,14,17,547", "tmp,mountains_surround0,28,12,230,18,10,998,23,16,405", "tmp,mountains_surround0,15,20,1000,14,17,420,8,18,525,21,9,6,29,29,21", "tmp,mountains_surround0,12,19,998,22,16,291", "tmp,1000mountains,16,11,17,11", "tmp,1000mountains,25,18,20,19", "tmp,mountains_surround0,16,4,1000,14,8,1000,20,21,0,2,7,0,19,24,0,9,20,0", "tmp", "tmp,1000mountains,19,4,18,10", "tmp,1000mountains,26,22,23,24", "tmp,1000mountains,9,19,11,22,8,27", "tmp,mountains_surround0,3,9,1000,9,11,1000,5,4,1000,7,26,0,29,15,0,14,21,0,23,24,0,24,20,0", "tmp,1000mountains,13,21,8,22,13,23,12,27", "tmp,1000mountains,23,25,18,20,28,21,24,18", "tmp,mountains_surround0,24,15,1000,28,16,1000,20,10,1000,22,12,1000,23,11,1000,19,23,0,3,20,0,16,26,0"}, {"tmp", "tmp,mountains_surround0,18,14,206,13,16,159", "tmp,mountains_surround0,20,16,210,15,11,281", "tmp,mountains_surround0,15,13,305,16,14,338", "tmp,mountains_surround0,14,20,189,8,10,118,13,15,332", "tmp,mountains_surround0,14,13,282,9,15,427", "tmp,mountains_surround0,14,16,529,16,20,343", "tmp,mountains_surround0,20,14,542,17,17,338", "tmp,mountains_surround0,8,23,202,17,21,628,18,17,322", "tmp,mountains_surround0,17,19,488,12,15,651,11,5,173,13,13,410", "tmp", "tmp", "tmp,1000mountains,15,14,15,14", "tmp,1000mountains,14,16,14,16", "tmp", "tmp", "tmp,1000mountains,17,11", "tmp,1000mountains,16,18,17,18", "tmp", "tmp", "tmp,1000mountains,17,22,16,25", "tmp,mountains_surround0,3,8,127,13,19,547,7,17,999", "tmp", "tmp", "tmp,1000mountains,11,24,11,22,10,28", "tmp,1000mountains,8,8,3,12,10,10,8,5", "tmp,1000mountains,8,13,9,17,6,13,2,18", "tmp,1000mountains,12,8,13,16,13,2,15,12", "tmp,1000mountains,21,2,16,3,21,6,19,6,18,13", "tmp,1000mountains,6,16,2,13,13,13,6,8"}, {"tmp", "tmp,mountains_surround0,12,19,132,19,24,87,21,20,144,25,18,57,13,25,161", "tmp,mountains_surround0,18,28,2,18,18,233,18,21,252,7,21,76", "tmp,mountains_surround0,19,20,288,17,16,243", "tmp,mountains_surround0,18,24,237,12,23,276,14,22,338", "tmp,mountains_surround0,15,22,458,10,23,299", "tmp,mountains_surround0,15,10,492,12,10,340,19,11,304", "tmp,mountains_surround0,18,15,316,12,15,596", "tmp,mountains_surround0,14,18,598,12,21,249,15,24,283,14,5,2", "tmp,mountains_surround0,13,18,487,13,10,233,15,16,729", "tmp,mountains_surround0,31,21,37,12,22,752,15,23,471", "tmp,mountains_surround0,15,21,839,22,28,39,16,17,431", "tmp,mountains_surround0,16,18,762,14,18,856", "tmp,mountains_surround0,19,15,485,17,20,916", "tmp", "tmp,mountains_surround0,16,18,1000,1,28,0", "tmp,mountains_surround0,24,15,395,17,17,983", "tmp,mountains_surround0,19,19,1000,19,31,0,28,24,0", "tmp,mountains_surround0,21,22,983,21,15,702", "tmp,mountains_surround0,6,15,1000,11,15,1000,28,23,0,20,23,0,18,3,0,29,16,0,7,4,0", "tmp", "tmp,1000mountains,10,18,6,19", "tmp,1000mountains,10,25,5,23", "tmp", "tmp,1000mountains,8,28,11,24", "tmp,1000mountains,5,27,11,25,12,29,12,22", "tmp,1000mountains,11,4,10,8,9,6,10,2", "tmp,1000mountains,18,28,17,27,25,22,14,22,15,30", "tmp,1000mountains,19,23,16,28,13,30,20,18,19,28", "tmp,1000mountains,12,24,9,25,17,24,12,30,15,26"}, {"tmp", "tmp", "tmp,mountains_surround0,17,12,264,6,24,77,17,14,302", "tmp,mountains_surround0,16,19,317,17,16,310", "tmp,mountains_surround0,14,19,282,16,16,377", "tmp,mountains_surround0,17,9,236,15,16,316,17,14,411", "tmp,mountains_surround0,17,21,439,17,18,393", "tmp,mountains_surround0,18,21,437,21,21,561,9,19,115", "tmp,mountains_surround0,17,15,579,14,14,400,18,24,362", "tmp,mountains_surround0,19,13,679,27,9,97,23,14,419", "tmp,mountains_surround0,15,15,475,23,12,160,13,16,419,18,14,762", "tmp,mountains_surround0,17,18,797,23,12,154,13,15,538", "tmp,mountains_surround0,15,14,925,21,14,438", "tmp,mountains_surround0,28,31,111,17,21,639,14,20,916", "tmp,1000mountains,15,16,15,16", "tmp,mountains_surround0,20,22,554,19,10,141,17,20,948,5,11,17", "tmp,mountains_surround0,21,8,663,19,10,1000", "tmp,1000mountains,24,18,24,18", "tmp,mountains_surround0,22,18,1000,22,15,706", "tmp", "tmp,1000mountains,22,12,22,10", "tmp,1000mountains,16,29,15,25,13,24", "tmp,1000mountains,23,23,23,26,28,25", "tmp,1000mountains,20,30,18,26", "tmp,mountains_surround0,21,25,1000,29,29,1000,22,21,1000,25,23,1000,4,2,0,9,26,0", "tmp,mountains_surround0,12,23,1000,11,26,1000,8,25,1000,14,30,1000,29,24,0,6,2,0", "tmp,1000mountains,24,31,17,24,21,27,25,24", "tmp,1000mountains,12,27,13,31,12,24", "tmp,1000mountains,16,7,18,2,11,9,11,5", "tmp,1000mountains,19,5,23,8,22,11,31,14"}, {"tmp", "tmp", "tmp,mountains_surround0,14,17,231,18,15,268", "tmp,mountains_surround0,14,23,236,22,23,209,17,24,268,17,22,280", "tmp,mountains_surround0,22,16,372,21,23,198", "tmp,mountains_surround0,24,25,208,18,22,451", "tmp,mountains_surround0,16,16,502,24,9,216,23,14,276", "tmp,mountains_surround0,16,17,560,13,15,365", "tmp,mountains_surround0,18,15,325,16,29,81,18,21,578", "tmp,mountains_surround0,16,11,552,17,9,617,16,16,332", "tmp,mountains_surround0,19,22,353,18,15,838", "tmp,mountains_surround0,19,19,527,19,13,793", "tmp,mountains_surround0,17,20,494,18,1,8,18,15,885", "tmp,mountains_surround0,20,13,571,16,12,1000", "tmp,mountains_surround0,19,12,955,14,12,484", "tmp,mountains_surround0,16,16,451,21,1,17,16,22,981", "tmp,mountains_surround0,17,15,970,22,14,337,19,13,631", "tmp,mountains_surround0,14,13,600,18,23,964", "tmp,1000mountains,18,21,18,22", "tmp,mountains_surround0,21,20,769,22,26,970", "tmp,1000mountains,15,28,21,26", "tmp,1000mountains,21,22,22,29", "tmp,1000mountains,18,4,18,10", "tmp,1000mountains,20,12,17,4", "tmp,1000mountains,24,9,19,10,22,3", "tmp,1000mountains,21,5,26,8,31,10,26,11", "tmp,1000mountains,22,24,19,24,21,30", "tmp,1000mountains,10,7,12,2,13,6,18,13", "tmp,1000mountains,27,7,22,11,25,5,28,16,31,5", "tmp,mountains_surround0,24,13,1000,20,21,1000,23,16,1000,28,17,1000,23,32,1000,11,32,0"}, {"tmp", "tmp,mountains_surround0,13,21,128,19,13,176,25,18,123", "tmp,mountains_surround0,30,19,76,24,11,230,18,9,135,17,21,107", "tmp", "tmp,mountains_surround0,21,17,408,12,20,211", "tmp,mountains_surround0,24,17,379,5,21,1,24,12,301", "tmp,mountains_surround0,17,13,392,20,9,221,22,14,473", "tmp,mountains_surround0,19,19,509,17,14,436", "tmp,mountains_surround0,32,2,28,22,15,619,15,16,443,15,9,201", "tmp,mountains_surround0,21,14,432,19,21,680", "tmp,mountains_surround0,20,19,789,16,16,423", "tmp,mountains_surround0,12,16,218,20,18,857,17,2,36,30,25,49", "tmp,mountains_surround0,18,15,882,31,1,24,26,22,282,25,14,461", "tmp", "tmp,mountains_surround0,27,15,565,22,15,959,21,13,694", "tmp,1000mountains,22,13,21,13", "tmp,1000mountains,16,15,16,14", "tmp,mountains_surround0,25,20,965,22,17,645", "tmp,mountains_surround0,10,17,289,20,19,998", "tmp,1000mountains,19,10,24,12", "tmp,1000mountains,17,30,16,24", "tmp", "tmp,1000mountains,14,30,15,23", "tmp,mountains_surround0,23,24,1000,28,21,1000,32,21,1000,24,26,1000,20,10,0,8,23,0,27,3,0", "tmp,1000mountains,19,31,20,20,18,26", "tmp,1000mountains,22,12,26,4,23,7", "tmp,1000mountains,24,7,27,13,28,2,28,7,29,9", "tmp,mountains_surround0,16,27,1000,25,28,1000,21,26,1000,22,19,1000,16,33,1000,23,33,0,15,8,0,31,12,0", "tmp,1000mountains,25,7,22,3,27,11,32,10,27,4", "tmp,1000mountains,21,15,34,13,24,14,29,9,31,13"}, {"tmp", "tmp", "tmp,mountains_surround0,18,22,208,14,18,253", "tmp,mountains_surround0,22,22,217,14,11,214,18,20,309", "tmp,mountains_surround0,19,17,378,7,12,199,26,16,224,25,24,165", "tmp,mountains_surround0,20,16,492,17,18,360,8,9,198", "tmp,mountains_surround0,28,6,112,24,6,151,19,18,334,17,14,474,15,17,420", "tmp,mountains_surround0,35,4,22,20,31,91,21,18,360,15,15,647", "tmp,mountains_surround0,16,14,456,20,16,670", "tmp,mountains_surround0,16,13,667,3,31,34,19,11,446", "tmp,mountains_surround0,17,22,815,14,18,457", "tmp,mountains_surround0,14,14,857,10,19,387,16,22,448", "tmp,mountains_surround0,15,20,889,6,21,215,10,22,380,17,19,534", "tmp,mountains_surround0,16,16,1000,35,31,0", "tmp,1000mountains,17,19,16,19", "tmp,mountains_surround0,21,21,1000,17,3,0,6,15,0", "tmp", "tmp,1000mountains,16,26,16,27", "tmp,1000mountains,18,7,18,11", "tmp,1000mountains,7,20,14,18", "tmp,mountains_surround0,12,8,1000,7,11,1000,17,32,0,22,31,0,29,31,0,32,17,0,29,18,0", "tmp,1000mountains,19,28,25,22", "tmp,1000mountains,6,18,11,28,8,19", "tmp,1000mountains,20,4,22,14,18,8", "tmp,1000mountains,10,23,14,4,16,21,13,21,11,19", "tmp,1000mountains,25,27,33,25,28,25", "tmp,1000mountains,14,33,12,28,16,22,9,22", "tmp,1000mountains,12,31,13,28,18,23,19,34,17,29", "tmp,1000mountains,6,18,14,14,13,13,9,15,3,24", "tmp,1000mountains,7,32,8,24,11,23,13,34,6,28"}, {"tmp", "tmp", "tmp,mountains_surround0,3,7,5,19,21,215,18,14,259", "tmp,mountains_surround0,19,18,375", "tmp,mountains_surround0,19,24,412,17,16,253", "tmp,mountains_surround0,20,20,486", "tmp,mountains_surround0,18,22,513,11,18,296", "tmp,mountains_surround0,16,23,205,13,11,289,18,15,549", "tmp,mountains_surround0,21,22,655,21,24,498,17,22,406", "tmp,mountains_surround0,19,22,663,17,28,387", "tmp,mountains_surround0,21,31,268,18,20,836", "tmp,1000mountains,19,18,19,18", "tmp,mountains_surround0,19,21,896,23,21,603", "tmp,mountains_surround0,25,18,566,30,25,304,20,18,963", "tmp,mountains_surround0,17,16,448,19,20,736,22,22,963", "tmp,1000mountains,16,24,17,24", "tmp,1000mountains,12,28,14,27", "tmp,1000mountains,17,25,19,30", "tmp,1000mountains,16,25,17,29", "tmp,mountains_surround0,17,11,1000,1,17,0,3,31,0,5,5,0,16,29,0,6,28,0", "tmp,1000mountains,25,19,16,14", "tmp,1000mountains,17,26,20,31", "tmp,1000mountains,21,32,17,26", "tmp,1000mountains,21,15,21,12,21,5", "tmp,1000mountains,21,26,23,35,19,32", "tmp,mountains_surround0,9,21,1000,7,26,1000,14,20,1000,7,33,1000,14,25,1000,5,9,0,6,4,0,4,28,0", "tmp,mountains_surround0,26,20,462,2,12,100,24,32,1000,23,24,909", "tmp,1000mountains,18,33,19,27,15,35,14,32", "tmp,1000mountains,16,23,14,34,17,29", "tmp,1000mountains,8,25,11,31,12,27,2,34,15,27"}, {"tmp", "tmp", "tmp,mountains_surround0,17,23,208,20,19,306", "tmp,mountains_surround0,16,17,330,14,23,243", "tmp,mountains_surround0,22,23,298,24,21,348,21,13,212", "tmp,mountains_surround0,14,21,317,22,18,487", "tmp,mountains_surround0,21,21,534,18,21,397", "tmp,mountains_surround0,11,22,242,16,15,541,18,21,420", "tmp,mountains_surround0,16,20,652,20,20,367,10,17,381", "tmp,mountains_surround0,22,17,690,22,22,510", "tmp,mountains_surround0,21,28,351,24,24,845", "tmp,mountains_surround0,25,17,558,22,20,837", "tmp,mountains_surround0,18,16,890,8,24,273,16,14,376,24,17,477", "tmp,mountains_surround0,27,18,348,26,14,360,20,17,922,19,2,33", "tmp,mountains_surround0,21,18,970,15,28,298,15,20,524,12,32,92", "tmp,mountains_surround0,20,22,997,29,24,458,32,37,65", "tmp,mountains_surround0,9,19,413,14,17,999,10,9,303", "tmp,mountains_surround0,24,12,982,18,15,560", "tmp,1000mountains,19,24", "tmp,mountains_surround0,22,27,897,27,7,38,34,13,34,24,28,1000,26,19,708", "tmp,1000mountains,7,21,15,22", "tmp,1000mountains,23,27,29,22", "tmp,mountains_surround0,7,27,1000,19,30,1000,14,27,1000,31,17,0,22,11,0", "tmp,1000mountains,26,26,29,25,28,32", "tmp", "tmp,1000mountains,34,13,27,9,27,15,32,7", "tmp,1000mountains,17,32,20,33,15,26,22,36", "tmp,1000mountains,11,32,11,35,8,25,7,31", "tmp,1000mountains,17,2,12,7,14,14,20,9,23,11", "tmp,1000mountains,12,36,18,31,5,32,15,28,6,25"}, {"tmp", "tmp", "tmp,mountains_surround0,26,12,158,15,19,326", "tmp,mountains_surround0,20,14,153,13,19,323,22,23,161", "tmp,mountains_surround0,15,17,381,23,18,276", "tmp,mountains_surround0,16,27,321,15,21,337,15,24,433", "tmp,mountains_surround0,18,19,516,11,22,370", "tmp,mountains_surround0,14,27,282,22,15,217,19,22,561,4,21,94", "tmp,mountains_surround0,21,17,426,20,22,718", "tmp,mountains_surround0,14,20,766,14,17,461", "tmp,mountains_surround0,20,21,803,28,17,171,15,12,312", "tmp,mountains_surround0,16,22,515,29,6,32,17,19,872", "tmp,mountains_surround0,20,20,494,18,23,866,14,27,457", "tmp,mountains_surround0,23,23,418,22,27,376,3,37,45,23,16,966,12,22,139", "tmp", "tmp,mountains_surround0,22,20,607,15,18,506,18,17,971", "tmp,1000mountains,14,14,13,14", "tmp,mountains_surround0,19,29,578,33,9,33,13,20,999", "tmp,mountains_surround0,15,18,1000,23,23,469,12,28,466,20,11,115", "tmp,1000mountains,15,17,12,15", "tmp,1000mountains,18,22,9,22", "tmp,1000mountains,18,20,17,6", "tmp,1000mountains,17,16,14,6", "tmp,mountains_surround0,5,22,1000,13,17,1000,31,17,0,14,11,0,31,13,0,9,37,0", "tmp,1000mountains,19,3,14,16,22,11", "tmp,1000mountains,10,36,12,32,9,32,14,28", "tmp,1000mountains,14,23,16,35,19,29,15,29", "tmp,1000mountains,17,2,22,8,16,11,13,7", "tmp,1000mountains,14,25,13,21,7,22,3,27,10,28", "tmp,mountains_surround0,10,29,1000,13,24,1000,9,37,1000,12,20,1000,6,23,1000,5,4,0,35,7,0,4,3,0"}, {"tmp", "tmp", "tmp,mountains_surround0,29,25,200,21,20,329", "tmp,mountains_surround0,25,17,294,20,17,251", "tmp,mountains_surround0,24,17,400,22,24,383", "tmp,mountains_surround0,22,23,464,29,18,239", "tmp,mountains_surround0,22,17,474,18,17,376,20,14,374", "tmp,mountains_surround0,14,21,416,19,19,605", "tmp,mountains_surround0,15,20,488,8,7,149,18,21,619,34,2,56", "tmp,mountains_surround0,14,19,695,18,15,495,16,22,524", "tmp,mountains_surround0,12,16,438,18,15,737", "tmp,mountains_surround0,15,21,859,15,27,445,6,6,21,16,33,186", "tmp,mountains_surround0,20,22,923,24,26,396,27,20,338", "tmp,mountains_surround0,21,26,944,26,23,614,7,28,202", "tmp,1000mountains,21,18,20,18", "tmp", "tmp,mountains_surround0,17,21,1000,39,9,0,17,3,0", "tmp,1000mountains,18,16", "tmp,1000mountains,11,17,7,19", "tmp,1000mountains,8,24,10,26", "tmp,1000mountains,18,27,17,31", "tmp,mountains_surround0,30,20,999,20,23,842,34,14,1,34,14,216,24,19,556", "tmp,1000mountains,23,11,35,11,26,7,28,11", "tmp,1000mountains,5,22,14,22", "tmp,mountains_surround0,25,14,1000,28,6,1000,4,36,0,37,18,0,18,29,0,15,18,0,30,33,0", "tmp,1000mountains,4,34,10,27,10,30,15,26", "tmp,1000mountains,3,23,9,25,7,21,10,17", "tmp,1000mountains,12,27,2,21,8,25,13,26,15,19", "tmp,1000mountains,13,25,2,19,8,18,6,19,6,16", "tmp,mountains_surround0,4,36,1000,13,27,1000,7,31,1000,7,28,1000,2,30,1000,39,28,0,7,13,0,28,26,0,4,10,0"}, {"tmp", "tmp", "tmp,mountains_surround0,27,23,201,21,25,281", "tmp,mountains_surround0,23,22,332,20,18,251", "tmp,mountains_surround0,23,26,344,21,23,362", "tmp,mountains_surround0,17,23,453,24,20,344", "tmp,mountains_surround0,19,16,380,20,18,499,29,11,246", "tmp,mountains_surround0,4,16,30,19,26,416,14,32,319,16,23,635,28,21,200", "tmp,mountains_surround0,27,31,141,19,18,498,35,5,150,18,21,674", "tmp,mountains_surround0,21,21,574,17,32,269,20,22,722", "tmp,mountains_surround0,22,21,458,16,27,105,26,22,762", "tmp,mountains_surround0,21,27,540,22,21,891", "tmp,mountains_surround0,19,9,41,18,23,929,22,24,394", "tmp,mountains_surround0,22,22,998", "tmp,mountains_surround0,21,21,1000,15,29,214", "tmp", "tmp", "tmp,mountains_surround0,19,23,1000", "tmp,1000mountains,14,26,11,23", "tmp", "tmp,1000mountains,29,29,29,27,36,28", "tmp,1000mountains,28,34,30,29", "tmp,mountains_surround0,20,28,1000,12,33,1000,37,10,0", "tmp,1000mountains,11,37,15,25,14,21,15,27", "tmp,1000mountains,36,11,24,12,31,13", "tmp,mountains_surround0,12,23,1000,3,14,1000,14,16,1000,15,40,0,39,2,0,27,24,0,27,39,0,32,39,0", "tmp,1000mountains,20,26,15,32,18,38", "tmp,mountains_surround0,29,16,1000,32,25,1000,39,21,1000,29,22,1000,37,38,0,13,3,0,13,12,0,34,7,0", "tmp,mountains_surround0,2,25,1000,15,23,1000,11,19,1000,15,31,1000,5,25,1000,4,4,0,34,8,0,38,4,0", "tmp,1000mountains,28,15,29,17,25,2,37,8,28,6"}, {"tmp", "tmp", "tmp,mountains_surround0,25,18,260,15,27,131,8,7,70,27,27,218", "tmp,mountains_surround0,23,19,343,26,22,244", "tmp,mountains_surround0,21,20,330,24,18,393,23,27,222", "tmp,mountains_surround0,25,18,454,18,19,267", "tmp,mountains_surround0,13,24,220,30,18,213,22,19,506", "tmp,mountains_surround0,21,18,277,23,26,558,19,30,384,21,30,405", "tmp,mountains_surround0,19,16,647,20,23,479", "tmp,mountains_surround0,16,14,431,12,17,440,20,13,641,26,17,331", "tmp,mountains_surround0,21,20,811,12,18,373", "tmp,mountains_surround0,37,26,76,20,20,939,15,21,651", "tmp,mountains_surround0,18,20,962,15,25,410", "tmp,mountains_surround0,27,10,415,25,17,943", "tmp,mountains_surround0,17,20,944,14,11,163,20,25,517,13,29,170", "tmp,mountains_surround0,2,39,109,22,13,622,30,9,401,16,14,1000", "tmp,mountains_surround0,21,27,1000,15,6,0,27,9,0", "tmp,1000mountains,22,16,18,13", "tmp,mountains_surround0,28,1,125,22,23,994,32,6,198,5,5,113", "tmp,1000mountains,20,33,21,30", "tmp", "tmp,1000mountains,23,30,23,36", "tmp,1000mountains,21,32,15,37,19,33", "tmp,1000mountains,24,28,21,32,18,38", "tmp,1000mountains,22,10,15,11,22,3", "tmp,mountains_surround0,9,18,1000,18,4,1000,13,12,1000,15,8,1000,36,16,0,3,21,0,38,26,0,23,39,0,40,27,0", "tmp,mountains_surround0,23,32,1000,25,38,1000,26,29,1000,21,34,1000,25,1,0,41,8,0,9,25,0,34,12,0", "tmp,1000mountains,20,39,18,29,21,31,22,32", "tmp,1000mountains,10,2,13,11,6,13,4,7,10,7", "tmp,1000mountains,14,21,15,3,9,14,11,11,22,17"}, {"tmp", "tmp", "tmp,mountains_surround0,20,15,312,19,38,80,16,21,257", "tmp,mountains_surround0,30,2,53,29,20,169,20,12,389,14,15,213", "tmp,mountains_surround0,16,22,228,19,17,457", "tmp,mountains_surround0,18,16,399,23,14,496,27,12,232", "tmp,mountains_surround0,6,14,126,33,31,122,22,15,507,22,20,393", "tmp,mountains_surround0,18,23,403,23,21,584", "tmp,mountains_surround0,28,21,296,25,13,647,22,12,463", "tmp,mountains_surround0,36,28,88,24,14,791,18,14,380,26,20,414", "tmp,mountains_surround0,20,23,511,25,25,863", "tmp,mountains_surround0,19,16,858,22,23,399", "tmp,mountains_surround0,13,14,203,25,22,876,22,22,665,23,30,423", "tmp,1000mountains,21,18,21,16", "tmp,1000mountains,25,28,26,23", "tmp,1000mountains,20,19,20,18", "tmp,mountains_surround0,27,11,38,20,26,687,23,16,378,15,30,980,39,37,108", "tmp,mountains_surround0,10,19,432,3,40,236,16,32,974,20,18,401,14,28,675", "tmp,1000mountains,22,36,23,27", "tmp,mountains_surround0,18,8,1000,17,18,1000,37,10,0,35,38,0,39,41,0,3,27,0", "tmp,mountains_surround0,22,14,1000", "tmp,mountains_surround0,26,39,988,23,29,992,26,27,939", "tmp,1000mountains,15,21,17,19,16,7", "tmp,1000mountains,19,4,23,12", "tmp,1000mountains,21,9,23,16,21,4", "tmp,1000mountains,21,27,13,34,11,40,8,28", "tmp,1000mountains,33,21,30,14,26,3,26,10", "tmp,1000mountains,11,7,22,4,15,11,13,17", "tmp,1000mountains,30,7,26,2,30,5,23,13,24,8", "tmp,1000mountains,24,30,16,37,13,31,4,35,18,40"}, {"tmp", "tmp", "tmp,mountains_surround0,22,14,160,20,20,115,23,22,183,27,27,222", "tmp,mountains_surround0,21,25,326,38,33,84,42,3,17,13,6,146,16,18,194", "tmp,mountains_surround0,20,22,259,24,21,384,22,34,231", "tmp,mountains_surround0,24,25,532", "tmp,mountains_surround0,25,23,571,28,29,226,20,22,498", "tmp,mountains_surround0,21,27,590,22,21,436", "tmp,mountains_surround0,20,25,668,20,20,402", "tmp,mountains_surround0,21,32,244,21,22,822,7,13,112,18,23,446", "tmp,mountains_surround0,19,24,444,27,26,738", "tmp,mountains_surround0,20,23,908,20,20,600", "tmp", "tmp,mountains_surround0,19,31,288,25,22,980", "tmp,mountains_surround0,13,17,566,19,19,941", "tmp,1000mountains,20,19,22,18", "tmp,1000mountains,20,25", "tmp,1000mountains,23,18", "tmp,mountains_surround0,24,27,789,18,25,998", "tmp,mountains_surround0,17,24,1000,12,17,1000,35,26,0,26,2,0", "tmp,1000mountains,31,23,23,18", "tmp,mountains_surround0,26,23,1000,28,37,1000,29,19,1000,42,2,0", "tmp,1000mountains,14,30,17,30,12,35,5,29", "tmp,mountains_surround0,15,20,247,14,31,830,18,35,655,8,28,985", "tmp,mountains_surround0,19,8,1000,16,17,1000,20,24,1000,31,26,0,25,32,0,10,26,0,18,33,0,27,22,0", "tmp,mountains_surround0,13,11,1000,10,14,1000,5,12,1000,19,15,1000,34,17,0,33,31,0,43,1,0,43,4,0", "tmp,mountains_surround0,40,28,1000,34,25,1000,28,30,1000,18,32,0,17,27,0,2,9,0", "tmp,1000mountains,40,40,16,24,10,27,9,15,22,23", "tmp,mountains_surround0,31,22,1000,34,17,1000,41,18,1000,29,16,1000,41,24,1000,13,39,0,16,14,0", "tmp,1000mountains,6,25,2,27,11,19,12,26,13,24"}, {"tmp", "tmp", "tmp,mountains_surround0,19,21,249,28,24,213", "tmp,mountains_surround0,20,20,294,25,21,291,38,35,126", "tmp,mountains_surround0,17,23,394,24,25,268", "tmp,mountains_surround0,19,23,429,26,22,341,21,30,244", "tmp,mountains_surround0,24,22,568,31,24,390", "tmp,mountains_surround0,18,23,586,22,16,416,29,11,115", "tmp,mountains_surround0,21,19,408,25,19,677,36,19,250", "tmp,mountains_surround0,5,3,53,27,22,339,18,15,426,19,22,777,21,9,258", "tmp,mountains_surround0,37,5,70,17,28,333,19,22,789,29,19,305", "tmp,mountains_surround0,23,19,899,25,23,608", "tmp,mountains_surround0,30,16,557,6,8,134,20,21,350,24,16,894,26,14,547", "tmp,mountains_surround0,33,23,491,28,18,915,13,20,196", "tmp,mountains_surround0,6,15,284,13,21,950,19,19,621", "tmp,mountains_surround0,31,23,985,28,20,650,33,32,375", "tmp,1000mountains,17,20,19,22", "tmp,1000mountains,26,27,32,33", "tmp,1000mountains,21,18,14,23", "tmp,1000mountains,18,20,12,16", "tmp,1000mountains,8,15,12,14", "tmp,1000mountains,18,18,6,18,14,19", "tmp,1000mountains,6,19,24,11", "tmp,mountains_surround0,5,25,1000,25,15,1000,17,21,1000,22,33,0", "tmp,1000mountains,18,17,18,11,3,17,16,16", "tmp,1000mountains,33,39,31,31,29,36,42,32", "tmp,1000mountains,17,15,4,11,13,10", "tmp,mountains_surround0,3,23,1000,12,34,1000,6,28,1000,19,24,1000,6,26,1000,28,17,0,30,38,0,36,14,0,32,20,0", "tmp,1000mountains,15,15,7,31,11,18,3,27,9,31", "tmp,mountains_surround0,22,13,1000,27,7,1000,17,10,1000,22,2,1000,30,8,1000,22,28,0,37,24,0,10,38,0,9,22,0"}, {"tmp", "tmp,mountains_surround0,24,18,231,24,34,39,23,11,150,19,32,92", "tmp,mountains_surround0,30,27,124,19,26,265,22,22,268", "tmp,mountains_surround0,25,30,216,25,35,329,42,25,46,24,20,170", "tmp,mountains_surround0,24,22,274,28,26,376,12,28,177,13,13,237", "tmp,mountains_surround0,28,26,317,30,18,456", "tmp,mountains_surround0,17,13,344,17,17,539,24,13,317", "tmp,mountains_surround0,24,15,434,22,19,548,14,18,236", "tmp,mountains_surround0,22,19,714,19,12,389", "tmp,mountains_surround0,20,25,451,29,27,695", "tmp,mountains_surround0,33,23,404,25,26,730,24,12,184,21,30,350", "tmp,mountains_surround0,18,25,540,20,18,955", "tmp,mountains_surround0,30,22,575,20,23,937", "tmp,mountains_surround0,39,31,448,29,32,941,40,26,275,30,28,615,13,19,42", "tmp,mountains_surround0,24,13,548,24,19,968", "tmp,1000mountains,25,15,19,16", "tmp,mountains_surround0,24,17,537,32,20,535,9,39,79,22,16,975", "tmp,mountains_surround0,35,40,281,24,45,103,30,30,604,25,26,975", "tmp,1000mountains,15,18,22,25", "tmp,1000mountains,30,16,28,8,30,13,35,15", "tmp,1000mountains,20,13,20,6", "tmp,1000mountains,22,9,19,12", "tmp,1000mountains,10,36,11,33,19,42,19,37", "tmp,1000mountains,26,5,28,17,32,11", "tmp,1000mountains,28,18,28,8,26,4,32,22", "tmp,mountains_surround0,22,11,1000,21,3,1000,18,15,1000,27,14,1000,17,8,1000,43,29,0,43,12,0,3,20,0,15,25,0,21,3,0", "tmp,1000mountains,31,25,35,21,21,21,43,18", "tmp,1000mountains,24,35,15,26,16,30,23,44,15,41", "tmp,mountains_surround0,20,20,1000,14,4,1000,20,16,1000,19,10,1000,23,22,1000,40,28,0,42,45,0", "tmp,1000mountains,26,13,43,14,35,13,40,19,33,16"}, {"tmp", "tmp", "tmp,mountains_surround0,28,22,303,32,19,255", "tmp,mountains_surround0,21,22,299,24,26,363", "tmp,mountains_surround0,20,23,306,29,22,356", "tmp,mountains_surround0,5,17,52,28,29,228,23,19,481", "tmp,mountains_surround0,27,29,392,23,25,557", "tmp,mountains_surround0,29,21,594,24,23,415,26,18,322", "tmp,mountains_surround0,17,24,455,24,24,654", "tmp,mountains_surround0,19,25,669,30,24,412,26,25,444,21,27,595", "tmp,mountains_surround0,18,18,335,15,27,777,20,25,580", "tmp,mountains_surround0,26,31,564,30,27,938", "tmp,mountains_surround0,18,19,507,23,10,380,24,20,932", "tmp,mountains_surround0,17,19,964,19,26,619", "tmp,mountains_surround0,18,27,960,21,26,696,12,11,261", "tmp,mountains_surround0,22,24,788,21,15,1000", "tmp,mountains_surround0,22,17,979,20,20,865", "tmp,mountains_surround0,26,24,984,22,30,604", "tmp,1000mountains,9,14,16,12,13,17", "tmp,mountains_surround0,28,9,1000,20,17,773,2,21,72,21,19,594,19,23,591", "tmp,mountains_surround0,14,26,1000,20,15,999", "tmp,1000mountains,27,9,18,13,21,15", "tmp,1000mountains,22,21,25,9", "tmp,1000mountains,26,30,24,25,41,25", "tmp,1000mountains,13,12,7,9,16,4", "tmp,1000mountains,15,29,3,29,10,34,8,19,16,25", "tmp,1000mountains,40,21,44,27,36,29,30,29", "tmp,1000mountains,22,20,3,26,20,17,28,18,12,11", "tmp,1000mountains,38,18,32,11,44,17,32,20,33,27", "tmp,1000mountains,44,26,38,24,34,36,42,29,40,33"}, {"tmp", "tmp,mountains_surround0,18,20,138,19,12,177,30,23,223", "tmp,mountains_surround0,31,18,263,13,22,212,24,11,129", "tmp,mountains_surround0,26,19,341,24,25,314", "tmp,mountains_surround0,14,28,139,30,19,346,34,19,340", "tmp,mountains_surround0,30,22,430,33,30,279,28,26,394", "tmp,mountains_surround0,26,30,524,19,27,428", "tmp,mountains_surround0,24,17,696,21,20,547", "tmp,mountains_surround0,28,21,500,29,22,641,23,26,325", "tmp,mountains_surround0,25,16,747,34,13,288,28,21,550", "tmp,mountains_surround0,29,28,773,24,26,551", "tmp,mountains_surround0,29,19,907,20,13,413", "tmp,mountains_surround0,32,20,983,44,23,179,28,25,539,32,17,621", "tmp,mountains_surround0,34,17,461,32,29,1000", "tmp,mountains_surround0,24,18,723,21,15,399,30,22,1000", "tmp,1000mountains,30,20,30,18", "tmp,1000mountains,33,26,33,31", "tmp,1000mountains,29,18,34,18", "tmp,1000mountains,31,37,21,37", "tmp,mountains_surround0,37,22,999,15,1,74,23,18,600", "tmp", "tmp,1000mountains,27,18,29,15,32,9", "tmp,1000mountains,40,18,29,23", "tmp,1000mountains,42,19,27,34,33,28", "tmp,1000mountains,22,13,22,18,25,5,27,18", "tmp,mountains_surround0,32,29,1000,42,28,1000,34,27,1000,33,3,0", "tmp,1000mountains,35,46,21,26,26,29,24,38,34,40", "tmp,1000mountains,29,45,20,34,22,40,27,40", "tmp,mountains_surround0,28,7,1000,34,4,1000,34,15,1000,32,23,1000,27,19,1000,37,2,0,1,5,0", "tmp,mountains_surround0,21,32,1000,30,44,1000,23,41,1000,19,39,1000,16,43,1000,44,25,0,5,19,0,6,15,0"}, {"tmp", "tmp,mountains_surround0,31,14,124,19,26,219,28,35,104,12,15,180", "tmp,mountains_surround0,30,35,206,27,21,306,27,21,96", "tmp,mountains_surround0,16,24,311,22,24,391", "tmp,mountains_surround0,23,28,318,21,21,331,45,20,3,21,26,418", "tmp,mountains_surround0,35,35,105,28,21,338,31,23,413,23,23,266", "tmp,mountains_surround0,23,29,518,26,24,411", "tmp,mountains_surround0,27,29,425,24,28,546,19,31,388,15,46,77,7,8,22", "tmp,mountains_surround0,21,23,642,16,27,301,25,21,403,6,14,199", "tmp,mountains_surround0,33,28,754,26,24,395", "tmp,mountains_surround0,27,22,835,27,28,501", "tmp,mountains_surround0,13,25,353,35,20,262,21,24,909,11,33,312", "tmp,mountains_surround0,34,26,549,30,25,888", "tmp,mountains_surround0,25,24,963,10,13,237", "tmp,1000mountains,32,25,27,22", "tmp,1000mountains,18,29,17,24", "tmp,1000mountains,29,26,34,25", "tmp,1000mountains,28,30,31,27", "tmp,1000mountains,20,32,10,35", "tmp,mountains_surround0,30,25,724,22,31,999,16,36,58", "tmp,1000mountains,9,30,18,31,22,41", "tmp,1000mountains,15,16,11,24", "tmp,1000mountains,40,23,34,25,45,25", "tmp,mountains_surround0,39,6,1000,25,19,1000,37,15,1000,28,43,0,18,44,0,13,45,0,5,44,0,1,6,0", "tmp,mountains_surround0,29,40,1000,37,37,1000,34,45,1000,42,29,1000,26,8,0,30,6,0,17,21,0,11,46,0,11,8,0", "tmp,1000mountains,33,36,34,29,42,22,46,19,34,25", "tmp,1000mountains,11,25,4,22,16,29,17,25", "tmp,1000mountains,2,24,7,14,19,19,16,24,7,24", "tmp,1000mountains,39,8,35,18,28,15,30,24,47,14", "tmp,1000mountains,11,32,2,36,3,32,18,33,20,30"}, {"tmp", "tmp", "tmp,mountains_surround0,21,24,348,32,35,112", "tmp,mountains_surround0,30,31,162,19,25,346,29,19,149,23,32,244", "tmp,mountains_surround0,16,30,259,22,30,456,15,16,202", "tmp,mountains_surround0,21,32,430,25,29,408", "tmp,mountains_surround0,29,27,577,24,31,368", "tmp,mountains_surround0,27,18,462,27,26,601", "tmp,mountains_surround0,20,31,704,34,29,361", "tmp,mountains_surround0,29,23,721,34,29,436", "tmp,mountains_surround0,9,36,321,28,33,465,20,32,797", "tmp,mountains_surround0,23,15,340,26,26,718,24,24,955", "tmp,mountains_surround0,27,34,419,21,32,930,14,30,527", "tmp,mountains_surround0,28,33,610,22,30,960", "tmp,1000mountains,24,32,20,34", "tmp,mountains_surround0,22,35,956,19,33,509,32,33,539", "tmp,mountains_surround0,38,25,727,19,37,65,33,27,1000,38,23,852", "tmp,1000mountains,30,28,26,32", "tmp,mountains_surround0,27,39,1000,29,30,1000,48,34,0,10,14,0", "tmp,1000mountains,20,34,27,31", "tmp,1000mountains,30,30,25,34", "tmp,1000mountains,24,28,19,35", "tmp,1000mountains,24,45,29,35,26,33", "tmp,mountains_surround0,6,24,1000,15,21,1000,10,18,1000,7,37,0,37,25,0", "tmp,mountains_surround0,10,26,1000,4,22,1000,19,24,1000,26,27,1000,49,27,0,8,14,0", "tmp,1000mountains,37,19,36,29,35,27,47,23", "tmp,1000mountains,13,18,3,12,22,12,10,13", "tmp,1000mountains,13,27,2,32,15,38,9,42,9,32", "tmp,1000mountains,31,38,27,46,19,28,26,33,30,36", "tmp,1000mountains,9,17,13,22,3,25,5,16,17,19"}};


// ---------------------------------------- variables ----------------------------------------

/* const bool use_KL = bool(1); */
bool use_KL = bool(1);

constexpr int Height_max = 1000;
/* constexpr int Measure_max = 10000; */
int Measure_max = 10000;
// constexpr int Measure_max = 100;


constexpr int Ndir = 4;
constexpr int dx[Ndir] = {1, 0, -1, 0};
constexpr int dy[Ndir] = {0, 1, 0, -1};

constexpr double V_height_randomness[31] = {
    0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                     // 1-10
    0, 0, 0.1, 0.1, 0.1, 0.2, 0.2, 0.2, 0.3, 0.3,     // 11-20
    0.3, 0.4, 0.4, 0.5, 0.6, 0.7, 0.7, 0.8, 0.8, 0.9  // 21-30
};                                                    // どれだけ height にノイズを持たせるか

constexpr int V_height_coef[31] = {
    0,
    2, 3, 4, 5, 6, 7, 9, 10, 10, 10,         // 1-10
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  // 11-20
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  // 21-30
};

constexpr int V_N_use_KL[31] = {
    0,
    // 10000, 1000, 100, 30, 10, 10, 10, 10, 10, 10,  // 1-10
    // 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,        // 11-20
    // 10, 10, 10, 10, 10, 10, 10, 10, 10, 5          // 21-30
    100000, 1000, 1000, 500, 500, 500, 500, 300, 200, 100,  // 1-100
    100, 100, 80, 60, 50, 40, 30, 20, 10, 10,               // 11-20
    10, 10, 10, 10, 10, 10, 10, 10, 10, 5                   // 21-30
    // 100000, 10000, 1000, 30, 100, 100, 100, 100, 100, 100,  // 1-100
    // 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,       // 11-20
    // 100, 100, 100, 100, 100, 100, 100, 100, 100, 50         // 21-30
    // 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000,  // 1-10
    // 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000,  // 11-20
    // 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000   // 21-30
};
int N_use_KL = -1;

using Pos = pair<int, int>;
using Vpos = vector<Pos>;
// using Tprob = long double;
using Tprob = double;

int L, N, S;
double height_randomness = 0;
int height_coef = 10;
Vpos Vpos_out;
vector<int> Vpos_out_x, Vpos_out_y;

vector<vector<Tprob>> Mprob_seq(1111, vector<Tprob>(1111, -1));  // Mprob_seq[height][meas] = P(meas | height)
const int erf_memo_len = 10000;
vector<Tprob> erf_memo(erf_memo_len);
vector<Tprob> Ventropy(Height_max + 1, 0.);                                   // sum_m -P(m|i)*log(P(m|i)) with height as index
vector<vector<Tprob>> KL(Height_max + 1, vector<Tprob>(Height_max + 1, 0.));  // KL[h(i)][h(j)] = KL_m(P(m|i) || P(m|j))

vector<vector<int>> board_init;

bool not_enough_time = false;


// ---------------------------------------- basic functions ----------------------------------------

vector<string> sep_string(string commaSeparated) {
    std::istringstream iss(commaSeparated);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(iss, token, ',')) {
        tokens.push_back(token);
    }

    return tokens;
}

std::vector<std::string> convertToVector(int argc, char* argv[]) {
    std::vector<std::string> result;

    for (int i = 0; i < argc; ++i) {
        result.emplace_back(argv[i]);
    }

    return result;
}

double dist(Pos p1, Pos p2) {
    auto [x1, y1] = p1;
    auto [x2, y2] = p2;
    int dx = min(abs(x1 - x2), L - abs(x1 - x2));
    int dy = min(abs(y1 - y2), L - abs(y1 - y2));
    return sqrt(dx * dx + dy * dy);
}

int dist_l1(Pos p1, Pos p2) {
    auto [x1, y1] = p1;
    auto [x2, y2] = p2;
    int dx = min(abs(x1 - x2), L - abs(x1 - x2));
    int dy = min(abs(y1 - y2), L - abs(y1 - y2));
    return dx + dy;
}

int dist_int(Pos p1, Pos p2) {
    return int(dist(p1, p2) + 0.5);
}

Pos operator+(Pos p, int idx) {
    p.first += dx[idx];
    p.first = (p.first == L ? 0 : p.first);
    p.first = (p.first == -1 ? L - 1 : p.first);
    p.second += dy[idx];
    p.second = (p.second == L ? 0 : p.second);
    p.second = (p.second == -1 ? L - 1 : p.second);
    return p;
}
Pos operator+(Pos p1, Pos p2) {
    auto [x1, y1] = p1;
    auto [x2, y2] = p2;
    x1 += x2;
    y1 += y2;
    x1 = (x1 >= L ? x1 - L : (x1 < 0 ? x1 + L : x1));
    y1 = (y1 >= L ? y1 - L : (y1 < 0 ? y1 + L : y1));
    return Pos(x1, y1);
}
Pos operator+=(Pos& p1, Pos p2) {
    p1 = p1 + p2;
    return p1;
}
Pos operator-(Pos p1, Pos p2) {
    auto [x1, y1] = p1;
    auto [x2, y2] = p2;
    x1 -= x2;
    y1 -= y2;
    x1 = (x1 >= L ? x1 - L : (x1 < 0 ? x1 + L : x1));
    y1 = (y1 >= L ? y1 - L : (y1 < 0 ? y1 + L : y1));
    return Pos(x1, y1);
}
Pos operator-=(Pos& p1, Pos p2) {
    p1 = p1 - p2;
    return p1;
}
Pos postprocess_dpos(Pos dpos) {
    if (dpos.first > L / 2)
        dpos.first -= L;
    if (dpos.first < -L / 2)
        dpos.first += L;
    if (dpos.second > L / 2)
        dpos.second -= L;
    if (dpos.second < -L / 2)
        dpos.second += L;
    return dpos;
}

inline Tprob calc_prob_seq(int height, int meas) {
    auto& prob = Mprob_seq[height][meas];
    if (prob >= 0)
        return prob;
    prob = 0;
    if (meas == 0) {
        /* for (int i = -S * 2; i <= 0; i++) { */
        /*     prob += exp(Tprob(-pow(abs(height - i), 2) / 2.0 / S / S)); */
        /* } */
        prob = erf_memo[height];
    } else if (meas == 1000) {
        /* for (int i = 1000 + S * 2; i >= 1000; i--) { */
        /*     prob += exp(Tprob(-pow(abs(height - i), 2) / 2.0 / S / S)); */
        /* } */
        prob = erf_memo[1000 - height];
    } else {
        prob = exp(Tprob(-pow(abs(height - meas), 2) / 2.0 / S / S));
        prob /= sqrt(2 * M_PI) * S;
    }
    return prob;
}

Tprob calc_xlogx(const Tprob& x);

Tprob calc_entropy(vector<Tprob>& Vprob) {
    Tprob entropy = 0;
    for (auto&& prob : Vprob) {
        if (prob == 0)
            continue;
        // Tprob entropy_tmp = -prob * log(prob);
        Tprob entropy_tmp = -calc_xlogx(prob);
        if (!isfinite(entropy_tmp))
            continue;
        if (entropy_tmp <= 0)
            continue;
        entropy += entropy_tmp;
    }
    return entropy;
}


float __int_as_float(int32_t a) {
    float r;
    memcpy(&r, &a, sizeof r);
    return r;
}
int32_t __float_as_int(float a) {
    int32_t r;
    memcpy(&r, &a, sizeof r);
    return r;
}

// https://stackoverflow.com/questions/39821367/very-fast-approximate-logarithm-natural-log-function-in-c
/* compute natural logarithm, maximum error 0.85089 ulps */
float my_logf(float a) {
    float i, m, r, s, t;
    int e;

#if PORTABLE
    m = frexpf(a, &e);
    if (m < 0.666666667f) {
        m = m + m;
        e = e - 1;
    }
    i = (float)e;
#else   // PORTABLE
    i = 0.0f;
    if (a < 1.175494351e-38f) {  // 0x1.0p-126
        a = a * 8388608.0f;      // 0x1.0p+23
        i = -23.0f;
    }
    e = (__float_as_int(a) - __float_as_int(0.666666667f)) & 0xff800000;
    m = __int_as_float(__float_as_int(a) - e);
    i = fmaf((float)e, 1.19209290e-7f, i);  // 0x1.0p-23
#endif  // PORTABLE
    /* m in [2/3, 4/3] */
    m = m - 1.0f;
    s = m * m;
    /* Compute log1p(m) for m in [-1/3, 1/3] */
    r = -0.130310059f;              // -0x1.0ae000p-3
    t = 0.140869141f;               //  0x1.208000p-3
    r = fmaf(r, s, -0.121483512f);  // -0x1.f198b2p-4
    t = fmaf(t, s, 0.139814854f);   //  0x1.1e5740p-3
    r = fmaf(r, s, -0.166846126f);  // -0x1.55b36cp-3
    t = fmaf(t, s, 0.200120345f);   //  0x1.99d8b2p-3
    r = fmaf(r, s, -0.249996200f);  // -0x1.fffe02p-3
    r = fmaf(t, m, r);
    r = fmaf(r, m, 0.333331972f);   //  0x1.5554fap-2
    r = fmaf(r, m, -0.500000000f);  // -0x1.000000p-1
    r = fmaf(r, s, m);
    r = fmaf(i, 0.693147182f, r);  //  0x1.62e430p-1 // log(2)
    if (!((a > 0.0f) && (a < INFINITY))) {
        r = a + a;  // silence NaNs if necessary
        if (a < 0.0f)
            r = INFINITY - INFINITY;  //  NaN
        if (a == 0.0f)
            r = -INFINITY;
    }
    return r;
}

/* natural log on [0x1.f7a5ecp-127, 0x1.fffffep127]. Maximum relative error 9.4529e-5 */
float my_faster_logf(float a) {
    float m, r, s, t, i, f;
    int32_t e;

    e = (__float_as_int(a) - 0x3f2aaaab) & 0xff800000;
    m = __int_as_float(__float_as_int(a) - e);
    i = (float)e * 1.19209290e-7f;  // 0x1.0p-23
    /* m in [2/3, 4/3] */
    f = m - 1.0f;
    s = f * f;
    /* Compute log1p(f) for f in [-1/3, 1/3] */
    r = fmaf(0.230836749f, f, -0.279208571f);  // 0x1.d8c0f0p-3, -0x1.1de8dap-2
    t = fmaf(0.331826031f, f, -0.498910338f);  // 0x1.53ca34p-2, -0x1.fee25ap-2
    r = fmaf(r, s, t);
    r = fmaf(r, s, f);
    r = fmaf(i, 0.693147182f, r);  // 0x1.62e430p-1 // log(2)
    return r;
}

// https://stackoverflow.com/questions/67521312/safe-equivalent-of-stdbit-cast-in-c11
template <class T2, class T1>
T2 cpp11_bit_cast(T1 t1) {
    static_assert(sizeof(T1) == sizeof(T2), "Types must match sizes");
    static_assert(std::is_pod<T1>::value, "Requires POD input");
    static_assert(std::is_pod<T2>::value, "Requires POD output");

    T2 t2;
    std::memcpy(std::addressof(t2), std::addressof(t1), sizeof(T1));
    return t2;
}

// https://github.com/nadavrot/fast_log/blob/main/src/log_approx.cc
/// @returns the exponent and a normalized mantissa with the relationship:
/// [a * 2^b] = x
std::pair<double, int> my_frexp(double x) {
    /* uint64_t bits = bit_cast<uint64_t, double>(x); */
    uint64_t bits = cpp11_bit_cast<uint64_t, double>(x);
    if (bits == 0) {
        return {0., 0};
    }
    // See:
    // https://en.wikipedia.org/wiki/IEEE_754#Basic_and_interchange_formats
    // Extract the 52-bit mantissa field.
    uint64_t mantissa = bits & 0xFFFFFFFFFFFFF;
    bits >>= 52;
    // Extract the 11-bit exponent field, and add the bias.
    int exponent = int(bits & 0x7ff) - 1023;
    bits >>= 11;
    // Extract the sign bit.
    uint64_t sign = bits;
    bits >>= 1;
    // Construct the normalized double;
    uint64_t res = sign;
    res <<= 11;
    res |= 1023 - 1;
    res <<= 52;
    res |= mantissa;
    /* double frac = bit_cast<double, uint64_t>(res); */
    double frac = cpp11_bit_cast<double, uint64_t>(res);
    return {frac, exponent + 1};
}
double __attribute__((noinline)) fastlog2(double x) {
    /// Extract the fraction, and the power-of-two exponent.
    auto a = my_frexp(x);
    x = a.first;
    int pow2 = a.second;
    // Use a 4-part polynom to approximate log2(x);
    double c[] = {1.33755322, -4.42852392, 6.30371424, -3.21430967};
    double log2 = 0.6931471805599453;
    // Use Horner's method to evaluate the polynomial.
    double val = c[3] + x * (c[2] + x * (c[1] + x * (c[0])));
    // Compute log2(x), and convert the result to base-e.
    /* return log2 * (pow2 + val); */
    return log2 * (pow2 + val);
}
double __attribute__((noinline)) fastlog(double x) {
    /// Extract the fraction, and the power-of-two exponent.
    auto a = my_frexp(x);
    x = a.first;
    int pow2 = a.second;
    // Use a 4-part polynom to approximate log2(x);
    double c[] = {1.33755322, -4.42852392, 6.30371424, -3.21430967};
    // Use Horner's method to evaluate the polynomial.
    double val = c[3] + x * (c[2] + x * (c[1] + x * (c[0])));
    // Compute log2(x), and convert the result to base-e.
    /* return log2 * (pow2 + val); */
    return (pow2 + val);
}


std::pair<float, int> my_frexp_float(float x) {
    /* uint64_t bits = bit_cast<uint64_t, float>(x); */
    uint32_t bits = cpp11_bit_cast<uint32_t, float>(x);
    if (bits == 0) {
        return {0., 0};
    }
    // See:
    // https://en.wikipedia.org/wiki/IEEE_754#Basic_and_interchange_formats
    // Extract the 23-bit mantissa field.
    uint32_t mantissa = bits & 0x7FFFFF;
    bits >>= 23;
    // Extract the 8-bit exponent field, and add the bias.
    int exponent = int(bits & 0xff) - 127;
    bits >>= 8;
    // Extract the sign bit.
    uint32_t sign = bits;
    bits >>= 1;
    // Construct the normalized float;
    uint32_t res = sign;
    res <<= 8;
    res |= 127 - 1;
    res <<= 23;
    res |= mantissa;
    /* float frac = bit_cast<float, uint32_t>(res); */
    float frac = cpp11_bit_cast<float, uint32_t>(res);
    return {frac, exponent + 1};
}
float __attribute__((noinline)) fastlog2_float(float x) {
    /// Extract the fraction, and the power-of-two exponent.
    auto a = my_frexp_float(x);
    x = a.first;
    int pow2 = a.second;
    // Use a 4-part polynom to approximate log2(x);
    float c[] = {1.33755322, -4.42852392, 6.30371424, -3.21430967};
    float log2 = 0.6931471805599453;
    // Use Horner's method to evaluate the polynomial.
    float val = c[3] + x * (c[2] + x * (c[1] + x * (c[0])));
    // Compute log2(x), and convert the result to base-e.
    /* return log2 * (pow2 + val); */
    return log2 * (pow2 + val);
}

/* const int log_memo_len = 100000; */
/* const double log_memo_len_double = double(log_memo_len); */
/* double log_memo[log_memo_len + 1] = {}; */

double __attribute__((noinline)) myfastxlogx(double x) {
    // double c[] = {6149. / 560., -7513. / 280., 13849. / 560, -1569. / 140, 1949. / 560.};
    // double val = x * (x - 1) * (c[4] + x * (c[3] + x * (c[2] + x * (c[1] + x * c[0]))));
    // double c[] = {91. / 60., -133. / 60.};
    // double val = (c[1] + x * c[0]) * x * (1 - x);
    // double c[] = {91. / 60., -133. / 60.};
    // double val = (c[1] + x * c[0]) * x * (1 - x);
    // double c[] = {-1.1666692353790284, 3.0000049037371, -1.783336035342495, -0.06666627313028406};
    // double val = (c[3] + x * (c[2] + x * (c[1] + x * c[0])));
    double c[] = {-4.87142856, 14.62857141, -16.97142856, 10.36190476, -3.14761905};
    double val = x * (c[4] + x * (c[3] + x * (c[2] + x * (c[1] + x * c[0]))));
    return val;
}

Tprob calc_xlogx(const Tprob& x) {
    if (x == 0)
        return 0;
    Tprob ret = x * log(x);
    /* Tprob ret = x * my_logf(x); */
    /* Tprob ret = x * my_faster_logf(x); */
    // Tprob ret = x * fastlog2(x);  // TODO より高次まで
    // Tprob ret = x * fastlog(x);
    /* Tprob ret = x * fastlog2_float(x); */
    /* Tprob ret = x * log_memo[int(x * log_memo_len_double)]; */
    // Tprob ret = myfastxlogx(x);
    // Tprob ret;
    // if (x > 0.5)
    //     ret = x * log(x);
    // else
    //     ret = x * log1p(x - 1);

    // if (!isfinite(ret))
    //     return 0;
    if (ret >= 0)
        return 0;
    return ret;
}

inline int calc_measure_cost(int x, int y) {
    x = min(x, L - x);
    y = min(y, L - y);
    return 100 * (10 + abs(x) + abs(y));
}

inline int calc_measure_cost(Pos dpos) {
    auto [x, y] = dpos;
    return calc_measure_cost(x, y);
}


int measure(int idx_in, int x, int y) {
    cout << idx_in << " " << y << " " << x << endl;
    int m;
    cin >> m;
    assert(m != -1);
    return m;
}
int measure(int idx_in, Pos dpos) {
    auto [x, y] = dpos;
    return measure(idx_in, x, y);
}

void print_ans(vector<int> Vans) {
    assert((int)Vans.size() == N);
    cout << -1 << " " << -1 << " " << -1 << endl;
    for (auto&& idx : Vans) {
        cout << idx << endl;
    }
    return;
}


// ---------------------------------------- classes ----------------------------------------

class Board {
public:
    vector<vector<int>> Mheight;            // 盤面
    vector<vector<double>> Mheight_double;  // 盤面
    vector<pair<Pos, int>> Vfixed;          // 値が固定されたマス
    vector<vector<int>> Mfixed;
    Board() {
        Mheight.resize(L, vector<int>(L, -1));
        Mfixed.resize(L, vector<int>(L, -1));
    }

    void initialize() {
        for (int i = 0; i < L; i++) {
            for (int j = 0; j < L; j++) {
                Mheight[i][j] = -1;
                Mfixed[i][j] = -1;
            }
        }
        Vfixed.clear();
    }

    void expand_Mheight() {
        Mheight.resize(L * 2);
        for (int i = 0; i < L; i++) {
            auto& row = Mheight[i];
            row.resize(L * 2, -1);
            for (int i = 0; i < L; i++) {
                row[i + L] = row[i];
            }
            Mheight[i + L] = row;
        }
        Mheight_double.resize(L * 2, vector<double>(L * 2, -1));
        for (int x = 0; x < L * 2; x++) {
            for (int y = 0; y < L * 2; y++) {
                Mheight_double[x][y] = double(Mheight[x][y]);
            }
        }
    }

    void add_randomness(double ratio = 1.0) {
        ratio = clamp(ratio, 0.0, 1.0);
        for (int x = 0; x < L; x++) {
            for (int y = 0; y < L; y++) {
                int rand = randrange(1001);
                Mheight[x][y] = int(Mheight[x][y] * (1 - ratio) + rand * ratio);
            }
        }
    }

    void test_pattern_1() {
        for (int x = 0; x < L; x++) {
            for (int y = 0; y < L; y++) {
                double period = 0.5;
                double pos = 1. * (x + y) / L * period;
                int val;
                if (int(pos * 2) % 2 == 0)
                    val = int(pos * 1000) % 1000;
                else
                    val = 1000 - int(pos * 1000) % 1000;
                Mheight[x][y] = val;
            }
        }
    }

    void test_pattern_2() {
        for (int x = 0; x < L; x++) {
            for (int y = 0; y < L; y++) {
                double d = dist_l1(Pos{x, y}, Pos{L / 2, L / 2});
                // int val = int(d / L * 300);
                // val += 400;
                int val = int(d / L * 500);
                // val += 400;
                Mheight[x][y] = val;

                if (x < L / 2 && y < L / 2) {
                    // Mheight[x][y] += 300;
                    Mheight[x][y] += 500;
                }
            }
        }
    }

    void test_pattern_3() {
        for (int x = 0; x < L; x++) {
            for (int y = 0; y < L; y++) {
                double period = 0.66;
                // int val = int(1. * (x + y) / L * period * 1000) % 1000;
                int pos = int(1. * (x + y) / L * period * 1000);
                int val;
                if (pos <= 1000)
                    val = pos;
                else
                    // val = (2000 - pos);
                    val = 1000 - (pos - 1000) * 3;
                Mheight[x][y] = val;
            }
        }
    }

    void test_pattern_4() {
        for (int x = 0; x < L; x++) {
            for (int y = 0; y < L; y++) {
                double period = 0.6;
                int val = int(1. * (x + y) / L * period * 1000) % 1000;
                Mheight[x][y] = val;
            }
        }
    }

    void test_pattern_5() {
        int range = 1000;
        for (int x = 0; x < L; x++) {
            for (int y = 0; y < L; y++) {
                double val;
                // val = randrange(1001);
                if (x < y) {
                    val = 1. * x / L * range;
                } else {
                    val = 1. * y / L * range;
                }
                val += (1000 - range) / 2;
                Mheight[x][y] = int(val);
            }
        }
    }

    void test_pattern_6() {
        int range = 600;
        for (int x = 0; x < L; x++) {
            for (int y = 0; y < L; y++) {
                double val;
                // val = randrange(1001);
                if (x < y) {
                    val = 1. * x / L * range;
                } else {
                    val = 1. * y / L * range;
                }
                val += (1000 - range) / 2;
                Mheight[x][y] = int(val);
            }
        }
    }

    void test_pattern_7() {
        int range = 600;
        for (int x = 0; x < L; x++) {
            for (int y = 0; y < L; y++) {
                double val;
                // val = randrange(1001);
                if (x < y) {
                    val = 1. * x / L * range;
                } else {
                    val = 1. * y / L * range;
                }
                val += (1000 - range) / 2;
                val += 1. * (L - x) / L * range / 4;
                val += 1. * (L - y) / L * range / 4;
                val -= range / 4;
                Mheight[x][y] = int(val);
            }
        }
        // for (size_t i = 0; i < 1; i++) {
        //     average_random(1.);
        // }
        // Mheight[L / 2][L / 2] = 1000;
        // Mheight[0][0] = 1000;
        // for (int i = 0; i < 4; i++) {
        //     int x = L / 2 + dx[i];
        //     int y = L / 2 + dy[i];
        //     Mheight[x][y] = 750;
        // }
    }

    void test_pattern_8(int mode, int r) {
        initialize();
        if (mode == 6) {
            int d = 0;
            set({L / 2, L / 2}, r * 2);
            for (int i = 0; i < L; i++) {
                set({i, d}, 0);
                set({i, -d}, 0);
                set({d, i}, 0);
                set({-d, i}, 0);
            }
        } else if (mode == 7) {
            int d = 0;
            set({L / 2, L / 2}, r * 2);
            for (int i = 0; i < L / 2; i++) {
                set({d + i, L / 2 - i}, 0);
                set({d + i, L / 2 + i}, 0);
                set({L - 1 - d - i, L / 2 - i}, 0);
                set({L - 1 - d - i, L / 2 + i}, 0);
            }
        } else if (mode == 8) {
            int d = 0;
            set({L / 2, L / 2}, r * 2);
            for (int i = 0; i < L; i++) {
                set({i, d}, 0);
                set({i, -d}, 0);
                set({d, i}, 0);
                set({-d, i}, 0);
            }
        }


        setup_board(1000);

        if (mode == 8) {
            for (int x = 0; x < L; x++) {
                for (int y = 0; y < L; y++) {
                    if (x == L / 2 && y == L / 2)
                        continue;
                    Mheight[x][y] = int(Mheight[x][y] * 0.8);
                }
            }
        }

        return;
    }

    void test_pattern_9(int mode, int r = 500, int d1 = 0, int d2 = 0) {
        initialize();
        if (mode == 0) {
            /* set({L / 2, L / 2}, r * 2); */
            set({d1, L / 2}, r * 2);
            set({L / 2, d1}, r * 2);
            set({-d1, L / 2}, r * 2);
            set({L / 2, -d1}, r * 2);
            for (int i = 0; i < L; i++) {
                set({i, d2}, 0);
                set({i, -d2}, 0);
                set({d2, i}, 0);
                set({-d2, i}, 0);
            }
            setup_board(1000);
        }
    }


    void test_pattern_10(int mode, int r = 500, int d1 = 0, int d2 = 0) {
        initialize();
        if (mode == 0) {
            /* set({L / 2, L / 2}, r * 2); */
            set({d1, L / 2}, r * 2);
            set({L / 2, d1}, r * 2);
            set({-d1, L / 2}, r * 2);
            set({L / 2, -d1}, r * 2);
            set({L / 2, L / 2}, 0);
            for (int i = 0; i < L; i++) {
                set({i, d2}, 0);
                set({i, -d2}, 0);
                set({d2, i}, 0);
                set({-d2, i}, 0);
            }
            setup_board(1000);
        }
    }


    void flatten_quarter() {
        for (int x = 0; x < L; x++) {
            for (int y = 0; y < L; y++) {
                if (x % 2 == 0 && y % 2 == 0) {

                    // take average
                    double val = 0;
                    for (int i = 0; i < 4; i++) {
                        val += get(Pos{x, y} + i);
                    }
                    val /= 4;
                    Mheight[x][y] = int(val);
                }
            }
        }
    }

    void average_random(double ratio) {
        ratio = clamp(ratio, 0.0, 1.0);
        for (int x = 0; x < L; x++) {
            for (int y = 0; y < L; y++) {
                if (randrange(1001) < ratio * 1000) {
                    // take average
                    double val = 0;
                    for (int i = 0; i < 4; i++) {
                        val += get(Pos{x, y} + i);
                    }
                    val /= 4;
                    Mheight[x][y] = int(val);
                }
            }
        }
    }

    void set(Pos pos, int val) {
        auto& [x, y] = pos;
        if (x < 0)
            x += L;
        if (y < 0)
            y += L;
        Vfixed.emplace_back(pos, val);
        Mfixed[x][y] = val;
        Mheight[x][y] = val;
    }

    int get(Pos pos) {
        auto [x, y] = pos;
        return Mheight[x][y];
    }

    inline int get_fast(int x, int y) {
        return Mheight[x][y];
    }
    inline double get_fast_double(int x, int y) {
        return Mheight_double[x][y];
    }

    void setup_board(int iter_max = 1000) {
        // assert((int)Vfixed.size() == N);

        vector<vector<double>> Mheight_double(L, vector<double>(L, -2));
        for (int x = 0; x < L; x++) {
            for (int y = 0; y < L; y++) {
                double inverse_dist_sum = 0;
                double val_weighted_sum = 0;
                for (auto&& [pos, val] : Vfixed) {
                    if (pos == Pos(x, y)) {
                        Mheight_double[x][y] = val;
                        inverse_dist_sum = -1;
                        break;
                    }
                    double d = dist_int(pos, Pos(x, y));
                    // inverse_dist_sum += 1. / d / d;
                    // val_weighted_sum += val / d / d;
                    inverse_dist_sum += 1. / d;
                    val_weighted_sum += val / d;
                }
                if (inverse_dist_sum == -1)
                    continue;
                double height = val_weighted_sum / inverse_dist_sum;
                Mheight_double[x][y] = height;
            }
        }

        double lr = 1;
        for (int iter = 0; iter < iter_max; iter++) {
            // bool flag_update = false;
            for (int x = 0; x < L; x++) {
                for (int y = 0; y < L; y++) {
                    if (Mfixed[x][y] != -1)
                        continue;
                    double grad = 0;
                    for (int idx = 0; idx < Ndir; idx++) {
                        auto [nx, ny] = Pos(x, y) + idx;
                        grad += Mheight_double[nx][ny] - Mheight_double[x][y];
                    }
                    grad /= Ndir;
                    Mheight_double[x][y] += lr * grad;
                }
            }
        }

        for (int x = 0; x < L; x++) {
            for (int y = 0; y < L; y++) {
                Mheight[x][y] = int(Mheight_double[x][y] + 0.5);
            }
        }

        return;
    }

    // void print_board() {
    //     for (int x = 0; x < L; x++) {
    //         for (int y = 0; y < L; y++) {
    //             cerr << Mheight[x][y] << " ";
    //         }
    //         cerr << endl;
    //     }
    // }

    friend ostream& operator<<(ostream& os, const Board& board) {
        for (int x = 0; x < L; x++) {
            for (int y = 0; y < L; y++) {
                os << board.Mheight[y][x] << " ";
            }
            os << endl;
        }
        return os;
    }
};

class Estimator {
public:
    vector<vector<Tprob>> Mprob;  // Mprob[idx_in][idx_out] = prob
                                  // sum(Mprob[idx_in][*]) = 1
    Estimator() {
        Mprob.resize(N, vector<Tprob>(N, 1. / N));
    }

    void add_measurement(int idx_in, int height_meas, vector<int> Vheight_out) {
        // if (idx_in == 11) {
        //     cerr << "meas : " << height_meas << endl;
        //     for (auto&& h : Vheight_out) {
        //         cerr << h << " ";
        //     }
        //     cerr << endl;
        // }
        for (int idx_out = 0; idx_out < N; idx_out++) {
            Mprob[idx_in][idx_out] *= calc_prob_seq(Vheight_out[idx_out], height_meas);
            // if (idx_in == 11)
            //     cerr << setprecision(10) << calc_prob_seq(Vheight_out[idx_out], height_meas) << " ";
        }
        // if (idx_in == 11)
        //     cerr << endl;

        // normalize
        Tprob sum = 0;
        for (int idx_out = 0; idx_out < N; idx_out++) {
            sum += Mprob[idx_in][idx_out];
        }
        for (int idx_out = 0; idx_out < N; idx_out++) {
            Mprob[idx_in][idx_out] /= sum;
        }

        return;
    }

    pair<int, Tprob> argmax(int idx_in) {
        int idx_out_max = -1;
        Tprob prob_max = -1;
        for (int idx_out = 0; idx_out < N; idx_out++) {
            if (chmax(prob_max, Mprob[idx_in][idx_out])) {
                idx_out_max = idx_out;
            }
        }
        return {idx_out_max, prob_max};
    }

    tuple<int, int, Tprob> argmax() {
        int idx_in_max = -1;
        int idx_out_max = -1;
        Tprob prob_max = 1;
        for (int idx_in = 0; idx_in < N; idx_in++) {
            auto [idx_out_max_tmp, prob_max_tmp] = argmax(idx_in);
            if (chmin(prob_max, prob_max_tmp)) {
                idx_in_max = idx_in;
                idx_out_max = idx_out_max_tmp;
            }
        }
        return {idx_in_max, idx_out_max, prob_max};
    }

    vector<Tprob> get_Vprob(int idx_in) {
        return Mprob[idx_in];
    }

    Tprob get_prob(int idx_in, int idx_out) {
        return Mprob[idx_in][idx_out];
    }

    vector<pair<int, Tprob>> argmax_multi(int idx_in, int num) {
        vector<pair<int, Tprob>> Vpair;
        for (int idx_out = 0; idx_out < N; idx_out++) {
            Vpair.emplace_back(idx_out, Mprob[idx_in][idx_out]);
        }
        sort(Vpair.begin(), Vpair.end(), [](auto& p1, auto& p2) { return p1.second > p2.second; });
        // partial_sort(Vpair.begin(), Vpair.begin() + num, Vpair.end(), [](auto& p1, auto& p2) { return p1.second > p2.second; });
        return vector<pair<int, Tprob>>(Vpair.begin(), Vpair.begin() + num);
    }

    // TODO entropy
};

// ---------------------------------------- library ----------------------------------------

ll convert_cost_double_2_logll(Tprob x) {
    Tprob x_log = log(x);
    ll magnification = 10000;
    /* ll y = (ll)INF + magnification * x_log; */
    ll y = -(ll)magnification * x_log;
    if (y < 0)
        return (ll)INF;
    if (!isfinite(y))
        return (ll)INF;
    chmax(y, 0ll);
    return y;
}

pair<vector<int>, Tprob> min_cost_bipartiate_matching(vector<tuple<int, int, Tprob>>& V, int no_use = -1) {
    assert(((int)V.size() % N) == 0);
    int num = (int)V.size() / N;
    vector<int> V_ret(N, -1);
    Tprob prob_ret = -1;

    mcf_graph<int, ll> graph(N * 2 + 2);
    int offset_idx = N;
    int s = N * 2;
    int t = N * 2 + 1;
    for (int i = 0; i < (int)V.size(); i++) {
        auto [u, v, cost] = V[i];
        int cap = 1;
        if (i == no_use)
            cap = 0;
        ll cost_ll = convert_cost_double_2_logll(cost);
        /* cerr << cost << " " << cost_ll << endl; */
        graph.add_edge(u, offset_idx + v, cap, cost_ll);
    }
    for (int i = 0; i < N; i++) {
        graph.add_edge(s, i, 1, 0);
        graph.add_edge(offset_idx + i, t, 1, 0);
    }
    auto [cap, cost] = graph.flow(s, t);

    // check if complete matching is made
    if (cap != N) {
        /* cerr << "cap =  " << cap << endl; */
        /* cerr << "N =  " << N << endl; */
        /* cerr << "prob_ret =  " << prob_ret << endl; */
        return {V_ret, prob_ret};
    }

    prob_ret = 1.;
    auto edges = graph.edges();
    for (int edge_idx = 0; edge_idx < (int)V.size(); edge_idx++) {
        auto& [u, v, p] = V[edge_idx];
        auto& edge = edges[edge_idx];
        int flow = edge.flow;
        if (flow == 1) {
            int in = edge.from;
            int out = edge.to - offset_idx;
            assert(0 <= in && in < N);
            assert(0 <= out && out < N);
            assert(V_ret[in] == -1);
            V_ret[in] = out;
            prob_ret *= p;
        } else if (flow != 0)
            assert(false);
    }

    // check if complete matching is made
    for (int i = 0; i < N; i++) {
        assert(V_ret[i] != -1);
    }

    return {V_ret, prob_ret};
}

int get_next_in_idx(Estimator& estimator, int mode = 0) {
    if (not_enough_time)
        mode = 0;
    int ret_in_idx = -1;
    if (mode == 0) {
        // 従来
        Tprob prob_thr_weak = 0.8;  // 重複していなければ ok とする TODO 順列の発生確率についてちゃんと考える
        Tprob min_max_prob = 1;
        int idx_in_min_max_prob = -1;
        vector<int> V_cnt(N, 0);
        vector<vector<pair<Tprob, int>>> V_duplicate(N, vector<pair<Tprob, int>>(0));
        for (int idx_in = 0; idx_in < N; idx_in++) {
            auto [idx_out_max, prob_max] = estimator.argmax(idx_in);
            V_duplicate[idx_out_max].emplace_back(prob_max, idx_in);
            V_cnt[idx_out_max]++;
            if (chmin(min_max_prob, prob_max)) {
                idx_in_min_max_prob = idx_in;
            }
        }
        for (int idx_out = 0; idx_out < N; idx_out++) {
            sort(begin(V_duplicate[idx_out]), end(V_duplicate[idx_out]));
        }
        int max_duplicate_cnt = *max_element(cbegin(V_cnt), cend(V_cnt));
        if (min_max_prob > prob_thr_weak && max_duplicate_cnt == 1) {
            ret_in_idx = -1;
            return ret_in_idx;
        }

        // 重複している要素のうち，確率が最も低いものを選ぶ
        if (max_duplicate_cnt >= 2) {
            Tprob min_prob = 1.;
            for (auto&& V : V_duplicate) {
                if ((int)V.size() == 1)
                    continue;
                for (auto&& [prob, idx_in] : V) {
                    if (chmin(min_prob, prob))
                        idx_in_min_max_prob = idx_in;
                }
            }
        }

        ret_in_idx = idx_in_min_max_prob;
    } else if (mode == 1) {
        Tprob prob_thr_min = 0.8;
        Tprob prob_ratio_thr = 0.01;
        auto [idx_in, idx_out, prob] = estimator.argmax();

        if (prob < prob_thr_min)
            return idx_in;

        {
            vector<int> Vcnt(N);
            for (int i = 0; i < N; i++) {
                auto [idx, p] = estimator.argmax(i);
                Vcnt[idx]++;
            }
            int max_dupl = *max_element(cbegin(Vcnt), cend(Vcnt));
            int cnt_dupl = 0;
            for (auto&& n : Vcnt) {
                if (n >= 2)
                    cnt_dupl += n;
            }
#ifdef FLAG_CERR
            cerr << "max_dupl : " << max_dupl << "  cnt_dupl : " << cnt_dupl << endl;
#endif
            if (cnt_dupl >= 3) {
                return get_next_in_idx(estimator, 0);
            }
        }


        int num_cand = N;

        vector<vector<pair<int, Tprob>>> V(N);
        for (int idx_in = 0; idx_in < N; idx_in++) {
            V[idx_in] = estimator.argmax_multi(idx_in, num_cand);
        }

        vector<tuple<int, int, Tprob>> Vmatching;
        Vmatching.reserve(N * num_cand);
        for (int idx_in = 0; idx_in < N; idx_in++) {
            for (auto [idx_out, prob] : V[idx_in]) {
                Vmatching.emplace_back(idx_in, idx_out, prob);
            }
        }

        auto [Matching, Prob_matching] = min_cost_bipartiate_matching(Vmatching);
#ifdef FLAG_CERR
        cerr << "prob : " << Prob_matching << endl;
#endif
        if (Prob_matching < 0)
            return idx_in;


        vector<int> use_edge(N, -1);
        for (int idx_in = 0; idx_in < N; idx_in++) {
            for (int j = 0; j < num_cand; j++) {
                if (Matching[idx_in] == V[idx_in][j].first) {
                    use_edge[idx_in] = j;
                    break;
                }
            }
        }

        for (auto&& i : use_edge) {
            assert(i >= 0);
        }

        bool flag_break = true;

        Tprob prob_min = 1.;

        Tprob prob_matching_nxt = 0.;
        double max_ratio = 0;
        for (int idx_in1 = 0; idx_in1 < N; idx_in1++) {
            for (int idx_in2 = 0; idx_in2 < idx_in1; idx_in2++) {
                int idx_out1 = Matching[idx_in1];
                int idx_out2 = Matching[idx_in2];
                int idx_out1_new = idx_out2;
                int idx_out2_new = idx_out1;
                Tprob prob = estimator.get_prob(idx_in1, idx_out1) * estimator.get_prob(idx_in2, idx_out2);
                Tprob prob_new = estimator.get_prob(idx_in1, idx_out1_new) * estimator.get_prob(idx_in2, idx_out2_new);
                if (prob_new > 0)
                    chmax(max_ratio, prob_new / prob);
                if (prob_new > prob * prob_ratio_thr) {
                    flag_break = false;
                    if (chmin(prob_min, estimator.get_prob(idx_in1, idx_out1)))
                        ret_in_idx = idx_in1;
                    if (chmin(prob_min, estimator.get_prob(idx_in2, idx_out2)))
                        ret_in_idx = idx_in2;
                }
            }
        }
#ifdef FLAG_CERR
        cerr << "max ratio = " << max_ratio << endl;
#endif


        /* cerr << ret_in_idx << " " << prob_min << endl; */

        if (flag_break)
            ret_in_idx = -1;

    } else if (mode == 2) {
        Tprob prob_thr_min = 0.5;
        Tprob prob_ratio_thr = 0.01;
        auto [idx_in, idx_out, prob] = estimator.argmax();

        if (prob < prob_thr_min)
            return idx_in;

        {
            vector<int> Vcnt(N);
            for (int i = 0; i < N; i++) {
                auto [idx, p] = estimator.argmax(i);
                Vcnt[idx]++;
            }
            int max_dupl = *max_element(cbegin(Vcnt), cend(Vcnt));
            int cnt_dupl = 0;
            for (auto&& n : Vcnt) {
                if (n >= 2)
                    cnt_dupl += n;
            }
#ifdef FLAG_CERR
            cerr << "max_dupl : " << max_dupl << "  cnt_dupl : " << cnt_dupl << endl;
#endif
            if (cnt_dupl >= 3) {
                return get_next_in_idx(estimator, 0);
            }
        }


        int num_cand = N;

        vector<vector<pair<int, Tprob>>> V(N);
        for (int idx_in = 0; idx_in < N; idx_in++) {
            V[idx_in] = estimator.argmax_multi(idx_in, num_cand);
        }

        vector<tuple<int, int, Tprob>> Vmatching;
        Vmatching.reserve(N * num_cand);
        for (int idx_in = 0; idx_in < N; idx_in++) {
            for (auto [idx_out, prob] : V[idx_in]) {
                Vmatching.emplace_back(idx_in, idx_out, prob);
            }
        }

        // for (int i = 0; i < 100; i++) {
        //     auto [Matching, Prob_matching] = min_cost_bipartiate_matching(Vmatching);
        // }
        auto [Matching, Prob_matching] = min_cost_bipartiate_matching(Vmatching);

#ifdef FLAG_CERR
        cerr << "prob : " << Prob_matching << endl;
#endif
        if (Prob_matching < 0)
            return idx_in;


        vector<int> use_edge(N, -1);
        for (int idx_in = 0; idx_in < N; idx_in++) {
            for (int j = 0; j < num_cand; j++) {
                if (Matching[idx_in] == V[idx_in][j].first) {
                    use_edge[idx_in] = j;
                    break;
                }
            }
        }

        for (auto&& i : use_edge) {
            assert(i >= 0);
        }

        bool flag_break = true;

        Tprob prob_min = 1.;

        Tprob prob_matching_nxt = 0.;
        double max_ratio = 0;
        for (int idx_in1 = 0; idx_in1 < N; idx_in1++) {
            for (int idx_in2 = 0; idx_in2 < idx_in1; idx_in2++) {
                int idx_out1 = Matching[idx_in1];
                int idx_out2 = Matching[idx_in2];
                int idx_out1_new = idx_out2;
                int idx_out2_new = idx_out1;
                Tprob prob = estimator.get_prob(idx_in1, idx_out1) * estimator.get_prob(idx_in2, idx_out2);
                Tprob prob_new = estimator.get_prob(idx_in1, idx_out1_new) * estimator.get_prob(idx_in2, idx_out2_new);
                if (prob_new > 0)
                    chmax(max_ratio, prob_new / prob);
                if (prob_new > prob * prob_ratio_thr) {
                    flag_break = false;
                    if (chmin(prob_min, estimator.get_prob(idx_in1, idx_out1)))
                        ret_in_idx = idx_in1;
                    if (chmin(prob_min, estimator.get_prob(idx_in2, idx_out2)))
                        ret_in_idx = idx_in2;
                }
            }
        }
#ifdef FLAG_CERR
        cerr << "max ratio = " << max_ratio << endl;
#endif


        /* cerr << ret_in_idx << " " << prob_min << endl; */

        if (flag_break)
            ret_in_idx = -1;

    } else if (mode == 3) {
        Tprob prob_thr_min = 0.3;
        Tprob prob_ratio_thr = 0.01;
        auto [idx_in, idx_out, prob] = estimator.argmax();

        if (prob < prob_thr_min)
            return idx_in;

        {
            vector<int> Vcnt(N);
            vector<vector<pair<int, Tprob>>> Vmemo(N);
            for (int i = 0; i < N; i++) {
                auto [idx, p] = estimator.argmax(i);
                Vcnt[idx]++;
                Vmemo[idx].emplace_back(i, p);
            }
            int max_dupl = *max_element(cbegin(Vcnt), cend(Vcnt));
            int cnt_dupl = 0;
            for (auto&& n : Vcnt) {
                if (n >= 2)
                    cnt_dupl += n;
            }
#ifdef FLAG_CERR
            cerr << "max_dupl : " << max_dupl << "  cnt_dupl : " << cnt_dupl << endl;
#endif
            if (max_dupl >= 3) {
                int ret = -1;
                Tprob min_prob = 1.;
                for (int idx = 0; idx < N; idx++) {
                    if (Vcnt[idx] >= 3) {
                        for (auto&& [i, prob] : Vmemo[idx]) {
                            if (chmin(min_prob, prob))
                                ret = i;
                        }
                    }
                }
                // cerr << "ret : " << ret << " " << min_prob << endl;
                return ret;
            }
        }


        int num_cand = N;

        vector<vector<pair<int, Tprob>>> V(N);
        for (int idx_in = 0; idx_in < N; idx_in++) {
            V[idx_in] = estimator.argmax_multi(idx_in, num_cand);
        }

        vector<tuple<int, int, Tprob>> Vmatching;
        Vmatching.reserve(N * num_cand);
        for (int idx_in = 0; idx_in < N; idx_in++) {
            for (auto [idx_out, prob] : V[idx_in]) {
                Vmatching.emplace_back(idx_in, idx_out, prob);
            }
        }

        // for (int i = 0; i < 100; i++) {
        //     auto [Matching, Prob_matching] = min_cost_bipartiate_matching(Vmatching);
        // }
        auto [Matching, Prob_matching] = min_cost_bipartiate_matching(Vmatching);

#ifdef FLAG_CERR
        cerr << "prob : " << Prob_matching << endl;
#endif
        if (Prob_matching < 0)
            return idx_in;


        vector<int> use_edge(N, -1);
        for (int idx_in = 0; idx_in < N; idx_in++) {
            for (int j = 0; j < num_cand; j++) {
                if (Matching[idx_in] == V[idx_in][j].first) {
                    use_edge[idx_in] = j;
                    break;
                }
            }
        }

        for (auto&& i : use_edge) {
            assert(i >= 0);
        }

        bool flag_break = true;

        Tprob prob_min = 1.;

        Tprob prob_matching_nxt = 0.;
        double max_ratio = 0;
        for (int idx_in1 = 0; idx_in1 < N; idx_in1++) {
            for (int idx_in2 = 0; idx_in2 < idx_in1; idx_in2++) {
                int idx_out1 = Matching[idx_in1];
                int idx_out2 = Matching[idx_in2];
                int idx_out1_new = idx_out2;
                int idx_out2_new = idx_out1;
                Tprob prob = estimator.get_prob(idx_in1, idx_out1) * estimator.get_prob(idx_in2, idx_out2);
                Tprob prob_new = estimator.get_prob(idx_in1, idx_out1_new) * estimator.get_prob(idx_in2, idx_out2_new);
                if (prob_new > 0)
                    chmax(max_ratio, prob_new / prob);
                if (prob_new > prob * prob_ratio_thr) {
                    flag_break = false;
                    if (chmin(prob_min, estimator.get_prob(idx_in1, idx_out1)))
                        ret_in_idx = idx_in1;
                    if (chmin(prob_min, estimator.get_prob(idx_in2, idx_out2)))
                        ret_in_idx = idx_in2;
                }
            }
        }
#ifdef FLAG_CERR
        cerr << "max ratio = " << max_ratio << endl;
#endif


        cerr << ret_in_idx << " " << prob_min << endl;

        if (flag_break)
            ret_in_idx = -1;
    } else
        assert(false);
    /* cerr << " get index in : " << ret_in_idx << endl; */
    return ret_in_idx;
}

vector<int> get_ans(Estimator& estimator, int mode = 0) {
    if (not_enough_time)
        mode = 0;
    vector<int> Vans(N);
    if (mode == 0) {
        // 従来
        Tprob prob_sum = 0;
        for (int idx_in = 0; idx_in < N; idx_in++) {
            auto [idx_out_max, prob_max] = estimator.argmax(idx_in);
            if (idx_out_max == -1)
                idx_out_max = 0;
            Vans[idx_in] = idx_out_max;
            prob_sum += prob_max;
#ifdef FLAG_CERR
            vector<Tprob> Vprob = estimator.get_Vprob(idx_in);
            Tprob entropy = calc_entropy(Vprob);
            cerr << idx_in << " " << idx_out_max << " " << prob_max << " " << entropy << endl;
#endif
        }
#ifdef FLAG_CERR
        cerr << "expected errors : " << N - prob_sum << endl;
#endif
    } else if (mode == 1) {
        // use min cost max flow

        int num = N;
        vector<vector<pair<int, Tprob>>> V(N);
        for (int idx_in = 0; idx_in < N; idx_in++) {
            V[idx_in] = estimator.argmax_multi(idx_in, num);
        }

        vector<tuple<int, int, Tprob>> Vmatching;
        Vmatching.reserve(N * num);
        for (int idx_in = 0; idx_in < N; idx_in++) {
            for (auto [idx_out, prob] : V[idx_in]) {
                Vmatching.emplace_back(idx_in, idx_out, prob);
            }
        }

        auto [Matching, Prob_matching] = min_cost_bipartiate_matching(Vmatching);
#ifdef FLAG_CERR
        cerr << "Prob matching " << Prob_matching << endl;
#endif
        Vans = Matching;
    } else
        assert(false);
    return Vans;
}


// ---------------------------------------- functions ----------------------------------------

void init_argc_argv(int& argc, vector<string>& argv) {
    if (!use_embedded)
        return;
    if (argc >= 2)
        return;
    int L_idx = L - 10;
    int S_idx = int(sqrt(S + 1)) - 1;
    cerr << L_idx << " " << S_idx << endl;
    argc = argc_memo[L_idx][S_idx];
    string argv_str = argv_memo[L_idx][S_idx];
    argv = sep_string(argv_str);
    return;
}

void init() {
    start = chrono::system_clock::now();
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    cin >> L >> N >> S;
    cerr << "[DATA] L = " << L << endl;
    cerr << "[DATA] N = " << N << endl;
    cerr << "[DATA] S = " << S << endl;

    /* for (int n = 1; n < log_memo_len; n++) { */
    /*     log_memo[n] = fastlog(1. * n / log_memo_len); */
    /* } */

    /* if (S > 36) { */
    /*     Measure_max = 0; */
    /*     use_KL = false; */
    /* } */
    /* use_KL = false; */
    /* use_KL = 1; */

    Vpos_out.resize(N);
    Vpos_out_x.resize(N);
    Vpos_out_y.resize(N);
    for (int i = 0; i < N; i++) {
        int x, y;
        cin >> y >> x;
        Vpos_out[i] = {x, y};
        Vpos_out_x[i] = x;
        Vpos_out_y[i] = y;
    }

    int S_sqrt = int(sqrt(S + 1));
    height_randomness = V_height_randomness[S_sqrt];
    height_coef = V_height_coef[S_sqrt];
    N_use_KL = min(L * L, V_N_use_KL[S_sqrt]);
    // N_use_KL = L * L;

    // erf
    {
        for (int i = 0; i < erf_memo_len; i++) {
            erf_memo[i] = exp(Tprob(-pow(i, 2) / 2. / S / S)) / (sqrt(2 * M_PI) * S);
        }
        for (int i = erf_memo_len - 2; i >= 0; i--) {
            erf_memo[i] += erf_memo[i + 1];
        }
        /* cerr << erf_memo[0] << endl; */
        /* exit(0); */
    }

    // 情報量回り
    for (int h = 0; h <= Height_max; h++) {
        Ventropy[h] = 0;
        for (int m = 0; m <= Height_max; m++) {
            // m : measure
            // Ventropy[h] += calc_prob_seq(h, m) * calc_entropy(h, m);
            Tprob prob = calc_prob_seq(h, m);
            /* Tprob ent = -prob * log(prob); */
            Tprob ent = -calc_xlogx(prob);
            if (prob > 0 && isfinite(ent) && ent > 0)
                Ventropy[h] += ent;
        }
        // cerr << "[DATA] Ventropy[" << h << "] = " << Ventropy[h] << endl;
    }


    /* if (use_KL) { */
    if (false) {
        // TODO 高速化
        vector<vector<Tprob>> Mlog_prob(Height_max + 1, vector<Tprob>(Height_max + 1, 0));
        for (int h1 = 0; h1 <= Height_max; h1++) {
            for (int h2 = 0; h2 <= Height_max; h2++) {
                Mlog_prob[h1][h2] = log(calc_prob_seq(h1, h2));
            }
        }

        for (int h1 = 0; h1 <= Height_max; h1++) {
            // cerr << "[DATA] h1 = " << h1 << endl;
            for (int h2 = 0; h2 <= Height_max; h2++) {
                Tprob& kl = KL[h1][h2];
                kl = 0;
                if (h1 > Height_max / 2) {
                    kl = KL[Height_max - h1][Height_max - h2];
                    continue;
                }
                for (int h = 0; h <= Height_max; h++) {
                    // Tprob kl_tmp = calc_prob_seq(h1, h) * log(calc_prob_seq(h1, h) / calc_prob_seq(h2, h));
                    Tprob kl_tmp = calc_prob_seq(h1, h) * (Mlog_prob[h1][h] - Mlog_prob[h2][h]);
                    if (isfinite(kl_tmp))
                        kl += kl_tmp;
                }

                // TODO use kl for gaussian pdf
            }
        }
    }

    // initialize board?
    /* if (argc >= 2) { */
    /*     cerr << L * L + 1 << " " << argc << endl; */
    /*     assert(argc == L * L + 1);  // argv includes info about board */
    /*     board_init.resize(L); */
    /*     for (auto&& b : board_init) { */
    /*         b.resize(L); */
    /*     } */

    /*     int cnt = 1; */
    /*     for (int x = 0; x < L; x++) { */
    /*         for (int y = 0; y < L; y++) { */
    /*             int val = stoi(argv[cnt]); */
    /*             board_init[y][x] = val; */
    /*             cnt++; */
    /*         } */
    /*     } */
    /* } */


    return;
}

// ---------------------------------------- solver ----------------------------------------

void solve_0811_1(int argc, vector<string> argv) {
    Board board;
    vector<int> Vheight_out(N);

    if (argc >= 2) {
        string mode(argv[1]);
        if (mode == "board") {
            assert(argc == L * L + 2);  // argv includes info about board
            board_init.resize(L);
            for (auto&& b : board_init) {
                b.resize(L);
            }

            int cnt = 2;
            for (int x = 0; x < L; x++) {
                for (int y = 0; y < L; y++) {
                    int val = stoi(argv[cnt]);
                    board_init[y][x] = val;
                    cnt++;
                }
            }

            board.Mheight = board_init;
        } else if (mode == "1000mountains") {
            assert(argc % 2 == 0);
            int n_mountains = (argc - 2) / 2;
            vector<Pos> Vpos(n_mountains);
            for (int i = 0; i < n_mountains; i++) {
                int x = stoi(argv[i * 2 + 2]);
                int y = stoi(argv[i * 2 + 3]);
                Pos pos{x, y};
                Vpos[i] = pos;
            }
            for (auto&& pos : Vpos) {
                auto [x, y] = pos;
                board.set({x, y}, 1000);
            }
            for (int i = 0; i < L; i++) {
                board.set({i, 0}, 0);
                board.set({0, i}, 0);
            }
            board.setup_board(1000);
        } else if (mode == "mountains_surround0") {
            assert((argc - 2) % 3 == 0);
            int n_mountains = (argc - 2) / 3;
            vector<Pos> Vpos(n_mountains);
            vector<int> Vheight(n_mountains);
            for (int i = 0; i < n_mountains; i++) {
                int x = stoi(argv[i * 3 + 2]);
                int y = stoi(argv[i * 3 + 3]);
                int height = stoi(argv[i * 3 + 4]);
                Pos pos{x, y};
                Vpos[i] = pos;
                Vheight[i] = height;
            }
            for (int i = 0; i < n_mountains; i++) {
                auto [x, y] = Vpos[i];
                int height = Vheight[i];
                board.set({x, y}, height);
            }
            for (int i = 0; i < L; i++) {
                board.set({i, 0}, 0);
                board.set({0, i}, 0);
            }
            board.setup_board(1000);
        } else if (mode == "mountains_noconstraint") {
            assert((argc - 2) % 3 == 0);
            int n_mountains = (argc - 2) / 3;
            vector<Pos> Vpos(n_mountains);
            vector<int> Vheight(n_mountains);
            for (int i = 0; i < n_mountains; i++) {
                int x = stoi(argv[i * 3 + 2]);
                int y = stoi(argv[i * 3 + 3]);
                int height = stoi(argv[i * 3 + 4]);
                Pos pos{x, y};
                Vpos[i] = pos;
                Vheight[i] = height;
            }
            for (int i = 0; i < n_mountains; i++) {
                auto [x, y] = Vpos[i];
                int height = Vheight[i];
                board.set({x, y}, height);
            }
            board.setup_board(1000);
        } else
            assert(false);
        goto end_board_configuration;
    }

    // make vector Vheight_out
    // 0, 2, 4, ..., N-3, N-1, N-2, N-4, ..., 3, 1

    for (int i = 0; i < N; i++) {
        if (i * 2 < N)
            Vheight_out[i] = i * 2;
        else
            Vheight_out[i] = 1 + (N - 1 - i) * 2;
    }

    // int height_coef = 10;
    // int height_coef = 10;
    for (auto&& height : Vheight_out) {
        height *= height_coef;
        height += int((100 - N) * height_coef / 2.);
    }

    // random_shuffle(Vheight_out.begin(), Vheight_out.end());

    for (int i = 0; i < N; i++) {
        Pos pos = Vpos_out[i];
        int height = Vheight_out[i];
        board.set(pos, height);
    }

    // cerr << board << endl;
    // board.setup_board(10000);
    // cerr << board << endl;
    // print_time();

    board.setup_board(1000);
    /* end_board_configuration: */

    /*     { */
    /*         board.initialize(); */

    /*         vector<pair<int, int>> Vdist(N); */
    /*         for (int i = 0; i < N; i++) { */
    /*             Vdist[i] = {dist_int({L / 2, L / 2}, Vpos_out[i]), i}; */
    /*         } */
    /*         sort(begin(Vdist), end(Vdist)); */
    /*         for (int i = 0; i < N; i++) { */
    /*             auto [d, idx] = Vdist[i]; */
    /*             Vheight_out[idx] = i * 3 + 10; */
    /*         } */


    /*         for (int i = 0; i < N; i++) { */
    /*             Pos pos = Vpos_out[i]; */
    /*             int height = Vheight_out[i]; */
    /*             board.set(pos, height); */
    /*         } */

    /*         board.setup_board(1000); */
    /*     } */

    /* for (int x = 0; x < L; x++) { */
    /*     for (int y = 0; y < L; y++) { */
    /*         board.Mheight[x][y] += 100; */
    /*         board.Mheight[x][y] /= 2; */
    /*     } */
    /* } */

    // int S_sqrt = int(sqrt(S + 1));
    // if (S > 100)
    //     board.add_randomness((S_sqrt - 1) / 15);

    board.add_randomness(height_randomness);
    // board.add_randomness();
    // board.flatten_quarter();
    // board.average_random(0.25);

    // board.test_pattern_5();
    // board.average_random(1.0);

    if (S >= 841)
        board.test_pattern_4();
    else if (S >= 361)
        board.test_pattern_2();
    else if (S >= 100)
        board.test_pattern_1();

    // board.test_pattern_6();
    // board.test_pattern_7();


    if (S <= 4) {
    } else if (S <= 25)
        board.test_pattern_8(6, 200);
    else if (S <= 49)
        board.test_pattern_8(6, 250);
    else if (S <= 64)
        board.test_pattern_8(6, 300);
    else if (S <= 144)
        board.test_pattern_8(6, 400);
    else if (S <= 289)
        board.test_pattern_8(6, 500);
    else if (S <= 729)
        board.test_pattern_8(7, 500);

    /* board.test_pattern_9(0, 500, 14, 10); */
    /* board.test_pattern_10(0, 500, L / 2 - 3, 0); */


end_board_configuration:

    if (false) {
        board.initialize();
        // int x = L / 2, y = L / 2;
        int x = L / 3 * 2, y = L / 3 * 2;
        Pos pos{x, y};
        for (int x = 0; x < L; x++) {
            board.set({x, 0}, 0);
            board.set({0, x}, 0);
        }
        board.set(pos, 1000);
        board.set({1, 1}, 1000);
        // for (int i = 0; i < 4; i++) {
        //     board.set(pos + i, 500);
        // }
        board.setup_board(1000);

        for (int x = 0; x < L; x++) {
            for (int y = 0; y < L; y++) {
                board.Mheight[x][y] = 0;
            }
        }

        for (int x = 0; x < L / 2; x++) {
            for (int y = 0; y < L / 2; y++) {
                board.Mheight[x][y] = 1000;
            }
        }

        board.initialize();
        for (int x = 0; x < L; x++) {
            for (int y = 0; y < L; y++) {
                board.Mheight[x][y] = 0;
            }
        }

        // int d = L / 3 * 2;
        // for (int x = 0; x < L; x++) {
        //     board.set({x, 0}, 0);
        //     board.set({0, x}, 0);
        //     board.set({x, d}, 0);
        //     board.set({d, x}, 0);
        // }
        // int n = d / 2;
        // int dd = 8;
        // for (int i = 0; i < n; i++) {
        //     board.set({randrange(1 + dd, d - dd), randrange(1 + dd, d - dd)}, 1000);
        // }

        for (int x = 0; x < L; x++) {
            board.set({x, x}, 0);
            board.set({x, L - 1 - x}, 0);
        }
        int n = 7;
        auto check_in = [&](int x, int y) {
            return ((x < y && x + y < L) || (x > y && x + y > L));
        };
        for (int i = 0; i < n; i++) {
            // board.set({randrange(1 + dd, d - dd), randrange(1 + dd, d - dd)}, 1000);
            int x = randrange(L);
            int y = randrange(L);
            while (!check_in(x, y)) {
                x = randrange(L);
                y = randrange(L);
            }
            board.set({x, y}, 1000);
        }


        // int d = L / 20;
        // int x_min = L / 2 - d, x_max = L / 2 + d;
        // board.set({x_min, x_min}, 1000);
        // board.set({x_min, x_max - 1}, 1000);
        // board.set({x_max - 1, x_min}, 1000);
        // board.set({x_max - 1, x_max - 1}, 1000);
        // for (int x = x_min; x < x_max; x++) {
        //     for (int y = x_min; y < x_max; y++) {
        //         board.set({x, y}, 1000);
        //     }
        // }
        // int dd = 4;
        // // surround by 0
        // for (int x = x_min - dd; x < x_max + dd; x++) {
        //     board.set({x, x_min - dd}, 0);
        //     board.set({x, x_max + dd - 1}, 0);
        //     board.set({x_min - dd, x}, 0);
        //     board.set({x_max + dd - 1, x}, 0);
        // }


        board.setup_board(1000);


        // board.setup_board(1000);
    }


    /* if (true) { */
    /*     board.initialize(); */
    /* } */


    /* if (true) { */
    /*     int MIN = 1000, MAX = 0; */
    /*     for (int x = 0; x < L; x++) { */
    /*         for (int y = 0; y < L; y++) { */
    /*             int height = board.Mheight[x][y]; */
    /*             chmin(MIN, height); */
    /*             chmax(MAX, height); */
    /*         } */
    /*     } */
    /*     int mid = (MAX - MIN) / 2; */
    /*     for (int x = 0; x < L; x++) { */
    /*         for (int y = 0; y < L; y++) { */
    /*             board.Mheight[x][y] += (500 - mid); */
    /*         } */
    /*     } */
    /* } */


    for (int i = 0; i < N; i++) {
        Vheight_out[i] = board.get(Vpos_out[i]);
    }


    cout << board;
    board.expand_Mheight();


    // dpos 離れた位置のうち、heightが最も異なる2点を計算
    vector<vector<Pos>> Mpos_max_height_difference(L, vector<Pos>(L, Pos(-1, -1)));
    for (int dx = 0; dx < L; dx++) {
        for (int dy = 0; dy < L; dy++) {
            Pos dpos = Pos(dx, dy);
            int max_height_difference = -1;
            Pos& pos_max_height_difference = Mpos_max_height_difference[dx][dy];
            for (int x = 0; x < L; x++) {
                for (int y = 0; y < L; y++) {
                    Pos pos = Pos(x, y);
                    int dheight = abs(board.get(pos + dpos) - board.get(pos));  // TODO optimize
                    if (chmax(max_height_difference, dheight)) {
                        pos_max_height_difference = pos;
                    }
                }
            }
        }
    }


    print_time();

    // measure
    // const Tprob prob_thr = 1. - (1. / N / 10);
    /* Tprob prob_thr = 0.99999999; */
    /* Tprob prob_thr_weak = 0.8;  // 重複していなければ ok とする TODO 順列の発生確率についてちゃんと考える */
    Estimator estimator;
    for (int iter = 0; iter < Measure_max; iter++) {
        if (get_time_remaining() < TIME_USE_KL)
            use_KL = false;
#ifdef FLAG_CERR
        cerr << "iter : " << iter << endl;
#endif
        /* Tprob min_max_prob = 1; */
        /* int idx_in_min_max_prob = -1; */
        /* vector<int> V_cnt(N, 0); */
        /* vector<vector<pair<Tprob, int>>> V_duplicate(N, vector<pair<Tprob, int>>(0)); */
        /* for (int idx_in = 0; idx_in < N; idx_in++) { */
        /*     auto [idx_out_max, prob_max] = estimator.argmax(idx_in); */
        /*     V_duplicate[idx_out_max].emplace_back(prob_max, idx_in); */
        /*     V_cnt[idx_out_max]++; */
        /*     if (chmin(min_max_prob, prob_max)) { */
        /*         idx_in_min_max_prob = idx_in; */
        /*     } */
        /* } */
        /* for (int idx_out = 0; idx_out < N; idx_out++) { */
        /*     sort(begin(V_duplicate[idx_out]), end(V_duplicate[idx_out])); */
        /* } */
        /* int max_duplicate_cnt = *max_element(cbegin(V_cnt), cend(V_cnt)); */
        /* if (min_max_prob > prob_thr_weak && max_duplicate_cnt == 1) { */
        /*     cerr << "weak break" << endl; */
        /*     break; */
        /* } */
        /* if (min_max_prob > prob_thr) { */
        /*     cerr << "hard break" << endl; */
        /*     break; */
        /* } */

        /* // 重複している要素のうち，確率が最も低いものを選ぶ */
        /* if (max_duplicate_cnt >= 2) { */
        /*     Tprob min_prob = 1.; */
        /*     for (auto&& V : V_duplicate) { */
        /*         if ((int)V.size() == 1) */
        /*             continue; */
        /*         for (auto&& [prob, idx_in] : V) { */
        /*             if (chmin(min_prob, prob)) */
        /*                 idx_in_min_max_prob = idx_in; */
        /*         } */
        /*     } */
        /* } */
        int idx_in_min_max_prob;
        if (S <= 4)
            idx_in_min_max_prob = get_next_in_idx(estimator, 3);
        else if (S <= 100)
            idx_in_min_max_prob = get_next_in_idx(estimator, 2);
        else
            idx_in_min_max_prob = get_next_in_idx(estimator, 1);
        if (idx_in_min_max_prob == -1)
            break;
        Tprob min_max_prob = estimator.argmax(idx_in_min_max_prob).second;

        /* cerr << idx_in_min_max_prob << " " << min_max_prob << endl; */
        Pos pos;
        vector<int> Vheight_out_now = Vheight_out;

        // vector<vector<Tprob>> M_KL(L, vector<Tprob>(L, 0));  // Entropy
        static vector<Tprob> V_KL(N_use_KL);  // Entropy

#ifdef FLAG_CERR
        if (idx_in_min_max_prob == 0)
            for (auto&& [idx_out, prob] : estimator.argmax_multi(idx_in_min_max_prob, 5)) {
                cerr << idx_out << " " << prob << endl;
            }
#endif


        // if (false) {
        // if (min_max_prob > 1. / N && randrange(2) == 0) {
        if (min_max_prob > 1. / N) {
            // if (true) {
            Pos dpos;

            // random move
            // dpos.first = randrange(-2, 2);
            // dpos.second = randrange(-2, 2);
            // dpos.first = randrange(-L / 2, L / 2);
            // dpos.second = randrange(-L / 2, L / 2);

            // find optimal dpos using 1 points
            if (!use_KL) {
                /* if (true) { */
                auto V_argmax_multi = estimator.argmax_multi(idx_in_min_max_prob, 2);
                int idx0 = V_argmax_multi[0].first;
                int idx1 = V_argmax_multi[1].first;
                Pos pos0 = Vpos_out[idx0];
                Pos pos1 = Vpos_out[idx1];
                auto [x_tmp, y_tmp] = pos1 - pos0;
                dpos = Mpos_max_height_difference[x_tmp][y_tmp] - pos0;
            }


            // find optimal dpos using KL divergence
            if (use_KL) {
                /* if (false) { */

                vector<Tprob> P_i(N);  // 事前確率 P(i)
                // vector<vector<Tprob>> P_mi(Height_max + 1, vector<Tprob>(N));  // P^v(m|i) dpos依存
                // vector<pair<Tprob, int>> P_i_idx(N);
                for (int i = 0; i < N; i++) {
                    P_i[i] = estimator.Mprob[idx_in_min_max_prob][i];
                    // P_i_idx[i] = {P_i[i], i};
                }
                // sort(P_i_idx.begin(), P_i_idx.end(), greater<pair<Tprob, int>>());

#ifdef FLAG_CERR
                Tprob entropy = calc_entropy(P_i);
                cerr << "entropy : " << entropy << endl;
#endif

                auto V_argmax_multi = estimator.argmax_multi(idx_in_min_max_prob, 2);
                // int idx_out1 = V_argmax_multi[0].first;
                // int idx_out2 = V_argmax_multi[1].first;
                // vector<pair<double, Pos>> V_score_dpos;

                vector<pair<double, Pos>> V_priority_dpos(0);
                V_priority_dpos.reserve(L * L);
                for (int x_dpos = 0; x_dpos < L; x_dpos++) {
                    for (int y_dpos = 0; y_dpos < L; y_dpos++) {
                        Pos dpos_tmp = Pos(x_dpos, y_dpos);
                        double ave = 0, ave2 = 0;
                        for (int n = 0; n < N; n++) {
                            int x = Vpos_out_x[n] + x_dpos;
                            int y = Vpos_out_y[n] + y_dpos;
                            // int height = board.Mheight[x][y];
                            double height = board.get_fast_double(x, y);
                            double height_prob = height * P_i[n];
                            ave += height_prob;
                            ave2 += height * height_prob;
                        }
                        double var = ave2 - pow(ave, 2);
                        V_priority_dpos.emplace_back(var, dpos_tmp);

                        // M_KL[x_dpos][y_dpos] = 0;
                    }
                }
                // sort(V_priority_dpos.begin(), V_priority_dpos.end(), greater<pair<double, Pos>>());
                partial_sort(V_priority_dpos.begin(), V_priority_dpos.begin() + N_use_KL, V_priority_dpos.end(), greater<pair<double, Pos>>());


                vector<Tprob> Vprob(Height_max + 1, 0.);
                // for (int x_dpos = 0; x_dpos < L; x_dpos++) {
                //     for (int y_dpos = 0; y_dpos < L; y_dpos++) {

                {
                    // for (auto&& [priority, dpos_tmp] : V_priority_dpos) {
                    for (int n_tmp = 0; n_tmp < N_use_KL; n_tmp++) {
                        auto [priority, dpos_tmp] = V_priority_dpos[n_tmp];
                        auto [x_dpos, y_dpos] = dpos_tmp;

                        // Pos dpos_tmp = Pos(x_dpos, y_dpos);
                        // Tprob& kl = M_KL[x_dpos][y_dpos];
                        Tprob& kl = V_KL[n_tmp];

                        kl = 0;

                        /* if (x_dpos == 0 && y_dpos == 0) */
                        /*     cerr << kl << endl; */
                        // 第2項目
                        int n_sigma = 3;
                        // double prob_thr = -1;
                        int dm = min(10, S / 4);
                        chmax(dm, 1);
                        // dm = 1;


                        /* for (int h = 0; h <= Height_max; h++) { */
                        /*     Vprob[h] = 0; */
                        /* } */
                        Tprob prob_sum_mid = 0;
                        int m_min_all = Height_max;
                        int m_max_all = 0;
                        for (int idx = 0; idx < N; idx++) {
                            // if (P_i[idx] < prob_thr)
                            //     continue;

                            // Pos pos = Vpos_out[idx] + dpos_tmp;
                            // int height = board.get(pos);
                            int x = Vpos_out_x[idx] + x_dpos;
                            int y = Vpos_out_y[idx] + y_dpos;
                            int height = board.get_fast(x, y);


                            kl -= Ventropy[height] * P_i[idx];

                            int m_min = max(0, height - n_sigma * S);
                            int m_max = min(Height_max, height + n_sigma * S);
                            m_min = m_min / dm * dm;
                            chmin(m_min_all, m_min);
                            chmax(m_max_all, m_max);
                            m_max = ((m_max - 1) / dm + 1) * dm;
                            for (int m = m_min; m <= m_max; m += dm) {
                                chmin(m, Height_max);
                                Tprob dprob = calc_prob_seq(height, m) * P_i[idx];
                                Vprob[m] += dprob;
                            }
                        }
                        // NEW
                        /* Tprob prob_init = Vprob[dm]; */
                        /* for (int m = 1; m < Height_max; m++) { */
                        /*     if (m % dm == 0) */
                        /*         prob_init = Vprob[m]; */
                        /*     else */
                        /*         Vprob[m] = prob_init; */
                        /* } */
                        /* Tprob prob_sum_mid2 = accumulate(begin(Vprob) + 1, end(Vprob) - 1, decltype(Vprob)::value_type(), [](const auto& acc, const auto& e) { return acc + e; }); */
                        /* cerr << prob_sum_mid << " " << prob_sum_mid2 << endl; */
                        /* assert(abs(prob_sum_mid - prob_sum_mid2) < 0.01); */

                        /* Tprob prob_init = Vprob[dm]; */
                        /* for (int m = 1; m < Height_max; m++) { */
                        /*     if (m % dm == 0) */
                        /*         prob_init = Vprob[m]; */
                        /*     else */
                        /*         Vprob[m] = prob_init; */
                        /* } */

                        Tprob entropy2;
                        if (false) {
                            Tprob prob_init = Vprob[(Height_max - 1) / dm * dm];
                            for (int m = Height_max - 1; m >= 1; m--) {
                                if (m % dm == 0)
                                    prob_init = Vprob[m];
                                else
                                    Vprob[m] = prob_init;
                            }
                            Tprob prob_sum_mid = accumulate(begin(Vprob) + 1, end(Vprob) - 1, decltype(Vprob)::value_type(), [](const auto& acc, const auto& e) { return acc + e; });
                            Tprob prob_sum_end = Vprob[0] + Vprob[Height_max];
                            Tprob prob_coef = (1 - prob_sum_end) / prob_sum_mid;
                            for (int m = 1; m < Height_max; m++) {
                                Vprob[m] *= prob_coef;
                            }
                            entropy2 = calc_entropy(Vprob);
                            kl += entropy2;
                        }


                        // BEGIN
                        for (int m = dm; m < Height_max; m += dm) {
                            Tprob& prob = Vprob[m];
                            if (m == (Height_max - 1) / dm * dm)
                                prob_sum_mid += prob * (((Height_max - 1) % dm) + dm);
                            else
                                prob_sum_mid += prob * dm;
                        }

                        Tprob entropy1 = 0;
                        Tprob prob_sum_end = Vprob[0] + Vprob[Height_max];
                        Tprob prob_coef = (1 - prob_sum_end) / prob_sum_mid;
                        /* for (int m = 0; m < Height_max + dm; m += dm) { */
                        for (int m = m_min_all; m <= m_max_all; m += dm) {
                            chmin(m, Height_max);
                            Tprob& prob = Vprob[m];
                            if (m != 0 && m != Height_max)
                                prob *= prob_coef;
                            Tprob dentropy = -calc_xlogx(prob);
                            if (m == 0 || m == Height_max)
                                entropy1 += dentropy;
                            else if (m == (Height_max - 1) / dm * dm)
                                entropy1 += dentropy * (((Height_max - 1) % dm) + dm);
                            else
                                entropy1 += dentropy * dm;
                            prob = 0;
                        }
                        kl += entropy1;
                        // END

                        if (false) {
                            // Pos pos1 = Vpos_out[idx_out1] + dpos_tmp;
                            // Pos pos2 = Vpos_out[idx_out2] + dpos_tmp;
                            // int height1 = board.get(pos1);
                            // int height2 = board.get(pos2);
                            // int dheight = abs(height1 - height2);
                            // V_score_dpos.emplace_back(dheight, dpos_tmp);

                            // double ave = 0;
                            // double ave2 = 0;
                            // for (int h = 0; h <= Height_max; h++) {
                            //     ave += h * Vprob[h];
                            //     ave2 += h * h * Vprob[h];
                            // }
                            // double var = ave2 - ave * ave;
                            // double sigma = sqrt(var);
                            // // V_score_dpos.emplace_back(var / calc_measure_cost(dpos_tmp), dpos_tmp);
                            // V_score_dpos.emplace_back(var, dpos_tmp);


                            for (int h = 0; h <= Height_max; h++) {
                                Vprob[h] = 0;
                            }
                        }


                        // cerr << entropy1 << " " << kl << " " << kl - entropy1 << endl;
                        /* cerr << entropy1 << " " << entropy2 << " " << abs(entropy1 - entropy2) << endl; */
                        /* assert(abs(entropy1 - entropy2) / entropy2 < 1e-4); */


                        /* if (x_dpos == 0 && y_dpos == 0) */
                        /*     cerr << kl << endl */
                        /*          << endl; */


                        // Goldberg's approximate formula
                        // https://ieeexplore.ieee.org/document/4218101
                        // Tprob P_i_sum = 0;
                        // Tprob P_i_thr = 1. / N;
                        // for (int i = 0; i < N; i++) {
                        //     Tprob kl_tmp = INF;
                        //     for (int j = 0; j < N; j++) {
                        //         // if (P_i[j] < P_i_thr)
                        //         //     continue;
                        //         Pos pos_i = Vpos_out[i] + dpos_tmp;
                        //         Pos pos_j = Vpos_out[j] + dpos_tmp;
                        //         int height_i = board.get(pos_i);
                        //         int height_j = board.get(pos_j);

                        //         Tprob tmp = KL[height_i][height_j] - log(P_i[j]);
                        //         if (isfinite(tmp))
                        //             chmin(kl_tmp, tmp);
                        //     }
                        //     if (kl_tmp != Tprob(INF)) {
                        //         kl += P_i[i] * kl_tmp;
                        //         P_i_sum = P_i[i];
                        //     }
                        // }

                        // variational upper bound
                        // vector<Tprob> V;

                        // for (int i = 0; i < N; i++) {
                        //     for (int j = 0; j < N; j++) {
                        /* int N_use = N; */
                        /* for (int ii = 0; ii < N_use; ii++) { */
                        /*     for (int jj = 0; jj < N_use; jj++) { */
                        /*         int i = P_i_idx[ii].second; */
                        /*         int j = P_i_idx[jj].second; */
                        /*         Pos pos_i = Vpos_out[i] + dpos_tmp; */
                        /*         Pos pos_j = Vpos_out[j] + dpos_tmp; */
                        /*         int height_i = board.get(pos_i); */
                        /*         int height_j = board.get(pos_j); */
                        /*         Tprob dkl = P_i[i] * P_i[j] * KL[height_i][height_j]; */
                        /*         kl += dkl; */
                        /*         // break; */
                        /*         // V.emplace_back(dkl); */
                        /*     } */
                        /* } */
                        // sort(V.begin(), V.end());
                        // for (int i = 0; i < 10; i++) {
                        //     cerr << V[i] << " ";
                        // }
                        // for (int i = 0; i < 10; i++) {
                        //     cerr << V[V.size() - 1 - i] << " ";
                        // }
                        // cerr << endl;
                        // cerr << endl;


                        // variational lower bound
                        // for (int i = 0; i < N; i++) {
                        // for (int ii = 0; ii < N_use; ii++) {
                        //     int i = P_i_idx[ii].second;
                        //     Pos pos_i = Vpos_out[i] + dpos_tmp;
                        //     int height_i = board.get(pos_i);
                        //     Tprob denom = 0;
                        //     // for (int j = 0; j < N; j++) {
                        //     for (int jj = 0; jj < N; jj++) {
                        //         int j = P_i_idx[jj].second;
                        //         Pos pos_j = Vpos_out[j] + dpos_tmp;
                        //         int height_j = board.get(pos_j);
                        //         denom += P_i[j] * exp(-KL[height_i][height_j]);
                        //     }
                        //     kl += -P_i[i] * log(denom);
                        // }


                        // initialize P_mi
                        // for (int m = 0; m <= Height_max; m++) {
                        //     for (int i = 0; i < N; i++) {
                        //         Pos pos_i = Vpos_out[i] + dpos_tmp;
                        //         int height_i = board.get(pos_i);
                        //         P_mi[m][i] = calc_prob_seq(height_i, m);
                        //     }
                        // }


                        // for (int m = 0; m <= Height_max; m++) {
                        //     vector<Tprob> PP_i(N);  // P(i)*P^v(i|m)
                        //     for (int i = 0; i < N; i++) {
                        //         PP_i[i] = P_i[i] * P_mi[m][i];
                        //     }
                        //     Tprob sum_PP_i = accumulate(PP_i.begin(), PP_i.end(), 0.0);

                        //     for (int i = 0; i < N; i++) {
                        //         entropy += PP_i[i] * log(PP_i[i]);
                        //         entropy -= PP_i[i] * log(sum_PP_i);
                        //     }
                        // }

                        // Mentropy[x_dpos][y_dpos] = entropy;
                    }
                }

                // find argmax dpos
                // Tprob KL_max = -INF;
                Tprob KL_cost_max = -INF;
                // for (int x_dpos = 0; x_dpos < L; x_dpos++) {
                //     for (int y_dpos = 0; y_dpos < L; y_dpos++) {
                {
                    for (int n_tmp = 0; n_tmp < N_use_KL; n_tmp++) {
                        auto [priority, dpos_tmp] = V_priority_dpos[n_tmp];
                        auto [x_dpos, y_dpos] = dpos_tmp;
                        // cerr << MKL[x_dpos][y_dpos] << endl;
                        if (chmax(KL_cost_max, V_KL[n_tmp] / calc_measure_cost(x_dpos, y_dpos))) {
                            // if (chmax(KL_cost_max, M_KL[x_dpos][y_dpos])) {
                            dpos = Pos(x_dpos, y_dpos);
                        }
                    }
                }


                // sort(begin(V_score_dpos), end(V_score_dpos), greater{});
                // for (int i = 0; i < (int)V_score_dpos.size(); i++) {
                //     auto [score, dpos_tmp] = V_score_dpos[i];
                //     if (dpos_tmp == dpos) {
                //         double per = 1. * i / (int)V_score_dpos.size();
                //         cerr << "percentage : " << per << endl;
                //         cerr << "number : " << i << endl;
                //     }
                // }

                // cerr << "KL max : " << KL_cost_max << endl;
            }


            dpos = postprocess_dpos(dpos);
            for (int idx_out = 0; idx_out < N; idx_out++) {
                Vheight_out_now[idx_out] = board.get(Vpos_out[idx_out] + dpos);
            }
            pos = dpos;
        }
        int m = measure(idx_in_min_max_prob, pos);
#ifdef FLAG_CERR
        cerr << "            output : " << idx_in_min_max_prob << " " << pos.first << " " << pos.second << endl;
        cerr << pos.first << " " << pos.second << " " << idx_in_min_max_prob << " " << m << " " << min_max_prob << " " << endl;
#endif
        estimator.add_measurement(idx_in_min_max_prob, m, Vheight_out_now);

        if (get_time_remaining() < 100)
            not_enough_time = true;

        if (iter % 10 == 0 && get_time_remaining() < 0)
            break;

        if (iter == Measure_max - 1)
            not_enough_time = true;
    }

    // cerr << "print ans" << endl;

    {

        Tprob prob_sum = 0;
        vector<int> Vans(N);
        for (int idx_in = 0; idx_in < N; idx_in++) {
            auto [idx_out_max, prob_max] = estimator.argmax(idx_in);
            if (idx_out_max == -1)
                idx_out_max = 0;
            Vans[idx_in] = idx_out_max;
            prob_sum += prob_max;
            vector<Tprob> Vprob = estimator.get_Vprob(idx_in);
            Tprob entropy = calc_entropy(Vprob);
            cerr << idx_in << " " << idx_out_max << " " << prob_max << " " << entropy << endl;
        }
    }
    /* cerr << "expected errors : " << N - prob_sum << endl; */


    vector<int> Vans = get_ans(estimator, 1);

    print_ans(Vans);
}

// ---------------------------------------- main ----------------------------------------

int main(int argc, char* argv_char[]) {
    init();

    vector<string> argv = convertToVector(argc, argv_char);
    init_argc_argv(argc, argv);

    solve_0811_1(argc, argv);
    print_time();
}

// ---------------------------------------- メモ ----------------------------------------

/*
やること
- threhsold の上げ下げ


細かいこと
- S に応じて height の差分を定める（現在10）
- S, L, N に応じた randomness
- threhold 最適化
- 終了条件を詰める
    - より探索するべきか，停止するべきかをスコアの期待値を比較することで決定したい
    - 各inについて，確率が高いoutを二つ選び，一番確率が高い順列の確率を求める
- KL divergence を計算する x, y を選別


思考
- KL情報量(諸説)と移動距離のトレードオフ？
- あれ，エネルギー最小にする盤面の解析解求まるじゃん，天才か？←嘘
    - E-L eq. をこねこねすれば微分方程式くらいは出てくるかも



盤面作成，入り口の指定（＝停止条件），移動方法の指定

盤面作成
- S によって場合分け
- いずれ L も考慮したい
- optuna を使って S, L ごとに最適化？(最終手段)
- 山1個が強すぎる
- 何故？
- S == 900 時の盤面作成が適当すぎる (test_pattern_4) のでなんとかする

入り口の指定
- 重複がないときは，min(確信度) > prob_thr_weak なら OK (大体0.8)
    - 最適化したい
    - 誤り数の推定をもっと頑張ればできそう
    - 順列に対してこれをするの難しい
    - Q : 順列が与えられた時に，誤り数の期待値を求める
- それ以外でも，min(確信度) > prob_thr 以上なら OK (大体0.999)
- 後者要らなくないか？？？？？
    - 調べたら後者は機能してなかった...

移動方法の指定
- 1st と 2nd の高低差が大きくなるような移動
- KL divergence gain / cost が最大となる手を選ぶようにすると若干スコアがあがる
    - 山1つの盤面では利得が少ない
    - 多分有効手が1つしかない？
        - 山1つだしそれはそうか
        - 山を増やすと状況が変わる？


Exact Equation

Original Prob. Dist.
     out   : P(i)
     meas. : P(m) := ∑_i{P(m|i)P(i)}
Prob. Dist. after measuring m
     Q^m(i) := P(i|m)*P(i)/P(m)
             = P(i|m)*P(i)/∑_i{P(m|i)P(i)}

Original Entropy
    H(P(i)) := - ∑_i{P(i)log(P(i))}
New Entropy after measuring m
    H(Q^m(i)) := - ∑_i{Q(i)log(Q(i))}
Expected value of New Entropy
    E_m[H(Q^m(i))] := - E_m[∑_i{Q(i)*log(Q(i))}]
                     = - ∑_m{P(m)∑_i{Q(i)*log(Q(i))}}
                     = - ∑_i,m{P(m|i)*P(i)*log(P(m|i)*P(i)/P(m))}
                     = - ∑_i{P(i)*log(P(i))} - ∑_i,m{P(m|i)*P(i)*log(P(m|i)/P(m))} // second term is expected KL divergence
                     = - ∑_i{P(i)*log(P(i))} - ∑_i,m{P(m|i)*P(i)*log(P(m|i))} + ∑_m{P(m)*log(P(m))} //あれ？こんな簡単な形に？ほんまか？←ほんまでした　俺が天才
Expected Entropy Reduction by Measurement (= Expected KL divergence E_i[D(P(m|i)||P(m))])
    dH := H(P(i)) - E_m[H(Q^m(i))]
        = ∑_i,m{P(m|i)*P(i)*log(P(m|i))} - ∑_m{P(m)*log(P(m))}
        = -E_i[H(P(m|i))] + H(P(m))

Goal : Select out index to maximize dH



argv modes
- board
- 1000mountains

*/


/*
メモ更新 8/17
- N_use の自動減衰
    - 現在は N_use を決め打っているが、その値を減衰
- dm と N_use のトレードオフの調整
    - 現在は雑に dm を決めているが、ここも本来ならちゃんと決めるべき
    - dm を動的に決めてもいいかも
- 高速化
    - N_use を大きく、dm を小さくするのが重要だが、そのためには高速化が必要
    - int -> double のキャストを減らす
    - N_use 個選ぶところで前の結果をそのままそっくり使い回せば N_use を大きくできそう
    - 全体を書き直す必要があるかも
- N_use 個の候補列挙の最適化
    - e.g. 多様性を持たせるため k 種の指標で列挙し、それぞれの上位 N_use/k 個を選ぶ
- 初期配置を optuna で再最適化
    - はい
- 終了条件の最適化
    - 全探索 or optuna
    - prob_thr_min の値が意外と本質
- spaghetti code をなんとかする
    - なんとかする（なんとかする？）
*/

/*
メモ更新 8/18
なんもわからん なんで47G出てるんだ？
*/
