// C
#ifndef _GLIBCXX_NO_ASSERT
#include <cassert>
#endif
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#if __cplusplus >= 201103L
#include <ccomplex>
#include <cfenv>
#include <cinttypes>
#include <cstdalign>
#include <cstdbool>
#include <cstdint>
#include <ctgmath>
#include <cuchar>
#include <cwchar>
#include <cwctype>
#endif

// C++
#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>

#include <array>
#include <atomic>
#include <chrono>
#include <codecvt>
#include <condition_variable>
#include <forward_list>
#include <future>
#include <initializer_list>
#include <mutex>
#include <random>
#include <ratio>
#include <regex>
#include <scoped_allocator>
#include <system_error>
#include <thread>
#include <tuple>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

#if __cplusplus >= 201402L
#include <shared_mutex>
#endif

#if __cplusplus >= 201703L
#include <charconv>
#endif

using namespace std;
using namespace std::chrono;
using ll = long long;
using ull = unsigned long long;
using pll = pair<long long, long long>;
using tlll = tuple<long long, long long, long long>;
using tllll = tuple<long long, long long, long long, long long>;
std::mt19937 mt(time(0)); //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
std::uniform_int_distribution<ll> toobig(0, 1e9);
string argnum;

class xor_shift_128
{
public:
    typedef uint32_t result_type;
    xor_shift_128(result_type seed = mt())
    {
        set_seed(seed);
    }
    void set_seed(result_type seed)
    {
        a = seed = 1812433253 * (seed ^ (seed >> 30));
        b = seed = 1812433253 * (seed ^ (seed >> 30)) + 1;
        c = seed = 1812433253 * (seed ^ (seed >> 30)) + 2;
        d = seed = 1812433253 * (seed ^ (seed >> 30)) + 3;
    }
    result_type operator()()
    {
        result_type t = (a ^ (a << 11));
        a = b;
        b = c;
        c = d;
        return d = (d ^ (d >> 19)) ^ (t ^ (t >> 8));
    }
    static constexpr result_type max() { return numeric_limits<result_type>::max(); }
    static constexpr result_type min() { return numeric_limits<result_type>::min(); }

private:
    result_type a, b, c, d;
};
xor_shift_128 xorrand;
ifstream in;
ofstream out;

using pid = pair<int, double>;
using pdd = pair<double, double>;

class UnionFind {
    
public:
    int sz;
    vector<int> par, size_;
    UnionFind(){}
    UnionFind(int node_size) : sz(node_size), par(sz), size_(sz, 1){
        iota(par.begin(), par.end(), 0);
    }
    int find(int x){
        if(par[x] == x) return x;
        else return par[x] = find(par[x]);
    }
    void unite(int x,int y){
        x = find(x), y = find(y);
        if(x == y) return;
        if(size_[x] < size_[y]) swap(x,y);
        par[y] = x;
        size_[x] += size_[y];
    }
    int size(int x){
        x = find(x);
        return size_[x];
    }
    bool same(int x,int y){
        return find(x) == find(y);
    }
};

vector<pll> wasd = {{1,0},{-1,0},{0,1},{0,-1}};
class solver
{
    double temp;
    vector<vector<int>> grid,gridon,isver;
    int nowver=0,allver=0;

    UnionFind uf;
    int div = 2000;
    public:
    int score = 1;
    void init()
    {
        int fish;
        cin>>fish;
        grid = vector<vector<int>>(100000/div,vector<int>(100000/div,0));
        gridon = vector<vector<int>>(100000/div,vector<int>(100000/div,0));
        isver = vector<vector<int>>(100000/div+1,vector<int>(100000/div+1,0));
        for(int i=0;i<fish;i++)
        {
            int x,y;
            cin>>x>>y;
            grid[x/div][y/div]++;
        }

        for(int i=0;i<fish;i++)
        {
            int x,y;
            cin>>x>>y;
            grid[x/div][y/div]--;
        }

        uf = UnionFind(9);

        reset();
    }
    void reset()
    {
        for(int i=0;i<grid.size();i++)
        {
            for(int j=0;j<grid.size();j++)
            {
                if(gridon[i][j])
                {
                    swapgrid(i,j);
                }
            }
        }
        score = 1;
        for(int i=(100000/div)*30/100;i<=(100000/div)*70/100;i++)
        {
            for(int j=(100000/div)*30/100;j<=(100000/div)*70/100;j++)
            {
                swapgrid(i,j);
                score += grid[i][j];
            }
        }
    }
    void modify(double t)
    {
        temp = t;

        int chox=xorrand()%grid.size(),choy=xorrand()%grid.size();

        while(!canswap(chox,choy))
        {
            chox=xorrand()%grid.size();
            choy=xorrand()%grid.size();
        }

        int scorediff = grid[chox][choy]*(gridon[chox][choy]?-1:1);

        if(calc_prob(scorediff))
        {
            swapgrid(chox,choy);
            if(nowver>1000||allver>400000/div)
            {
                swapgrid(chox,choy);
                return;
            }
            score += scorediff;
        }
    }
    
    int deka = (1 << 16);
    bool calc_prob(double diff)
    {
        if (diff >= 0)
        {
            return true;
        }
        return exp(diff / temp) * deka > xorrand() % deka;
    }

    vector<pll> qezc={{-1,-1},{1,-1},{-1,1},{1,1}};
    bool canswap(int x,int y)
    {
        int now = gridon[x][y];

        bool ok = false;
        for(auto &w:wasd)
        {
            int xx = x+w.first,yy=y+w.second;
            if(xx<0||xx>=grid.size()||yy<0||yy>=grid.size())
            continue;
            if(gridon[xx][yy]!=now)
            ok=true;
        }
        if(!ok)
        return false;

        for(int i=0;i<9;i++)
        {
            uf.par[i] = i;
            uf.size_[i] = 1;
        }

        ok=false;
        int cnt = 0;
        for(auto &w:qezc)
        {
            int xx = x+w.first,yy=y+w.second;
            if(xx<0||xx>=grid.size()||yy<0||yy>=grid.size())
            continue;
            if(gridon[xx][yy]==now)
            for(auto &ww:wasd)
            {
                int xxx = xx+ww.first,yyy=yy+ww.second;
                if(xxx<0||xxx>=grid.size()||yyy<0||yyy>=grid.size()||xxx<x-1||xxx>x+1||yyy<y-1||yyy>y+1)
                continue;
                if(gridon[xxx][yyy]==now)
                uf.unite((1+w.first+ww.first)*3+(1+w.second+ww.second),(1+w.first)*3+(1+w.second));
            }
        }

        for(int i=-1;i<=1;i++)
        {
            for(int j=-1;j<=1;j++)
            {
                if(i==0&&j==0)
                continue;
                int xx=x+i,yy=y+j;
                if(xx<0||xx>=grid.size()||yy<0||yy>=grid.size())
                continue;

                cnt+= (now==gridon[xx][yy]);
            }
        }

        for(int i=-1;i<=1;i++)
        {
            for(int j=-1;j<=1;j++)
            {
                if(i==0&&j==0)
                continue;
                int xx=x+i,yy=y+j;
                if(xx<0||xx>=grid.size()||yy<0||yy>=grid.size())
                continue;

                if(uf.size((i+1)*3+j+1)!=cnt&&gridon[xx][yy]==now)
                return false;
            }
        }

        return true;
    }

    void swapgrid(int x, int y)
    {
        gridon[x][y] ^= 1;
        for(int i=0;i<2;i++)
        {
            for(int j=0;j<2;j++)
            {
                allver -= ((isver[x+i][y+j]==0||isver[x+i][y+j]==4)*-1);
                isver[x+i][y+j]+=(gridon[x][y]?1:-1);
                nowver += (isver[x+i][y+j]%2 ? 1 : -1);
                allver += ((isver[x+i][y+j]==0||isver[x+i][y+j]==4)*-1);
            }
        }
    }

    bool existline(int xa,int ya,int xb,int yb)
    {
        if(xa>xb||ya>yb)
        {
            swap(xa,xb);
            swap(ya,yb);
        }
        int cnt = 0;

        if(xb-xa==1)
        {
            if(xa>=0&&xa<grid.size()&&ya>=0&&ya<grid.size())
            cnt+=gridon[xa][ya];
            if(xa>=0&&xa<grid.size()&&ya-1>=0&&ya-1<grid.size())
            cnt+=gridon[xa][ya-1];
        }
        else
        {
            if(xa>=0&&xa<grid.size()&&ya>=0&&ya<grid.size())
            cnt+=gridon[xa][ya];
            if(xa-1>=0&&xa-1<grid.size()&&ya>=0&&ya<grid.size())
            cnt+=gridon[xa-1][ya];
        }
        return cnt==1;
    }

    vector<pll> answer()
    {
        int x,y;
        for(int i=0;i<isver.size();i++)
        {
            for(int j=0;j<isver.size();j++)
            {
                if(isver[i][j]==3)
                {
                    x=i;y=j;
                }
            }
        }

        int stx=x,sty=y;

        vector<pll> ans;
        pll dir;
        for(auto &w:wasd)
        {
            int xx=x+w.first,yy=y+w.second;
            if(xx<0||xx>=isver.size()||yy<0||yy>=isver.size())
            continue;
            if(existline(x,y,xx,yy))
            {
                dir=w;
                x=xx;
                y=yy;
                if(isver[x][y]%2)
                ans.push_back({x*div,y*div});
                break;
            }
        }

        while(x!=stx||y!=sty)
        {
            if(isver[x][y]%2)
            {
                dir.first*=-1;
                dir.second*=-1;

                for(auto &w:wasd)
                {
                    if(w==dir)
                    continue;
                    int xx=x+w.first,yy=y+w.second;
                    if(xx<0||xx>=isver.size()||yy<0||yy>=isver.size())
                    continue;
                    if(existline(x,y,xx,yy))
                    {
                        dir=w;
                        x=xx;
                        y=yy;
                        if(isver[x][y]%2)
                        ans.push_back({x*div,y*div});
                        break;
                    }
                }
            }
            else
            {
                for(auto &w:wasd)
                {
                    if(w!=dir)
                    continue;
                    int xx=x+w.first,yy=y+w.second;
                    if(xx<0||xx>=isver.size()||yy<0||yy>=isver.size())
                    continue;
                    if(existline(x,y,xx,yy))
                    {
                        dir=w;
                        x=xx;
                        y=yy;
                        if(isver[x][y]%2)
                        ans.push_back({x*div,y*div});
                        break;
                    }
                }
            }
        }
        return ans;
        
    }
};
int main(int argc, char **argv)
{
    ios::sync_with_stdio(false);
    cin.tie(0);
    long long a = 0, b = 0, c = 0, d = 0, e = 0, f = 0, m = 0, n = 0, p = 0, q = 0, mod = 998244353;
    long double pi = 3.1415926535, ad = 0, bd = 0, cd = 0, dd = 0;
    string s1, s2;
    char moji, moja;

    if (argc > 2)
    {
        in.open(argv[1]);
        cin.rdbuf(in.rdbuf());
        out.open(argv[2]);
        cout.rdbuf(out.rdbuf());
    }

    solver sol;

    sol.init();
    auto starttime = chrono::system_clock::now();
    ll timelimit = 1950;
    ll loop = 32,nowloop = 1;
    ll bestscore=0;
    vector<pll> bestans;
    double starttemp = 1.5, endtemp = -0.5, nowtemp = starttemp;

    for (ll turn = 0; nowloop<=loop; turn++)
    {
        if ((turn & 31) == 31)
        {
            ll timediff = duration_cast<milliseconds>(chrono::system_clock::now() - starttime).count();
            if (timediff > timelimit * nowloop / loop)
            {
                n = turn;
                nowloop++;
                auto ans = sol.answer();
                if(bestscore<sol.score)
                {
                    bestans = ans;
                    bestscore = sol.score;
                }
                sol.reset();
            }
            nowtemp = endtemp - ((endtemp - starttemp) * pow(timelimit * nowloop / loop - timediff, 1)) / pow(timelimit / loop, 1);
            nowtemp = pow(10,nowtemp);
        }
        sol.modify(nowtemp);
    }
    cerr << n << endl;
    cout << bestans.size() << endl;
    for(auto &x:bestans)
    {
        cout << x.first << ' ' << x.second << endl;
    }
    cerr << "Score = " << bestscore << endl;
    return 0;
}