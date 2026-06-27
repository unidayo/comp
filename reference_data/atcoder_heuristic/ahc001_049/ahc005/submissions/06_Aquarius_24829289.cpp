#ifndef __VECTOR_UTIL_H__
#define __VECTOR_UTIL_H__

/* updated: 2021-07-03 */

#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <limits>

template <class T>
std::vector<T> inputVector(int size, bool one_based = false, std::istream& ist = std::cin);
template <class T>
std::vector<std::vector<T>> inputVector2D(int height, int width, bool one_based = false, std::istream& ist = std::cin);
template <class T>
void outputVector(const std::vector<T>& vec, bool one_based = false, const std::string& delimiter = " ", std::ostream& ost = std::cout);
template <class T>
std::vector<T> makeVector(int size, const T& value);
template <class T>
std::vector<T> makeIota(int size, const T& initial_value = T());

template <class T, class Func>
void apply(std::vector<T>& vec, Func func);
template <class T, class BinaryFunc>
std::vector<T> elementwise(std::vector<T>& a, std::vector<T>& b, BinaryFunc func);
template <class T, class Pred>
void sortAndUnique(std::vector<T>& vec, Pred pred, bool use_pred_to_check_equivalence = false);
template <class T>
void sortAndUnique(std::vector<T>& vec);
template <class T>
int mex(const std::vector<T>& vec);
template <class T, class Pred>
std::vector<int> sortWithOriginalPositions(std::vector<T>& vec, Pred pred);
template <class T>
std::vector<int> sortWithOriginalPositions(std::vector<T>& vec);
template <class T, class Pred>
std::vector<int> sortWithDestinations(std::vector<T>& vec, Pred pred);
template <class T>
std::vector<int> sortWithDestinations(std::vector<T>& vec);

template <class T>
T maxOf(const std::vector<T>& vec);
template <class T>
T minOf(const std::vector<T>& vec);
template <class T>
T sumOf(const std::vector<T>& vec);
template <class T, class U>
U sumOf(const std::vector<T>& vec, const U& initial_value);

template <class T>
void sortVector(std::vector<T>& vec);
template <class T, class Pred>
void sortVector(std::vector<T>& vec, Pred pred);


/* 標準入力から値を受け取り vector を生成 */
template <class T>
std::vector<T> inputVector(int size, bool one_based, std::istream& ist) {
	std::vector<T> ret(size);
	for (int i = 0; i < size; ++i) {
		ist >> ret[i];
		if (one_based) {
			--ret[i];
		}
	}
	return ret;
}

/* 標準入力から値を受け取り2次元 vector を生成 */
template <class T>
std::vector<std::vector<T>> inputVector2D(int height, int width, bool one_based, std::istream& ist) {
	std::vector<std::vector<T>> ret(height, std::vector<T>(width));
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			ist >> ret[i][j];
			if (one_based) {
				--ret[i][j];
			}
		}
	}
	return ret;
}

/* vector の内容を delimiter 区切りで出力 */
template <class T>
void outputVector(const std::vector<T>& vec, bool one_based, const std::string& delimiter, std::ostream& ost) {
	if (vec.empty()) {
		ost << std::endl;
		return;
	}

	int size = vec.size();
	for (int i = 0; i < size; ++i) {
		T value = vec[i];
		if (one_based) {
			++value;
		}

		ost << value;
		if (i == size - 1) {
			ost << std::endl;
		} else {
			ost << delimiter;
		}
	}
}

/* vector 生成のヘルパー関数(主に多次元配列用) */
template <class T>
std::vector<T> makeVector(int size, const T& value) {
	return std::vector<T>(size, value);
}

template <class T>
std::vector<T> makeIota(int size, const T& initial_value) {
	std::vector<T> ret(size);
	std::iota(ret.begin(), ret.end(), initial_value);
	return ret;
}

/* vector の各要素に関数を適用する */
template <class T, class Func>
void apply(std::vector<T>& vec, Func func) {
	std::transform(vec.begin(), vec.end(), vec.begin(), func);
}

/* 2つの vector に対して要素ごとの演算を行って1つの vector を生成する */
template <class T, class BinaryFunc>
std::vector<T> elementwise(std::vector<T>& a, std::vector<T>& b, BinaryFunc func) {
	assert(a.size() == b.size());
	
	const int size = a.size();
	std::vector<T> ret(size);
	for (int i = 0; i < size; ++i) {
		ret[i] = func(a[i], b[i]);
	}
	return ret;
}

/* sort + unique O(n log n) */
/* use_pred_to_check_equivalence が true の場合, !pred(a, b) && !pred(b, a) により等値性を判断する */
/* そうでない場合, operator== により等値性を判断する */
template <class T, class Pred>
void sortAndUnique(std::vector<T>& vec, Pred pred, bool use_pred_to_check_equivalence) {
	std::sort(vec.begin(), vec.end(), pred);
	typename std::vector<T>::iterator it;
	if (use_pred_to_check_equivalence) {
		auto is_equal_to = [&pred](const T& a, const T& b) {
			return !pred(a, b) && !pred(b, a);
		};
		it = std::unique(vec.begin(), vec.end(), is_equal_to);
	} else {
		it = std::unique(vec.begin(), vec.end());
	}
	vec.erase(it, vec.end());
}

/* sort + unique O(n log n) */
template <class T>
void sortAndUnique(std::vector<T>& vec) {
	sortAndUnique(vec, std::less<T>());
}

/* vector に含まれない非負整数のうち、最小のもの (minimum excluded) を得る O(n log n) */
/* template である必要ないかも */
template <class T>
int mex(const std::vector<T>& vec) {
	std::vector<int> copied(vec);
	sortAndUnique(copied);
	
	int value = 0;
	while (value < copied.size() && copied[value] == value) {
		++value;
	}
	return value;
}

/* ソート + 復元用の配列を生成 O(n log n) */
/* vector を破壊的ソート + ソート後の各要素についてもともとの位置を格納した配列を返す */
template <class T, class Pred>
std::vector<int> sortWithOriginalPositions(std::vector<T>& vec, Pred pred) {
	std::vector<int> order(vec.size());
	std::iota(order.begin(), order.end(), 0);

	std::sort(order.begin(), order.end(), [&vec, pred](int i, int j) {
		return pred(vec[i], vec[j]);
	});

	std::sort(vec.begin(), vec.end(), pred);
	return order;
}

/* 比較関数なし版 O(n log n) */
template <class T>
std::vector<int> sortWithOriginalPositions(std::vector<T>& vec) {
	return sortWithOriginalPositions(vec, std::less<T>());
}

/* ソート + 復元用の配列を生成 O(n log n) */
/* vector を破壊的ソート + ソート前の各要素について移動先を格納した配列を返す */
/* 戻り値は sortWithOriginalPositions の戻り値の逆写像になる */
template <class T, class Pred>
std::vector<int> sortWithDestinations(std::vector<T>& vec, Pred pred) {
	const int size = vec.size();
	std::vector<int> positions = sortWithOriginalPositions(vec, pred);
	std::vector<int> ret(size);
	for (int i = 0; i < size; ++i) {
		ret[positions[i]] = i;
	}
	return ret;
}

/* 比較関数なし版 O(n log n) */
template <class T>
std::vector<int> sortWithDestinations(std::vector<T>& vec) {
	return sortWithDestinations(vec, std::less<T>());
}

/* 最大値を取得, vector が空の場合は numeric_limits<T>::lowest を返す O(n) */
template <class T>
T maxOf(const std::vector<T>& vec) {
	T ret = std::numeric_limits<T>::lowest();
	for (const T& element : vec) {
		ret = std::max(ret, element);
	}
	return ret;
}

/* 最小値を取得, vector が空の場合は numeric_limits<T>::max を返す O(n) */
template <class T>
T minOf(const std::vector<T>& vec) {
	T ret = std::numeric_limits<T>::max();
	for (const T& element : vec) {
		ret = std::min(ret, element);
	}
	return ret;
}

/* 合計を取得, 戻り値の型は要素の型 O(n) */
template <class T>
T sumOf(const std::vector<T>& vec) {
	T ret = T();
	for (const T& element : vec) {
		ret += element;
	}
	return ret;
}

/* 合計を取得, 戻り値の型が要素の型と異なるときのための関数 O(n) */
template <class T, class U>
U sumOf(const std::vector<T>& vec, const U& initial_value) {
	U ret(initial_value);
	for (const T& element : vec) {
		ret += element;
	}
	return ret;
}

template<class T>
void sortVector(std::vector<T>& vec) {
	std::sort(vec.begin(), vec.end());
}

template<class T, class Pred>
void sortVector(std::vector<T>& vec, Pred pred) {
	std::sort(vec.begin(), vec.end(), pred);
}

#endif

#ifndef TIMER_H
#define TIMER_H

#include <cstdint>

#ifdef ONLY_MY_ENVIR

#include <chrono>
class Timer {
	const std::chrono::time_point<std::chrono::system_clock> start;
	int64_t limit_microsec = 0;

public:
	Timer();
	void set(int64_t time_microsec);
	bool elapsed() const;
	int64_t getMicrosec() const;
};

#else

class Timer {
	static const uint64_t ClocksPerMicrosec;
	const uint64_t start_clocks;
	uint64_t limit_clocks;

	uint64_t getClocks() const;

public:
	Timer();
	void set(int64_t time_microsec);
	bool elapsed() const;
	int64_t getMicrosec() const;
};

#endif

#endif

#ifdef ONLY_MY_ENVIR
#include "Timer.h"
#endif

#ifdef ONLY_MY_ENVIR

using namespace std::chrono;

Timer::Timer()
	: start(system_clock::now()) {
}

void Timer::set(int64_t time_microsec) {
	limit_microsec += time_microsec;
}

bool Timer::elapsed() const {
	return getMicrosec() >= limit_microsec;
}

int64_t Timer::getMicrosec() const {
	return duration_cast<microseconds>(system_clock::now() - start).count();
}

#else

const uint64_t Timer::ClocksPerMicrosec = 2987;

uint64_t Timer::getClocks() const {
	unsigned int lo, hi;
	__asm__ volatile ("rdtsc" : "=a" (lo), "=d" (hi));
	return ((uint64_t)hi << 32) | lo;
}

Timer::Timer() 
	: start_clocks(getClocks()), limit_clocks(start_clocks) {
}

void Timer::set(int64_t time_microsec) {
	limit_clocks += time_microsec * ClocksPerMicrosec;
}

bool Timer::elapsed() const {
	return getClocks() >= limit_clocks;
}

int64_t Timer::getMicrosec() const {
	return (getClocks() - start_clocks) / ClocksPerMicrosec;
}

#endif

#ifndef RANDOM_H
#define RANDOM_H

#include <cstdint>
#include <utility>

uint32_t rnd();
uint32_t rnd(uint32_t max);

template <class T>
void shuffle(T& arr);

template <class T>
void shuffle(T& arr) {
	for (int i = arr.size(); i > 1; --i) {
		std::swap(arr[i - 1], arr[rnd(i)]);
	}
}

#endif
#ifdef ONLY_MY_ENVIR
#include "Random.h"
#endif

#include <vector>
#include <cstdint>

uint32_t rnd() {
	static uint32_t val = 3141592653ul;
	val ^= val << 13;
	val ^= val >> 17;
	val ^= val << 15;
	return val;
}

uint32_t rnd(uint32_t max) {
	return rnd() % max;
}

#ifndef __GRAPH_H__
#define __GRAPH_H__

/* updated: 2021-03-16 */

#include <vector>
#include <cstdint>
#include <iostream>
#include <string>
#include <sstream>

struct GraphEdge {
	int tail = 0;
	int head = 0;

	GraphEdge() = default;
	explicit GraphEdge(int tail, int head);
};

template <class T>
class WeightedGraph;

/* 有向グラフクラス(特に無向グラフクラスは用意せず、両方向に辺を張ることで対応する) */
class Graph {
public:
	using EdgeID = int;
	using Edge = GraphEdge;
	using List = std::vector<EdgeID>;

private:
	std::vector<List> ids_m;
 	std::vector<Edge> edges_m;

public:
	Graph() = default;
	Graph(Graph&&) = default;
	Graph(const Graph&) = default;
	Graph& operator=(Graph&&) = default;
	Graph& operator=(const Graph&) = default;

	explicit Graph(int size);

	int size() const;
	int edgeCount() const;
	void init(int new_size);
	void reserve(int edge_count);
	
	void add(int tail, int head, bool directed = true);
	const List& from(int tail) const;
	const Edge& getEdge(EdgeID id) const;
	const std::vector<Edge>& getEdges() const;
	EdgeID reverseEdgeOf(EdgeID id) const;

	template <class T>
	WeightedGraph<T> toWeighted(const T& weight = T(1)) const;

	std::string toString() const;
	void debug() const;
};

template<class T>
WeightedGraph<T> Graph::toWeighted(const T& weight) const {
	return WeightedGraph<T>(*this, weight);
}

Graph inputGraphImpl(int size, int edge_count, bool directed, bool one_based = true, std::istream& ist = std::cin);
Graph inputUndirectedGraph(int size, int edge_count, bool one_based = true, std::istream& ist = std::cin);
Graph inputDirectedGraph(int size, int edge_count, bool one_based = true, std::istream& ist = std::cin);
Graph inputTree(int size, bool one_based = true, std::istream& ist = std::cin);
void outputGraph(const Graph& graph, bool directed = true, bool one_based = true, std::ostream& ost = std::cout);

template <class T>
using Weights = std::vector<T>;

template <class T>
class WeightedGraph {
	Graph graph_m;
	Weights<T> weights_m;

public:
	WeightedGraph() = default;
	WeightedGraph(WeightedGraph&&) = default;
	WeightedGraph(const WeightedGraph&) = default;
	WeightedGraph& operator=(WeightedGraph&&) = default;
	WeightedGraph& operator=(const WeightedGraph&) = default;

	explicit WeightedGraph(int size);
	explicit WeightedGraph(const Graph& graph, const T& weight = T(1));
	explicit WeightedGraph(const Graph& graph, const Weights<T>& weights);

	int size() const;
	int edgeCount() const;
	void init(int new_size);
	void reserve(int edge_count);

	void add(int tail, int head, T&& weight, bool directed = true);
	void add(int tail, int head, const T& weight, bool directed = true);

	const Graph& getGraph() const;
	const Weights<T>& getWeights() const;

	std::string toString() const;
	void debug() const;
};

template<class T>
WeightedGraph<T>::WeightedGraph(int size) 
	: graph_m(size) {
}

template<class T>
WeightedGraph<T>::WeightedGraph(const Graph& graph, const T& weight)
	: graph_m(graph), weights_m(graph.edgeCount(), weight) {
}

template<class T>
WeightedGraph<T>::WeightedGraph(const Graph& graph, const Weights<T>& weights) 
	: graph_m(graph), weights_m(weights) {
	assert(graph.edgeCount() == weights.size());
}

template <class T>
int WeightedGraph<T>::size() const {
	return graph_m.size();
}

template<class T>
int WeightedGraph<T>::edgeCount() const {
	return graph_m.edgeCount();
}

template<class T>
void WeightedGraph<T>::init(int new_size) {
	graph_m.init(new_size);
}

template<class T>
void WeightedGraph<T>::reserve(int edge_count) {
	graph_m.reserve(edge_count);
	weights_m.reserve(edge_count);
}

template<class T>
void WeightedGraph<T>::add(int tail, int head, T&& weight, bool directed) {
	if (!directed) {
		add(tail, head, std::move(weight), true);
		add(head, tail, std::move(weight), true);
		return;
	}

	graph_m.add(tail, head);
	weights_m.push_back(std::move(weight));
}

template<class T>
void WeightedGraph<T>::add(int tail, int head, const T& weight, bool directed) {
	if (!directed) {
		add(tail, head, weight, true);
		add(head, tail, weight, true);
		return;
	}

	graph_m.add(tail, head);
	weights_m.push_back(weight);
}

template<class T>
const Graph& WeightedGraph<T>::getGraph() const {
	return graph_m;
}

template<class T>
const Weights<T>& WeightedGraph<T>::getWeights() const {
	return weights_m;
}

template<class T>
std::string WeightedGraph<T>::toString() const {
	std::ostringstream stream;
	for (int tail = 0; tail < graph_m.size(); ++tail) {
		stream << tail << ": [";
		for (auto id : graph_m.from(tail)) {
			const auto& edge = graph_m.getEdge(id);
			int head = edge.head;
			stream << ' ' << head << '(' << weights_m[id] << ')';
		}
		stream << " ]" << std::endl;
	}
	return stream.str();
}

template<class T>
void WeightedGraph<T>::debug() const {
	std::cerr << toString() << std::endl;
}

template <class T>
WeightedGraph<T> inputWeightedGraphImpl(int size, int edge_count, bool directed, bool one_based = true, std::istream& ist = std::cin);
template <class T>
WeightedGraph<T> inputWeightedUndirectedGraph(int size, int edge_count, bool one_based = true, std::istream& ist = std::cin);
template <class T>
WeightedGraph<T> inputWeightedDirectedGraph(int size, int edge_count, bool one_based = true, std::istream& ist = std::cin);
template <class T>
WeightedGraph<T> inputWeightedTree(int size, bool one_based = true, std::istream& ist = std::cin);
template <class T>
void outputWeightedGraph(const WeightedGraph<T>& wgraph, bool directed = true, bool one_based = true, std::ostream& ost = std::cout);

template <class T>
WeightedGraph<T> inputWeightedGraphImpl(int size, int edge_count, bool directed, bool one_based, std::istream& ist) {
	WeightedGraph<T> ret(size);
	for (int j = 0; j < edge_count; ++j) {
		int a, b;
		T weight;
		ist >> a >> b >> weight;
		if (one_based) {
			--a; --b;
		}

		if (directed) {
			ret.add(a, b, weight);
		} else {
			ret.add(a, b, weight);
			ret.add(b, a, weight);
		}
	}
	return ret;
}

template <class T>
WeightedGraph<T> inputWeightedUndirectedGraph(int size, int edge_count, bool one_based, std::istream& ist) {
	return inputWeightedGraphImpl<T>(size, edge_count, false, one_based, ist);
}

template <class T>
WeightedGraph<T> inputWeightedDirectedGraph(int size, int edge_count, bool one_based, std::istream& ist) {
	return inputWeightedGraphImpl<T>(size, edge_count, true, one_based, ist);
}

template<class T>
WeightedGraph<T> inputWeightedTree(int size, bool one_based, std::istream& ist) {
	return inputWeightedGraphImpl<T>(size, size - 1, false, one_based, ist);
}

template<class T>
void outputWeightedGraph(const WeightedGraph<T>& wgraph, bool directed, bool one_based, std::ostream& ost) {
	const Graph& graph = wgraph.getGraph();
	const Weights<T>& weights = wgraph.getWeights();
	if (directed) {
		/* 有向グラフとみなして出力 */
		ost << graph.size() << ' ' << graph.edgeCount() << std::endl;
		for (Graph::EdgeID id = 0; id < graph.edgeCount(); ++id) {
			const auto& edge = graph.getEdge(id);
			int tail = edge.tail;
			int head = edge.head;
			if (one_based) {
				++tail;
				++head;
			}
			ost << tail << ' ' << head << ' ' << weights[id] << std::endl;
		}
	} else {
		/* 無向グラフとみなして出力(無向グラフでなかった場合の保証はない) */
		ost << graph.size() << ' ' << graph.edgeCount() / 2 << std::endl;

		/* 辺 (tail, head) と (head, tail) が交互に追加されているので片方を出力 */
		bool is_to_output = true;
		for (Graph::EdgeID id = 0; id < graph.edgeCount(); ++id) {
			const auto& edge = graph.getEdge(id);
			if (is_to_output) {
				int tail = edge.tail;
				int head = edge.head;
				if (tail > head) {
					std::swap(tail, head);
				}
				if (one_based) {
					++tail;
					++head;
				}
				ost << tail << ' ' << head << ' ' << weights[id] << std::endl;
			}
			is_to_output = !is_to_output;
		}
	}
}

#endif
#ifdef ONLY_MY_ENVIR
#include "Graph.h"
#endif

#include <cassert>
#include <sstream>

GraphEdge::GraphEdge(int tail, int head)
	: tail(tail), head(head) {
}

Graph::Graph(int size) 
	: ids_m(size) {
}

int Graph::size() const {
	return ids_m.size();
}

int Graph::edgeCount() const {
	return edges_m.size();
}

void Graph::init(int new_size) {
	assert(new_size >= size());
	ids_m.resize(new_size);
}

void Graph::reserve(int edge_count) {
	edges_m.reserve(edge_count);
}

void Graph::add(int tail, int head, bool directed) {
	if (!directed) {
		add(tail, head, true);
		add(head, tail, true);
		return;
	}

	ids_m[tail].push_back(edges_m.size());
	edges_m.push_back(Edge(tail, head));
}

const Graph::List& Graph::from(int tail) const {
	assert(tail >= 0 && tail < size());
	return ids_m[tail];
}

const Graph::Edge& Graph::getEdge(EdgeID id) const {
	assert(id >= 0 && id < edgeCount());
	return edges_m[id];
}

const std::vector<Graph::Edge>& Graph::getEdges() const {
	return edges_m;
}

/* 無向グラフ用: 逆辺を参照する */
Graph::EdgeID Graph::reverseEdgeOf(EdgeID id) const {
	return id ^ 0x1;
}

std::string Graph::toString() const {
	std::ostringstream stream;
	for (int tail = 0; tail < size(); ++tail) {
		stream << tail << ": [";
		for (auto id : from(tail)) {
			const auto& edge = getEdge(id);
			int head = edge.head;
			stream << ' ' << head;
		}
		stream << " ]" << std::endl;
	}
	return stream.str();
}

void Graph::debug() const {
	std::cerr << toString() << std::endl;
}

Graph inputGraphImpl(int size, int edge_count, bool directed, bool one_based, std::istream& ist) {
	Graph ret(size);
	for (int j = 0; j < edge_count; ++j) {
		int a, b;
		ist >> a >> b;
		if (one_based) {
			--a; --b;
		}

		if (directed) {
			ret.add(a, b);
		} else {
			ret.add(a, b);
			ret.add(b, a);
		}
	}
	return ret;
}

Graph inputUndirectedGraph(int size, int edge_count, bool one_based, std::istream& ist) {
	return inputGraphImpl(size, edge_count, false, one_based, ist);
}

Graph inputDirectedGraph(int size, int edge_count, bool one_based, std::istream& ist) {
	return inputGraphImpl(size, edge_count, true, one_based, ist);
}

Graph inputTree(int size, bool one_based, std::istream& ist) {
	return inputGraphImpl(size, size - 1, false, one_based, ist);
}

void outputGraph(const Graph& graph, bool directed, bool one_based, std::ostream& ost) {
	if (directed) {
		/* 有向グラフとみなして出力 */
		ost << graph.size() << ' ' << graph.edgeCount() << std::endl;
		for (const auto& edge : graph.getEdges()) {
			int tail = edge.tail;
			int head = edge.head;
			if (one_based) {
				++tail;
				++head;
			}
			ost << tail << ' ' << head << std::endl;
		}
	} else {
		/* 無向グラフとみなして出力(無向グラフでなかった場合の保証はない) */
		ost << graph.size() << ' ' << graph.edgeCount() / 2 << std::endl;
		
		/* 辺 (tail, head) と (head, tail) が交互に追加されているので片方を出力 */
		bool is_to_output = true;
		for (const auto& edge : graph.getEdges()) {
			if (is_to_output) {
				int tail = edge.tail;
				int head = edge.head;
				if (tail > head) {
					std::swap(tail, head);
				}
				if (one_based) {
					++tail;
					++head;
				}
				ost << tail << ' ' << head << std::endl;
			}
			is_to_output = !is_to_output;
		}
	}
}


#ifndef __SHORTEST_H__
#define __SHORTEST_H__

/* updated: 2020-07-24 */

#ifdef ONLY_MY_ENVIR
#include "Graph.h"
#endif

#include <vector>
#include <queue>
#include <utility>
#include <limits>
#include <algorithm>

/* 復元用の配列の中身を辺IDにするかどうか */

template <class T>
struct DijkstraResult {
	std::vector<T> distance;
	std::vector<int> previous;
};

template <class T>
struct BellmanFordResult {
	std::vector<T> distance;
	std::vector<int> previous;
	std::vector<bool> is_unbounded;		// 始点からの距離がいくらでも小さくなる(この場合 distance と previous の値は意味を持たない)
};

template <class T>
DijkstraResult<T> dijkstra(const WeightedGraph<T>& wgraph, int start_vertex, T infinity = std::numeric_limits<T>::max());
template <class T>
DijkstraResult<T> dijkstra(const WeightedGraph<T>& wgraph, std::vector<int> start_vertices, T infinity = std::numeric_limits<T>::max());
template <class T>
BellmanFordResult<T> bellmanFord(const WeightedGraph<T>& wgraph, int start, T infinity = std::numeric_limits<T>::max());
template <class T>
std::vector<std::vector<T>> warshallFloyd(const WeightedGraph<T>& wgraph, T infinity = std::numeric_limits<T>::max());
template <class T>
bool hasNegativeCycle(const WeightedGraph<T>& wgraph);

/* ダイクストラ法により単一始点最短路を求める O(m log n) */
template <class T>
DijkstraResult<T> dijkstra(const WeightedGraph<T>& wgraph, int start_vertex, T infinity) {
	return dijkstra(wgraph, std::vector<int>(1, start_vertex), infinity);
}

/* ダイクストラ法により最短路を求める (複数始点版) */
template <class T>
DijkstraResult<T> dijkstra(const WeightedGraph<T>& wgraph, std::vector<int> start_vertices, T infinity) {
	struct State {
		T dist;
		int vertex;

		State(T dist, int vertex)
			: dist(dist), vertex(vertex) {
		}

		// 順序を逆転させる
		bool operator<(const State& right) const {
			return dist > right.dist;
		}
	};

	const Graph& graph = wgraph.getGraph();
	const auto& costs = wgraph.getWeights();
	const int size = graph.size();

	std::vector<T> distance(size, infinity);
	std::vector<int> previous(size, -1);

	std::priority_queue<State> que;
	for (int start_vertex : start_vertices) {
		distance[start_vertex] = 0;
		que.push(State(T(), start_vertex));
	}

	while (!que.empty()) {
		const State state = que.top();
		que.pop();

		const T dist = state.dist;
		const int tail = state.vertex;

		if (dist > distance[tail]) continue;

		for (Graph::EdgeID id : graph.from(tail)) {
			const auto& edge = graph.getEdge(id);
			const int head = edge.head;
			const T cost = costs[id];
			const T dist_new = dist + cost;

			if (dist_new >= distance[head]) continue;

			distance[head] = dist_new;
			previous[head] = tail;
			que.push(State(dist_new, head));
		}
	}

	DijkstraResult<T> result;
	result.distance = std::move(distance);
	result.previous = std::move(previous);
	return result;
}


/* ベルマンフォード法により単一始点最短路を求める O(nm) */
/* 負閉路検出には別の関数を使う: hasNegativeCycle */
template <class T>
BellmanFordResult<T> bellmanFord(const WeightedGraph<T>& wgraph, int start, T infinity) {
	const Graph& graph = wgraph.getGraph();
	const auto& costs = wgraph.getWeights();
	const int size = graph.size();

	std::vector<T> distance(size, infinity);
	std::vector<int> previous(size, -1);
	std::vector<bool> is_unbounded(size, false);

	distance[start] = T();

	// 始点からの距離を更新
	auto update = [size, infinity, &graph, &costs](std::vector<T>& distance, std::vector<int>& previous) -> void {
		for (int tail = 0; tail < size; ++tail) {
			if (distance[tail] == infinity) continue;
			for (Graph::EdgeID id : graph.from(tail)) {
				const auto& edge = graph.getEdge(id);
				const int head = edge.head;
				const T cost = costs[id];

				if (distance[tail] + cost < distance[head]) {
					distance[head] = distance[tail] + cost;
					previous[head] = tail;
				}
			}
		}
	};

	// 負閉路を検出(あれば is_unbounded を true とする)
	auto detect = [size, infinity, &graph, &costs](std::vector<T>& distance, std::vector<bool>& is_unbounded) -> void {
		for (int tail = 0; tail < size; ++tail) {
			if (distance[tail] == infinity) continue;
			for (Graph::EdgeID id : graph.from(tail)) {
				const auto& edge = graph.getEdge(id);
				const int head = edge.head;
				const T cost = costs[id];

				if (distance[tail] + cost < distance[head]) {
					is_unbounded[tail] = true;
					break;
				}
			}
		}
	};

	// 負閉路を伝搬
	auto propagate = [size, &graph](std::vector<bool>& is_unbounded) -> void {
		for (int tail = 0; tail < size; ++tail) {
			if (!is_unbounded[tail]) continue;
			for (Graph::EdgeID id : graph.from(tail)) {
				const auto& edge = graph.getEdge(id);
				const int head = edge.head;
				is_unbounded[head] = true;
			}
		}
	};

	for (int z = 1; z < size; ++z) {
		update(distance, previous);
	}

	detect(distance, is_unbounded);

	for (int z = 1; z < size; ++z) {
		propagate(is_unbounded);
	}

	BellmanFordResult<T> result;
	result.distance = std::move(distance);
	result.previous = std::move(previous);
	result.is_unbounded = std::move(is_unbounded);
	return result;
}

/* ワーシャルフロイド法により全点間最短路を求める O(n^3) */
template <class T>
std::vector<std::vector<T>> warshallFloyd(const WeightedGraph<T>& wgraph, T infinity) {
	const Graph& graph = wgraph.getGraph();
	const auto& costs = wgraph.getWeights();
	const int size = graph.size();

	std::vector<std::vector<T>> distance(size, std::vector<T>(size, infinity));

	for (int i = 0; i < size; ++i) {
		distance[i][i] = 0;
	}

	for (int tail = 0; tail < size; ++tail) {
		for (Graph::EdgeID id : graph.from(tail)) {
			const auto& edge = graph.getEdge(id);
			int head = edge.head;
			T cost = costs[id];

			distance[tail][head] = std::min(distance[tail][head], cost);
		}
	}

	for (int k = 0; k < size; ++k) {
		for (int i = 0; i < size; ++i) {
			if (distance[i][k] == infinity) continue;
			for (int j = 0; j < size; ++j) {
				if (distance[k][j] == infinity) continue;
				distance[i][j] = std::min(distance[i][j], distance[i][k] + distance[k][j]);
			}
		}
	}

	return distance;
}

/* ベルマンフォード法により負閉路を検出 O(nm) */
template <class T>
bool hasNegativeCycle(const WeightedGraph<T>& wgraph) {
	const Graph& graph = wgraph.getGraph();
	const auto& costs = wgraph.getWeights();
	const int size = graph.size();

	std::vector<T> distance(size, T());

	auto update = [size, &graph, &costs](std::vector<T>& distance) -> bool {
		bool is_updated = false;
		for (int tail = 0; tail < size; ++tail) {
			for (Graph::EdgeID id : graph.from(tail)) {
				const auto& edge = graph.getEdge(id);
				const int head = edge.head;
				const T cost = costs[id];

				if (distance[tail] + cost < distance[head]) {
					distance[head] = distance[tail] + cost;
					is_updated = true;
				}
			}
		}
		return is_updated;
	};

	for (int z = 1; z < size; ++z) {
		update(distance);
	}

	// n 回目の更新で距離が変化したなら負閉路がある
	return update(distance);
}


#endif


#ifndef __GRAPH_UTIL_H__
#define __GRAPH_UTIL_H__

/* updated: 2021-03-16 */

// required: Graph

#ifdef ONLY_MY_ENVIR
#include "Graph.h"
#endif

#include <vector>

Graph createReverseGraph(const Graph& graph);
template <class T>
WeightedGraph<T> createReverseWeightedGraph(const WeightedGraph<T>& wgraph);

Graph createComplementGraph(const Graph& graph);

bool isTree(const Graph& graph);

bool isBipartite(const Graph& graph);
std::vector<bool> colorBipartiteGraph(const Graph& graph);
bool isBipartiteSub(const Graph& graph, std::vector<bool>& coloring);

struct ConnectedComponent {
	Graph component;
	std::vector<int> vertices;
};
std::vector<ConnectedComponent> connectedComponents(const Graph& graph);

Graph constructSubgraph(const Graph& graph, const std::vector<Graph::EdgeID>& ids);
template <class T>
WeightedGraph<T> constructWeightedSubgraph(const WeightedGraph<T>& wgraph, const std::vector<Graph::EdgeID>& ids);

std::vector<int> reconstructPath(const std::vector<int>& previous, int goal);

/* 辺の向きを逆転させた重みつきグラフを作成 O(n + m) */
template<class T>
WeightedGraph<T> createReverseWeightedGraph(const WeightedGraph<T>& wgraph) {
	const Graph& graph = wgraph.getGraph();
	const Weights<T>& weights = wgraph.getWeights();
	const int size = graph.size();
	const int edge_count = graph.edgeCount();

	WeightedGraph<T> ret(size);
	ret.reserve(edge_count);

	for (Graph::EdgeID id = 0; id < edge_count; ++id) {
		const auto& edge = graph.getEdge(id);
		ret.add(edge.head, edge.tail, weights[id]);
	}
	return ret;
}

/* 辺IDのリストから部分グラフを構築(重み付き, 頂点は変更しない) O(n + m) */
template <class T>
WeightedGraph<T> constructWeightedSubgraph(const WeightedGraph<T>& wgraph, const std::vector<Graph::EdgeID>& ids) {
	const Graph& graph = wgraph.getGraph();
	const Weights<T>& weights = wgraph.getWeights();

	WeightedGraph<T> ret(graph.size());
	for (Graph::EdgeID id : ids) {
		const auto& edge = graph.getEdge(id);
		int tail = edge.tail;
		int head = edge.head;
		const T& weight = weights[id];
		ret.add(tail, head, weight);
		ret.add(head, tail, weight);
	}
	return ret;
}

#endif
#ifdef ONLY_MY_ENVIR
#include "GraphUtil.h"
#endif

#include <cassert>
#include <stack>
#include <algorithm>

/* 辺の向きを逆転させたグラフを作成 O(n + m) */
Graph createReverseGraph(const Graph& graph) {
	const int size = graph.size();
	Graph ret(size);
	ret.reserve(graph.edgeCount());

	for (const auto& edge : graph.getEdges()) {
		ret.add(edge.head, edge.tail);
	}
	return ret;
}

/* TODO: 無向グラフは？ */
/* 補グラフを作成 O(n^2 + m) */
Graph createComplementGraph(const Graph& graph) {
	const int size = graph.size();
	std::vector<std::vector<bool>> matrix(size, std::vector<bool>(size, true));
	for (int i = 0; i < size; ++i) {
		matrix[i][i] = false;
	}

	for (const auto& edge : graph.getEdges()) {
		matrix[edge.head][edge.tail] = false;
	}

	Graph ret(size);
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			if (matrix[i][j]) {
				ret.add(i, j);
			}
		}
	}
	return ret;
}

/* 無向グラフが木であるかどうか判定する O(n + m) */
/* 無向グラフであることは前提 (判定しない) */
bool isTree(const Graph& graph) {
	/* 辺数の判定 */
	if (graph.edgeCount() != 2 * (graph.size() - 1)) return false;

	/* 連結性の判定 */
	std::vector<bool> visited(graph.size());
	std::stack<int> stack;
	stack.push(0);

	while (!stack.empty()) {
		int tail = stack.top();
		stack.pop();

		if (visited[tail]) continue;
		visited[tail] = true;

		for (auto id : graph.from(tail)) {
			int head = graph.getEdge(id).head;
			stack.push(head);
		}
	}
	
	for (bool value : visited) {
		if (!value) return false;
	}
	return true;
}

/* 無向グラフの二部グラフ判定 O(n + m) */
bool isBipartite(const Graph& graph) {
	std::vector<bool> coloring(graph.size());
	return isBipartiteSub(graph, coloring);
}

/* 二部グラフとなるような頂点の分割を一つ見つける O(n + m) */
/* out: 頂点 -> 組 { 0, 1 } */
std::vector<bool> colorBipartiteGraph(const Graph& graph) {
	std::vector<bool> coloring(graph.size());
	bool succeeded = isBipartiteSub(graph, coloring);
	assert(succeeded);
	return coloring;
}

/* out: (bool, coloring) = (二部グラフかどうか, 二部グラフのときの塗り分け) */
bool isBipartiteSub(const Graph& graph, std::vector<bool>& coloring) {
	assert(graph.size() == coloring.size());

	struct State {
		int vertex;
		bool color;
		State(int vertex, bool color)
			: vertex(vertex), color(color) {
		}
	};

	const int size = graph.size();
	std::vector<bool> visited(size);
	for (int tail = 0; tail < size; ++tail) {
		if (visited[tail]) continue;

		std::stack<State> stack;
		stack.push(State(tail, false));
		while (!stack.empty()) {
			const State state = stack.top();
			stack.pop();

			int tail = state.vertex;
			bool color = state.color;
			if (visited[tail]) {
				if (coloring[tail] != color) return false;
				continue;
			}

			visited[tail] = true;
			coloring[tail] = color;

			for (auto id : graph.from(tail)) {
				const auto& edge = graph.getEdge(id);
				int head = edge.head;
				stack.push(State(head, !color));
			}
		}
	}
	return true;
}

/* 連結成分分解 O(n + m) */
/* out: 組(連結成分, 頂点リスト) のリスト */
/* 既知の問題点: 対応する辺が順に並ばない */
std::vector<ConnectedComponent> connectedComponents(const Graph& graph) {
	std::vector<ConnectedComponent> ret;

	const int size = graph.size();
	std::vector<bool> visited(size);
	std::vector<int> destination(size);		// 各頂点の移動先を保持しておくための配列
	auto search = [&graph, &visited, &destination](int start) -> ConnectedComponent {
		std::vector<int> group;
		std::stack<int> stack;
		std::vector<Graph::EdgeID> edge_ids;
		stack.push(start);

		int counter = 0;
		while (!stack.empty()) {
			int tail = stack.top();
			stack.pop();

			if (visited[tail]) continue;
			visited[tail] = true;
			group.push_back(tail);
			destination[tail] = counter;
			++counter;

			for (auto id : graph.from(tail)) {
				edge_ids.push_back(id);
				const auto& edge = graph.getEdge(id);
				int head = edge.head;
				stack.push(head);
			}
		}

		/* 連結成分グラフの構成 */
		Graph component(group.size());
		for (auto id : edge_ids) {
			const auto& edge = graph.getEdge(id);
			int tail_new = destination[edge.tail];
			int to_new = destination[edge.head];
			component.add(tail_new, to_new);
		}

		ConnectedComponent ret;
		ret.component = std::move(component);
		ret.vertices = std::move(group);
		return ret;
	};

	for (int tail = 0; tail < size; ++tail) {
		if (visited[tail]) continue;
		ret.push_back(search(tail));
	}

	return ret;
}

/* 辺IDのリストから部分グラフを構築(頂点は変更しない), O(n + m) */
Graph constructSubgraph(const Graph& graph, const std::vector<Graph::EdgeID>& ids) {
	Graph ret(graph.size());
	for (Graph::EdgeID id : ids) {
		const auto& edge = graph.getEdge(id);
		int tail = edge.tail;
		int head = edge.head;
		ret.add(tail, head);
		ret.add(head, tail);
	}
	return ret;
}

/* previous の配列からパスを復元 */
std::vector<int> reconstructPath(const std::vector<int>& previous, int goal) {
	std::vector<int> path;

	int curr = goal;
	while (curr != -1) {	// TODO: 始点に -1 を使うことを明文化しよう
		path.push_back(curr);
		curr = previous[curr];
	}

	std::reverse(path.begin(), path.end());
	return path;
}
#ifndef __MAIN_H__
#define __MAIN_H__

#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <utility>
#include <algorithm>
#include <functional>
#include <cmath>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <iomanip>
#include <sstream>
#include <bitset>
#include <limits>
#include <numeric>
#include <valarray>
#include <fstream>
#include <array>
#include <random>
#include <unordered_set>
#include <unordered_map>

using namespace std;
using uint = uint32_t;
using LL = int64_t;
using ULL = uint64_t;
using PP = pair<LL, LL>;
template <typename T> using PriorityQ = priority_queue<T, vector<T>, greater<T> >;
#define REP(i, a, n) for(LL i = (a), i##_max_ = (n); i < i##_max_; ++i)
#define REM(i, a, n) for(LL i = (LL)(n) - 1, i##_min_ = (a); i >= i##_min_; --i)
#define FLOAT fixed << setprecision(16)
#define SPEEDUP { cin.tie(NULL); ios::sync_with_stdio(false); }
const int INF = 0x3FFFFFFF;
const LL INFLL = 0x3FFFFFFF3FFFFFFF;
const double INFD = 1.0e+308;
const double EPS = 1.0e-9;

void YesNo(bool b) { cout << (b ? "Yes" : "No") << endl; }
void YESNO(bool b) { cout << (b ? "YES" : "NO") << endl; }
template <class T, class U> istream& operator>>(istream& ist, pair<T, U>& right) { return ist >> right.first >> right.second; }
template <class T, class U> ostream& operator<<(ostream& ost, const pair<T, U>& right) { return ost << right.first << ' ' << right.second; }
template <class T, class TCompatible, size_t N> void Fill(T(&dest)[N], const TCompatible& val) { fill(dest, dest + N, val); }
template <class T, class TCompatible, size_t M, size_t N> void Fill(T(&dest)[M][N], const TCompatible& val) { for (int i = 0; i < M; ++i) Fill(dest[i], val); }
template <class T> T Next() { T buf; cin >> buf; return buf; }
istream& Ignore(istream& ist) { string s; ist >> s; return ist; }
bool Inside(int i, int j, int h, int w) { return i >= 0 && i < h&& j >= 0 && j < w; }
void pL() { std::cout << std::endl; }
template <class Head> void pL(Head&& head) { std::cout << head << std::endl; }
template <class Head, class... Tail> void pL(Head&& head, Tail&&... tail) { std::cout << head << ' '; pL(std::forward<Tail>(tail)...); }

#ifdef __GNUC__
using LLL = __int128;
istream& operator>>(istream& ist, __int128& val) { LL tmp; ist >> tmp; val = tmp; return ist; }
ostream& operator<<(ostream& ost, __int128 val) { LL tmp = val; ost << tmp; return ost; }
#else
using LLL = LL;
#endif

#ifdef ONLY_MY_ENVIR
#include "Accumulator.h"
#include "Algebraic.h"
#include "BinaryMatrix.h"
#include "BinaryTree.h"
#include "Bipartite.h"
#include "BIT.h"
#include "BitConvolution.h"
#include "BitOperations.h"
#include "BruteForce.h"
#include "CartesianTree.h"
#include "Compressor.h"
#include "Decompositions.h"
#include "DiscreteLog.h"
#include "DominatorTree.h"
#include "DynamicMod.h"
#include "Exponential.h"
#include "Factorial.h"
#include "Factorization.h"
#include "FFT.h"
#include "Field.h"
#include "FlatTree.h"
#include "FlowSolver.h"
#include "Geometric2D.h"
#include "Geometric2DFloat.h"
#include "Geometric3D.h"
#include "Geometric3DFloat.h"
#include "Graph.h"
#include "GraphUtil.h"
#include "HLD.h"
#include "Interpolation.h"
#include "IntMod.h"
#include "IntModExtended.h"
#include "Inversion.h"
#include "KDTree.h"
#include "LazySegmentTree.h"
#include "LCA.h"
#include "LIS.h"
#include "List.h"
#include "Math.h"
#include "MathUtil.h"
#include "Matrix.h"
#include "MatrixTree.h"
#include "Multivariate.h"
#include "MinCostFlowSolver.h"
#include "MinMax.h"
#include "Numbers.h"
#include "Optimize.h"
#include "Permutation.h"
#include "Polynomial.h"
#include "Position.h"
#include "Quadratic.h"
#include "Random.h"
#include "Range.h"
#include "Rational.h"
#include "RollingHash.h"
#include "RuntimeMod.h"
#include "SafeInt.h"
#include "SegmentTree.h"
#include "SegmentTree2D.h"
#include "Sets.h"
#include "Shortest.h"
#include "SlidingWindow.h"
#include "SpanningTree.h"
#include "StringSearching.h"
#include "SuffixArray.h"
#include "SAUtilizer.h"
#include "SwitchList.h"
#include "Timer.h"
#include "Tree.h"
#include "TreeUtil.h"
#include "UnionFind.h"
#include "Unicyclic.h"
#include "Util.h"
#include "VectorUtil.h"
#endif

#endif
#ifdef ONLY_MY_ENVIR
#include "Main.h"
#endif

#ifdef ONLY_MY_ENVIR
ifstream input_stream("aain.txt");
ofstream output_stream("aaout.txt");
#define cin input_stream
#define cout output_stream
#endif

int n, si, sj, sv;
string f[69];
vector<vector<int>> dist;
vector<vector<int>> cds;
vector<vector<bool>> incds;

// private
vector<PP> vtop;
map<PP, int> ptov;
WeightedGraph<int> g;

class Helper {
	vector<int> rs;
	vector<int> vs;
	int h;
	int sc;

public:
	Helper() {
		h = cds.size();
		rs.resize(h + 1);
		vs.resize(h + 1);
		REP(i, 0, h) rs[i] = i;
		vs[0] = vs[h] = sv;
		REP(i, 1, h) vs[i] = cds[i][0];
		calcScore();
	}

	void calcScore() {
		int sum = 0;
		REP(i, 0, h) {
			sum += dist[vs[i]][vs[i + 1]];
		}
		sc = sum;
	}

	int getScore() const {
		return sc;
	}

	int estFlip(int l, int r) {
		int av = vs[l - 1];
		int bv = vs[l];
		int cv = vs[r - 1];
		int dv = vs[r];
		int sum = 0;
		sum -= dist[av][bv];
		sum -= dist[bv][cv];
		sum -= dist[cv][dv];
		sum += dist[av][cv];
		sum += dist[cv][bv];
		sum += dist[bv][dv];
		return sum;
	}

	int estChange(int i, int v) {
		int l = i;
		int r = i;
		while (l > 1 && incds[rs[l - 1]][v]) --l;
		while (r < h && incds[rs[r]][v]) ++r;

		int sum = 0;
		REP(k, l, r + 1) sum -= dist[vs[k - 1]][vs[k]];
		sum += dist[vs[l - 1]][v];
		sum += dist[v][vs[r]];
		return sum;
	}

	void step(double tmp) {
		if (rnd(2)) {
			int l = rnd(h - 1) + 1;
			int r = rnd(h - 2) + 1;
			if (r >= l) ++r;
			if (l > r) swap(l, r);
			++r;

			int del = estFlip(l, r);
			if (isAccepted(del, tmp)) {
				reverse(rs.begin() + l, rs.begin() + r);
				reverse(vs.begin() + l, vs.begin() + r);
				sc += del;
			}
		} else {
			int i = rnd(h - 1) + 1;
			if (cds[rs[i]].size() <= 1) return;
			int j = rnd(cds[rs[i]].size());
			int v = cds[rs[i]][j];
			if (v == vs[i]) return;

			int del = estChange(i, v);
			if (isAccepted(del, tmp)) {
				int l = i;
				int r = i;
				while (l > 1 && incds[rs[l - 1]][v]) --l;
				while (r < h && incds[rs[r]][v]) ++r;
				REP(i, l, r) {
					vs[i] = v;
				}
				sc += del;
			}
		}
	}

	bool isAccepted(int del, double tmp) {
		return del <= 0 || (del <= 8 * tmp && rnd() < std::exp(-del / tmp) * (1ll << 32));
	}

	vector<int> get() const {
		return vs;
	}
};

vector<int> solve() {
	Helper helper;
	const int iterations = 1e8;
	const int limit = 2920000;

	double tmp_start = 1000;
	double tmp_finish = 0.1;
	double tmp = tmp_start;

	Timer timer;
	timer.set(limit);

	int z = 0;
#if ONLY_MY_ENVIR
	while (z < iterations) {
#else
	while (z % 4096 != 0 || !timer.elapsed()) {
#endif
		++z;
		if (z % 4096 == 0) {
#if ONLY_MY_ENVIR
			double ratio = double(z) / iterations;
#else
			double ratio = double(timer.getMicrosec()) / limit;
#endif
			tmp = tmp_start * std::pow(tmp_finish / tmp_start, ratio);
#if ONLY_MY_ENVIR
			if (z % 16384 == 0) {
				cerr << tmp << ' ' << helper.getScore() << endl;
			}
#endif
		}
		helper.step(tmp);
	}

#if !EVALUATE
	cerr << z << endl;
#endif

	auto out = helper.get();
	return out;
	}

void pre() {
	const int di[] = { 0, 1, 0, -1 };
	const int dj[] = { 1, 0, -1, 0 };

	int ver = 0;
	REP(i, 0, n) {
		REP(j, 0, n) {
			if (f[i][j] == '#') continue;
			vtop.push_back(PP(i, j));
			ptov[PP(i, j)] = ver;
			++ver;
		}
	}
	sv = ptov[PP(si, sj)];

	int x = vtop.size();
	vector<bool> isc(x);
	REP(v, 0, x) {
		int i = vtop[v].first;
		int j = vtop[v].second;

		bool a = false;
		bool b = false;
		REP(k, 0, 4) {
			int ni = i + di[k];
			int nj = j + dj[k];
			if (!Inside(ni, nj, n, n)) continue;
			if (f[ni][nj] == '#') continue;
			(k % 2 ? a : b) = true;
		}
		if (a && b) {
			isc[v] = true;
		}
	}

	// need to speed up
	g = WeightedGraph<int>(x);
	REP(v, 0, x) {
		int i = vtop[v].first;
		int j = vtop[v].second;
		REP(k, 0, 4) {
			int ni = i + di[k];
			int nj = j + dj[k];
			if (!Inside(ni, nj, n, n)) continue;
			if (f[ni][nj] == '#') continue;
			int nv = ptov[PP(ni, nj)];
			g.add(v, nv, f[ni][nj] - '0');
		}
	}

	REP(v, 0, x) {
		if (isc[v] || v == sv) {
			dist.emplace_back(dijkstra(g, v).distance);
		} else {
			dist.emplace_back(vector<int>());
		}
	}

	REP(v, 0, x) {
		int i = vtop[v].first;
		int j = vtop[v].second;
		vector<int> cd;
		int cnt = 0;
		if (isc[v] || v == sv) {
			cd.push_back(v);
		}
		REP(k, 0, 4) {
			int ci = i;
			int cj = j;
			while (true) {
				ci += di[k];
				cj += dj[k];
				if (!Inside(ci, cj, n, n)) break;
				if (f[ci][cj] == '#') break;
				int cv = ptov[PP(ci, cj)];
				if (!isc[cv]) continue;
				cd.push_back(cv);
			}
		}
		sortVector(cd);
		cds.push_back(move(cd));
	}
	sort(cds.begin(), cds.end(), [](const auto& a, const auto& b) {
		return a.size() < b.size();
		});

	auto isin = [x](const vector<int>& a, const vector<int>& b) -> bool {
		int j = 0;
		REP(i, 0, a.size()) {
			while (j < b.size() && b[j] < a[i]) {
				++j;
			}
			if (j >= b.size() || b[j] != a[i]) return false;
		}
		return true;
	};

	vector<vector<int>> cds2;
	cds2.push_back(vector<int>(1, sv));
	REP(i, 0, cds.size()) {
		bool ok = true;
		REP(j, 0, i) {
			if (isin(cds[j], cds[i])) {
				ok = false;
				break;
			}
		}
		if (ok) {
			cds2.push_back(cds[i]);
		}
	}
	cds = move(cds2);

	incds.resize(cds.size());
	REP(z, 0, cds.size()) {
		incds[z].resize(x);
		for (int nv : cds[z]) {
			incds[z][nv] = true;
		}
	}
}

string restore(const vector<int>&v) {
	assert(vtop[v.front()] == PP(si, sj));
	assert(vtop[v.back()] == PP(si, sj));

	string ret;
	REP(k, 1, v.size()) {
		int pv = v[k - 1];
		int nv = v[k];
		auto res = dijkstra(g, pv);
		auto path = reconstructPath(res.previous, nv);

		PP pp = vtop[path[0]];
		REP(z, 1, path.size()) {
			PP np = vtop[path[z]];
			if (pp.first == np.first) {
				if (np.second == pp.second + 1) {
					ret += 'R';
				} else if (np.second == pp.second - 1) {
					ret += 'L';
				} else {
					assert(false);
				}
			} else if (pp.second == np.second) {
				if (np.first == pp.first + 1) {
					ret += 'D';
				} else if (np.first == pp.first - 1) {
					ret += 'U';
				} else {
					assert(false);
				}
			} else {
				assert(false);
			}
			pp = np;
		}
	}
	return ret;
}

int main() {
	cin >> n >> si >> sj;
	REP(i, 0, n) cin >> f[i];
	pre();
	auto res = solve();
	cout << restore(res) << endl;
	return 0;
}

// Included Files: 
//   * VectorUtil.h
//   * Timer.h
//   * Timer.cpp
//   * Random.h
//   * Random.cpp
//   * Graph.h
//   * Graph.cpp
//   * Shortest.h
//   * GraphUtil.h
//   * GraphUtil.cpp
//   * Main.h
//   * Main.cpp
