#include <algorithm>
#include <iostream>
#include <cassert>
#include <vector>
#include <random>
using namespace std;
typedef pair<int, int> P;

struct S
{
    int v;
    int i;
    int h;
};

void remove(std::vector<std::vector<int>> &b, int i, std::vector<S> &ans)
{
    ans.push_back(S{b[i].back() + 1, 0, 0});
    b[i].pop_back();
}

void remove(int c, std::vector<S> &ans)
{
    ans.push_back(S{c + 1, 0, 0});
}

void idou(std::vector<std::vector<int>> &b, int i, int j, int k, vector<S> &ans)
{
    ans.push_back(S{b[i][k] + 1, j + 1, (int)b[i].size() - k + 1});
    for (int l = k; l < (int)b[i].size(); l++)
    {
        b[j].push_back(b[i][l]);
    }
    b[i].resize(k);
}

void soroeru(std::vector<std::vector<int>> &b, int i, int n, int m, int &c, int *sorted_size, vector<S> &ans)
{
    int left;
    for (left = 0; left < (int)b[i].size(); left++)
    {
        bool f = true;
        for (int j = left + 1; j < (int)b[i].size(); j++)
        {
            if (b[i][j] > b[i][left])
            {
                f = false;
                break;
            }
        }
        if (!f)
        {
            break;
        }
    }
    int r[12];
    for (int x = 0; x < m; x++)
    {
        r[x] = -1;
    }
    while ((int)b[i].size() > left)
    {
        if (b[i].back() == c)
        {
            remove(b, i, ans);
            c++;
            continue;
        }
        int j = b[i].size() - 1;
        int l;
        for (l = j; l >= 0; l--)
        {
            if (l == 0 || b[i][l] < b[i][l - 1])
            {
                break;
            }
        }
        if (b[i][l] == c)
        {
            l++;
        }
        int p = -1;
        for (int x = 0; x < m; x++)
        {
            if (x == i)
            {
                continue;
            }
            if (r[x] < b[i][l] && (p == -1 || r[x] > r[p] || (r[x] == r[p] && (b[p].empty() || (b[x].size() && b[x].back() < b[i][l] && (b[p].back() > b[i][l] || b[p].back() < b[x].back()))))))
            {
                p = x;
            }
        }
        if (p == -1)
        {
            int x = (i + 1) % m, y = (i + 2) % m;
            for (int u = (int)b[x].size() - 1; u >= 0; u--)
            {
                if (u == 0 || b[x][u - 1] < b[i].back())
                {
                    int c = (int)b[x].size() - u;
                    idou(b, x, y, u, ans);
                    idou(b, i, x, j, ans);
                    idou(b, y, x, (int)b[y].size() - c, ans);
                    break;
                }
            }
            continue;
        }
        idou(b, i, p, l, ans);
        r[p] = b[p].back();
    }
    int s = n;
    if (left)
    {
        s = b[i][left - 1];
    }
    while (true)
    {
        int p = -1;
        for (int x = 0; x < m; x++)
        {
            if (i == x || b[x].size() == sorted_size[x])
            {
                continue;
            }
            if (b[x].back() < s && (p == -1 || b[x].back() > b[p].back()))
            {
                p = x;
            }
        }
        if (p == -1)
        {
            break;
        }
        int ss = b[p].back();
        if((int)b[p].size() - sorted_size[p] >= 2 && b[p][(int)b[p].size() - 2] > b[p].back() && b[p][(int)b[p].size() - 2] < s)
        {
            idou(b, p, i, (int)b[p].size() - 2, ans);
        }
        else
        {
            idou(b, p, i, (int)b[p].size() - 1, ans);
        }
        s = ss;
    }
    sorted_size[i] = b[i].size();
}

vector<S> solve(vector<vector<int>> b, vector<int> &p, int n, int m)
{
    vector<S> ans;
    int c = 0;
    int sorted_size[12]{0};
    for (int i = 0; i < m; i++)
    {
        while (true)
        {
            bool f = false;
            for (int x = 0; x < m; x++)
            {
                if (b[x].size() && b[x].back() == c)
                {
                    remove(b, x, ans);
                    c++;
                    sorted_size[x] = min(sorted_size[x], (int)b[x].size());
                    f = true;
                }
            }
            if (!f)
            {
                break;
            }
        }
        soroeru(b, p[i], n, m, c, sorted_size, ans);
    }
    while (c < n)
    {
        remove(c++, ans);
    }
    return ans;
}

int calc(vector<S> &ans)
{
    int res = 10000;
    for (S s : ans)
    {
        res -= s.h;
    }
    return res;
}

int main()
{
    int start_time = clock();
    mt19937 mtseed(0);
    int n, m;
    cin >> n >> m;
    vector<vector<int>> b(m, vector<int>(n / m));
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n / m; j++)
        {
            cin >> b[i][j];
            b[i][j]--;
        }
    }
    vector<int> p(m);
    for (int i = 0; i < m; i++)
    {
        p[i] = i;
    }
    vector<S> ans;
    int s = -10000000;
    while(clock() - start_time < 1900000)
    {
        vector<S> temp = solve(b, p, n, m);
        int t = calc(temp);
        if (t > s)
        {
            ans = temp;
            s = t;
        }
        mt19937 mt(mtseed());
        shuffle(p.begin(), p.end(), mt);
    }
    for (S s : ans)
    {
        cout << s.v << " " << s.i << endl;
    }
}
