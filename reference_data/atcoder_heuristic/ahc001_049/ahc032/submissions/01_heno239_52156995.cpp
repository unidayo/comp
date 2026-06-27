#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#include<iostream>
#include<string>
#include<cstdio>
#include<vector>
#include<cmath>
#include<algorithm>
#include<functional>
#include<iomanip>
#include<queue>
#include<ciso646>
#include<random>
#include<map>
#include<set>
#include<bitset>
#include<stack>
#include<unordered_map>
#include<unordered_set>
#include<utility>
#include<cassert>
#include<complex>
#include<numeric>
#include<array>
#include<chrono>
using namespace std;

//#define int long long
typedef long long ll;

typedef unsigned long long ul;
typedef unsigned int ui;
//ll mod = 1;
constexpr int mod = 998244353;
//constexpr ll mod = 1000000007;
const int mod17 = 1000000007;
const ll INF = (ll)mod17 * mod17;
typedef pair<int, int>P;

#define rep(i,n) for(int i=0;i<n;i++)
#define per(i,n) for(int i=n-1;i>=0;i--)
#define Rep(i,sta,n) for(int i=sta;i<n;i++)
#define rep1(i,n) for(int i=1;i<=n;i++)
#define per1(i,n) for(int i=n;i>=1;i--)
#define Rep1(i,sta,n) for(int i=sta;i<=n;i++)
#define all(v) (v).begin(),(v).end()
typedef pair<ll, ll> LP;

using ld = double;
typedef pair<ld, ld> LDP;
const ld eps = 1e-10;
const ld pi = acosl(-1.0);

template<typename T>
void chmin(T& a, T b) {
	a = min(a, b);
}
template<typename T>
void chmax(T& a, T b) {
	a = max(a, b);
}
template<typename T>
vector<T> vmerge(vector<T>& a, vector<T>& b) {
	vector<T> res;
	int ida = 0, idb = 0;
	while (ida < a.size() || idb < b.size()) {
		if (idb == b.size()) {
			res.push_back(a[ida]); ida++;
		}
		else if (ida == a.size()) {
			res.push_back(b[idb]); idb++;
		}
		else {
			if (a[ida] < b[idb]) {
				res.push_back(a[ida]); ida++;
			}
			else {
				res.push_back(b[idb]); idb++;
			}
		}
	}
	return res;
}
template<typename T>
void cinarray(vector<T>& v) {
	rep(i, v.size())cin >> v[i];
}
template<typename T>
void coutarray(vector<T>& v) {
	rep(i, v.size()) {
		if (i > 0)cout << " "; cout << v[i];
	}
	cout << "\n";
}
ll mod_pow(ll x, ll n, ll m = mod) {
	if (n < 0) {
		ll res = mod_pow(x, -n, m);
		return mod_pow(res, m - 2, m);
	}
	if (abs(x) >= m)x %= m;
	if (x < 0)x += m;
	//if (x == 0)return 0;
	ll res = 1;
	while (n) {
		if (n & 1)res = res * x % m;
		x = x * x % m; n >>= 1;
	}
	return res;
}
//mod should be <2^31
struct modint {
	int n;
	modint() :n(0) { ; }
	modint(ll m) {
		if (m < 0 || mod <= m) {
			m %= mod; if (m < 0)m += mod;
		}
		n = m;
	}
	operator int() { return n; }
};
bool operator==(modint a, modint b) { return a.n == b.n; }
bool operator<(modint a, modint b) { return a.n < b.n; }
modint operator+=(modint& a, modint b) { a.n += b.n; if (a.n >= mod)a.n -= (int)mod; return a; }
modint operator-=(modint& a, modint b) { a.n -= b.n; if (a.n < 0)a.n += (int)mod; return a; }
modint operator*=(modint& a, modint b) { a.n = ((ll)a.n * b.n) % mod; return a; }
modint operator+(modint a, modint b) { return a += b; }
modint operator-(modint a, modint b) { return a -= b; }
modint operator*(modint a, modint b) { return a *= b; }
modint operator^(modint a, ll n) {
	if (n == 0)return modint(1);
	modint res = (a * a) ^ (n / 2);
	if (n % 2)res = res * a;
	return res;
}

ll inv(ll a, ll p) {
	return (a == 1 ? 1 : (1 - p * inv(p % a, a)) / a + p);
}
modint operator/(modint a, modint b) { return a * modint(inv(b, mod)); }
modint operator/=(modint& a, modint b) { a = a / b; return a; }
const int max_n = 1 << 20;
modint fact[max_n], factinv[max_n];
void init_f() {
	fact[0] = modint(1);
	for (int i = 0; i < max_n - 1; i++) {
		fact[i + 1] = fact[i] * modint(i + 1);
	}
	factinv[max_n - 1] = modint(1) / fact[max_n - 1];
	for (int i = max_n - 2; i >= 0; i--) {
		factinv[i] = factinv[i + 1] * modint(i + 1);
	}
}
modint comb(int a, int b) {
	if (a < 0 || b < 0 || a < b)return 0;
	return fact[a] * factinv[b] * factinv[a - b];
}
modint combP(int a, int b) {
	if (a < 0 || b < 0 || a < b)return 0;
	return fact[a] * factinv[a - b];
}

ll gcd(ll a, ll b) {
	a = abs(a); b = abs(b);
	if (a < b)swap(a, b);
	while (b) {
		ll r = a % b; a = b; b = r;
	}
	return a;
}
template<typename T>
void addv(vector<T>& v, int loc, T val) {
	if (loc >= v.size())v.resize(loc + 1, 0);
	v[loc] += val;
}
/*const int mn = 2000005;
bool isp[mn];
vector<int> ps;
void init() {
	fill(isp + 2, isp + mn, true);
	for (int i = 2; i < mn; i++) {
		if (!isp[i])continue;
		ps.push_back(i);
		for (int j = 2 * i; j < mn; j += i) {
			isp[j] = false;
		}
	}
}*/

//[,val)
template<typename T>
auto prev_itr(set<T>& st, T val) {
	auto res = st.lower_bound(val);
	if (res == st.begin())return st.end();
	res--; return res;
}

//[val,)
template<typename T>
auto next_itr(set<T>& st, T val) {
	auto res = st.lower_bound(val);
	return res;
}
using mP = pair<modint, modint>;
mP operator+(mP a, mP b) {
	return { a.first + b.first,a.second + b.second };
}
mP operator+=(mP& a, mP b) {
	a = a + b; return a;
}
mP operator-(mP a, mP b) {
	return { a.first - b.first,a.second - b.second };
}
mP operator-=(mP& a, mP b) {
	a = a - b; return a;
}
LP operator+(LP a, LP b) {
	return { a.first + b.first,a.second + b.second };
}
LP operator+=(LP& a, LP b) {
	a = a + b; return a;
}
LP operator-(LP a, LP b) {
	return { a.first - b.first,a.second - b.second };
}
LP operator-=(LP& a, LP b) {
	a = a - b; return a;
}

mt19937 mt(time(0));

const string drul = "DRUL";
string senw = "SENW";
//DRUL,or SENW
//int dx[4] = { 1,0,-1,0 };
//int dy[4] = { 0,1,0,-1 };
int dx[4] = { 1,0,-1,0 };
int dy[4] = { 0,1,0,-1 };
//------------------------------------

using ar = array<int, 3>;

using mar = array<int, 9>;


pair<ll, vector<ar>> sol(int n, int m, int k, vector<int> a, vector<vector<int>> s) {
	vector<mar> vs[10];
	vector<mar> vops[10];

	mar nw; rep(i, 9)nw[i] = 0;
	vector<int> cur;

	function<void()> dfs13 = [&]() {
		mar nwar; rep(i, 9)nwar[i] = -1;
		rep(i, cur.size())nwar[i] = cur[i];
		vs[cur.size()].push_back(nw);
		vops[cur.size()].push_back(nwar);
		if (cur.size() == 5)return;
		int sta = 0; if (cur.size())sta = cur.back();
		Rep(i, sta, m) {
			cur.push_back(i);
			rep(j, 9) {
				nw[j] += s[i][j];
				if (nw[j] >= mod)nw[j] -= mod;
			}
			dfs13();
			cur.pop_back();
			rep(j, 9) {
				nw[j] -= s[i][j];
				if (nw[j] < 0)nw[j] += mod;
			}
		}
	};
	dfs13();
	auto getvs = [&](int ma) {
		vector<mar> res;
		rep(i, ma + 1) {
			for (auto a : vs[i])res.push_back(a);
		}
		return res;
	};
	auto getvops = [&](int ma) {
		vector<mar> res;
		rep(i, ma + 1) {
			for (auto a : vops[i])res.push_back(a);
		}
		return res;
	};
	//cout << vs.size() << "\n";

	vector<vector<ar>> preop;
	vector<vector<int>> prea;
	vector<ll> preval;
	preop.push_back({});
	prea.push_back(a);
	preval.push_back(0);

	auto preupd = [&](int i, int j, vector<P> evals, int ran, int maran, int obj, int adma) {
		vector<vector<ar>> npreop;
		vector<vector<int>> nprea;
		vector<ll> npreval;

		priority_queue<ll, vector<ll>, greater<ll>> valque;
		vector<ar> cpreop;
		vector<int> cprea;
		ll cpreval;
		rep(loc, preop.size()) {
			int len = maran;
			if ((int)preop[loc].size() < obj) {
				len += min(adma, obj - (int)preop[loc].size());
			}
			rep(z, len + 1) {
				rep(t, vs[z].size()) {
					cpreop = preop[loc];
					cprea = prea[loc];
					cpreval = preval[loc];

					rep(x, 3)rep(y, 3) {
						int& v = cprea[(i + x) * 9 + (j + y)];
						v += vs[z][t][x * 3 + y];
						if (v >= mod)v -= mod;
					}
					for (auto p : evals) {
						int x = i + p.first, y = j + p.second;
						cpreval += cprea[x * 9 + y];
					}
					if (valque.size() >= ran && cpreval <= valque.top())continue;
					rep(k, 9) {
						if (vops[z][t][k] < 0)break;
						cpreop.push_back({ vops[z][t][k],i,j });
					}

					npreop.push_back(cpreop);
					nprea.push_back(cprea);
					npreval.push_back(cpreval);
					valque.push(cpreval);
					if (valque.size() > ran)valque.pop();
				}
			}
		}
		vector<LP> ps;
		rep(i, npreval.size()) {
			ps.push_back({ npreval[i],i });
		}
		sort(all(ps), greater<LP>());
		if (ps.size() > ran)ps.resize(ran);
		preop.clear();
		preval.clear();
		prea.clear();
		rep(i, ps.size()) {
			int id = ps[i].second;
			preop.push_back(npreop[id]);
			prea.push_back(nprea[id]);
			preval.push_back(npreval[id]);
		}
	};
	int optmp = 0; optmp++;
	rep(i, 6)rep(j, 6) {
		preupd(i, j, { {0,0} }, 1000, 1, optmp, 2);
		optmp++;
	}

	const int subran = 130;
	if (preop.size() >= subran) {
		preop.resize(subran);
		prea.resize(subran);
		preval.resize(subran);
	}
	rep(i, 6) {
		preupd(6, i, { {0,0},{1,0},{2,0} }, subran, 3, optmp, 1);
		optmp += 3;
	}
	for (int i = 6; i > 0; i--) {
		preupd(i, 6, { {2,0},{2,1},{2,2} }, subran, 3, optmp, 1);
		optmp += 3;
	}

	vector<ar> ops;
	ll alsum = 0;
	/*alsum = preval[0];
	a = prea[0];
	for (auto op : preop[0]) {
		ops.push_back(op);
	}*/

	ll ma = -1;
	int chk = -1;
	vector<int> mem;

	int tlen = min(6, (int)prea.size());

	auto t1 = clock();
	int lasx = 0, lasy = 6;
	function<void()> dfs9 = [&]() {
		rep(t, tlen) {
			ll sum = preval[t];
			rep(i, 3)rep(j, 3) {
				int v = prea[t][(lasx + i) * 9 + (lasy + j)];
				v += nw[i * 3 + j]; if (v >= mod)v -= mod;
				sum += v;
			}
			if (ma < sum) {
				ma = sum;
				mem = cur;
				chk = t;
			}
		}

		if (cur.size() == 8)return;
		int sta = 0; if (cur.size())sta = cur.back();
		Rep(i, sta, m) {
			cur.push_back(i);
			rep(j, 9) {
				nw[j] += s[i][j];
				if (nw[j] >= mod)nw[j] -= mod;
			}
			dfs9();
			cur.pop_back();
			rep(j, 9) {
				nw[j] -= s[i][j];
				if (nw[j] < 0)nw[j] += mod;
			}
		}
	};
	dfs9();
	auto t2 = clock();
	auto dif = (t2 - t1) / (ld)CLOCKS_PER_SEC;
	cerr << dif << "\n";

	alsum = ma;
	ops = preop[chk];
	for (int id : mem)ops.push_back({ id,lasx,lasy });

	return { alsum,ops };
}

void rotate(vector<vector<int>>& v, vector<vector<int>>& orid) {
	int n = v.size();
	auto nv = v;
	auto norid = orid;
	rep(i, n)rep(j, n) {
		nv[i][j] = v[n - 1 - j][i];
		norid[i][j] = orid[n - 1 - j][i];
	}
	swap(v, nv);
	swap(orid, norid);
}

void rotate1(vector<int>& v, vector<vector<int>>& orid) {
	int n = round(sqrt(v.size()));
	vector<vector<int>> a(n, vector<int>(n));
	rep(i, n)rep(j, n) {
		a[i][j] = v[i * n + j];
	}
	rotate(a, orid);
	rep(i, n * n)v[i] = a[i / n][i % n];
}
void solve() {

	int n, m, k; cin >> n >> m >> k;
	vector<int> ads;
	vector<int> ids;
	rep(i, 3)rep(j, 3) {
		ids.push_back(i * 3 + j);
		ads.push_back(i * 9 + j);
	}

	vector<int> a;
	rep(i, n)rep(j, n) {
		int x; cin >> x; a.push_back(x);
	}
	vector<vector<int>> s(m, vector<int>(9));
	rep(i, m)rep(j, 9)cin >> s[i][j];

	vector<vector<int>> aid(n, vector<int>(n));
	rep(i, n)rep(j, n)aid[i][j] = i * n + j;

	//iranai
	vector<vector<int>> sid(3, vector<int>(3));
	ll ma = -1;
	int chk = -1;
	vector<ar> ops;
	rep(r, 4) {
		auto p = sol(n, m, k, a, s);

		if (ma < p.first) {
			ma = p.first;
			chk = r;
			ops.clear();
			auto vv = p.second;
			rep(i, vv.size()) {
				int mi = n * n;
				rep(j, 3)rep(k, 3) {
					int x = vv[i][1] + j;
					int y = vv[i][2] + k;
					chmin(mi, aid[x][y]);
				}
				ops.push_back({ vv[i][0],mi / n,mi % n });
			}
		}

		rotate1(a, aid);
		rep(j, m)rotate1(s[j], sid);
	}
	cout << ops.size() << "\n";
	for (auto a : ops) {
		cout << a[0] << " " << a[1] << " " << a[2] << "\n";
	}
	cerr << ma << " " << chk << "\n";
}



signed main() {
	ios::sync_with_stdio(false);
	cin.tie(0);
	//cout << fixed<<setprecision(10);
	//init_f();
	//init();
	//while(true)
	//expr();
	//int t; cin >> t; rep(i, t)
	solve();
	return 0;
}