#ifndef __UNION_FIND_H__
#define __UNION_FIND_H__

/* updated: 2020-07-24 */

#include <vector>
#include <cstdint>

class UnionFindTree {
	int size_ = 0;						// 頂点数
	mutable std::vector<int> parent_;	// 親の添字, 根である場合は自身の添字を入れる
	std::vector<int> card_;			// 部分木のサイズ
	int group_count_ = 0;				// 現在のグループ数

public:
	UnionFindTree() = default;
	explicit UnionFindTree(int size);

	void initialize(int size);

private:
	bool isValidIndex(int i) const;

public:
	int rootOf(int i) const;
	bool isSame(int i, int j) const;
	void unite(int i, int j);

	int size() const;
	int cardinalityOf(int i) const;
	int groupCount() const;
	std::vector<std::vector<int>> toGroups() const;
};

class WeightedUnionFindTree {
	const int size_;
	mutable std::vector<int> parent_;
	mutable std::vector<std::uint64_t> diff_;	// diff_[i] : i の親ノード p との間の重み差 (weight[i] - weight[p] = diff_[i]) 
	std::vector<int> card_;				// 部分木のサイズ
	int group_count_;					// 現在のグループ数

public:
	explicit WeightedUnionFindTree(int size);

private:
	bool isValidIndex(int i) const;

public:
	int rootOf(int i) const;
	bool isSame(int i, int j) const;
	std::int64_t query(int i, int j) const;
	void unite(int i, int j, std::int64_t diff);

private:
	std::int64_t weight(int i) const;

public:
	int size() const;
	int cardinalityOf(int i) const;
	int groupCount() const;
};

#endif

#if ONLY_MY_ENVIR
#include "UnionFind.h"
#endif

#include <cassert>
#include <utility>
#include <algorithm>

UnionFindTree::UnionFindTree(int size) {
	initialize(size);
}

void UnionFindTree::initialize(int size) {
	parent_.clear();
	card_.clear();

	size_ = size;
	parent_.resize(size);
	card_.resize(size, 1);
	group_count_ = size;
	
	for (int i = 0; i < size; ++i) {
		parent_[i] = i;
	}
}

bool UnionFindTree::isValidIndex(int i) const {
	return i >= 0 && i < size();
}

int UnionFindTree::rootOf(int i) const {
	assert(isValidIndex(i));

	if (parent_[i] == i) return i;
	return parent_[i] = rootOf(parent_[i]);
}

bool UnionFindTree::isSame(int i, int j) const {
	assert(isValidIndex(i) && isValidIndex(j));

	int ri = rootOf(i);
	int rj = rootOf(j);
	return ri == rj;
}

void UnionFindTree::unite(int i, int j) {
	assert(isValidIndex(i) && isValidIndex(j));

	int ri = rootOf(i);
	int rj = rootOf(j);

	if (ri == rj) return;
	if (card_[ri] < card_[rj]) {
		std::swap(ri, rj);
	}

	// assert(card_[ri] >= card_[rj]);
	parent_[rj] = ri;
	card_[ri] += card_[rj];

	--group_count_;
}

int UnionFindTree::size() const {
	return size_;
}

/* 頂点 i の属する集合のサイズ */
int UnionFindTree::cardinalityOf(int i) const {
	assert(isValidIndex(i));

	int ri = rootOf(i);
	return card_[ri];
}

int UnionFindTree::groupCount() const {
	return group_count_;
}

std::vector<std::vector<int>> UnionFindTree::toGroups() const {
	std::vector<std::vector<int>> groups(size_);

	for (int i = 0; i < size_; ++i) {
		int ri = rootOf(i);
		groups[ri].push_back(i);
	}

	std::vector<std::vector<int>> ret;
	for (int i = 0; i < size_; ++i) {
		if (groups[i].empty()) continue;
		ret.push_back(groups[i]);
	}
	return ret;
}

WeightedUnionFindTree::WeightedUnionFindTree(int size)
	: size_(size), parent_(size), diff_(size), card_(size, 1), group_count_(size) {
	for (int i = 0; i < size; ++i) {
		parent_[i] = i;
	}
}

bool WeightedUnionFindTree::isValidIndex(int i) const {
	return i >= 0 && i < size();
}

int WeightedUnionFindTree::rootOf(int i) const {
	assert(isValidIndex(i));

	if (parent_[i] == i) return i;
	
	diff_[i] += weight(parent_[i]);
	return parent_[i] = rootOf(parent_[i]);
}

bool WeightedUnionFindTree::isSame(int i, int j) const {
	assert(isValidIndex(i) && isValidIndex(j));

	return rootOf(i) == rootOf(j);
}

// weight[j] - weight[i] を返す
std::int64_t WeightedUnionFindTree::query(int i, int j) const {
	assert(isValidIndex(i) && isValidIndex(j));

	return weight(j) - weight(i);
}

void WeightedUnionFindTree::unite(int i, int j, std::int64_t diff) {
	assert(isValidIndex(i) && isValidIndex(j));

	std::int64_t diff_roots = diff + weight(i) - weight(j);
	int ri = rootOf(i);
	int rj = rootOf(j);
	if (ri == rj) return;

	if (card_[ri] < card_[rj]) {
		std::swap(ri, rj);
		diff_roots = -diff_roots;
	}

	// assert(card_[ri] >= card_[rj]);
	parent_[rj] = ri;
	card_[ri] += card_[rj];
	diff_[rj] = diff_roots;
	--group_count_;

	return;
}

std::int64_t WeightedUnionFindTree::weight(int i) const {
	rootOf(i);		// 再計算のため
	return diff_[i];
}

int WeightedUnionFindTree::size() const {
	return size_;
}

int WeightedUnionFindTree::cardinalityOf(int i) const {
	assert(isValidIndex(i));

	int ri = rootOf(i);
	return card_[ri];
}

int WeightedUnionFindTree::groupCount() const {
	return group_count_;
}
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

	template <class Container>
	void shuffle(Container& arr);

	template <class T>
	std::vector<T> generateVector(int size, std::uint32_t max);
	template <class T>
	std::vector<T> generateVector(int size, std::int32_t min, std::int32_t max);
};

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


#ifndef __SPANNING_TREE_H__
#define __SPANNING_TREE_H__

/* updated: 2020-07-24 */

// required: Graph, UnionFind

#ifdef ONLY_MY_ENVIR
#include "Graph.h"
#include "UnionFind.h"
#endif

#include <queue>
#include <algorithm>
#include <numeric>

template <class T>
std::vector<Graph::EdgeID> kruskal(const WeightedGraph<T>& wgraph);
template <class T>
std::vector<Graph::EdgeID> prim(const WeightedGraph<T>& wgraph, int root);
template <class T>
std::vector<Graph::EdgeID> boruvka(const WeightedGraph<T>& wgraph);

/* クラスカル法で最小全域木を求める (入力は無向グラフとみなす) O(m log m) */
/*  最小全域木を構成する辺(単方向)のリストを返す */
template <class T>
std::vector<Graph::EdgeID> kruskal(const WeightedGraph<T>& wgraph) {
	using EdgeID = Graph::EdgeID;
	const Graph& graph = wgraph.getGraph();
	const auto& costs = wgraph.getWeights();
	std::vector<EdgeID> ids;

	const int size = graph.size();
	for (int tail = 0; tail < size; ++tail) {
		for (EdgeID id : graph.from(tail)) {
			ids.push_back(id);
		}
	}

	auto compareByCost = [&costs](EdgeID a, EdgeID b) -> bool {
		return costs[a] < costs[b];
	};

	std::sort(ids.begin(), ids.end(), compareByCost);

	std::vector<EdgeID> ret;
	UnionFindTree uf_tree(size);
	for (EdgeID id : ids) {
		const auto& edge = graph.getEdge(id);
		int tail = edge.tail;
		int head = edge.head;

		if (uf_tree.isSame(tail, head)) continue;
		uf_tree.unite(tail, head);

		ret.push_back(id);
	}
	return ret;
}

/* プリム法で最小全域木を求める (入力は無向グラフを仮定) O(m log n) */
/* 最小全域木を構成する辺(単方向)のリストを返す */
template <class T>
std::vector<Graph::EdgeID> prim(const WeightedGraph<T>& wgraph, int root) {
	using EdgeID = Graph::EdgeID;
	const Graph& graph = wgraph.getGraph();
	const auto& costs = wgraph.getWeights();

	// 比較関数(コスト小の辺から処理, priority_queue に使用するので不等号の向きに注意)
	auto compareByCost = [&costs](EdgeID a, EdgeID b) -> bool {
		return costs[a] > costs[b];
	};

	const int size = graph.size();
	std::vector<bool> visited(size, false);
	std::priority_queue<EdgeID, std::vector<EdgeID>, decltype(compareByCost)> que(compareByCost);

	visited[root] = true;
	for (EdgeID id : graph.from(root)) {
		que.push(id);
	}

	std::vector<EdgeID> ret;
	while (!que.empty()) {
		const EdgeID id = que.top();
		que.pop();

		const auto& edge = graph.getEdge(id);
		const int tail = edge.head;

		if (visited[tail]) continue;
		visited[tail] = true;
		ret.push_back(id);

		for (Graph::EdgeID id_next : graph.from(tail)) {
			const auto& edge = graph.getEdge(id_next);
			const int head = edge.head;
			if (visited[head]) continue;
			que.push(id_next);
		}
	}

	return ret;
}

/* ブルーフカ法で最小全域木を求める (入力は無向グラフを仮定) O(m log n) */
/* 最小全域木を構成する辺(単方向)のリストを返す */
template <class T>
std::vector<Graph::EdgeID> boruvka(const WeightedGraph<T>& wgraph) {
	using EdgeID = Graph::EdgeID;
	const Graph& graph = wgraph.getGraph();
	const auto& costs = wgraph.getWeights();
	const int size = graph.size();

	auto update = [size, &graph, &costs](UnionFindTree& uf_tree, std::vector<EdgeID>& ret) -> bool {
		// 各グループ (uf_tree の連結成分) に対し最適辺 (他グループに伸びる辺でコスト最小) を保持
		constexpr EdgeID Undefined = -1;
		std::vector<EdgeID> best_ids(size, Undefined);

		/* 最適辺を探索 */
		for (int tail = 0; tail < size; ++tail) {
			const int group = uf_tree.rootOf(tail);
			for (EdgeID id : graph.from(tail)) {
				const auto& edge = graph.getEdge(id);
				const int head = edge.head;
				if (uf_tree.isSame(tail, head)) continue;

				// コストが小さければ更新
				const T cost = costs[id];
				if (best_ids[group] == Undefined || cost < costs[best_ids[group]]) {
					best_ids[group] = id;
				}
			}
		}

		/* 連結成分を併合 */
		bool updated = false;
		for (EdgeID id : best_ids) {
			if (id == Undefined) continue;
			const auto& edge = graph.getEdge(id);
			const int tail = edge.tail;
			const int head = edge.head;

			if (uf_tree.isSame(tail, head)) continue;
			uf_tree.unite(tail, head);
			ret.push_back(id);
			updated = true;
		}
		return updated;
	};

	UnionFindTree uf_tree(size);
	std::vector<EdgeID> ret;

	// 更新が行われなくなるまで反復
	while (update(uf_tree, ret))
		;

	return ret;
}

#endif

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

const int n = 400;
const int m = 1995;
const int q = 45;

class UF {
	array<int, n> v;
	array<int, n> s;
	
public:
	UF() {
		REP(i, 0, n) v[i] = -1;
		REP(i, 0, n) s[i] = 1;
	}

	int vroot(int i) {
		if (v[i] == -1) return i;
		return v[i] = vroot(v[i]);
	}

	bool vsame(int i, int j) {
		return vroot(i) == vroot(j);
	}

	void vunite(int i, int j) {
		int ri = vroot(i);
		int rj = vroot(j);
		if (ri == rj) return;
		if (s[ri] < s[rj]) std::swap(ri, rj);
		v[rj] = ri;
		s[ri] += s[rj];
	}
};

int x[n];
int y[n];
int a[m];
int b[m];
int d[m];
int nd[m];
bool chosen[m];

void judge() {
	int sum = 0;
	REP(j, 0, m) {
		if (chosen[j]) {
			sum += nd[j];
		}
	}
	cerr << "out: " << sum << endl;

	WeightedGraph<int> wg(n);
	REP(j, 0, m) {
		wg.add(a[j], b[j], nd[j], false);
	}
	auto res = kruskal(wg);
	int act = 0;
	for (int j : res) {
		act += wg.getWeights()[j];
	}
	cerr << "act: " << act << endl;
	cerr << "score: " << FLOAT << double(act) / sum << endl;
}

UF uf;
Random rnd;
pair<int, int> estd[q][m];
int rem = n - 1;
int diff(int l, int j) {
	auto test = [&](bool flg) -> int {
		UF ufc = uf;
		int sum = 0;
		int cnt = 0;
		if (flg) {
			ufc.vunite(a[j], b[j]);
			++cnt;
		}
		for (auto&& p : estd[l]) {
			int k = p.second;
			if (k <= j) continue;
			int c = p.first;
			if (ufc.vsame(a[k], b[k])) continue;
			sum += c;
			++cnt;
			ufc.vunite(a[k], b[k]);
			if (cnt == rem) {
				break;
			}
		}
		if (!flg && !ufc.vsame(a[j], b[j])) return INF;
		return sum;
	};
	
	int suma = test(true);
	int sumb = test(false);
	//cerr << suma << ' ' << sumb << endl;
	return sumb - suma;
}

int main() {
	SPEEDUP;
	REP(i, 0, n) cin >> x[i] >> y[i];
	REP(j, 0, m) {
		cin >> a[j] >> b[j];
		d[j] = round(hypot(x[b[j]] - x[a[j]], y[b[j]] - y[a[j]]));
		REP(l, 0, q) {
			estd[l][j] = make_pair(rnd.rnd(2 * d[j] + 1) + d[j], j);
		}
	}
	REP(l, 0, q) sort(estd[l], estd[l] + m);
	REP(j, 0, m) {
		int c;
		cin >> c;
		nd[j] = c;
		if (!uf.vsame(a[j], b[j])) {
			double ave = 0;
			REP(l, 0, q) ave += diff(l, j);
			ave /= q;
			chosen[j] = c <= ave + 2.3;
			if (chosen[j]) {
				uf.vunite(a[j], b[j]);
				REP(k, 0, n) {
					uf.vroot(k);
				}
				--rem;
			}
		}
		cout << chosen[j] << endl;
	}

#ifdef ONLY_MY_ENVIR
	judge();
#endif 
	return 0;
}

// Included Files: 
//   * UnionFind.h
//   * UnionFind.cpp
//   * Random.h
//   * Random.cpp
//   * Graph.h
//   * Graph.cpp
//   * SpanningTree.h
//   * Main.h
//   * Main.cpp
