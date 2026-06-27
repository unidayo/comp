#define _USE_MATH_DEFINES
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <complex>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <bitset>
#include <numeric>
#include <limits>
#include <climits>
#include <cfloat>
#include <functional>
#include <iterator>
#include <memory>
#include <regex>

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#endif
using namespace std;

class TicToc
{
private:
    double getCurrTime(){
    #ifdef _WIN32
        return GetTickCount() * 1e-3;
    #else
        uint32_t low, high;
        __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
        uint64_t t = ((uint64_t)low) | ((uint64_t)high << 32);
        return t / (1250000000LL * 2.45);
    #endif
    }
    double startTime;
public:
    void tic(){
        startTime = getCurrTime();
    }
    double toc(){
        return getCurrTime() - startTime;
    }
};

class Xorshift
{
private:
    uint32_t x, y, z, w;
public:
    Xorshift(uint32_t seed=88675123, int32_t loop=50){
        x = 123456789;
        y = 362436069;
        z = 521288629;
        w = seed;
        while(--loop >= 0){
            (*this)();
        }
    }
    uint32_t operator()(){
        uint32_t t=(x^(x<<11));
        x=y; y=z; z=w;
        return w=(w^(w>>19))^(t^(t>>8));
    }
    uint32_t operator()(uint32_t size){
        return (*this)() % size;
    }
    uint64_t get64(){
        uint64_t a = (*this)();
        uint64_t b = (*this)();
        return (a << 32) | b;
    }
    uint64_t get64(uint64_t size){
        return get64() % size;
    }
    template <class T>
    void shuffle(vector<T>& v){
        uint32_t n = v.size();
        for(uint32_t i=0; i<n-1; ++i){
            uint32_t j = (*this)(n-i) + i;
            swap(v[i], v[j]);
        }
    }
};
Xorshift xorshift;

/************************************************************************/

const int dy[] = {0, -1, 0, 1};
const int dx[] = {-1, 0, 1, 0};

const int CONNECT[][4] =
{
    {1, 0, -1, -1},
    {3, -1, -1, 0},
    {-1, -1, 3, 2},
    {-1, 2, 1, -1},
    {1, 0, 3, 2},
    {3, 2, 1, 0},
    {2, -1, 0, -1},
    {-1, 3, -1, 1},
};

const int ROTATE[] = {1, 2, 3, 0, 5, 4, 7, 6};
const int ROTATE_CNT[] = {4, 4, 4, 4, 2, 2, 2, 2};

class SolverSub
{
private:
    int h, w;
    vector<vector<int> > tile;
    vector<bitset<32> > used;

    vector<vector<int> > maxLen;
    vector<vector<vector<vector<int> > > > ans;

    void dfs(int y, int x, int d, int len)
    {
        if(!(0 <= y && y < h && 0 <= x && x < w)){
            if(maxLen[y+1][x+1] < len){
                maxLen[y+1][x+1] = len;
                ans[y+1][x+1] = tile;
            }
            return;
        }

        if(used[y][x]){
            int d2 = CONNECT[tile[y][x]][d];
            if(d2 != -1){
                int y2 = y + dy[d2];
                int x2 = x + dx[d2];
                dfs(y2, x2, (d2 + 2) % 4, len+1);
            }
            return;
        }

        used[y][x] = true;

        for(int i=0; i<ROTATE_CNT[tile[y][x]]; ++i){
            int d2 = CONNECT[tile[y][x]][d];
            if(d2 != -1){
                int y2 = y + dy[d2];
                int x2 = x + dx[d2];
                dfs(y2, x2, (d2 + 2) % 4, len+1);
            }
            tile[y][x] = ROTATE[tile[y][x]];
        }

        used[y][x] = false;
    }

public:
    SolverSub()
    {
        h = w = 0;
    }
    SolverSub(const vector<vector<int> >& tile)
    {
        this->tile = tile;
        h = tile.size();
        w = tile[0].size();
        used.assign(h, 0);
    }

    void solve(int y, int x)
    {
        int d;
        if(y == -1)
            d = 1;
        else if(y == h)
            d = 3;
        else if(x == -1)
            d = 0;
        else
            d = 2;

        maxLen.assign(h + 2, vector<int>(w + 2, -1));
        ans.assign(h + 2, vector<vector<vector<int> > >(w + 2, {{}}));

        y -= dy[d];
        x -= dx[d];
        dfs(y, x, d, 0);
    }

    int getMaxLen(int y, int x)
    {
        return maxLen[y+1][x+1];
    }

    const vector<vector<int> >& getAns(int y, int x)
    {
        return ans[y+1][x+1];
    }
};

class Solver
{
private:
    int h, w;
    vector<vector<int> > initTile;
    vector<vector<int> > tile;

    vector<vector<int> > getSubTile(int y1, int x1, int y2, int x2)
    {
        int h2 = y2 - y1;
        int w2 = x2 - x1;
        vector<vector<int> > subTile(h2, vector<int>(w2));
        for(int y=0; y<h2; ++y){
            for(int x=0; x<w2; ++x){
                subTile[y][x] = tile[y1+y][x1+x];
            }
        }
        return subTile;
    }

    void setSubTile(int y1, int x1, const vector<vector<int> >& subTile)
    {
        int h2 = subTile.size();
        int w2 = subTile[0].size();
        for(int y=0; y<h2; ++y){
            for(int x=0; x<w2; ++x){
                tile[y1+y][x1+x] = subTile[y][x];
            }
        }
    }

    void rotateMat(vector<vector<int> >& mat)
    {
        int h = mat.size();
        int w = mat[0].size();
        vector<vector<int> > mat2(w, vector<int>(h));
        for(int y=0; y<h; ++y){
            for(int x=0; x<w; ++x){
                mat2[w-1-x][y] = ROTATE[mat[y][x]];
            }
        }
        mat = move(mat2);
    }

public:
    void solve()
    {
        int yDiv = 4;
        int xDiv = 4;
        vector<int> sy(yDiv + 1);
        vector<int> sx(xDiv + 1);
        for(int yi=0; yi<=yDiv; ++yi)
            sy[yi] = h * yi / yDiv;
        for(int xi=0; xi<=xDiv; ++xi)
            sx[xi] = w * xi / xDiv;

        int maxScore = -1;
        vector<vector<int> > bestTile;

        for(int j=0; j<4; ++j){
            tile = initTile;
            vector<vector<SolverSub> > ss(yDiv, vector<SolverSub>(xDiv));
            vector<int> score(2, 0);

            for(int yi=0; yi<yDiv; ++yi){
                int yPrev = -1;
                for(int xi=0; xi<xDiv; ++xi){
                    int h2 = sy[yi+1] - sy[yi];
                    int w2 = sx[xi+1] - sx[xi];
                    vector<vector<int> > subTile = getSubTile(sy[yi], sx[xi], sy[yi+1], sx[xi+1]);

                    ss[yi][xi] = SolverSub(subTile);
                    if(xi == 0){
                        if(yi % 2 == 0)
                            ss[yi][xi].solve(h2, 0);
                        else
                            ss[yi][xi].solve(-1, 0);
                    }
                    else{
                        ss[yi][xi].solve(yPrev, -1);
                    }

                    int maxLen = -1;
                    if(xi == xDiv - 1){
                        if(yi % 2 == 0)
                            continue;

                        int x2 = -1;
                        for(int x=0; x<w2; ++x){
                            int len1 = ss[yi-1][xi].getMaxLen(sy[yi]-sy[yi-1], x);
                            int len2 = ss[yi][xi].getMaxLen(-1, x);
                            if(len1 != -1 && len2 != -1 && maxLen < len1 + len2){
                                maxLen = len1 + len2;
                                x2 = x;
                            }
                        }
                        subTile = ss[yi-1][xi].getAns(sy[yi]-sy[yi-1], x2);
                        setSubTile(sy[yi-1], sx[xi], subTile);
                        subTile = ss[yi][xi].getAns(-1, x2);
                        setSubTile(sy[yi], sx[xi], subTile);
                    }
                    else{
                        for(int y=0; y<h2; ++y){
                            int len = ss[yi][xi].getMaxLen(y, w2);
                            if(maxLen < len){
                                maxLen = len;
                                yPrev = y;
                            }
                        }
                        subTile = ss[yi][xi].getAns(yPrev, w2);
                        setSubTile(sy[yi], sx[xi], subTile);
                    }
                    score[yi/2] += maxLen;
                }
            }

            if(maxScore < score[0] * score[1]){
                maxScore = score[0] * score[1];
                bestTile = tile;
            }

            rotateMat(initTile);
            rotateMat(bestTile);
            swap(h, w);
        }

        tile = bestTile;
    }

    void read()
    {
        h = w = 30;
        initTile.assign(h, vector<int>(w));
        for(int y=0; y<h; ++y){
            string s;
            cin >> s;
            for(int x=0; x<w; ++x){
                initTile[y][x] = s[x] - '0';
            }
        }
    }

    void write()
    {
        for(int y=0; y<h; ++y){
            for(int x=0; x<w; ++x){
                int ans = 0;
                while(initTile[y][x] != tile[y][x]){
                    initTile[y][x] = ROTATE[initTile[y][x]];
                    ++ ans;
                }
                cout << ans;
            }
        }
        cout << endl;
    }
};

int main()
{
    Solver s;
    s.read();
    s.solve();
    s.write();

    return 0;
}
