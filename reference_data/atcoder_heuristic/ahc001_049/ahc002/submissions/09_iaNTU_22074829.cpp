// Exported by Exporter.exe

// Included from sol.cpp
#pragma GCC optimize("O3")
#include <bits/stdc++.h>
using namespace std;
#define PB push_back
#define F first
#define S second
#define MP make_pair
#define MTP make_tuple
#define R Read
#define RD Read_Digit
#define RP Read_P
#define RL Read_Loop
#define RLD Read_Loop_Digit
#define RLP Read_Loop_P
#ifdef ONLINE_JUDGE
	#define Debug(x) ;
	#define Debugln(x) ;
	#define Debug_Array(n,x) ;
	#define Debugln_Array(n,x) ;
	#define NL ;
#else
	#define Debug(x) printf("%s :", (#x)), _Debug(x)
	#define Debugln(x) printf("%s :", (#x)), _Debugln(x)
	#define Debug_Array(n,x) printf("%s :", (#x)), _Debug_Array((n), (x))
	#define Debugln_Array(n,x) printf("%s :", (#x)), _Debugln_Array((n), (x))
	#define NL printf("\n")
#endif
typedef long long int ll;
typedef unsigned long long int ull;

constexpr int kN = 50;
constexpr int kM = kN * kN;
// constexpr int kMod = 998244353;
// constexpr int kMod = int(1E9 + 7);
// constexpr int kInf = 0x3f3f3f3f;
// constexpr ll kInf = 0x3f3f3f3f3f3f3f3f;
// constexpr double kPi = acos(-1);
// constexpr double kEps = 1E-9;


// Included from C:\Users\ianli\Desktop\CP\template\Various\Fast_IO\Fast_IO.cpp
// --- Get ---
static inline char Get_Raw_Char() {
	static char buf[1 << 16], *p = buf, *end = buf;
	if (p == end) {
		if ((end = buf + fread(buf, 1, 1 << 16, stdin)) == buf) return '\0';
		p = buf;
	}
	return *p++;
}

// --- Read ---
template <typename T> static inline void Read_P(T &n) {
	static_assert(is_integral<T>::value);
	char c;
	while (!isdigit(c = Get_Raw_Char())) ;
	n = int(c - '0');
	while (isdigit(c = Get_Raw_Char())) n = n * 10 + int(c - '0');
	return ;
}

template <typename T> static inline void Read(T &n) {
	static_assert(is_integral<T>::value);
	char c;
	bool neg = false;
	while (!isdigit(c = Get_Raw_Char())) if (c == '-') neg = true;
	n = int(c - '0');
	while (isdigit(c = Get_Raw_Char())) n = n * 10 + int(c - '0');
	if (neg) n = -n;
	return ;
}

template <typename T> static inline void Read_Digit(T &n) {
	static_assert(is_integral<T>::value);
	char c;
	while (!isdigit(c = Get_Raw_Char())) ;
	n = int(c - '0');
	return ;
}

// --- Read multiple ---
template <typename T, typename... Targs> static inline void Read(T &n, Targs&... Fargs) {Read(n); return Read(Fargs...);}
template <typename T, typename... Targs> static inline void Read_Digit(T &n, Targs&... Fargs) {Read_Digit(n); return Read_Digit(Fargs...);}
template <typename T, typename... Targs> static inline void Read_P(T &n, Targs&... Fargs) {Read_P(n); return Read_P(Fargs...);}

// --- Read Loop ---
template <typename T> static inline void Read_Loop_i(int i, T *a) {return Read(a[i]);}
template <typename T, typename... Targs> static inline void Read_Loop_i(int i, T *a, Targs*... Fargs) {Read(a[i]); return Read_Loop_i(i, Fargs...);}
template <typename... Targs> static inline void Read_Loop(int n, Targs*... Fargs) {for (int i = 1; i <= n; i++) Read_Loop_i(i, Fargs...);}

template <typename T> static inline void Read_Loop_Digit_i(int i, T *a) {return Read_Digit(a[i]);}
template <typename T, typename... Targs> static inline void Read_Loop_Digit_i(int i, T *a, Targs*... Fargs) {Read_Digit(a[i]); return Read_Loop_Digit_i(i, Fargs...);}
template <typename... Targs> static inline void Read_Loop_Digit(int n, Targs*... Fargs) {for (int i = 1; i <= n; i++) Read_Loop_Digit_i(i, Fargs...);}

template <typename T> static inline void Read_Loop_P_i(int i, T *a) {return Read_P(a[i]);}
template <typename T, typename... Targs> static inline void Read_Loop_P_i(int i, T *a, Targs*... Fargs) {Read_P(a[i]); return Read_Loop_P_i(i, Fargs...);}
template <typename... Targs> static inline void Read_Loop_P(int n, Targs*... Fargs) {for (int i = 1; i <= n; i++) Read_Loop_P_i(i, Fargs...);}

// --- Float ---
template <int mul, typename T> static inline void Read(T &n) {
	char c;
	bool neg = false;
	while (!isdigit(c = Get_Raw_Char())) if (c == '-') neg = true;
	n = int(c - '0');
	while (isdigit(c = Get_Raw_Char())) n = n * 10 + int(c - '0');
	
	int cnt = 0;

	if (c == '.') {
		while (isdigit(c = Get_Raw_Char())) {
			n = n * 10 + int(c - '0');
			cnt++;
		}
	}

	while (cnt++ < mul) n = n * 10;

	if (neg) n = -n;
	return ;
}

template <int mul, typename T> static inline void Read_P(T &n) {
	char c;
	while (!isdigit(c = Get_Raw_Char())) ;
	
	n = int(c - '0');
	while (isdigit(c = Get_Raw_Char())) n = n * 10 + int(c - '0');
	
	int cnt = 0;

	if (c == '.') {
		while (isdigit(c = Get_Raw_Char())) {
			n = n * 10 + int(c - '0');
			cnt++;
		}
	}

	while (cnt++ < mul) n = n * 10;
	return ;
}

template <int mul, typename T, typename... Targs> static inline void Read(T &n, Targs&... Fargs) {Read<mul>(n); return Read<mul>(Fargs...);}
template <int mul, typename T, typename... Targs> static inline void Read_P(T &n, Targs&... Fargs) {Read_P<mul>(n); return Read_P<mul>(Fargs...);}

// --- init ---
inline void IOS() {ios::sync_with_stdio(false); cin.tie(0);}
inline void Freopen(const char *in, const char *out) {freopen(in, "r", stdin); freopen(out, "w", stdout);}

// --- Output ---
template <typename T> void Print(T x) {
	if (x < 0) {
		printf("-");
		x = -x;
	}
	if (x == 0) printf("0");
	else {
		static int val[100];
		int idx = -1;
		while (x) {
			val[++idx] = x % 10;
			x /= 10;
		}
		while (idx >= 0) printf("%d", val[idx--]);
	}
} 
// End of C:\Users\ianli\Desktop\CP\template\Various\Fast_IO\Fast_IO.cpp

// Included from C:\Users\ianli\Desktop\CP\template\Various\Useful_Functions\Useful_Functions.cpp
template <typename T> inline void sort(vector<T> &v) {return sort(v.begin(), v.end());}
template <typename T> inline void sort_r(vector<T> &v) {return sort(v.begin(), v.end(), greater<T>());}

template <typename T> inline void reverse(vector<T> &v) {return reverse(v.begin(), v.end());}

template <typename T> inline void Merge(vector<T> &a, vector<T> &b, vector<T> &c) {
	if (c.size() < a.size() + b.size()) c.resize(a.size() + b.size());
	merge(a.begin(), a.end(), b.begin(), b.end(), c.begin());
	return ;
}

template <typename T> inline void Discrete(vector<T> &v) {sort(v); v.resize(unique(v.begin(), v.end()) - v.begin());}

template <typename T> using PQ = priority_queue<T>;
template <typename T> using PQ_R = priority_queue<T, vector<T>, greater<T>>;

template <typename T> inline T ABS(T n) {return n >= 0 ? n : -n;}
template <typename T> __attribute__((target("bmi"))) inline T gcd(T a, T b) {
	if (a == 0 || b == 0) return a + b;
	int n = __builtin_ctzll(a);
	int m = __builtin_ctzll(b);
	a >>= n;
	b >>= m;
	while (a != b) {
		int m = __builtin_ctzll(a - b);
		bool f = a > b;
		T c = f ? a : b;
		b = f ? b : a;
		a = (c - b) >> m;
	}
	return a << min(n, m);
}
template <typename T> inline T lcm(T a, T b) {return a * b / gcd(a, b);}
template <typename T, typename... Targs> inline T gcd(T a, T b, T c, Targs... args) {return gcd(a, gcd(b, c, args...));}
template <typename T, typename... Targs> inline T lcm(T a, T b, T c, Targs... args) {return lcm(a, lcm(b, c, args...));}
template <typename T, typename... Targs> inline T min(T a, T b, T c, Targs... args) {return min(a, min(b, c, args...));}
template <typename T, typename... Targs> inline T max(T a, T b, T c, Targs... args) {return max(a, max(b, c, args...));}
template <typename T, typename... Targs> inline void chmin(T &a, T b, Targs... args) {a = min(a, b, args...); return ;}
template <typename T, typename... Targs> inline void chmax(T &a, T b, Targs... args) {a = max(a, b, args...); return ;}

template <typename T> inline int Digit_Sum(T a) {
	int ans = 0;
	while (a) {
		ans += a % 10;
		a /= 10;
	}
	return ans;
}
template <typename T> inline int Num_Length(T a) {
	if (a == 0) return 1;
	int ans = 0;
	while (a /= 10) ans++;
	return ans;
}
// End of C:\Users\ianli\Desktop\CP\template\Various\Useful_Functions\Useful_Functions.cpp

// Included from C:\Users\ianli\Desktop\CP\template\Various\Debug\Debug.cpp
void _print(int x) {printf("%d", x);}
void _print(long long int x) {printf("%lld", x);}
void _print(double x) {printf("%lf", x);}
template <typename T> void _print(T x) {return x.out();}
template <typename T1, typename T2> void _print(pair<T1, T2> x) {printf("("); _print(x.first); printf(", "); _print(x.second); printf(")");}

template <typename T> void _Debug(T x) {_print(x); printf("\n");}
template <typename T> void _Debug(vector<T> v) {
	if (v.empty()) printf(" empty");
	else for (T i : v) printf(" "), _print(i); 
	printf("\n");
}

template <typename T1, typename T2, typename T3> void _Debug(priority_queue<T1, T2, T3> pq) {
	if (pq.empty()) printf(" empty");
	else {
		while (!pq.empty()) {
			printf(" ");
			_print(pq.top());
			pq.pop();
		}
	}
	printf("\n");
}

template <typename T> void _Debug(queue<T> q) {
	if (q.empty()) printf(" empty");
	else {
		while (!q.empty()) {
			printf(" ");
			_print(q.front());
			q.pop();
		}
	}
	printf("\n");
}

template <typename T> void _Debug(stack<T> st) {
	if (st.empty()) printf(" empty");
	else {
		while (!st.empty()) {
			printf(" ");
			_print(st.top());
			st.pop();
		}
	}
	printf("\n");
}

template <typename T> void _Debug(deque<T> dq) {
	if (dq.empty()) printf(" empty");
	else {
		while (!dq.empty()) {
			printf(" ");
			_print(dq.front());
			dq.pop_front();
		}
	}
	printf("\n");
}

template <typename T> void _Debugln(vector<T> v) {
	if (v.empty()) printf(" empty\n");
	else {
		for (T i : v) printf("\n"), _print(i); 
		printf("\n");
	}
}

template <typename T1, typename T2, typename T3> void _Debugln(priority_queue<T1, T2, T3> pq) {
	if (pq.empty()) printf(" empty");
	else {
		while (!pq.empty()) {
			printf("\n");
			_print(pq.top());
			pq.pop();
		}
	}
	printf("\n");
}

template <typename T> void _Debugln(queue<T> q) {
	if (q.empty()) printf(" empty");
	else {
		while (!q.empty()) {
			printf("\n");
			_print(q.front());
			q.pop();
		}
	}
	printf("\n");
}

template <typename T> void _Debugln(stack<T> st) {
	if (st.empty()) printf(" empty");
	else {
		while (!st.empty()) {
			printf("\n");
			_print(st.top());
			st.pop();
		}
	}
	printf("\n");
}

template <typename T> void _Debugln(deque<T> dq) {
	if (dq.empty()) printf(" empty");
	else {
		while (!dq.empty()) {
			printf("\n");
			_print(dq.front());
			dq.pop_front();
		}
	}
	printf("\n");
}

template <typename T> void _Debug_Array(int n, const T *x) {for (int i = 1; i <= n; i++) printf(" "), _print(x[i]); printf("\n");}
template <typename T> void _Debugln_Array(int n, const T *x) {printf("\n"); for (int i = 1; i <= n; i++) _print(x[i]), printf("\n");}
// End of C:\Users\ianli\Desktop\CP\template\Various\Debug\Debug.cpp

// Included from C:\Users\ianli\Desktop\CP\template\Various\Random\Random.cpp
double rng_double(double l = 0, double r = 1) {
	random_device rd;
	mt19937 eng(rd());
	uniform_real_distribution<double> distr(l, r);
	return distr(eng);
}

template <int l, int r> int rng_int() {
	static random_device rd;
	static mt19937 eng(rd());
	static uniform_int_distribution<int> distr(l, r);
	return distr(eng);
}

int rng_int(int l, int r) {
	static random_device rd;
	static mt19937 eng(rd());
	return eng() % (r - l + 1) + l;
}

template <int l, int r> void rng_array(int n, int *a) {
	unordered_set<int> used;

	int cnt = 0;
	while (cnt < n) {
		int tmp = rng_int<l, r>();
		if (used.find(tmp) == used.end()) used.insert(a[++cnt] = tmp);
	}

	return ;
}

// End of C:\Users\ianli\Desktop\CP\template\Various\Random\Random.cpp

constexpr char dir[4] = {'L', 'R', 'U', 'D'};

int p[kN][kN], t[kN][kN], si, sj;
pair<int, int> ma[256];

inline bool Valid(int x, int y) {return 0 <= x && x <= 49 && 0 <= y && y <= 49;}

void Pre() {
	ma['L'] = MP(0, -1);
	ma['R'] = MP(0, 1);
	ma['U'] = MP(-1, 0);
	ma['D'] = MP(1, 0);
	return ;
}

int Cal(string s) {
	int x = si, y = sj;
	int ans = p[x][y];
	for (char c : s) {
		auto [dx, dy] = ma[c];
		x += dx, y += dy;
		ans += p[x][y];
	}
	return ans;
}

namespace N4 {
	string s[24];
	int Pre() {
		s[0] = "LRUD";
		for (int i = 1; i < 24; i++) {
			s[i] = s[i - 1];
			next_permutation(s[i].begin(), s[i].end());
		}
		return 1;
	}
}

string Nxt4() {
	static int pre = N4::Pre();
	return N4::s[rng_int<0, 23>()];
}

string random_valid(string path) {
	static bitset<kM> went_tile;
	went_tile.reset();
	
	int x = si, y = sj;
	went_tile[t[x][y]] = true;
	for (char c : path) {
		auto [dx, dy] = ma[c];
		x += dx, y += dy;
		went_tile[t[x][y]] = true;
	}

	bool walked = true;
	while (walked) {
		walked = false;
		for (char c : Nxt4()) {
			auto [dx, dy] = ma[c];
			x += dx, y += dy;
			if (Valid(x, y) && !went_tile[t[x][y]]) {
				went_tile[t[x][y]] = true;
				path += c;
				walked = true;
				break;
			}
			x -= dx, y -= dy;
		}
	}

	return path;
}

namespace Dfs {
	char from[kN][kN];
	bitset<kN> went_cell[kN];
	bitset<kM> went_tile;
	int tx, ty;
}
bool dfs(int x, int y) {
	using namespace Dfs;
	if (x == tx && y == ty) return true;
	went_cell[x][y] = true;
	for (char c : Nxt4()) {
		auto [dx, dy] = ma[c];
		x += dx, y += dy;
		if (Valid(x, y) && !went_cell[x][y] && !went_tile[t[x][y]]) {
			went_tile[t[x][y]] = true;
			if (dfs(x, y)) {
				from[x][y] = c;
				return true;
			}
			went_tile[t[x][y]] = false;
		}
		x -= dx, y -= dy;
	}
	return false;
}

string walk(int sx, int sy, int tx, int ty, bitset<kM> &went_tile) {
	Dfs::tx = tx, Dfs::ty = ty;
	Dfs::went_tile = went_tile;
	for (int i = 0; i < kN; i++) Dfs::went_cell[i].reset();

	if (dfs(sx, sy)) {
		string ans;
		int x = tx, y = ty;

		while (x != sx || y != sy) {
			auto [dx, dy] = ma[Dfs::from[x][y]];
			ans += Dfs::from[x][y];
			x -= dx, y -= dy;
		}

		reverse(ans.begin(), ans.end());
		return ans;
	}
	else return "X";
}

int main() {
	Pre();

	cin >> si >> sj;
	for (int i = 0; i < kN; i++) for (int j = 0; j < kN; j++) cin >> t[i][j];
	for (int i = 0; i < kN; i++) for (int j = 0; j < kN; j++) cin >> p[i][j];

	string ans;
	int score = 0;

	constexpr int kRandom_Valid_Time = 0.1 * CLOCKS_PER_SEC;
	while (clock() < kRandom_Valid_Time) {
		int len = int(ans.size());
		string tmp = random_valid(ans.substr(0, int(double(len) * clock() / kRandom_Valid_Time)));
		int tscore = Cal(tmp);
		if (tscore > score) {
			score = tscore;
			ans = tmp;
		}
	}

	constexpr int kTotal_Time = 1.98 * CLOCKS_PER_SEC;
	while (clock() < kTotal_Time) {
		int len = int(ans.size());
		int mlen = double(len) * (kTotal_Time - clock()) / (kTotal_Time - kRandom_Valid_Time) * 0.4;
		int from = rng_int(0, len - mlen);
		int to = min(len, from + mlen);

		bitset<kM> went_tile;
		int x = si, y = sj;
		string lhs, rhs;

		went_tile[t[x][y]] = true;
		for (int i = 0; i < from; i++) {
			auto [dx, dy] = ma[ans[i]];
			x += dx, y += dy;
			went_tile[t[x][y]] = true;
			lhs += ans[i];
		}

		int sx = x, sy = y;
		for (int i = from; i < to; i++) {
			auto [dx, dy] = ma[ans[i]];
			x += dx, y += dy;
		}

		int tx = x, ty = y;
		for (int i = to; i < len; i++) {
			auto [dx, dy] = ma[ans[i]];
			x += dx, y += dy;
			went_tile[t[x][y]] = true;
			rhs += ans[i];
		}

		string tmp = walk(sx, sy, tx, ty, went_tile);

		if (tmp != "X") {
			string tmp_ans = lhs + tmp + rhs;

			int tmp_score = Cal(tmp_ans);
			if (tmp_score > score) {
				score = tmp_score;
				ans = tmp_ans;
			}
		}
	}

	//bitset<kM> went_tile;
	//int x = si, y = sj;
	//went_tile[t[x][y]] = true;
	//for (char c : ans) {
	//	auto [dx, dy] = ma[c];
	//	x += dx, y += dy;
	//	assert(!went_tile[t[x][y]]);
	//	went_tile[t[x][y]] = true;
	//}
	cout << ans << "\n";
}
// End of sol.cpp
