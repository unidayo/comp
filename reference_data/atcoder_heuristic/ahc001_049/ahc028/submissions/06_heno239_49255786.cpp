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
ll mod = 1;
//constexpr ll mod = 998244353;
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

const string ruld = "RULD";
string senw = "SENW";
//DRUL,or SENW
//int dx[4] = { 1,0,-1,0 };
//int dy[4] = { 0,1,0,-1 };

//------------------------------------

bool isp(int x) {
	if (x == 1)return false;
	for (int i = 2; i * i <= x; i++) {
		if (x % i == 0)return false;
	}
	return true;
}
//1.1*10^9だと2.2*10^9がintでoverflowする
mt19937 mtle(time(0));
uniform_int_distribution<> udle(900000000, 1000000000);
int findmodp() {
	int le = udle(mtle);
	while (!isp(le))le++;
	return le;
}
using ar = array<ll, 2>;

ar ms;
ar ts;

ar rhs[1005];
void init_hash() {
	rep(i, 2) {
		ms[i] = findmodp();
		ts[i] = findmodp();
		while (ts[i] % ms[i] == 0) {
			ts[i] = findmodp();
		}
		ts[i] %= ms[i];
	}
	rhs[0] = { 1,1 };
	rep(i, 1004) {
		rep(j, 2) {
			rhs[i + 1][j] = rhs[i][j] * ts[j] % ms[j];
		}
	}
}

ar operator*(ar a, ll v) {
	rep(j, 2) {
		ll adv = v % ms[j];
		if (adv < 0)adv += ms[j];
		a[j] *= adv; a[j] %= ms[j];
	}
	return a;
}
ar operator*(ar a, ar b) {
	rep(j, 2) {
		(a[j] *= b[j]) %= ms[j];
	}
	return a;
}
ar operator+(ar a, ar b) {
	rep(j, 2) {
		a[j] += b[j]; if (a[j] >= ms[j])a[j] -= ms[j];
	}
	return a;
}
ar operator-(ar a, ar b) {
	rep(j, 2) {
		a[j] -= b[j]; if (a[j] < 0)a[j] += ms[j];
	}
	return a;
}

ll get_hash(vector<int>& v) {
	ar res = { 0,0 };
	rep(i, v.size()) {
		res = res * rhs[1] + ar{ v[i] + 5,v[i] + 5 };
	}
	return res[0] * ms[1] + res[1];
}

vector<int> ran_array(int n) {
	vector<int> v(n);
	rep(i, n)v[i] = i;
	rep(i, n) {
		uniform_int_distribution<> ud(0, i);
		int to = ud(mt);
		swap(v[i], v[to]);
	}
	return v;
}
struct uf {
private:
	vector<int> par, ran;
public:
	uf(int n) {
		par.resize(n, 0);
		ran.resize(n, 0);
		rep(i, n) {
			par[i] = i;
		}
	}
	int find(int x) {
		if (par[x] == x)return x;
		else return par[x] = find(par[x]);
	}
	void unite(int x, int y) {
		x = find(x), y = find(y);
		if (x == y)return;
		if (ran[x] < ran[y]) {
			par[x] = y;
		}
		else {
			par[y] = x;
			if (ran[x] == ran[y])ran[x]++;
		}
	}
	bool same(int x, int y) {
		return find(x) == find(y);
	}
};



int n, m;
int sx, sy;
char a[15][15];
string t[200];
string adt[200][200];
int b[200][200];
vector<int> locs[26];
int adc[225][225];

void init() {
	cin >> n >> m;
	cin >> sx >> sy;
	rep(i, 15)rep(j, 15)cin >> a[i][j];
	rep(i, m)cin >> t[i];
	rep(i, m)rep(j, m) {
		for (int len = 1; len <= min(t[i].size(), t[j].size()); len++) {
			string sl = t[i].substr(t[i].size() - len, len);
			string sr = t[j].substr(0, len);
			if (sl == sr) {
				b[i][j] = len;
			}
		}
		adt[i][j] = t[j].substr(b[i][j], t[j].size() - b[i][j]);
	}
	rep(i, n)rep(j, n) {
		locs[a[i][j] - 'A'].push_back(i * n + j);
	}
	rep(i, n)rep(j, n)rep(k, n)rep(l, n) {
		int c = abs(i - k) + abs(j - l) + 1;
		adc[i * n + j][k * n + l] = c;
	}
}

int dp[10000][225];
int pre[10000][225];
unordered_map<ll, int> mp_score[16][16];
int calc_score(vector<int>& v, int sx, int sy) {
	ll hv = get_hash(v);
	auto& mp = mp_score[sx][sy];
	if (mp.find(hv) != mp.end())return mp[hv];

	int stac = t[v[0]][0] - 'A';
	if (sx == n) {
		for (int id : locs[stac])dp[0][id] = 0;
	}
	else {
		for (int id : locs[stac]) {
			int c = abs(sx - id / n) + abs(sy - id % n) + 1;
			dp[0][id] = c;
		}
	}
	int tmp = 0;
	char las = 'A' + stac;
	auto upd = [&](char c) {
		if (las == c) {
			for (int id : locs[c - 'A']) {
				dp[tmp + 1][id] = dp[tmp][id] + 1;
			}
		}
		else {
			for (int id : locs[c - 'A']) {
				dp[tmp + 1][id] = mod17;
				for (int pid : locs[las - 'A']) {
					int val = dp[tmp][pid] + adc[id][pid];
					chmin(dp[tmp + 1][id], val);
				}
			}
		}
		las = c;
		tmp++;
	};
	for (int i = 1; i < t[v[0]].size(); i++) {
		upd(t[v[0]][i]);
	}
	rep(i, v.size() - 1) {
		string& ads = adt[v[i]][v[i + 1]];
		for (char c : ads)upd(c);
	}
	int mi = mod17;
	for (int id : locs[las - 'A']) {
		chmin(mi, dp[tmp][id]);
	}
	return mp[hv] = mi;
}
vector<P> get_ops(vector<int> v) {
	//rep(i, 10000)rep(j, 225)pre[i][j] = -1;
	int stac = t[v[0]][0] - 'A';
	for (int id : locs[stac]) {
		int c = abs(sx - id / n) + abs(sy - id % n) + 1;
		dp[0][id] = c;
	}
	int tmp = 0;
	char las = 'A' + stac;
	auto upd = [&](char c) {
		if (las == c) {
			for (int id : locs[c - 'A']) {
				dp[tmp + 1][id] = dp[tmp][id] + 1;
				pre[tmp + 1][id] = id;
			}
		}
		else {
			for (int id : locs[c - 'A']) {
				dp[tmp + 1][id] = mod17;
				for (int pid : locs[las - 'A']) {
					int val = dp[tmp][pid] + adc[id][pid];
					if (val < dp[tmp + 1][id]) {
						dp[tmp + 1][id] = val;
						pre[tmp + 1][id] = pid;
					}
				}
			}
		}
		las = c;
		tmp++;
	};
	for (int i = 1; i < t[v[0]].size(); i++) {
		upd(t[v[0]][i]);
	}
	rep(i, v.size() - 1) {
		string& ads = adt[v[i]][v[i + 1]];
		//cout << "?? " << ads.size() << "\n";
		for (char c : ads)upd(c);
	}
	int mi = mod17;
	int chk = -1;

	vector<P> res;
	for (int id : locs[las - 'A']) {
		if (dp[tmp][id] < mi) {
			mi = dp[tmp][id];
			chk = id;
		}
	}
	res.push_back({ chk / n,chk % n });
	per(i, tmp) {
		//assert(chk >= 0);
		chk = pre[i + 1][chk];
		res.push_back({ chk / n,chk % n });
	}
	reverse(all(res));
	//cerr << "res " << mi << "\n";
	return res;
}
vector<P> get_locs(vector<int> v) {
	vector<P> vp = get_ops(v);
	vector<P> res(v.size());
	int tmp = 0;
	rep(i, v.size()) {
		int le = tmp;
		if (i > 0)le -= b[v[i - 1]][v[i]];
		P sum = { 0,0 };
		Rep(j, le, le + t[v[i]].size()) {
			sum.first += vp[j].first;
			sum.second += vp[j].second;
		}
		res[i] = sum;
		tmp = le + t[v[i]].size();
	}
	return res;
}

vector<int> modify(vector<int> v) {
	auto vp = get_locs(v);
	vector<P> vx, vy;
	rep(i, vp.size()) {
		vx.push_back({ vp[i].first,i });
		vy.push_back({ vp[i].second,i });
	}
	sort(all(vx));
	sort(all(vy));
	P mx = vx[v.size() / 2];
	P my = vy[v.size() / 2];
	vector<int> vv[4];
	rep(i, v.size()) {
		int id = 0;
		if (vp[i] >= mx)id |= 1;
		if (vp[i] >= my)id |= 2;
		vv[id].push_back(v[i]);
	}
	vector<int> ord(4); rep(i, 4)ord[i] = i;
	vector<int> res;
	int mi = mod17;
	do {
		vector<int> cv;
		rep(i, 4) {
			for (int id : vv[ord[i]])cv.push_back(id);
		}
		int val = calc_score(cv, sx, sy);
		if (val < mi) {
			mi = val;
			res = cv;
		}
	} while (next_permutation(all(ord)));
	return res;
}
vector<int> mkinitans(vector<int> oriv = {}) {
	if (oriv.empty())rep(i, m)oriv.push_back(i);
	vector<vector<int>> vs(m);
	rep(i, m)vs[i] = { oriv[i] };
	rep(_, m - 1) {
		int mi = mod17;
		int ci, cj;
		rep(i, vs.size())rep(j, vs.size())if (i != j) {
			vector<int> c = vs[i]; for (int id : vs[j])c.push_back(id);
			int cost = calc_score(c, n, n) - calc_score(vs[i], n, n) - calc_score(vs[j], n, n);
			if (cost < mi) {
				mi = cost;
				ci = i, cj = j;
			}
		}
		for (int id : vs[cj])vs[ci].push_back(id);
		vs.erase(vs.begin() + cj);
		//cout << vs.size() << "\n";
	}
	return vs[0];
}


vector<int> SA(vector<int> v, double pre_time) {
	uniform_real_distribution<> udr(0.0, 1.0);
	double start_time = clock();
	int score = calc_score(v, sx, sy);
	uniform_int_distribution<> udn(0, v.size() - 1);
	double start_temp = 0, end_temp = 0;

	double TIME_LIMIT = 1.9 * CLOCKS_PER_SEC - (start_time - pre_time);
	while (true) {
		double now_time = clock();
		if (now_time - start_time > TIME_LIMIT)break;

		int x = udn(mt);
		int y = udn(mt);
		while (x == y)y = udn(mt);
		int z = udn(mt);
		while (z == x || z == y)z = udn(mt);
		vector<int> sv = { x,y,z }; sort(all(sv));
		x = sv[0], y = sv[1], z = sv[2];
		//swap [x,y],[y+1,z]
		vector<int> nv;
		rep(i, x)nv.push_back(v[i]);
		Rep1(i, y + 1, z)nv.push_back(v[i]);
		Rep1(i, x, y)nv.push_back(v[i]);
		Rep(i, z + 1, v.size())nv.push_back(v[i]);

		int new_score = calc_score(nv, sx, sy);
		double temp = start_temp + (end_temp - start_temp) * (now_time - start_time) / TIME_LIMIT;
		double prob = exp((score - new_score) / temp);
		if (score>=new_score) {
			v = nv;
			score = new_score;
		}
	}
	return v;
}

void solve() {
	auto start_time = clock();
	init();
	int mi = mod17;
	vector<int> ansv(m); rep(i, m)ansv[i] = i;
	rep(_, 0) {
		ansv = modify(ansv);
	}
	ansv = mkinitans(ansv);
	rep(_, 0) {
		ansv = modify(ansv);
	}
	ansv = mkinitans(ansv);

	//return;
	ansv = SA(ansv, start_time);
	int val = calc_score(ansv, sx, sy);
	auto vp = get_ops(ansv);
	//cerr << val << "\n";
	//cerr << vp.size() << "\n";
	int sum = 0;
	sum += adc[sx * n + sy][vp[0].first * n + vp[0].second];
	rep(i, vp.size() - 1) {
		sum += adc[vp[i].first * n + vp[i].second][vp[i + 1].first * n + vp[i + 1].second];
	}
	//cerr << sum << "\n";
	for (auto p : vp)cout << p.first << " " << p.second << "\n";
	int res_score = 10000 - val;
	cerr << res_score << "\n";
}


signed main() {
	ios::sync_with_stdio(false);
	cin.tie(0);
	//cout << fixed<<setprecision(10);
	//init_f();
	init_hash();
	//while(true)
	//expr();
	//int t; cin >> t; rep(i, t)
	solve();
	return 0;
}