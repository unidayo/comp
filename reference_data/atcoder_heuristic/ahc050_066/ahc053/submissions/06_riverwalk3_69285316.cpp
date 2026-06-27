#include <bits/stdc++.h>
#define pb push_back
#define MID ((L+U)/2)
#define D ((U-L)/2)
using namespace std;
typedef long long ll;
const int C = 3e6;
int N, M; ll L, U;
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
vector<ll> a;
vector<ll> b;
vector<ll> radix[C];
vector<ll> m[C];
void Add(ll x)
{
    a.pb(x);
    cout<<x<<" ";
}
void Query()
{
    ll x; cin>>x; b.pb(x);
}
void Judge(vector<ll> a, vector<ll> b, vector<int> res)
{
    vector<ll> s(M);
    for(int i=0; i<res.size(); i++)
    {
        if(res[i] > 0)
            s[res[i]-1] += a[i];
    }
    ll diff = 0;
    for(int i=0; i<M; i++)
    {
        cout<<abs(b[i] - s[i])<<endl;
        diff += abs(b[i] - s[i]);
    }
    ll score = 5e7 * (20.0 - log10(diff + 1));
    cerr<<score * 150<<endl;
    return;
}
void Trivial_Solve(int N, int M, ll L, ll U)
{
    for(int i=0; i<N; i++)
    {
        Add(MID/10);
    }
    cout<<""<<endl;
    for(int i=0; i<M; i++)
    {
        Query();
    }
    for(int i=0; i<N; i++)
    {
        cout<<(i/10)+1<<" ";
    }
    cout<<""<<endl;
}
vector<ll> RadixSort(vector<ll> x)
{
    ll cur = ((ll)C) * ((ll)(C))/2;
    for(int i=0; i<C; i++)
    {
        m[i].clear(); radix[i].clear();
    }
    ll adj = MID/2 - (ll(C) * C/2);
    for(int i=0; i<x.size(); i++)
    {
        x[i] = x[i] - adj;
        m[x[i] % C].pb(x[i]);
    }
    for(int i=0; i<C; i++)
    {
        for(int j=0; j<m[i].size(); j++)
        {
            radix[m[i][j]/C].pb(m[i][j]);
        }
    }
    int counter = 0;
    for(int i=0; i<C; i++)
    {
        for(int j=0; j<radix[i].size(); j++)
        {
            x[counter] = radix[i][j] + adj;
            ++counter;
        }
    }
    return x;
}
void Solve1(int N, int M, ll L, ll U)
{
    for(int i=0; i<N/2; i++)
    {
        Add(MID/10 - D/10);
    }
    for(int i=0; i<N/2; i++)
    {
        Add(MID/10 + D/10);
    }
    cout<<""<<endl;
    for(int i=0; i<M; i++)
    {
        Query();
    }
    int ta = 0; int tb = 0;
    vector<int> res(N);
    int counter1 = 0; int counter2 = 0;
    for(int i=0; i<M; i++)
    {
        ll dev = b[i] - MID;
        ll cur = (dev + D + D/10) / (D/5);
        int tb_used = min(N/2-tb, (int)(cur));
        tb_used = max(10 - (N/2-ta), tb_used);
        for(int j=ta; j<ta + (10 - tb_used); j++)
        {
            res[j] = i+1;
        }
        for(int j=N/2 + tb; j < N/2 + tb + tb_used; j++)
        {
            res[j] = i+1;
        }
        ta += (10 - tb_used); tb += tb_used;
    }
    for(int i=0; i<N; i++)
    {
        cout<<res[i]<<" ";
    }
    cout<<""<<endl;
}
void Solve2(int N, int M, ll L, ll U)
{
    for(int i=0; i<N/2; i++)
    {
        Add(MID/8 - D/8);
    }
    for(int i=0; i<N/2; i++)
    {
        Add(MID/8 + D/8);
    }
    cout<<""<<endl;
    for(int i=0; i<M; i++)
    {
        Query();
    }
    int ta = 0; int tb = 0;
    vector<int> res(N);
    int counter1 = 0; int counter2 = 0;
    for(int i=0; i<M; i++)
    {
        ll dev = b[i] - MID;
        ll cur = (dev + D + D/8) / (D/4);
        int tb_used = min(N/2-tb, (int)(cur));
        tb_used = max(8 - (N/2-ta), tb_used);
        for(int j=ta; j<ta + (8 - tb_used); j++)
        {
            res[j] = i+1;
        }
        for(int j=N/2 + tb; j < N/2 + tb + tb_used; j++)
        {
            res[j] = i+1;
        }
        ta += (8 - tb_used); tb += tb_used;
    }
    assert(ta < N/2); assert(tb < N/2);
    for(int i=0; i<N; i++)
    {
        cout<<res[i]<<" ";
    }
    cout<<""<<endl;
    //Judge(a, b, res);
}
void Solve3(int N, int M, ll L, ll U)
{
    normal_distribution R((double)(MID/6), (double)(D)/sqrt(17));
    for(int i=0; i<N; i++)
    {
        Add(R(rng));
    }
    cout<<""<<endl;
    for(int i=0; i<M; i++)
    {
        Query();
    }
    vector<bool> used(N);
    vector<int> res(N);
    for(int i=0; i<M; i++)
    {
        vector<int> cur, cur3; vector<bool> used3(N);
        for(int j=0; j<N; j++)
        {
            if(!used[j])
            {
                cur3.pb(j);
            }
        }
        for(int j=0; j<min(240, (int)cur3.size()); j++)
        {
            int S = cur3.size();
            int t = rng() % S;
            if(!used3[t])
            {
                used3[t] = true;
                cur.pb(cur3[t]);
            }
            else
            {
                --j;
            }
        }
        //cur = cur3;
        vector<ll> p1, p2;
        for(int j=0; j<cur.size()/2; j++)
        {
            for(int k=j+1; k<cur.size()/2; k++)
            {
                for(int l=k+1; l<cur.size()/2; l++)
                {
                    p1.pb(a[cur[j]]+a[cur[k]]+a[cur[l]]);
                }
            }
        }
        for(int j=cur.size()/2; j<cur.size(); j++)
        {
            for(int k=j+1; k<cur.size(); k++)
            {
                for(int l=k+1; l<cur.size(); l++)
                {
                    p2.pb(a[cur[j]]+a[cur[k]]+a[cur[l]]);
                }
            }
        }
        sort(p1.begin(), p1.end()); sort(p2.begin(), p2.end());
        int counter = 0;
        ll best = b[i];
        ll t1 = 0; ll t2 = 0;
        vector<ll> used2;
        for(int j=p1.size()-1; j>=0; j--)
        {
            while(true)
            {
                if((counter == (p2.size()-1) || (abs(p1[j] + p2[counter] - b[i]) < abs(p1[j] + p2[counter+1] - b[i]))))
                {
                    break;
                }
                ++counter;
            }
            if(abs(p1[j] + p2[counter] - b[i]) < best)
            {
                best = abs(p1[j] + p2[counter] - b[i]);
                t1 = p1[j]; t2 = p2[counter];
            }
        }
        bool flag = false;
        for(int j=0; j<cur.size()/2; j++)
        {
            for(int k=j+1; k<cur.size()/2; k++)
            {
                for(int l=k+1; l<cur.size()/2; l++)
                {
                    if(a[cur[j]]+a[cur[k]]+a[cur[l]] == t1)
                    {
                        used2.pb(cur[j]); used2.pb(cur[k]); used2.pb(cur[l]);
                        flag = true;
                        break;
                    }
                }
                if(flag)
                {
                    break;
                }
            }
            if(flag)
            {
                break;
            }
        }
        flag = false;
        for(int j=cur.size()/2; j<cur.size(); j++)
        {
            for(int k=j+1; k<cur.size(); k++)
            {
                for(int l=k+1; l<cur.size(); l++)
                {
                    if(a[cur[j]]+a[cur[k]]+a[cur[l]] == t2)
                    {
                        used2.pb(cur[j]); used2.pb(cur[k]); used2.pb(cur[l]); flag = true; break;
                    }
                }
                if(flag)
                {
                    break;
                }
            }
            if(flag)
            {
                break;
            }
        }
        for(int j=0; j<used2.size(); j++)
        {
            //cout<<used2[j]<<" ";
            used[used2[j]] = true; res[used2[j]] = i+1;
        }
        //cout<<""<<endl;
    }
    for(int i=0; i<N; i++)
    {
        cout<<res[i]<<" ";
    }
    cout<<""<<endl;
    //Judge(a, b, res);
}
int main()
{
    cin>>N>>M>>L>>U;
    Solve3(N, M, L, U);
}
