#define DEBUG

#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <functional>
#include <cmath>
#include <random>
#include <queue>
#include <numeric>
#include <map>
using namespace std;
#define TEMPERATURE_MAX (1000)
#define TEMPERATURE_MIN (0)
#define QUERY_COUNT_MAX (10000)

random_device seed_gen;
mt19937 engine(seed_gen());
uniform_real_distribution<> uniform_distribution(0.0, 1.0);
double getRandomValue()
{
	return uniform_distribution(engine);
}

void outputCongiguration(int L, vector<vector<int>>& P) {
	for (int y = 0;y < L;y++) {
		for (int x = 0;x < L;x++) {
			assert(0 <= P[y][x] && P[y][x] <= 1000);
			std::cout << P[y][x] << ' ';
		}
		std::cout << endl;
	}
}
void outputPredictions(int N, vector<int>& predictions) {
	std::cout << -1 << ' ' << -1 << ' ' << -1 << endl;
	for (int i = 0;i < N;i++) {
		std::cout << predictions[i] << endl;
	}
}
int queryMeasurement(int id, int dy, int dx) {
	static int totalCount = 0;
	if (totalCount >= QUERY_COUNT_MAX) {
		return -1;
	}
	totalCount++;
	std::cout << id << ' ' << dy << ' ' << dx << endl;
	int m;
	cin >> m;
	if (m == -1) {
		exit(0);
	}
	return m;
}

/***********************************************************************/

vector<vector<int>> determineConfiguration(int L, int N, int S, vector<int>& Y, vector<int>& X, vector<int>& cellOrder) {
	vector<vector<int>> haveExitCell(L, vector<int>(L, 0));
	for (int i = 0;i < N;i++) {
		haveExitCell[Y[i]][X[i]] = 1;
	}
	vector<int> aroundExitCnt(N, 0);
	for (int i = 0;i < N;i++) {
		for (int dy = -2;dy <= 2;dy++) {
			for (int dx = -2;dx <= 2;dx++) {
				int y = (Y[i] + L + dy) % L;
				int x = (X[i] + L + dx) % L;
				aroundExitCnt[i] += haveExitCell[y][x];
			}
		}
	}
	int target = 0;
	for (int i = 1;i < N;i++) {
		if (aroundExitCnt[i] > aroundExitCnt[target]) {
			target = i;
		}
	}
	int y_center = Y[target];
	int x_center = X[target];

	vector<vector<int>> P(L, vector<int>(L));
	if (S == 1) {
		for (int y = 0;y < L;y++) {
			for (int x = 0;x < L;x++) {
				int x_dist = min(abs(x_center - x), L - abs(x_center - x));
				int y_dist = min(abs(y_center - y), L - abs(y_center - y));
				double dist = sqrt(x_dist * x_dist + y_dist * y_dist);
				// int dist = x_dist + y_dist;
				P[y][x] = max<int>(0, TEMPERATURE_MAX - 36.0 * dist / sqrt(L));
			}
		}
	}
	else if (S == 4) {
		for (int y = 0;y < L;y++) {
			for (int x = 0;x < L;x++) {
				int x_dist = min(abs(x_center - x), L - abs(x_center - x));
				int y_dist = min(abs(y_center - y), L - abs(y_center - y));
				// int dist = x_dist + y_dist;
				double dist = sqrt(x_dist * x_dist + y_dist * y_dist);
				P[y][x] = max<int>(0, TEMPERATURE_MAX - 50.0 * dist / sqrt(L));
			}
		}
	}
	else if (S == 9) {
		for (int y = 0;y < L;y++) {
			for (int x = 0;x < L;x++) {
				int x_dist = min(abs(x_center - x), L - abs(x_center - x));
				int y_dist = min(abs(y_center - y), L - abs(y_center - y));
				// int dist = x_dist + y_dist;
				double dist = sqrt(x_dist * x_dist + y_dist * y_dist);
				P[y][x] = max<int>(0, TEMPERATURE_MAX - 200.0 * dist / pow(L, 0.75));
			}
		}
	}
	else if (N * S <= 70000) {
		double f = 6.0 * S / sqrt(L);
		if (S <= 225) {
			f = 8.0 * S / sqrt(L);
		}
		double Gamma = 1.0;
		if (S <= 16) {
			Gamma = 0.9;
		}
		else if (S <= 25) {
			Gamma = 0.75;
		}
		else if (S <= 36) {
			Gamma = 0.7;
		}
		else if (S <= 49) {
			Gamma = 0.65;
		}
		else if (S <= 64) {
			Gamma = 0.55;
		}
		else if (S <= 81) {
			Gamma = 0.53;
		}
		else if (S <= 100) {
			Gamma = 0.5;
		}
		else if (S <= 121) {
			Gamma = 0.45;
		}
		else if (S <= 144) {
			Gamma = 0.45;
		}
		else if (S <= 169) {
			Gamma = 0.4;
		}
		else if (S <= 196) {

			Gamma = 0.375;
		}
		else if (S <= 225) {
			Gamma = 0.35;
		}
		else if (S <= 256) {
			Gamma = 0.4;
		}
		else if (S <= 289) {
			if (L <= 30) {
				Gamma = 0.35;
			}
			else {
				Gamma = 0.5;
			}
		}
		else if (S <= 324) {
			Gamma = 0.4;
		}
		else if (S <= 361) {
			Gamma = 0.35;
		}
		else if (S <= 441) {
			Gamma = 0.3;
		}
		else {
			Gamma = 1.0 / 4.0;
		}
		for (int y = 0;y < L;y++) {
			for (int x = 0;x < L;x++) {
				int x_dist1 = min(abs(x_center - x), L - abs(x_center - x));
				int y_dist1 = min(abs(y_center - y), L - abs(y_center - y));
				// int dist = x_dist + y_dist;
				double dist = sqrt(x_dist1 * x_dist1 + y_dist1 * y_dist1);
				P[y][x] = max(TEMPERATURE_MIN, TEMPERATURE_MAX - (int)(pow(dist, Gamma) * f));
			}
		}
	}
	else {
		int squareSize = min(L / 4, 8);
		int cnt = 0;
		int y_start = -1;
		int x_start = -1;
		for (int y = 0;y < L;y++) {
			for (int x = 0;x < L;x++) {
				int cnt_tmp = 0;
				for (int dy = 0;dy < squareSize;dy++) {
					for (int dx = 0;dx < squareSize;dx++) {
						cnt_tmp += haveExitCell[(y + dy) % L][(x + dx) % L];
					}
				}
				if (cnt_tmp > cnt) {
					cnt = cnt_tmp;
					y_start = y;
					x_start = x;
				}
			}
		}
		for (int y = 0;y < L;y++) {
			for (int x = 0;x < L;x++) {
				P[y][x] = TEMPERATURE_MIN;
			}
		}
		for (int dy = 0;dy < squareSize;dy++) {
			for (int dx = 0;dx < squareSize;dx++) {
				P[(y_start + dy) % L][(x_start + dx) % L] = TEMPERATURE_MAX * min(squareSize, 2 * squareSize - 1 - dy - dx) / squareSize;
			}
		}
		y_center = (y_start + squareSize / 2) % L;
		x_center = (x_start + squareSize / 2) % L;
	}
	priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
	for (int cellId = 0;cellId < N;cellId++) {
		int x_dist = min(abs(x_center - X[cellId]), L - abs(x_center - X[cellId]));
		int y_dist = min(abs(y_center - Y[cellId]), L - abs(y_center - Y[cellId]));
		int dist = x_dist + y_dist;
		pq.push(make_pair(dist, cellId));
	}
	for (int i = 0;i < N;i++) {
		cellOrder[i] = pq.top().second;
		pq.pop();
	}
#ifdef DEBUG
	for (int y = 0;y < L;y++) {
		for (int x = 0;x < L;x++) {
			assert(TEMPERATURE_MIN <= P[y][x] && P[y][x] <= TEMPERATURE_MAX);
		}
	}
#endif
	return P;
}

/****************************************************************************/


function<double(double)> cumulativeProbability = [](double x) {
	// ロジスティック関数で近似
	return 1.0 / (1.0 + exp(-1.7 * x));
};

// 座標(y, x)を観測した場合に気温temperatureを観測する確率
double log10ProbabilityToObserveAt(int y, int x, int temperature, int L, int N, int S, vector<int>& Y, vector<int>& X, vector<vector<int>>& P) {
	// メモ化
	static vector<vector<double>> memo(TEMPERATURE_MAX + 1, vector<double>(TEMPERATURE_MAX + 1, -1));
	if (memo[temperature][P[y][x]] != -1) {
		return memo[temperature][P[y][x]];
	}
	// 正規化
	double diff_upper = (temperature + 0.5 - P[y][x]) / S;
	double diff_lower = (temperature - 0.5 - P[y][x]) / S;

	if (temperature == TEMPERATURE_MIN) {
		return memo[temperature][P[y][x]] = log10(cumulativeProbability(diff_upper));
	}
	if (temperature == TEMPERATURE_MAX) {
		return memo[temperature][P[y][x]] = log10(1.0 - cumulativeProbability(diff_lower));
	}
	return memo[temperature][P[y][x]] = log10(cumulativeProbability(diff_upper) - cumulativeProbability(diff_lower));
}

tuple<int, int, int> determineNextObservationPoint(int id, int L, int N, int S, vector<int>& Y, vector<int>& X, vector<vector<int>>& P, vector<double>& log10Prob, vector<int>& used, vector<int>& cellOrder) {
	int k1 = -1;
	int k2 = -1;
	int k3 = -1;
	vector<int> nearMaximumCellIds;
	for (int i = 0;i < N;i++) {
		int cellId = cellOrder[i];
		if (used[cellId] == 1) {
			continue;
		}
		if (log10Prob[cellId] >= -0.5) {
			nearMaximumCellIds.push_back(cellId);
		}
		if (k1 == -1) {
			k1 = cellId;
		}
		else if (k2 == -1) {
			k2 = cellId;
			if (log10Prob[k1] < log10Prob[k2]) {
				swap(k1, k2);
			}
		}
		else if (k3 == -1) {
			k3 = cellId;
			if (log10Prob[k2] < log10Prob[k3]) {
				swap(k2, k3);
				if (log10Prob[k1] < log10Prob[k2]) {
					swap(k1, k2);
				}
			}
		}
		else if (log10Prob[cellId] > log10Prob[k1]) {
			k3 = k2;
			k2 = k1;
			k1 = cellId;
		}
		else if (log10Prob[cellId] > log10Prob[k2]) {
			k3 = k2;
			k2 = cellId;
		}
		else if (log10Prob[cellId] > log10Prob[k3]) {
			k3 = cellId;
		}
	}

	if (k2 == -1) {
		return { 0,0,k1 };
	}

	int ret_dy = 0;
	int ret_dx = 0;
	double r = 0.0;
	if (k3 != -1) {
		r = pow(10.0, log10Prob[k3] - log10Prob[k2]);
	}
	double ret_value = -1.0e100;
	int searchRange = L / 2;

	for (int dy = -searchRange;dy <= searchRange;dy++) {
		for (int dx = -searchRange;dx <= searchRange;dx++) {
			/*
			double value = calcEntropy(k1, k2, k3, dy, dx, L, N, S, Y, X, P, log10Prob);
			if (ret_value < value) {
				ret_value = value;
				ret_dy = dy;
				ret_dx = dx;
			}
			*/
			int y1 = (Y[k1] + L + dy) % L;
			int x1 = (X[k1] + L + dx) % L;
			int y2 = (Y[k2] + L + dy) % L;
			int x2 = (X[k2] + L + dx) % L;
			double value = 0.0;
			value += (exp(abs(P[y1][x1] - P[y2][x2]) / (double)S / 2.0) - 1) / (double)(10 + abs(dy) + abs(dx));
			if (k3 != -1) {
				int y3 = (Y[k3] + L + dy) % L;
				int x3 = (X[k3] + L + dx) % L;
				value += r * (exp(abs(P[y1][x1] - P[y3][x3]) / (double)S / 2.0) - 1) / (double)(10 + abs(dy) + abs(dx));
			}
			if (abs(P[y1][x1] - P[y2][x2]) >= 7 * S) {
				if (abs(dy) + abs(dx) < abs(ret_dy) + abs(ret_dx)) {
					ret_dy = dy;
					ret_dx = dx;
				}
				else if (ret_value < value) {
					ret_value = value;
					ret_dy = dy;
					ret_dx = dx;
				}
			}
			else {
				int y1 = (Y[k1] + L + ret_dy) % L;
				int x1 = (X[k1] + L + ret_dx) % L;
				int y2 = (Y[k2] + L + ret_dy) % L;
				int x2 = (X[k2] + L + ret_dx) % L;
				if (abs(P[y1][x1] - P[y2][x2]) >= 7 * S) {
					// do nothing
				}
				else if (ret_value < value) {
					ret_value = value;
					ret_dy = dy;
					ret_dx = dx;
				}
			}
		}
	}
#ifdef DEBUG
	std::cout << "# most probable (" << Y[k1] << ", " << X[k1] << ") " << log10Prob[k1] << endl;
	std::cout << "# next probable (" << Y[k2] << ", " << X[k2] << ") " << log10Prob[k2] << endl;
	if (k3 != -1) {
		std::cout << "# next probable (" << Y[k3] << ", " << X[k3] << ") " << log10Prob[k3] << endl;
	}
#endif
	return { ret_dy,ret_dx,k1 };
}


int investigateWormhole(int id, int L, int N, int S, vector<int>& Y, vector<int>& X, vector<vector<int>>& P, vector<int>& used, vector<double>& log10Prob, vector<int>& cellOrder) {
	vector<pair<pair<int, int>, int>> history;
	int queryCount = 0;
	while (true) {
		double maxLogProb = -1.0e100;
		for (int cellId = 0;cellId < N;cellId++) {
			if (used[cellId] == 1) {
				continue;
			}
			maxLogProb = max<double>(maxLogProb, log10Prob[cellId]);
		}
		double sum = 0.0;
		for (int cellId = 0;cellId < N;cellId++) {
			if (used[cellId] == 1) {
				continue;
			}
			log10Prob[cellId] -= maxLogProb;
			sum += pow(10, log10Prob[cellId]);
		}
		bool finish = false;
		double threshold = 0.002;
		if (S >= 169) {
			threshold = 0.001;
		}
		if (S >= 676) {
			threshold = 0.0001;
		}

		for (int cellId = 0;cellId < N;cellId++) {
			if (used[cellId] == 1) {
				continue;
			}
			if (log10Prob[cellId] - log10(sum) >= -threshold) {
				finish = true;
			}
		}
		if (finish) {
			break;
		}

		if (S >= 625) {
			if (queryCount > 2 * 10000 / N) {
				return -1;
			}
		}
		else {
			if (queryCount > 10000 / N) {
				return -1;
			}
		}

		auto [dy, dx, cellId_tmp] = determineNextObservationPoint(id, L, N, S, Y, X, P, log10Prob, used, cellOrder);
		/*
		if (queryCount == 0 && P[Y[cellId_tmp]][X[cellId_tmp]] > S) {
			dy = 0;
			dx = 0;
		}
		*/

		int m = queryMeasurement(id, dy, dx);
		queryCount++;
		if (m == -1) {
			break;
		}
		history.push_back(make_pair(make_pair(dy, dx), m));
		for (int cellId = 0;cellId < N;cellId++) {
			if (used[cellId] == 1) {
				continue;
			}
			int y = (Y[cellId] + L + dy) % L;
			int x = (X[cellId] + L + dx) % L;
			double p = log10ProbabilityToObserveAt(y, x, m, L, N, S, Y, X, P);
			log10Prob[cellId] += p;
		}
	}

	int target = -1;
	for (int cellId = 0;cellId < N;cellId++) {
		if (used[cellId] == 1) {
			continue;
		}
		if (target == -1) {
			target = cellId;
		}
		else if (log10Prob[cellId] > log10Prob[target]) {
			target = cellId;
		}
	}

	for (auto p : history) {
		int dy = p.first.first;
		int dx = p.first.second;
		int m = p.second;
		int y = (Y[target] + L + dy) % L;
		int x = (X[target] + L + dx) % L;
	}
	return target;
}

vector<int> observeTemperatures(int L, int N, int S, vector<int> Y, vector<int> X, vector<vector<int>> P, vector<int> cellOrder) {
	vector<int> predictions(N, -1);
	vector<int> used(N, 0);
	vector<vector<double>> log10Probs(N, vector<double>(N, 0.0));
	int notDetermined = N;
	for (int i = 0;notDetermined > 0;i = (i + 1) % N) {
		if (predictions[i] != -1) {
			continue;
		}
		int target = investigateWormhole(i, L, N, S, Y, X, P, used, log10Probs[i], cellOrder);
		if (target != -1) {
			predictions[i] = target;
			used[target] = 1;
			notDetermined--;
		}
	}
	return predictions;
}

int main()
{
	int L, N, S;
	cin >> L >> N >> S;
	vector<int> Y(N), X(N);
	for (int i = 0;i < N;i++) {
		cin >> Y[i] >> X[i];
	}
	vector<int> cellOrder(N);
	vector<vector<int>> P = determineConfiguration(L, N, S, Y, X, cellOrder);
	outputCongiguration(L, P);
	vector<int> predictions = observeTemperatures(L, N, S, Y, X, P, cellOrder);
	outputPredictions(N, predictions);
}