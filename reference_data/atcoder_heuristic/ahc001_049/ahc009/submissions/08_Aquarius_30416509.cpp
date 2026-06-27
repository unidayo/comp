#ifndef TIMER_H
#define TIMER_H

#include <cstdint>

#ifdef ONLY_MY_ENVIR

#include <chrono>
class Timer {
	const std::chrono::time_point<std::chrono::system_clock> start;
	std::int64_t limit_microsec = 0;

public:
	Timer();
	void set(std::int64_t time_microsec);
	bool elapsed() const;
	std::int64_t getMicrosec() const;
};

#else

class Timer {
	static const std::uint64_t ClocksPerMicrosec;
	const std::uint64_t start_clocks;
	std::uint64_t limit_clocks;

	std::uint64_t getClocks() const;

public:
	Timer();
	void set(std::int64_t time_microsec);
	bool elapsed() const;
	std::int64_t getMicrosec() const;
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

void Timer::set(std::int64_t time_microsec) {
	limit_microsec += time_microsec;
}

bool Timer::elapsed() const {
	return getMicrosec() >= limit_microsec;
}

std::int64_t Timer::getMicrosec() const {
	return duration_cast<microseconds>(system_clock::now() - start).count();
}

#else

const std::uint64_t Timer::ClocksPerMicrosec = 2987;

std::uint64_t Timer::getClocks() const {
	unsigned int lo, hi;
	__asm__ volatile ("rdtsc" : "=a" (lo), "=d" (hi));
	return ((std::uint64_t)hi << 32) | lo;
}

Timer::Timer() 
	: start_clocks(getClocks()), limit_clocks(start_clocks) {
}

void Timer::set(std::int64_t time_microsec) {
	limit_clocks += time_microsec * ClocksPerMicrosec;
}

bool Timer::elapsed() const {
	return getClocks() >= limit_clocks;
}

std::int64_t Timer::getMicrosec() const {
	return (getClocks() - start_clocks) / ClocksPerMicrosec;
}

#endif

#ifndef RANDOM_H
#define RANDOM_H

#include <cassert>
#include <cstdint>
#include <utility>
#include <vector>

class Random {
	std::uint32_t seed_;

public:
	Random(std::uint32_t seed = 3141592653ul);

	std::uint32_t rnd();
	std::uint32_t rnd(std::uint32_t max);

	// [min, max) の一様乱数
	std::uint32_t rnd(std::uint32_t min, std::uint32_t max);

	template <class Container>
	auto sample(const Container& arr) -> decltype(arr[0]);
	template <class Container>
	void shuffle(Container& arr);

	template <class T>
	std::vector<T> generateVector(int size, std::uint32_t max);
	template <class T>
	std::vector<T> generateVector(int size, std::int32_t min, std::int32_t max);
};

template <class Container>
auto Random::sample(const Container& arr) -> decltype(arr[0]) {
	assert(!arr.empty());
	return arr[rnd(arr.size())];
}

template <class Container>
void Random::shuffle(Container& arr) {
	for (int i = std::size(arr); i > 1; --i) {
		std::swap(arr[i - 1], arr[rnd(i)]);
	}
}

template <class T>
std::vector<T> Random::generateVector(int size, std::uint32_t max) {
	std::vector<T> ret(size);
	for (int i = 0; i < size; ++i) {
		ret[i] = rnd(max);
	}
	return ret;
}

template <class T>
std::vector<T> Random::generateVector(int size, std::int32_t min, std::int32_t max) {
	assert(min < max);
	std::vector<T> ret(size);
	for (int i = 0; i < size; ++i) {
		ret[i] = min + (std::int64_t)rnd(std::int64_t(max) - min);
	}
	return ret;
}

#endif
#ifdef ONLY_MY_ENVIR
#include "Random.h"
#endif

Random::Random(std::uint32_t seed) :
	seed_(seed) {
	assert(seed != 0);
}

std::uint32_t Random::rnd() {
	seed_ ^= seed_ << 13;
	seed_ ^= seed_ >> 17;
	seed_ ^= seed_ << 15;
	return seed_;
}

std::uint32_t Random::rnd(std::uint32_t max) {
	return rnd() % max;
}

std::uint32_t Random::rnd(std::uint32_t min, std::uint32_t max) {
	assert(min < max);
	return min + rnd(max - min);
}

#ifndef __GRAPH_H__
#define __GRAPH_H__

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
	std::vector<List> ids_;
 	std::vector<Edge> edges_;

public:
	Graph() = default;
	Graph(Graph&&) = default;
	Graph(const Graph&) = default;
	Graph& operator=(Graph&&) = default;
	Graph& operator=(const Graph&) = default;

	explicit Graph(int size);

	int size() const;
	int edgeCount() const;
	void expand(int new_size);
	void reserve(int edge_count);
	
	void add(int tail, int head, bool directed = true);
	void add(const Edge& edge);
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
	Graph graph_;
	Weights<T> weights_;

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
	void expand(int new_size);
	void reserve(int edge_count);

	void add(int tail, int head, T&& weight, bool directed = true);
	void add(int tail, int head, const T& weight, bool directed = true);
	void add(const Graph::Edge& edge, const T& weight);

	const Graph& getGraph() const;
	const Weights<T>& getWeights() const;

	std::string toString() const;
	void debug() const;
};

template<class T>
WeightedGraph<T>::WeightedGraph(int size) 
	: graph_(size) {
}

template<class T>
WeightedGraph<T>::WeightedGraph(const Graph& graph, const T& weight)
	: graph_(graph), weights_(graph.edgeCount(), weight) {
}

template<class T>
WeightedGraph<T>::WeightedGraph(const Graph& graph, const Weights<T>& weights) 
	: graph_(graph), weights_(weights) {
	assert(graph.edgeCount() == weights.size());
}

template <class T>
int WeightedGraph<T>::size() const {
	return graph_.size();
}

template<class T>
int WeightedGraph<T>::edgeCount() const {
	return graph_.edgeCount();
}

template<class T>
void WeightedGraph<T>::expand(int new_size) {
	graph_.expand(new_size);
}

template<class T>
void WeightedGraph<T>::reserve(int edge_count) {
	graph_.reserve(edge_count);
	weights_.reserve(edge_count);
}

template<class T>
void WeightedGraph<T>::add(int tail, int head, T&& weight, bool directed) {
	if (!directed) {
		add(tail, head, std::move(weight), true);
		add(head, tail, std::move(weight), true);
		return;
	}

	graph_.add(tail, head);
	weights_.push_back(std::move(weight));
}

template<class T>
void WeightedGraph<T>::add(int tail, int head, const T& weight, bool directed) {
	if (!directed) {
		add(tail, head, weight, true);
		add(head, tail, weight, true);
		return;
	}

	graph_.add(tail, head);
	weights_.push_back(weight);
}

template<class T>
void WeightedGraph<T>::add(const Graph::Edge& edge, const T& weight) {
	add(edge.tail, edge.head, weight);
}

template<class T>
const Graph& WeightedGraph<T>::getGraph() const {
	return graph_;
}

template<class T>
const Weights<T>& WeightedGraph<T>::getWeights() const {
	return weights_;
}

template<class T>
std::string WeightedGraph<T>::toString() const {
	std::ostringstream stream;
	for (int tail = 0; tail < graph_.size(); ++tail) {
		stream << tail << ": [";
		for (auto id : graph_.from(tail)) {
			const auto& edge = graph_.getEdge(id);
			int head = edge.head;
			stream << ' ' << head << '(' << weights_[id] << ')';
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
	: ids_(size) {
}

int Graph::size() const {
	return ids_.size();
}

int Graph::edgeCount() const {
	return edges_.size();
}

void Graph::expand(int new_size) {
	assert(new_size >= size());
	ids_.resize(new_size);
}

void Graph::reserve(int edge_count) {
	edges_.reserve(edge_count);
}

void Graph::add(int tail, int head, bool directed) {
	if (!directed) {
		add(tail, head, true);
		add(head, tail, true);
		return;
	}

	ids_[tail].push_back(edges_.size());
	edges_.push_back(Edge(tail, head));
}

void Graph::add(const Edge& edge) {
	add(edge.tail, edge.head);
}

const Graph::List& Graph::from(int tail) const {
	assert(tail >= 0 && tail < size());
	return ids_[tail];
}

const Graph::Edge& Graph::getEdge(EdgeID id) const {
	assert(id >= 0 && id < edgeCount());
	return edges_[id];
}

const std::vector<Graph::Edge>& Graph::getEdges() const {
	return edges_;
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


#ifndef __SHORTEST_UTIL_H__
#define __SHORTEST_UTIL_H__

#include <vector>
#include <algorithm>

#ifdef ONLY_MY_ENVIR
#include "Graph.h"
#endif

template <class T>
struct DijkstraResult {
	std::vector<T> distance;
	std::vector<Graph::EdgeID> entrance_id;
};

/* entrance_id の配列からパスを復元 (辺 ID の列を返す) */
inline std::vector<Graph::EdgeID> reconstructPath(const Graph& graph, const std::vector<int>& entrance_id, int goal) {
	std::vector<Graph::EdgeID> path;

	int curr = goal;
	while (true) {
		auto id = entrance_id[curr];
		if (id == -1) break;	// TODO: 始点に -1 を使うことを明文化しよう
		path.push_back(id);
		curr = graph.getEdge(id).tail;
	}

	std::reverse(path.begin(), path.end());
	return path;
}

#endif

#ifndef __SHORTEST_H__
#define __SHORTEST_H__

/* updated: 2021-08-15 */

#ifdef ONLY_MY_ENVIR
#include "Graph.h"
#include "ShortestUtil.h"
#endif

#include <vector>
#include <queue>
#include <utility>
#include <limits>
#include <algorithm>

/* 復元用の配列の中身を辺IDにするかどうか */

template <class T>
struct BellmanFordResult {
	std::vector<T> distance;
	std::vector<Graph::EdgeID> entrance_id;

	// is_unbounded[v] == true のとき始点からの距離がいくらでも小さくなる
	// (この場合 distance と entrance_id の値は意味を持たない)
	std::vector<bool> is_unbounded;
};

template <class T>
DijkstraResult<T> dijkstra(const WeightedGraph<T>& wgraph, int start_vertex, T infinity = std::numeric_limits<T>::max());
template <class T>
DijkstraResult<T> dijkstra(const WeightedGraph<T>& wgraph, std::vector<int> start_vertices, T infinity = std::numeric_limits<T>::max());
template <class T>
BellmanFordResult<T> bellmanFord(const WeightedGraph<T>& wgraph, int start_vertex, T infinity = std::numeric_limits<T>::max());
template <class T>
BellmanFordResult<T> bellmanFord(const WeightedGraph<T>& wgraph, std::vector<int> start_vertices, T infinity = std::numeric_limits<T>::max());
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
	std::vector<Graph::EdgeID> entrance_id(size, -1);

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
			entrance_id[head] = id;
			que.push(State(dist_new, head));
		}
	}

	DijkstraResult<T> result;
	result.distance = std::move(distance);
	result.entrance_id = std::move(entrance_id);
	return result;
}

/* ベルマンフォード法により単一始点最短路を求める O(nm) */
/* 負閉路検出には別の関数を使う: hasNegativeCycle */
template <class T>
BellmanFordResult<T> bellmanFord(const WeightedGraph<T>& wgraph, int start_vertex, T infinity) {
	return bellmanFord(wgraph, std::vector<int>(1, start_vertex), infinity);
}

/* ベルマンフォード法により最短路を求める (複数始点版) */
template <class T>
BellmanFordResult<T> bellmanFord(const WeightedGraph<T>& wgraph, std::vector<int> start_vertices, T infinity) {
	const Graph& graph = wgraph.getGraph();
	const auto& costs = wgraph.getWeights();
	const int size = graph.size();

	std::vector<T> distance(size, infinity);
	std::vector<int> entrance_id(size, -1);
	std::vector<bool> is_unbounded(size, false);

	for (int start_vertex : start_vertices) {
		distance[start_vertex] = T();
	}

	// 始点からの距離を更新
	auto update = [size, infinity, &graph, &costs](std::vector<T>& distance, std::vector<int>& entrance_id) -> void {
		for (int tail = 0; tail < size; ++tail) {
			if (distance[tail] == infinity) continue;
			for (Graph::EdgeID id : graph.from(tail)) {
				const auto& edge = graph.getEdge(id);
				const int head = edge.head;
				const T cost = costs[id];

				if (distance[tail] + cost < distance[head]) {
					distance[head] = distance[tail] + cost;
					entrance_id[head] = id;
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
		update(distance, entrance_id);
	}

	detect(distance, is_unbounded);

	for (int z = 1; z < size; ++z) {
		propagate(is_unbounded);
	}

	BellmanFordResult<T> result;
	result.distance = std::move(distance);
	result.entrance_id = std::move(entrance_id);
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


#ifndef __BINARY_MATRIX_H__
#define __BINARY_MATRIX_H__

/* updated: 2020-07-28 */

#include <vector>
#include <functional>
#include <iostream>
#include <initializer_list>
#include <iterator>

class BinaryMatrixBoolReference {
	std::uint64_t& var_;
	std::uint64_t mask_;

public:
	BinaryMatrixBoolReference(const BinaryMatrixBoolReference&) = default;
	explicit BinaryMatrixBoolReference(std::uint64_t& var, int bit);

	BinaryMatrixBoolReference& operator=(bool value);
	BinaryMatrixBoolReference& operator|=(bool value);
	BinaryMatrixBoolReference& operator&=(bool value);
	BinaryMatrixBoolReference& operator^=(bool value);

	operator bool() const;

	void flip();
	void swapFor(BinaryMatrixBoolReference& other);
};

void swap(BinaryMatrixBoolReference& left, BinaryMatrixBoolReference& right);

class BinaryMatrixRowConstIterator {
	static const int Bits = 64;
	
public:
	using iterator_category = std::random_access_iterator_tag;
	using difference_type = std::int64_t;
	using value_type = bool;
	using reference = bool;
	using pointer = bool*;

protected:
	std::uint64_t* ptr_;
	std::int64_t offset_;

public:
	BinaryMatrixRowConstIterator() = default;
	explicit BinaryMatrixRowConstIterator(const std::uint64_t* ptr, std::int64_t offset);
	BinaryMatrixRowConstIterator(const BinaryMatrixRowConstIterator&) = default;
	BinaryMatrixRowConstIterator& operator=(const BinaryMatrixRowConstIterator&) = default;

	reference operator*() const;
	reference operator->() const;
	reference operator[](difference_type offset) const;

	BinaryMatrixRowConstIterator& operator++();
	BinaryMatrixRowConstIterator& operator--();
	BinaryMatrixRowConstIterator operator++(int);
	BinaryMatrixRowConstIterator operator--(int);

	BinaryMatrixRowConstIterator& operator+=(difference_type offset);
	BinaryMatrixRowConstIterator& operator-=(difference_type offset);

	static int compare(const BinaryMatrixRowConstIterator& left, const BinaryMatrixRowConstIterator& right);
};

bool operator==(const BinaryMatrixRowConstIterator& left, const BinaryMatrixRowConstIterator& right);
bool operator!=(const BinaryMatrixRowConstIterator& left, const BinaryMatrixRowConstIterator& right);
bool operator<(const BinaryMatrixRowConstIterator& left, const BinaryMatrixRowConstIterator& right);
bool operator<=(const BinaryMatrixRowConstIterator& left, const BinaryMatrixRowConstIterator& right);
bool operator>(const BinaryMatrixRowConstIterator& left, const BinaryMatrixRowConstIterator& right);
bool operator>=(const BinaryMatrixRowConstIterator& left, const BinaryMatrixRowConstIterator& right);

BinaryMatrixRowConstIterator operator+(const BinaryMatrixRowConstIterator& itr, int offset);
BinaryMatrixRowConstIterator operator+(int offset, const BinaryMatrixRowConstIterator& itr);
BinaryMatrixRowConstIterator operator-(const BinaryMatrixRowConstIterator& itr, int offset);


class BinaryMatrixRowIterator : public BinaryMatrixRowConstIterator {
	using Base = BinaryMatrixRowConstIterator;
	static const int Bits = 64;

public:
	using iterator_category = std::random_access_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = bool;
	using reference = BinaryMatrixBoolReference;
	using pointer = BinaryMatrixBoolReference*;

	BinaryMatrixRowIterator() = default;
	explicit BinaryMatrixRowIterator(std::uint64_t* ptr, std::int64_t offset);
	BinaryMatrixRowIterator(const BinaryMatrixRowIterator&) = default;
	BinaryMatrixRowIterator& operator=(const BinaryMatrixRowIterator&) = default;

	reference operator*() const;
	reference operator->() const;
	reference operator[](int offset) const;

	BinaryMatrixRowIterator& operator++();
	BinaryMatrixRowIterator& operator--();
	BinaryMatrixRowIterator operator++(int);
	BinaryMatrixRowIterator operator--(int);

	BinaryMatrixRowIterator& operator+=(difference_type offset);
	BinaryMatrixRowIterator& operator-=(difference_type offset);
};

BinaryMatrixRowIterator operator+(const BinaryMatrixRowIterator& itr, int offset);
BinaryMatrixRowIterator operator+(int offset, const BinaryMatrixRowIterator& itr);
BinaryMatrixRowIterator operator-(const BinaryMatrixRowIterator& itr, int offset);


class BinaryMatrixRow {
	using Reference = BinaryMatrixBoolReference;
	using Iterator = BinaryMatrixRowIterator;
	using ConstIterator = BinaryMatrixRowConstIterator;
	static const int Bits = 64;

	int size_ = 0;
	std::vector<std::uint64_t> arr_;

public:
	BinaryMatrixRow() = default;
	explicit BinaryMatrixRow(int size);
	BinaryMatrixRow(const BinaryMatrixRow&) = default;
	BinaryMatrixRow(BinaryMatrixRow&&) = default;

	BinaryMatrixRow& operator=(const BinaryMatrixRow&) = default;
	BinaryMatrixRow& operator=(BinaryMatrixRow&&) = default;

	BinaryMatrixRow& operator+=(const BinaryMatrixRow& right);
	
	Reference operator[](int index);
	bool operator[](int index) const;

	int size() const;
	void clear();
	void flip();

	Iterator begin();
	Iterator end();
	ConstIterator begin() const;
	ConstIterator end() const;

private:
	static int popcount(std::uint64_t value);

public:
	int count() const;

	static int compare(const BinaryMatrixRow& left, const BinaryMatrixRow& right);
};

bool operator==(const BinaryMatrixRow& left, const BinaryMatrixRow& right);
bool operator!=(const BinaryMatrixRow& left, const BinaryMatrixRow& right);
bool operator<(const BinaryMatrixRow& left, const BinaryMatrixRow& right);
bool operator<=(const BinaryMatrixRow& left, const BinaryMatrixRow& right);
bool operator>(const BinaryMatrixRow& left, const BinaryMatrixRow& right);
bool operator>=(const BinaryMatrixRow& left, const BinaryMatrixRow& right);
BinaryMatrixRow operator+(const BinaryMatrixRow& left, const BinaryMatrixRow& right);


class BinaryMatrix {
public:
	using Row = BinaryMatrixRow;
	using Iterator = std::vector<Row>::iterator;
	using ConstIterator = std::vector<Row>::const_iterator;

private:
	int height_ = 0;
	int width_ = 0;
	std::vector<Row> mat_;

public:
	BinaryMatrix() = default;
	explicit BinaryMatrix(int height, int width);
	explicit BinaryMatrix(int height, int width, const std::function<bool(int, int)>& func);
	explicit BinaryMatrix(const std::initializer_list<std::initializer_list<bool>>& lists);
	
	BinaryMatrix(const BinaryMatrix&) = default;
	BinaryMatrix(BinaryMatrix&&) = default;

	BinaryMatrix& operator=(const BinaryMatrix&) = default;
	BinaryMatrix& operator=(BinaryMatrix&&) = default;

	BinaryMatrixRow& operator[](int index);
	const BinaryMatrixRow& operator[](int index) const;

	const BinaryMatrix& operator+() const;
	const BinaryMatrix& operator-() const;

	BinaryMatrix& operator+=(const BinaryMatrix& right);
	BinaryMatrix& operator-=(const BinaryMatrix& right);
	BinaryMatrix& operator*=(const BinaryMatrix& right);
	BinaryMatrix& operator*=(bool scalar);

	Iterator begin();
	Iterator end();
	ConstIterator begin() const;
	ConstIterator end() const;

	int height() const;
	int width() const;
	bool isSquare() const;

	BinaryMatrix trans() const;
	BinaryMatrix inverse() const;
	bool determinant() const;
	int rank() const;
	BinaryMatrix block(int ai, int aj, int bi, int bj) const;

	static bool isAddable(const BinaryMatrix& left, const BinaryMatrix& right);
	static bool isMultipliable(const BinaryMatrix& left, const BinaryMatrix& right);

	static int compare(const BinaryMatrix& left, const BinaryMatrix& right);
	static BinaryMatrix multiply(const BinaryMatrix& left, const BinaryMatrix& right);

	static BinaryMatrix identity(int size);
	static BinaryMatrix joinHorizontally(const BinaryMatrix& left, const BinaryMatrix& right);
	static BinaryMatrix joinVertically(const BinaryMatrix& top, const BinaryMatrix& bottom);
};

bool operator==(const BinaryMatrix& left, const BinaryMatrix& right);
bool operator!=(const BinaryMatrix& left, const BinaryMatrix& right);
bool operator<(const BinaryMatrix& left, const BinaryMatrix& right);
bool operator<=(const BinaryMatrix& left, const BinaryMatrix& right);
bool operator>(const BinaryMatrix& left, const BinaryMatrix& right);
bool operator>=(const BinaryMatrix& left, const BinaryMatrix& right);

BinaryMatrix operator+(const BinaryMatrix& left, const BinaryMatrix& right);
BinaryMatrix operator-(const BinaryMatrix& left, const BinaryMatrix& right);
BinaryMatrix operator*(const BinaryMatrix& left, const BinaryMatrix& right);
BinaryMatrix operator*(const BinaryMatrix& matrix, bool scalar);
BinaryMatrix operator*(bool scalar, const BinaryMatrix& matrix);

std::ostream& operator<<(std::ostream& ost, const BinaryMatrix& matrix);

// TODO: std::rank との衝突を避ける (どうやら後ろに '<' があると衝突するらしい)
struct BinaryMatrixReductionInfo {
	BinaryMatrix matrix;
	int rank;
	bool determinant;
};

BinaryMatrixReductionInfo reduceByGauss(const BinaryMatrix& mat_arg, int row_max = -1);
BinaryMatrix solveLinearEquation(const BinaryMatrix& A, const BinaryMatrix& b);

BinaryMatrix inputBinaryMatrix(int height, int width, char zero = '0', char one = '1', std::istream& ist = std::cin);

#endif


#ifdef ONLY_MY_ENVIR
#include "BinaryMatrix.h"
#endif

#ifdef _MSC_VER
#include <intrin.h>
#endif

#include <algorithm>
#include <cassert>
#include <stdexcept>

/*-----------------------------------------*/
/*----   BinaryMatrixRowConstIterator   ---*/
/*-----------------------------------------*/

BinaryMatrixRowConstIterator::BinaryMatrixRowConstIterator(const std::uint64_t* ptr, std::int64_t offset)
	: ptr_(const_cast<std::uint64_t*>(ptr)), offset_(offset) {
}

BinaryMatrixRowConstIterator::reference BinaryMatrixRowConstIterator::operator*() const {
	int pos = offset_ / Bits;
	int bit = offset_ % Bits;
	return ptr_[pos] & ((std::uint64_t)1 << bit);
}

BinaryMatrixRowConstIterator::reference BinaryMatrixRowConstIterator::operator->() const {
	return *(*this);
}

BinaryMatrixRowConstIterator::reference BinaryMatrixRowConstIterator::operator[](difference_type offset) const {
	return *(*this + offset);
}

BinaryMatrixRowConstIterator& BinaryMatrixRowConstIterator::operator++() {
	++offset_;
	return *this;
}

BinaryMatrixRowConstIterator& BinaryMatrixRowConstIterator::operator--() {
	--offset_;
	return *this;
}

BinaryMatrixRowConstIterator BinaryMatrixRowConstIterator::operator++(int) {
	BinaryMatrixRowConstIterator preserved(*this);
	++(*this);
	return preserved;
}

BinaryMatrixRowConstIterator BinaryMatrixRowConstIterator::operator--(int) {
	BinaryMatrixRowConstIterator preserved(*this);
	--(*this);
	return preserved;
}

BinaryMatrixRowConstIterator& BinaryMatrixRowConstIterator::operator+=(difference_type offset) {
	offset_ += offset;
	return *this;
}

BinaryMatrixRowConstIterator& BinaryMatrixRowConstIterator::operator-=(difference_type offset) {
	offset_ -= offset;
	return *this;
}

int BinaryMatrixRowConstIterator::compare(const BinaryMatrixRowConstIterator& left, const BinaryMatrixRowConstIterator& right) {
	assert(left.ptr_ == right.ptr_);

	if (left.offset_ < right.offset_) return -1;
	if (left.offset_ > right.offset_) return 1;
	return 0;
}

bool operator==(const BinaryMatrixRowConstIterator& left, const BinaryMatrixRowConstIterator& right) {
	return BinaryMatrixRowConstIterator::compare(left, right) == 0;
}

bool operator!=(const BinaryMatrixRowConstIterator& left, const BinaryMatrixRowConstIterator& right) {
	return BinaryMatrixRowConstIterator::compare(left, right) != 0;
}

bool operator<(const BinaryMatrixRowConstIterator& left, const BinaryMatrixRowConstIterator& right) {
	return BinaryMatrixRowConstIterator::compare(left, right) < 0;
}

bool operator<=(const BinaryMatrixRowConstIterator& left, const BinaryMatrixRowConstIterator& right) {
	return BinaryMatrixRowConstIterator::compare(left, right) <= 0;
}

bool operator>(const BinaryMatrixRowConstIterator& left, const BinaryMatrixRowConstIterator& right) {
	return BinaryMatrixRowConstIterator::compare(left, right) > 0;
}

bool operator>=(const BinaryMatrixRowConstIterator& left, const BinaryMatrixRowConstIterator& right) {
	return BinaryMatrixRowConstIterator::compare(left, right) >= 0;
}

BinaryMatrixRowConstIterator operator+(const BinaryMatrixRowConstIterator& itr, int offset) {
	BinaryMatrixRowConstIterator ret(itr);
	ret += offset;
	return ret;
}

BinaryMatrixRowConstIterator operator+(int offset, const BinaryMatrixRowConstIterator& itr) {
	BinaryMatrixRowConstIterator ret(itr);
	ret += offset;
	return ret;
}

BinaryMatrixRowConstIterator operator-(const BinaryMatrixRowConstIterator& itr, int offset) {
	BinaryMatrixRowConstIterator ret(itr);
	ret -= offset;
	return ret;
}

/*-----------------------------------------*/
/*------   BinaryMatrixRowIterator   ------*/
/*-----------------------------------------*/

BinaryMatrixRowIterator::BinaryMatrixRowIterator(std::uint64_t* ptr, std::int64_t offset) 
	: Base(ptr, offset) {
}

BinaryMatrixRowIterator::reference BinaryMatrixRowIterator::operator*() const {
	int pos = offset_ / Bits;
	int bit = offset_ % Bits;
	return reference(ptr_[pos], bit);
}

BinaryMatrixRowIterator::reference BinaryMatrixRowIterator::operator->() const {
	return *(*this);
}

BinaryMatrixRowIterator::reference BinaryMatrixRowIterator::operator[](int offset) const {
	return *(*this + offset);
}

BinaryMatrixRowIterator& BinaryMatrixRowIterator::operator++() {
	Base::operator++();
	return *this;
}

BinaryMatrixRowIterator& BinaryMatrixRowIterator::operator--() {
	Base::operator--();
	return *this;
}

BinaryMatrixRowIterator BinaryMatrixRowIterator::operator++(int) {
	BinaryMatrixRowIterator preserved(*this);
	++(*this);
	return preserved;
}

BinaryMatrixRowIterator BinaryMatrixRowIterator::operator--(int) {
	BinaryMatrixRowIterator preserved(*this);
	--(*this);
	return preserved;
}

BinaryMatrixRowIterator& BinaryMatrixRowIterator::operator+=(difference_type offset) {
	Base::operator+=(offset);
	return *this;
}

BinaryMatrixRowIterator& BinaryMatrixRowIterator::operator-=(difference_type offset) {
	Base::operator-=(offset);
	return *this;
}

BinaryMatrixRowIterator operator+(const BinaryMatrixRowIterator& itr, int offset) {
	BinaryMatrixRowIterator ret(itr);
	ret += offset;
	return ret;
}

BinaryMatrixRowIterator operator+(int offset, const BinaryMatrixRowIterator& itr) {
	BinaryMatrixRowIterator ret(itr);
	ret += offset;
	return ret;
}

BinaryMatrixRowIterator operator-(const BinaryMatrixRowIterator& itr, int offset) {
	BinaryMatrixRowIterator ret(itr);
	ret -= offset;
	return ret;
}

/*-----------------------------------------*/
/*-----   BinaryMatrixBoolReference   -----*/
/*-----------------------------------------*/

BinaryMatrixBoolReference::BinaryMatrixBoolReference(std::uint64_t& var, int bit)
	: var_(var), mask_((std::uint64_t)1 << bit) {
}

BinaryMatrixBoolReference& BinaryMatrixBoolReference::operator=(bool value) {
	if (value) {
		var_ |= mask_;
	} else {
		var_ &= ~mask_;
	}
	return *this;
}

BinaryMatrixBoolReference& BinaryMatrixBoolReference::operator|=(bool value) {
	if (value) {
		var_ |= mask_;
	}
	return *this;
}

BinaryMatrixBoolReference& BinaryMatrixBoolReference::operator&=(bool value) {
	if (!value) {
		var_ &= ~mask_;
	}
	return *this;
}

BinaryMatrixBoolReference& BinaryMatrixBoolReference::operator^=(bool value) {
	if (value) {
		var_ ^= mask_;
	}
	return *this;
}

BinaryMatrixBoolReference::operator bool() const {
	return var_ & mask_;
}

void BinaryMatrixBoolReference::flip() {
	var_ ^= mask_;
}

void BinaryMatrixBoolReference::swapFor(BinaryMatrixBoolReference& other) {
	if (*this == other) return;
	flip();
	other.flip();
}

void swap(BinaryMatrixBoolReference& left, BinaryMatrixBoolReference& right) {
	left.swapFor(right);
}

/*-----------------------------------------*/
/*----------   BinaryMatrixRow   ----------*/
/*-----------------------------------------*/

BinaryMatrixRow::BinaryMatrixRow(int size) 
	: size_(size), arr_((size + Bits - 1) / Bits) {
}

BinaryMatrixRow& BinaryMatrixRow::operator+=(const BinaryMatrixRow& right) {
	assert(size_ == right.size_);

	for (int i = 0; i < arr_.size(); ++i) {
		arr_[i] ^= right.arr_[i];
	}
	return *this;
}

BinaryMatrixRow::Reference BinaryMatrixRow::operator[](int index) {
	int pos = index / Bits;
	int bit = index % Bits;
	return Reference(arr_[pos], bit);
}

bool BinaryMatrixRow::operator[](int index) const {
	int pos = index / Bits;
	int bit = index % Bits;
	return arr_[pos]& ((std::uint64_t)1 << bit);
}

int BinaryMatrixRow::size() const {
	return size_;
}

void BinaryMatrixRow::clear() {
	std::fill(arr_.begin(), arr_.end(), 0);
}

void BinaryMatrixRow::flip() {
	for (std::uint64_t& value : arr_) {
		value = ~value;
	}

	// 使用していない領域は 0 にする
	int rest_bit = size_ % Bits;
	if (rest_bit == 0) return;

	std::uint64_t mask = ((std::uint64_t)1 << rest_bit) - 1;
	arr_.back() &= mask;
}

BinaryMatrixRow::Iterator BinaryMatrixRow::begin() {
	return Iterator(arr_.data(), 0);
}

BinaryMatrixRow::Iterator BinaryMatrixRow::end() {
	return Iterator(arr_.data(), size_);
}

BinaryMatrixRow::ConstIterator BinaryMatrixRow::begin() const {
	return ConstIterator(arr_.data(), 0);
}

BinaryMatrixRow::ConstIterator BinaryMatrixRow::end() const {
	return ConstIterator(arr_.data(), size_);
}

int BinaryMatrixRow::popcount(std::uint64_t value) {
#ifdef _MSC_VER
	return __popcnt64(value);
#elif defined(__GNUC__)
	return __builtin_popcountll(value);
#else
	value = (value & 0x5555555555555555) + ((value >>  1) & 0x5555555555555555);
	value = (value & 0x3333333333333333) + ((value >>  2) & 0x3333333333333333);
	value = (value & 0x0f0f0f0f0f0f0f0f) + ((value >>  4) & 0x0f0f0f0f0f0f0f0f);
	value = (value & 0x00ff00ff00ff00ff) + ((value >>  8) & 0x00ff00ff00ff00ff);
	value = (value & 0x0000ffff0000ffff) + ((value >> 16) & 0x0000ffff0000ffff);
	value = (value & 0x00000000ffffffff) + ((value >> 32) & 0x00000000ffffffff);
	return value;
#endif
}

int BinaryMatrixRow::count() const {
	int sum = 0;
	for (std::int64_t value : arr_) {
		sum += popcount(value);
	}
	return sum;
}

/* 注意: 辞書順ではない(bit を下位から使用しているため) */
int BinaryMatrixRow::compare(const BinaryMatrixRow& left, const BinaryMatrixRow& right) {
	if (left.size_ < right.size_) return -1;
	if (left.size_ > right.size_) return 1;

	const int N = left.arr_.size();
	for (int i = 0; i < N; ++i) {
		if (left.arr_[i] < right.arr_[i]) return -1;
		if (left.arr_[i] > right.arr_[i]) return 1;
	}
	return 0;
}

bool operator==(const BinaryMatrixRow& left, const BinaryMatrixRow& right) {
	return BinaryMatrixRow::compare(left, right) == 0;
}

bool operator!=(const BinaryMatrixRow& left, const BinaryMatrixRow& right) {
	return BinaryMatrixRow::compare(left, right) != 0;
}

bool operator<(const BinaryMatrixRow& left, const BinaryMatrixRow& right) {
	return BinaryMatrixRow::compare(left, right) < 0;
}

bool operator<=(const BinaryMatrixRow& left, const BinaryMatrixRow& right) {
	return BinaryMatrixRow::compare(left, right) <= 0;
}

bool operator>(const BinaryMatrixRow& left, const BinaryMatrixRow& right) {
	return BinaryMatrixRow::compare(left, right) > 0;
}

bool operator>=(const BinaryMatrixRow& left, const BinaryMatrixRow& right) {
	return BinaryMatrixRow::compare(left, right) >= 0;
}

BinaryMatrixRow operator+(const BinaryMatrixRow& left, const BinaryMatrixRow& right) {
	BinaryMatrixRow ret(left);
	ret += right;
	return ret;
}

/*-----------------------------------------*/
/*------------   BinaryMatrix   -----------*/
/*-----------------------------------------*/

BinaryMatrix::BinaryMatrix(int height, int width) 
	: height_(height), width_(width), mat_(height, Row(width)) {
}

BinaryMatrix::BinaryMatrix(int height, int width, const std::function<bool(int, int)>& func)
	: BinaryMatrix(height, width) {

	for (int i = 0; i < height_; ++i) {
		for (int j = 0; j < width_; ++j) {
			mat_[i][j] = func(i, j);
		}
	}
}

BinaryMatrix::BinaryMatrix(const std::initializer_list<std::initializer_list<bool>>& lists) {
	int height = lists.size();
	int width = 0;
	for (const auto& list : lists) {
		width = std::max<int>(width, list.size());
	}

	*this = BinaryMatrix(height, width);

	int i = 0;
	for (const auto& list : lists) {
		int j = 0;
		for (bool value : list) {
			mat_[i][j] = value;
			++j;
		}
		++i;
	}
}

BinaryMatrixRow& BinaryMatrix::operator[](int index) {
	return mat_[index];
}

const BinaryMatrixRow& BinaryMatrix::operator[](int index) const {
	return mat_[index];
}

const BinaryMatrix& BinaryMatrix::operator+() const {
	return *this;
}

const BinaryMatrix& BinaryMatrix::operator-() const {
	return *this;
}

BinaryMatrix& BinaryMatrix::operator+=(const BinaryMatrix& right) {
	assert(isAddable(*this, right));
	
	for (int i = 0; i < height_; ++i) {
		mat_[i] += right.mat_[i];
	}
	return *this;
}

BinaryMatrix& BinaryMatrix::operator-=(const BinaryMatrix& right) {
	*this += right;
	return *this;
}

BinaryMatrix& BinaryMatrix::operator*=(const BinaryMatrix& right) {
	assert(isMultipliable(*this, right));

	*this = multiply(*this, right);
	return *this;
}

BinaryMatrix& BinaryMatrix::operator*=(bool scalar) {
	if (!scalar) {
		for (Row& row : mat_) {
			row.clear();
		}
	}
	return *this;
}

BinaryMatrix::Iterator BinaryMatrix::begin() {
	return mat_.begin();
}

BinaryMatrix::Iterator BinaryMatrix::end() {
	return mat_.end();
}

BinaryMatrix::ConstIterator BinaryMatrix::begin() const {
	return mat_.begin();
}

BinaryMatrix::ConstIterator BinaryMatrix::end() const {
	return mat_.end();
}

int BinaryMatrix::height() const {
	return height_;
}

int BinaryMatrix::width() const {
	return width_;
}

bool BinaryMatrix::isSquare() const {
	return height_ == width_;
}

BinaryMatrix BinaryMatrix::trans() const {
	BinaryMatrix ret(width_, height_);

	for (int i = 0; i < height_; ++i) {
		for (int j = 0; j < width_; ++j) {
			ret.mat_[j][i] = mat_[i][j];
		}
	}
	
	return ret;
}

/* これより計算量の良い方法はある？ */
BinaryMatrix BinaryMatrix::inverse() const {
	assert(isSquare());

	const int size = height_;
	BinaryMatrix extended = joinHorizontally(*this, BinaryMatrix::identity(size));
	const auto& res = reduceByGauss(extended, size);

	// std::rank との衝突を避ける
	if ((res.rank) < size) throw std::runtime_error("matrix not invertible");
	return res.matrix.block(0, size, size, 2 * size);
}

bool BinaryMatrix::determinant() const {
	assert(isSquare());

	const auto& res = reduceByGauss(*this);
	return res.determinant;
}

int BinaryMatrix::rank() const {
	const auto& res = reduceByGauss(*this);
	return res.rank;
}

/* 部分行列 [ai, bi) × [aj, bj) を構築, 引数の順序に注意 */
/* 要高速化 */
BinaryMatrix BinaryMatrix::block(int ai, int aj, int bi, int bj) const {
	assert(ai >= 0 && ai < height_ && aj >= 0 && aj < width_);
	assert(bi >= 0 && bi <= height_ && bj >= 0 && bj <= width_);
	assert(ai <= bi && aj <= bj);

	BinaryMatrix ret(bi - ai, bj - aj);
	for (int i = ai; i < bi; ++i) {
		for (int j = aj; j < bj; ++j) {
			ret.mat_[i - ai][j - aj] = mat_[i][j];
		}
	}
	return ret;
}

bool BinaryMatrix::isAddable(const BinaryMatrix& left, const BinaryMatrix& right) {
	return left.height_ == right.height_ && left.width_ == right.width_;
}

bool BinaryMatrix::isMultipliable(const BinaryMatrix& left, const BinaryMatrix& right) {
	return left.width_ == right.height_;
}

/* 注意: 辞書順ではない(bit を下位から使用しているため) */
int BinaryMatrix::compare(const BinaryMatrix& left, const BinaryMatrix& right) {
	if (left.height_ < right.height_) return -1;
	if (left.height_ > right.height_) return 1;
	if (left.width_ < right.width_) return -1;
	if (left.width_ > right.width_) return 1;

	const int M = left.height_;
	for (int i = 0; i < M; ++i) {
		int res_comp = BinaryMatrixRow::compare(left.mat_[i], right.mat_[i]);
		if (res_comp != 0) return res_comp;
	}
	return 0;
}

BinaryMatrix BinaryMatrix::multiply(const BinaryMatrix& left, const BinaryMatrix& right) {
	assert(isMultipliable(left, right));
	
	const int L = left.height_;
	const int M = left.width_;
	const int N = right.width_;

	BinaryMatrix ret(L, N);

	const BinaryMatrix tr = right.trans();
	for (int i = 0; i < L; ++i) {
		for (int j = 0; j < N; ++j) {
			ret.mat_[i][j] = (left.mat_[i] + right.mat_[j]).count() % 2;
		}
	}

	return ret;
}

BinaryMatrix BinaryMatrix::identity(int size) {
	BinaryMatrix ret(size, size);

	for (int i = 0; i < size; ++i) {
		ret.mat_[i][i] = 1;
	}
	return ret;
}

/* 要高速化 */
BinaryMatrix BinaryMatrix::joinHorizontally(const BinaryMatrix& left, const BinaryMatrix& right) {
	assert(left.height_ == right.height_);

	BinaryMatrix ret(left.height_, left.width_ + right.width_);

	for (int i = 0; i < left.height_; ++i) {
		for (int j = 0; j < left.width_; ++j) {
			ret.mat_[i][j] = left.mat_[i][j];
		}
	}

	for (int i = 0; i < right.height_; ++i) {
		for (int j = 0; j < right.width_; ++j) {
			ret.mat_[i][j + left.width_] = right.mat_[i][j];
		}
	}

	return ret;
}

BinaryMatrix BinaryMatrix::joinVertically(const BinaryMatrix& top, const BinaryMatrix& bottom) {
	assert(top.width_ == bottom.width_);

	BinaryMatrix ret(top.height_ + bottom.height_, top.width_);

	for (int i = 0; i < top.height_; ++i) {
		ret.mat_[i] = top.mat_[i];
	}

	for (int i = 0; i < bottom.height_; ++i) {
		ret.mat_[i + top.height_] = bottom.mat_[i];
	}

	return ret;
}

bool operator==(const BinaryMatrix& left, const BinaryMatrix& right) {
	return BinaryMatrix::compare(left, right) == 0;
}

bool operator!=(const BinaryMatrix& left, const BinaryMatrix& right) {
	return BinaryMatrix::compare(left, right) != 0;
}

bool operator<(const BinaryMatrix& left, const BinaryMatrix& right) {
	return BinaryMatrix::compare(left, right) < 0;
}

bool operator<=(const BinaryMatrix& left, const BinaryMatrix& right) {
	return BinaryMatrix::compare(left, right) <= 0;
}

bool operator>(const BinaryMatrix& left, const BinaryMatrix& right) {
	return BinaryMatrix::compare(left, right) > 0;
}

bool operator>=(const BinaryMatrix& left, const BinaryMatrix& right) {
	return BinaryMatrix::compare(left, right) >= 0;
}

BinaryMatrix operator+(const BinaryMatrix& left, const BinaryMatrix& right) {
	BinaryMatrix ret(left);
	ret += right;
	return ret;
}

BinaryMatrix operator-(const BinaryMatrix& left, const BinaryMatrix& right) {
	BinaryMatrix ret(left);
	ret -= right;
	return ret;
}

BinaryMatrix operator*(const BinaryMatrix& left, const BinaryMatrix& right) {
	return BinaryMatrix::multiply(left, right);
}

BinaryMatrix operator*(const BinaryMatrix& matrix, bool scalar) {
	BinaryMatrix ret(matrix);
	ret *= scalar;
	return ret;
}

BinaryMatrix operator*(bool scalar, const BinaryMatrix& matrix) {
	BinaryMatrix ret(matrix);
	ret *= scalar;
	return ret;
}

std::ostream& operator<<(std::ostream& ost, const BinaryMatrix& matrix) {
	for (int i = 0; i < matrix.height(); ++i) {
		for (int j = 0; j < matrix.width(); ++j) {
			ost << matrix[i][j];
			if (j + 1 == matrix.width()) {
				ost << std::endl;
			} else {
				ost << ' ';
			}
		}
	}
	return ost;
}


BinaryMatrixReductionInfo reduceByGauss(const BinaryMatrix& mat_arg, int col_max) {
	BinaryMatrix mat = mat_arg;
	if (col_max == -1) {
		col_max = mat.width();
	}

	const int M = mat.height();
	const int N = mat.width();

	int rank = 0;
	for (int col = 0; col < col_max; ++col) {
		/* ピボット選択 */
		int pivot_i = -1;
		for (int i = rank; i < M; ++i) {
			if (mat[i][col]) {
				pivot_i = i;
				break;
			}
		}

		/* 列 col 内の値がすべて 0 なら飛ばす */
		if (pivot_i == -1) continue;

		/* 入れ替え */
		if (pivot_i != rank) {
			using std::swap;
			swap(mat[pivot_i], mat[rank]);		// 実装注意
		}

		/* 基本変形 */
		for (int i = 0; i < M; ++i) {
			if (i == rank) continue;
			if (!mat[i][col]) continue;

			mat[i] += mat[rank];
		}
		++rank;
	}

	BinaryMatrixReductionInfo ret;
	ret.matrix = std::move(mat);
	ret.rank = rank;
	ret.determinant = (rank == M);
	return ret;
}

/* 連立方程式 Ax = b の解を求める; 特殊なケースのみ(他の場合は reduceByGauss をそのまま使う) */
BinaryMatrix solveLinearEquation(const BinaryMatrix& A, const BinaryMatrix& b) {
	assert(A.isSquare());
	assert(A.height() == b.height());
	assert(b.width() == 1);

	const int size = A.height();
	const BinaryMatrix extended = BinaryMatrix::joinHorizontally(A, b);
	const auto& res = reduceByGauss(extended, size);

	// std::rank との衝突を避ける
	if ((res.rank) < size) throw std::runtime_error("matrix not invertible");
	return res.matrix.block(0, size, size, size + 1);
}

BinaryMatrix inputBinaryMatrix(int height, int width, char zero, char one, std::istream& ist) {
	BinaryMatrix ret(height, width);
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			char buffer;
			ist >> buffer;

			if (buffer == zero) {
				ret[i][j] = false;
			} else if (buffer == one) {
				ret[i][j] = true;
			} else {
				throw std::runtime_error("unexpected character");
			}
		}
	}
	return ret;
}

#ifndef __MAIN_H__
#define __MAIN_H__

#ifndef ONLY_MY_ENVIR
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
#endif

#ifdef ONLY_MY_ENVIR
#include "Includes.h"
#endif

using namespace std;
using uint = std::uint32_t;
using LL = std::int64_t;
using ULL = std::uint64_t;
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

#endif
#ifdef ONLY_MY_ENVIR
#include "Main.h"
#endif

#ifdef ONLY_MY_ENVIR
ifstream input_stream("ain.txt");
ofstream output_stream("aout.txt");
#define cin input_stream
#define cout output_stream
#endif

constexpr int n = 20;
constexpr int q = 200;
constexpr int U = 0;
constexpr int L = 1;
constexpr int D = 2;
constexpr int R = 3;
constexpr int di[] = { -1, 0, 1, 0 };
constexpr int dj[] = { 0, -1, 0, 1 };

constexpr int dir(char c) {
	if (c == 'U') return 0;
	if (c == 'L') return 1;
	if (c == 'D') return 2;
	if (c == 'R') return 3;
	return -1;
}

constexpr char chr(int k) {
	return "ULDR"[k];
}

struct InputData {
	int si;
	int sj;
	int ti;
	int tj;
	double p;
	array<array<array<bool, 4>, n>, n> f = {};
};

InputData input() {
	InputData in;
	cin >> in.si >> in.sj >> in.ti >> in.tj >> in.p;
	auto h = inputBinaryMatrix(n, n - 1, '0', '1', cin);
	auto v = inputBinaryMatrix(n - 1, n, '0', '1', cin);

	REP(i, 0, n) {
		REP(j, 0, n - 1) {
			if (h[i][j]) {
				in.f[i][j][R] = true;
				in.f[i][j + 1][L] = true;
			}
		}
	}

	REP(i, 0, n - 1) {
		REP(j, 0, n) {
			if (v[i][j]) {
				in.f[i][j][D] = true;
				in.f[i + 1][j][U] = true;
			}
		}
	}

	REP(j, 0, n) {
		in.f[0][j][U] = true;
		in.f[n - 1][j][D] = true;
	}
	REP(i, 0, n) {
		in.f[i][0][L] = true;
		in.f[i][n - 1][R] = true;
	}
	return in;
}

pair<string, LL> solveRnd(const InputData& in) {
	string s;
	Random rnd;
	REP(i, 0, q) {
		s += "ULDR"[rnd.rnd(4)];
	}
	return { s, 0 };
}

LL judge(const InputData& in, const string& s) {
	double prob[n][n] = {};
	prob[in.si][in.sj] = 1.0;

	double score = 0.0;
	REP(z, 0, s.size()) {
		int k = dir(s[z]);
		if (k == U) {
			REP(i, 0, n) {
				REP(j, 0, n) {
					if (!in.f[i][j][k]) {
						double del = (1 - in.p) * prob[i][j];
						prob[i - 1][j] += del;
						prob[i][j] -= del;
					}
				}
			}
		} else if (k == D) {
			REM(i, 0, n) {
				REP(j, 0, n) {
					if (!in.f[i][j][k]) {
						double del = (1 - in.p) * prob[i][j];
						prob[i + 1][j] += del;
						prob[i][j] -= del;
					}
				}
			}
		} else if (k == L) {
			REP(i, 0, n) {
				REP(j, 0, n) {
					if (!in.f[i][j][k]) {
						double del = (1 - in.p) * prob[i][j];
						prob[i][j - 1] += del;
						prob[i][j] -= del;
					}
				}
			}
		} else if (k == R) {
			REP(i, 0, n) {
				REM(j, 0, n) {
					if (!in.f[i][j][k]) {
						double del = (1 - in.p) * prob[i][j];
						prob[i][j + 1] += del;
						prob[i][j] -= del;
					}
				}
			}
		} else {
			assert(false);
		}
		score += (400 - z) * prob[in.ti][in.tj];
		prob[in.ti][in.tj] = 0.0;
	}
	return LL(std::round(250000.0 * score));
}

pair<string, LL> solve(const InputData& in) {
	auto ver = [](int k, int i, int j) -> int {
		assert(k >= 0 && k <= 4);
		assert(i >= 0 && i < n);
		assert(j >= 0 && j < n);
		return k * n * n + i * n + j;
	};

	WeightedGraph<int> g(5 * n * n);
	REP(ai, 0, n) {
		REP(aj, 0, n) {
			REP(k, 0, 4) {
				g.add(ver(4, ai, aj), ver(k, ai, aj), 0);
				if (in.f[ai][aj][k]) {
					g.add(ver(k, ai, aj), ver(4, ai, aj), 0);
				} else {
					int bi = ai + di[k];
					int bj = aj + dj[k];
					g.add(ver(k, ai, aj), ver(k, bi, bj), 1);
				}
			}
		}
	}

	int vs = ver(4, in.si, in.sj);
	auto res = dijkstra(g, vs, INF);
	int mnvt = -1;
	{
		int mn = q + 1;
		REP(k, 0, 4) {
			int vt = ver(k, in.ti, in.tj);
			if (res.distance[vt] < mn) {
				mn = res.distance[vt];
				mnvt = vt;
			}
		}
	}

	if (mnvt == -1) return solveRnd(in);
	auto path = reconstructPath(g.getGraph(), res.entrance_id, mnvt);

	string tmp;
	for (int id : path) {
		auto e = g.getGraph().getEdge(id);
		int a = e.tail % (n * n);
		int b = e.head % (n * n);
		int ai = a / n;
		int aj = a % n;
		int bi = b / n;
		int bj = b % n;

		char c = 0;
		if (ai < bi) c = 'D';
		if (ai > bi) c = 'U';
		if (aj < bj) c = 'R';
		if (aj > bj) c = 'L';
		if (c == 0) continue;
		REP(z, 0, g.getWeights()[id]) {
			tmp += c;
		}
	}

	string ans;
	REP(i, 0, q) {
		ans += tmp[i * tmp.size() / q];
	}
	return { ans, judge(in, ans) };
}

class Helper {
	const InputData& in_;
	Random rnd_;
	string s_;
	double score_ = 0;
	double pscore_ = 0;

public:
	Helper(const InputData& in, string init) 
		: in_(in)
		, s_(init)
	{
		score_ = judge(in_, s_);
	}

	void solve() {
		const int iterations = 1e5;
		const int limit = 1800000;
		const double tmp_start = 1e5;
		const double tmp_finish = 1e1;
		double tmp = tmp_start;

		Timer timer;
		timer.set(limit);

		int z = 0;
#if ONLY_MY_ENVIR
		while (z < iterations) {
#else
		while (z % 1024 != 0 || !timer.elapsed()) {
#endif
			++z;
			if (z % 1024 == 0) {
#if ONLY_MY_ENVIR
				double ratio = double(z) / iterations;
#else
				double ratio = double(timer.getMicrosec()) / limit;
#endif
				tmp = tmp_start * std::pow(tmp_finish / tmp_start, ratio);
			}

			if (z % 1024 == 0) {
				cerr << tmp << ' ' << score_ << endl;
			}

			double delta = step();
			if (!isAccepted(delta, tmp)) {
				undo();
			}
		}
		cerr << z << endl;
		return;
	}

	bool isAccepted(double del, double tmp) {
		return del >= 0 || (del >= -8 * tmp && rnd_.rnd() < std::exp(del / tmp) * (1ll << 32));
	}

	int mt = 0;
	int mi = 0;
	char mc = 0;
	double step() {
		mi = rnd_.rnd(q);
		int k = rnd_.rnd(4);
		
		mt = rnd_.rnd(2);
		if (mt == 0) {
			mc = s_.back();
			s_.pop_back();
			s_.insert(s_.begin() + mi, chr(k));
		} else {
			std::rotate(s_.begin() + mi, s_.begin() + mi + 1, s_.end());
		}

		pscore_ = score_;
		score_ = judge(in_, s_);
		return score_ - pscore_;
	}

	void undo() {
		score_ = pscore_;

		if (mt == 0) {
			s_.erase(s_.begin() + mi);
			s_.push_back(mc);
		} else {
			std::rotate(s_.begin() + mi, s_.end() - 1, s_.end());
		}
	}

	pair<string, LL> get() {
		return { s_, score_ };
	}
};

pair<string, LL> solveAnneal(const InputData& in) {
	auto ans = solve(in).first;
	Helper helper(in, ans);
	helper.solve();
	return helper.get();
}

int main() {
	auto in = input();
	auto out = solveAnneal(in);
	cout << out.first << endl;
	cerr << out.second << endl;
	return 0;
}

// Included Files: 
//   * Timer.h
//   * Timer.cpp
//   * Random.h
//   * Random.cpp
//   * Graph.h
//   * Graph.cpp
//   * ShortestUtil.h
//   * Shortest.h
//   * BinaryMatrix.h
//   * BinaryMatrix.cpp
//   * Main.h
//   * Main.cpp
