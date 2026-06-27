//
// Created by shibh308 on 11/12/22.
//
// #define AVX512
#ifdef AVX512
#pragma GCC target("avx512f")
#else
#pragma GCC target("avx2")
#endif
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")


#include "bits/stdc++.h"
#include <immintrin.h>

using namespace std;

#ifdef AVX512
constexpr int SKIP = 64;
#else
constexpr int SKIP = 32;
#endif

using i64 = long long;

constexpr int num_queries = 100;

template <typename T>
bool chmin(T& x, T y){
    if(x > y){
        x = y;
        return true;
    }
    return false;
}

template <typename T>
bool chmax(T& x, T y){
    if(x < y){
        x = y;
        return true;
    }
    return false;
}

vector<vector<int>> gsizes =
        {{1,2,3,3,4,4,4,4,4,4,5,5,5,6,6,6,6,7,7,7,8,8,8,9,10,10,11,11,11,14,16,16,16,19,19,21,26,29,31,33,33},
         {1,2,3,3,4,4,4,4,4,4,5,5,5,6,6,6,6,7,7,7,8,8,8,9,10,10,11,11,11,14,16,16,16,19,19,21,26,29,31,33,33},
         {1,2,3,3,4,4,4,4,4,4,5,5,5,6,6,6,6,7,7,7,8,8,8,9,10,10,11,11,11,14,16,16,16,19,19,21,26,29,31,33,33},
         {1,2,3,3,4,4,4,4,4,4,5,5,5,6,6,6,6,7,7,7,8,8,8,9,10,10,11,11,11,14,16,16,16,19,19,21,26,29,31,33,33},
         {1,3,3,3,3,4,4,4,5,5,5,6,6,6,6,6,7,7,7,8,8,8,8,10,10,10,12,13,13,15,16,16,18,20,21,25,26,30,32,33,33},
         {1,3,3,3,3,4,4,4,5,5,5,6,6,6,6,6,7,7,7,8,8,8,8,10,10,10,12,13,13,15,16,16,18,20,21,25,26,30,32,33,33},
         {1,3,3,3,3,4,4,4,5,5,5,6,6,6,6,6,7,7,7,8,8,8,8,10,10,10,12,13,13,15,16,16,18,20,21,25,26,30,32,33,33},
         {1,3,3,3,3,4,4,4,5,5,5,6,6,6,6,6,7,7,7,8,8,8,8,10,10,10,12,13,13,15,16,16,18,20,21,25,26,30,32,33,33},
         {1,3,3,3,3,4,4,4,5,5,5,6,6,6,6,6,7,7,7,8,8,8,8,10,10,10,12,13,13,15,16,16,18,20,21,25,26,30,32,33,33},
         {1,3,3,4,4,5,5,5,5,6,6,6,6,7,7,7,7,7,9,9,9,9,10,10,10,11,12,13,14,15,17,19,19,19,23,24,26,30,32,33,33},
         {1,3,3,4,4,5,5,5,5,6,6,6,6,7,7,7,7,7,9,9,9,9,10,10,10,11,12,13,14,15,17,19,19,19,23,24,26,30,32,33,33},
         {1,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,6,7,7,8,8,8,9,9,11,11,12,13,14,15,15,16,20,21,23,25,25,25,25},
         {1,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,6,7,7,8,8,8,9,9,11,11,12,13,14,15,15,16,20,21,23,25,25,25,25},
         {1,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,6,7,7,8,8,8,9,9,11,11,12,13,14,15,15,16,20,21,23,25,25,25,25},
         {1,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,6,7,7,8,8,8,9,9,11,11,12,13,14,15,15,16,20,21,23,25,25,25,25},
         {1,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,6,7,7,8,8,8,9,9,11,11,12,13,14,15,15,16,20,21,23,25,25,25,25},
         {1,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,6,7,7,8,8,8,9,9,11,11,12,13,14,15,15,16,20,21,23,25,25,25,25},
         {1,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,6,7,7,8,8,8,9,9,11,11,12,13,14,15,15,16,20,21,23,25,25,25,25},
         {1,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,6,7,7,8,8,8,9,9,11,11,12,13,14,15,15,16,20,21,23,25,25,25,25},
         {1,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,6,6,6,7,8,8,9,9,10,10,11,11,12,14,15,16,18,20,23,23,25,25,25,25},
         {1,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,6,6,6,7,8,8,9,9,10,10,11,11,12,14,15,16,18,20,23,23,25,25,25,25},
         {1,3,3,3,3,4,4,4,4,4,5,5,5,6,6,6,6,6,7,7,8,8,9,9,9,10,10,12,12,13,15,15,17,19,20,23,25,25,25,25,25},
         {1,3,3,3,3,4,4,4,4,4,5,5,5,6,6,6,6,6,7,7,8,8,9,9,9,10,10,12,12,13,15,15,17,19,20,23,25,25,25,25,25},
         {1,3,3,3,3,4,4,4,4,4,5,5,5,6,6,6,6,6,7,7,8,8,9,9,9,10,10,12,12,13,15,15,17,19,20,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,6,6,7,8,8,8,8,9,9,10,10,12,12,13,15,16,18,18,20,21,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,6,6,7,8,8,8,8,9,9,10,10,12,12,13,15,16,18,18,20,21,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,6,6,7,8,8,8,8,9,9,10,10,12,12,13,15,16,18,18,20,21,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,6,6,7,8,8,8,8,9,9,10,10,12,12,13,15,16,18,18,20,21,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,6,6,7,8,8,8,8,9,9,10,10,12,12,13,15,16,18,18,20,21,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,6,6,7,8,8,8,8,8,10,11,11,12,12,12,14,16,18,20,20,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,6,6,7,8,8,8,8,8,10,11,11,12,12,12,14,16,18,20,20,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,6,6,7,8,8,8,8,8,10,11,11,12,12,12,14,16,18,20,20,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,6,6,7,8,8,8,8,8,10,11,11,12,12,12,14,16,18,20,20,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,6,6,7,8,8,8,8,8,10,11,11,12,12,12,14,16,18,20,20,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,7,7,8,8,8,9,10,10,10,10,12,13,14,14,15,18,18,21,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,7,7,8,8,8,9,10,10,10,10,12,13,14,14,15,18,18,21,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,7,7,8,8,8,9,10,10,10,10,12,13,14,14,15,18,18,21,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,7,7,8,8,8,9,10,10,10,10,12,13,14,14,15,18,18,21,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,7,7,8,8,8,9,10,10,10,10,12,13,14,14,15,18,18,21,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,5,5,5,5,6,6,6,7,7,7,7,7,8,8,8,11,11,11,12,12,12,13,14,16,19,21,22,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,5,5,5,5,6,6,6,7,7,7,7,7,8,8,8,11,11,11,12,12,12,13,14,16,19,21,22,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,5,5,5,5,6,6,6,7,7,7,7,7,8,8,8,11,11,11,12,12,12,13,14,16,19,21,22,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,5,5,5,5,6,6,6,7,7,7,7,7,8,8,8,11,11,11,12,12,12,13,14,16,19,21,22,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,4,5,5,5,5,6,6,6,7,7,7,7,7,8,8,8,11,11,11,12,12,12,13,14,16,19,21,22,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,5,5,5,5,5,5,6,6,6,6,7,7,8,8,8,9,10,11,11,12,12,14,15,15,15,17,19,21,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,5,5,5,5,5,5,6,6,6,6,7,7,8,8,8,9,10,11,11,12,12,14,15,15,15,17,19,21,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,5,5,5,5,5,5,6,6,6,6,7,7,8,8,8,9,10,11,11,12,12,14,15,15,15,17,19,21,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,5,5,5,5,5,5,6,6,6,6,7,7,8,8,8,9,10,11,11,12,12,14,15,15,15,17,19,21,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,5,5,5,5,5,5,6,6,6,6,7,7,8,8,8,9,10,11,11,12,12,14,15,15,15,17,19,21,23,25,25,25,25,25},
         {1,3,3,4,4,4,4,4,5,5,5,6,6,6,6,7,7,7,7,8,8,8,9,10,10,11,11,12,13,14,15,17,18,20,23,23,25,25,25,25,25},
         {1,3,3,4,4,4,4,4,5,5,5,6,6,6,6,7,7,7,7,8,8,8,9,10,10,11,11,12,13,14,15,17,18,20,23,23,25,25,25,25,25},
         {1,3,3,4,4,4,4,4,5,5,5,6,6,6,6,7,7,7,7,8,8,8,9,10,10,11,11,12,13,14,15,17,18,20,23,23,25,25,25,25,25},
         {1,3,3,4,4,4,4,4,5,5,5,6,6,6,6,7,7,7,7,8,8,8,9,10,10,11,11,12,13,14,15,17,18,20,23,23,25,25,25,25,25},
         {1,3,3,4,4,4,4,4,5,5,5,6,6,6,6,7,7,7,7,8,8,8,9,10,10,11,11,12,13,14,15,17,18,20,23,23,25,25,25,25,25},
         {1,3,3,3,4,4,4,5,5,5,6,6,6,6,6,7,7,7,7,8,8,9,10,10,10,12,12,12,13,15,16,16,19,21,22,24,25,25,25,25,25},
         {1,3,3,3,4,4,4,5,5,5,6,6,6,6,6,7,7,7,7,8,8,9,10,10,10,12,12,12,13,15,16,16,19,21,22,24,25,25,25,25,25},
         {1,3,3,3,4,4,4,5,5,5,6,6,6,6,6,7,7,7,7,8,8,9,10,10,10,12,12,12,13,15,16,16,19,21,22,24,25,25,25,25,25},
         {1,3,3,3,4,4,4,5,5,5,6,6,6,6,6,7,7,7,7,8,8,9,10,10,10,12,12,12,13,15,16,16,19,21,22,24,25,25,25,25,25},
         {1,3,3,3,4,4,4,5,5,5,6,6,6,6,6,7,7,7,7,8,8,9,10,10,10,12,12,12,13,15,16,16,19,21,22,24,25,25,25,25,25},
         {1,3,3,4,4,4,4,4,5,5,6,6,6,6,6,6,7,7,7,8,8,9,9,10,10,11,12,13,13,15,15,17,18,21,22,25,25,25,25,25,25},
         {1,3,3,4,4,4,4,4,5,5,6,6,6,6,6,6,7,7,7,8,8,9,9,10,10,11,12,13,13,15,15,17,18,21,22,25,25,25,25,25,25},
         {1,3,3,4,4,4,4,4,5,5,6,6,6,6,6,6,7,7,7,8,8,9,9,10,10,11,12,13,13,15,15,17,18,21,22,25,25,25,25,25,25},
         {1,3,3,4,4,4,4,4,5,5,6,6,6,6,6,6,7,7,7,8,8,9,9,10,10,11,12,13,13,15,15,17,18,21,22,25,25,25,25,25,25},
         {1,3,3,4,4,4,4,4,5,5,6,6,6,6,6,6,7,7,7,8,8,9,9,10,10,11,12,13,13,15,15,17,18,21,22,25,25,25,25,25,25},
         {1,3,3,4,4,4,4,4,5,5,6,6,6,6,6,7,7,8,8,8,8,9,10,11,11,11,12,13,13,15,15,18,18,21,23,25,25,25,25,25,25},
         {1,3,3,4,4,4,4,4,5,5,6,6,6,6,6,7,7,8,8,8,8,9,10,11,11,11,12,13,13,15,15,18,18,21,23,25,25,25,25,25,25},
         {1,3,3,4,4,4,4,4,5,5,6,6,6,6,6,7,7,8,8,8,8,9,10,11,11,11,12,13,13,15,15,18,18,21,23,25,25,25,25,25,25},
         {1,3,3,4,4,4,4,4,5,5,6,6,6,6,6,7,7,8,8,8,8,9,10,11,11,11,12,13,13,15,15,18,18,21,23,25,25,25,25,25,25},
         {1,3,3,4,4,4,4,4,5,5,6,6,6,6,6,7,7,8,8,8,8,9,10,11,11,11,12,13,13,15,15,18,18,21,23,25,25,25,25,25,25},
         {1,3,3,4,4,4,4,5,5,5,6,6,6,6,6,7,7,8,8,8,9,9,11,11,11,11,13,13,14,15,15,18,19,20,23,25,25,25,25,25,25},
         {1,3,3,4,4,4,4,5,5,5,6,6,6,6,6,7,7,8,8,8,9,9,11,11,11,11,13,13,14,15,15,18,19,20,23,25,25,25,25,25,25},
         {1,3,3,4,4,4,4,5,5,5,6,6,6,6,6,7,7,8,8,8,9,9,11,11,11,11,13,13,14,15,15,18,19,20,23,25,25,25,25,25,25},
         {1,3,3,4,4,4,4,5,5,5,6,6,6,6,6,7,7,8,8,8,9,9,11,11,11,11,13,13,14,15,15,18,19,20,23,25,25,25,25,25,25},
         {1,3,3,4,4,4,4,5,5,5,6,6,6,6,6,7,7,8,8,8,9,9,11,11,11,11,13,13,14,15,15,18,19,20,23,25,25,25,25,25,25},
         {1,3,3,4,4,4,5,5,5,5,6,6,6,6,6,7,7,8,8,9,9,9,10,10,11,12,13,14,14,15,15,18,21,22,22,24,25,25,25,25,25},
         {1,3,3,4,4,4,5,5,5,5,6,6,6,6,6,7,7,8,8,9,9,9,10,10,11,12,13,14,14,15,15,18,21,22,22,24,25,25,25,25,25},
         {1,3,3,4,4,4,5,5,5,5,6,6,6,6,6,7,7,8,8,9,9,9,10,10,11,12,13,14,14,15,15,18,21,22,22,24,25,25,25,25,25},
         {1,3,3,4,4,4,5,5,5,5,6,6,6,6,6,7,7,8,8,9,9,9,10,10,11,12,13,14,14,15,15,18,21,22,22,24,25,25,25,25,25},
         {1,3,3,4,4,4,5,5,5,5,6,6,6,6,6,7,7,8,8,9,9,9,10,10,11,12,13,14,14,15,15,18,21,22,22,24,25,25,25,25,25},
         {1,3,3,4,4,4,5,5,5,5,6,6,6,6,7,7,8,8,9,9,9,9,11,11,11,12,13,14,15,15,16,19,20,20,24,25,25,25,25,25,25},
         {1,3,3,4,4,4,5,5,5,5,6,6,6,6,7,7,8,8,9,9,9,9,11,11,11,12,13,14,15,15,16,19,20,20,24,25,25,25,25,25,25},
         {1,2,3,3,3,3,4,4,5,5,5,5,5,5,5,6,6,6,7,7,8,8,8,9,9,10,10,11,12,14,14,16,16,19,19,20,20,20,20,20,20},
         {1,2,3,3,3,3,4,4,5,5,5,5,5,5,5,6,6,6,7,7,8,8,8,9,9,10,10,11,12,14,14,16,16,19,19,20,20,20,20,20,20},
         {1,2,3,3,3,3,4,4,5,5,5,5,5,5,5,6,6,6,7,7,8,8,8,9,9,10,10,11,12,14,14,16,16,19,19,20,20,20,20,20,20},
         {1,2,3,3,3,3,4,4,5,5,5,5,5,5,5,6,6,6,7,7,8,8,8,9,9,10,10,11,12,14,14,16,16,19,19,20,20,20,20,20,20},
         {1,2,3,3,3,3,4,4,5,5,5,5,5,5,5,6,6,6,7,7,8,8,8,9,9,10,10,11,12,14,14,16,16,19,19,20,20,20,20,20,20},
         {1,2,3,3,3,3,4,4,5,5,5,5,5,5,5,6,6,6,7,7,8,8,8,9,9,10,10,11,12,14,14,16,16,19,19,20,20,20,20,20,20},
         {1,2,3,3,3,3,4,4,5,5,5,5,5,5,5,6,6,6,7,7,8,8,8,9,9,10,10,11,12,14,14,16,16,19,19,20,20,20,20,20,20},
         {1,2,3,3,3,3,4,4,5,5,5,5,5,5,5,6,6,6,7,7,8,8,8,9,9,10,10,11,12,14,14,16,16,19,19,20,20,20,20,20,20},
         {1,2,3,3,3,3,4,4,5,5,5,5,5,5,5,6,6,6,6,7,7,7,7,9,9,10,11,11,12,14,14,16,18,19,20,20,20,20,20,20,20},
         {1,2,3,3,3,3,4,4,5,5,5,5,5,5,5,6,6,6,6,7,7,7,7,9,9,10,11,11,12,14,14,16,18,19,20,20,20,20,20,20,20}};

struct Env{
    int second_iter = 1500;
    void init(char** argv){
        second_iter = atoi(argv[3]);
    }
};

double elapsed_time(timespec& time_st){
    timespec time_now;
    clock_gettime(CLOCK_REALTIME, &time_now);
    return (time_now.tv_sec - time_st.tv_sec) + (time_now.tv_nsec - time_st.tv_nsec) / 1000000000.0;
}

struct Rnd{
    // doc: https://shibh308.github.io/library/library/lib/functions/xorshift.cpp.html
    uint64_t x;
    Rnd(uint64_t seed) :
            x(0xdeadbeef0110dead ^ seed)
    {
    }
    uint64_t rnd(){
        x ^= x << 7;
        x ^= x >> 9;
        return x;
    }
    uint64_t rnd(int n){
        return rnd() % n;
    }
    double rnd_double(){
        return 1.0 * rnd() / numeric_limits<uint64_t>::max();
    }
    vector<int> rnd_perm(int n){
        vector<int> v(n);
        iota(v.begin(), v.end(), 0);
        for(int i = n - 1; i >= 1; --i){
            int j = rnd(i + 1);
            swap(v[i], v[j]);
        }
        return v;
    }
    template<typename T>
    void shuffle(vector<T>& v){
        int n = v.size();
        for(int i = n - 1; i >= 1; --i){
            int j = rnd(i + 1);
            swap(v[i], v[j]);
        }
    }
    vector<vector<int>> shuffle_triangular_mat(const vector<vector<int>>& v){
        int n = v.size();
        auto perm = rnd_perm(n);
        vector<vector<int>> nv(v);
        for(int i = 0; i < n; ++i){
            for(int j = 0; j < i + 1; ++j){
                nv[i][j] = perm[i] <= perm[j] ? v[perm[j]][perm[i]] : v[perm[i]][perm[j]];
            }
        }
        return nv;
    }
};

vector<vector<int>> make_triangular_mat(int n){
    vector<vector<int>> v(n);
    for(int i = 0; i < n; ++i){
        v[i].resize(i + 1, 0);
    }
    return v;
}

template<int N = 5050>
bitset<N> from_triangular_mat(const vector<vector<int>>& graph){
    int n = graph.size();
    bitset<N> bs;
    int cn = 0;
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < i + 1; ++j){
            bs[cn++] = graph[i][j];
        }
    }
    return bs;
}

template<int N>
vector<vector<int>> to_triangular_mat(int n, const bitset<N>& graph){
    vector<vector<int>> v = make_triangular_mat(n);
    int cn = 0;
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < i + 1; ++j){
            v[i][j] = graph[cn++];
        }
    }
    return v;
}

int from_mat_idx(int i, int j){
    if(i < j){
        swap(i, j);
    }
    return i * (i + 1) / 2 + j;
}

vector<int> _table;
pair<int,int> to_mat_idx(int x){
    if(_table.empty()){
        _table.resize(5100, 0);
        int c = 0;
        for(int k = 1; k <= 5100; ++k){
            // 0, 1, 3, 6, 10
            // 0, 1, 1, 2, 2, 2, 3, 3, 3, 3
            if(c * (c + 1) / 2 < k){
                ++c;
            }
            _table[k - 1] = c;
        }
    }
    return {_table[x], x - _table[x] * (_table[x] - 1) / 2 };
}

const vector<pair<bitset<5050>, int>> warp_graphs(int n, int q, const vector<bitset<5050>>& graphs, double eps, uint32_t seed = 0, bool shuffle = true){
    int num_graphs = graphs.size();
    Rnd gen(seed);
    vector<pair<bitset<5050>, int>> after_graphs(q);
    for(int i = 0; i < q; ++i){
        int graph_idx = gen.rnd(num_graphs);
        auto g = graphs[graph_idx];
        for(int j = 0; j < n * (n + 1) / 2; ++j){
            if(gen.rnd_double() < eps){
                g.flip(j);
            }
        }
        for(int k = 0; k < n; ++k){
            g.reset(from_mat_idx(k, k));
        }
        if(shuffle){
            after_graphs[i] = {from_triangular_mat<>(gen.shuffle_triangular_mat(to_triangular_mat<5050>(n, g))), graph_idx};
        }
        else{
            after_graphs[i] = {g, graph_idx};
        }
    }
    return after_graphs;
}

#ifdef AVX512
int sum_vec(__m512i add_8bit, __m512i sub_8bit){
    auto zero = _mm512_setzero_si512();

    auto add_16_lo = _mm512_unpacklo_epi8(add_8bit, zero);
    auto add_16_hi = _mm512_unpackhi_epi8(add_8bit, zero);
    auto add_16 = _mm512_add_epi16(add_16_lo, add_16_hi);

    auto sub_16_lo = _mm512_unpacklo_epi8(sub_8bit, zero);
    auto sub_16_hi = _mm512_unpackhi_epi8(sub_8bit, zero);
    auto sub_16 = _mm512_add_epi16(sub_16_lo, sub_16_hi);

    auto sum_32 = _mm512_sub_epi16(add_16, sub_16);
    __m256i sum_32_l = _mm512_extracti64x4_epi64(sum_32, 0);
    __m256i sum_32_r = _mm512_extracti64x4_epi64(sum_32, 1);

    auto sum_16 = _mm256_sub_epi16(sum_32_l, sum_32_r);
    __m128i sum_16_l = _mm256_extractf128_si256(sum_16, 0);
    __m128i sum_16_r = _mm256_extractf128_si256(sum_16, 1);

    __m128i sum_8 = _mm_add_epi16(sum_16_l, sum_16_r);
    __m128i sum_8_shifted = _mm_srli_si128(sum_8, 8);

    uint64_t sum_4 = _mm_extract_epi64(_mm_add_epi16(sum_8, sum_8_shifted), 0);

    return short(sum_4 & 65535) + short((sum_4 >> 16) & 65535)
           + short((sum_4 >> 32) & 65535) + short((sum_4 >> 48) & 65535);
}
#else
int sum_vec(__m256i v1, __m256i v2){
    auto zero = _mm256_setzero_si256();

    // 8bit to 16bit
    auto sum_8bit = _mm256_add_epi8(v1, v2);
    // signed extension
    auto cmp = _mm256_cmpgt_epi8(zero, sum_8bit);
    auto sum_16bit_lo = _mm256_unpacklo_epi8(sum_8bit, cmp);
    auto sum_16bit_hi = _mm256_unpackhi_epi8(sum_8bit, cmp);
    auto sum_16 = _mm256_add_epi16(sum_16bit_lo, sum_16bit_hi);

    // 16bit to 32bit
    __m128i sum_16_l = _mm256_extractf128_si256(sum_16, 0);
    __m128i sum_16_r = _mm256_extractf128_si256(sum_16, 1);
    __m128i sum_8 = _mm_add_epi16(sum_16_l, sum_16_r);

    // 32bit to 64bit
    __m128i sum_8_shifted = _mm_srli_si128(sum_8, 8);
    uint64_t sum_4 = _mm_extract_epi64(_mm_add_epi16(sum_8, sum_8_shifted), 0);

    return short(sum_4 & 65535) + short((sum_4 >> 16) & 65535)
           + short((sum_4 >> 32) & 65535) + short((sum_4 >> 48) & 65535);
}
#endif

struct State{
    int n, n_ceil;
    int ed_sum = 0;
    vector<int> v;
    vector<vector<uint8_t>> fls;
    vector<vector<int>> lis;
    State(){}

    State(int num_groups, int group_size, const vector<int>& init, const vector<vector<int8_t>>& adj, Rnd& rnd) :
            n(num_groups * group_size),
            n_ceil((n + SKIP - 1) / SKIP * SKIP)
    {
        v = init;
        fls.resize(num_groups, vector<uint8_t>(n_ceil, 0));
        lis.resize(num_groups);
        for(int i = 0; i < n; ++i){
            fls[v[i]][i] = 255;
            lis[v[i]].emplace_back(i);
        }
        for(int i = 0; i < n; ++i){
            for(int j = 0; j < n; ++j){
                if(i != j && v[i] == v[j]){
                    ed_sum += adj[i][j];
                }
            }
        }
    }
    void reset(int xg, int yg, int xi, int yi, int bef){
        int x = lis[xg][xi];
        int y = lis[yg][yi];
        std::swap(lis[xg][xi], lis[yg][yi]);

        ed_sum = bef;
        fls[v[x]][x] = 0;
        fls[v[y]][y] = 0;
        std::swap(v[x], v[y]);
        fls[v[x]][x] = 255;
        fls[v[y]][y] = 255;
    }
    bool swap(const vector<vector<int8_t>>& adj, int xg, int yg, int xi, int yi){

        int x = lis[xg][xi];
        int y = lis[yg][yi];
        std::swap(lis[xg][xi], lis[yg][yi]);

        fls[v[x]][x] = 0;
        fls[v[y]][y] = 0;

        for(int i = 0; i < n_ceil; i += SKIP){
#ifdef AVX512
            __m512i mask_x = _mm512_loadu_si512((__m512i*)(fls[v[x]].data() + i));
            __m512i mask_y = _mm512_loadu_si512((__m512i*)(fls[v[y]].data() + i));

            __m512i adj_x = _mm512_loadu_si512((__m512i*)(adj[x].data() + i));
            __m512i adj_y = _mm512_loadu_si512((__m512i*)(adj[y].data() + i));

            auto xy = _mm512_and_si512(mask_x, adj_y);
            auto xx = _mm512_and_si512(mask_x, adj_x);
            auto yy = _mm512_and_si512(mask_y, adj_y);
            auto yx = _mm512_and_si512(mask_y, adj_x);

            // xy + yx - xx - yy
            auto add_val = _mm512_add_epi8(xy, yx);
            auto sub_val = _mm512_add_epi8(xx, yy);
#else

            __m256i mask_x = _mm256_loadu_si256((__m256i*)(fls[v[x]].data() + i));
            __m256i mask_y = _mm256_loadu_si256((__m256i*)(fls[v[y]].data() + i));

            __m256i adj_x = _mm256_loadu_si256((__m256i*)(adj[x].data() + i));
            __m256i adj_y = _mm256_loadu_si256((__m256i*)(adj[y].data() + i));

            // xy + yx - xx - yy
            auto v1 = _mm256_and_si256(mask_x, _mm256_sub_epi8(adj_y, adj_x));
            auto v2 = _mm256_and_si256(mask_y, _mm256_sub_epi8(adj_x, adj_y));

#endif

            ed_sum += sum_vec(v1, v2);

            /*
            int d1 = sum_vec(v1, v2);
            int d2 = 0;

            for(int j = 0; j < SKIP; ++j){
                if(fls[v[x]][i + j]){
                    d2 += adj[y][i + j] - adj[x][i + j];
                    // d2 -= adj[x][i + j] - adj[y][i + j];
                }
                if(fls[v[y]][i + j]){
                    d2 += adj[x][i + j] - adj[y][i + j];
                }
            }
            cout << d1 << " " << d2 << endl;
            assert(d1 == d2);
             */
        }

        std::swap(v[x], v[y]);
        fls[v[x]][x] = 255;
        fls[v[y]][y] = 255;
        return true;
    }
};

int solve(double eps, int num_groups, int group_size, const bitset<5050>& graph, const vector<vector<int>>& masks, Env& env, bool update_env = true){
    constexpr double tim = 0.048;
    if(update_env){
        if(eps <= 0.05){
            env.second_iter = 800;
        }else if(eps <= 0.1){
            env.second_iter = 500;
        }else if(eps <= 0.2){
            env.second_iter = 500;
        }else if(eps <= 0.25){
            env.second_iter = 800;
        }else if(eps <= 0.3){
            env.second_iter = 1500;
        }else if(eps <= 0.35){
            env.second_iter = 3500;
        }else{
            env.second_iter = 4500;
        }
    }
    timespec st;
    clock_gettime(CLOCK_REALTIME, &st);
    int n = num_groups * group_size;

    Rnd gen(42);

    vector<bitset<100>> graph_mat(n);
    int ed_sum = 0;
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < i; ++j){
            if(graph[from_mat_idx(i, j)]){
                graph_mat[i].set(j);
                graph_mat[j].set(i);
                ++ed_sum;
            }
        }
    }

    vector<vector<int8_t>> adj(n, vector<int8_t>((n + SKIP - 1) / SKIP * SKIP, 0));
    for(int i = 0; i < n; ++i){
        for(int j = 0; j <= i; ++j){
            auto bs = graph_mat[i] ^ ~graph_mat[j];
            for(int k = n; k < 100; ++k){
                bs.reset(k);
            }
            bs.reset(i);
            bs.reset(j);
            adj[i][j] = adj[j][i] = bs.count();
        }
    }

    vector<int> init(n);
    for(int i = 0; i < n; ++i){
        init[i] = i % num_groups;
    }

    pair<double,int> best(1e9, 0);

    for(int _ = 0;; ++_){
        if(elapsed_time(st) >= tim){
            break;
        }
        gen.shuffle(init);

        State state(num_groups, group_size, init, adj, gen);

        int iter_max = env.second_iter;
        uint64_t rn = gen.rnd();
        for(int it = 0; it < iter_max; ++it){
            if(it & 1){
                rn >>= 32;
            }
            else{
                rn = gen.rnd();
            }
            int bef_sc = state.ed_sum;
            uint32_t x = (rn & 65535) % n;
            uint32_t y = (rn >> 16) % n;
            int xg = x % num_groups;
            int xi = x / num_groups;
            int yg = y % num_groups;
            int yi = y / num_groups;
            if(state.swap(adj, xg, yg, xi, yi)){
                int aft_sc = state.ed_sum;
                if(aft_sc < bef_sc){
                    state.reset(xg, yg, xi, yi, bef_sc);
                }
            }
        }
        /*
        if(chmax(best_cn, state.ed_sum)){
            rn = gen.rnd();
            for(int it = 0; it < 500; ++it){
                if(it & 1){
                    rn >>= 32;
                }
                else{
                    rn = gen.rnd();
                }
                uint32_t x = (rn & 65535) % n;
                uint32_t y = (rn >> 16) % n;
                int bef_sc = state.ed_sum;
                if(state.swap(adj, x, y)){
                    int aft_sc = state.ed_sum;
                    if(aft_sc < bef_sc){
                        state.reset(x, y, bef_sc);
                    }
                }
            }
        }
         */
        vector<int> ed_cand(num_groups * (num_groups + 1) / 2, 0);
        vector<int> ed_real(num_groups * (num_groups + 1) / 2, 0);
        for(int i = 0; i < n; ++i){
            for(int j = 0; j < i; ++j){
                int idx = from_mat_idx(state.v[i], state.v[j]);
                ++ed_cand[idx];
                if(graph[from_mat_idx(i, j)]){
                    ++ed_real[idx];
                }
            }
        }

        vector<double> prob_vec;
        int cn = 0;
        for(int i = 0; i < num_groups; ++i){
            for(int j = 0; j <= i; ++j){
                prob_vec.emplace_back(1.0 * ed_real[cn] / ed_cand[cn]);
                ++cn;
            }
        }

        for(int q = 0; q < masks.size(); ++q){
            for(auto mask : masks[q]){
                double diff_sum = 0.0;

                for(int c = 0; c < prob_vec.size(); ++c){
                    double diff;
                    if(mask & (1 << c)){
                        diff = max(0.0, (1.0 - eps) - prob_vec[c]);
                    }
                    else{
                        diff = max(0.0, prob_vec[c] - eps);
                    }
                    if(c == 0 || c == 2 || c == 5 || c == 9 || c == 14){
                        if(eps >= 0.3){
                            diff /= 2.6;
                        }
                        else if(eps >= 0.2){
                            diff /= 2.2;
                        }
                        /*
                        else if(eps >= 0.05){
                            diff /= 1.4;
                        }
                         */
                        else{
                            diff /= 1.4;
                        }
                    }
                    diff_sum += diff;
                    if(get<0>(best) <= diff_sum){
                        break;
                    }
                }
                double sc = diff_sum;// - 3e-9 * state.ed_sum;
                if(sc < get<0>(best)){
                    best = make_pair(sc, q);
                }
            }
        }
    }
    // cout << endl;
    return get<1>(best);
}

bitset<5050> make_graph(int num_groups, int group_size, int mask){
    int n = num_groups * group_size;

    int c = 0;
    vector<int> labels(n);
    for(int l = 0; l < num_groups; ++l){
        for(int i = 0; i < group_size; ++i)labels[c++] = l;
    }

    vector<vector<int>> graph_flag_mat(num_groups, vector<int>(num_groups, 0));
    c = 0;
    for(int i = 0; i < num_groups; ++i){
        for(int j = 0; j <= i; ++j){
            graph_flag_mat[i][j] = graph_flag_mat[j][i] = bool(mask & (1 << (c++)));
        }
    }

    auto mat = make_triangular_mat(n);
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < i + 1; ++j){
            if(graph_flag_mat[labels[i]][labels[j]]){
                mat[i][j] = 1;
            }
            else{
                mat[i][j] = 0;
            }
        }
    }
    for(int i = 0; i < n; ++i){
        mat[i][i] = 0;
    }
    // cout << bitset<15>(mask) << " => " << cn << endl;
    /*
    for(int i = 0; i < n; ++i){
        cout << "# ";
        for(int j = 0; j < n; ++j){
            cout << mat[max(i, j)][min(i, j)];
        }
        cout << endl;
    }
     */
    return from_triangular_mat(mat);
}

template<int SIZE>
vector<vector<int>> enumerate_unlabeled_graphs(){
    constexpr int SZ = SIZE * (SIZE + 1) / 2;
    bitset<(1 << SZ)> exs;
    vector<vector<int>> ms;
    for(int mask = 0; mask < (1 << SZ); ++mask){
        bitset<SZ> bs(mask);
        vector<int> perm(SIZE, 0);
        bool fl = true;
        iota(perm.begin(), perm.end(), 0);
        unordered_set<int> us;
        do{
            int nbs = 0;
            int cc = 0;
            for(int x = 0; x < SIZE; ++x){
                for(int y = 0; y <= x; ++y){
                    if(bs[cc]){
                        int j = from_mat_idx(perm[x], perm[y]);
                        nbs |= (1 << j);
                    }
                    ++cc;
                }
            }
            if(us.count(nbs) == 0 && exs[nbs]){
                fl = false;
                break;
            }
            us.emplace(nbs);
            exs.set(nbs);
        }while(next_permutation(perm.begin(), perm.end()));
        if(fl){
            ms.emplace_back(us.begin(), us.end());
        }
    }
    return ms;
}

template<int SIZE>
vector<vector<int>> enumerate_unlabeled_dontselfloop_graphs(){
    constexpr int SZ = SIZE * (SIZE + 1) / 2;
    constexpr int SZM = SIZE * (SIZE - 1) / 2;
    bitset<(1 << SZ)> exs;
    vector<vector<int>> ms;
    for(int mask_ = 0; mask_ < (1 << SZM); ++mask_){
        int mask = 0;
        int c = 0;
        int c2 = 0;
        for(int i = 0; i < SIZE; ++i){
            for(int j = 0; j < i; ++j){
                if(mask_ & (1 << (c2++))){
                    mask |= (1 << c);
                }
                c++;
            }
            c++;
        }

        bitset<SZ> bs(mask);
        vector<int> perm(SIZE, 0);
        bool fl = true;
        iota(perm.begin(), perm.end(), 0);
        unordered_set<int> us;
        do{
            int nbs = 0;
            int cc = 0;
            for(int x = 0; x < SIZE; ++x){
                for(int y = 0; y <= x; ++y){
                    if(bs[cc]){
                        int j = from_mat_idx(perm[x], perm[y]);
                        nbs |= (1 << j);
                    }
                    ++cc;
                }
            }
            if(us.count(nbs) == 0 && exs[nbs]){
                fl = false;
                break;
            }
            us.emplace(nbs);
            exs.set(nbs);
        }while(next_permutation(perm.begin(), perm.end()));
        if(fl){
            ms.emplace_back(us.begin(), us.end());
        }
    }
    return ms;
}

pair<vector<bitset<5050>>, vector<vector<int>>> make_graphs(int num_groups, int group_size, int m, bool dontselfloop = false){
    vector<vector<int>> ms_;
    if(dontselfloop){
        if(m <= 11){
            ms_ = enumerate_unlabeled_dontselfloop_graphs<4>();
        }
        else if(m <= 34){
            ms_ = enumerate_unlabeled_dontselfloop_graphs<5>();
        }
        else{
            ms_ = enumerate_unlabeled_dontselfloop_graphs<6>();
        }
    }
    else{
        if(m <= 20){
            ms_ = enumerate_unlabeled_graphs<3>();
        }else if(m <= 90){
            ms_ = enumerate_unlabeled_graphs<4>();
        }else{
            ms_ = enumerate_unlabeled_graphs<5>();
        }
    }
    vector<bitset<5050>> ret;
    vector<vector<int>> ms;

    vector<pair<int,int>> vs;
    for(int i = 0; i < ms_.size(); ++i){
        vs.emplace_back(ms_[i].size(), i);
    }
    sort(vs.begin(), vs.end(), greater<>());
    for(auto [_, i] : vs){
        ms.emplace_back(ms_[i]);
        auto mat_bs = make_graph(num_groups, group_size, ms_[i][0]);
        ret.emplace_back(mat_bs);
        if(m <= ret.size()){
            break;
        }
    }
    return make_pair(ret, ms);
}

pair<int, int> simulate(int num_groups, int group_size, int m, double eps, Env& env, int seed = 0, bool update_env = true){

    timespec time_st;
    clock_gettime(CLOCK_REALTIME, &time_st);

    int n = num_groups * group_size;
    vector<bitset<5050>> graphs; vector<int> masks; vector<vector<int>> ms;
    tie(graphs, ms) = make_graphs(num_groups, group_size, m, eps == 0.0);
    clock_t st = clock();

    // auto graph = make_graph(n, sizes, 0, 0.0, 0);
    // graphs = { graph };

    double score = 1e9 / n;
    int cn = 0;
    for(auto [g, ans] : warp_graphs(n, num_queries, graphs, eps, seed, true)){

        int pred = solve(eps, num_groups, group_size, g, ms, env, update_env);
        if(pred == ans){
            ++cn;
        }
        else{
            score *= 0.9;
        }
        /*
        cout << pred << " - " << ans << endl;
        cout << endl;
         */
    }
    score = round(score);
    return {cn, score};
}

pair<int,double> get_group_data(int m, double eps){
    int num_groups, group_size;
    if(eps == 0.0){
        if(m <= 11){
            num_groups = 4;
        }
        else if(m <= 34){
            num_groups = 5;
        }
        else{
            num_groups = 6;
        }
    }
    else{
        if(m <= 20){
            num_groups = 3;
        }
        else if(m <= 90){
            num_groups = 4;
        }
        else{
            num_groups = 5;
        }
    }

    group_size = gsizes[m - 10][round(eps * 100)];
    return {num_groups, group_size};
}

#ifdef HAND
void main_test(){
    constexpr int THREADS = 12;
    vector<future<void>> v(THREADS);
    vector<Env> envs(THREADS);
    for(int i = 0; i < THREADS; ++i){
        auto func = [i, &envs] {
            ofstream of("outs/rand_out_" + to_string(i) + ".txt", ios_base::out | ios_base::app);
            Rnd gen(i64(time(NULL)) + 0xdeadbeef + i);
            while(true){
                int m = 10 + gen.rnd(91);
                double eps = gen.rnd(41) * 0.01;
                auto [num_groups, real_group_size] = get_group_data(m, eps);
                int group_size = gen.rnd(100 / num_groups) + 1;
                if(group_size + 4 <= real_group_size){
                    of << m << " " << eps << " " << group_size << " " << 0 << endl;
                }
                else if(real_group_size <= group_size - 4){
                    of << m << " " << eps << " " << group_size << " " << 100 << endl;
                }
                else{
                    auto [cn, sc] = simulate(num_groups, group_size, m, eps, envs[i]);
                    of << m << " " << eps << " " << group_size << " " << cn << endl;
                }
            }
        };
        v[i] = async(launch::async, func);
    }
    for(auto& x : v){
        x.get();
    }
}

void main_env(char** argv){
    Env env;
    env.init(argv);
    constexpr int CASES = 10;
    constexpr int THREADS = 10;
    vector<future<vector<int>>> v(THREADS);
    vector<Env> envs(THREADS);
    for(int i = 0; i < THREADS; ++i){
        auto func = [i, &env] {
            Rnd gen(0xdeadbeef + i);
            vector<int> cns;
            for(int j = 0; j < CASES; ++j){
                int m = 10 + gen.rnd(91);
                double eps = gen.rnd(41) * 0.01;
                int num_groups = get_group_data(m, eps).first;
                int group_size = gen.rnd(100 / num_groups) + 1;
                auto[cn, sc] = simulate(num_groups, group_size, m, eps, env, i + j, false);
                cns.emplace_back(cn);
            }
            return cns;
        };
        v[i] = async(launch::async, func);
    }

    int cn_sum = 0;
    vector<int> w;
    for(auto& x : v){
        for(auto y : x.get()){
            cn_sum += y;
        }
    }
    cout << cn_sum << endl;
}

void main_sim3(int argc, char** argv){
    int m = atoi(argv[1]);
    double eps = atof(argv[2]);
    int num_groups = get_group_data(m, eps).first;
    int group_size = atof(argv[3]);

    constexpr int THREADS = 10;
    // assert(THREADS % 2 == 0);
    vector<future<pair<int,int>>> v(THREADS);
    vector<Env> envs(THREADS);
    for(int i = 0; i < THREADS; ++i){
        auto func = [i, m, eps, num_groups, group_size, &envs] {
            /*
			if(i < THREADS / 2){
				return simulate(num_groups, group_size + 1, m, eps, envs[i], 0xdeadbeef + i);
			}
			else{
				return simulate(num_groups, group_size, m, eps, envs[i], 0xdeadbeef + i);
			}
             */
            return simulate(num_groups, group_size, m, eps, envs[i], 0xdeadbeef + i);
        };
        v[i] = async(launch::async, func);
    }

    i64 cn_sum1 = 0;
    i64 sc_sum1 = 0;
    // i64 cn_sum2 = 0;
    // i64 sc_sum2 = 0;
    for(int i = 0; i < THREADS; ++i){
        /*
        if(i < THREADS / 2){
            auto [cn, sc] = v[i].get();
            cn_sum1 += cn;
            sc_sum1 += sc;
        }
        else{
            auto [cn, sc] = v[i].get();
            cn_sum2 += cn;
            sc_sum2 += sc;
        }
         */
        auto [cn, sc] = v[i].get();
        cn_sum1 += cn;
        sc_sum1 += sc;
    }
    int cn_ave1 = round(1.0 * cn_sum1 / THREADS);
    // int cn_ave2 = round(1.0 * cn_sum2 / (THREADS / 2));
    // cout << cn_ave1 << " " << sc_sum1 << " " << cn_ave2 << " " << sc_sum2 << endl;
    cout << cn_ave1 << " " << sc_sum1 << endl;
}

void main_local(){
    constexpr int CASES = 10;
    constexpr int THREADS = 10;
    vector<future<i64>> v(THREADS);
    vector<Env> envs(THREADS);
    for(int i = 0; i < THREADS; ++i){
        auto func = [i, &envs] {
            Rnd gen(0xdeadbeef + i);
            i64 sc_sum = 0;
            for(int j = 0; j < CASES; ++j){
                int m = 10 + gen.rnd(91);
                double eps = gen.rnd(41) * 0.01;
                // double eps = 0.3 + gen.rnd(11) * 0.01;
                auto[num_groups, group_size] = get_group_data(m, eps);
                auto[cn, sc] = simulate(num_groups, group_size, m, eps, envs[i], i * CASES + j);
                sc_sum += sc;
            }
            return sc_sum;
        };
        v[i] = async(launch::async, func);
    }

    i64 sc_sum = 0;
    for(auto& x : v){
        sc_sum += x.get();
    }
    cout << "score: " << i64(50.0 * sc_sum / (CASES * THREADS)) << endl;
}

void main_local2(char** argv){
    int m_min = atoi(argv[1]);
    int m_max = atoi(argv[2]);
    int eps_min = atoi(argv[3]);
    int eps_max = atoi(argv[4]);
    Env env;
    env.second_iter = atoi(argv[5]);

    constexpr int CASES = 10;
    constexpr int THREADS = 10;
    vector<future<i64>> v(THREADS);
    for(int i = 0; i < THREADS; ++i){
        auto func = [i, m_min, m_max, eps_min, eps_max, &env] {
            Rnd gen(0xdeadbeef + i);
            i64 sc_sum = 0;
            for(int j = 0; j < CASES; ++j){
                int m = 10 + m_min + gen.rnd(m_max - m_min + 1);
                double eps = (eps_min + gen.rnd(eps_max - eps_min + 1)) * 0.01;
                auto[num_groups, group_size] = get_group_data(m, eps);
                auto[cn, sc] = simulate(num_groups, group_size, m, eps, env, i, false);
                sc_sum += sc;
            }
            return sc_sum;
        };
        v[i] = async(launch::async, func);
    }

    i64 sc_sum = 0;
    for(auto& x : v){
        sc_sum += x.get();
    }
    cout << sc_sum << endl;
}
#endif

int main(int argc, char** argv){

    if(!true){
        Env env;
        auto [cn, sc] = simulate(5, 20, 100, 0.4, env);
        cout << "simulate: " << cn << " " << sc << endl;
        exit(0);
    }

#ifdef HAND
#ifdef SIMULATE
    main_test();
#elif defined(SIMULATE2)
    main_env(argv);
#elif defined(SIMULATE3)
    main_sim3(argc, argv);
#elif defined(SIMULATE4)
    main_local2(argv);
#else
    main_local();
#endif
#else

    int m;
    double eps;

    cin >> m >> eps;

    auto [num_groups, group_size] = get_group_data(m, eps);
    auto [graphs, ms] = make_graphs(num_groups, group_size, m, eps == 0.0);
    Env env;
    int n = num_groups * group_size;
    cout << n << endl;

    for(int g = 0; g < m; ++g){
        for(int i = 0; i < n; ++i){
            for(int j = i + 1; j < n; ++j){
                cout << graphs[g][from_mat_idx(i, j)];
            }
        }
        cout << endl;
    }

    for(int q = 0; q < num_queries; ++q){
        string s;
        cin >> s;
        vector<vector<int>> mat = make_triangular_mat(n);
        int c = 0;
        for(int i = 0; i < n; ++i){
            mat[i][i] = 0;
            for(int j = i + 1; j < n; ++j){
                mat[j][i] = (s[c++] == '1');
            }
        }
        auto g = from_triangular_mat(mat);
        int pred = solve(eps, num_groups, group_size, g, ms, env);
        // cout << "#time: " << 1.0 * (clock() - st) / CLOCKS_PER_SEC << endl;
        cout << pred << endl;
    }
#endif
}