#include "iostream"
#include "climits"
#include "list"
#include "queue"
#include "stack"
#include "set"
#include "functional"
#include "algorithm"
#include "string"
#include "map"
#include "unordered_map"
#include "unordered_set"
#include "iomanip"
#include "cmath"
#include "random"
#include "bitset"
#include "cstdio"
#include "numeric"
#include "cassert"
#include "ctime"

using namespace std;

/*
* めちゃでかいやつM個と残りN-M個の小さめなやつで調整
* 最初出力するものは毎回同じで良い
* ベースはLで固定するよりもちょっとずつ増やしていくのがよさそうではある
*	期待値より少し小さいのがいいんだろうけどどうやって計算するんだろう
* 1山あたり平均で9個は小さいやつを使える
*	42個(log_2{4e12})使えるなら完全に一致できそう
*	全部の合計は期待値よりも大きくなるべきっぽい
*
*/

constexpr int N = 500, M = 50;
constexpr long long L = 998000000000000, R = 1002000000000000;

vector<long long>a;

void Init() {
	long long dummy;
	cin >> dummy >> dummy >> dummy >> dummy;
}

int fst = 445;
int snd = 55;

class XorShift {
	unsigned int x, y, z, w, t;
public:
	XorShift() {
		x = 133553533;
		y = 314867339;
		z = 664298413;
		w = 999999937;
		t = 0;
	}
	unsigned int rand() {
		t = x ^ (x << 11);
		x = y;
		y = z;
		z = w;
		w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
		return w & 0x7fffffff;
	}
};

XorShift xs;

void Solve1() {
	long double v = 2.5e12;
	for (int i = 0; i < fst; i++) {
		a.push_back(round(v));
		v /= 1.03 + (double)(xs.rand() % 60) / 1000;
	}
	for (int i = 0; i < snd; i++) {
		a.push_back(L + (R - L) * max(i - 12, 0) / (M - 4));
	}
	sort(a.rbegin(), a.rend());
	for (auto i : a)cout << i << " ";
	cout << endl;
}



long long calculate_score(vector<long long>& b, vector<long long>& c) {
	long long ret = 0;
	for (int i = 1; i <= M; i++) {
		ret += abs(b[i] - c[i]);
	}
	return ret;
}

void Solve2() {
	vector<long long>b(M + 1);
	for (int i = 1; i <= M; i++) {
		cin >> b[i];
	}
	cerr << accumulate(a.begin(), a.end(), 0LL) << endl;
	cerr << accumulate(b.begin(), b.end(), 0LL) << endl;
	
	
	auto st = clock();
	int cnt = 0;
	long long best_score = 1e18;
	vector<int>best_ans(N);
	vector<long long>best_c(M + 1);

	int TL = 1900;

	while (1) {
		cnt++;
		if (cnt % 100 == 0) {
			auto ed = clock();
			int t = (ed - st) * 1000 / CLOCKS_PER_SEC;
			if (t > TL) {
				cerr << cnt << endl;
				break;
			}
		}
		vector<long long>c(M + 1);
		vector<int>ans(N);

		for (int i = 1; i <= M; i++) {
			vector<int>box;
			for (int j = 0; j < snd; j++) {
				if (ans[j])continue;
				if (a[j] > b[i])continue;
				box.push_back(j);
			}
			if (box.empty()) {
				for (int j = snd - 1; j >= 0; j--) {
					if (ans[j])continue;
					ans[j] = i;
					c[i] += a[j];
					break;
				}
			}
			else {
				int idx = box[xs.rand() % box.size()];
				ans[idx] = i;
				c[i] += a[idx];
			}
		}

		for (int i = snd; i < N; i++) {
			int idx = 0;
			long long mn = 1e16;
			for (int j = 1; j <= M; j++) {
				if (b[j] >= c[j] + a[i]) {
					if (mn > b[j] - c[j]) {
						mn = b[j] - c[j];
						idx = j;
					}
				}
			}
			if (idx > 0) {
				ans[i] = idx;
				c[idx] += a[i];
			}
		}

		for (int i = N - 1; i >= snd; i--) {
			if (ans[i])continue;
			long long mn = 1e16;
			int idx = 0;
			for (int j = 1; j <= M; j++) {
				if (b[j] < c[j] + a[i] && b[j] - c[j]>c[j] + a[i] - b[j]) {
					if (mn > c[j] + a[i] - b[j]) {
						mn = c[j] + a[i] - b[j];
						idx = j;
					}
				}
			}
			if (idx > 0) {
				ans[i] = idx;
				c[idx] += a[i];
			}
		}
		auto current_score = calculate_score(b, c);
		if (best_score > current_score) {
			best_score = current_score;
			best_c = c;
			best_ans = ans;
		}
		
	}

	for (auto i : best_ans)cout << i << " ";
	cout << endl;
	cerr << best_score << endl;
	cerr << (20 - log10(best_score + 1)) * 5e7 << endl;
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(0);

	Init();
	Solve1();
	Solve2();
}