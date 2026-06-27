#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx")


#include <bits/stdc++.h>
#define debug cerr << "-- " << __LINE__ << " --" << endl
#define ALL(o) (o).begin(), (o).end()

#define GRID_SIZE 30

typedef long long int ll;

using namespace std;
using namespace std::chrono;
system_clock::time_point t0;
int elapsedTimeCache;
int elapsedTime() {
  return duration_cast<milliseconds>(system_clock::now() - t0).count();
}
const int timeLimit = 1850;
bool shouldHurryUp() {
  elapsedTimeCache = elapsedTime();
  return elapsedTimeCache >= timeLimit;
}

template <typename T> string to_string(const vector<T> &xs) {string s="["; for(int i=0;i<xs.size();i++) s += (i>0 ? " " : "") + to_string(xs[i]); return s + "]";}
template <typename T> ll argmax(const T &xs) {return distance(xs.begin(), max_element(xs.begin(), xs.end()));}
template <typename T> ll argmin(const T &xs) {return distance(xs.begin(), min_element(xs.begin(), xs.end()));}
ll nextLL(){ll x; scanf("%lld", &x); return x;}
int nextInt(){int x; scanf("%d", &x); return x;}
string nextString(){string s; cin >> s; return s;}
float frand(){return (0.0 + rand()) / RAND_MAX;}

template<typename T> using Matrix = vector<vector<T>>;

inline float safeSqrt(const float x) {return sqrt(max(0.0f, x));}


const vector<float> lcbSchedule = {
	1.541,  1.273,  0.773,  0.55 ,  0.55 ,  0.319,  0.319,  0.067,  -0.08 , -0.08 , -0.08
	//1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0 /*LCB_SCHEDULE*/
};
float getLcbValue(int iter){
	// return 1.0;
	// return max(0.0, 1.0 - (0.0 + iter) / 500.0);
	// return max(-1.0, 1.0 - (0.0 + iter) / 250.0);
	// return 1.0 - (0.0 + iter) / 1000.0;
	// return 1.0 - (0.0 + iter) / 2000.0;
	// return (iter < 500) ? 1.0 : 0.0;
	// return min(1.0, 2.0 - (0.0 + iter) / 500.0);
	int idx = iter / 100;
	int rem = iter - idx * 100;
	return (lcbSchedule[idx] * (100 - rem) + lcbSchedule[idx + 1] * rem) / 100;
}
const int modelFixIter = 500;
// const float eta = 1.0; // NOTE: 1なら計算式から省く．
const float tendencyToSelectEst2 = log(
	1.0
);
const int adjustCovarianceInterval = 10000;
const float adjustCovarianceGapThreshold = 1.0;
const float adjustCovarianceCoef = 0.0;


struct Edge{
	int st;
	int ed;
	float cost;
	Edge(int st, int ed, float cost){
		this->st = st;
		this->ed = ed;
		this->cost = cost;
	}
};

struct Graph{
	int V; // 頂点数
	int E; // 辺数
	vector<Edge> edges; // 辺の一覧
	vector<vector<int>> adjList; // 隣接リスト．格納されるのは辺の ID

	Graph(const vector<int> &st, const vector<int> &ed, const vector<float> &cost){
		/*
			st, ed は 0-indexed．辺の向きは問わない．
			st, ed, cost は同じ長さである必要がある．
			頂点数 V は最大インデックスから推論される．
			cost は省略可能（移譲コンストラクタによる．省略された場合，全辺のコストは 1 となる）
		*/
		assert(st.size() == ed.size());
		assert(st.size() == cost.size());
		E = st.size();
		V = max(*max_element(st.begin(), st.end()), *max_element(ed.begin(), ed.end())) + 1;
		for(int i=0;i<E;i++) edges.emplace_back(Edge(st[i], ed[i], cost[i]));

		// adjList と costList を作成
		adjList = vector<vector<int>>(V);
		for(int i=0;i<E;i++){
			assert(0 <= st[i] && st[i] < V);
			assert(0 <= ed[i] && ed[i] < V);
			adjList[st[i]].emplace_back(i);
			adjList[ed[i]].emplace_back(i);
		}
	}
	// コストが省略された場合の移譲コンストラクタ
	Graph(const vector<int> &st, const vector<int> &ed): Graph(st, ed, vector<float>(st.size(), 1.0)){}

	void setCost(const vector<float> &cost){
		assert(cost.size() == E);
		for(int i=0;i<E;i++) edges[i].cost = cost[i];
	}

	tuple<vector<int>, vector<int>, float> findShortestPath(const int start, const int goal){
		/*
		Returns:
			(頂点IDの配列，辺IDの配列，最短距離)
		*/
		float INFTY = 1e9;
		vector<float> dist(V, INFTY);
		vector<int> fromMemo(V);
		priority_queue<tuple<float, int, int>> pq; pq.push(make_tuple(-0, start, -1));
		while(!pq.empty()){
			auto top = pq.top(); pq.pop();
			float newDist = -get<0>(top);
			int v = get<1>(top);
			if (dist[v] <= newDist) continue;
			dist[v] = newDist;
			fromMemo[v] = get<2>(top);
			if (v == goal) break;
			for(int eid : adjList[v]){
				Edge &e = edges[eid];
				int v2 = e.st ^ e.ed ^ v;
				if (dist[v2] < INFTY) continue;
				pq.push(make_tuple(-(newDist + e.cost), v2, eid));
			}
		}
		// 経路復元
		vector<int> vpath = {goal}, epath;
		int now = goal;
		while(now != start){
			int eid = fromMemo[now];
			Edge &e = edges[eid];
			int prev = e.st ^ e.ed ^ now;
			vpath.emplace_back(prev);
			epath.emplace_back(eid);
			now = prev;
		}
		reverse(vpath.begin(), vpath.end());
		reverse(epath.begin(), epath.end());
		return make_tuple(vpath, epath, dist[goal]);
	}

};


Graph MakeGridGraph(int n){
	vector<int> st, ed;
	// horizontal
	for(int i=0;i<n;i++){
		for(int j=0;j<n-1;j++){
			int v = i * n + j;
			int v2 = i * n + (j + 1);
			st.emplace_back(v); ed.emplace_back(v2);
		}
	}
	// vertical
	for(int i=0;i<n-1;i++){
		for(int j=0;j<n;j++){
			int v = i * n + j;
			int v2 = (i + 1) * n + j;
			st.emplace_back(v); ed.emplace_back(v2);
		}
	}
	return Graph(st, ed);
}


struct Estimator{
	int n;
	vector<float> mu;
	Matrix<float> sigma;
	bool zerofix;
	float logLikelihood;
	vector<vector<int>> streets;
	vector<int> gapFound;
	int gapFoundCount;
	/*
	Estimator(int n){
		this->n = n;
		this->mu = vector<float>(n);
		this->sigma = Matrix<float>::I(n);
	}
	*/
	Estimator(const vector<float> &mu, const Matrix<float> &sigma, bool zerofix){
		/* mu: 平均ベクトル，sigma: 共分散行列 */
		this->n = mu.size();
		this->mu = mu;
		this->sigma = sigma;
		this->zerofix = zerofix;
		logLikelihood = 0.0;

		// Estimator一般の機能ではないけれど...
		// (adjustCovariance 用)
		streets = vector<vector<int>>(60);
		for(int i=0;i<30;i++)for(int j=0;j<29;j++){
			streets[i].emplace_back(i * 29 + j);
		}
		for(int j=0;j<30;j++)for(int i=0;i<29;i++){
			streets[30 + j].emplace_back(870 + j + 30 * i);
		}
		gapFound = vector<int>(60);
		gapFoundCount = 0;
	}
	void observe(const vector<int> &indices, float sum, float noiseVar){
		/*
		sum(x[indices]) * (1 + eps) が sum であることを観測した（ことを報告するための関数）
		ただし eps は「平均 0 分散 noiseVar の正規分布」に従う
		*/
		// sigmaAB, sigmaBB, xBMinusMuB の計算
		vector<float> sigmaAB(n);
		for(int i=0;i<n;i++)for(int j : indices) sigmaAB[i] += sigma[i][j];
		
		float sigmaSum = 0, muSum = 0;
		for(int i : indices)for(int j : indices){
			sigmaSum += sigma[i][j];
			muSum += mu[i] * mu[j];
		}
		float sigmaBB = (1.0 + noiseVar) * sigmaSum + noiseVar * muSum;
		float xBMinusMuB = sum;
		for(int i : indices) xBMinusMuB -= mu[i];

		// mu の更新
		for(int i=0;i<n;i++){
			mu[i] += /*eta * */sigmaAB[i] / sigmaBB * xBMinusMuB;
		}
		// sigma の更新
		if (zerofix){
			for(int i=0;i<n;i++)for(int j=0;j<n;j++){
				if (sigma[i][j] == 0) continue;
				sigma[i][j] -= /*eta * */sigmaAB[i] / sigmaBB * sigmaAB[j];
			}
		}else{
			for(int i=0;i<n;i++)for(int j=0;j<n;j++){
				sigma[i][j] -= /*eta * */sigmaAB[i] / sigmaBB * sigmaAB[j];
			}			
		}
		// logLikelihood の加算
		// N(muB, sigmaBB) における xB の対数尤度
		// N(0, sigmaBB) における xBMinusMuB の対数尤度
		logLikelihood += -0.5 * log(2 * M_PI * sigmaBB) - xBMinusMuB * xBMinusMuB / (2 * sigmaBB);
	}
	vector<float> lcb(float coef){
		vector<float> ret(n);
		for(int i=0;i<n;i++) ret[i] = max(0.0f, mu[i] - safeSqrt(sigma[i][i]) * coef);
		// NOTE: 辺の長さの推定値として0未満の値が返ると都合が悪いので max(0.0, -) で回避している
		return ret;
	}

	void adjustCovariance(){
		/*
		「切れ目」を見つけて，その両側の辺長の共分散を0に近づける
		*/
		for(int h=0;h<streets.size();h++){
			// if (gapFound[h]) continue;
			vector<int> &s = streets[h];
			int argmaxi = -1;
			float maxi = 0.0;
			for(int i=1;i<s.size();i++){
				// s[i-1] と s[i] の統計情報に断絶があるような i を見つけたい
				float score = fabs(mu[s[i-1]] - mu[s[i]]) / (safeSqrt(sigma[s[i-1]][s[i-1]]) + safeSqrt(sigma[s[i]][s[i]]));
				if (score > maxi){
					maxi = score;
					argmaxi = i;
				}
			}
			if (maxi >= adjustCovarianceGapThreshold){
				gapFoundCount++;
				for(int j=0;j<argmaxi;j++)for(int k=argmaxi;k<s.size();k++){
					sigma[s[j]][s[k]] *= adjustCovarianceCoef;
					sigma[s[k]][s[j]] *= adjustCovarianceCoef;
				}
			}
		}
	}
};


string vpathToString(vector<int> path){
	/* グリッドグラフの頂点配列を受け取って LRUD 文字列を返す．*/
	string ret = "";
	for(int i=1;i<path.size();i++){
		int diff = path[i] - path[i-1];
		if (diff < -1) ret += "U";
		else if (diff == -1) ret += "L";
		else if (diff == +1) ret += "R";
		else ret += "D";
	}
	return ret;
}



vector<float> getPriorMean(){
	assert(GRID_SIZE == 30);
	int n = 2 * GRID_SIZE * (GRID_SIZE - 1);
	vector<float> ret(n, 5000.0);
	// 僅かにノイズを入れる．（結果としてジグザグの経路が最初に探索されるようになる）
	// for(int i=0;i<n;i++) ret[i] += frand() - 0.5;
	return ret;
}

Matrix<float> getPriorCovariance(int m){
	assert(GRID_SIZE == 30);
	assert(0 <= m && m <= 2);
	int n = 2 * GRID_SIZE * (GRID_SIZE - 1);
	// Matrix<float> ret(n, n);
	Matrix<float> ret(n, vector<float>(n));
	for(int i=0;i<n;i++){
		int sectionI = (i < 870) ? (i / 29) : (30 + (i - 870) % 30);
		int numI = (i < 870) ? (i % 29) : ((i - 870) / 30);
		for(int j=0;j<n;j++){
			int sectionJ = (j < 870) ? (j / 29) : (30 + (j - 870) % 30);
			int numJ = (j < 870) ? (j % 29) : ((j - 870) / 30);
			if (sectionI != sectionJ){
				ret[i][j] = 0;
			} else if (i != j){
				switch (m){
					case 1:
						ret[i][j] = 3001111.1;
						break;
					case 2:
						ret[i][j] = 3001111.1 * (1.0 - fabs(numI - numJ)/28.0);
						break;
					case 0:
						ret[i][j] = 3001111.1 * (1.0 - fabs(numI - numJ)/56.0);
						break;
				}
			} else {
				ret[i][j] = 3468888.9;
			}
		}
	}
	return ret;
}


tuple<vector<int>, vector<int>, float> findAdhocPath(int s, int g){
	int si = s / GRID_SIZE, sj = s % GRID_SIZE;
	int gi = g / GRID_SIZE, gj = g % GRID_SIZE;
	vector<int> vpath, epath;
	if (sj != gj){
		//上に行く
		for(int i=si;i>0;i--){
			vpath.emplace_back(i * GRID_SIZE + sj);
			epath.emplace_back(GRID_SIZE * (GRID_SIZE - 1) + (i - 1) * GRID_SIZE + sj);
		}
		//左右に行く
		int d = (sj < gj) ? +1 : -1;
		for(int j=sj;j!=gj;j+=d){
			vpath.emplace_back(j);
			epath.emplace_back((d == +1) ? j : (j-1));			
		}
		//下に行く
		for(int i=0;i<gi;i++){
			vpath.emplace_back(i * GRID_SIZE + gj);
			epath.emplace_back(GRID_SIZE * (GRID_SIZE - 1) + i * GRID_SIZE + gj);
		}
		vpath.emplace_back(g);
	}else{
		//左に行く
		for(int j=sj;j>0;j--){
			vpath.emplace_back(si * GRID_SIZE + j);
			epath.emplace_back(si * (GRID_SIZE - 1) + j - 1);
		}
		//上下にいく
		int d = (si < gi) ? +1 : -1;
		for(int i=si;i!=gi;i+=d){
			vpath.emplace_back(GRID_SIZE * i);
			epath.emplace_back(GRID_SIZE * (GRID_SIZE - 1) + GRID_SIZE * ((d == +1) ? i : (i-1)));		
		}
		//右に行く
		for(int j=0;j<gj;j++){
			vpath.emplace_back(gi * GRID_SIZE + j);
			epath.emplace_back(gi * (GRID_SIZE - 1) + j);
		}
		vpath.emplace_back(g);
	}
	return make_tuple(vpath, epath, 0.0f);
}



int main(void){
	t0 = system_clock::now();
	int numEdges = 2 * GRID_SIZE * (GRID_SIZE - 1);
	Graph graph = MakeGridGraph(GRID_SIZE);

	// Estimator est(getPriorMean(), getPriorCovariance(0));
	Estimator est1(getPriorMean(), getPriorCovariance(1), false);
	Estimator est2(getPriorMean(), getPriorCovariance(2), false);
	vector<int> count(2);
	vector<int> modelSwitchedIndices;
	int lastWinner = -1;
	int fixedWinner = -1;
	int est2PrioritizedCount = 0;

	for(int i=0;i<1000;i++){
		int si, sj, ti, tj;
		cin >> si >> sj >> ti >> tj;
		int vs = si * GRID_SIZE + sj, vg = ti * GRID_SIZE + tj;
		// graph.setCost(est.lcb(1.0));
		int winner = -1;
		if (fixedWinner == -1){
			if (est1.logLikelihood > est2.logLikelihood + tendencyToSelectEst2){
				winner = 1;
			} else {
				if (est1.logLikelihood > est2.logLikelihood){
					est2PrioritizedCount++;
				}
				winner = 2;
			}
		} else {
			winner = fixedWinner;
		}

		if (i == modelFixIter){
			fixedWinner = winner;
		}

		// if (i < modelFixIter) winner = 2; // 最初はひたすら est2 を使う設定

		if (winner == 1){
			graph.setCost(est1.lcb(getLcbValue(i)));
			count[0]++;
		}else{
			graph.setCost(est2.lcb(getLcbValue(i)));
			count[1]++;
		}

		if (lastWinner != winner){
			lastWinner = winner;
			modelSwitchedIndices.emplace_back(i);
		}

		auto pathInfo = graph.findShortestPath(vs, vg);
		/*if (i < 3){ // 最初の何回かは，デザインされた経路を回答する？
			pathInfo = findAdhocPath(vs, vg);
		}*/
		auto vpath = get<0>(pathInfo);
		auto epath = get<1>(pathInfo);
		cout << vpathToString(vpath) << endl;
		fflush(stdout);
		int trueDist;
		cin >> trueDist;
		if (!shouldHurryUp()){
			// est.observe(epath, trueDist, 0.2*0.2/12);
			if (fixedWinner != 2){
				est1.observe(epath, trueDist, 0.2*0.2/12);
			}
			if (fixedWinner != 1){
				est2.observe(epath, trueDist, 0.2*0.2/12);
				if ((i + 1) % adjustCovarianceInterval == 0) est2.adjustCovariance();
			}
		}
	}
	// cerr << "# Model used count: " << to_string(count) << endl;
	// cerr << "# Last switched: " << to_string(modelSwitchedIndices) << endl;
	// cerr << "# GapFoundCount of est2: " << est2.gapFoundCount << endl;
	// cerr << "# est2PrioritizedCount: " << est2PrioritizedCount << endl;
    return 0;
}


