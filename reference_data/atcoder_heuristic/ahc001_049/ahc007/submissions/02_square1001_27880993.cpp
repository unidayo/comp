#ifndef CLASS_DISJOINT_SET
#define CLASS_DISJOINT_SET

#include <vector>
#include <cstdint>
#include <cstring>
#include <algorithm>

class disjoint_set {
private:
	typedef std::int32_t value_type;
	std::vector<value_type> val;
public:
	explicit disjoint_set() : val() {};
	explicit disjoint_set(std::size_t n) : val(n, -1) {};
	std::size_t size() const { return val.size(); }
	std::size_t size(std::size_t elem) { return std::size_t(-val[root(elem)]); }
	std::size_t root(std::size_t elem) {
		// path halving
		while (val[elem] >= 0 && val[val[elem]] >= 0) {
			val[elem] = val[val[elem]];
			elem = val[elem];
		}
		return std::size_t(val[elem] >= 0 ? val[elem] : elem);
	}
	void link(std::size_t elemx, std::size_t elemy) {
		elemx = root(elemx);
		elemy = root(elemy);
		if (elemx == elemy) return;
		if (val[elemx] > val[elemy]) {
			std::swap(elemx, elemy);
		}
		val[elemx] += val[elemy];
		val[elemy] = elemx;
	}
	bool connected(std::size_t elemx, std::size_t elemy) {
		return root(elemx) == root(elemy);
	}
};

#endif // CLASS_DISJOINT_SET

#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

uint64_t rand_seed = 123456789;
uint64_t xorshift64() {
	rand_seed ^= rand_seed << 13;
	rand_seed ^= rand_seed >> 7;
	rand_seed ^= rand_seed << 17;
	return rand_seed;
}
int rand_int(int l, int r) {
	return l + int(xorshift64() % (r - l));
}
double randouble() {
	return double(xorshift64()) / double(uint64_t(-1));
}

class tester {
public:
	static const int REPS = 5;
	static const int N = 400;
	static const int M = (N - 1) * REPS;
	vector<int> X, Y;
	vector<int> U, V;
	vector<int> D, L;
	int cur_index;
	bool IS_DEBUG;
	tester() : X(vector<int>()), Y(vector<int>()), U(vector<int>()), V(vector<int>()), D(vector<int>()), L(vector<int>()), cur_index(0), IS_DEBUG(false) {};
	tester(const vector<int>& X_, const vector<int>& Y_, const vector<int>& U_, const vector<int>& V_, bool IS_DEBUG_) : X(X_), Y(Y_), U(U_), V(V_), cur_index(0), IS_DEBUG(IS_DEBUG_) {
		D = vector<int>(M);
		for (int i = 0; i < M; ++i) {
			D[i] = int(hypot(X[U[i]] - X[V[i]], Y[U[i]] - Y[V[i]]) + 0.5);
		}
		L = vector<int>(M, -1);
		if (IS_DEBUG) {
			for (int i = 0; i < M; ++i) {
				L[i] = rand_int(D[i], 3 * D[i] + 1);
			}
		}
	};
	static tester random_generate() {
		vector<int> X(N), Y(N), U, V;
		for (int i = 0; i < N; ++i) {
			while (true) {
				X[i] = rand_int(0, 800 + 1);
				Y[i] = rand_int(0, 800 + 1);
				bool flag = true;
				for (int j = 0; j < i; ++j) {
					if ((X[i] - X[j]) * (X[i] - X[j]) + (Y[i] - Y[j]) * (Y[i] - Y[j]) <= 25) {
						flag = false;
						break;
					}
				}
				if (flag) {
					break;
				}
			}
		}
		vector<vector<int> > all_d(N, vector<int>(N));
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				all_d[i][j] = int(hypot(X[i] - X[j], Y[i] - Y[j]) + 0.5);
			}
		}
		vector<vector<int> > edges;
		for (int i = 0; i < N; ++i) {
			for (int j = i + 1; j < N; ++j) {
				edges.push_back(vector<int>({ all_d[i][j], i, j }));
			}
		}
		/*
		for (int i = 0; i < int(edges.size()); ++i) {
			int u = rand_int(0, i + 1);
			if (u != i) {
				swap(edges[u], edges[i]);
			}
		}
		*/
		sort(edges.begin(), edges.end());
		const int INF = 1012345678;
		for (int i = 0; i < REPS; ++i) {
			disjoint_set uf(N);
			for (int j = 0; j < int(edges.size()); ++j) {
				int va = edges[j][1], vb = edges[j][2];
				if (all_d[va][vb] != INF && !uf.connected(va, vb)) {
					uf.link(va, vb);
					U.push_back(va);
					V.push_back(vb);
					all_d[va][vb] = INF;
				}
			}
		}
		for (int j = 0; j < M; ++j) {
			int u = rand_int(0, j + 1);
			if (u != j) {
				swap(U[u], U[j]);
				swap(V[u], V[j]);
			}
		}
		return tester(X, Y, U, V, true); // IS_DEBUG = true
	}
	double optimal_MST() const {
		vector<int> perm(M);
		for (int i = 0; i < M; ++i) {
			perm[i] = i;
		}
		sort(perm.begin(), perm.end(), [&](int va, int vb) { return L[va] < L[vb]; });
		disjoint_set uf(N);
		int sumcost = 0;
		for (int i = 0; i < M; ++i) {
			if (!uf.connected(U[perm[i]], V[perm[i]])) {
				uf.link(U[perm[i]], V[perm[i]]);
				sumcost += L[perm[i]];
			}
		}
		return sumcost;
	}
	int get_next() {
		if (IS_DEBUG) {
			return L[cur_index++];
		}
		cin >> L[cur_index];
		return L[cur_index++];
	}
};

const vector<double> tdist_table = {
	0, 0, 31830.98861, 223.60344, 47.92773, 23.33218, 15.54685, 12.03165, 10.10268, 8.90703,
	8.10206, 7.52695, 7.09736, 6.76517, 6.50114, 6.28655, 6.10887, 5.95944, 5.83210, 5.72233,
	5.62677, 5.54284, 5.46856, 5.40237, 5.34303, 5.28952, 5.24104, 5.19692, 5.15658, 5.11958,
	5.08550, 5.05403, 5.02487, 4.99779, 4.97255, 4.94899, 4.92695, 4.90627, 4.88685, 4.86856,
	4.85131, 4.83502, 4.81961, 4.80501, 4.79115, 4.77799, 4.76547, 4.75354, 4.74216, 4.73131,
	4.72093, 4.71101, 4.70151, 4.69240, 4.68366, 4.67528, 4.66722, 4.65947, 4.65202, 4.64484,
	4.63792, 4.63125, 4.62482, 4.61860, 4.61260, 4.60679, 4.60118, 4.59575, 4.59049, 4.58539,
	4.58045, 4.57567, 4.57102, 4.56651, 4.56213, 4.55788, 4.55375, 4.54973, 4.54582, 4.54202,
	4.53832, 4.53472, 4.53121, 4.52779, 4.52445, 4.52120, 4.51804, 4.51494, 4.51193, 4.50898,
	4.50610, 4.50330, 4.50055, 4.49787, 4.49525, 4.49268, 4.49018, 4.48773, 4.48533, 4.48298,
	4.48068, 4.47843, 4.47622, 4.47406, 4.47195, 4.46987, 4.46784, 4.46585, 4.46389, 4.46198,
	4.46010, 4.45825, 4.45644, 4.45467, 4.45293, 4.45121, 4.44953, 4.44788, 4.44626, 4.44467,
	4.44310, 4.44156, 4.44005, 4.43857, 4.43710, 4.43567, 4.43425, 4.43286, 4.43150, 4.43015,
	4.42883, 4.42752, 4.42624, 4.42498, 4.42374, 4.42251, 4.42131, 4.42012, 4.41895, 4.41780,
	4.41667, 4.41555, 4.41445, 4.41337, 4.41230, 4.41124, 4.41020, 4.40918, 4.40817, 4.40717,
	4.40619, 4.40522, 4.40426, 4.40332, 4.40239, 4.40147, 4.40057, 4.39967, 4.39879, 4.39792,
	4.39706, 4.39621, 4.39538, 4.39455, 4.39373, 4.39293, 4.39213, 4.39135, 4.39057, 4.38980,
	4.38904, 4.38830, 4.38756, 4.38682, 4.38610, 4.38539, 4.38468, 4.38399, 4.38330, 4.38262,
	4.38194, 4.38128, 4.38062, 4.37997, 4.37932, 4.37869, 4.37806, 4.37744, 4.37682, 4.37621,
	4.37561, 4.37501, 4.37442, 4.37384, 4.37326, 4.37269, 4.37213, 4.37157, 4.37102, 4.37047,
	4.36993, 4.36939, 4.36886, 4.36833, 4.36781, 4.36730, 4.36679, 4.36628, 4.36578, 4.36529,
	4.36480, 4.36431, 4.36383, 4.36336, 4.36288, 4.36242, 4.36195, 4.36150, 4.36104, 4.36059,
	4.36015, 4.35971, 4.35927, 4.35883, 4.35841, 4.35798, 4.35756, 4.35714, 4.35673, 4.35632,
	4.35591, 4.35551, 4.35511, 4.35471, 4.35432, 4.35393, 4.35354, 4.35316, 4.35278, 4.35241,
	4.35203, 4.35166, 4.35130, 4.35093, 4.35057, 4.35022, 4.34986, 4.34951, 4.34916, 4.34882,
	4.34847, 4.34813, 4.34780, 4.34746, 4.34713, 4.34680, 4.34647, 4.34615, 4.34583, 4.34551,
	4.34519, 4.34488, 4.34457, 4.34426, 4.34395, 4.34365, 4.34335, 4.34305, 4.34275, 4.34245,
	4.34216, 4.34187, 4.34158, 4.34130, 4.34101, 4.34073, 4.34045, 4.34017, 4.33989, 4.33962,
	4.33935, 4.33908, 4.33881, 4.33854, 4.33828, 4.33802, 4.33776, 4.33750, 4.33724, 4.33699,
	4.33673, 4.33648, 4.33623, 4.33599, 4.33574, 4.33550, 4.33525, 4.33501, 4.33477, 4.33453,
	4.33430, 4.33406, 4.33383, 4.33360, 4.33337, 4.33314, 4.33291, 4.33269, 4.33246, 4.33224,
	4.33202, 4.33180, 4.33158, 4.33137, 4.33115, 4.33094, 4.33072, 4.33051, 4.33030, 4.33010,
	4.32989, 4.32968, 4.32948, 4.32927, 4.32907, 4.32887, 4.32867, 4.32847, 4.32828, 4.32808,
	4.32789, 4.32769, 4.32750, 4.32731, 4.32712, 4.32693, 4.32674, 4.32656, 4.32637, 4.32619,
	4.32601, 4.32582, 4.32564, 4.32546, 4.32528, 4.32511, 4.32493, 4.32475, 4.32458, 4.32441,
	4.32423, 4.32406, 4.32389, 4.32372, 4.32355, 4.32338, 4.32322, 4.32305, 4.32289, 4.32272,
	4.32256, 4.32240, 4.32224, 4.32208, 4.32192, 4.32176, 4.32160, 4.32144, 4.32129, 4.32113,
	4.32098, 4.32083, 4.32067, 4.32052, 4.32037, 4.32022, 4.32007, 4.31992, 4.31978, 4.31963,
	4.31948, 4.31934, 4.31919, 4.31905, 4.31891, 4.31876, 4.31862, 4.31848, 4.31834, 4.31820,
	4.31806, 4.31793, 4.31779, 4.31765, 4.31752, 4.31738, 4.31725, 4.31711, 4.31698, 4.31685,
	4.31672, 4.31659, 4.31646, 4.31633, 4.31620, 4.31607, 4.31594, 4.31581, 4.31569, 4.31556,
	4.31544, 4.31531, 4.31519, 4.31506, 4.31494, 4.31482, 4.31470, 4.31458, 4.31446, 4.31434,
	4.31422, 4.31410, 4.31398, 4.31386, 4.31375, 4.31363, 4.31351, 4.31340, 4.31328, 4.31317,
	4.31306, 4.31294, 4.31283, 4.31272, 4.31261, 4.31250, 4.31239, 4.31228, 4.31217, 4.31206,
	4.31195, 4.31184, 4.31173, 4.31163, 4.31152, 4.31141, 4.31131, 4.31120, 4.31110, 4.31099,
	4.31089, 4.31079, 4.31069, 4.31058, 4.31048, 4.31038, 4.31028, 4.31018, 4.31008, 4.30998,
	4.30988, 4.30978, 4.30968, 4.30958, 4.30949, 4.30939, 4.30929, 4.30920, 4.30910, 4.30901,
	4.30891, 4.30882, 4.30872, 4.30863, 4.30854, 4.30844, 4.30835, 4.30826, 4.30817, 4.30808,
	4.30798, 4.30789, 4.30780, 4.30771, 4.30762, 4.30754, 4.30745, 4.30736, 4.30727, 4.30718,
	4.30710, 4.30701, 4.30692, 4.30684, 4.30675, 4.30666, 4.30658, 4.30649, 4.30641, 4.30633
};

double solve(bool IS_DEBUG) {
	// step #1. input initial informations
	const int N = tester::N;
	const int M = tester::M;
	tester T;
	if (IS_DEBUG) {
		T = tester::random_generate();
	}
	else {
		vector<int> X(N), Y(N);
		for (int i = 0; i < N; ++i) {
			cin >> X[i] >> Y[i];
		}
		vector<int> U(M), V(M);
		for (int i = 0; i < M; ++i) {
			cin >> U[i] >> V[i];
		}
		T = tester(X, Y, U, V, false);
	}
	
	// step #2. main game
	const int INF = 1012345678;
	vector<int> L(M, -1);
	vector<bool> flag(M, false);
	disjoint_set curtree(N);
	int sumlength = 0;
	for (int i = 0; i < M; ++i) {
		L[i] = T.get_next();
		// check if "must-do"
		disjoint_set excepttree = curtree;
		for (int j = i + 1; j < M; ++j) {
			excepttree.link(T.U[j], T.V[j]);
		}
		bool answer = false;
		if (curtree.connected(T.U[i], T.V[i])) {
			answer = false;
		}
		else if (!excepttree.connected(T.U[i], T.V[i])) {
			answer = true;
		}
		else {
			// Monte-Carlo method
			const int samples = 1000;
			double advantage = 0.0;
			double advantage_error = 0.0;
			double advantage_sum = 0.0;
			double advantage_sum2 = 0.0;
			for (int j = 1; j <= samples; ++j) {
				vector<int> extl(M - i - 1);
				for (int k = 0; k < M - i - 1; ++k) {
					extl[k] = rand_int(T.D[k + (i + 1)], 3 * T.D[k + (i + 1)] + 1);
				}
				int maxl = *max_element(extl.begin(), extl.end());
				vector<int> cntl(maxl + 3);
				for (int k = 0; k < M - i - 1; ++k) {
					++cntl[extl[k] + 2];
				}
				for (int k = 3; k <= maxl + 2; ++k) {
					cntl[k] += cntl[k - 1];
				}
				vector<int> perm(M - i - 1);
				for (int k = 0; k < M - i - 1; ++k) {
					perm[cntl[extl[k] + 1]++] = k + (i + 1);
				}
				disjoint_set ntree = curtree;
				int linklevel = INF;
				for (int k = 0; k <= maxl; ++k) {
					for (int l = cntl[k]; l < cntl[k + 1]; ++l) {
						ntree.link(T.U[perm[l]], T.V[perm[l]]);
					}
					if (ntree.connected(T.U[i], T.V[i])) {
						linklevel = k;
						break;
					}
				}
				double delta = linklevel - L[i] + 3.0 - 1.0 * double(i) / M;
				advantage_sum += delta;
				advantage_sum2 += delta * delta;
				advantage = advantage_sum / j;
				advantage_error = sqrt((advantage_sum2 / j - advantage * advantage) / (j - 1));
				if (j >= 3) {
					double tvalue = (j < 500 ? tdist_table[j] : 4.30624);
					if (advantage - tvalue * advantage_error > 0.0 || advantage + tvalue * advantage_error < 0.0) {
						break;
					}
				}
			}
			// cerr << i << ' ' << advantage << ' ' << advantage_error << endl;
			if (advantage >= 0.0) {
				answer = true;
			}
			else {
				answer = false;
			}
		}
		if (answer == true) {
			flag[i] = true;
			curtree.link(T.U[i], T.V[i]);
			sumlength += L[i];
			cout << 1 << endl;
		}
		else {
			cout << 0 << endl;
		}
	}
	int optimal_mst = T.optimal_MST();
	cerr << "Created MST = " << sumlength << endl;
	cerr << "Optimal MST = " << optimal_mst << endl;
	return double(optimal_mst) / sumlength;
}

int main() {
	solve(false);
	/*
	double sum = 0.0, sum2 = 0.0;
	for (int i = 1; i <= 100; ++i) {
		double score = solve(true);
		sum += score;
		sum2 += score * score;
		double stdev = sqrt(sum2 / i - sum * sum / (i * i));
		cerr << "Average: " << sum / i << " ± " << stdev / sqrt(i) << endl;
	}
	*/
	return 0;
}