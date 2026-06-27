#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

#include <bits/stdc++.h>
#include <sys/time.h>
#include <unistd.h>
using namespace std;
#define rep(i,n)   for(int i = 0; (i) < ((int)(n)); ++(i))
#define bit(n)      (((int)1)<<((int)(n)))
#define sz(v)       ((int)((v).size()))

std::string dbgDelim(int &i){ return (i++ == 0 ? "" : ","); }
#define dbgEmbrace(exp) { int i = 0; os << "{"; { exp; } os << "}"; return os; }
template <class T> std::ostream& operator<<(std::ostream &os,std::vector<T> v);
template <class T> std::ostream& operator<<(std::ostream &os,std::set<T> v);
template <class T> std::ostream& operator<<(std::ostream &os,std::queue<T> q);
template <class T> std::ostream& operator<<(std::ostream &os,std::priority_queue<T> q);
template <class T,class K> std::ostream& operator<<(std::ostream &os,std::pair<T,K> p);
template <class T,class K> std::ostream& operator<<(std::ostream &os,std::map<T,K> mp);
template <class T,class K> std::ostream& operator<<(std::ostream &os,std::unordered_map<T,K> mp);
template <int INDEX,class TUPLE> void dbgDeploy(std::ostream &os,TUPLE _){}
template <int INDEX,class TUPLE,class H,class ...Ts> void dbgDeploy(std::ostream &os,TUPLE t){ os << (INDEX == 0 ? "" : ",") << get<INDEX>(t); dbgDeploy<INDEX + 1,TUPLE,Ts...>(os,t); }
template <class T,class K> void dbgDeploy(std::ostream &os,std::pair<T,K> p,std::string delim){ os << "(" << p.first << delim << p.second << ")"; }
template <class ...Ts> std::ostream& operator<<(std::ostream &os,std::tuple<Ts...> t){ os << "("; dbgDeploy<0,std::tuple<Ts...>,Ts...>(os,t); os << ")"; return os; }
template <class T,class K> std::ostream& operator<<(std::ostream &os,std::pair<T,K> p){ dbgDeploy(os,p,","); return os; }
template <class T> std::ostream& operator<<(std::ostream &os,std::vector<T> v){ dbgEmbrace( for(T t: v){ os << dbgDelim(i) << t; }); }
template <class T> std::ostream& operator<<(std::ostream &os,std::set<T> s){ dbgEmbrace( for(T t: s){ os << dbgDelim(i) << t; }); }
template <class T> std::ostream& operator<<(std::ostream &os,std::queue<T> q){ dbgEmbrace( for(; q.size(); q.pop()){ os << dbgDelim(i) << q.front(); }); }
template <class T> std::ostream& operator<<(std::ostream &os,std::priority_queue<T> q){ dbgEmbrace( for(; q.size(); q.pop()){ os << dbgDelim(i) << (T) q.top();   }); }
template <class T,class K> std::ostream& operator<<(std::ostream &os,std::map<T,K> mp){ dbgEmbrace( for(auto p: mp){ os << dbgDelim(i); dbgDeploy(os,p,"->"); }); }
template <class T,class K> std::ostream& operator<<(std::ostream &os,std::unordered_map<T,K> mp){ dbgEmbrace( for(auto p: mp){ os << dbgDelim(i); dbgDeploy(os,p,"->"); }); }
#define DBG_OUT std::cerr
#define DBG_OVERLOAD(_1,_2,_3,_4,_5,_6,macro_name,...) macro_name
#define DBG_LINE() { char s[99]; sprintf(s,"line:%3d | ",__LINE__); DBG_OUT << s; }
#define DBG_OUTPUT(v) { DBG_OUT << (#v) << "=" << (v); }
#define DBG1(v,...) { DBG_OUTPUT(v); }
#define DBG2(v,...) { DBG_OUTPUT(v); DBG_OUT << ","; DBG1(__VA_ARGS__); }
#define DBG3(v,...) { DBG_OUTPUT(v); DBG_OUT << ","; DBG2(__VA_ARGS__); }
#define DBG4(v,...) { DBG_OUTPUT(v); DBG_OUT << ","; DBG3(__VA_ARGS__); }
#define DBG5(v,...) { DBG_OUTPUT(v); DBG_OUT << ","; DBG4(__VA_ARGS__); }
#define DBG6(v,...) { DBG_OUTPUT(v); DBG_OUT << ","; DBG5(__VA_ARGS__); }

#define DEBUG0() { DBG_LINE(); DBG_OUT << std::endl; }
#define DEBUG(...)                                                      \
{                                                                     \
DBG_LINE();                                                         \
DBG_OVERLOAD(__VA_ARGS__,DBG6,DBG5,DBG4,DBG3,DBG2,DBG1)(__VA_ARGS__); \
DBG_OUT << std::endl;                                               \
}

#ifdef LOCAL
#define my_assert(b) { if(!(b)) DEBUG(""); assert(b); }
#else
#define my_assert(b) assert(true)
#endif

class Timer
{
public:
void restart()
{
if(start_time == -1){
start_time = get_time();
start_clock = get_cycle();
}
}

double get_elapsed()
{
if(rdtsc_per_sec_inv != -1) return (double)(get_cycle() - start_clock) * rdtsc_per_sec_inv;

const double RDTSC_MEASUREMENT_INTERVAL = 0.1;
double res = get_time() - start_time;
if(res <= RDTSC_MEASUREMENT_INTERVAL) return res;

rdtsc_per_sec_inv = (get_time() - start_time) / (get_cycle() - start_clock);
return get_elapsed();
}

Timer()
{
restart();
}
private:

static double rdtsc_per_sec_inv;
static double start_time;
static unsigned long long int start_clock;

double get_time()
{
timeval tv;
gettimeofday(&tv,0);
return tv.tv_sec + tv.tv_usec * 0.000001;
}

unsigned long long int get_cycle()
{
unsigned int low,high;
__asm__ volatile ("rdtsc" : "=a" (low),"=d" (high));
return ((unsigned long long int)low) | ((unsigned long long int)high << 32);
}
};
double Timer::rdtsc_per_sec_inv = -1;
double Timer::start_time = -1;
unsigned long long int Timer::start_clock = -1;


class MyRand
{
public:
MyRand(int seed)
{
mt.seed(seed);
}

int rand_int()
{
return mt() & (bit(30) - 1);
}

double rand_double()
{
const int N = 10000000;
return ((double) (rand_int() % N)) / (N - 1.0);
}

private:
mt19937 mt;
};



























class Graph
{
public:
Graph(int n) : n(n) 
{
rep(i,n) edges.push_back(vector<int>(n,0));
}

Graph copy()
{
Graph graph(n);
graph.edges = edges;
return graph;
}

static Graph parse(string s)
{
int n = 1;
while(n * (n - 1) / 2 < sz(s)) ++n;
my_assert(n * (n - 1) / 2 == sz(s));

Graph graph(n);
int index = 0;
rep(i,n)for(int j = i + 1; j < n; ++j){
if(s[index++] == '1') graph.set_edge(i,j);
}
return graph;
}

string flat_str()
{
stringstream ss;
rep(i,n)for(int j = i + 1; j < n; ++j) ss << edges[i][j];
return ss.str();
}

vector<long> get_edge_vec()
{
vector<long> res;
long num_digits = 0;
long buffer = 0;
rep(i,n)for(int j = i + 1; j < n; ++j){
buffer = (buffer << 1) | edges[i][j];
++num_digits;
if(num_digits == 60){
res.push_back(buffer);
buffer = 0;
num_digits = 0;
}
}
if(0 < num_digits) res.push_back(buffer);
return res;
}

long get_edge_mask()
{
long num_digits = 0;
long buffer = 0;
rep(i,n)for(int j = i + 1; j < n; ++j){
buffer = (buffer << 1) | edges[i][j];
++num_digits;
my_assert(num_digits < 60);
}
return buffer;
}

void swap_nodes(int e0,int e1)
{
if(e0 == e1) return;
rep(i,sz(edges)) swap(edges[i][e0],edges[i][e1]);
rep(i,sz(edges)) swap(edges[e0][i],edges[e1][i]);
}

void set_edge(int e0,int e1)
{
my_assert(e0 != e1);
edges[e0][e1] = true;
edges[e1][e0] = true;
}

void unset_edge(int e0,int e1)
{
my_assert(e0 != e1);
edges[e0][e1] = false;
edges[e1][e0] = false;
}

void flip_all()
{
rep(i,n)rep(j,i) flip_edge(i,j);
}

void flip_edge(int e0,int e1)
{
my_assert(e0 != e1);
edges[e0][e1] = !edges[e0][e1];
edges[e1][e0] = !edges[e1][e0];
}

bool has_edge(int e0,int e1)
{
my_assert(e0 != e1);
return edges[e0][e1];
}

int get_n()
{
return n;
}

private:
int n;
vector<vector<int>> edges;
};

class ExpNum
{
public:
ExpNum() : e(0.1)
{
mul = 0;
expo = 1e40;
}

ExpNum(double e) : e(e) 
{
my_assert(e < 1.0);
}

static ExpNum from_double(double d,double e)
{
if(d == 0){
return ExpNum(e);
}
ExpNum num(e);
double l = log(d) / log(e);
num.mul = 1;
num.expo = l;
return num;
}

ExpNum& operator=(const ExpNum other)
{
this->e = other.e;
this->expo = other.expo;
this->mul = other.mul;
return *this;
}

void add(ExpNum num)
{
my_assert(num.e == e);
add(num.mul,num.expo);
}

void add(double mul,double expo)
{
if(mul == 0) return;

double total_mul = 0.0;
if(expo < this->expo){
total_mul += mul;
total_mul += this->mul * pow(e,this->expo - expo);
this->expo = expo;
}else{
total_mul += mul * pow(e,expo - this->expo);
total_mul += this->mul;
}
this->mul = total_mul;

normalize();
}

void multiply(ExpNum num)
{
multiply(num.mul,num.expo);
}

void multiply(double mul,double expo)
{
this->mul *= mul;
this->expo += expo;
normalize();
}

void normalize()
{
if(this->mul == 0) return;

int debug = 0;
while(1 < this->mul){
this->mul *= e;
this->expo -= 1;
++debug;
}
while(this->mul < e){
this->mul /= e;
this->expo += 1;
++debug;
}
}

bool is_bigger_than(ExpNum &exp_num)
{
my_assert(exp_num.e == e);
double min_expo = min(expo,exp_num.expo);
double my_score = mul * pow(e,expo - min_expo);
double op_score = exp_num.mul * pow(e,exp_num.expo - min_expo);
return my_score > op_score;
}

friend std::ostream& operator<<(std::ostream& os,const ExpNum &exp_num)
{
os << "(mul=" << exp_num.mul << ",expo=" << exp_num.expo << ",e=" << exp_num.e << ")";
return os;
}

double expo = 1e20;
double mul = 0;

private:
double e;
};

class BayesComputer
{
public:
BayesComputer()
{

}

BayesComputer(int num_nodes,double error_rate) : num_nodes(num_nodes)
{
flip_counts = vector<double>(num_nodes * (num_nodes - 1) / 2 + 1,0.0);

error_probs.push_back(1.0);
error_probs_rev.push_back(1.0);
while(sz(error_probs) <= sz(flip_counts)){
error_probs.push_back(error_probs.back() * error_rate);
error_probs_rev.push_back(error_probs_rev.back() * (1 - error_rate));
}
}

class MyGraph
{
public:
vector<pair<long,double>> permutations;

void optimize()
{
vector<pair<long,double>> next_per;
unordered_map<long,vector<int>> mp;
for(auto &p: permutations){
long hash = p.first;
int next_index = -1;
for(auto index: mp[hash]){
bool ok = true;
ok &= next_per[index].first == p.first;
if(ok) next_index = index;
}
if(next_index == -1){
mp[hash].push_back(sz(next_per));
next_per.push_back(p);
}else{
next_per[next_index].second += p.second;
}
}
permutations = next_per;
}
};

/**
* Returns average prob.
*/
double compute_prob(MyGraph &my_graph,long edges)
{
rep(i,sz(flip_counts)) flip_counts[i] = 0;

for(pair<long,double> &p: my_graph.permutations){
int cnt = __builtin_popcountll(edges ^ p.first);
flip_counts[cnt] += p.second;
}

double total = 0.0;
for(int i = sz(flip_counts) - 1; 0 <= i; --i){
total += flip_counts[i] * error_probs[i] * error_probs_rev[sz(flip_counts) - 1 - i];
}
return total / fact(num_nodes);
}

MyGraph to_my_graph(Graph &graph)
{
my_assert(graph.get_n() == num_nodes);

MyGraph my_graph;
vector<int> indexes;
rep(i,num_nodes) indexes.push_back(i);
to_my_graph(graph,indexes,my_graph);
return my_graph; 
}

private:

void to_my_graph(Graph &graph,vector<int> &indexes,MyGraph &my_graph,int depth = 0)
{
if(depth == sz(indexes)){
long buffer = 0;
rep(i,graph.get_n())for(int j = i + 1; j < graph.get_n(); ++j){
buffer = (buffer << 1) | graph.has_edge(indexes[i],indexes[j]);
}
my_graph.permutations.push_back(make_pair(buffer,1.0));
return;
}
for(int i = depth; i < sz(indexes); ++i){
swap(indexes[depth],indexes[i]);
to_my_graph(graph,indexes,my_graph,depth + 1);
swap(indexes[depth],indexes[i]);
}
}

static double fact(int n)
{
if(n == 0) return 1.0;
return n * fact(n - 1);
}

int num_nodes;
vector<double> flip_counts;
vector<double> error_probs;
vector<double> error_probs_rev;
};


class ClusteringComplete
{
public:

static int count_flipped_edges(Graph &graph,vector<int> cluster_indexes)
{
int cnt = 0;
rep(i,graph.get_n())rep(j,i){
cnt += ((graph.has_edge(i,j)) == (cluster_indexes[i] == cluster_indexes[j])) ? 0 : 1;
}
return cnt;
}

static vector<int> cluster_complete_graphs(Graph &graph,int num_clusters,bool use_simulated_annealing,int num_attempts = 1,int seed = 1234)
{
MyRand my_rand(seed);
vector<int> best_cluster_indexes = cluster_complete_graphs(graph,num_clusters,my_rand,use_simulated_annealing);

rep(i,num_attempts - 1){
vector<int> cluster_indexes = cluster_complete_graphs(graph,num_clusters,my_rand,use_simulated_annealing);
if(count_flipped_edges(graph,cluster_indexes) < count_flipped_edges(graph,best_cluster_indexes)){
best_cluster_indexes = cluster_indexes;
}
}
return best_cluster_indexes;
}

private:

static vector<int> cluster_complete_graphs(Graph &graph,int num_clusters,MyRand &my_rand,bool simulated_annealing)
{
#define i64_bit_lower_63(t) (((uint64_t) 1) << (t & 63))
#define above_64(t) ((64 <= t) ? 1 : 0)

my_assert(num_clusters < 15);

int n = graph.get_n();

uint64_t clusters[15][2];
uint64_t has_edges[100][2];
uint64_t non_edges[100][2];
memset(clusters,0,sizeof(clusters));
memset(has_edges,0,sizeof(has_edges));
memset(non_edges,0,sizeof(non_edges));

vector<int> cluster_indexes;
rep(i,n) cluster_indexes.push_back(my_rand.rand_int() % num_clusters);
rep(i,n) clusters[cluster_indexes[i]][above_64(i)] |= i64_bit_lower_63(i);
rep(i,n)rep(j,i){
if(graph.has_edge(i,j)){
has_edges[i][above_64(j)] |= i64_bit_lower_63(j);
has_edges[j][above_64(i)] |= i64_bit_lower_63(i);
}else{
non_edges[i][above_64(j)] |= i64_bit_lower_63(j);
non_edges[j][above_64(i)] |= i64_bit_lower_63(i);
}
}

int total_edges[101];
rep(i,graph.get_n()){
total_edges[i] = 0;
rep(j,graph.get_n())if(i != j) total_edges[i] += graph.has_edge(i,j) ? 1 : 0;
}

vector<int> indexes;
rep(i,graph.get_n()) indexes.push_back(i);

int num_loops = 100;
rep(main_loop,num_loops){
rep(i,graph.get_n()) swap(indexes[i],indexes[my_rand.rand_int() % (i + 1)]);
for(int node_index: indexes){
int curr_cluster_index = cluster_indexes[node_index];
int next_cluster_index = my_rand.rand_int() % num_clusters;
if(curr_cluster_index == next_cluster_index) continue;

int score = 0;
rep(i,2){
score += __builtin_popcountll(clusters[next_cluster_index][i] & has_edges[node_index][i]);
score -= __builtin_popcountll(clusters[next_cluster_index][i] & non_edges[node_index][i]);
score -= __builtin_popcountll(clusters[curr_cluster_index][i] & has_edges[node_index][i]);
score += __builtin_popcountll(clusters[curr_cluster_index][i] & non_edges[node_index][i]);
}

bool update = 0 <= score;
if(simulated_annealing && !update){
double temp = (num_loops * 0.5 - main_loop) / (num_loops * 0.5);
update |= (0 < temp) && (my_rand.rand_double() < exp(score / 10 / pow(temp,2)));
}
if(update){
cluster_indexes[node_index] = next_cluster_index;
clusters[curr_cluster_index][above_64(node_index)] &= ~i64_bit_lower_63(node_index);
clusters[next_cluster_index][above_64(node_index)] |= i64_bit_lower_63(node_index);
}
}
}

return cluster_indexes;

#undef i64_bit_lower_63
#undef above_64
}
};

class Solver
{
public:
virtual int get_n() = 0;
virtual vector<Graph> get_graphs() = 0;
virtual int predict(Graph graph) = 0;
virtual ~Solver(){}
};

class BayesSolver: public Solver
{
public:
BayesSolver(double error_rate,int num_nodes,vector<Graph> graphs)
{
this->num_nodes = num_nodes;
this->graphs = graphs;
bayes_computer = BayesComputer(num_nodes,error_rate);

rep(i,sz(graphs)){
my_graphs[i] = bayes_computer.to_my_graph(graphs[i]);
my_graphs[i].optimize();
}
}

static BayesSolver* create_optimal(int num_graphs,double error_rate)
{
int num_nodes = 4;
vector<Graph> graphs;
for(num_nodes = 4; true; ++num_nodes){
graphs = get_all_distinct_graphs(num_nodes);
if(num_graphs <= sz(graphs)) break;
}
while(num_graphs < sz(graphs)) graphs.pop_back();
return new BayesSolver(error_rate,num_nodes,graphs);
}

static BayesSolver* create_sloth(int num_graphs,double error_rate,int num_nodes)
{
MyRand my_rand(1234);
vector<Graph> distinct_graphs = get_all_distinct_graphs(num_nodes);
rep(i,sz(distinct_graphs)) swap(distinct_graphs[i],distinct_graphs[my_rand.rand_int() % (i + 1)]);

vector<Graph> graphs;
rep(i,num_graphs) graphs.push_back(distinct_graphs[i % sz(distinct_graphs)]);
return new BayesSolver(error_rate,num_nodes,graphs);
}

static BayesSolver* create_sparse_8(double error_rate,vector<long> edge_masks)
{
int num_nodes = 8;
vector<Graph> graphs;
for(long mask: edge_masks){
Graph graph(num_nodes);
int index = 0;
rep(i,num_nodes)rep(j,i){
if(mask & (1l << (27 - index))) graph.set_edge(i,j);
++index;
}
graphs.push_back(graph);
}
return new BayesSolver(error_rate,num_nodes,graphs);
}

static BayesSolver* create_sparse_8(int num_graphs,double error_rate)
{
vector<long> edge_masks({
2374904,234869856,121435744,43904576,78017632,0,5226080,191822328,268046400,206568304,
158170176,47367908,206477408,222289472,256837606,218953312,167201856,253784304,33846368,
233799280,268147824,239075320,217487360,235168752,66818112,65308736,247384128,171953216,
143636064,268376064,225376224,35039075,165515328,58710112,24545344,71187016,221182576,
77548608,263404144,127072612,192731200,67007584,159801712,167425632,251050608,111574016,
129947648,242221561,225809504,260888672,64588864,135055424,14595680,134184312,245656640,
263455729,91049024,268427004,248313440,177344576,131790688,267363936,57933692,165600352,
106331760,225951484,46093408,2022000,186580576,185070448,264496868,259484736,67090016,
130023008,131065696,188634208,104659568,233564000,55574368,196578368,266338304,95976690,
48099392,267319392,215977024,167730288,133380600,268435392,220633088,164754288,117688037,
225951232,183429088,205578464,33065024,231507937,265112640,74374240,101409938,52564032
});
while(num_graphs < sz(edge_masks)) edge_masks.pop_back();
return create_sparse_8(error_rate,edge_masks);
}

vector<Graph> get_graphs()
{
return graphs;
}

int predict(Graph graph)
{
long graph_edge = graph.get_edge_mask();

int best_index = 0;
double best_score = -1e100;
for(auto p: my_graphs){
double score = bayes_computer.compute_prob(p.second,graph_edge);
if(best_score < score){
best_score = score;
best_index = p.first;
}
}
return best_index;
}

int get_n()
{
return num_nodes;
}

static vector<Graph> get_all_distinct_graphs(int num_nodes)
{
int num_edges = num_nodes * (num_nodes - 1) / 2;
vector<Graph> res;
unordered_set<long> used;
rep(mask,bit(num_edges)){
Graph graph(num_nodes);
for(int index = 0,i = 0; i < num_nodes; ++i)rep(j,i){
if(mask & bit(index)) graph.set_edge(i,j);
++index;
}

vector<long> edges = graph.get_edge_vec();
my_assert(sz(edges) == 1);
if(used.count(edges[0])) continue;

res.push_back(graph);
int t = sz(used);
populate_permutation(graph,0,used);
}

return res;
}

static void populate_permutation(Graph &graph,int i,unordered_set<long> &used)
{
if(i == graph.get_n()){
vector<long> v = graph.get_edge_vec();
my_assert(sz(v) == 1);
used.insert(v[0]);
return;
}
for(int j = i; j < graph.get_n(); ++j){
graph.swap_nodes(i,j);
populate_permutation(graph,i + 1,used);
graph.swap_nodes(i,j);
}
}

private:
int num_nodes;
BayesComputer bayes_computer;
vector<Graph> graphs;
unordered_map<int,BayesComputer::MyGraph> my_graphs;
};

class CompleteGraphSolver : public Solver
{
public:

static int calc_hash(vector<int> v)
{
my_assert(sz(v) <= 4);

sort(v.begin(),v.end());
int mask = 0;
for(int size: v){
my_assert(0 <= size && size <= 100);
mask = (mask * 105) + (size + 1);
}
return mask;
}

class CompleteGraph
{
public:
CompleteGraph(vector<int> sizes) : sizes(sizes)
{
sort(this->sizes.begin(),this->sizes.end());
reverse(this->sizes.begin(),this->sizes.end());
while(sz(sizes) && sizes.back() == 0) sizes.pop_back();
sort(this->sizes.begin(),this->sizes.end());
}

int get_num_complete_graphs()
{
return sz(sizes);
}

long get_hash()
{
return CompleteGraphSolver::calc_hash(sizes);
}

Graph to_graph(int num_nodes)
{
if(sizes.empty()){
Graph graph(num_nodes);
if(flip_all) graph.flip_all();
return graph;
}
Graph graph(num_nodes);
rep(i,num_nodes)rep(j,i)if(get_graph_id(i) == get_graph_id(j)) graph.set_edge(i,j);
if(flip_all) graph.flip_all();
return graph;
}

int flip_all = 0;
vector<int> sizes;

private:
int get_graph_id(int node_index)
{
rep(i,sz(sizes)){
if(node_index < sizes[i]) return i;
node_index -= sizes[i];
}
my_assert(false);
}

};

CompleteGraphSolver(vector<CompleteGraph> graphs,double error_rate)
{
// DEBUG(sz(graphs));
this->error_rate = error_rate;

int num_nodes = 0;
if(sz(graphs)){
for(int size: graphs.back().sizes) num_nodes += size;
}

rep(i,sz(graphs)){
this->graphs.push_back(graphs[i].to_graph(num_nodes));
complete_graphs[graphs[i].flip_all][graphs[i].get_num_complete_graphs()].push_back(make_pair(graphs[i],i));
}
}

static void create_packed_recur(int rem,int stride,int min_nodes,vector<int> &sizes,vector<CompleteGraph> &res)
{
if(rem == 0){
res.push_back(CompleteGraph(sizes));
return;
}

for(int i = min_nodes; i <= rem; i += stride){
sizes.push_back(i);
create_packed_recur(rem - i,stride,i,sizes,res);
sizes.pop_back();
}
if(0 < (rem - min_nodes) % stride && min_nodes <= rem){
sizes.push_back(rem);
create_packed_recur(rem - rem,stride,rem,sizes,res);
sizes.pop_back();
}
}

static CompleteGraphSolver* create_packed(int num_graphs,double error_rate,int stride,int min_nodes)
{
vector<int> sizes;
vector<CompleteGraph> graphs;
int num_nodes = 0;
for(num_nodes = 0; sz(graphs) < num_graphs; ++num_nodes){
graphs.clear();
create_packed_recur(num_nodes,stride,min_nodes,sizes,graphs);
}
while(num_graphs < sz(graphs)) graphs.pop_back();
return new CompleteGraphSolver(graphs,error_rate);
}

class SparseGraphs
{
public:
SparseGraphs()
{
{
unordered_map<int,unordered_map<int,vector<int>>> sparse_graph3_ints;
sparse_graph3_ints[40][6] = vector<int>({0,0,2,2,0,3,1,2,1,1,0,2,0,1,});
sparse_graph3_ints[40][8] = vector<int>({0,0,2,3,1,3,0,4,2,2,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[40][10] = vector<int>({0,0,3,3,2,4,0,5,1,4,2,3,0,4,1,3,2,2,0,1,0,3,1,2,0,2,1,1,});
sparse_graph3_ints[40][12] = vector<int>({4,4,0,0,0,6,3,4,2,5,1,5,3,3,2,4,0,5,0,1,1,4,2,3,0,2,0,4,1,3,2,2,1,1,0,3,1,2,});
sparse_graph3_ints[40][14] = vector<int>({4,5,0,0,0,7,4,4,0,2,3,5,0,6,2,6,1,6,2,5,3,4,0,1,1,5,3,3,2,4,0,5,1,1,1,4,0,4,2,3,0,3,1,2,2,2,1,3,});
sparse_graph3_ints[40][16] = vector<int>({5,5,0,0,0,3,0,8,4,6,4,5,3,6,0,2,2,7,1,7,0,7,3,5,4,4,2,6,0,4,0,6,0,1,1,6,2,5,3,4,2,2,1,1,1,2,1,5,3,3,2,4,0,5,1,4,1,3,2,3,});
sparse_graph3_ints[40][18] = vector<int>({6,6,0,0,0,4,0,8,5,6,4,7,2,2,0,9,5,5,4,6,3,7,0,3,1,8,2,8,4,5,2,7,3,6,0,2,1,7,1,3,0,7,0,6,0,1,3,5,1,2,2,6,4,4,0,5,1,6,1,1,1,4,2,5,3,4,2,3,2,4,1,5,3,3,});
sparse_graph3_ints[40][20] = vector<int>({6,7,0,0,0,4,0,10,6,6,2,2,5,7,4,8,0,9,5,6,1,4,4,7,1,9,3,8,2,9,0,5,0,8,5,5,0,2,2,8,4,6,0,6,3,7,2,3,1,8,1,3,0,1,2,4,2,7,0,3,4,5,3,6,1,2,1,7,3,3,0,7,4,4,1,5,1,1,3,5,2,6,3,4,1,6,2,5,});
sparse_graph3_ints[40][23] = vector<int>({7,8,0,0,3,3,0,11,7,7,1,3,6,8,1,11,1,5,6,7,5,9,0,10,5,8,0,5,0,1,0,6,3,4,4,9,1,10,6,6,1,4,2,10,1,1,0,7,5,7,3,10,2,4,4,8,3,9,0,9,2,3,1,9,2,5,5,6,0,4,2,9,3,5,4,7,0,3,0,8,3,8,1,6,0,2,4,4,2,2,1,7,5,5,1,2,4,6,2,8,4,5,3,7,1,8,2,6,3,6,2,7,});
sparse_graph3_ints[40][26] = vector<int>({8,9,0,0,0,4,0,12,4,4,8,8,7,9,0,8,0,13,2,2,7,8,4,5,6,10,0,6,1,12,6,9,3,5,7,7,2,12,5,10,0,10,2,4,6,8,0,7,0,2,3,4,0,11,5,9,4,11,1,11,2,6,0,5,4,6,3,11,4,10,6,7,2,11,2,5,1,6,1,4,0,9,5,8,3,6,2,3,3,3,0,1,1,5,6,6,1,7,3,10,4,9,5,5,2,10,1,3,1,10,5,7,1,8,0,3,2,7,5,6,4,8,1,9,3,9,4,7,1,2,2,8,3,7,2,9,3,8,1,1,});
sparse_graph3_ints[40][30] = vector<int>({10,10,0,0,0,4,0,14,5,5,0,8,9,10,0,15,9,9,4,6,2,2,8,11,0,10,8,10,0,6,6,6,1,14,4,4,7,11,8,9,5,6,2,14,0,12,7,10,4,5,2,4,0,9,0,2,0,7,0,13,6,12,8,8,1,13,4,7,0,5,3,6,6,11,7,9,2,6,5,7,1,4,3,5,0,11,2,13,1,8,5,12,6,7,3,13,3,7,6,10,4,13,2,7,2,3,2,8,7,8,3,4,1,9,1,6,1,7,7,7,4,12,1,12,5,11,2,12,2,5,6,8,6,9,1,10,4,8,1,5,3,8,1,3,3,3,0,1,3,12,2,9,0,3,5,8,1,11,5,10,4,11,2,10,5,9,3,9,4,9,3,11,2,11,4,10,1,2,3,10,1,1,});
sparse_graph3_ints[40][35] = vector<int>({11,12,0,0,3,5,0,17,7,7,1,3,0,12,11,11,1,17,10,12,5,5,0,5,7,8,0,10,10,11,0,16,9,13,6,7,0,9,3,3,6,6,9,12,0,13,1,16,10,10,6,8,5,6,1,5,0,11,0,8,0,1,2,3,5,7,2,16,0,7,9,11,4,5,5,8,8,13,0,15,8,8,4,6,1,4,1,7,1,1,0,6,1,11,4,7,3,4,1,15,1,9,7,9,8,12,0,14,3,7,4,4,0,4,3,6,5,9,3,16,9,10,4,8,7,14,2,5,1,10,6,9,1,6,3,15,3,8,1,8,9,9,7,13,1,13,2,15,8,11,4,9,2,7,2,4,3,9,2,6,0,3,2,8,2,9,8,9,1,12,5,15,4,15,8,10,6,14,2,10,3,10,7,12,2,2,2,11,7,10,1,14,5,14,5,10,4,10,6,13,});
sparse_graph3_ints[40][40] = vector<int>({13,13,0,0,0,8,0,20,0,14,0,4,9,9,6,6,12,14,8,8,4,4,12,13,0,12,0,19,2,2,8,9,11,14,1,19,0,10,0,16,7,7,12,12,4,6,8,10,11,13,2,19,7,8,0,6,5,6,10,15,0,18,9,10,6,8,0,15,10,14,2,18,0,13,7,9,4,5,6,7,11,12,0,11,2,4,5,5,7,10,4,18,10,10,6,9,0,9,5,7,0,5,2,6,0,2,0,7,4,7,1,13,1,18,10,13,6,10,3,18,9,15,5,8,1,14,3,6,4,8,3,5,1,12,1,4,1,8,5,9,1,10,3,4,1,11,0,17,8,11,2,8,3,7,2,3,1,9,1,6,4,9,11,11,5,10,2,7,3,8,2,5,1,7,2,10,2,9,3,3,2,12,4,10,7,11,3,9,6,11,8,16,10,12,1,15,9,14,1,3,});
sparse_graph3_ints[40][45] = vector<int>({15,15,0,0,5,5,0,22,9,9,1,3,0,17,0,12,11,11,0,7,7,7,14,15,1,22,13,16,10,10,14,14,0,14,5,20,0,18,8,8,10,11,1,21,13,15,0,10,7,8,3,5,0,16,9,11,5,7,0,4,0,21,9,10,3,21,0,15,6,6,12,16,8,9,0,9,0,13,3,3,6,7,13,14,10,12,2,21,8,10,0,19,7,9,4,5,0,11,12,15,9,12,6,8,1,15,4,20,5,6,8,11,1,17,11,17,7,10,11,12,0,20,0,8,1,13,0,5,4,6,6,9,7,19,5,8,1,7,1,11,4,7,1,5,0,1,2,3,1,9,7,11,13,13,4,4,5,9,3,7,1,4,3,6,3,4,1,1,0,6,4,8,6,10,1,8,1,16,8,12,1,12,3,8,2,5,1,10,1,14,2,7,4,9,1,6,3,9,2,4,});
sparse_graph3_ints[40][50] = vector<int>({16,17,0,0,8,8,0,25,4,4,12,12,0,20,0,4,0,12,0,16,10,10,6,22,6,6,16,16,15,17,12,13,0,24,9,9,0,18,0,8,11,11,7,7,2,2,0,14,4,6,7,21,11,12,15,16,8,9,0,19,1,24,14,18,10,12,0,10,6,8,8,10,11,13,8,21,14,17,2,24,10,11,0,21,0,17,0,6,6,7,9,10,13,13,0,15,7,8,4,5,9,11,15,15,0,22,4,23,10,13,0,13,7,9,14,16,1,19,3,23,13,18,12,14,5,6,9,12,5,22,8,11,1,18,11,14,7,10,0,23,0,11,1,17,6,9,5,5,2,4,5,7,2,6,1,2,0,9,5,8,1,15,8,12,10,14,1,23,8,20,13,17,9,13,6,10,1,20,2,23,1,16,7,11,1,13,12,19,14,15,2,18,4,22,10,20,13,14,9,14,});
sparse_graph3_ints[40][55] = vector<int>({18,18,0,0,0,14,0,27,3,3,13,13,0,22,9,9,5,5,9,23,0,18,11,11,7,7,1,3,0,10,17,19,14,14,17,18,7,24,1,27,10,10,13,14,0,23,0,19,7,8,11,12,0,16,3,5,0,12,9,10,0,21,8,8,12,12,16,19,8,23,13,15,5,7,10,11,0,26,0,15,0,20,12,13,8,9,9,11,0,17,6,6,0,9,7,9,0,13,1,5,0,3,4,5,17,17,11,13,5,25,1,21,6,7,8,10,12,14,10,12,16,18,14,15,0,11,0,7,10,22,6,8,2,26,1,19,0,24,9,12,12,15,15,20,6,24,1,17,7,10,5,6,11,14,1,26,8,11,10,13,1,15,1,22,15,15,15,19,7,23,11,15,6,9,9,13,1,20,0,8,5,8,7,11,1,13,3,26,16,17,0,25,9,22,10,14,8,12,1,18,});
sparse_graph3_ints[40][60] = vector<int>({12,12,0,0,20,20,0,30,4,4,8,8,0,20,16,16,0,24,0,4,8,26,0,12,0,16,10,10,14,14,6,6,19,20,10,25,6,22,0,8,12,13,9,9,0,18,0,22,15,15,10,11,7,7,18,21,0,26,1,29,9,25,13,13,15,16,19,19,0,14,11,11,2,2,4,6,8,9,11,12,14,15,18,20,12,24,6,20,0,10,6,8,9,10,0,29,0,21,14,16,12,14,7,26,0,25,0,17,10,12,8,10,0,19,0,23,13,14,6,7,0,6,11,13,9,11,0,15,15,17,11,24,2,29,13,15,17,21,7,8,10,13,8,20,18,19,6,27,0,28,10,24,16,17,4,5,13,16,11,14,9,12,7,9,0,13,1,19,1,24,4,28,8,11,1,21,12,15,5,6,14,17,10,14,1,23,1,17,7,10,1,20,6,21,9,13,17,20,12,16,});
sparse_graph3_ints[40][70] = vector<int>({8,8,23,23,0,0,0,24,10,30,0,6,14,14,0,30,18,19,0,18,0,35,0,10,10,11,8,26,0,14,1,2,16,16,12,12,6,22,6,6,14,28,0,22,0,28,4,4,22,24,9,9,19,19,6,32,0,34,8,24,15,15,12,13,0,20,0,26,10,10,0,16,7,7,17,17,12,29,22,23,13,13,18,18,11,11,14,15,0,12,8,9,4,6,2,2,0,8,10,24,6,20,15,16,11,12,19,20,15,27,21,24,8,31,16,17,13,14,6,7,8,10,10,12,0,21,8,22,17,18,18,20,22,22,13,28,0,31,2,34,8,25,14,16,12,14,0,25,0,29,11,29,1,34,0,19,0,23,16,18,9,25,0,27,9,10,11,13,13,15,0,17,7,8,7,22,9,30,15,17,17,19,21,23,20,20,16,27,7,31,9,24,0,32,9,11,11,14,13,16,});
sparse_graph3_ints[40][80] = vector<int>({0,14,0,40,0,0,26,27,18,18,0,24,0,6,0,32,14,14,10,10,12,28,22,22,18,31,0,20,0,10,8,36,8,24,0,28,0,36,8,8,1,2,12,12,16,16,6,6,20,20,0,18,10,26,12,34,4,4,6,20,8,30,26,26,16,32,22,23,11,11,9,9,7,7,0,16,0,22,15,15,12,13,19,19,0,26,17,17,6,24,0,30,0,34,14,15,8,26,14,28,6,37,10,11,13,13,16,17,20,21,25,27,18,19,23,23,17,31,0,12,8,9,4,6,2,2,0,8,6,22,11,12,15,16,13,14,21,21,17,18,10,24,6,32,13,27,10,35,6,7,19,20,8,10,10,12,14,16,0,21,12,14,16,18,8,28,8,22,0,25,0,29,0,19,0,23,9,10,7,8,0,17,11,13,1,39,14,33,21,22,25,26,18,20,13,15,15,17,9,11,});
sparse_graph3_ints[40][100] = vector<int>({33,33,0,0,0,42,0,26,0,10,20,20,16,42,26,26,0,18,0,34,0,50,16,16,12,12,16,30,0,6,8,8,24,38,8,38,10,24,0,38,29,29,0,22,20,34,8,46,0,30,22,22,8,34,0,46,1,2,0,14,10,10,14,14,14,38,6,30,18,18,6,6,14,28,24,24,8,22,4,4,14,34,6,42,12,26,20,40,12,44,6,20,11,11,9,9,0,20,0,16,7,7,18,32,27,27,32,34,30,30,8,32,13,13,0,24,0,28,15,15,6,26,17,17,19,19,21,21,23,23,26,37,22,34,0,32,0,36,0,40,25,25,8,36,10,40,0,44,28,28,14,30,6,47,18,41,16,36,8,28,0,12,5,5,10,11,8,9,12,13,6,22,14,15,16,17,12,28,18,19,20,21,22,23,18,30,8,24,25,26,10,36,6,34,10,26,17,18,15,16,});
sparse_graph3_ints[39][6] = vector<int>({0,0,2,2,0,3,1,2,1,1,0,2,0,1,});
sparse_graph3_ints[39][8] = vector<int>({0,0,2,3,0,4,1,3,2,2,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[39][10] = vector<int>({3,3,0,0,2,4,0,5,1,4,2,3,0,4,1,3,2,2,0,1,0,3,0,2,1,2,1,1,});
sparse_graph3_ints[39][12] = vector<int>({4,4,0,0,0,6,3,4,2,5,0,2,1,5,3,3,2,4,0,5,1,4,0,1,2,3,0,4,1,3,2,2,1,1,0,3,1,2,});
sparse_graph3_ints[39][14] = vector<int>({4,5,0,0,0,6,0,2,4,4,3,5,0,7,2,6,1,6,3,4,2,5,0,4,1,5,0,1,3,3,2,4,1,1,0,5,0,3,1,4,1,2,2,3,2,2,1,3,});
sparse_graph3_ints[39][16] = vector<int>({5,5,0,0,0,3,0,8,4,6,4,5,0,4,3,6,2,7,1,7,0,2,0,7,4,4,3,5,2,6,2,2,0,6,1,6,1,2,2,5,0,1,3,4,1,3,1,1,1,5,2,4,3,3,0,5,1,4,2,3,});
sparse_graph3_ints[39][18] = vector<int>({6,6,0,0,0,4,0,8,5,6,2,2,4,7,0,9,5,5,4,6,1,3,3,7,1,8,2,8,0,2,4,5,0,6,2,7,3,6,0,3,1,7,0,5,0,7,4,4,1,2,3,5,2,6,1,4,0,1,2,3,2,4,1,6,1,1,3,4,2,5,3,3,1,5,});
sparse_graph3_ints[39][20] = vector<int>({6,7,0,0,0,4,0,10,6,6,2,2,5,7,0,6,4,8,0,9,5,6,4,7,2,4,1,9,0,2,0,5,3,8,0,8,2,9,5,5,1,4,4,6,2,8,2,3,3,7,1,8,0,3,3,3,1,3,4,5,1,5,0,1,2,7,1,2,0,7,3,6,4,4,3,4,1,7,1,6,3,5,2,5,2,6,1,1,});
sparse_graph3_ints[39][23] = vector<int>({7,8,0,0,1,3,0,11,3,4,7,7,6,8,0,7,1,11,0,5,6,7,5,9,0,10,3,3,0,1,1,5,5,8,0,6,1,1,3,5,1,10,6,6,1,4,4,9,2,10,5,7,2,3,0,9,3,10,4,4,2,5,0,3,4,8,2,4,1,9,3,9,0,8,0,4,1,6,5,6,2,9,1,7,2,2,5,5,4,7,4,5,3,8,1,2,2,6,0,2,4,6,3,6,1,8,2,8,3,7,2,7,});
sparse_graph3_ints[39][26] = vector<int>({8,9,0,0,0,4,0,12,4,4,8,8,0,8,7,9,0,13,2,2,4,5,7,8,0,6,6,10,4,6,1,12,0,2,2,4,6,9,0,10,0,7,7,7,2,12,5,5,3,5,5,10,0,5,6,8,0,11,3,4,2,6,1,11,5,9,4,11,0,9,2,5,0,3,3,6,6,7,3,11,1,6,2,11,6,6,1,4,4,10,1,7,5,6,3,3,2,3,1,8,1,5,5,8,2,7,1,3,5,7,1,10,3,10,2,10,4,9,3,7,4,7,1,9,2,8,1,2,4,8,3,9,3,8,2,9,0,1,1,1,});
sparse_graph3_ints[39][30] = vector<int>({10,10,0,0,0,10,0,4,0,14,6,6,2,4,9,10,0,15,0,8,8,11,5,6,0,2,0,6,9,9,5,5,8,10,1,14,4,4,2,2,4,6,0,12,6,7,7,11,2,14,8,9,4,5,0,9,0,5,5,7,0,7,7,10,4,7,0,13,8,8,1,13,6,12,0,11,3,5,1,4,3,6,3,4,7,7,2,6,0,3,1,8,2,5,3,7,6,11,7,9,2,13,1,9,1,6,4,8,2,3,2,7,3,13,2,8,6,8,1,7,3,3,1,10,5,12,7,8,4,13,3,8,1,5,5,8,1,3,6,10,1,12,4,12,2,9,2,12,6,9,1,11,5,11,3,12,2,10,1,2,3,9,5,9,4,9,5,10,4,11,2,11,4,10,3,10,3,11,0,1,1,1,});
sparse_graph3_ints[39][35] = vector<int>({11,12,0,0,0,8,0,17,7,7,1,3,0,12,11,11,5,5,1,17,8,8,0,5,10,12,3,5,6,6,0,13,10,11,7,8,0,10,0,16,9,13,6,7,3,3,4,5,1,1,1,5,0,11,2,16,7,9,9,12,5,6,0,9,6,8,1,16,0,14,0,7,5,7,10,10,4,4,0,4,4,6,9,11,6,9,8,13,3,16,0,15,5,8,1,11,1,7,3,4,2,3,0,1,1,4,1,9,4,7,0,6,3,6,8,9,2,5,0,3,3,7,1,6,5,9,1,8,2,4,4,8,2,6,2,2,2,7,1,10,1,15,3,8,1,13,9,9,3,15,8,12,9,10,4,9,7,14,1,12,2,8,3,9,5,15,2,9,4,15,8,10,2,10,2,15,5,10,7,13,8,11,7,10,2,11,3,10,6,14,6,10,4,10,1,14,3,11,7,12,});
sparse_graph3_ints[39][40] = vector<int>({0,0,13,13,0,12,0,20,0,4,9,9,0,8,6,6,0,16,12,14,8,8,4,4,4,18,12,13,10,10,0,14,7,7,0,10,2,4,0,2,5,5,8,9,1,19,11,14,0,6,6,7,9,10,0,19,12,12,0,15,7,8,4,6,8,10,2,19,11,13,2,2,6,8,4,5,0,13,0,18,10,15,7,9,5,6,0,11,0,7,5,7,7,10,3,18,10,14,8,11,0,9,6,9,1,14,2,18,11,12,9,11,5,17,0,17,5,8,3,5,0,5,4,7,1,13,6,10,2,6,4,8,3,6,1,4,1,10,3,4,1,12,1,8,2,5,0,3,5,9,1,9,1,6,3,7,1,11,1,15,2,7,2,3,2,8,1,7,3,3,3,8,7,11,4,9,9,15,1,18,10,11,5,10,6,17,10,13,6,11,2,10,11,11,2,12,2,9,1,5,});
sparse_graph3_ints[39][45] = vector<int>({15,15,0,0,0,16,0,8,0,22,7,7,11,11,1,3,0,12,9,9,7,19,5,5,0,18,14,15,3,3,10,11,8,8,6,6,0,14,3,5,0,3,0,10,1,22,10,10,13,16,7,8,0,17,14,14,5,20,11,12,8,9,5,7,0,13,0,7,9,10,13,15,1,21,0,19,9,11,0,15,6,7,6,19,7,9,0,11,10,12,0,21,1,17,3,21,4,5,8,10,6,8,5,6,0,5,0,9,12,16,9,12,13,14,1,15,2,21,7,10,4,4,5,8,8,11,12,12,1,13,6,9,4,6,1,5,1,11,4,7,0,6,1,1,3,4,4,20,0,20,1,9,1,16,5,9,7,11,8,18,8,12,12,15,9,13,11,17,3,6,2,3,3,7,6,10,1,7,4,8,1,14,1,10,2,5,1,4,1,8,1,12,3,8,2,7,4,9,1,6,});
sparse_graph3_ints[39][50] = vector<int>({0,0,16,17,8,8,0,25,0,8,12,12,0,20,2,2,0,12,0,16,8,21,10,10,6,6,4,4,16,16,13,13,6,22,0,24,9,9,0,18,11,11,7,7,0,14,15,17,0,4,0,10,12,13,9,10,0,21,4,6,0,6,6,7,7,21,8,9,11,12,10,11,7,8,0,19,0,15,15,16,8,10,10,12,12,14,14,18,1,24,9,20,4,18,0,17,2,24,0,22,6,8,4,5,0,2,2,4,5,6,0,13,11,13,9,11,7,9,14,17,10,20,13,14,4,23,10,13,8,11,5,5,0,11,1,19,15,15,8,20,11,14,9,12,7,10,3,4,5,7,6,9,1,15,1,18,8,12,10,14,5,22,0,9,5,8,1,16,1,20,14,16,3,23,13,18,9,13,14,14,6,10,1,17,7,11,1,13,0,23,2,18,6,21,9,14,1,14,4,7,});
sparse_graph3_ints[39][55] = vector<int>({18,18,0,0,9,9,0,27,5,5,0,21,14,14,3,3,0,16,9,23,11,11,0,12,0,3,7,7,0,23,0,8,0,18,13,13,17,19,7,19,10,11,8,8,1,27,10,22,15,15,12,12,0,14,6,6,9,10,1,3,3,5,0,19,17,18,5,25,0,10,7,8,10,10,12,13,5,19,14,15,8,23,0,24,0,20,0,15,11,12,8,9,5,7,0,17,9,11,7,9,0,13,0,7,6,7,13,14,16,19,0,22,10,12,7,24,13,15,11,13,8,10,0,26,17,17,11,22,6,19,0,11,4,5,1,17,6,8,9,12,12,14,5,6,1,19,7,10,1,5,8,11,1,15,10,13,6,24,1,21,2,26,16,18,12,15,0,9,6,9,9,13,6,18,14,16,11,14,9,22,15,20,1,23,7,23,1,26,13,16,6,20,1,18,7,11,1,13,5,8,});
sparse_graph3_ints[39][60] = vector<int>({12,12,0,0,20,20,0,30,4,4,0,14,0,22,16,16,8,26,2,2,10,10,6,6,0,18,0,26,8,8,14,14,6,22,0,10,12,24,0,6,0,2,6,18,9,9,0,20,0,16,11,11,7,7,19,20,13,13,15,15,0,24,0,12,5,5,8,20,4,28,16,17,10,25,10,11,8,9,0,8,6,7,13,14,11,12,2,4,9,10,18,21,15,16,12,13,19,19,11,24,14,15,7,8,10,12,6,20,0,17,0,21,18,20,15,17,12,14,8,22,6,27,0,29,0,25,8,10,0,19,0,4,4,5,0,15,6,8,9,11,13,23,11,13,14,16,9,25,1,29,7,21,4,18,0,23,5,6,0,13,7,9,13,15,17,17,10,13,8,11,7,26,0,28,7,22,11,14,9,12,5,19,12,15,10,24,14,17,17,21,2,29,6,21,13,16,18,19,12,23,});
sparse_graph3_ints[39][70] = vector<int>({23,23,0,14,0,0,0,35,14,14,4,4,0,22,18,18,10,10,0,30,14,28,2,2,0,26,10,24,6,6,0,18,6,32,12,12,8,8,8,22,16,16,0,10,20,20,6,20,0,6,0,2,10,30,11,11,9,9,0,20,0,16,7,7,6,28,0,24,0,28,22,24,16,27,13,13,12,24,15,15,19,19,17,17,6,24,6,18,0,12,5,5,10,11,8,9,13,14,0,34,8,31,22,23,15,27,16,17,19,20,0,32,8,20,6,26,11,12,14,15,11,25,17,18,15,16,0,8,6,7,12,13,9,10,10,22,12,29,7,31,4,18,6,22,12,14,14,16,8,26,2,34,10,12,0,29,0,25,7,8,0,17,0,21,8,10,18,20,16,18,13,28,21,24,18,19,0,19,0,23,11,13,13,15,7,20,8,24,15,17,9,11,2,4,0,27,0,31,11,24,});
sparse_graph3_ints[39][80] = vector<int>({26,27,0,0,12,12,0,40,4,4,0,28,20,20,0,14,12,34,16,16,0,20,6,28,2,2,0,36,0,24,10,10,23,23,6,6,14,26,0,32,6,22,20,30,8,8,14,14,18,18,6,32,0,10,0,18,10,22,8,36,16,28,6,18,0,6,0,2,12,24,16,32,10,30,22,22,0,16,9,9,7,7,26,26,11,11,0,22,0,26,0,30,13,13,6,24,15,15,0,34,17,17,8,28,19,19,0,12,5,5,4,38,6,20,21,21,10,24,10,11,8,9,14,15,12,13,14,28,18,31,10,35,0,8,23,24,16,17,18,19,12,26,6,26,20,21,6,7,8,20,15,16,13,14,4,18,11,12,9,10,8,32,14,33,25,27,17,18,22,23,19,20,19,30,15,27,8,22,6,30,13,25,16,18,14,16,11,23,21,22,18,20,2,4,7,8,4,6,0,4,});
sparse_graph3_ints[39][100] = vector<int>({12,12,0,50,33,33,0,0,0,36,20,20,4,4,26,26,16,42,0,28,0,14,16,16,0,44,16,30,0,20,2,2,24,38,8,30,18,34,6,38,0,24,10,10,8,42,0,32,30,30,6,6,22,22,12,24,14,14,0,40,8,8,10,45,6,24,12,30,18,18,24,24,0,18,28,28,0,10,10,38,6,20,16,38,24,34,6,28,6,34,0,46,0,2,0,6,14,26,14,34,4,48,20,40,20,30,10,34,28,36,0,22,10,22,22,32,0,16,0,26,0,30,0,34,0,38,0,42,0,12,7,7,9,9,5,18,11,11,13,13,7,21,5,5,15,15,6,40,12,40,16,28,0,8,20,36,8,46,14,43,17,17,14,36,5,43,6,26,6,30,19,19,21,21,23,23,12,26,6,36,25,25,18,30,27,27,12,34,29,29,32,34,10,24,8,32,14,28,2,4,});
sparse_graph3_ints[38][6] = vector<int>({0,0,2,2,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[38][8] = vector<int>({0,0,2,3,0,4,1,3,2,2,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[38][10] = vector<int>({3,3,0,0,0,5,2,4,1,4,2,3,0,4,1,3,2,2,0,1,0,2,0,3,1,2,1,1,});
sparse_graph3_ints[38][12] = vector<int>({4,4,0,0,0,6,0,2,3,4,2,5,1,5,3,3,2,4,0,5,0,1,1,4,2,3,0,4,1,1,1,3,2,2,0,3,1,2,});
sparse_graph3_ints[38][14] = vector<int>({4,5,0,0,0,2,0,6,4,4,3,5,0,7,0,3,2,6,1,6,3,4,2,5,1,2,1,5,0,4,3,3,0,1,2,4,0,5,2,2,1,1,1,4,2,3,1,3,});
sparse_graph3_ints[38][16] = vector<int>({5,5,0,0,0,4,0,8,4,6,0,2,4,5,2,2,3,6,0,7,2,7,1,7,0,3,4,4,3,5,2,6,0,6,1,2,1,6,3,4,1,3,2,5,0,5,0,1,1,4,1,1,2,4,1,5,2,3,3,3,});
sparse_graph3_ints[38][18] = vector<int>({6,6,0,0,0,4,0,8,5,6,2,2,0,9,4,7,5,5,0,2,2,3,4,6,1,8,3,7,0,6,2,8,0,5,4,5,0,3,2,7,3,6,1,4,1,7,2,4,1,2,0,7,4,4,1,3,3,5,2,6,3,3,0,1,3,4,1,6,1,1,1,5,2,5,});
sparse_graph3_ints[38][20] = vector<int>({6,7,0,0,0,4,0,10,6,6,2,4,0,2,5,7,0,6,2,2,0,9,5,6,4,8,0,5,1,9,4,7,0,8,3,3,3,8,2,9,5,5,2,3,4,6,1,4,2,8,0,3,3,4,3,7,1,8,4,4,1,5,1,2,0,7,1,3,4,5,2,5,3,6,2,7,1,6,1,7,3,5,2,6,0,1,1,1,});
sparse_graph3_ints[38][23] = vector<int>({0,0,7,8,0,7,0,11,0,3,7,7,3,3,6,8,1,11,3,5,1,3,6,7,4,4,0,10,5,9,0,5,5,8,3,4,0,6,0,1,0,4,0,8,6,6,1,10,1,5,1,1,2,3,2,5,4,9,0,9,2,10,5,7,4,5,1,4,3,10,5,5,2,4,1,6,4,8,1,9,2,2,1,7,3,9,5,6,1,2,2,6,3,6,2,9,4,7,4,6,3,8,1,8,2,7,3,7,0,2,2,8,});
sparse_graph3_ints[38][26] = vector<int>({8,9,0,0,4,4,0,12,0,4,0,8,8,8,0,13,7,9,5,5,0,2,0,6,2,2,4,5,7,8,2,4,0,10,6,10,1,12,4,6,6,9,3,4,2,12,5,6,7,7,0,7,0,3,3,5,0,5,0,9,6,6,5,10,6,8,0,11,2,6,1,4,3,6,1,11,2,5,3,3,1,6,1,2,2,3,1,7,4,11,6,7,1,8,5,9,3,11,2,7,2,11,1,5,4,7,4,10,5,7,3,7,1,3,5,8,1,9,2,8,1,10,2,10,3,10,4,9,4,8,3,8,2,9,3,9,1,1,0,1,});
sparse_graph3_ints[38][30] = vector<int>({10,10,0,0,0,10,0,4,0,15,4,4,6,6,9,10,2,4,0,2,0,14,6,7,0,8,8,11,9,9,5,6,0,6,0,12,1,14,8,10,5,5,2,2,4,6,2,14,7,7,3,4,7,11,0,11,4,5,5,7,8,9,0,9,3,3,0,7,0,3,0,5,4,7,3,5,6,8,3,6,7,10,0,13,6,12,8,8,1,13,2,6,1,4,1,9,4,8,2,5,1,6,1,2,1,8,2,3,3,7,1,7,5,8,3,13,1,5,2,7,1,10,2,8,2,13,7,8,7,9,6,11,3,8,1,3,4,13,1,11,5,12,2,9,1,12,6,9,4,12,6,10,2,10,4,9,2,12,3,9,5,11,5,9,3,12,5,10,3,10,2,11,4,10,4,11,3,11,1,1,0,1,});
sparse_graph3_ints[38][35] = vector<int>({11,12,0,0,0,13,3,3,0,17,5,5,8,8,0,3,0,10,7,7,11,11,1,17,0,7,10,12,6,6,1,3,3,5,7,9,0,12,0,8,5,6,7,8,3,16,0,14,10,11,9,13,0,16,6,7,0,11,8,9,2,16,4,4,5,7,0,5,0,9,9,12,6,8,4,5,0,6,1,1,3,4,9,9,4,6,5,15,0,15,1,11,6,9,10,10,1,16,2,3,5,8,3,6,1,5,1,9,1,13,4,7,1,7,0,4,2,5,3,7,5,9,1,4,1,6,1,8,2,4,2,2,0,1,2,6,8,13,9,11,4,8,1,2,2,7,1,10,1,12,4,15,7,10,3,8,9,10,3,15,4,9,1,15,2,8,6,10,7,14,3,9,8,10,8,12,5,10,2,9,2,10,2,11,6,14,4,10,7,13,2,15,8,11,1,14,3,10,3,11,});
sparse_graph3_ints[38][40] = vector<int>({0,0,13,13,0,10,0,20,8,8,2,2,0,14,4,4,10,10,6,6,4,18,0,16,12,14,0,6,0,2,9,9,7,7,0,12,5,5,0,8,2,4,12,13,9,10,6,7,6,17,0,4,4,5,7,8,8,10,0,15,0,19,11,14,5,6,8,9,1,19,12,12,5,17,6,8,9,11,0,13,0,18,4,6,0,11,7,9,3,4,11,13,1,15,2,19,10,11,5,7,10,15,7,10,0,9,8,16,8,11,6,9,0,17,3,18,1,14,5,8,0,7,4,7,3,3,1,2,3,5,0,5,3,6,1,13,4,8,6,10,1,11,2,6,1,10,2,5,2,3,1,8,3,7,5,9,1,9,1,6,0,3,1,4,1,7,1,12,7,11,2,7,1,5,2,8,3,8,10,14,2,14,2,18,11,12,7,16,11,11,4,9,6,11,1,16,5,10,2,10,});
sparse_graph3_ints[38][45] = vector<int>({0,16,0,0,15,15,0,8,0,22,7,7,11,11,1,3,7,19,0,12,9,9,0,19,5,5,3,3,14,15,11,12,8,9,6,6,0,14,1,1,0,10,3,5,10,10,7,8,0,5,0,17,1,22,8,18,5,6,8,8,10,11,0,13,0,18,14,14,9,10,5,20,13,16,12,12,0,15,6,7,7,9,9,11,4,5,0,11,5,15,13,15,6,19,0,21,10,12,0,6,5,7,0,9,8,10,6,8,3,21,1,17,4,4,0,3,0,7,1,13,4,6,7,10,11,13,5,8,1,15,1,21,9,12,9,18,6,9,8,11,3,4,0,20,4,20,12,16,10,13,13,14,2,21,1,18,7,11,1,11,2,3,4,7,7,18,9,13,1,16,8,12,13,13,11,17,5,19,6,10,3,17,5,9,1,14,1,9,3,6,1,5,1,7,4,8,3,7,12,13,});
sparse_graph3_ints[38][50] = vector<int>({0,0,16,17,0,10,0,25,10,10,2,2,0,20,0,14,13,13,6,22,8,8,4,4,6,6,0,18,6,18,10,20,12,12,0,6,0,2,16,16,0,16,9,9,7,7,0,12,11,11,5,5,0,22,0,24,8,21,6,16,13,14,15,17,0,8,8,9,10,11,6,7,2,4,12,13,9,10,7,21,7,8,0,15,0,19,10,12,8,10,2,24,0,17,5,18,12,14,11,12,15,16,9,20,4,6,0,4,6,8,0,21,9,11,14,18,14,14,4,16,0,13,7,9,5,6,11,13,1,24,8,20,8,11,5,22,4,18,3,4,0,11,4,5,5,7,7,10,9,12,6,9,1,15,11,14,14,17,10,13,8,12,4,23,6,17,1,17,11,19,15,15,10,14,0,9,5,8,7,11,6,10,1,20,1,16,1,13,9,13,12,15,1,19,2,16,7,17,3,23,});
sparse_graph3_ints[38][55] = vector<int>({11,11,0,0,18,18,0,8,0,27,0,20,7,7,1,3,15,15,0,16,11,22,7,19,0,12,9,9,0,23,13,13,5,25,5,5,3,3,1,1,5,17,0,18,0,14,8,8,10,10,6,6,17,19,9,23,12,12,14,14,0,10,3,5,0,24,1,27,5,21,10,22,17,18,14,15,8,9,11,12,0,21,9,11,6,7,0,17,0,5,12,13,5,19,9,10,7,8,0,15,0,19,5,6,0,13,10,11,7,9,7,24,5,15,13,14,9,19,11,13,15,16,0,22,7,17,8,10,4,4,0,11,5,7,0,6,0,9,6,8,4,5,0,3,10,12,13,15,16,19,12,21,6,24,6,20,12,14,0,26,8,23,17,17,14,16,1,17,7,10,9,12,1,15,6,17,13,21,8,19,3,26,1,19,1,23,8,11,10,13,11,14,6,9,0,7,4,6,2,3,});
sparse_graph3_ints[38][60] = vector<int>({0,0,12,12,20,20,0,30,4,4,0,14,0,22,16,16,8,26,2,2,0,18,6,6,10,10,6,22,0,26,8,8,14,14,0,10,12,24,6,18,0,6,0,2,0,16,0,20,10,20,0,24,9,9,7,7,0,12,4,28,5,5,11,11,13,13,4,16,17,17,19,20,6,20,15,15,13,23,0,8,8,9,6,7,10,11,12,13,8,22,2,4,14,15,9,10,11,12,16,17,13,14,4,18,7,8,0,4,4,5,10,25,8,18,10,12,8,10,18,21,15,16,12,14,5,6,6,8,0,15,0,17,0,13,7,9,9,11,19,19,14,23,6,27,9,21,16,18,14,16,11,24,11,13,6,16,7,19,4,24,13,15,0,19,0,21,0,23,18,20,15,17,8,20,9,25,0,27,1,29,7,26,0,25,5,19,7,22,0,29,5,27,10,21,8,11,10,13,12,15,});
sparse_graph3_ints[38][70] = vector<int>({23,23,0,20,0,0,0,35,4,4,16,16,0,28,0,14,12,13,14,28,2,2,10,10,0,24,20,20,6,28,6,6,8,20,8,8,6,32,10,22,18,18,0,10,0,18,0,32,14,14,6,18,14,24,0,6,0,2,10,30,6,24,18,26,11,11,0,16,0,22,0,26,0,30,10,26,9,9,7,7,0,12,5,5,4,16,0,8,6,20,12,12,15,15,12,22,17,17,22,24,19,19,6,26,13,14,8,9,10,11,6,22,5,18,15,16,13,13,16,27,20,21,10,20,17,18,12,24,12,29,6,7,2,4,0,4,8,31,14,15,11,12,16,17,19,20,22,23,9,10,10,24,18,19,15,27,4,33,7,8,4,6,8,18,14,16,8,22,13,24,16,18,11,29,5,29,0,34,4,26,8,26,12,14,11,21,10,12,4,22,8,10,6,16,7,31,13,15,15,17,});
sparse_graph3_ints[38][80] = vector<int>({26,27,15,15,0,0,0,40,0,14,0,32,0,24,4,4,12,30,11,11,22,22,18,18,2,2,0,20,0,28,20,30,9,9,8,36,0,36,10,22,6,7,14,24,13,13,0,10,6,28,14,33,8,20,6,32,0,18,20,20,6,18,0,2,24,24,0,6,16,28,8,8,16,17,10,28,0,16,5,5,0,12,4,24,10,24,0,30,0,34,0,22,10,10,0,26,4,16,6,26,12,12,14,14,4,38,16,26,0,8,6,34,12,34,6,20,6,6,18,31,6,30,7,8,8,22,16,16,12,24,19,19,21,21,23,23,26,26,10,30,17,18,2,4,5,17,8,10,10,11,12,13,14,15,15,25,18,27,16,32,10,35,0,4,12,22,4,26,10,20,19,20,17,17,12,26,21,29,15,16,13,14,11,12,8,18,21,22,18,19,14,28,23,24,20,21,10,26,4,30,});
sparse_graph3_ints[38][100] = vector<int>({33,33,6,6,0,50,0,26,23,23,0,0,18,18,0,42,20,40,0,18,8,30,12,12,27,28,12,36,2,2,12,44,0,30,4,4,14,14,8,8,0,38,0,22,16,28,20,30,0,34,8,22,20,20,10,10,6,44,24,34,0,14,6,36,16,36,16,16,30,31,0,10,5,29,28,36,6,40,0,47,6,20,0,2,14,26,12,22,26,26,0,6,14,32,8,46,16,39,22,32,0,44,21,22,24,38,0,20,0,16,8,32,13,24,9,40,24,25,0,12,0,28,0,24,29,29,7,7,9,9,6,18,16,42,11,11,5,26,0,36,10,34,0,40,20,36,0,32,5,5,10,42,4,16,4,48,12,30,19,28,0,8,13,13,8,20,15,15,16,33,8,26,6,38,4,32,17,17,19,19,13,39,4,43,4,35,10,22,4,24,5,17,10,27,16,26,3,3,18,34,});
sparse_graph3_ints[37][6] = vector<int>({0,0,2,2,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[37][8] = vector<int>({2,3,0,0,0,4,1,3,2,2,0,3,1,2,0,2,0,1,1,1,});
sparse_graph3_ints[37][10] = vector<int>({3,3,0,0,0,5,2,4,0,4,2,3,1,4,0,2,1,3,2,2,0,1,0,3,1,2,1,1,});
sparse_graph3_ints[37][12] = vector<int>({4,4,0,0,0,6,0,2,3,4,2,5,1,5,3,3,2,4,0,5,0,4,0,1,1,4,2,3,1,1,0,3,2,2,1,3,1,2,});
sparse_graph3_ints[37][14] = vector<int>({4,5,0,0,0,2,0,6,4,4,0,3,3,5,0,7,2,6,0,4,1,6,3,4,2,5,1,2,1,5,2,2,3,3,2,4,0,1,0,5,1,1,1,4,2,3,1,3,});
sparse_graph3_ints[37][16] = vector<int>({5,5,0,0,0,4,0,8,4,6,0,2,2,2,4,5,3,6,0,7,0,3,2,7,1,7,4,4,0,6,3,5,2,6,1,2,1,3,1,6,0,5,3,4,2,5,2,3,1,4,1,1,2,4,0,1,3,3,1,5,});
sparse_graph3_ints[37][18] = vector<int>({6,6,0,0,0,4,0,8,5,6,0,2,2,4,0,9,4,7,2,2,5,5,0,6,4,6,1,8,0,5,3,7,2,8,0,3,2,3,4,5,1,4,2,7,3,6,1,2,3,3,1,7,0,7,4,4,1,3,3,5,3,4,2,6,1,5,1,6,1,1,2,5,0,1,});
sparse_graph3_ints[37][20] = vector<int>({6,7,0,0,0,4,0,10,2,4,6,6,0,2,0,6,5,7,2,2,3,3,0,9,5,6,4,8,0,8,4,4,1,9,4,7,0,5,0,3,3,4,5,5,2,9,3,8,2,3,1,4,4,6,0,7,2,8,1,2,1,5,2,5,3,7,1,8,1,3,4,5,1,6,3,5,3,6,2,6,2,7,1,7,1,1,0,1,});
sparse_graph3_ints[37][23] = vector<int>({7,8,0,0,0,7,0,11,0,3,3,3,7,7,4,4,1,11,6,8,1,3,3,5,6,7,0,10,0,6,5,5,1,1,0,5,5,9,0,8,3,4,5,8,0,4,4,5,1,5,1,10,6,6,0,9,2,3,0,1,1,4,2,5,1,2,2,10,1,7,4,9,2,4,5,7,1,6,2,2,3,10,3,6,5,6,1,9,2,6,4,8,3,9,4,6,4,7,2,9,1,8,2,7,3,7,3,8,0,2,2,8,});
sparse_graph3_ints[37][26] = vector<int>({8,9,0,0,0,8,2,2,0,13,5,5,8,8,0,6,0,2,0,12,4,4,7,9,6,6,0,4,0,10,2,4,4,5,7,8,1,12,6,10,4,6,3,4,5,6,2,12,6,9,0,9,0,7,0,3,0,5,3,5,3,3,7,7,3,6,2,3,1,2,2,5,1,4,2,6,1,6,0,11,6,8,4,7,5,10,1,7,1,11,1,8,6,7,1,5,2,7,4,11,5,7,3,11,1,3,3,7,2,11,5,9,1,9,2,8,4,10,5,8,1,10,4,8,3,8,2,10,3,10,4,9,2,9,3,9,1,1,0,1,});
sparse_graph3_ints[37][30] = vector<int>({10,10,0,0,6,6,2,2,0,15,4,4,0,10,0,6,0,2,9,10,7,7,0,14,5,5,0,8,0,12,8,11,6,7,2,14,9,9,0,4,2,4,5,6,8,10,4,5,6,8,1,14,0,11,3,4,4,6,0,9,5,7,3,3,0,3,0,7,0,5,3,5,1,2,2,5,2,3,4,7,7,11,2,6,1,4,3,6,5,8,8,9,1,6,3,13,7,8,1,9,0,13,8,8,6,12,4,8,1,8,1,10,1,7,1,5,3,7,2,7,1,13,7,10,4,13,2,8,1,11,3,8,2,13,7,9,1,3,6,11,2,10,5,12,6,9,2,9,4,9,4,12,1,12,5,9,3,9,6,10,2,12,5,11,3,12,3,10,5,10,4,10,2,11,4,11,3,11,1,1,0,1,});
sparse_graph3_ints[37][35] = vector<int>({11,12,0,0,5,5,0,17,1,3,0,12,8,8,0,8,3,3,7,7,5,15,11,11,0,14,1,1,6,6,9,9,1,17,0,10,3,5,0,5,10,12,7,9,5,6,0,13,6,7,4,4,0,9,7,8,0,6,0,3,0,11,4,5,5,7,8,9,9,13,3,16,0,16,10,11,0,7,6,8,1,13,2,3,4,6,3,4,2,16,0,15,4,15,6,9,9,12,1,11,5,8,3,6,1,9,1,5,4,7,0,4,1,7,7,10,2,5,1,2,2,4,3,7,1,4,1,6,10,10,5,9,6,14,1,16,8,10,1,12,1,8,4,8,2,6,1,10,2,7,8,13,6,10,9,11,9,10,7,14,3,8,5,10,3,15,2,2,4,9,1,15,2,8,2,12,3,9,2,9,2,11,1,14,2,10,4,10,7,13,5,11,8,12,3,11,5,14,8,11,});
sparse_graph3_ints[37][40] = vector<int>({0,0,13,13,0,10,0,20,8,8,2,2,0,14,4,4,10,10,6,6,6,17,0,16,0,6,0,2,0,12,7,7,12,14,9,9,5,5,0,8,0,4,2,4,6,7,9,10,4,18,12,13,4,5,7,8,10,11,8,9,5,6,8,16,0,17,0,13,8,10,6,8,0,15,1,19,4,14,11,14,5,17,9,11,0,11,7,9,4,6,3,4,12,12,0,19,5,7,7,16,0,18,2,19,8,11,6,9,1,15,11,11,7,10,11,13,6,16,0,9,5,8,3,3,1,2,0,7,1,13,10,15,4,7,3,5,6,10,8,12,3,18,1,14,7,11,1,16,1,11,0,5,3,6,4,8,5,9,1,12,2,14,1,9,2,6,2,3,2,5,0,3,1,10,3,7,1,6,1,8,1,4,1,7,1,5,2,7,4,9,2,8,3,8,2,12,6,11,10,12,});
sparse_graph3_ints[37][45] = vector<int>({0,16,0,0,15,15,0,8,0,22,11,11,7,7,1,3,7,19,0,12,9,9,0,19,5,5,3,3,1,1,5,15,12,12,0,14,8,8,6,6,0,10,4,4,9,10,0,6,14,15,5,20,7,8,5,6,9,18,11,12,10,10,0,18,0,15,8,9,1,22,5,16,6,7,0,13,0,17,7,9,10,11,14,14,8,18,13,16,11,13,3,21,0,20,9,11,0,11,3,5,5,7,0,5,0,9,0,3,4,5,6,8,8,10,3,15,6,19,10,12,13,15,7,15,12,13,0,7,2,3,4,6,1,13,7,10,3,4,1,15,5,8,6,9,8,11,3,17,0,21,4,20,1,17,9,17,9,12,7,11,1,21,10,13,6,15,7,18,13,13,12,16,2,21,11,17,5,19,9,13,13,14,1,19,5,13,1,11,4,7,6,10,4,16,10,17,8,12,1,14,});
sparse_graph3_ints[37][50] = vector<int>({0,10,16,17,0,0,0,18,0,25,12,12,2,2,8,8,10,20,0,14,4,4,0,22,10,10,6,6,6,18,14,14,6,22,0,6,0,2,0,20,0,16,6,16,0,12,7,7,9,9,5,5,11,11,13,13,16,16,0,8,4,14,2,4,0,4,7,8,9,10,5,6,11,12,8,9,4,16,8,21,10,11,13,14,15,17,7,18,2,24,12,13,11,19,6,8,7,21,0,24,4,18,8,10,10,12,6,7,0,15,0,13,0,17,0,21,4,23,12,14,7,17,0,19,9,11,7,9,4,6,9,20,5,15,3,4,0,11,4,5,11,13,15,16,5,18,5,7,8,11,6,9,7,10,9,12,5,22,14,18,13,15,2,14,10,13,6,14,11,14,8,20,5,19,12,19,1,24,8,18,14,15,5,17,10,14,12,15,10,19,14,17,8,12,8,16,15,15,6,21,});
sparse_graph3_ints[37][55] = vector<int>({0,0,18,18,11,11,0,8,0,27,7,7,0,21,0,16,9,23,1,3,15,15,0,12,9,9,7,17,0,24,13,13,5,5,0,18,9,19,3,3,1,1,5,25,13,21,5,15,0,14,5,21,8,8,6,6,0,10,4,4,10,10,12,12,0,6,14,14,17,19,5,17,15,16,11,22,7,19,7,24,1,27,0,23,0,20,7,8,9,10,11,12,13,14,5,6,3,15,8,9,10,11,5,19,12,13,6,7,0,15,0,13,7,9,0,17,0,19,9,11,6,17,0,22,6,21,14,15,17,18,0,25,6,24,12,21,11,13,10,19,4,5,0,11,5,7,0,3,3,4,0,5,0,9,6,8,8,10,16,16,4,16,10,12,13,15,9,17,10,22,3,13,12,14,3,26,6,20,8,23,5,18,14,16,8,19,10,13,8,11,5,16,7,10,9,12,11,14,16,19,});
sparse_graph3_ints[37][60] = vector<int>({20,20,8,8,0,0,0,24,4,4,8,26,11,11,0,18,0,30,2,2,8,20,0,14,17,17,6,6,13,13,0,10,6,18,12,24,15,15,0,20,0,26,9,9,0,2,0,6,0,16,4,24,12,20,0,22,0,12,4,28,6,16,7,7,10,10,5,5,0,8,4,14,12,12,14,14,8,22,14,23,8,18,16,16,19,20,3,3,0,4,4,22,4,16,10,11,12,13,14,15,17,18,10,20,10,25,6,27,0,28,8,9,6,7,7,8,9,10,11,12,4,18,6,20,6,24,13,14,15,16,13,23,10,18,16,17,11,21,5,6,9,25,15,22,19,19,12,14,14,16,16,18,18,21,10,12,4,20,8,16,6,22,11,24,5,27,10,22,13,15,9,19,11,13,4,5,8,10,15,17,18,18,11,20,9,11,6,8,14,22,7,26,9,21,7,19,5,24,18,20,});
sparse_graph3_ints[37][70] = vector<int>({0,14,0,0,23,23,0,35,14,14,4,4,0,28,10,10,14,28,0,24,0,4,0,18,20,20,6,28,6,6,16,16,8,22,6,18,8,8,6,32,12,22,12,12,0,10,0,32,18,18,0,20,0,2,14,24,0,6,18,26,10,30,8,20,0,16,6,26,0,12,0,26,0,22,0,30,6,16,10,26,7,7,9,9,5,5,0,8,4,14,4,24,11,11,13,13,8,18,15,15,4,22,3,3,12,20,17,17,19,19,21,21,4,16,2,2,4,26,10,24,10,20,14,22,22,24,16,24,16,27,12,29,8,28,4,33,4,30,8,24,8,31,12,26,4,20,14,15,12,13,16,17,10,11,8,9,7,8,9,10,11,12,13,14,10,18,15,16,18,19,6,7,4,18,10,22,6,22,17,18,12,24,4,28,20,21,4,6,6,20,8,16,6,24,12,14,14,16,10,12,});
sparse_graph3_ints[37][80] = vector<int>({4,4,0,22,26,27,18,18,0,0,0,32,12,34,0,14,14,14,0,40,10,10,22,22,0,4,12,28,0,28,0,18,6,6,8,22,0,36,20,30,12,12,8,8,8,28,16,16,6,34,20,20,0,10,16,28,0,24,0,2,12,22,8,18,24,24,0,6,6,16,8,36,4,28,14,24,4,32,16,32,0,16,0,12,0,26,0,20,7,7,9,9,10,32,5,5,4,38,0,8,4,24,0,30,16,26,8,24,0,34,4,20,4,14,8,20,11,11,13,13,15,15,19,27,8,30,8,26,0,38,17,17,12,20,3,3,4,16,4,26,4,30,11,25,19,19,4,22,21,21,12,30,16,24,4,34,23,23,22,29,14,22,7,17,2,2,10,20,11,27,14,30,18,31,14,33,8,32,10,35,18,26,6,37,4,18,10,11,12,13,14,15,11,12,13,14,26,26,10,18,});
sparse_graph3_ints[37][100] = vector<int>({33,33,0,50,0,0,22,23,0,34,0,24,8,8,4,4,0,42,15,15,14,24,10,38,24,38,12,12,29,29,2,2,20,30,0,20,17,38,0,30,13,30,6,6,8,46,0,38,17,17,10,10,0,46,26,26,0,14,20,20,6,26,16,42,6,30,6,20,0,2,24,32,0,10,0,18,12,34,5,24,0,6,6,44,4,38,10,30,18,34,28,36,24,24,0,26,5,17,12,41,14,14,12,22,0,16,31,31,18,19,5,40,19,28,22,36,15,26,0,12,7,7,9,9,5,33,10,21,20,40,0,22,0,32,15,34,9,33,0,28,12,44,27,28,11,11,13,13,5,5,21,21,26,34,0,36,11,25,14,37,0,44,18,26,4,48,8,20,6,18,5,42,22,31,0,40,6,28,16,16,0,8,8,36,15,31,4,16,10,40,5,35,10,28,0,48,20,35,4,14,});
sparse_graph3_ints[36][6] = vector<int>({2,2,0,0,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[36][8] = vector<int>({2,3,0,0,0,4,2,2,1,3,0,3,1,2,0,2,0,1,1,1,});
sparse_graph3_ints[36][10] = vector<int>({3,3,0,0,0,5,2,4,0,2,1,4,2,3,0,4,1,3,2,2,0,1,0,3,1,2,1,1,});
sparse_graph3_ints[36][12] = vector<int>({4,4,0,0,0,2,0,6,3,4,2,5,1,5,1,1,3,3,2,4,0,5,0,4,0,3,1,4,2,3,0,1,1,2,2,2,1,3,});
sparse_graph3_ints[36][14] = vector<int>({4,5,0,0,0,3,0,6,4,4,0,2,0,7,3,5,0,4,2,6,1,6,2,2,3,4,2,5,1,2,1,5,3,3,1,1,2,4,0,5,1,3,0,1,2,3,1,4,});
sparse_graph3_ints[36][16] = vector<int>({5,5,0,0,0,4,0,8,0,2,4,6,2,2,4,5,0,6,3,6,0,3,0,7,4,4,2,7,1,7,2,3,3,5,1,2,2,6,1,3,0,5,1,4,1,6,3,4,1,1,2,4,2,5,3,3,1,5,0,1,});
sparse_graph3_ints[36][18] = vector<int>({6,6,0,0,0,4,0,8,0,2,2,4,5,6,0,9,2,2,4,7,0,6,5,5,4,6,3,3,0,3,1,8,0,5,3,7,2,8,2,3,4,5,1,2,1,4,4,4,0,7,2,7,3,6,3,4,1,3,1,7,1,5,3,5,1,1,2,5,2,6,1,6,0,1,});
sparse_graph3_ints[36][20] = vector<int>({6,7,0,0,2,2,0,10,0,6,0,2,6,6,4,4,0,4,5,7,2,4,3,3,0,9,5,6,0,8,4,8,0,3,3,4,1,9,0,5,4,7,5,5,2,3,1,2,0,7,2,9,2,5,3,8,1,4,4,6,1,5,4,5,2,8,1,3,3,5,1,6,1,8,3,7,1,1,2,6,3,6,1,7,2,7,0,1,});
sparse_graph3_ints[36][23] = vector<int>({7,8,0,0,1,3,0,11,5,5,3,3,0,8,7,7,1,1,1,11,4,4,6,8,0,5,0,7,0,3,4,5,0,6,6,7,0,10,3,5,5,9,0,4,3,4,2,3,0,9,5,8,1,5,1,2,2,5,6,6,2,10,1,7,1,4,2,2,0,1,2,4,3,6,1,10,1,6,4,6,4,9,5,7,5,6,3,10,2,6,1,9,3,9,4,8,2,7,1,8,4,7,3,7,2,9,0,2,3,8,2,8,});
sparse_graph3_ints[36][26] = vector<int>({0,0,8,9,4,4,0,13,2,2,0,10,6,6,0,6,0,2,8,8,0,8,5,5,0,12,7,9,0,4,3,4,5,6,2,12,2,4,4,6,7,8,6,10,1,12,0,9,4,5,3,3,0,3,0,7,0,5,3,5,1,2,2,3,5,7,6,9,3,6,2,5,7,7,1,4,2,6,4,7,0,11,1,6,1,5,1,8,6,7,5,10,1,7,6,8,3,11,3,7,4,11,1,9,1,11,2,7,1,3,2,8,2,11,5,8,5,9,4,10,1,10,4,8,1,1,3,8,2,10,2,9,3,10,4,9,3,9,0,1,});
sparse_graph3_ints[36][30] = vector<int>({0,0,10,10,6,6,2,2,0,15,4,4,0,10,0,6,0,2,7,7,0,12,9,10,2,14,0,8,5,5,0,4,2,4,6,7,0,14,8,11,4,5,6,8,9,9,4,13,5,6,0,11,8,10,7,8,3,4,4,6,1,14,0,9,5,7,3,3,1,2,0,7,3,5,0,5,8,8,7,11,5,8,4,7,3,13,8,9,0,13,6,12,3,6,2,3,0,3,2,5,1,10,4,8,2,6,1,4,1,6,1,9,1,8,1,5,1,7,1,11,3,7,1,3,1,1,2,7,2,8,6,9,5,12,2,10,3,8,7,9,1,13,7,10,2,13,4,9,5,9,4,12,1,12,2,9,6,11,3,9,6,10,3,10,2,12,4,10,5,11,3,12,2,11,5,10,4,11,3,11,0,1,});
sparse_graph3_ints[36][35] = vector<int>({11,12,0,0,0,8,0,17,7,7,1,3,0,12,9,9,5,5,5,15,3,3,0,14,1,1,0,10,6,6,8,8,11,11,4,4,0,6,5,6,7,8,1,17,10,12,8,9,0,13,6,14,6,7,0,11,0,15,3,5,0,3,0,5,0,9,5,7,4,5,7,9,0,7,2,3,3,4,4,6,6,8,3,16,1,13,9,13,8,10,10,11,4,15,0,16,1,11,6,9,5,8,1,2,1,5,3,6,1,9,4,7,2,4,1,7,0,4,7,10,2,5,2,16,5,9,9,10,7,14,3,7,9,12,10,10,6,10,1,4,1,12,4,8,1,6,2,2,2,6,1,8,5,11,1,10,8,13,1,14,1,16,3,15,9,11,5,10,2,7,3,8,2,12,4,9,5,14,3,9,3,11,7,11,1,15,7,13,2,8,2,13,2,11,2,9,6,11,4,10,});
sparse_graph3_ints[36][40] = vector<int>({13,13,0,0,0,10,0,20,8,8,2,2,0,14,4,4,10,10,6,6,8,16,0,16,0,6,0,2,4,18,0,12,4,14,0,8,5,5,7,8,12,14,10,11,9,9,6,7,2,4,0,4,4,5,7,7,6,17,9,10,12,13,0,18,8,9,5,6,11,11,6,8,8,10,0,13,5,14,0,15,5,17,0,11,3,4,4,6,4,12,7,16,1,19,0,17,7,9,9,11,5,7,11,14,0,9,12,12,6,9,8,11,6,16,2,19,10,12,2,14,7,10,5,8,0,7,2,3,1,1,3,3,3,5,6,14,10,15,0,19,4,7,6,10,1,13,1,16,3,18,8,12,5,13,9,15,11,13,9,12,11,12,1,12,7,11,1,15,5,9,6,12,1,11,1,14,4,8,2,16,4,17,6,11,8,15,7,12,2,12,3,6,5,10,10,14,6,13,0,5,});
sparse_graph3_ints[36][45] = vector<int>({0,14,0,0,15,15,0,22,3,3,9,9,5,5,0,18,12,12,7,19,7,7,1,1,0,12,0,8,5,15,1,3,11,11,0,16,0,10,6,6,8,8,0,20,3,21,9,18,5,17,10,10,4,4,0,6,5,13,14,15,12,13,7,8,5,6,10,11,8,9,6,7,0,13,0,15,7,15,9,10,0,17,7,9,0,11,3,5,5,7,0,3,0,5,0,9,4,5,3,13,5,20,0,19,11,12,8,18,1,22,5,16,9,11,6,8,3,15,8,10,11,13,14,14,10,17,13,16,6,19,0,7,2,3,3,4,4,6,13,13,10,12,6,16,1,13,6,9,4,14,7,10,5,8,8,11,3,17,11,17,13,15,10,13,4,20,9,17,9,12,1,11,1,15,6,15,0,21,1,17,7,18,2,21,1,19,7,11,4,16,5,19,9,13,5,11,5,14,7,16,});
sparse_graph3_ints[36][50] = vector<int>({16,17,0,0,8,8,0,25,4,4,12,12,0,18,0,4,0,14,10,20,6,6,0,22,10,10,0,10,6,16,14,14,6,22,0,2,0,6,8,18,0,16,0,12,0,20,4,14,0,8,7,7,5,5,9,9,4,20,11,11,3,3,13,13,4,16,2,2,8,16,16,16,11,19,4,23,10,11,8,9,6,7,4,12,9,10,13,14,11,12,4,18,8,21,7,8,5,6,6,14,6,18,12,13,15,17,14,15,12,19,7,21,0,24,2,24,9,17,10,12,4,5,8,10,6,8,0,13,0,15,0,17,0,19,0,21,5,15,12,14,5,19,9,20,9,11,7,9,11,13,5,17,5,22,0,11,5,7,4,6,3,4,3,14,7,18,5,16,8,11,10,13,13,15,7,17,9,12,5,14,7,10,6,9,7,15,11,14,14,18,8,20,15,16,12,15,10,19,8,17,});
sparse_graph3_ints[36][55] = vector<int>({0,12,0,27,0,0,18,18,11,11,3,3,0,22,14,14,0,16,9,23,5,5,9,9,1,1,7,7,5,21,5,17,0,18,0,8,16,16,1,3,9,17,13,21,5,25,0,25,0,14,12,12,5,15,0,20,11,19,0,10,6,6,8,8,4,4,10,10,0,6,5,13,5,19,13,13,15,15,7,21,0,23,7,17,17,19,9,19,11,22,7,24,1,27,3,13,11,12,9,10,7,8,6,7,8,9,10,11,13,14,7,15,5,6,3,23,12,13,7,19,3,17,0,21,0,17,0,19,0,13,0,15,0,11,4,5,15,16,3,15,7,9,9,11,3,19,0,24,14,15,6,24,12,21,11,13,10,19,6,16,3,21,8,17,5,7,0,4,3,4,0,9,6,8,8,10,10,12,6,14,3,26,6,21,13,15,10,22,17,18,12,14,6,18,10,18,8,23,14,20,});
sparse_graph3_ints[36][60] = vector<int>({20,20,4,4,0,22,0,0,0,30,0,14,10,10,16,16,0,4,8,26,8,18,0,18,6,6,0,26,8,8,12,12,14,23,0,10,14,14,6,24,0,2,6,16,0,6,12,20,0,16,18,18,0,12,8,20,0,24,0,20,4,14,4,28,0,8,7,7,5,5,4,20,9,9,12,24,4,22,0,28,11,11,3,3,13,13,4,16,15,15,10,20,17,17,8,16,4,24,10,22,2,2,4,12,10,18,4,18,16,22,10,25,6,27,19,20,10,11,8,9,6,7,12,13,6,14,9,10,11,12,14,15,6,18,8,22,13,14,13,20,15,16,6,20,7,8,16,17,17,18,13,23,12,18,6,22,9,17,5,6,10,12,12,14,4,5,9,25,11,21,14,16,4,25,9,20,5,27,8,23,11,24,15,22,13,15,11,13,9,18,16,18,12,21,11,19,15,17,7,16,});
sparse_graph3_ints[36][70] = vector<int>({0,0,13,13,6,32,6,6,23,23,0,28,17,17,0,16,0,4,10,10,0,24,14,28,4,4,8,26,0,20,0,32,8,20,19,19,8,8,15,15,0,35,0,10,0,2,6,16,0,14,12,26,0,6,18,26,12,20,21,21,4,20,4,24,14,22,12,12,0,18,0,12,10,30,0,30,8,24,4,30,4,14,7,7,0,22,5,5,0,26,16,24,0,8,8,18,9,9,11,11,4,26,14,14,8,28,4,16,8,22,3,3,4,22,4,28,16,16,12,22,18,18,8,16,20,20,4,12,10,18,2,2,1,2,0,1,4,33,10,20,4,18,14,24,10,26,12,29,8,31,16,27,10,24,6,14,10,11,9,10,11,12,12,13,6,18,13,14,6,20,14,15,6,24,6,22,10,22,15,16,14,20,6,26,11,19,16,22,12,24,16,17,6,28,17,18,8,9,6,8,});
sparse_graph3_ints[36][80] = vector<int>({0,0,17,17,0,40,0,20,26,27,4,4,0,30,12,28,7,7,13,13,12,34,0,4,0,24,20,20,8,22,0,36,0,16,11,11,20,30,9,9,10,24,18,26,0,10,4,30,15,15,24,24,0,2,9,33,0,26,0,6,8,18,0,14,6,6,22,22,4,38,8,30,4,18,16,32,0,18,0,34,16,24,0,12,14,30,18,18,8,8,4,34,10,26,0,28,10,10,0,22,12,20,4,28,5,5,14,22,4,24,8,36,12,12,4,16,0,32,10,18,0,8,4,20,0,38,20,27,4,14,14,14,4,26,4,22,16,16,8,16,22,29,4,32,8,24,19,19,12,26,8,32,15,27,2,3,21,21,0,1,1,2,5,35,3,4,7,27,12,30,17,29,23,23,4,12,13,21,15,23,11,19,7,21,7,29,25,25,7,15,18,24,4,5,5,6,6,7,7,8,});
sparse_graph3_ints[36][100] = vector<int>({0,0,0,26,0,14,0,50,23,23,20,40,5,34,3,4,33,33,11,11,15,15,28,28,0,44,16,36,12,24,0,18,12,44,0,30,1,2,20,20,0,10,15,32,0,22,0,34,8,38,13,13,26,34,9,9,12,29,22,30,17,17,0,41,10,22,6,6,6,42,6,22,6,47,2,3,25,25,6,29,8,8,6,18,10,34,0,38,18,38,28,36,0,47,10,42,18,27,31,31,0,20,0,16,24,32,21,22,4,5,10,28,10,10,0,1,5,27,7,7,5,37,18,18,24,38,0,24,20,28,11,39,0,12,0,28,15,24,16,42,18,32,20,34,0,32,14,14,26,27,6,20,0,36,12,12,22,36,29,30,12,36,16,26,14,40,5,45,5,25,5,17,16,16,14,30,4,40,10,20,5,32,10,24,0,8,8,33,19,19,4,15,5,6,0,6,0,2,0,4,});
sparse_graph3_ints[35][6] = vector<int>({2,2,0,0,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[35][8] = vector<int>({2,3,0,0,0,4,2,2,1,3,0,3,1,2,0,2,0,1,1,1,});
sparse_graph3_ints[35][10] = vector<int>({3,3,0,0,0,5,0,2,2,4,0,4,2,3,1,4,1,3,0,1,2,2,0,3,1,2,1,1,});
sparse_graph3_ints[35][12] = vector<int>({4,4,0,0,0,2,0,6,3,4,0,3,2,5,3,3,1,5,2,4,1,1,0,5,0,4,1,2,1,4,2,3,0,1,2,2,1,3,});
sparse_graph3_ints[35][14] = vector<int>({4,5,0,0,0,2,0,6,2,2,4,4,0,7,3,5,0,4,2,6,0,3,1,6,3,4,1,2,2,5,3,3,1,1,1,5,0,5,2,4,1,3,2,3,1,4,0,1,});
sparse_graph3_ints[35][16] = vector<int>({5,5,0,0,0,4,0,8,0,2,4,6,2,2,4,5,2,3,0,6,3,6,0,3,0,7,4,4,1,7,2,7,0,5,1,2,3,5,2,4,2,6,1,3,1,4,1,1,3,3,3,4,1,6,2,5,1,5,0,1,});
sparse_graph3_ints[35][18] = vector<int>({6,6,0,0,3,3,0,9,0,2,0,4,5,6,0,6,0,8,2,2,4,7,5,5,2,4,4,6,0,3,0,5,1,8,4,4,2,8,2,3,1,2,3,4,3,7,1,4,4,5,0,7,1,3,1,1,1,5,2,7,3,6,1,7,2,5,3,5,1,6,2,6,0,1,});
sparse_graph3_ints[35][20] = vector<int>({0,0,6,7,2,2,0,10,0,6,0,2,4,4,6,6,0,4,2,4,5,7,0,8,3,4,0,3,3,3,1,9,5,6,4,8,0,9,0,5,0,7,1,2,5,5,2,3,4,7,2,5,4,5,2,9,1,4,1,5,1,1,3,5,1,3,3,8,1,6,4,6,2,8,2,6,1,8,3,7,3,6,1,7,2,7,0,1,});
sparse_graph3_ints[35][23] = vector<int>({0,0,7,8,0,8,1,3,0,11,5,5,3,3,1,1,7,7,4,4,1,11,0,5,6,8,0,7,0,3,4,5,2,3,0,6,3,5,0,10,6,7,5,9,0,9,3,4,5,6,0,4,1,2,1,5,2,4,4,6,2,10,1,7,5,8,6,6,2,5,1,4,3,6,2,2,1,10,1,6,0,2,3,10,5,7,2,6,4,9,1,9,2,7,1,8,3,9,3,7,4,7,4,8,0,1,2,9,2,8,3,8,});
sparse_graph3_ints[35][26] = vector<int>({8,9,0,0,4,4,0,13,2,2,0,10,6,6,0,6,0,2,0,8,8,8,5,5,2,12,2,4,7,9,0,12,5,6,0,4,4,5,3,4,6,10,6,7,0,9,4,6,7,8,1,12,3,3,0,3,0,7,5,7,0,5,3,5,7,7,6,9,0,11,3,11,4,7,3,6,2,3,1,1,1,4,2,5,1,2,2,6,1,5,1,8,1,3,1,6,1,9,4,11,1,7,3,7,6,8,5,10,2,8,2,7,4,8,1,11,5,8,2,11,1,10,4,10,3,8,5,9,2,9,2,10,3,10,4,9,3,9,0,1,});
sparse_graph3_ints[35][30] = vector<int>({0,0,10,10,0,10,2,2,0,15,4,4,7,7,0,6,0,2,6,6,0,12,4,13,0,8,5,5,9,10,0,4,3,3,7,8,4,5,6,7,0,14,2,14,8,11,5,6,6,8,0,11,9,9,6,12,8,8,3,4,4,6,0,9,5,7,0,7,2,3,1,1,2,4,8,10,3,13,1,11,1,14,3,5,5,8,0,5,1,2,4,7,3,6,1,4,2,5,0,13,0,3,2,6,1,3,1,5,1,10,1,6,1,8,4,8,5,12,8,9,7,11,6,9,1,9,1,7,3,7,2,10,5,9,2,7,4,12,7,9,2,8,4,9,3,8,1,12,7,10,2,13,3,9,4,10,2,9,1,13,6,11,3,10,6,10,2,11,2,12,5,10,5,11,3,12,3,11,4,11,0,1,});
sparse_graph3_ints[35][35] = vector<int>({11,12,0,0,3,3,7,7,0,17,0,13,5,5,1,1,9,9,0,10,5,15,1,3,0,8,0,15,0,6,6,6,4,4,8,8,0,12,11,11,3,13,1,17,6,14,6,7,8,9,5,6,7,8,0,11,0,14,3,5,0,3,0,5,0,9,5,7,4,5,10,12,7,9,9,10,3,16,0,7,2,3,3,4,6,8,4,6,7,14,5,11,1,11,8,10,9,13,4,15,1,13,0,16,10,11,5,8,6,9,4,7,7,10,3,11,1,9,0,4,0,2,2,4,3,6,2,2,1,5,1,7,2,5,1,4,1,2,2,16,10,10,5,9,4,12,3,7,4,8,5,14,6,10,8,13,9,12,7,11,9,11,7,13,1,12,3,15,1,14,1,10,5,10,1,6,6,11,4,9,2,6,1,8,3,9,2,12,3,8,2,7,2,13,1,16,2,10,4,10,});
sparse_graph3_ints[35][40] = vector<int>({13,13,0,0,0,10,0,20,8,8,0,4,0,16,4,4,10,10,8,16,6,6,0,14,0,2,0,6,4,14,0,12,4,18,0,8,5,5,7,7,11,11,9,9,3,3,0,18,4,12,6,14,2,2,12,14,6,17,6,7,8,9,10,11,5,6,7,8,4,5,9,10,6,8,3,4,4,6,0,11,0,13,12,13,8,10,7,16,5,14,5,7,7,9,0,15,2,12,2,19,0,17,0,9,10,12,5,13,5,17,9,11,9,15,12,12,6,9,2,14,5,8,7,10,4,15,11,14,8,11,6,16,11,12,10,15,9,12,6,10,6,12,8,12,1,19,2,16,3,18,4,10,3,13,5,15,8,15,0,19,7,11,3,15,7,13,5,12,1,15,1,13,1,11,4,17,6,13,1,16,6,11,5,9,1,14,4,8,5,10,7,12,11,13,7,15,4,13,5,11,});
sparse_graph3_ints[35][45] = vector<int>({0,14,0,0,15,15,0,22,3,3,10,10,5,5,0,18,7,7,9,18,1,1,12,12,5,17,0,8,0,12,5,13,1,3,8,8,0,10,3,21,0,16,6,6,0,20,4,4,5,15,0,6,7,19,9,9,11,11,13,13,7,15,3,13,11,17,14,15,5,20,7,8,5,6,3,11,9,15,10,11,8,9,6,7,9,10,11,12,3,15,0,4,4,5,0,11,3,4,0,9,0,13,5,7,3,17,0,15,7,9,0,17,7,13,0,19,12,13,8,18,6,16,9,11,0,2,2,2,0,7,0,5,3,5,4,6,6,8,5,11,8,10,4,14,1,22,6,19,10,12,4,17,11,13,10,17,14,14,7,16,6,14,4,20,4,13,1,11,1,13,5,8,7,10,6,9,8,11,0,21,6,15,1,15,1,17,1,19,4,15,13,16,9,12,6,17,9,17,7,11,});
sparse_graph3_ints[35][50] = vector<int>({16,17,0,0,8,8,0,25,4,4,12,12,0,18,0,4,0,14,10,20,6,6,0,22,10,10,0,10,6,16,14,14,0,2,6,22,0,6,8,18,4,14,0,12,0,20,0,16,0,8,5,5,7,7,4,20,9,9,3,3,11,11,4,12,6,14,8,16,13,13,12,19,4,18,2,2,4,23,4,16,16,16,8,21,14,15,9,17,10,11,12,13,11,12,9,10,8,9,6,7,7,8,3,11,5,6,6,18,4,5,13,14,11,19,2,24,0,24,7,21,10,16,5,20,10,12,7,16,8,10,6,8,3,4,0,11,4,6,0,9,5,7,5,13,0,13,3,13,7,9,5,15,0,15,0,17,0,19,0,21,9,11,8,14,10,18,15,17,15,15,12,14,9,20,5,22,5,19,11,13,5,17,5,14,2,18,0,23,2,20,8,17,14,18,13,15,7,15,2,16,});
sparse_graph3_ints[35][55] = vector<int>({11,11,3,3,18,18,0,27,0,0,0,14,14,14,0,18,5,5,5,21,9,9,0,21,13,21,7,7,1,1,5,25,7,17,0,8,0,12,1,3,11,19,0,23,5,15,16,16,9,23,0,16,12,12,0,10,0,25,6,6,8,8,4,4,9,17,10,10,5,13,7,21,3,19,0,6,13,13,7,15,15,15,3,13,0,19,5,19,9,19,3,23,3,11,0,3,0,5,1,27,7,24,11,22,5,6,6,7,7,8,4,5,3,4,0,11,0,9,3,15,8,9,9,10,5,17,3,21,3,17,10,11,9,15,11,17,7,19,11,12,0,13,0,15,0,17,0,20,6,14,5,23,12,13,0,22,13,14,17,19,14,20,15,16,0,24,3,26,9,21,0,7,2,3,0,1,4,12,5,7,14,15,16,17,12,19,1,2,7,9,9,11,6,16,8,17,0,4,3,5,});
sparse_graph3_ints[35][60] = vector<int>({0,0,20,20,6,6,0,30,8,18,0,16,0,4,10,10,16,16,12,24,0,20,4,4,0,26,8,8,12,12,6,24,0,10,14,14,0,2,0,14,0,22,6,16,8,26,0,6,18,18,10,22,4,14,0,12,8,20,0,24,0,18,4,28,16,22,7,7,5,5,4,20,0,8,12,18,0,28,14,20,4,22,9,9,11,11,4,16,13,13,3,3,4,12,8,16,4,24,15,15,17,17,4,18,1,2,0,1,12,20,8,22,10,18,2,2,6,14,14,23,10,25,4,26,6,27,3,11,6,22,10,20,6,20,10,16,8,24,6,7,8,9,7,8,9,10,10,11,6,18,11,12,5,6,4,5,2,3,3,4,0,11,0,9,12,13,8,14,13,14,0,7,14,15,12,22,0,5,15,16,5,13,0,13,0,15,4,6,5,7,6,8,0,17,3,13,8,10,7,9,});
sparse_graph3_ints[35][70] = vector<int>({0,14,15,15,23,23,0,0,8,31,0,28,4,4,11,11,0,22,19,19,0,4,0,35,0,18,16,27,7,23,9,9,6,28,0,10,13,13,7,7,4,22,14,22,0,2,0,32,17,17,0,6,21,21,6,6,8,16,10,18,6,14,10,26,12,20,0,16,0,26,0,12,16,24,0,20,12,29,8,8,0,30,5,5,0,24,4,33,4,18,10,24,4,26,0,8,20,25,10,10,4,14,4,20,12,12,8,18,14,14,13,26,4,24,6,30,4,28,16,16,10,20,2,3,4,12,18,18,0,1,1,2,7,25,20,20,3,4,7,21,16,22,14,20,3,31,10,28,7,15,3,17,10,22,18,26,14,28,12,24,3,11,4,5,6,7,5,6,0,11,0,9,18,24,10,30,8,28,6,32,14,24,0,13,3,3,0,7,12,18,8,26,12,22,22,22,7,8,8,9,});
sparse_graph3_ints[35][80] = vector<int>({0,0,0,40,11,23,0,16,26,27,0,8,0,24,16,32,12,12,0,32,22,22,15,15,1,2,0,12,19,19,0,20,8,36,0,28,10,10,15,27,0,36,8,8,8,32,8,22,2,3,8,18,12,26,17,17,5,5,20,30,5,17,4,28,7,7,4,32,13,31,0,18,0,14,19,26,0,1,8,28,13,13,9,9,3,4,25,25,0,26,6,6,0,22,16,24,4,38,0,10,4,24,12,34,12,20,14,22,11,11,10,18,0,30,20,20,4,20,0,34,0,38,4,16,23,23,4,34,12,28,8,24,17,29,14,14,4,22,21,27,4,30,4,26,16,16,8,30,4,13,0,6,8,34,18,18,8,26,18,24,0,2,4,5,0,4,23,28,8,16,4,36,12,30,21,21,5,14,16,22,24,24,11,19,3,12,11,25,7,21,16,28,14,20,0,11,0,13,6,7,});
sparse_graph3_ints[35][100] = vector<int>({18,18,33,33,16,42,0,24,0,8,0,40,0,0,0,16,25,25,0,50,0,32,14,29,8,40,13,14,29,29,0,4,0,12,24,38,9,28,16,36,0,20,6,24,22,22,11,12,8,46,0,44,0,36,9,10,16,16,20,20,0,2,14,34,7,8,24,32,0,28,0,6,10,34,20,28,0,10,6,20,10,42,11,22,0,47,27,27,4,35,18,38,14,26,28,36,5,39,22,30,6,28,31,31,0,18,0,14,0,22,14,15,8,9,10,11,26,34,5,44,9,32,6,7,20,40,0,26,12,24,6,18,0,30,23,24,15,31,20,34,12,44,0,34,22,36,18,32,12,13,5,6,12,38,14,40,4,48,18,27,9,19,4,32,16,22,18,24,6,36,6,26,9,37,0,42,17,17,11,20,5,17,4,42,4,5,0,38,9,30,0,1,3,4,4,24,5,15,1,2,});
sparse_graph3_ints[34][6] = vector<int>({2,2,0,0,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[34][8] = vector<int>({2,3,0,0,0,4,2,2,1,3,0,2,0,3,1,2,0,1,1,1,});
sparse_graph3_ints[34][10] = vector<int>({3,3,0,0,0,2,0,5,2,4,0,4,2,3,1,4,0,1,2,2,1,3,1,1,0,3,1,2,});
sparse_graph3_ints[34][12] = vector<int>({4,4,0,0,0,2,0,6,3,4,0,3,2,5,0,4,3,3,1,5,1,1,2,4,0,5,1,2,2,2,1,4,2,3,0,1,1,3,});
sparse_graph3_ints[34][14] = vector<int>({4,5,0,0,0,4,0,7,0,2,4,4,2,2,0,6,3,5,0,3,2,6,3,4,1,6,1,2,2,5,1,3,1,1,3,3,0,5,2,3,1,5,2,4,1,4,0,1,});
sparse_graph3_ints[34][16] = vector<int>({5,5,0,0,0,4,0,8,0,2,2,2,4,6,0,6,4,5,3,3,0,3,3,6,2,3,1,7,4,4,0,7,1,2,0,5,2,7,2,4,3,5,1,1,1,4,1,3,2,6,3,4,1,6,2,5,1,5,0,1,});
sparse_graph3_ints[34][18] = vector<int>({6,6,0,0,3,3,0,9,0,2,0,4,0,6,5,6,0,8,2,2,4,4,4,7,5,5,2,4,0,3,4,6,0,5,1,8,3,4,2,3,1,1,2,8,1,4,1,2,0,7,3,7,4,5,1,3,1,5,2,5,3,5,3,6,2,7,1,7,1,6,2,6,0,1,});
sparse_graph3_ints[34][20] = vector<int>({0,0,6,7,2,2,0,10,4,4,0,6,0,2,6,6,0,4,3,3,0,8,5,7,3,4,0,3,2,4,4,5,1,9,4,8,5,6,0,7,2,3,1,1,0,5,0,9,3,5,1,2,1,4,5,5,2,5,2,9,4,7,1,5,1,3,1,6,4,6,3,8,2,6,2,8,3,6,1,8,1,7,3,7,2,7,0,1,});
sparse_graph3_ints[34][23] = vector<int>({0,0,7,8,0,8,1,3,0,11,5,5,3,3,1,1,0,6,7,7,1,11,4,4,0,3,6,8,4,5,0,5,0,9,3,4,5,6,0,7,2,3,3,5,0,4,5,9,0,10,6,7,4,6,2,10,2,4,0,2,2,2,1,5,1,7,1,2,1,4,6,6,2,5,3,6,3,10,1,6,5,8,5,7,2,6,1,10,1,9,4,9,3,7,1,8,4,7,2,7,3,9,4,8,2,9,2,8,3,8,0,1,});
sparse_graph3_ints[34][26] = vector<int>({0,0,8,9,4,4,0,13,0,4,0,10,6,6,0,2,0,6,0,8,5,5,8,8,2,12,3,3,2,2,5,6,7,9,0,12,6,7,4,5,4,11,3,4,0,9,4,6,2,3,1,1,0,7,2,4,5,7,6,10,7,8,1,12,0,11,3,5,0,5,7,7,4,7,3,11,0,3,3,6,1,9,2,5,1,4,1,2,2,6,1,3,1,5,1,6,1,8,4,8,6,9,6,8,5,10,3,7,1,7,2,8,1,10,2,7,5,8,4,10,3,8,1,11,2,11,2,9,5,9,2,10,3,9,3,10,4,9,0,1,});
sparse_graph3_ints[34][30] = vector<int>({0,0,10,10,0,10,0,4,0,15,4,4,7,7,0,2,0,12,0,6,6,6,4,13,0,8,5,5,3,3,8,8,2,2,9,10,6,7,4,5,6,12,0,14,4,10,5,6,2,14,7,8,0,11,3,4,4,6,6,8,8,11,0,9,9,9,5,7,5,12,2,3,1,1,0,7,0,13,3,13,2,10,2,4,3,5,8,10,5,8,1,14,4,7,6,9,0,5,8,9,4,12,1,11,3,6,4,8,7,11,7,9,5,9,1,9,0,3,2,5,1,4,2,6,1,8,1,10,3,7,1,3,1,6,1,2,1,5,1,7,1,12,2,8,4,9,3,8,2,7,3,10,3,9,2,9,2,11,6,11,2,13,7,10,5,10,6,10,2,12,1,13,5,11,3,11,3,12,4,11,0,1,});
sparse_graph3_ints[34][35] = vector<int>({0,12,0,0,11,12,3,3,0,17,5,5,9,9,1,1,7,7,5,15,0,8,0,14,1,3,0,10,0,6,3,13,6,6,4,4,8,8,7,14,11,11,3,11,1,17,0,15,9,10,7,8,5,6,6,7,8,9,0,11,0,13,4,5,0,4,0,9,3,4,4,12,3,16,5,7,0,7,2,2,0,2,0,5,3,5,6,14,10,12,7,9,4,6,6,8,1,11,8,10,5,11,10,10,4,15,9,13,5,13,0,16,1,13,5,8,1,9,2,3,0,3,2,4,4,7,5,9,6,9,7,10,10,11,7,13,2,16,4,13,3,9,3,6,4,11,7,11,8,13,1,7,1,5,2,5,1,2,1,4,3,7,9,11,5,14,6,10,4,8,5,12,4,9,2,13,1,12,5,10,1,15,3,15,6,11,4,10,1,10,3,12,8,11,9,12,2,11,1,14,});
sparse_graph3_ints[34][40] = vector<int>({13,13,0,10,0,0,0,20,9,9,0,4,0,16,4,4,7,7,8,16,0,14,0,2,11,11,0,6,6,14,6,6,0,12,4,18,4,12,0,8,5,5,0,18,8,8,3,3,10,10,4,14,2,2,4,10,5,6,4,5,6,7,8,9,12,14,6,17,7,8,10,11,6,12,4,16,9,15,9,10,3,4,0,11,0,9,4,6,5,7,6,8,0,13,3,13,10,12,2,10,8,10,7,9,2,12,0,15,5,14,0,17,2,19,7,16,12,13,0,7,2,4,3,5,2,3,1,2,0,5,0,1,0,3,11,12,7,14,5,17,9,11,5,13,2,16,10,15,12,12,6,10,2,14,8,12,7,10,6,9,5,8,3,11,5,12,5,15,8,11,6,16,3,18,3,15,9,12,7,13,11,14,8,15,4,7,4,13,3,12,7,11,4,15,6,13,8,14,5,11,});
sparse_graph3_ints[34][45] = vector<int>({0,0,0,12,15,15,0,22,3,3,11,11,0,16,5,5,7,19,9,9,1,1,7,7,5,17,0,8,0,18,1,3,13,13,0,14,0,10,7,13,11,17,0,20,3,13,6,6,4,4,3,21,9,15,8,8,0,6,5,13,10,10,3,17,12,12,3,11,7,15,3,15,9,18,5,20,0,3,0,5,5,6,3,4,4,5,0,11,0,9,6,7,7,8,8,9,5,15,9,10,5,11,0,7,0,13,0,15,10,11,0,17,3,19,0,19,2,3,7,17,0,1,5,7,11,12,14,15,12,13,1,2,0,4,3,5,4,6,3,9,6,8,7,9,4,13,6,14,8,10,9,11,8,15,8,18,4,17,4,20,1,22,0,21,4,15,10,17,6,19,6,16,4,11,10,12,4,14,4,18,11,13,7,14,8,16,7,11,4,16,4,12,6,13,1,17,1,15,1,19,});
sparse_graph3_ints[34][50] = vector<int>({0,10,16,17,0,0,4,16,0,25,0,4,13,13,0,14,9,9,10,20,4,4,7,7,0,22,11,11,0,2,0,16,8,16,6,22,0,6,0,20,6,6,0,12,14,15,4,20,0,18,0,8,5,5,4,14,8,8,8,18,10,10,4,12,4,18,3,3,12,12,12,19,8,14,4,23,1,2,0,1,4,10,2,2,10,16,6,14,14,14,8,21,10,18,6,20,6,18,0,23,2,24,2,3,3,4,4,5,5,6,0,9,0,11,6,7,6,16,0,7,6,12,7,8,8,9,9,10,10,11,11,12,12,13,16,16,14,18,15,15,13,14,0,5,0,13,2,10,3,13,2,16,2,18,0,15,2,12,0,21,0,17,0,19,3,21,3,19,5,14,9,15,10,17,5,16,4,6,11,19,7,21,7,18,6,8,5,7,7,9,8,10,4,13,7,13,7,16,10,12,});
sparse_graph3_ints[34][55] = vector<int>({18,18,0,17,0,0,0,27,3,3,13,13,6,6,0,21,11,22,8,8,11,11,7,21,1,1,0,14,0,8,7,17,16,16,2,2,0,25,0,12,5,13,7,24,5,5,11,17,3,21,0,19,10,10,0,10,7,7,7,15,15,15,4,4,3,26,11,19,15,20,9,9,0,23,3,13,3,17,7,19,12,12,0,6,3,23,3,19,14,14,3,11,0,16,7,13,0,3,0,1,0,5,9,17,13,21,9,23,5,23,5,25,9,21,13,19,3,4,0,11,0,9,5,6,4,5,3,15,9,15,5,21,9,19,5,19,0,7,17,17,5,17,5,15,6,7,0,13,0,15,7,8,5,11,0,18,0,20,8,9,9,10,0,22,0,4,3,9,0,24,10,11,11,12,2,3,1,27,0,26,12,13,13,14,12,18,10,16,14,15,15,16,4,13,8,14,4,15,4,17,});
sparse_graph3_ints[34][60] = vector<int>({20,20,0,30,0,0,12,12,4,4,0,16,0,24,16,16,2,2,7,7,12,24,0,20,8,22,9,9,0,10,8,18,0,2,14,14,6,16,8,26,0,6,0,14,0,26,18,18,6,6,0,12,0,18,6,14,4,20,4,28,14,20,4,24,11,11,12,18,16,22,8,8,5,5,0,22,0,8,0,28,4,22,4,12,10,10,8,20,13,13,8,16,8,24,3,15,12,20,4,26,3,3,0,1,1,2,0,5,15,15,17,17,3,19,10,18,3,11,4,16,12,22,6,12,3,4,4,5,0,11,0,9,5,6,6,7,14,23,10,16,10,25,10,22,0,7,10,20,6,24,6,22,6,20,8,14,6,18,6,27,0,13,7,8,0,15,0,4,8,9,2,26,2,24,2,22,4,13,0,17,0,19,9,10,10,11,2,3,3,18,5,14,2,10,0,21,11,12,12,13,});
sparse_graph3_ints[34][70] = vector<int>({23,23,0,16,15,15,0,0,0,35,4,4,0,24,11,11,6,26,19,19,1,2,7,7,8,31,16,27,0,28,9,9,0,10,13,13,6,22,4,16,14,24,2,3,0,32,17,17,7,17,11,23,21,21,0,14,8,28,0,22,6,6,0,18,12,20,0,12,10,18,0,1,12,29,8,8,0,26,0,6,0,30,0,20,4,33,5,5,10,10,6,24,7,19,20,25,17,23,12,12,4,18,3,31,4,14,14,22,0,8,11,27,14,14,9,21,16,16,4,22,0,2,8,26,0,4,4,20,3,29,18,18,3,4,4,12,3,25,14,26,11,25,7,14,3,27,14,20,9,23,20,20,12,18,18,24,10,16,8,15,6,30,0,9,0,11,0,13,6,28,16,22,6,7,5,6,3,11,4,5,22,22,18,26,0,33,0,15,7,8,14,28,10,30,6,13,8,9,0,17,});
sparse_graph3_ints[34][80] = vector<int>({0,0,17,17,5,5,0,22,0,40,26,27,0,32,15,30,9,9,21,21,12,12,8,22,3,3,10,26,7,7,0,26,8,36,0,36,0,16,0,2,8,32,20,30,15,15,24,24,6,15,0,12,19,19,9,18,2,2,16,24,0,18,4,4,4,28,18,26,4,32,0,28,4,20,0,14,11,28,12,34,14,22,8,8,0,20,10,10,13,13,6,6,13,20,16,32,4,38,22,23,4,24,8,24,0,24,0,30,11,11,11,32,0,9,0,34,4,34,0,38,14,14,21,27,4,30,4,22,7,16,4,15,15,28,23,28,0,1,4,26,16,16,8,26,0,7,0,11,4,36,8,30,8,17,11,18,1,2,18,18,13,25,4,13,0,5,20,20,11,30,18,29,11,20,11,23,8,34,18,24,25,25,4,18,14,27,7,29,3,12,16,22,7,21,20,26,0,13,0,10,});
sparse_graph3_ints[34][100] = vector<int>({20,20,12,12,33,33,0,15,0,32,4,4,0,0,25,25,12,44,12,34,16,16,0,47,0,26,24,32,0,4,0,42,10,28,20,40,10,10,16,36,6,6,0,22,0,36,14,14,30,30,14,28,18,18,6,42,6,23,23,23,0,2,6,47,28,36,0,6,0,17,17,31,5,34,27,27,0,50,11,22,5,30,18,38,5,5,0,20,6,19,10,42,7,7,9,9,9,33,21,22,0,28,26,34,18,27,9,38,5,26,4,37,1,44,8,8,14,23,24,38,11,11,23,29,0,24,0,13,20,34,16,42,0,38,22,36,4,40,7,20,11,39,15,25,5,45,14,40,28,29,13,13,0,30,15,15,10,25,19,32,0,34,21,27,14,35,5,18,0,11,31,32,13,31,8,36,3,16,5,28,17,17,10,30,5,32,11,20,0,40,5,16,0,1,0,9,19,19,1,2,});
sparse_graph3_ints[32][6] = vector<int>({2,2,0,0,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[32][8] = vector<int>({2,3,0,0,0,4,0,2,2,2,1,3,0,3,0,1,1,2,1,1,});
sparse_graph3_ints[32][10] = vector<int>({3,3,0,0,0,2,0,5,2,4,0,4,1,1,2,3,1,4,2,2,0,1,0,3,1,3,1,2,});
sparse_graph3_ints[32][12] = vector<int>({4,4,0,0,0,2,0,6,2,2,3,4,0,4,2,5,1,1,3,3,0,3,1,5,2,4,1,2,0,5,2,3,0,1,1,3,1,4,});
sparse_graph3_ints[32][14] = vector<int>({4,5,0,0,0,4,0,7,0,2,2,2,4,4,0,6,3,5,2,3,1,1,2,6,0,3,3,4,0,5,1,6,1,2,3,3,1,3,2,5,2,4,1,4,0,1,1,5,});
sparse_graph3_ints[32][16] = vector<int>({5,5,0,0,0,4,0,8,0,2,3,3,4,6,0,6,2,2,4,5,1,1,2,3,2,4,0,3,4,4,0,5,1,7,3,6,1,2,0,7,1,4,3,4,2,7,1,3,3,5,2,6,1,5,0,1,2,5,1,6,});
sparse_graph3_ints[32][18] = vector<int>({0,0,6,6,2,2,0,9,0,6,0,2,4,4,0,4,3,3,5,6,0,8,3,4,1,1,2,3,5,5,2,8,4,7,2,4,0,5,0,3,0,7,4,6,1,8,4,5,1,2,1,4,2,5,1,3,3,5,1,5,3,7,1,6,3,6,2,6,1,7,2,7,0,1,});
sparse_graph3_ints[32][20] = vector<int>({6,7,0,0,2,2,0,10,4,4,0,6,0,2,0,8,0,4,3,3,6,6,4,5,4,8,3,4,1,1,2,3,5,7,5,5,1,9,0,7,2,4,0,5,0,3,3,5,5,6,2,9,1,2,1,4,2,5,0,9,4,6,1,3,2,6,1,5,1,6,3,8,3,6,4,7,1,7,2,8,0,1,1,8,3,7,2,7,});
sparse_graph3_ints[32][23] = vector<int>({7,8,0,0,3,3,0,11,5,5,1,1,0,8,1,3,0,6,4,4,3,10,5,6,7,7,0,9,1,11,0,3,0,5,3,4,4,5,0,7,2,3,6,6,6,8,3,5,0,10,5,9,4,6,0,4,0,2,2,2,2,4,1,2,0,1,1,7,1,5,2,10,6,7,3,6,1,9,5,7,1,4,2,5,3,7,4,9,4,7,1,6,2,6,1,8,3,9,5,8,2,7,1,10,2,8,4,8,3,8,2,9,});
sparse_graph3_ints[32][26] = vector<int>({8,9,0,0,4,4,0,13,0,4,0,10,6,6,0,2,0,6,0,8,4,11,5,5,3,3,2,2,6,7,8,8,0,11,2,12,4,5,5,6,7,7,0,9,0,12,6,10,4,8,3,4,4,6,3,11,7,9,0,7,2,4,0,5,1,2,2,3,0,1,0,3,3,5,5,7,2,8,2,10,5,10,7,8,6,8,4,7,3,6,4,10,1,12,1,1,1,9,2,6,1,3,1,4,2,5,5,8,1,10,3,7,1,8,3,8,1,7,1,6,1,5,3,9,2,7,6,9,2,9,4,9,2,11,5,9,3,10,1,11,});
sparse_graph3_ints[32][30] = vector<int>({6,6,0,0,10,10,0,4,0,15,4,4,0,10,8,8,0,2,0,12,0,6,4,13,0,8,4,10,5,5,3,3,7,7,6,12,2,2,2,10,0,13,2,14,4,5,5,6,6,7,9,10,7,8,0,1,1,2,0,9,3,4,2,3,0,7,4,6,0,11,3,11,0,14,0,5,4,8,5,12,8,9,6,8,2,8,3,5,2,4,0,3,5,7,3,13,3,10,8,11,7,9,4,12,9,9,2,12,6,9,7,11,5,8,5,10,4,7,3,9,3,6,6,10,8,10,5,9,1,14,1,12,1,10,4,11,4,9,1,11,1,9,2,6,3,7,1,1,3,8,6,11,2,11,5,11,1,8,2,5,1,4,1,3,2,9,3,12,1,7,1,6,2,7,1,5,7,10,2,13,1,13,});
sparse_graph3_ints[32][35] = vector<int>({0,13,0,0,11,12,0,6,0,17,5,5,9,9,1,1,7,7,0,10,7,14,2,2,0,15,0,8,5,13,4,4,6,6,3,11,3,16,3,3,8,8,0,11,10,10,5,11,0,3,0,5,3,9,3,13,0,1,5,15,0,9,0,7,4,5,3,4,5,6,0,12,0,4,0,14,6,7,1,17,0,16,7,8,7,11,9,13,11,11,9,10,8,9,2,3,4,11,6,14,3,14,1,2,3,10,3,5,1,9,5,7,4,6,7,9,5,9,4,12,6,12,1,13,1,11,4,15,6,8,8,13,4,13,9,11,8,10,10,12,7,13,0,2,1,3,3,12,1,15,5,12,2,16,4,10,4,7,6,9,7,10,5,8,3,7,2,11,2,13,6,10,6,13,10,11,8,11,6,11,3,15,2,14,5,10,2,12,4,9,2,9,5,14,2,4,1,7,4,8,});
sparse_graph3_ints[32][40] = vector<int>({0,12,0,0,13,13,0,20,0,4,9,9,0,8,0,16,7,7,8,16,0,2,5,5,6,14,0,6,11,11,4,12,0,10,4,18,0,14,4,4,0,18,6,6,8,8,4,14,3,3,10,10,4,10,0,1,1,2,4,16,8,14,10,15,2,2,6,12,12,12,2,10,6,17,0,5,0,9,0,7,4,5,3,4,5,6,2,16,2,14,2,12,6,7,0,11,7,8,8,9,8,12,2,3,0,13,2,19,2,8,0,15,5,12,9,10,10,11,12,14,11,12,0,17,3,12,3,10,6,10,5,14,7,14,4,8,6,8,3,14,8,10,9,15,5,15,7,16,5,17,3,17,7,12,10,12,4,6,3,15,5,11,8,13,5,13,0,3,7,9,3,11,3,13,6,15,3,16,6,13,9,11,7,10,6,9,5,7,4,11,4,13,7,11,7,13,5,8,2,9,});
sparse_graph3_ints[32][45] = vector<int>({0,0,0,14,15,15,0,5,11,11,5,5,7,19,0,18,0,22,7,7,9,9,1,1,0,8,7,15,0,12,13,13,5,13,11,17,0,10,0,20,0,16,6,6,4,4,0,3,3,21,3,3,8,8,3,17,9,15,10,10,3,11,3,13,3,19,7,13,0,7,12,12,0,1,2,2,7,17,3,15,3,9,0,9,5,17,9,18,0,6,4,5,5,6,0,11,5,15,5,20,3,4,0,13,4,11,0,15,5,12,0,17,6,7,0,19,11,15,7,8,9,13,0,4,8,9,9,10,2,3,3,10,5,11,0,21,13,14,6,13,10,11,11,12,13,16,9,16,12,13,5,18,6,15,8,15,2,17,2,15,2,19,1,22,2,8,7,11,4,15,4,17,8,13,2,13,10,14,7,16,5,19,8,18,10,17,14,14,1,2,5,9,7,9,4,13,9,11,6,14,});
sparse_graph3_ints[32][50] = vector<int>({0,14,16,17,3,3,0,25,0,0,11,11,0,18,5,5,8,21,9,9,0,2,14,14,4,18,7,7,2,2,0,22,0,8,0,12,10,16,6,14,12,19,0,16,0,10,4,20,6,6,0,20,4,23,12,12,8,8,4,14,4,4,8,16,10,10,3,12,10,18,0,1,6,20,15,15,0,5,8,14,13,13,0,7,1,2,5,12,2,18,0,23,4,10,0,9,0,11,8,18,0,6,4,5,3,16,10,20,14,18,5,6,6,7,12,17,6,16,0,13,6,18,6,22,2,20,2,24,3,4,2,22,3,9,0,15,0,17,6,13,2,14,7,8,4,16,10,14,8,9,0,4,9,10,10,11,5,11,2,3,0,19,0,21,11,16,6,12,3,10,7,14,11,12,8,19,0,24,4,21,12,13,8,15,8,17,2,16,4,17,5,18,3,14,8,12,2,8,2,12,});
sparse_graph3_ints[32][55] = vector<int>({11,11,18,18,0,7,0,27,0,0,7,7,0,21,14,14,0,16,0,3,7,24,5,5,9,9,4,21,0,12,0,5,16,16,4,15,9,17,11,22,5,17,8,20,0,14,0,25,0,10,0,18,6,6,0,1,8,8,4,4,13,13,0,23,15,20,5,23,13,18,9,15,10,10,3,13,3,26,3,3,0,9,1,1,1,3,6,18,12,12,9,21,12,17,3,17,3,11,8,14,15,15,6,12,3,20,0,20,3,23,7,13,17,17,0,11,13,21,7,19,0,8,5,6,6,7,4,5,0,13,11,16,11,19,9,23,0,17,0,15,7,8,4,13,5,25,0,6,3,4,6,15,6,21,10,18,8,9,12,20,0,19,3,9,3,15,4,11,9,10,0,22,0,24,3,19,0,26,7,22,14,19,7,16,10,11,10,15,11,12,3,24,3,21,1,27,3,10,2,3,});
sparse_graph3_ints[32][60] = vector<int>({0,0,0,14,0,30,0,22,3,4,20,20,10,10,16,16,12,24,1,2,0,10,0,26,12,12,8,8,14,14,5,20,6,6,10,22,4,14,2,3,5,16,9,19,18,18,4,26,8,26,0,16,0,20,0,12,16,22,0,1,7,7,0,8,6,22,0,24,0,18,5,5,9,16,9,9,14,19,4,28,11,11,0,28,7,16,10,20,13,13,0,6,4,12,0,2,15,15,14,21,0,4,7,24,3,20,3,24,4,4,6,14,17,17,12,17,8,18,4,22,3,11,0,9,13,18,0,11,4,18,11,16,6,7,3,16,5,24,12,22,5,6,0,13,4,5,8,14,6,18,0,15,2,26,8,22,7,8,7,12,9,24,14,23,8,9,0,17,2,22,12,20,19,19,7,20,16,20,10,25,6,27,4,13,0,7,3,3,0,19,0,21,10,11,2,29,2,18,9,15,});
sparse_graph3_ints[32][70] = vector<int>({0,16,0,35,0,8,0,0,23,23,0,24,16,16,14,24,10,10,6,26,2,2,0,12,10,17,8,31,0,20,0,28,12,12,8,8,14,28,18,18,0,2,20,22,10,20,3,3,6,16,0,10,3,31,4,23,7,7,9,26,0,18,0,14,0,30,6,18,14,21,0,22,6,6,9,9,17,26,10,22,15,15,17,22,4,33,6,24,9,28,0,26,13,13,4,5,0,32,5,15,11,11,20,25,6,20,0,1,14,14,4,14,20,20,14,26,1,2,6,28,0,5,17,17,11,29,8,20,0,7,5,5,11,23,13,20,3,29,3,12,3,27,17,24,6,30,9,16,19,19,3,20,12,24,3,22,3,25,12,17,16,21,22,22,0,11,6,7,3,14,3,18,13,18,12,26,0,13,8,15,5,6,4,4,7,22,8,13,7,12,8,9,1,33,8,25,0,9,2,17,});
sparse_graph3_ints[32][80] = vector<int>({13,25,8,8,26,27,0,40,0,9,0,0,0,32,13,13,16,32,0,24,0,5,17,17,22,24,0,18,8,36,10,10,0,28,0,36,12,30,6,6,7,21,7,28,15,15,7,16,0,14,1,2,19,19,21,21,5,33,0,7,0,20,20,30,4,27,16,28,10,24,11,20,0,16,0,22,12,34,0,11,9,9,0,26,0,1,7,34,25,25,17,23,7,7,4,38,5,21,12,12,12,28,0,34,21,27,6,15,18,25,7,30,0,30,14,14,5,5,11,11,0,38,7,23,15,22,8,17,13,20,4,15,23,28,14,31,0,2,4,4,3,31,16,16,18,29,2,2,8,24,3,36,18,18,4,13,0,12,11,18,2,4,4,20,9,31,5,28,15,26,11,25,9,18,10,33,3,26,14,21,22,22,19,26,3,33,3,12,20,20,3,24,10,28,24,24,4,18,6,7,});
sparse_graph3_ints[32][100] = vector<int>({0,47,15,36,18,18,33,33,0,15,22,22,8,34,20,40,0,0,0,34,0,8,0,22,14,14,0,29,0,4,10,10,8,46,25,25,10,28,6,6,17,30,22,34,5,40,30,30,0,38,12,12,16,16,15,40,20,20,28,36,0,26,0,19,0,2,5,28,0,41,22,30,28,28,0,50,0,6,8,43,14,30,10,25,6,19,5,5,9,20,0,17,12,36,23,24,9,9,7,7,24,38,5,33,10,22,12,44,9,40,18,24,16,22,26,32,18,32,11,30,0,24,4,43,8,8,26,27,0,32,6,30,16,42,11,11,19,38,19,34,0,43,14,20,0,13,23,36,4,48,31,32,13,33,0,36,14,26,5,26,4,17,18,27,13,13,27,34,4,36,4,24,21,28,12,40,5,38,9,36,15,15,24,31,6,17,8,19,0,11,17,37,4,46,0,1,0,45,1,2,});
sparse_graph3_ints[30][6] = vector<int>({2,2,0,0,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[30][8] = vector<int>({2,3,0,0,0,2,0,4,2,2,1,3,0,1,0,3,1,2,1,1,});
sparse_graph3_ints[30][10] = vector<int>({3,3,0,0,0,2,0,4,2,4,0,5,1,1,2,3,2,2,1,4,0,3,0,1,1,2,1,3,});
sparse_graph3_ints[30][12] = vector<int>({4,4,0,0,0,2,0,6,2,2,0,4,3,4,1,2,0,1,0,3,2,5,3,3,1,1,0,5,2,4,1,5,2,3,1,3,1,4,});
sparse_graph3_ints[30][14] = vector<int>({4,5,0,0,0,4,0,7,0,2,2,2,4,4,0,6,3,3,3,5,2,3,1,2,0,1,0,3,2,6,0,5,1,1,1,3,3,4,1,6,2,4,1,4,2,5,1,5,});
sparse_graph3_ints[30][16] = vector<int>({5,5,0,0,0,4,0,8,0,2,3,3,0,6,2,2,4,6,4,4,2,3,1,2,0,1,0,3,0,5,1,1,2,4,4,5,3,4,1,7,1,3,3,6,0,7,1,4,2,7,3,5,1,5,2,6,2,5,1,6,});
sparse_graph3_ints[30][18] = vector<int>({6,6,0,0,0,6,0,2,0,9,0,4,4,4,3,3,2,2,0,8,2,8,5,6,3,4,0,1,1,2,2,3,4,5,0,7,0,5,0,3,2,4,4,7,5,5,1,1,3,5,1,3,1,4,2,5,1,8,4,6,3,7,2,6,1,5,1,6,3,6,1,7,2,7,});
sparse_graph3_ints[30][20] = vector<int>({0,0,6,7,0,4,0,10,4,4,0,2,0,6,0,8,3,3,5,5,2,2,2,9,6,6,4,5,4,8,0,1,1,2,3,4,2,3,0,7,0,5,0,3,2,4,3,5,4,6,5,7,1,1,2,6,5,6,3,8,0,9,1,9,3,6,1,3,2,5,1,4,1,7,1,5,1,6,2,8,4,7,2,7,1,8,3,7,});
sparse_graph3_ints[30][23] = vector<int>({7,8,0,0,0,6,0,11,5,5,1,1,0,4,0,9,3,3,3,10,4,4,6,6,0,8,2,2,0,3,0,1,0,7,0,5,3,4,1,11,0,10,4,5,5,9,5,6,7,7,3,7,2,3,3,5,1,2,0,2,1,3,1,9,1,7,5,7,2,4,3,9,4,6,6,8,4,9,6,7,2,10,4,7,3,6,1,5,2,8,2,5,1,4,2,6,2,7,3,8,1,8,1,6,5,8,4,8,2,9,1,10,});
sparse_graph3_ints[30][26] = vector<int>({0,0,8,9,4,4,0,13,0,4,6,6,0,10,0,2,0,6,0,8,4,11,5,5,3,3,7,7,2,10,2,2,0,1,1,2,2,8,0,12,2,12,6,10,4,8,4,5,0,7,0,5,3,4,0,9,0,11,5,6,6,7,8,8,2,3,4,10,4,6,0,3,2,6,2,4,3,9,6,8,3,11,5,10,7,9,5,7,7,8,2,9,3,5,5,8,4,7,3,6,1,1,3,10,3,7,3,8,1,8,4,9,1,10,1,9,1,12,2,5,2,7,5,9,1,7,1,4,1,6,1,3,1,5,1,11,6,9,2,11,});
sparse_graph3_ints[30][30] = vector<int>({10,10,0,0,0,6,0,15,6,6,1,2,0,10,8,8,4,4,0,12,6,12,0,8,4,10,0,1,0,4,5,5,3,3,7,7,4,13,0,2,0,13,2,10,2,2,2,8,2,14,2,12,0,5,0,7,4,5,3,4,0,9,0,11,4,8,5,10,8,9,5,6,0,14,2,3,6,7,7,8,3,10,4,12,7,11,6,10,9,9,6,8,3,11,2,9,4,6,9,10,2,6,0,3,5,12,8,11,2,4,4,11,3,13,3,9,5,7,7,9,5,9,2,11,5,8,4,7,3,6,3,12,1,1,5,11,6,9,4,9,3,8,3,5,3,7,1,8,1,10,1,11,6,11,1,12,1,9,8,10,7,10,2,5,2,7,1,13,1,14,1,7,2,13,1,4,1,6,1,3,1,5,});
sparse_graph3_ints[30][35] = vector<int>({11,12,3,3,0,0,7,7,0,17,0,9,0,13,9,9,1,1,7,14,5,5,2,2,5,11,0,11,0,15,0,7,3,16,6,6,4,4,5,13,10,10,3,11,8,8,0,5,0,1,0,3,3,9,3,13,0,8,0,10,5,15,9,13,0,6,7,12,0,12,0,14,3,4,4,5,0,16,5,6,1,17,0,4,5,9,6,7,3,7,2,3,7,11,7,8,8,9,4,11,4,13,2,11,3,15,2,9,2,13,1,15,9,10,11,11,6,11,7,13,4,9,3,14,9,11,1,13,5,10,7,9,4,12,6,12,6,14,8,13,10,11,1,11,1,2,5,7,4,15,3,5,1,9,2,7,6,13,4,10,4,14,2,14,2,12,5,12,6,10,8,10,4,7,3,10,6,8,3,12,8,11,6,9,7,10,2,10,5,8,0,2,4,8,4,6,1,7,2,15,});
sparse_graph3_ints[30][40] = vector<int>({0,0,0,12,13,13,0,20,2,2,0,8,10,10,0,16,8,8,8,16,6,6,0,2,3,3,6,14,0,10,0,14,4,18,5,5,4,12,0,18,4,10,4,4,9,13,4,14,9,9,7,7,12,12,0,1,0,5,0,7,1,2,4,16,10,15,6,12,11,11,2,10,0,9,0,6,2,16,6,17,4,5,2,14,0,11,0,13,5,6,2,19,7,15,3,4,8,12,6,7,4,11,0,15,6,11,2,8,0,4,7,8,0,17,2,3,2,12,8,9,9,10,4,9,0,19,9,14,7,14,10,11,3,14,7,12,3,16,5,16,6,10,2,9,5,14,2,18,11,12,12,14,2,13,2,11,4,8,10,14,8,13,5,12,2,17,2,15,5,15,6,16,8,14,5,13,7,11,5,10,7,13,3,15,3,17,10,12,9,15,11,14,7,16,3,13,6,8,0,3,});
sparse_graph3_ints[30][45] = vector<int>({5,5,0,0,15,15,9,9,3,21,0,14,0,18,0,3,7,15,0,7,7,7,13,13,0,5,7,19,11,11,0,12,0,20,5,13,11,17,0,10,0,16,6,6,0,1,4,4,8,8,0,22,3,17,9,16,4,11,7,13,3,19,3,3,0,9,1,1,12,12,10,10,2,2,3,10,3,13,7,17,3,15,0,8,9,14,0,11,5,6,5,17,4,5,14,14,9,18,5,15,5,20,11,15,0,13,0,6,3,4,0,15,0,17,3,9,5,11,0,19,6,7,7,8,7,12,0,21,8,9,13,16,2,12,5,18,4,13,9,15,7,14,3,16,10,11,3,18,9,10,9,13,4,10,2,3,0,4,2,8,11,12,12,13,7,11,1,22,7,16,5,16,5,14,5,19,13,14,9,17,2,15,6,11,2,9,2,20,10,14,2,13,11,16,7,18,1,19,1,17,});
sparse_graph3_ints[30][50] = vector<int>({12,12,0,14,4,4,0,0,4,23,16,17,0,20,0,4,9,10,0,10,6,18,7,8,14,14,0,2,6,6,0,23,12,19,4,16,3,3,4,13,0,12,10,17,0,18,8,21,0,16,0,25,11,11,5,5,6,16,8,9,2,20,0,7,0,1,10,15,15,15,1,2,0,9,6,7,3,12,2,3,3,22,8,12,13,13,7,20,3,15,4,10,8,14,11,18,0,11,0,8,6,12,4,20,3,18,8,18,14,18,0,22,5,6,4,5,0,13,8,16,5,11,10,20,0,6,10,11,6,22,5,14,12,16,0,15,2,24,8,8,0,17,7,7,0,19,3,9,3,4,6,15,9,9,5,21,9,19,13,17,4,18,8,15,0,21,6,19,2,14,10,16,11,12,10,10,2,17,0,24,3,10,10,14,2,21,13,14,12,13,6,17,6,14,15,16,8,17,14,15,});
sparse_graph3_ints[30][55] = vector<int>({4,4,0,15,11,16,0,0,1,27,18,18,0,20,10,11,1,3,9,23,6,7,8,9,3,23,0,10,1,1,7,17,16,16,3,3,13,21,0,17,12,12,11,19,0,12,0,22,7,21,5,25,5,6,7,8,5,17,9,10,0,24,15,15,3,21,3,11,3,17,0,7,13,13,0,1,3,13,0,26,0,3,0,9,6,12,8,14,0,5,8,19,5,5,7,13,14,14,0,18,15,20,4,15,10,15,0,13,4,24,6,19,13,18,17,17,8,22,4,19,12,17,6,6,0,11,11,11,0,8,5,21,7,7,11,22,4,5,8,8,9,17,14,19,6,15,0,14,4,13,12,20,0,16,10,18,7,24,9,9,3,26,3,15,9,20,0,19,0,21,10,21,10,10,4,11,6,23,7,16,11,12,3,18,0,23,3,4,0,25,0,6,9,15,3,9,2,20,2,25,});
sparse_graph3_ints[30][60] = vector<int>({0,30,0,12,0,0,12,12,20,20,0,20,0,4,16,16,12,24,0,8,9,10,0,26,0,16,7,8,14,14,0,2,5,18,5,24,0,6,10,22,0,23,18,18,0,10,8,26,7,14,9,20,0,18,4,22,6,7,16,22,4,28,5,6,8,9,0,28,15,20,4,5,6,19,13,18,11,11,4,15,3,11,11,16,0,1,3,4,1,2,4,12,2,3,2,20,13,13,7,24,15,15,12,22,7,20,10,15,5,13,0,15,3,27,1,24,17,17,9,14,3,18,10,10,0,13,14,19,12,17,3,21,6,6,2,26,6,22,5,5,0,11,8,22,3,24,6,16,8,18,8,16,9,24,3,14,19,19,8,8,4,16,14,23,4,4,7,7,0,9,10,18,12,20,9,9,0,14,6,27,11,19,10,25,0,21,0,19,5,26,2,29,0,17,10,11,7,13,18,21,});
sparse_graph3_ints[30][70] = vector<int>({11,11,16,16,0,35,1,2,0,17,6,7,17,22,4,4,23,23,0,30,0,22,8,27,0,26,0,0,16,27,14,14,7,24,6,32,0,10,8,9,7,17,1,4,6,21,18,18,0,14,13,23,0,20,10,28,19,23,0,12,13,20,10,10,0,24,4,28,7,8,0,1,0,28,13,13,4,31,5,6,9,21,13,28,6,16,20,25,0,33,20,20,0,7,4,14,10,24,12,12,3,13,5,15,10,30,8,18,3,25,0,2,15,15,2,2,16,25,14,24,2,4,9,19,0,9,3,20,3,27,15,20,12,19,5,20,22,22,1,18,17,17,9,9,10,22,11,26,1,31,19,19,7,26,3,33,6,23,5,29,3,23,0,15,6,6,12,17,3,18,16,21,6,8,5,5,7,30,11,16,13,26,10,15,0,13,9,14,8,8,8,13,7,12,0,11,18,26,21,21,});
sparse_graph3_ints[30][80] = vector<int>({20,20,0,32,2,3,0,40,16,32,0,0,26,27,11,11,0,16,16,16,0,23,12,24,4,5,14,28,6,7,8,36,0,28,23,24,0,36,13,13,9,9,8,30,11,18,18,18,1,2,6,20,4,27,22,29,0,18,3,4,22,22,0,21,11,32,0,14,8,26,5,6,17,29,0,26,18,24,4,32,8,8,8,22,12,34,16,23,4,38,13,20,0,1,0,12,25,25,10,10,8,32,5,19,21,27,12,26,4,34,0,34,0,30,12,12,0,38,20,25,14,14,8,28,0,10,8,24,5,14,4,18,19,30,4,36,4,25,0,2,7,21,2,2,15,15,11,29,0,8,17,27,14,31,0,6,2,18,2,23,8,34,14,21,4,13,3,31,7,7,17,17,9,16,7,14,12,19,10,17,19,19,14,26,8,15,5,28,3,24,3,12,15,22,21,21,11,23,6,6,});
sparse_graph3_ints[30][100] = vector<int>({19,19,10,10,4,5,16,42,0,3,6,27,33,33,28,28,15,16,21,30,13,13,22,22,14,32,8,46,2,45,23,36,0,42,0,38,0,50,17,37,8,8,9,31,6,23,0,13,0,0,6,17,9,38,0,26,28,36,10,27,0,30,0,20,25,25,0,5,8,43,0,17,31,31,20,21,20,28,0,11,13,30,16,35,0,7,22,34,4,41,0,24,14,22,26,27,8,34,5,30,20,40,12,41,0,32,7,20,15,24,29,30,14,19,0,36,23,24,0,15,12,44,17,28,11,39,24,38,0,28,5,38,5,33,4,48,0,22,17,32,27,34,20,33,14,27,19,38,9,9,14,14,10,35,0,34,4,36,26,32,11,11,0,1,7,7,6,25,6,42,13,35,24,31,17,18,18,25,6,19,0,44,0,40,12,12,11,23,0,48,6,6,11,19,4,46,4,15,5,16,});
sparse_graph3_ints[27][6] = vector<int>({2,2,0,0,0,2,0,3,1,2,0,1,1,1,});
sparse_graph3_ints[27][8] = vector<int>({2,3,0,0,0,2,0,4,2,2,0,1,1,3,1,1,0,3,1,2,});
sparse_graph3_ints[27][10] = vector<int>({3,3,0,0,0,2,0,5,2,2,0,4,2,4,0,1,0,3,1,1,2,3,1,2,1,4,1,3,});
sparse_graph3_ints[27][12] = vector<int>({4,4,0,0,0,2,0,6,2,2,0,4,0,1,0,3,3,4,2,3,1,1,1,2,3,3,2,5,0,5,1,3,2,4,1,5,1,4,});
sparse_graph3_ints[27][14] = vector<int>({4,5,0,0,0,4,0,2,0,7,3,3,2,2,0,6,0,1,4,4,1,2,0,5,2,6,2,3,0,3,2,4,1,1,3,5,3,4,1,3,1,4,1,6,1,5,2,5,});
sparse_graph3_ints[27][16] = vector<int>({5,5,0,0,0,2,0,8,3,3,0,4,0,6,4,4,2,2,0,1,1,2,2,7,0,5,4,6,3,4,2,3,0,3,2,4,0,7,1,1,4,5,2,6,3,6,2,5,1,4,1,3,1,7,1,5,3,5,1,6,});
sparse_graph3_ints[27][18] = vector<int>({0,0,6,6,0,4,0,9,4,4,0,2,0,6,3,3,2,2,2,8,0,7,0,1,1,2,4,5,0,8,0,5,3,4,2,3,4,7,2,6,5,5,0,3,2,4,1,1,5,6,3,7,3,5,4,6,2,5,3,6,1,8,1,6,1,4,1,7,1,5,1,3,2,7,});
sparse_graph3_ints[27][20] = vector<int>({0,0,6,7,0,4,0,10,4,4,0,2,0,6,0,8,5,5,3,3,4,8,2,2,0,1,1,2,2,9,0,7,0,5,3,4,4,5,2,3,2,6,2,8,6,6,0,9,4,6,2,7,0,3,2,4,5,6,1,1,3,8,3,5,3,6,2,5,5,7,3,7,1,7,1,8,1,9,1,6,1,4,1,5,4,7,1,3,});
sparse_graph3_ints[27][23] = vector<int>({0,0,7,8,0,8,2,2,5,5,3,10,1,1,0,6,0,10,4,4,6,6,3,3,1,11,0,1,0,3,0,5,3,7,5,9,0,7,0,9,0,11,0,4,3,4,3,9,4,5,2,3,1,9,5,6,1,7,7,7,5,7,2,9,2,7,6,7,3,5,4,7,1,2,4,9,2,8,4,8,1,5,0,2,4,6,2,5,3,6,3,8,2,10,2,6,1,3,6,8,2,4,1,8,5,8,1,6,1,4,1,10,});
sparse_graph3_ints[27][26] = vector<int>({8,9,0,6,0,0,0,13,5,5,0,2,0,10,2,2,7,7,0,8,4,11,4,4,2,10,6,6,3,3,0,1,1,2,0,5,2,8,6,10,0,12,2,12,0,7,4,8,0,4,0,9,4,5,3,4,0,11,5,9,2,3,5,6,3,10,6,7,7,8,2,6,2,9,8,8,4,10,5,8,3,8,4,6,1,8,0,3,3,9,6,8,3,6,3,7,5,10,1,1,3,11,4,9,1,11,5,7,2,4,4,7,1,10,1,9,7,9,2,11,2,5,2,7,3,5,1,7,6,9,1,6,1,5,1,4,1,3,1,12,});
sparse_graph3_ints[27][30] = vector<int>({0,0,10,10,6,6,2,2,0,15,4,4,0,10,0,2,8,8,0,6,6,12,4,10,0,13,0,8,5,5,2,10,0,1,7,7,3,3,4,13,1,2,0,5,0,12,3,12,2,8,6,10,9,9,0,7,2,14,0,9,4,5,3,4,4,8,0,11,0,14,5,6,8,11,0,4,6,7,2,3,2,12,7,8,3,10,2,9,5,11,8,9,5,10,2,6,2,7,5,9,7,10,2,11,4,12,5,8,7,11,3,8,3,11,2,13,3,7,3,9,5,12,6,9,6,8,4,11,0,3,8,10,9,10,6,11,7,9,4,9,1,1,1,12,1,13,3,13,1,11,4,6,5,7,4,7,1,10,1,9,2,4,2,5,3,6,1,8,3,5,1,7,1,6,1,5,1,4,1,3,1,14,});
sparse_graph3_ints[27][35] = vector<int>({0,13,0,0,11,12,0,7,1,17,0,3,9,9,5,5,7,7,0,5,5,15,0,11,4,13,0,9,0,1,6,6,4,11,4,4,0,16,3,3,8,8,9,13,1,1,2,2,4,9,0,14,6,13,10,11,0,8,7,11,0,10,2,14,7,14,3,16,2,10,5,6,4,5,0,6,3,11,3,4,8,12,6,10,0,12,9,10,0,15,3,15,7,8,2,9,4,12,0,17,8,9,6,7,2,13,3,7,10,10,5,13,5,9,6,11,2,3,0,4,2,15,4,8,2,11,11,11,7,13,8,11,2,12,5,14,2,7,6,12,9,12,2,6,5,10,5,11,4,14,1,9,3,13,9,11,7,12,7,10,8,13,5,12,7,9,10,12,3,8,6,14,4,10,1,15,8,10,3,9,6,9,1,13,3,12,3,14,1,11,3,10,1,2,5,8,5,7,6,8,});
sparse_graph3_ints[27][40] = vector<int>({0,20,2,10,0,0,13,13,0,4,9,9,0,8,0,16,8,16,0,2,7,7,11,11,5,5,3,3,0,14,4,18,4,12,7,14,0,10,0,18,4,14,6,6,8,8,0,1,4,4,4,16,10,10,1,2,4,10,0,7,2,3,10,15,12,12,0,13,6,12,4,5,0,11,0,9,5,6,2,16,9,13,6,7,6,17,8,12,2,19,2,14,0,6,4,11,3,4,8,9,0,12,7,8,6,13,0,15,2,17,6,16,0,17,4,13,9,10,2,12,8,15,6,14,2,8,10,11,0,19,6,10,10,14,4,9,4,15,11,12,6,11,12,14,4,17,2,15,2,9,2,18,2,13,8,14,2,11,0,5,2,2,4,8,10,13,12,13,9,12,8,11,5,16,7,11,7,15,11,14,9,14,7,13,5,15,10,12,11,13,9,15,2,7,8,13,5,10,2,6,});
sparse_graph3_ints[27][45] = vector<int>({0,20,0,0,4,4,15,15,0,15,9,14,1,3,6,7,7,19,8,9,0,10,4,15,1,1,10,10,13,13,3,21,3,3,0,13,11,17,0,17,4,17,5,5,7,8,0,22,7,17,0,7,11,11,3,9,6,12,0,1,5,11,0,3,0,5,8,13,3,19,4,10,12,12,0,8,5,6,11,15,0,18,0,11,4,5,4,13,9,9,7,15,14,14,0,9,9,18,2,18,6,14,9,16,5,20,0,14,5,18,0,12,7,7,2,14,2,16,6,6,8,8,4,11,9,10,13,16,0,16,0,21,9,15,3,4,0,19,0,6,3,10,11,14,7,16,6,13,4,16,9,17,5,19,10,11,11,12,4,14,3,20,5,17,9,13,8,12,11,16,7,18,6,15,13,14,2,17,2,12,1,22,2,15,2,19,2,8,6,11,2,13,12,13,2,9,13,15,10,14,});
sparse_graph3_ints[27][50] = vector<int>({9,10,0,25,3,4,13,13,0,14,0,0,0,20,16,17,0,10,1,2,10,20,7,8,4,20,11,11,4,13,8,16,6,22,1,4,4,16,0,12,15,15,0,18,4,5,6,7,0,1,8,18,0,16,5,6,0,23,5,17,0,8,11,16,0,2,8,9,2,2,7,13,0,6,12,12,14,18,2,20,3,12,3,22,8,14,14,14,11,18,4,10,3,15,7,20,6,6,2,24,5,5,6,12,10,15,10,10,8,8,0,11,0,13,3,18,4,4,0,9,12,19,0,22,6,19,4,23,5,11,8,21,9,19,5,21,6,15,9,9,10,11,13,17,7,7,0,17,8,15,0,15,4,19,6,13,0,19,11,12,6,18,3,16,13,16,3,9,11,17,2,14,0,21,10,16,2,21,15,16,10,14,8,17,12,13,0,24,3,10,0,7,13,14,3,3,2,18,9,18,});
sparse_graph3_ints[27][55] = vector<int>({12,12,8,8,0,26,18,18,0,9,0,0,0,5,11,22,4,15,7,19,0,12,0,23,0,15,0,3,5,17,16,16,5,23,10,11,11,19,0,7,6,7,0,17,0,21,5,6,7,24,9,9,15,20,11,17,0,1,0,19,13,13,3,26,3,11,2,23,4,13,1,1,4,4,6,18,8,21,15,15,1,3,3,3,5,5,8,14,10,16,7,13,14,14,0,10,7,8,14,19,3,21,9,15,3,17,1,27,0,13,6,21,6,6,9,10,17,17,2,20,4,12,11,11,0,11,13,18,10,21,7,7,6,15,2,25,8,9,9,19,3,15,0,16,13,21,5,25,4,5,0,24,4,22,9,23,11,12,0,8,7,16,10,10,12,20,6,12,8,17,5,20,4,19,0,14,12,13,8,22,9,18,0,18,12,15,3,18,0,20,11,18,4,11,14,15,13,17,0,22,});
sparse_graph3_ints[27][60] = vector<int>({20,20,4,4,10,12,1,2,8,26,0,17,6,7,10,18,0,30,0,26,17,17,0,0,9,9,0,10,7,17,10,22,13,13,4,16,1,4,15,15,5,24,2,20,16,22,4,22,12,24,4,12,0,12,4,28,0,24,0,1,2,12,14,19,5,6,0,28,0,20,7,8,10,10,0,7,4,14,0,2,16,20,12,12,12,18,0,15,2,2,6,16,2,4,7,20,7,24,0,9,14,14,13,21,5,20,3,27,10,15,8,22,16,16,8,9,1,18,6,6,7,14,8,18,11,16,3,24,5,5,19,19,1,22,11,20,7,7,6,22,0,13,2,26,9,24,9,20,8,15,9,10,4,19,3,18,0,11,8,8,5,14,18,18,0,16,14,23,2,29,11,12,4,13,12,17,5,26,10,11,0,18,2,23,3,21,6,27,10,25,11,23,0,21,14,17,18,21,4,5,});
sparse_graph3_ints[27][70] = vector<int>({12,22,0,25,10,10,23,23,4,5,6,32,1,2,20,20,6,7,5,26,0,16,0,20,14,28,0,0,12,12,16,16,0,32,8,18,14,14,10,20,0,35,18,18,6,16,8,8,1,4,0,29,4,24,0,14,0,22,10,30,6,12,3,4,21,22,0,12,0,18,4,31,20,25,12,27,5,6,10,25,18,23,0,1,16,21,8,29,17,26,0,27,15,25,2,22,0,10,0,2,11,11,8,24,14,19,7,17,6,21,2,2,13,13,0,8,2,4,1,30,15,15,7,8,7,28,3,16,13,23,9,19,3,30,7,22,4,19,3,18,17,17,3,14,13,18,11,26,11,21,7,26,5,20,15,20,12,17,14,24,7,7,3,33,3,23,6,6,9,10,19,19,11,16,8,13,17,22,8,9,10,15,1,33,9,14,6,24,4,28,1,14,0,13,5,15,3,26,9,22,});
sparse_graph3_ints[27][80] = vector<int>({26,27,7,33,1,2,10,10,16,32,0,22,4,5,10,17,13,24,23,24,0,26,0,32,9,23,0,40,0,18,18,24,0,36,0,0,10,35,8,8,17,17,12,12,0,14,15,15,13,29,19,19,4,31,1,4,20,30,5,19,8,30,19,26,4,27,4,38,6,7,14,26,0,28,5,16,3,4,15,30,25,25,0,16,7,21,5,6,14,21,21,27,21,23,0,20,11,32,12,19,23,28,0,1,8,28,0,12,0,24,0,30,0,34,9,9,4,33,0,38,18,29,16,23,4,25,11,11,4,36,16,27,13,13,5,14,0,2,10,25,13,33,0,10,2,2,6,20,2,4,7,26,19,21,14,14,11,28,8,15,0,8,8,34,8,22,4,13,9,16,16,16,11,18,7,7,3,30,13,20,3,24,18,18,10,31,7,36,3,12,21,21,3,35,16,21,3,22,11,23,});
sparse_graph3_ints[27][100] = vector<int>({10,45,0,3,0,38,0,50,0,32,33,33,6,20,10,32,5,6,22,23,16,16,19,19,26,27,26,37,12,39,6,39,0,7,0,17,10,28,20,38,21,28,0,0,5,16,0,29,0,47,10,14,0,13,14,35,31,31,0,24,17,18,19,32,5,31,1,42,11,19,21,21,8,8,18,28,19,36,13,33,5,26,28,28,17,26,9,26,0,5,23,34,1,26,4,43,0,9,4,48,29,30,17,23,27,34,14,43,0,19,0,34,9,35,8,42,24,31,13,28,22,39,12,16,4,38,24,24,30,35,0,22,0,15,14,30,10,37,7,32,0,40,7,11,17,31,18,41,24,36,10,22,0,36,11,11,5,28,15,22,16,40,6,23,26,32,17,35,4,45,0,45,13,13,12,42,23,29,0,1,4,36,7,7,8,44,21,33,10,10,13,21,15,38,8,40,5,18,0,2,});
sparse_graph3_ints[23][6] = vector<int>({2,2,0,0,0,1,0,3,0,2,1,2,1,1,});
sparse_graph3_ints[23][8] = vector<int>({2,3,0,0,0,2,0,4,0,1,2,2,1,1,0,3,1,2,1,3,});
sparse_graph3_ints[23][10] = vector<int>({3,3,0,0,0,2,0,5,2,2,0,1,0,4,0,3,2,4,1,2,1,1,2,3,1,4,1,3,});
sparse_graph3_ints[23][12] = vector<int>({4,4,0,0,0,2,0,6,2,2,0,4,0,1,3,3,1,2,0,5,2,5,2,3,0,3,1,1,3,4,2,4,1,4,1,3,1,5,});
sparse_graph3_ints[23][14] = vector<int>({0,0,4,5,0,4,0,2,0,7,3,3,2,2,0,1,0,6,1,2,2,6,0,5,3,4,2,3,4,4,2,4,0,3,1,1,1,5,3,5,1,4,1,6,1,3,2,5,});
sparse_graph3_ints[23][16] = vector<int>({5,5,0,0,1,2,0,8,3,3,0,1,0,6,0,4,4,4,0,2,2,7,2,2,0,5,0,7,3,4,2,3,4,6,2,6,2,4,4,5,0,3,1,6,1,1,1,5,3,5,3,6,2,5,1,7,1,4,1,3,});
sparse_graph3_ints[23][18] = vector<int>({6,6,0,0,2,2,0,9,4,4,0,2,0,7,0,5,3,3,0,1,2,8,1,2,5,5,0,6,0,8,0,4,2,6,4,7,3,4,2,3,4,5,2,7,4,6,2,4,1,6,0,3,1,1,3,6,5,6,3,7,1,7,3,5,1,5,2,5,1,4,1,8,1,3,});
sparse_graph3_ints[23][20] = vector<int>({6,7,0,0,2,2,0,10,4,4,0,2,0,6,0,8,3,7,3,3,0,1,4,8,1,2,0,5,5,6,0,7,4,5,2,9,0,4,3,4,0,9,2,6,2,3,5,5,6,6,2,8,3,6,1,8,4,6,4,7,1,6,2,4,1,7,0,3,1,1,3,8,5,7,2,7,3,5,1,5,2,5,1,9,1,4,1,3,});
sparse_graph3_ints[23][23] = vector<int>({0,0,0,8,7,8,0,3,0,5,5,6,3,10,4,4,0,10,0,1,3,3,1,1,0,7,1,11,2,2,3,7,6,7,5,9,2,9,0,6,4,5,0,9,4,8,3,4,5,5,0,11,6,6,0,4,2,6,2,3,7,7,2,7,2,8,3,9,4,7,5,8,5,7,1,9,1,5,6,8,1,7,2,5,4,9,3,8,4,6,3,5,2,10,3,6,1,2,0,2,1,10,1,8,1,6,2,4,1,3,1,4,});
sparse_graph3_ints[23][26] = vector<int>({4,4,0,13,0,0,8,9,1,2,0,10,6,7,2,3,0,8,0,1,5,6,0,6,2,10,6,10,0,2,0,4,2,12,4,5,2,8,3,3,5,9,0,7,7,8,0,11,4,11,4,8,0,9,3,4,5,5,0,12,6,6,7,7,3,8,2,11,0,5,2,2,8,8,4,10,2,6,5,8,6,9,3,10,3,7,3,9,1,8,1,10,6,8,7,9,3,6,5,10,4,9,5,7,1,9,1,11,4,6,4,7,3,11,2,9,1,6,1,7,1,5,2,7,0,3,1,1,3,5,1,12,2,4,2,5,1,4,1,3,});
sparse_graph3_ints[23][30] = vector<int>({0,11,0,0,10,10,3,3,2,14,0,2,0,8,8,8,5,6,0,4,6,12,0,13,5,10,4,4,7,7,0,1,2,8,3,10,1,2,0,7,0,15,2,3,4,5,4,8,9,9,2,12,0,9,0,6,4,13,6,7,5,5,0,12,3,4,0,10,8,11,4,12,6,10,0,14,6,6,5,9,2,9,6,11,7,8,2,10,8,9,2,13,3,11,8,10,0,5,2,2,2,7,2,6,7,10,5,11,3,12,5,8,1,12,9,10,7,11,4,10,7,9,6,9,6,8,3,8,3,7,4,11,5,12,3,9,1,13,1,11,4,9,3,13,2,11,5,7,4,7,1,9,1,10,4,6,1,14,1,6,1,5,1,7,1,8,3,6,0,3,3,5,1,1,2,4,2,5,1,3,1,4,});
sparse_graph3_ints[23][35] = vector<int>({0,0,8,8,1,17,11,12,2,2,4,4,0,14,6,6,0,10,5,13,1,1,10,10,4,10,3,3,5,15,5,5,0,16,0,7,4,12,2,10,0,1,0,3,0,5,7,7,9,9,9,13,0,8,0,13,4,5,2,15,6,11,5,6,8,11,0,11,0,9,7,14,6,7,3,16,7,8,4,11,7,13,0,15,3,4,0,12,0,6,2,14,2,12,10,11,2,8,5,14,2,11,8,9,6,10,7,11,2,13,4,13,11,11,8,12,2,9,3,15,0,17,4,9,9,10,9,12,4,8,6,13,6,12,9,11,10,12,4,14,6,9,8,13,5,9,7,12,2,3,8,10,2,7,5,11,6,14,0,4,2,6,3,13,7,10,5,12,7,9,1,15,3,14,3,11,4,15,1,16,5,10,1,13,3,12,6,8,2,16,3,7,3,8,3,9,1,14,1,11,});
sparse_graph3_ints[23][40] = vector<int>({0,20,1,2,9,9,8,16,0,16,3,4,6,7,0,0,0,10,13,13,11,11,0,14,4,18,6,13,1,4,4,12,4,5,0,18,0,1,2,10,4,16,5,6,0,8,8,8,0,2,4,10,7,14,2,2,0,6,10,10,10,15,12,12,2,16,7,8,5,5,0,13,4,13,6,6,4,4,0,11,0,9,9,13,2,19,6,12,8,9,7,7,6,16,4,11,9,10,8,12,6,17,0,15,2,17,8,15,10,11,4,14,2,14,0,17,10,14,0,12,6,14,11,12,4,15,2,18,4,17,12,14,2,11,2,15,0,7,3,3,0,19,2,12,6,11,6,10,12,13,2,13,8,14,2,8,10,13,9,12,2,9,11,14,7,15,5,16,8,11,9,14,4,9,11,13,10,12,7,11,4,8,9,15,7,13,5,15,8,13,7,16,6,15,9,11,7,12,5,13,});
sparse_graph3_ints[23][45] = vector<int>({0,17,15,15,0,7,0,3,0,13,0,21,9,9,10,14,0,0,7,19,5,5,7,7,6,14,2,3,11,17,0,11,10,11,0,15,13,13,4,19,5,12,0,9,2,18,0,1,6,6,6,16,1,1,4,4,8,8,1,22,4,11,1,3,3,3,7,17,4,14,11,12,5,6,6,7,9,10,3,21,2,14,0,10,11,15,14,14,0,18,0,12,8,14,4,10,4,5,2,20,9,18,8,9,7,8,0,14,6,13,10,10,0,8,2,17,11,11,5,20,9,16,5,11,13,16,6,15,12,13,0,20,8,15,9,17,3,9,4,18,4,16,11,16,0,16,12,12,7,18,4,17,6,12,3,10,0,19,13,14,2,15,2,19,8,13,6,17,2,16,4,15,8,12,5,19,13,15,9,15,11,14,3,4,0,22,12,15,2,12,0,6,10,13,2,13,9,13,10,16,});
sparse_graph3_ints[23][50] = vector<int>({14,14,7,14,0,14,2,4,0,0,5,6,4,23,0,20,12,19,9,9,12,12,7,8,7,17,0,2,0,23,0,9,16,17,4,13,2,2,0,12,8,21,0,18,10,16,0,25,6,20,0,16,4,4,6,7,10,18,10,10,0,1,2,20,4,16,1,2,1,4,6,13,1,6,11,11,5,5,8,15,4,10,5,17,4,20,13,13,6,6,5,11,0,10,15,16,8,9,3,15,12,17,1,12,0,11,3,18,2,22,8,8,0,15,9,10,10,11,2,24,14,18,6,12,8,14,11,12,10,20,7,13,12,13,8,19,6,22,7,7,5,18,4,5,6,19,0,17,4,11,9,16,0,21,13,14,14,16,3,16,0,8,12,16,8,18,10,17,14,15,12,18,4,21,8,20,10,14,0,19,10,19,6,21,4,22,0,22,16,16,2,21,2,14,2,18,14,17,0,24,});
sparse_graph3_ints[23][55] = vector<int>({18,18,3,4,7,19,11,22,0,17,0,3,0,10,14,15,0,24,1,13,0,0,11,11,1,27,8,9,0,21,12,13,6,13,6,7,16,16,5,25,5,17,12,19,2,3,15,20,0,12,11,17,8,22,4,5,0,1,4,22,4,12,9,15,5,6,7,8,0,26,10,10,1,1,4,16,1,3,0,8,1,22,3,3,7,14,6,18,3,15,0,15,10,16,13,14,9,9,6,6,7,7,6,22,11,12,1,19,2,25,14,19,5,5,17,17,0,13,13,17,3,21,2,17,8,8,8,15,0,11,4,13,9,23,2,20,7,20,10,11,9,10,0,16,12,12,13,13,10,20,7,13,8,14,4,24,9,16,2,23,11,18,3,26,13,21,10,17,3,18,4,19,14,14,5,20,12,20,6,12,0,18,15,15,7,24,8,21,9,19,10,21,15,16,4,11,14,18,6,23,});
sparse_graph3_ints[23][60] = vector<int>({9,15,1,2,0,30,6,6,8,9,20,20,0,22,10,25,15,15,4,16,0,0,3,4,1,14,17,17,7,22,1,25,0,10,0,17,16,22,4,28,13,13,10,10,8,24,5,21,1,4,14,20,0,13,11,18,4,19,6,8,0,1,0,24,10,12,0,20,11,22,3,6,4,14,0,2,2,2,1,28,2,4,13,18,5,6,4,4,6,16,0,27,10,16,7,26,4,12,8,20,7,8,14,14,6,19,3,25,0,12,9,9,16,20,12,12,6,7,10,20,7,14,4,23,6,24,1,18,16,16,3,18,9,18,13,22,5,26,9,10,3,21,19,19,0,14,9,22,11,12,0,16,12,20,5,14,8,8,12,13,5,5,14,23,10,11,13,14,2,23,0,11,4,13,18,18,11,17,8,15,0,18,14,15,9,16,2,29,6,18,14,19,3,27,7,13,12,24,18,21,});
sparse_graph3_ints[23][70] = vector<int>({1,2,0,12,14,14,11,11,0,20,0,0,23,23,6,28,20,20,0,28,17,26,4,5,11,29,7,24,5,32,12,25,18,18,0,32,9,16,0,25,12,22,9,10,0,35,7,20,7,8,16,16,12,18,1,4,21,22,0,14,0,22,15,25,3,4,20,25,4,14,0,30,5,6,18,23,14,28,16,21,0,1,2,14,6,7,4,25,9,27,8,9,12,12,8,31,2,22,0,2,0,10,5,15,14,19,8,15,9,25,2,2,3,31,2,4,4,4,6,30,6,26,13,13,11,21,9,20,3,23,0,17,12,27,3,18,3,29,6,18,6,22,7,7,10,10,2,17,1,26,11,16,8,8,15,15,3,27,13,23,17,17,15,20,8,22,10,11,9,9,6,6,5,20,10,18,3,20,6,16,8,18,19,19,17,22,1,14,0,13,2,34,9,29,13,18,1,33,14,15,});
sparse_graph3_ints[23][80] = vector<int>({1,2,0,25,8,32,9,26,7,15,16,23,9,9,25,25,22,22,12,34,12,18,6,37,18,18,0,40,0,36,0,0,7,20,17,27,12,13,7,8,21,27,13,21,4,29,0,18,16,32,0,30,0,22,20,30,1,4,0,14,18,24,0,27,0,16,0,20,12,28,26,27,3,4,4,33,16,17,4,5,9,22,23,24,5,6,23,28,0,34,15,30,0,32,18,29,6,7,20,21,4,31,20,25,0,38,8,30,5,25,0,12,0,1,10,10,6,35,12,15,13,25,5,14,15,26,9,24,8,16,12,32,0,2,4,24,9,35,2,2,11,12,0,10,2,4,2,22,4,13,4,4,18,20,8,21,9,33,3,36,8,8,14,15,10,27,6,19,11,30,3,38,6,26,4,19,3,23,12,24,14,29,10,11,15,22,12,20,2,27,3,18,7,29,15,16,3,28,7,7,});
sparse_graph3_ints[23][100] = vector<int>({26,27,4,46,13,18,7,7,30,34,0,27,0,0,4,17,14,32,12,44,0,41,19,19,23,23,12,40,12,15,1,2,7,11,4,31,21,29,8,36,5,6,21,21,15,26,6,18,16,34,15,20,26,34,4,36,29,30,0,36,22,37,0,5,10,30,0,7,18,39,5,28,5,33,12,26,0,9,5,21,8,41,12,36,12,31,4,42,0,19,21,35,8,46,25,25,25,32,10,10,11,12,16,38,23,31,0,32,28,28,4,48,0,23,4,44,8,22,0,50,16,17,0,29,8,43,7,9,0,46,0,17,8,38,0,44,16,42,8,34,19,27,0,48,8,20,4,39,20,40,31,31,33,33,14,41,0,15,8,24,18,22,20,24,27,36,22,26,14,37,9,27,20,32,18,30,12,13,24,38,16,28,0,13,18,35,0,39,0,25,0,34,14,15,11,23,15,22,4,27,});
sparse_graph3_ints[20][6] = vector<int>({2,2,0,0,0,2,0,1,0,3,1,2,1,1,});
sparse_graph3_ints[20][8] = vector<int>({2,3,0,0,0,2,0,4,0,1,0,3,2,2,1,1,1,2,1,3,});
sparse_graph3_ints[20][10] = vector<int>({0,5,0,0,0,2,3,3,2,2,0,1,0,4,0,3,2,4,1,2,2,3,1,1,1,3,1,4,});
sparse_graph3_ints[20][12] = vector<int>({4,4,0,0,0,2,0,6,2,2,0,4,0,1,3,3,1,2,0,5,2,5,2,3,0,3,2,4,1,1,3,4,1,4,1,5,1,3,});
sparse_graph3_ints[20][14] = vector<int>({4,5,0,0,1,2,0,7,3,3,0,1,0,4,0,2,0,6,2,2,2,6,0,5,3,4,2,3,4,4,2,4,2,5,0,3,1,1,1,5,3,5,1,4,1,3,1,6,});
sparse_graph3_ints[20][16] = vector<int>({0,8,0,0,2,2,5,5,0,2,0,6,3,3,4,4,0,1,2,7,1,2,0,5,0,4,0,7,3,4,2,3,2,6,4,6,4,5,1,5,1,6,3,5,2,4,0,3,3,6,1,1,2,5,1,7,1,4,1,3,});
sparse_graph3_ints[20][18] = vector<int>({6,6,0,0,2,2,0,9,4,4,0,2,0,7,0,5,0,1,3,3,2,8,1,2,5,5,0,6,2,6,0,8,4,7,3,4,0,4,4,5,2,3,2,7,4,6,1,5,1,6,3,6,5,6,3,7,1,7,3,5,2,4,0,3,1,1,2,5,1,8,1,4,1,3,});
sparse_graph3_ints[20][20] = vector<int>({0,0,6,7,2,2,0,10,4,4,0,2,0,6,0,8,3,7,0,1,3,3,1,2,4,8,0,5,5,6,4,5,0,7,2,9,3,4,0,9,2,6,5,5,0,4,2,3,6,6,2,8,3,6,4,7,1,8,4,6,5,7,1,6,1,7,1,5,3,8,3,5,2,7,2,4,1,9,0,3,2,5,1,1,1,3,1,4,});
sparse_graph3_ints[20][23] = vector<int>({0,0,0,6,7,8,1,11,5,5,1,1,2,2,0,8,4,4,0,10,0,1,6,7,0,3,3,3,5,9,0,5,2,8,4,8,3,4,3,10,5,6,4,5,0,7,0,9,2,9,6,6,0,11,7,7,2,7,2,6,2,3,0,4,3,9,3,7,5,8,4,7,5,7,6,8,1,9,4,9,3,8,4,6,2,10,1,5,1,7,2,5,1,10,3,5,3,6,1,8,1,2,1,6,2,4,0,2,1,3,1,4,});
sparse_graph3_ints[20][26] = vector<int>({8,9,2,3,0,0,0,8,2,12,6,7,1,2,0,11,4,5,0,1,3,4,2,8,0,6,0,2,5,6,6,10,2,2,0,13,2,10,5,9,4,4,7,8,0,9,4,11,4,8,3,3,5,5,0,7,0,10,6,6,0,12,7,7,2,11,8,8,3,8,4,10,2,7,2,6,5,8,6,9,0,5,3,10,0,4,6,8,7,9,3,9,1,10,3,7,5,10,4,9,5,7,3,11,1,11,1,9,4,6,4,7,2,9,1,12,1,6,1,7,1,5,3,6,1,8,3,5,2,4,2,5,0,3,1,1,1,3,1,4,});
sparse_graph3_ints[20][30] = vector<int>({10,10,1,2,6,6,2,14,0,8,0,0,0,11,8,8,2,3,6,12,0,13,4,5,0,1,5,10,3,4,2,8,0,2,0,6,2,2,7,7,3,10,0,15,5,6,4,4,2,12,9,9,0,9,4,12,5,5,6,7,2,10,8,11,6,11,4,13,3,3,0,12,0,10,0,7,0,14,5,9,7,8,6,10,8,9,2,9,2,13,4,8,8,10,3,11,7,10,5,11,9,10,7,11,3,12,7,9,2,7,2,6,5,8,4,10,1,12,6,9,5,12,0,5,6,8,4,11,0,4,3,13,1,13,3,9,3,8,3,7,1,11,4,9,5,7,2,11,1,14,1,9,4,7,4,6,1,10,1,7,1,6,1,5,3,6,1,8,3,5,2,4,2,5,0,3,1,1,1,3,1,4,});
sparse_graph3_ints[20][35] = vector<int>({5,15,0,3,0,8,6,6,9,9,0,14,11,12,0,0,1,17,0,10,2,3,4,12,0,12,8,12,4,10,3,4,0,1,5,5,0,16,1,1,8,8,5,13,2,2,0,6,7,7,4,5,5,6,2,15,6,11,9,13,4,4,10,11,6,7,0,9,7,8,0,11,2,11,9,10,3,16,7,14,4,11,2,10,0,15,2,14,5,14,8,9,7,11,10,10,0,13,2,12,11,11,6,10,2,13,4,13,7,13,3,3,0,7,2,8,3,15,9,12,2,9,8,11,0,17,6,13,4,9,9,11,10,12,6,12,4,8,8,13,4,14,7,12,8,10,6,14,5,11,6,9,5,9,7,10,3,13,5,12,7,9,4,15,1,15,3,14,2,16,2,7,3,11,1,16,2,6,5,10,1,13,3,12,6,8,0,5,1,14,0,4,3,9,1,11,4,7,5,8,});
sparse_graph3_ints[20][40] = vector<int>({0,20,0,16,13,13,1,2,9,9,5,6,3,4,0,0,9,13,7,8,4,18,8,16,0,14,0,10,6,13,1,4,4,12,0,18,0,1,4,5,4,16,6,7,0,8,0,2,10,10,2,2,4,10,0,6,3,9,12,12,5,5,0,11,8,8,10,15,7,14,2,16,0,13,4,13,8,9,7,7,10,11,6,6,4,4,6,12,0,9,4,11,2,19,9,10,6,16,0,12,11,12,6,17,8,15,0,15,2,17,3,10,8,12,10,14,11,11,0,17,4,14,12,14,6,14,4,17,2,14,2,18,4,15,12,13,2,15,0,19,8,14,2,12,6,11,9,14,11,13,2,13,6,10,9,12,11,14,7,15,0,7,3,3,10,13,10,12,9,15,5,16,2,8,2,9,8,11,7,13,7,11,5,15,8,13,7,16,9,11,6,15,5,17,3,17,5,13,7,12,});
sparse_graph3_ints[20][45] = vector<int>({12,12,15,15,3,21,9,9,5,5,0,0,3,3,7,7,0,19,0,12,10,11,7,17,1,1,11,17,0,14,0,17,1,3,6,13,0,21,4,4,6,15,7,19,0,9,4,11,6,6,0,1,11,15,0,3,2,14,3,19,8,8,2,3,2,5,5,12,9,13,5,6,0,10,6,7,14,14,2,18,2,16,9,10,1,22,0,11,9,15,0,13,8,9,10,10,5,17,7,8,4,10,13,13,5,11,6,12,0,15,11,12,6,14,9,18,4,5,13,16,5,20,7,16,9,16,12,13,0,8,9,17,11,16,11,11,5,19,13,14,7,18,5,18,3,9,0,18,2,17,0,16,9,14,4,16,7,15,3,10,0,20,13,15,3,20,2,15,4,17,8,13,2,19,8,12,4,14,4,15,11,14,12,15,10,14,14,15,12,16,1,21,10,16,12,14,10,17,10,15,});
sparse_graph3_ints[20][50] = vector<int>({13,13,11,11,12,19,6,6,1,6,0,18,0,0,6,20,8,9,0,25,0,15,0,10,5,18,1,2,6,13,16,17,1,12,15,15,10,18,1,4,9,15,8,21,2,22,1,19,4,5,4,23,0,1,7,7,4,12,7,14,0,2,2,2,2,20,10,10,2,4,5,6,4,4,10,16,4,16,1,16,7,8,12,12,0,12,9,9,6,7,14,14,5,5,8,8,0,23,0,14,8,14,6,12,0,11,12,17,0,21,10,11,9,10,3,15,11,12,5,17,14,18,7,13,12,13,6,19,5,12,0,16,8,15,2,24,9,16,13,14,10,20,10,17,3,16,4,11,3,18,4,10,6,22,4,20,12,18,12,16,8,18,16,16,14,17,8,20,14,15,10,19,8,19,7,17,4,22,0,19,0,9,6,21,4,21,15,16,2,21,0,22,0,8,13,17,2,23,0,24,});
sparse_graph3_ints[20][55] = vector<int>({18,18,2,5,8,8,11,12,0,21,9,23,0,3,0,10,0,0,1,27,11,17,0,16,15,16,6,21,6,7,5,19,13,21,1,13,7,13,1,24,0,18,2,3,12,19,0,12,13,14,4,17,0,23,4,5,0,1,9,9,4,24,4,13,5,6,9,15,1,1,1,3,0,26,7,22,3,3,3,5,3,16,7,15,10,11,7,7,10,16,12,13,7,19,6,6,7,8,9,10,3,19,1,19,4,21,17,17,14,15,10,19,0,13,11,11,0,15,8,9,15,20,8,17,5,5,4,12,10,10,12,12,13,13,3,23,7,24,8,15,3,26,0,11,14,18,6,12,9,21,7,14,0,17,14,14,10,17,15,15,12,18,11,21,3,15,6,13,5,18,4,16,2,25,16,17,2,21,8,20,13,20,11,22,5,25,15,19,16,16,0,19,17,19,9,22,2,22,4,11,});
sparse_graph3_ints[20][60] = vector<int>({17,21,12,24,3,20,8,8,1,2,11,17,6,27,0,10,0,30,0,0,0,27,11,12,14,20,5,16,16,16,0,16,0,13,10,10,6,6,11,20,0,18,0,24,1,4,12,14,18,18,10,23,20,20,3,4,4,23,4,26,0,1,8,22,6,8,3,6,0,2,8,19,4,15,2,2,9,9,9,15,2,4,0,22,4,4,5,6,7,24,14,14,3,28,9,25,2,18,5,21,1,25,6,18,8,14,7,8,6,7,1,14,0,12,12,18,9,17,8,9,10,11,15,15,7,21,4,14,9,10,5,24,4,18,12,12,1,28,13,22,0,14,17,17,1,20,7,16,12,13,6,14,11,11,13,14,6,19,4,13,10,19,13,19,14,15,9,16,14,23,0,17,2,26,16,20,4,12,3,21,15,16,0,19,2,23,5,5,19,19,16,17,10,22,0,11,13,20,17,18,});
sparse_graph3_ints[20][70] = vector<int>({8,20,1,2,17,18,10,24,17,26,0,35,20,20,10,30,15,16,0,23,4,28,0,0,11,12,0,15,5,15,13,14,7,29,0,28,7,7,0,18,10,21,1,4,0,12,3,4,21,22,4,5,6,24,23,23,0,26,17,21,20,25,0,33,14,28,9,17,5,6,4,23,14,24,8,8,11,26,9,10,6,16,7,31,0,1,14,21,9,12,0,10,4,33,1,29,12,18,4,31,0,2,0,20,2,2,8,16,7,26,12,13,2,4,4,4,6,7,2,15,17,23,13,19,3,21,3,30,10,28,6,12,0,31,14,15,9,23,2,20,7,8,8,9,0,14,6,19,7,22,13,27,16,17,10,10,11,17,13,23,1,24,3,25,11,11,5,26,4,15,5,18,3,18,2,14,6,28,12,21,5,21,18,19,12,12,16,25,6,6,13,13,0,13,14,14,9,26,8,19,});
sparse_graph3_ints[20][80] = vector<int>({7,14,3,35,12,13,6,6,17,17,16,32,26,27,22,22,16,24,4,31,0,18,4,38,8,24,24,24,4,21,5,23,0,0,0,2,12,34,2,2,18,19,3,3,0,32,12,27,4,4,0,40,0,28,0,21,0,26,9,27,7,7,23,27,10,30,0,34,0,13,4,27,20,30,21,25,4,13,0,30,8,15,13,21,6,13,18,27,19,24,15,29,0,24,13,14,11,24,6,28,12,32,0,11,8,8,10,21,14,15,7,31,11,11,3,12,16,26,5,5,15,16,9,34,7,33,10,10,4,18,18,29,3,17,18,21,0,36,7,20,0,16,3,37,0,1,9,9,9,20,0,9,1,2,12,25,14,22,6,18,1,4,9,17,6,35,0,38,13,19,2,21,3,4,4,33,8,36,13,28,15,23,20,21,3,30,11,31,12,18,3,26,8,29,11,17,7,23,7,26,});
sparse_graph3_ints[20][100] = vector<int>({10,27,4,46,26,26,8,40,31,31,10,11,7,8,2,4,20,21,24,38,0,10,0,2,19,30,0,43,10,21,5,21,5,34,24,24,14,32,0,20,15,21,26,34,33,33,4,32,29,35,11,14,0,0,22,36,0,13,0,4,22,23,18,25,16,23,0,37,5,29,15,15,18,38,2,6,20,40,9,37,21,31,16,42,13,30,14,27,7,12,10,34,23,32,16,18,8,9,12,44,17,34,9,32,4,37,19,35,10,23,0,16,13,37,20,27,8,46,5,39,29,30,11,16,5,27,4,42,10,25,0,33,11,12,0,18,5,19,19,19,5,25,0,48,5,17,14,34,8,42,10,43,23,27,13,39,0,39,4,48,17,28,10,19,14,41,0,50,0,31,4,44,5,23,0,29,26,31,0,27,0,46,16,16,0,25,25,29,28,28,7,31,0,35,0,23,6,6,11,28,});
sparse_graph3_ints[17][6] = vector<int>({2,2,0,0,0,1,0,2,0,3,1,1,1,2,});
sparse_graph3_ints[17][8] = vector<int>({2,3,0,0,0,2,0,4,0,1,1,2,2,2,0,3,1,1,1,3,});
sparse_graph3_ints[17][10] = vector<int>({0,5,0,0,0,2,2,2,3,3,0,1,1,2,0,4,2,4,2,3,0,3,1,1,1,3,1,4,});
sparse_graph3_ints[17][12] = vector<int>({0,0,4,4,0,4,0,6,0,1,1,2,3,3,0,2,2,2,0,5,2,5,2,3,2,4,0,3,3,4,1,1,1,4,1,5,1,3,});
sparse_graph3_ints[17][14] = vector<int>({0,0,4,5,2,2,0,7,0,2,0,5,0,1,3,4,1,2,2,6,0,6,2,4,0,4,2,3,4,4,3,3,2,5,1,5,3,5,0,3,1,1,1,6,1,4,1,3,});
sparse_graph3_ints[17][16] = vector<int>({0,0,0,8,2,2,5,5,0,2,0,6,3,3,0,1,4,4,1,2,2,7,0,5,0,7,3,4,0,4,2,6,2,3,4,6,4,5,1,5,1,6,3,5,3,6,2,4,2,5,1,7,0,3,1,1,1,3,1,4,});
sparse_graph3_ints[17][18] = vector<int>({6,6,0,0,0,4,0,9,4,4,0,2,0,7,3,3,0,1,1,2,2,8,0,6,5,5,2,3,2,6,3,4,0,8,4,7,4,5,2,7,0,5,2,2,4,6,5,6,3,6,1,5,3,7,1,6,3,5,1,7,2,4,2,5,1,8,0,3,1,1,1,3,1,4,});
sparse_graph3_ints[17][20] = vector<int>({0,0,0,6,6,7,0,10,1,2,4,5,0,8,0,1,2,3,3,4,0,2,0,4,2,8,5,6,4,8,4,4,3,3,0,7,2,9,5,5,0,9,2,6,6,6,3,7,0,5,2,2,4,7,3,6,4,6,5,7,1,8,3,8,1,7,1,6,1,5,3,5,2,7,1,9,2,4,2,5,0,3,1,1,1,3,1,4,});
sparse_graph3_ints[17][23] = vector<int>({0,0,7,8,2,2,0,8,1,11,5,6,0,6,1,1,0,10,4,5,0,1,3,4,0,3,5,9,0,5,2,8,6,7,3,3,4,8,4,4,5,5,3,10,0,7,0,9,2,9,6,6,7,7,0,11,2,7,2,6,3,9,5,8,4,7,3,7,2,3,5,7,6,8,0,4,4,9,1,9,2,10,4,6,3,8,1,10,1,7,3,5,3,6,1,5,2,5,1,8,2,4,1,6,1,2,0,2,1,3,1,4,});
sparse_graph3_ints[17][26] = vector<int>({0,6,2,8,0,0,0,13,8,9,1,2,6,6,4,4,4,10,0,1,0,11,2,3,0,8,0,2,2,2,5,5,7,8,3,4,2,10,4,5,6,7,2,12,6,10,0,9,5,6,3,3,4,11,0,7,0,10,7,7,4,8,2,9,0,12,8,8,2,11,5,9,6,9,5,8,2,7,7,9,6,8,2,6,3,10,5,10,3,9,4,7,4,9,3,7,5,7,3,11,0,5,1,11,0,4,3,8,1,9,4,6,1,10,1,12,1,7,3,6,1,8,3,5,1,6,1,5,2,4,2,5,0,3,1,1,1,3,1,4,});
sparse_graph3_ints[17][30] = vector<int>({0,11,10,10,1,2,0,0,2,14,7,8,0,8,6,12,5,6,2,3,0,13,0,1,4,10,3,4,4,5,0,2,2,2,2,8,0,6,6,7,6,10,9,9,0,15,4,4,2,10,6,6,5,5,0,9,2,12,7,7,4,13,8,8,8,11,4,12,6,11,0,12,2,11,8,9,0,10,3,3,5,10,0,7,2,13,0,14,2,9,8,10,4,9,4,8,9,10,7,10,5,11,7,11,5,9,7,9,3,12,5,12,3,11,6,9,6,8,4,11,2,7,3,13,2,6,1,13,3,9,5,8,3,10,1,11,5,7,4,7,1,12,1,14,3,7,0,5,0,4,1,9,3,8,4,6,1,10,1,7,3,6,1,8,3,5,1,6,1,5,2,4,2,5,0,3,1,1,1,3,1,4,});
sparse_graph3_ints[17][35] = vector<int>({7,8,11,12,0,3,5,6,0,0,0,8,2,11,1,17,9,10,5,15,4,12,2,3,0,10,0,14,0,1,3,4,0,16,1,1,4,5,5,13,1,3,3,3,6,7,8,9,5,5,6,6,4,10,0,11,7,13,10,11,7,7,2,15,4,4,9,13,0,9,8,8,2,10,9,9,4,11,3,16,7,14,10,10,7,11,11,11,5,14,0,15,6,11,2,14,0,12,8,12,0,13,4,13,2,12,6,10,2,13,3,15,9,12,8,11,10,12,0,17,2,8,9,11,6,13,0,6,0,7,2,9,8,13,6,12,7,12,4,14,6,14,8,10,4,9,4,8,5,11,7,10,3,13,4,15,5,12,7,9,6,9,2,16,5,9,1,15,3,14,1,16,3,11,5,10,1,13,3,12,6,8,1,14,2,7,2,6,3,9,1,11,5,8,3,10,5,7,1,12,});
sparse_graph3_ints[17][40] = vector<int>({11,11,0,20,1,2,5,6,0,0,9,9,6,17,13,13,4,12,3,4,0,12,4,16,7,8,0,14,10,15,0,10,0,18,1,4,0,16,8,14,6,13,0,1,4,5,0,2,6,7,2,2,4,10,3,3,4,4,10,10,6,6,5,5,8,8,12,12,6,12,8,9,2,19,7,7,0,11,2,16,9,10,4,11,3,10,7,14,4,13,10,11,0,9,8,16,0,8,11,12,4,18,3,9,10,14,0,15,0,13,6,16,8,15,12,14,8,12,2,17,12,13,0,17,6,14,4,17,2,18,4,14,4,15,10,13,2,14,9,13,0,19,2,15,11,13,11,14,9,15,7,15,9,14,9,12,10,12,6,11,2,12,5,16,6,10,2,13,7,13,7,16,8,13,5,15,8,11,9,11,6,15,7,11,5,17,3,17,2,8,2,9,0,6,0,7,5,13,7,12,});
sparse_graph3_ints[17][45] = vector<int>({8,16,0,14,3,21,0,10,0,0,0,3,0,17,7,8,9,10,0,20,11,12,6,15,7,19,15,15,11,17,0,12,2,3,2,5,4,5,4,18,5,6,0,1,3,16,1,1,0,22,6,7,6,12,2,14,1,3,11,15,3,3,8,9,4,4,7,13,4,10,10,11,6,6,5,5,5,11,12,13,9,9,14,14,0,11,8,8,8,14,10,10,0,13,11,11,3,15,7,7,5,12,6,13,4,11,0,18,0,15,5,19,12,12,8,15,13,13,9,17,13,16,2,20,9,18,11,16,4,16,13,14,7,18,10,15,2,19,5,20,0,9,10,14,6,17,13,15,6,16,0,19,0,8,3,9,3,10,2,18,0,16,4,17,4,19,14,15,0,21,2,15,12,16,10,16,12,15,7,17,12,14,8,13,8,17,2,21,10,17,9,15,11,14,10,13,11,13,9,16,});
sparse_graph3_ints[17][50] = vector<int>({16,17,10,10,0,18,10,20,0,12,0,0,1,2,0,25,14,15,10,16,12,12,4,17,6,22,8,9,0,14,1,4,0,16,8,15,3,4,3,6,0,23,5,6,6,18,6,7,2,20,0,1,6,20,0,10,7,8,0,2,2,2,4,12,2,4,10,18,4,4,1,19,7,13,6,6,11,11,13,14,7,7,9,9,3,15,8,8,9,10,5,5,1,12,4,13,8,14,14,18,10,11,12,13,6,13,0,11,11,12,4,16,2,24,9,16,2,22,6,12,7,14,15,16,0,15,5,18,12,18,0,21,13,13,14,14,9,15,3,16,10,17,0,17,12,19,4,11,14,16,12,17,4,10,5,17,8,21,12,16,16,16,6,19,4,23,8,20,10,19,8,18,8,19,0,19,14,17,4,22,4,20,6,21,15,15,4,21,2,21,15,17,13,17,0,22,13,18,2,23,});
sparse_graph3_ints[17][55] = vector<int>({13,19,6,7,18,18,8,8,9,16,10,10,0,0,0,3,2,22,7,24,14,14,4,16,2,3,3,26,2,5,12,12,4,13,11,18,0,12,4,5,5,23,15,20,11,22,5,6,1,13,0,19,0,26,1,21,8,20,16,16,0,10,0,16,0,1,5,17,1,1,9,9,1,3,6,18,3,3,1,24,3,5,7,19,8,15,8,22,7,7,11,11,7,14,10,17,3,15,6,6,7,8,13,13,2,13,9,10,15,15,0,13,11,12,8,9,10,11,7,13,17,17,12,13,0,17,4,15,9,15,8,14,13,14,11,20,4,12,4,19,5,5,14,15,1,19,10,16,6,21,5,20,0,11,11,17,2,25,4,17,6,12,0,23,15,16,1,27,13,21,3,23,16,17,0,21,11,19,0,18,15,19,13,20,9,23,11,21,17,19,8,21,5,25,17,18,13,18,13,17,});
sparse_graph3_ints[17][60] = vector<int>({10,16,6,14,14,14,0,0,0,13,20,20,5,24,10,25,3,28,0,8,0,16,7,8,16,22,1,4,0,27,14,20,16,16,18,18,7,20,11,18,6,6,0,18,0,25,0,23,9,21,8,9,7,25,9,10,1,10,1,19,10,11,5,5,11,12,0,1,4,19,9,15,4,22,1,2,3,4,0,2,0,30,2,2,4,26,2,4,8,14,6,27,4,5,12,19,6,8,13,13,8,23,4,17,4,14,6,19,8,8,6,7,3,21,13,22,1,14,15,15,9,9,0,12,6,22,12,12,1,21,0,14,10,10,3,24,17,17,11,21,10,17,12,13,11,11,10,23,13,14,5,14,1,28,8,15,9,16,14,15,0,17,5,17,19,19,15,16,10,18,14,19,14,23,1,18,12,18,6,18,15,21,4,13,7,14,4,12,17,18,7,19,0,21,18,21,17,20,2,26,});
sparse_graph3_ints[17][70] = vector<int>({13,13,11,11,0,2,6,10,0,18,0,21,10,20,14,28,23,23,14,24,4,24,17,17,4,27,19,19,0,13,0,0,0,25,2,2,4,20,13,22,2,4,10,30,7,13,6,28,10,27,6,32,20,21,6,6,7,7,6,30,0,30,20,25,6,21,12,17,0,23,17,24,1,14,16,21,0,11,5,5,12,12,14,26,10,24,15,15,6,17,11,16,8,17,17,26,14,14,2,15,9,19,2,18,16,16,7,22,3,33,0,9,3,31,12,20,0,34,0,1,1,26,4,4,1,2,3,29,7,26,17,22,0,32,1,4,9,10,3,4,18,18,20,23,5,16,3,23,9,29,7,19,11,25,9,22,6,12,3,19,8,9,4,15,0,28,13,18,14,20,6,24,11,22,10,11,7,8,8,24,3,26,0,14,11,12,0,16,12,13,8,16,7,15,6,7,4,21,15,18,});
sparse_graph3_ints[17][80] = vector<int>({1,4,3,4,1,2,0,1,6,7,6,26,0,22,22,23,10,10,25,25,18,20,11,18,13,29,3,36,20,30,16,18,6,30,11,12,13,24,16,32,0,11,7,23,12,34,7,8,9,33,4,17,15,30,21,27,26,27,4,15,8,19,14,14,9,31,19,24,12,13,16,25,23,28,4,29,13,26,0,32,12,19,4,22,3,26,15,15,18,29,15,21,16,16,18,22,21,21,0,25,6,32,12,32,19,26,0,20,6,22,5,5,6,19,13,20,9,35,10,28,12,23,10,26,10,22,6,36,16,27,10,16,2,22,9,25,3,38,4,19,0,18,0,40,2,25,23,24,8,16,6,34,0,38,3,34,3,11,0,0,3,32,3,28,2,15,0,36,0,34,0,9,0,30,0,28,4,12,0,2,8,14,0,15,2,2,6,12,18,18,7,28,2,4,4,4,6,6,8,9,});
sparse_graph3_ints[17][100] = vector<int>({20,20,28,28,19,32,15,16,0,33,13,31,30,35,4,41,24,25,11,12,4,4,0,17,0,0,2,2,2,47,11,42,18,18,13,14,21,34,0,14,10,10,9,31,12,36,31,31,26,37,17,26,22,39,5,43,4,8,13,26,3,37,8,8,18,41,20,27,26,27,0,2,14,20,14,43,12,24,15,40,2,4,0,41,8,36,0,20,19,38,10,22,4,6,29,30,10,45,23,36,7,19,27,34,8,26,7,44,9,9,6,47,23,23,1,24,5,33,0,37,12,13,0,46,4,28,10,20,33,33,0,30,8,39,9,28,14,15,5,30,16,22,23,28,6,18,25,33,12,34,0,50,8,34,5,24,17,37,5,17,13,39,17,24,22,31,9,41,9,19,0,44,13,29,0,28,20,29,15,36,17,28,0,23,16,31,4,39,16,33,21,21,3,24,25,30,21,23,0,35,});
sparse_graph3_ints[13][6] = vector<int>({2,2,0,0,0,1,0,2,0,3,1,1,1,2,});
sparse_graph3_ints[13][8] = vector<int>({0,0,2,3,0,2,0,4,0,1,1,2,2,2,0,3,1,1,1,3,});
sparse_graph3_ints[13][10] = vector<int>({0,5,0,0,0,2,2,2,3,3,0,1,1,2,0,4,2,4,2,3,0,3,1,1,1,3,1,4,});
sparse_graph3_ints[13][12] = vector<int>({4,4,0,0,0,4,0,6,0,1,1,2,0,2,3,3,2,2,0,5,2,5,2,3,2,4,3,4,1,5,1,4,1,3,0,3,1,1,});
sparse_graph3_ints[13][14] = vector<int>({4,5,0,0,2,2,0,7,0,2,3,3,0,1,0,5,1,2,2,6,0,6,3,4,4,4,0,4,2,3,2,5,2,4,3,5,1,5,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[13][16] = vector<int>({5,5,0,0,0,4,0,8,0,2,0,6,3,3,0,1,4,4,1,2,2,7,2,3,3,4,0,7,2,6,4,6,4,5,0,5,2,2,3,5,1,5,1,6,3,6,2,4,1,7,2,5,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[13][18] = vector<int>({6,6,2,2,0,0,0,9,4,5,0,2,0,7,3,3,0,1,1,2,2,3,2,8,4,4,0,6,3,4,4,6,0,8,4,7,2,6,2,7,5,5,0,5,5,6,0,4,3,7,3,6,3,5,1,7,1,5,1,6,1,8,2,4,2,5,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[13][20] = vector<int>({6,7,0,10,0,6,0,0,1,2,4,5,0,8,0,1,2,3,0,2,3,4,2,2,2,8,5,6,4,4,3,3,4,8,2,9,5,5,0,7,6,6,0,9,2,7,2,6,4,7,4,6,5,7,3,7,0,5,0,4,3,8,1,9,3,6,1,7,3,5,1,8,2,5,1,5,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[13][23] = vector<int>({0,3,7,8,0,0,0,6,0,10,4,5,4,8,0,8,0,1,1,11,2,3,1,1,3,4,2,2,5,6,4,4,5,9,5,5,6,7,3,3,2,8,3,10,0,7,0,9,2,9,7,7,6,6,0,11,3,9,5,8,2,7,6,8,5,7,2,6,4,7,3,7,4,9,2,10,1,9,4,6,3,8,0,5,1,10,0,4,1,7,3,6,3,5,1,8,2,5,1,5,2,4,1,6,1,3,1,4,1,2,0,2,});
sparse_graph3_ints[13][26] = vector<int>({0,13,1,2,8,9,0,0,3,4,6,7,2,8,4,10,0,1,2,3,0,8,4,5,0,2,2,2,0,6,2,10,5,6,4,4,7,8,5,5,6,6,6,10,2,12,0,9,2,9,7,7,4,11,3,3,8,8,0,12,0,10,0,7,4,9,4,8,0,11,2,11,6,9,6,8,7,9,5,9,5,10,3,11,5,8,2,7,3,9,3,10,5,7,2,6,1,11,4,7,3,7,1,12,1,9,3,8,4,6,1,10,0,5,0,4,1,7,3,6,1,8,3,5,2,5,1,5,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[13][30] = vector<int>({6,12,4,5,1,2,0,15,0,0,6,7,10,10,0,10,4,11,8,9,2,3,0,13,0,1,0,8,3,4,0,2,2,10,2,2,5,6,0,6,7,8,5,5,6,6,4,4,2,14,4,10,7,7,8,8,0,9,2,9,8,10,4,13,8,11,6,10,4,12,6,11,2,12,0,12,2,11,9,9,2,13,0,14,2,8,9,10,3,3,0,11,0,7,7,11,7,10,4,9,7,9,5,11,4,8,6,9,5,9,5,12,3,13,5,10,3,11,6,8,3,12,1,13,1,14,3,9,5,8,3,10,1,11,2,7,5,7,1,12,2,6,4,7,3,7,1,9,3,8,1,10,4,6,0,5,0,4,1,7,3,6,1,8,3,5,2,5,1,5,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[13][35] = vector<int>({1,17,4,11,7,7,0,10,0,0,1,1,1,3,0,14,5,15,11,12,8,9,3,3,4,4,4,13,0,16,5,5,0,1,0,3,6,6,2,3,2,11,3,4,4,5,7,11,5,6,7,8,10,11,6,7,0,11,7,13,9,10,8,8,0,9,4,10,2,10,9,13,0,8,3,15,4,12,10,10,7,14,11,11,9,9,5,14,3,16,8,12,5,13,0,15,2,14,9,12,10,12,2,15,9,11,6,11,8,11,6,10,0,12,0,13,8,13,2,13,2,12,6,13,0,17,6,12,6,14,7,12,8,10,4,14,2,8,2,9,4,15,0,6,0,7,2,16,5,11,7,10,3,13,5,12,7,9,1,15,4,9,3,14,4,8,1,16,6,9,5,9,3,11,5,10,1,13,3,12,6,8,1,14,3,9,1,11,5,8,3,10,2,7,1,12,5,7,2,6,});
sparse_graph3_ints[13][40] = vector<int>({5,6,4,16,13,13,9,9,0,16,0,0,1,2,0,12,0,20,8,14,1,4,0,18,0,14,3,4,0,10,10,15,11,11,4,5,4,12,6,7,0,1,6,13,4,18,0,2,4,10,2,2,8,8,2,4,4,4,6,6,5,5,10,10,7,8,8,9,7,7,12,12,6,12,0,11,9,10,4,11,8,16,10,11,3,10,4,13,11,12,2,16,10,14,3,9,6,15,0,13,8,15,12,14,0,15,2,19,12,13,6,17,0,9,6,16,6,14,2,17,0,8,0,17,8,13,8,12,11,13,4,17,2,18,11,14,10,13,9,13,4,14,9,15,4,15,0,19,9,14,10,12,7,15,2,14,2,15,7,16,7,13,9,12,5,17,7,14,5,15,5,16,9,11,6,11,3,17,6,10,8,11,7,11,2,12,2,13,3,18,5,13,7,12,5,14,3,15,1,19,});
sparse_graph3_ints[13][45] = vector<int>({7,17,10,11,0,19,11,17,8,8,6,6,0,0,0,14,0,3,3,15,2,3,2,5,15,15,0,10,0,21,4,5,0,12,7,19,5,12,4,18,7,7,1,22,9,9,4,16,0,1,6,13,1,1,11,15,11,12,7,14,1,3,3,3,2,14,8,15,4,4,5,6,6,7,5,5,7,8,9,10,12,13,8,9,0,11,10,10,5,11,14,14,11,11,0,13,6,12,7,13,4,11,4,10,12,12,0,15,8,14,3,21,2,15,13,13,13,16,13,14,9,18,3,16,9,15,9,17,11,16,4,17,0,18,10,15,13,15,5,19,2,20,7,18,5,20,14,15,12,16,6,17,12,15,6,16,3,9,10,16,3,10,12,14,0,20,4,19,2,18,2,19,10,14,10,17,0,9,8,17,8,18,8,16,11,14,9,16,11,13,0,16,6,18,0,8,0,17,10,13,});
sparse_graph3_ints[13][50] = vector<int>({3,17,10,20,10,16,8,15,4,12,1,4,1,23,4,4,0,0,1,2,6,22,0,21,6,13,11,11,5,5,16,17,9,9,6,6,7,7,2,16,0,15,10,18,0,12,13,13,10,10,7,14,0,1,12,12,0,7,0,25,0,2,0,19,5,18,2,2,14,14,2,4,3,6,8,8,5,6,6,7,13,17,6,19,7,8,8,9,1,12,0,14,6,12,10,11,9,10,2,21,0,10,11,12,14,18,6,14,1,16,8,14,15,16,9,16,12,13,0,11,14,15,5,12,13,14,9,15,0,16,5,17,8,19,3,16,2,24,12,19,3,18,16,16,15,15,6,18,10,19,3,22,4,11,6,20,4,10,12,16,8,20,12,18,8,21,11,16,14,17,2,20,4,23,0,20,15,17,8,18,14,16,13,16,6,21,13,18,11,18,4,20,0,22,11,19,11,17,12,17,});
sparse_graph3_ints[13][55] = vector<int>({9,9,17,17,9,17,6,6,2,3,0,3,0,0,1,25,11,22,3,4,8,20,7,13,7,24,4,5,11,11,12,18,1,20,14,14,4,13,3,26,7,7,15,20,8,8,7,15,0,16,3,17,9,15,6,19,13,13,5,23,3,21,15,15,0,12,11,17,1,1,0,8,0,19,0,1,0,27,1,3,12,12,3,3,0,22,10,21,14,18,0,24,5,5,1,17,5,6,4,18,6,9,6,7,5,21,9,10,1,14,2,12,10,11,5,13,10,13,8,9,17,19,0,13,10,10,4,12,12,13,7,14,15,16,13,14,13,16,9,16,7,16,6,13,15,19,16,17,6,18,10,17,0,17,12,20,10,19,4,17,13,21,17,18,0,10,4,19,3,16,14,19,16,16,0,11,7,22,12,19,10,20,9,23,18,18,6,21,0,20,2,23,7,23,3,24,16,18,5,25,});
sparse_graph3_ints[13][60] = vector<int>({0,18,12,24,6,6,9,16,1,4,1,2,0,1,5,10,7,21,20,20,14,17,7,25,0,13,5,5,3,27,6,11,6,14,12,12,12,21,3,25,16,22,16,19,8,14,11,16,0,26,13,16,4,18,0,22,0,30,0,9,15,18,6,18,4,4,11,20,2,18,0,28,1,24,0,0,2,9,9,9,10,19,4,28,8,26,4,23,0,2,2,22,1,20,8,23,0,7,2,2,3,20,4,14,8,8,5,21,2,4,5,16,1,16,6,8,7,16,8,19,0,15,9,12,19,19,6,7,18,18,12,14,6,23,10,10,0,12,9,10,18,21,11,12,17,17,14,14,14,22,14,20,16,16,0,14,11,11,5,14,12,13,9,15,15,15,12,18,13,19,16,20,8,18,0,16,12,23,11,17,12,22,16,21,9,21,10,22,4,12,14,23,5,20,1,18,9,24,14,21,});
sparse_graph3_ints[13][70] = vector<int>({12,12,13,26,1,4,0,2,18,26,5,5,10,24,11,18,21,21,16,17,6,32,0,35,7,7,23,23,15,22,4,19,6,25,0,0,8,11,7,29,12,29,3,30,19,20,17,24,6,6,12,20,10,12,14,14,7,21,15,15,17,18,6,15,5,24,6,19,9,30,4,14,0,22,0,11,0,31,9,18,2,22,4,22,18,19,7,26,11,15,9,21,3,33,0,17,0,14,0,26,6,11,8,16,4,31,0,33,0,9,3,27,22,22,10,28,0,29,19,24,4,4,8,27,3,10,2,26,11,26,2,14,9,23,4,11,3,17,21,24,16,26,4,28,1,20,14,24,12,24,18,22,13,22,0,1,1,24,8,8,3,24,2,2,1,2,0,7,0,19,2,4,5,17,14,20,16,23,7,13,16,20,13,18,15,25,17,21,9,14,9,9,14,28,10,11,13,27,12,13,});
sparse_graph3_ints[13][80] = vector<int>({24,25,20,30,9,9,22,23,0,1,19,22,16,16,12,25,14,33,0,33,6,6,12,13,1,4,18,18,10,11,6,24,3,35,14,15,26,27,4,21,1,16,0,26,0,30,10,35,19,28,1,2,6,37,5,5,2,39,8,8,7,7,10,33,12,30,11,12,6,16,1,14,14,27,13,14,16,30,5,32,7,17,0,35,12,19,17,17,7,29,19,20,1,20,0,13,4,16,9,26,16,24,2,21,9,17,8,31,14,22,17,25,16,22,2,27,13,26,21,22,11,23,4,24,15,28,10,28,11,29,13,20,13,31,9,32,23,24,6,21,8,21,4,36,19,25,0,24,17,31,0,11,10,18,4,27,4,31,1,38,6,28,0,37,8,25,25,26,10,22,0,19,2,30,21,28,17,28,6,13,20,26,7,35,24,28,22,26,23,27,0,9,22,29,4,4,0,0,2,5,});
sparse_graph3_ints[13][100] = vector<int>({14,23,26,31,20,32,24,38,31,31,5,26,5,43,21,22,2,2,11,11,0,0,3,11,0,36,4,4,26,27,14,18,14,33,12,14,14,29,16,40,33,33,15,37,0,32,20,40,6,18,10,34,23,23,16,25,22,34,19,30,0,25,8,39,12,44,20,20,0,22,25,25,18,28,5,38,23,36,27,33,16,34,8,46,11,40,18,18,4,48,6,6,0,10,1,19,7,12,30,34,0,2,7,44,4,41,0,50,1,45,2,4,6,23,1,40,0,43,12,37,0,28,29,30,7,20,0,48,3,32,22,26,27,36,8,9,23,31,0,16,19,35,20,37,28,28,1,38,10,32,6,28,14,31,12,28,22,29,14,16,6,35,14,14,16,42,10,36,13,41,18,26,8,31,9,24,0,30,0,18,17,17,13,22,18,20,4,34,15,24,11,27,10,26,10,43,7,30,4,46,});
sparse_graph3_ints[10][6] = vector<int>({2,2,0,0,0,1,0,2,0,3,1,1,1,2,});
sparse_graph3_ints[10][8] = vector<int>({0,0,2,3,0,2,0,1,0,4,1,2,2,2,0,3,1,1,1,3,});
sparse_graph3_ints[10][10] = vector<int>({0,5,0,0,0,2,2,2,3,3,0,1,1,2,0,4,2,4,2,3,1,3,1,1,0,3,1,4,});
sparse_graph3_ints[10][12] = vector<int>({4,4,0,0,1,2,0,6,0,1,2,3,0,2,0,4,2,4,2,5,2,2,0,5,3,3,3,4,1,5,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[10][14] = vector<int>({1,2,0,0,4,5,0,6,0,1,2,3,0,2,2,2,3,4,0,7,2,6,3,3,4,4,0,5,0,4,2,5,2,4,3,5,1,5,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[10][16] = vector<int>({1,2,5,5,0,0,0,6,0,8,0,1,2,3,4,4,0,2,2,2,3,3,2,7,3,4,2,6,0,7,4,6,4,5,0,5,0,4,3,5,3,6,2,5,1,5,1,7,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[10][18] = vector<int>({3,3,6,6,0,0,0,9,0,2,4,5,0,7,2,2,0,1,1,2,2,3,4,4,2,8,0,6,3,4,4,6,4,7,0,8,2,6,2,7,5,5,5,6,3,7,0,5,3,6,3,5,1,7,0,4,1,8,2,5,1,5,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[10][20] = vector<int>({4,5,1,2,0,0,0,10,6,7,0,6,0,8,0,1,2,3,0,2,2,2,3,4,5,6,4,4,2,8,3,3,4,8,5,5,0,7,2,9,6,6,0,9,2,7,2,6,5,7,4,7,4,6,3,7,3,8,1,9,0,5,1,7,3,6,3,5,1,8,0,4,2,5,1,5,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[10][23] = vector<int>({0,3,4,5,7,8,0,0,0,6,0,10,4,8,0,8,0,1,1,11,2,3,1,1,2,2,3,4,5,6,4,4,5,5,5,9,6,7,3,3,2,8,3,10,7,7,0,9,0,7,6,6,2,9,6,8,3,9,0,11,5,8,5,7,2,7,4,9,4,7,3,7,2,10,2,6,1,9,3,8,4,6,1,10,1,7,0,5,3,6,3,5,1,8,0,4,2,5,1,5,2,4,1,6,1,3,1,4,1,2,0,2,});
sparse_graph3_ints[10][26] = vector<int>({4,10,0,0,8,9,1,2,4,5,0,13,6,7,2,3,0,1,0,8,3,4,0,2,2,8,2,2,0,6,5,6,2,10,4,4,7,8,5,5,6,6,6,10,0,9,7,7,2,12,2,9,8,8,4,11,3,3,0,12,2,11,0,7,0,10,4,9,7,9,4,8,0,11,6,9,6,8,5,9,5,10,3,11,5,8,3,9,3,10,5,7,1,11,2,7,1,12,4,7,3,7,2,6,1,9,3,8,4,6,1,10,1,7,0,5,3,6,1,8,3,5,0,4,2,5,1,5,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[10][30] = vector<int>({2,2,0,2,6,6,6,12,0,0,10,10,0,14,0,12,8,9,3,3,0,9,4,4,4,11,0,1,5,5,1,2,2,9,2,3,3,4,7,8,6,7,4,5,5,6,0,8,7,7,4,10,2,14,8,8,2,8,8,10,8,11,2,11,4,13,6,10,4,12,6,11,0,15,9,9,2,12,9,10,2,10,2,13,7,11,0,13,7,10,5,11,7,9,0,6,0,10,0,7,0,11,5,12,4,9,6,9,5,9,4,8,3,13,5,10,3,11,3,12,6,8,1,13,1,14,3,9,5,8,1,11,3,10,1,12,5,7,2,7,4,7,3,7,2,6,1,9,3,8,1,10,4,6,1,7,0,5,3,6,1,8,3,5,0,4,2,5,1,5,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[10][35] = vector<int>({7,8,5,6,0,14,9,10,0,0,0,3,2,3,11,12,3,4,4,13,0,16,4,5,6,7,0,11,1,17,0,1,1,1,1,3,7,13,8,9,3,3,2,11,4,4,4,11,5,5,6,6,7,7,10,11,0,10,7,11,8,8,2,10,4,10,9,13,3,15,10,10,0,9,7,14,4,12,11,11,5,15,0,8,9,9,5,14,8,12,10,12,3,16,5,13,7,12,0,15,9,12,9,11,2,14,8,13,2,15,6,11,6,14,6,10,0,17,6,12,8,11,6,13,4,14,8,10,2,13,2,12,0,12,0,13,4,15,2,16,5,11,7,10,3,13,5,12,1,15,3,14,7,9,1,16,0,6,2,8,2,9,0,7,4,9,6,9,5,9,4,8,3,11,5,10,1,13,3,12,1,14,6,8,3,9,1,11,5,8,3,10,1,12,5,7,2,7,4,7,});
sparse_graph3_ints[10][40] = vector<int>({0,20,6,14,11,12,9,9,4,10,0,0,0,11,0,2,2,2,2,4,4,12,4,4,5,5,12,14,4,18,8,16,6,6,0,13,8,8,10,10,1,14,0,1,0,16,1,2,1,4,1,6,8,14,2,16,4,5,6,7,5,6,7,8,6,12,0,10,8,9,7,7,10,11,9,10,0,12,10,14,4,11,12,13,11,11,4,13,6,13,12,12,6,15,0,14,10,15,8,15,2,17,3,9,3,10,13,13,6,16,6,17,0,17,2,19,11,13,4,16,11,14,0,9,10,13,8,13,0,18,9,13,9,15,4,17,8,12,2,18,0,8,9,14,10,12,7,15,0,19,4,14,4,15,7,16,5,17,7,13,9,12,7,14,5,15,5,16,2,14,2,15,9,11,3,17,3,18,8,11,6,11,7,11,6,10,1,19,5,13,7,12,3,15,5,14,3,16,1,17,});
sparse_graph3_ints[10][45] = vector<int>({13,14,11,17,9,9,6,6,0,13,5,5,1,3,0,0,1,1,3,3,0,15,7,14,7,7,7,9,3,19,15,15,7,19,7,17,10,10,6,13,0,9,11,11,3,5,12,13,2,15,0,21,1,22,0,1,3,13,0,3,3,11,2,3,4,17,2,5,8,15,3,16,5,6,0,19,6,7,0,11,7,8,8,9,13,16,0,10,9,10,6,12,11,12,9,16,0,12,10,11,7,13,11,15,11,16,5,12,13,15,12,12,9,17,4,11,7,15,13,13,3,15,9,18,0,14,5,18,0,16,9,15,3,21,7,18,4,16,14,14,5,19,0,18,14,15,5,20,12,16,12,15,10,16,12,14,3,9,3,10,5,17,10,17,0,20,3,20,11,14,10,14,9,13,8,18,2,18,10,15,8,16,1,21,2,19,8,17,11,13,6,18,6,19,4,5,10,13,7,16,});
sparse_graph3_ints[10][50] = vector<int>({10,18,15,15,16,17,13,14,12,12,8,10,8,8,8,21,5,5,3,18,1,4,0,2,0,0,0,13,4,4,4,23,0,25,10,10,6,20,10,16,4,13,0,16,6,14,7,17,8,14,0,19,2,16,4,19,12,18,1,23,5,8,0,1,0,7,2,2,1,2,14,18,11,11,2,4,3,11,3,6,6,6,5,6,7,7,0,21,7,8,8,9,12,17,4,12,9,10,0,12,9,12,8,19,10,13,10,20,12,13,7,16,12,16,12,19,6,13,8,20,14,17,6,19,6,15,14,14,0,11,0,10,10,19,5,13,0,15,13,13,3,16,5,17,14,15,16,16,0,17,3,17,2,20,8,18,11,16,6,22,3,22,6,21,2,24,4,11,15,16,1,22,0,20,13,17,0,14,15,17,9,15,14,16,3,21,8,17,13,18,4,22,11,19,10,15,5,19,11,17,});
sparse_graph3_ints[10][55] = vector<int>({17,19,14,14,13,20,10,11,10,17,4,13,8,22,1,11,7,7,1,3,0,0,1,27,0,8,3,21,1,1,0,15,7,9,13,13,15,15,4,24,7,19,12,17,16,16,6,15,17,17,9,23,5,25,4,22,0,26,0,23,1,24,3,4,2,12,3,17,3,8,1,21,0,19,0,1,2,19,15,19,4,19,8,17,0,3,4,15,3,3,2,3,0,17,5,5,4,5,7,13,9,15,14,18,10,21,6,7,10,13,8,11,11,22,6,9,9,9,6,6,12,19,8,9,10,18,11,14,10,20,11,11,12,12,3,13,0,13,7,16,6,23,13,21,6,21,10,16,10,10,7,24,12,18,8,14,14,17,5,17,15,20,13,14,5,20,6,18,8,21,6,12,6,22,10,19,8,20,2,25,14,15,3,15,17,18,3,26,12,16,16,17,0,20,0,18,0,10,11,21,});
sparse_graph3_ints[10][60] = vector<int>({6,6,8,26,2,2,1,16,0,0,20,20,16,22,9,9,0,9,16,16,12,24,16,19,13,16,3,20,4,28,7,22,3,24,0,30,0,2,6,14,12,21,8,8,11,20,0,25,0,21,8,23,4,4,4,25,17,20,13,22,9,24,5,26,1,28,2,9,19,19,0,27,6,11,5,10,5,5,18,18,7,7,18,21,3,16,3,13,8,20,4,22,15,18,15,21,12,18,9,18,17,17,5,20,0,12,14,20,14,23,0,1,0,7,4,18,1,2,4,14,0,15,6,18,14,17,2,4,10,16,0,18,1,4,14,14,1,13,13,19,11,23,7,16,5,16,11,14,15,15,8,14,10,22,7,12,10,25,6,7,10,19,9,12,12,15,12,12,11,17,10,10,9,21,7,25,13,13,0,23,11,12,6,24,6,27,8,17,1,12,6,21,11,11,7,19,2,26,0,20,});
sparse_graph3_ints[10][70] = vector<int>({14,23,3,27,19,25,4,4,7,28,12,12,0,4,8,8,0,0,14,14,0,11,23,23,8,21,12,29,0,32,10,10,19,22,10,24,15,18,8,31,4,33,0,2,0,35,1,5,14,26,0,27,0,16,7,7,9,9,11,11,10,27,13,18,13,22,19,20,5,5,7,26,17,20,3,15,15,15,6,21,15,27,4,28,4,31,7,30,11,28,12,17,22,22,15,24,11,16,18,24,0,29,4,11,13,13,1,33,1,30,8,18,21,24,0,14,20,23,11,25,3,10,5,12,10,15,5,20,21,21,3,23,17,26,6,6,11,22,8,24,7,17,4,25,9,14,0,25,16,25,9,22,0,9,18,18,2,22,6,16,17,23,3,30,8,13,7,12,4,19,14,20,13,25,16,22,12,20,5,15,3,18,5,23,16,19,1,21,15,21,16,16,0,20,17,17,1,18,1,2,});
sparse_graph3_ints[10][80] = vector<int>({22,22,24,24,4,38,14,27,14,14,6,6,12,12,10,35,4,25,10,10,26,27,0,2,0,8,0,22,8,8,0,33,12,22,10,28,16,29,20,21,20,28,3,36,6,30,0,0,3,3,0,40,8,20,23,28,2,2,12,32,6,28,6,32,18,26,17,31,5,5,7,7,12,30,9,9,14,33,11,11,14,25,7,16,16,22,13,13,2,30,9,31,0,13,5,16,18,19,8,25,4,27,9,33,17,18,5,14,0,28,6,35,20,30,0,20,13,24,17,23,14,19,4,23,15,21,19,20,10,26,0,18,9,16,4,13,17,25,0,26,15,28,0,38,3,34,7,36,16,16,3,12,23,23,0,36,3,32,2,23,25,25,11,21,13,18,6,20,15,15,10,17,0,11,8,23,21,26,4,20,10,20,23,26,12,17,0,30,21,24,19,27,15,31,18,29,0,16,20,23,});
sparse_graph3_ints[10][100] = vector<int>({20,20,16,20,12,28,8,37,20,29,33,33,22,32,12,41,26,37,29,30,3,4,0,0,6,7,12,12,14,14,5,27,7,11,0,28,0,37,2,2,8,28,0,10,14,37,24,24,11,20,1,23,9,12,6,21,22,39,8,41,18,41,22,23,4,38,14,43,18,35,9,34,0,19,4,42,19,38,15,34,16,31,26,30,23,36,8,44,17,28,4,46,27,34,19,32,23,29,31,31,10,45,0,50,27,27,30,34,6,47,0,2,4,44,4,36,0,48,19,24,21,35,25,33,4,40,16,38,10,38,0,35,16,25,0,46,0,33,11,24,5,25,8,8,9,32,0,26,14,29,0,43,0,31,10,16,12,26,11,22,0,15,0,41,13,32,25,26,0,21,6,17,12,35,0,39,16,40,16,23,10,19,16,18,5,29,16,16,0,17,15,15,21,26,6,33,5,32,19,19,});
sparse_graph3_ints[7][6] = vector<int>({2,2,0,0,0,1,0,2,0,3,1,1,1,2,});
sparse_graph3_ints[7][8] = vector<int>({0,0,2,3,0,2,0,1,0,4,1,2,2,2,1,3,1,1,0,3,});
sparse_graph3_ints[7][10] = vector<int>({0,1,0,5,0,0,2,2,0,2,3,3,1,2,0,4,2,4,2,3,1,3,1,4,1,1,0,3,});
sparse_graph3_ints[7][12] = vector<int>({1,2,0,0,4,4,0,6,0,1,2,3,0,2,2,2,2,4,2,5,0,4,0,5,3,3,3,4,1,5,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[7][14] = vector<int>({0,0,1,2,0,6,4,5,0,1,2,3,0,2,2,2,3,4,0,7,3,3,2,6,4,4,3,5,0,5,0,4,2,5,2,4,1,5,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[7][16] = vector<int>({0,8,0,0,1,2,5,5,0,6,0,1,2,3,0,2,4,4,2,2,3,3,3,4,2,7,4,6,2,6,0,7,4,5,3,5,3,6,0,5,1,7,0,4,2,5,1,5,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[7][18] = vector<int>({0,9,3,3,6,6,0,0,0,2,4,5,0,7,2,2,0,1,1,2,2,3,4,4,0,6,2,8,3,4,5,5,4,7,0,8,4,6,2,6,2,7,5,6,3,7,3,6,3,5,1,7,1,8,0,5,0,4,2,5,1,5,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[7][20] = vector<int>({0,6,0,10,4,8,0,0,0,8,1,2,4,5,0,1,2,3,0,2,2,2,3,4,4,4,5,6,3,3,6,7,5,5,2,8,6,6,0,7,2,9,0,9,5,7,3,7,4,7,2,7,2,6,4,6,3,8,1,9,1,7,3,6,3,5,1,8,0,5,0,4,2,5,1,5,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[7][23] = vector<int>({2,3,3,4,2,9,0,1,0,8,0,3,5,9,4,5,0,0,1,11,1,1,5,6,2,2,3,3,4,4,6,7,5,5,7,8,6,6,2,8,7,7,0,6,0,9,3,10,0,7,3,9,0,10,6,8,4,8,5,8,0,11,5,7,4,9,2,10,4,7,3,7,2,7,1,9,2,6,3,8,4,6,1,10,1,7,3,6,3,5,1,8,0,5,0,4,2,5,1,5,2,4,1,6,1,3,1,4,1,2,0,2,});
sparse_graph3_ints[7][26] = vector<int>({4,5,0,8,0,13,0,0,8,9,2,10,6,7,3,4,2,3,1,2,0,1,5,6,0,2,2,2,2,8,3,3,4,4,4,10,7,8,5,5,6,6,7,7,6,10,0,9,2,9,8,8,4,11,2,12,7,9,0,12,2,11,0,6,5,9,0,7,6,9,4,9,4,8,0,10,6,8,0,11,5,10,3,11,3,9,5,8,3,10,5,7,1,11,1,12,4,7,3,7,2,7,1,9,3,8,2,6,1,10,4,6,1,7,3,6,1,8,3,5,0,5,0,4,2,5,1,5,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[7][30] = vector<int>({3,4,1,2,4,5,7,7,10,10,0,1,0,14,2,10,2,8,0,0,8,8,0,2,2,3,2,2,3,3,6,6,5,6,0,8,6,12,4,10,6,7,9,9,5,5,7,8,4,4,4,11,0,9,2,9,8,9,2,14,8,11,2,11,4,12,6,11,8,10,6,10,4,13,9,10,0,15,7,11,2,12,2,13,0,12,7,10,5,11,7,9,0,13,5,12,3,13,6,9,0,6,5,9,0,7,4,9,3,11,5,10,0,10,0,11,3,12,4,8,6,8,1,13,1,14,3,9,5,8,1,11,3,10,1,12,5,7,4,7,3,7,2,7,1,9,3,8,2,6,1,10,4,6,1,7,3,6,1,8,3,5,0,5,0,4,2,5,1,5,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[7][35] = vector<int>({11,12,7,14,9,9,2,10,7,7,6,6,0,3,0,0,2,3,0,8,5,5,4,13,0,16,0,10,3,4,4,11,8,8,1,17,1,1,0,1,10,11,1,3,3,3,5,6,6,7,5,14,9,13,0,9,7,8,9,10,4,10,8,9,2,11,7,13,0,11,0,14,10,10,5,15,11,11,4,12,3,15,4,5,5,13,7,12,4,4,10,12,8,12,7,11,3,16,9,12,0,15,9,11,8,13,2,15,2,14,6,14,6,11,0,17,6,12,8,11,6,13,6,10,4,14,8,10,4,15,2,16,2,12,2,13,0,12,0,13,5,11,3,13,7,10,5,12,1,15,3,14,7,9,1,16,6,9,0,6,5,9,0,7,4,9,3,11,5,10,1,13,2,8,3,12,2,9,1,14,6,8,4,8,3,9,1,11,5,8,3,10,1,12,5,7,4,7,3,7,});
sparse_graph3_ints[7][40] = vector<int>({8,14,2,17,0,20,10,10,6,13,4,18,8,16,8,8,6,8,5,5,6,6,3,4,0,2,0,0,1,4,0,16,0,9,1,10,4,12,12,13,11,12,0,1,2,2,1,2,10,14,2,4,6,15,0,8,6,7,8,9,8,15,7,8,9,10,0,10,6,12,3,11,5,6,6,16,10,15,0,14,9,9,0,18,6,17,10,11,4,16,12,14,5,12,4,11,12,12,11,11,6,14,3,9,1,14,3,10,8,13,0,12,2,19,8,12,4,17,0,13,13,13,2,18,2,16,0,17,11,13,11,14,9,13,9,15,10,13,0,19,9,14,4,14,7,15,10,12,4,15,4,5,7,16,4,4,5,17,2,15,7,13,9,12,7,14,5,15,5,16,9,11,3,17,3,18,2,14,1,19,8,11,7,11,5,13,7,12,3,15,5,14,3,16,1,17,6,10,1,18,});
sparse_graph3_ints[7][45] = vector<int>({11,15,7,17,3,21,11,11,9,10,7,8,1,3,0,0,2,7,0,11,1,1,0,7,6,6,8,9,3,11,0,22,3,16,4,19,11,17,15,15,4,13,12,13,7,19,13,14,3,3,7,15,13,16,0,1,5,5,0,19,9,17,0,3,2,3,5,11,9,14,3,5,6,13,7,14,5,6,7,18,7,7,6,7,9,15,9,18,5,20,9,9,4,18,10,11,1,16,0,10,8,8,11,16,6,12,3,12,10,10,11,12,0,21,13,15,3,15,2,14,2,20,13,13,12,12,9,13,6,14,0,15,0,14,6,17,0,20,2,19,2,18,14,14,0,18,14,15,4,17,5,19,3,9,12,16,4,16,3,10,0,12,12,15,0,13,9,16,10,16,12,14,10,17,8,12,10,14,11,14,8,18,2,21,8,16,10,15,8,17,6,18,11,13,6,19,4,20,6,16,});
sparse_graph3_ints[7][50] = vector<int>({16,17,0,16,14,14,7,14,12,19,0,21,10,10,0,10,0,7,10,15,6,6,8,19,0,2,5,10,0,0,4,23,6,12,0,25,4,21,0,23,8,21,0,19,2,2,12,17,12,12,9,14,11,11,8,9,14,16,7,7,6,17,14,18,2,7,2,12,3,3,12,14,10,18,10,20,4,17,0,1,1,2,4,4,1,4,8,16,1,14,6,20,5,6,6,22,8,18,8,20,4,22,6,7,5,5,8,8,6,21,10,17,3,15,7,13,9,10,10,19,10,16,4,13,2,22,2,10,12,16,4,20,4,16,4,12,2,24,6,19,9,9,10,11,1,12,0,11,12,15,12,18,6,18,8,17,12,13,2,23,6,13,8,14,0,22,10,14,14,17,14,15,0,24,3,16,0,20,16,16,1,16,2,20,2,21,0,18,4,18,11,12,4,19,6,16,15,16,15,17,});
sparse_graph3_ints[7][55] = vector<int>({14,15,13,21,5,25,5,17,9,21,11,14,1,23,5,11,1,3,0,13,0,0,0,8,17,17,1,27,6,19,1,1,0,21,5,7,8,11,0,17,10,17,3,24,10,13,11,18,13,19,15,16,10,22,0,11,0,26,18,18,7,20,15,18,15,20,7,8,7,17,13,17,3,4,11,20,4,15,1,19,2,8,4,5,6,15,12,16,2,13,13,13,9,19,7,23,0,1,11,11,5,13,12,12,0,3,3,3,2,3,1,15,5,23,8,18,4,20,7,22,4,19,7,24,3,23,3,26,7,16,5,24,7,21,9,12,7,7,9,9,6,7,8,14,3,25,10,10,3,22,9,20,8,9,0,25,7,13,3,11,4,16,5,21,11,19,5,22,1,22,3,18,10,16,5,6,4,13,12,15,13,18,2,20,13,20,0,12,2,16,15,17,15,19,14,14,1,18,15,15,});
sparse_graph3_ints[7][60] = vector<int>({8,18,4,28,4,20,7,21,8,25,3,3,0,9,0,2,20,20,0,19,5,11,17,18,16,22,12,19,12,22,0,0,12,13,0,30,9,12,0,25,1,27,0,22,14,15,3,11,1,11,11,12,13,14,2,2,7,24,15,16,16,17,13,23,18,19,2,16,4,24,11,18,13,20,8,22,16,20,18,21,4,4,14,21,15,19,5,16,0,16,14,18,11,21,3,8,5,8,7,8,5,18,1,20,1,23,10,24,12,16,13,17,9,16,9,23,5,5,10,17,6,26,0,7,10,14,1,14,6,14,11,15,6,23,3,14,0,1,7,12,1,2,3,26,5,25,10,20,1,4,4,23,6,19,10,10,8,15,2,29,4,27,8,10,6,27,1,26,3,22,2,28,5,21,7,17,6,6,7,20,8,14,0,13,8,26,2,24,8,9,0,29,9,19,5,17,2,21,10,25,});
sparse_graph3_ints[7][70] = vector<int>({15,18,3,27,8,24,4,31,9,9,11,12,0,0,2,2,0,16,13,14,2,19,6,19,6,6,10,30,4,4,22,22,20,20,18,18,18,26,16,24,14,28,14,22,0,25,0,35,0,2,12,26,9,26,2,4,5,5,8,8,7,7,10,21,9,28,7,31,12,13,6,28,15,16,14,15,12,18,2,22,4,33,9,20,3,30,7,23,0,11,11,17,0,32,0,19,4,19,19,19,8,16,1,33,6,22,8,19,10,16,0,22,21,21,6,16,3,24,0,29,19,24,23,23,10,24,4,16,5,12,8,13,17,22,4,25,2,16,21,24,11,22,15,20,7,26,19,22,6,13,4,11,17,25,17,20,3,10,10,10,0,9,4,28,15,26,15,23,0,14,1,27,16,27,4,22,13,24,13,27,14,25,17,17,1,30,11,28,13,19,12,29,12,23,13,21,11,25,7,29,});
sparse_graph3_ints[7][80] = vector<int>({16,27,14,14,10,29,7,19,0,40,20,20,0,36,6,6,0,27,18,18,8,8,10,35,23,26,0,8,0,2,10,10,12,12,16,16,9,21,13,31,13,22,21,22,6,17,24,28,0,0,0,22,3,3,0,15,2,2,0,25,20,28,12,26,5,5,17,30,7,7,18,24,9,9,14,33,5,24,10,12,5,16,3,21,13,29,0,13,15,19,10,33,0,20,10,31,7,35,5,14,7,33,7,31,10,24,12,21,11,25,20,26,9,28,6,30,11,18,4,36,0,18,3,35,13,27,8,27,19,19,13,18,15,15,17,23,9,23,7,26,5,29,4,13,3,38,17,28,22,23,6,37,3,32,0,38,4,33,4,28,19,25,6,25,0,34,15,23,23,24,0,11,9,16,2,31,18,31,8,20,11,16,21,29,3,27,1,30,13,13,5,22,25,26,0,29,26,27,16,21,});
sparse_graph3_ints[7][100] = vector<int>({7,21,11,33,12,13,4,40,15,18,8,8,25,26,0,22,4,48,24,36,0,8,31,32,18,27,15,39,17,33,0,2,4,36,6,42,11,44,12,27,27,28,2,11,0,47,22,22,8,28,20,38,7,14,20,21,17,21,23,24,0,27,13,35,23,32,11,41,19,36,14,29,9,25,18,30,0,41,5,6,28,32,7,44,10,22,0,45,9,10,32,34,9,38,0,0,1,17,18,18,4,29,0,38,0,50,29,29,18,41,22,39,2,4,5,26,2,2,16,36,29,35,24,29,22,27,25,33,11,16,21,30,0,25,0,30,0,20,26,37,13,24,8,30,20,33,15,31,0,43,14,21,10,20,11,30,9,36,11,39,22,35,0,15,11,12,4,33,8,34,6,20,5,19,15,42,0,36,16,26,4,38,4,31,4,25,16,24,15,16,14,15,0,34,0,32,8,46,4,46,});
sparse_graph3_ints[3][6] = vector<int>({2,2,0,0,0,1,0,2,0,3,1,1,1,2,});
sparse_graph3_ints[3][8] = vector<int>({0,4,0,0,0,1,1,2,0,2,2,3,2,2,1,3,1,1,0,3,});
sparse_graph3_ints[3][10] = vector<int>({1,2,0,0,3,3,0,1,0,5,0,2,2,2,2,4,0,4,2,3,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[3][12] = vector<int>({0,1,2,3,4,4,0,6,0,0,1,2,0,2,2,2,3,3,3,4,2,4,2,5,0,5,0,4,1,5,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[3][14] = vector<int>({4,5,0,0,2,3,0,6,0,1,1,2,0,2,2,2,3,4,3,3,4,4,0,7,2,6,3,5,1,5,2,5,2,4,0,5,0,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[3][16] = vector<int>({4,4,0,8,0,0,1,2,0,1,4,6,0,6,2,3,0,2,2,2,3,3,3,4,4,5,5,5,2,6,2,7,0,7,3,5,3,6,1,7,2,5,1,5,2,4,1,6,0,5,0,4,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[3][18] = vector<int>({1,2,2,3,0,9,0,0,6,6,0,6,0,1,3,4,0,2,2,2,4,5,4,4,3,3,0,7,5,5,2,8,5,6,0,8,4,6,4,7,3,7,2,6,2,7,3,6,1,7,3,5,1,8,2,5,1,5,2,4,1,6,0,5,0,4,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[3][20] = vector<int>({4,8,0,2,2,2,4,4,0,0,0,10,0,8,0,6,0,1,2,3,1,2,5,6,3,4,4,5,6,7,5,5,3,3,6,6,2,8,0,7,2,9,5,7,0,9,3,7,4,7,4,6,3,8,2,7,1,9,2,6,1,7,3,6,1,8,3,5,2,5,1,5,2,4,1,6,0,5,0,4,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[3][23] = vector<int>({7,8,3,10,2,2,0,3,0,1,5,5,0,9,4,4,6,7,0,0,2,9,3,3,3,4,1,1,4,5,5,6,2,3,5,9,6,6,1,11,7,7,4,8,0,8,0,10,0,6,0,7,6,8,3,9,2,8,5,8,5,7,0,11,4,9,2,10,4,7,3,7,1,9,3,8,4,6,1,10,2,7,2,6,1,7,3,6,1,8,3,5,2,5,1,5,2,4,1,6,0,5,0,4,1,3,1,4,1,2,0,2,});
sparse_graph3_ints[3][26] = vector<int>({2,11,0,2,3,3,0,8,6,7,8,9,0,0,7,8,5,6,4,4,0,1,0,11,0,5,4,5,4,11,3,4,4,10,6,6,5,5,2,2,1,2,2,10,7,7,8,8,0,13,0,10,2,12,0,9,4,9,0,12,6,10,7,9,5,9,6,9,4,8,6,8,5,10,0,6,0,7,3,11,2,8,2,9,3,9,5,8,3,10,1,11,5,7,1,12,4,7,3,7,1,9,3,8,1,10,4,6,2,7,2,6,1,7,3,6,1,8,3,5,2,5,1,5,2,4,1,6,2,3,0,4,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[3][30] = vector<int>({4,13,2,2,3,3,4,4,0,8,0,13,6,7,8,9,0,1,8,11,10,10,7,8,0,0,0,15,4,11,5,6,6,10,6,12,4,12,0,11,4,5,6,11,6,6,5,5,2,12,2,14,8,10,4,10,3,4,8,8,7,7,0,14,2,13,0,10,0,9,0,12,0,5,0,3,0,2,9,9,9,10,7,11,2,10,2,11,7,10,5,11,7,9,5,12,4,9,3,13,5,9,6,9,4,8,3,11,5,10,3,12,1,13,6,8,0,6,1,14,0,7,2,8,2,9,3,9,1,11,5,8,3,10,1,12,5,7,4,7,3,7,1,9,3,8,1,10,4,6,2,7,2,6,1,7,3,6,1,8,3,5,2,5,1,5,2,4,1,6,2,3,0,4,1,3,1,4,1,2,1,1,});
sparse_graph3_ints[3][35] = vector<int>({0,13,6,7,4,10,8,9,6,10,3,15,6,13,9,10,5,15,7,14,0,11,10,11,3,4,5,5,11,12,1,3,0,0,1,1,9,13,8,12,0,1,7,11,0,3,1,17,3,3,3,16,2,3,7,8,0,16,4,13,4,5,4,4,0,15,5,6,2,14,7,7,4,12,6,6,0,9,0,14,0,8,9,9,8,8,6,11,0,10,0,12,11,11,10,10,5,13,2,13,2,12,4,11,10,12,7,13,2,15,9,12,9,11,8,13,6,14,8,11,0,17,5,14,6,12,7,12,4,14,8,10,4,15,2,16,2,10,2,11,5,11,3,13,7,10,5,12,1,15,3,14,7,9,1,16,4,9,4,8,5,9,6,9,3,11,5,10,1,13,3,12,1,14,6,8,0,6,0,7,2,8,2,9,3,9,1,11,5,8,3,10,1,12,5,7,4,7,3,7,});
sparse_graph3_ints[3][40] = vector<int>({0,19,0,18,2,16,5,14,7,11,2,19,0,20,7,14,8,12,10,11,6,16,11,12,9,13,12,13,4,5,7,8,2,2,0,0,0,2,5,6,1,6,0,13,3,12,6,7,10,14,8,9,9,10,4,10,4,18,2,4,0,1,2,18,1,2,0,15,0,17,1,4,5,11,4,4,5,15,2,17,6,17,4,6,0,16,6,6,1,10,0,14,8,8,7,7,0,10,0,12,10,10,9,9,7,12,3,10,12,14,2,14,2,15,12,12,4,13,11,11,3,9,6,11,9,14,0,8,4,14,0,9,13,13,9,15,11,14,5,16,11,13,8,16,10,15,8,14,8,15,10,13,7,15,10,12,2,12,7,16,5,17,5,10,9,12,7,13,4,16,4,17,6,14,8,13,6,15,3,17,9,11,3,18,1,19,6,12,4,15,5,13,3,15,1,17,6,13,1,18,});
sparse_graph3_ints[3][45] = vector<int>({10,14,8,18,3,19,4,4,7,16,9,10,6,9,7,13,6,11,3,15,0,5,1,1,5,7,0,0,3,21,0,19,5,5,0,15,10,11,0,22,4,16,0,17,11,12,3,11,9,13,0,13,6,19,12,13,13,14,14,15,11,15,10,17,5,14,0,21,5,18,7,7,1,6,3,3,0,10,0,1,8,8,3,14,3,20,1,3,0,3,1,11,3,13,0,20,5,17,0,18,4,5,2,18,5,6,7,15,6,7,0,16,8,17,3,12,9,9,0,14,9,11,0,11,2,17,5,13,11,11,7,12,8,13,7,11,13,13,12,12,13,15,10,15,9,14,6,15,15,15,12,16,7,17,3,10,4,17,14,14,5,11,11,17,13,16,8,9,12,15,10,16,0,8,5,19,2,19,0,9,12,14,9,17,11,16,7,19,9,18,9,15,8,16,11,14,11,13,7,18,});
sparse_graph3_ints[3][50] = vector<int>({16,17,0,19,4,17,6,12,11,15,8,20,12,12,0,7,0,0,2,2,7,8,0,2,0,10,4,20,14,16,5,8,8,14,10,11,8,18,6,14,12,16,9,10,14,18,12,18,10,20,9,16,10,13,2,7,10,18,7,17,6,22,0,13,4,4,5,18,6,20,4,22,3,15,6,10,2,4,3,19,3,12,1,11,2,21,2,23,0,24,0,22,0,21,13,14,2,24,0,1,2,22,0,20,1,2,0,25,3,14,6,6,4,23,4,21,1,4,0,18,0,17,6,19,0,23,3,13,3,11,6,21,5,6,6,11,0,16,5,5,7,16,8,21,8,19,7,7,10,17,9,15,10,19,10,12,2,18,9,9,10,10,6,15,4,16,8,13,11,14,12,19,12,17,0,11,8,12,0,14,0,15,14,15,5,14,14,17,13,13,14,14,6,16,16,16,2,17,7,12,});
sparse_graph3_ints[3][55] = vector<int>({5,5,9,10,0,19,0,16,6,9,14,15,8,22,3,3,0,27,1,1,17,17,13,20,0,0,11,13,4,4,5,8,11,17,9,12,6,20,0,21,7,10,6,14,9,17,9,20,14,18,18,18,15,16,16,19,12,15,13,17,4,20,10,15,13,13,11,21,9,23,1,14,9,14,8,16,7,21,7,15,3,14,11,19,0,7,6,12,3,17,4,12,7,18,6,17,7,24,5,25,4,18,5,16,5,23,3,25,2,7,2,21,0,26,0,13,0,5,0,1,8,8,0,22,3,24,4,22,3,16,3,26,1,3,0,3,7,7,3,15,0,20,3,11,4,21,5,24,5,7,0,18,5,6,2,22,2,23,0,24,0,25,6,13,9,19,9,18,9,11,9,9,0,17,9,13,2,20,0,23,4,19,11,22,11,20,11,18,0,12,11,12,0,11,0,10,11,11,13,21,});
sparse_graph3_ints[3][60] = vector<int>({0,23,0,29,10,11,0,18,6,18,6,24,10,22,12,24,14,20,12,13,16,16,12,16,16,22,6,6,8,8,4,4,0,0,2,2,3,18,18,18,0,2,8,26,5,5,13,18,9,9,10,20,0,26,11,12,10,16,15,15,17,17,7,22,19,19,1,21,8,20,20,20,0,20,6,11,17,20,9,18,11,18,18,21,0,9,15,21,15,18,16,19,5,10,3,13,2,9,13,14,1,16,7,7,13,22,14,23,10,14,11,23,12,21,9,24,10,25,4,9,9,13,0,12,8,12,0,15,7,19,8,17,7,25,8,23,8,15,6,16,2,12,0,7,6,21,5,20,6,13,6,27,5,23,5,26,4,22,4,28,2,4,4,19,4,25,3,21,4,14,3,24,3,27,2,26,0,25,0,24,5,15,7,14,2,29,0,30,2,17,0,1,0,27,1,2,0,28,1,4,});
sparse_graph3_ints[3][70] = vector<int>({14,26,18,24,5,13,23,23,14,17,9,12,12,12,6,6,17,18,6,30,4,4,0,0,2,2,0,22,14,14,4,11,8,24,4,29,9,19,19,20,12,23,0,2,0,32,3,3,10,30,5,5,16,22,14,20,13,13,8,9,11,16,3,32,9,25,18,26,8,18,12,17,16,17,5,26,0,34,13,24,10,15,0,11,15,21,2,28,9,14,18,19,12,20,10,20,7,17,6,21,20,21,1,30,5,20,6,16,3,23,7,28,7,7,11,28,15,27,21,22,2,22,6,14,4,19,6,24,0,14,4,24,3,18,15,24,0,25,7,22,11,22,21,24,3,10,9,22,3,16,0,20,12,26,18,22,3,26,1,26,0,18,0,9,0,28,16,25,15,15,1,16,13,28,11,25,2,14,10,27,9,29,10,10,11,11,8,31,7,26,6,32,8,11,0,7,4,33,2,34,});
sparse_graph3_ints[3][80] = vector<int>({10,32,14,14,8,28,12,12,6,10,0,0,18,31,0,10,2,2,8,36,10,11,4,20,18,18,17,24,3,32,6,24,22,22,20,20,4,4,4,36,3,24,23,26,24,28,0,40,0,38,0,2,7,7,0,6,6,33,4,38,20,28,6,6,17,29,0,32,14,30,5,15,15,32,10,17,10,25,9,10,12,22,12,34,4,14,0,13,14,26,9,16,0,15,0,19,14,24,13,13,8,15,14,19,5,21,4,18,16,16,11,28,15,15,3,34,8,30,15,20,17,27,17,17,11,26,16,21,13,18,11,21,0,23,13,23,17,22,20,26,10,20,8,20,9,24,7,27,7,34,19,19,12,29,12,17,0,21,6,26,9,31,20,22,0,36,11,33,4,29,0,34,23,23,3,28,3,26,0,17,0,30,21,29,0,28,0,26,1,24,18,25,15,27,25,26,26,27,24,24,});
sparse_graph3_ints[3][100] = vector<int>({22,23,15,34,14,21,4,42,7,19,8,15,10,10,8,8,0,30,5,33,4,5,0,0,27,34,26,26,28,29,4,46,17,36,0,6,6,7,16,29,11,19,21,31,12,30,0,19,8,26,18,24,23,36,20,20,0,22,19,38,7,38,24,24,14,26,15,40,8,35,16,42,6,30,13,38,12,44,20,40,11,42,0,36,8,46,24,38,20,27,30,30,0,11,28,36,22,28,15,23,11,37,31,32,32,34,0,34,0,28,0,50,2,2,0,44,0,48,12,34,0,2,7,11,4,48,4,44,0,38,18,19,21,35,25,33,6,23,11,14,5,16,11,22,9,40,4,38,17,17,23,32,0,46,10,29,8,33,4,36,4,20,25,29,11,26,0,15,2,41,0,17,17,31,3,24,12,32,5,27,14,15,0,25,10,18,13,14,18,33,15,17,0,32,8,43,0,40,19,25,});
sparse_graph3_ints[0][6] = vector<int>({0,1,1,1,0,0,0,2,2,2,1,2,0,3,});
sparse_graph3_ints[0][8] = vector<int>({0,2,0,3,1,2,0,0,2,2,2,3,1,3,0,4,0,1,1,1,});
sparse_graph3_ints[0][10] = vector<int>({0,1,1,1,0,2,1,4,1,2,0,3,0,5,0,0,2,3,3,3,2,2,2,4,1,3,0,4,});
sparse_graph3_ints[0][12] = vector<int>({0,6,2,3,0,0,2,4,1,3,0,5,0,3,1,5,0,2,3,4,2,5,3,3,0,4,1,1,1,4,2,2,1,2,4,4,0,1,});
sparse_graph3_ints[0][14] = vector<int>({2,5,1,1,1,5,0,7,0,5,3,3,0,3,0,4,3,5,3,4,1,6,2,3,2,2,0,6,2,4,1,3,0,2,1,2,1,4,4,5,2,6,0,1,4,4,0,0,});
sparse_graph3_ints[0][16] = vector<int>({1,4,2,7,2,6,2,5,0,0,1,6,0,1,1,5,3,4,0,8,3,5,3,6,2,3,3,3,0,6,0,5,5,5,2,4,1,7,0,7,0,3,4,4,1,1,4,6,1,3,0,2,0,4,4,5,1,2,2,2,});
sparse_graph3_ints[0][18] = vector<int>({3,7,0,6,0,2,5,5,2,2,2,7,2,8,4,7,4,6,3,4,5,6,1,4,2,6,1,1,0,7,1,6,1,3,0,3,0,0,1,5,0,9,1,8,3,5,0,8,3,6,1,2,6,6,3,3,4,4,4,5,0,5,1,7,2,5,0,1,2,4,2,3,0,4,});
sparse_graph3_ints[0][20] = vector<int>({2,5,1,4,0,3,0,4,2,9,5,6,4,7,3,4,3,7,0,7,1,6,4,5,2,8,6,6,0,9,1,3,0,2,2,6,0,0,1,5,4,6,5,5,4,4,0,8,3,3,2,4,1,9,2,2,1,2,2,7,3,5,2,3,3,8,6,7,1,1,0,10,0,5,0,6,5,7,1,8,4,8,1,7,3,6,0,1,});
sparse_graph3_ints[0][23] = vector<int>({0,1,3,5,0,5,1,3,2,9,4,6,1,10,4,9,3,3,1,11,0,7,1,6,0,6,5,5,4,4,2,8,7,7,3,4,3,9,0,8,4,8,1,8,2,7,1,9,5,9,0,10,6,8,0,11,0,2,5,7,0,3,5,6,3,7,3,8,1,4,6,7,2,2,2,10,7,8,5,8,4,7,1,2,2,3,6,6,0,4,3,6,3,10,0,0,2,4,2,6,1,7,1,5,0,9,4,5,2,5,1,1,});
sparse_graph3_ints[0][26] = vector<int>({2,12,3,4,4,10,1,1,4,11,4,7,5,9,2,9,3,8,3,9,0,4,3,6,4,4,3,11,0,11,5,7,2,8,4,6,8,8,6,6,2,10,0,5,0,7,6,7,0,6,0,10,0,1,7,9,1,12,1,11,2,4,1,3,3,3,5,8,5,6,5,10,1,9,1,7,4,5,7,7,0,0,1,4,4,9,6,9,2,3,0,12,3,7,2,2,3,10,7,8,4,8,6,8,1,5,1,10,0,13,1,8,8,9,2,7,6,10,0,8,0,9,0,3,5,5,2,6,2,5,1,2,2,11,3,5,1,6,0,2,});
sparse_graph3_ints[0][30] = vector<int>({2,3,0,7,0,11,2,12,8,9,5,6,1,12,7,11,3,8,5,7,4,12,0,14,3,9,2,13,5,12,6,9,1,6,2,8,2,5,0,12,6,11,10,10,3,13,2,6,2,7,4,5,8,10,5,10,7,7,3,7,6,10,3,3,2,2,1,13,1,3,3,12,9,10,1,9,4,4,1,2,0,0,4,7,7,10,0,10,6,6,4,9,2,9,0,3,5,8,4,13,6,12,0,2,5,11,4,10,3,4,8,8,0,13,1,10,0,4,4,11,7,9,1,4,6,8,0,15,0,9,2,14,4,6,1,7,0,1,6,7,3,10,4,8,1,1,7,8,3,11,8,11,5,5,1,8,3,5,0,6,1,14,2,10,0,8,3,6,1,5,0,5,1,11,9,9,5,9,2,11,2,4,});
sparse_graph3_ints[0][35] = vector<int>({4,11,2,6,3,4,9,13,1,16,6,9,11,11,0,8,0,9,2,14,0,1,4,15,5,8,6,6,6,12,3,6,6,7,2,8,3,16,9,10,0,3,3,15,3,3,5,12,8,11,3,7,5,5,0,17,0,6,1,14,1,10,0,10,0,12,7,8,7,14,3,12,0,13,4,5,5,7,2,5,4,8,0,11,2,4,7,10,1,15,0,15,4,10,5,11,4,12,2,10,5,10,0,0,4,4,4,13,8,13,3,9,4,14,10,12,7,11,2,11,3,11,4,9,0,2,1,13,1,3,10,11,0,5,9,9,0,16,2,13,5,9,1,12,5,15,1,5,9,12,2,3,2,12,3,8,6,13,7,9,7,12,2,7,2,2,6,8,1,8,4,7,5,13,8,8,3,10,1,11,0,4,7,13,4,6,7,7,1,17,10,10,1,6,2,15,1,7,6,10,});
sparse_graph3_ints[0][40] = vector<int>({2,5,3,13,11,12,8,14,13,13,0,7,6,9,3,4,2,16,0,15,6,16,0,20,2,2,0,16,7,15,1,7,4,15,1,6,6,15,5,9,9,9,8,16,1,19,7,9,0,13,3,6,5,16,4,5,0,3,2,18,2,4,8,10,3,16,6,13,10,11,0,6,12,12,8,9,0,18,0,1,3,12,6,10,1,8,2,15,10,12,1,1,6,7,4,18,10,15,0,10,1,10,9,14,2,6,4,11,4,13,0,0,2,3,7,10,3,15,2,9,0,14,9,10,0,2,4,12,2,14,11,14,11,13,0,5,3,3,3,17,3,7,2,7,5,7,7,11,0,4,6,6,2,13,2,19,1,4,8,8,5,12,1,12,3,9,2,17,2,10,9,15,6,17,8,15,1,17,4,9,1,11,3,11,3,14,0,11,4,10,4,8,7,14,6,14,7,16,1,2,});
sparse_graph3_ints[0][45] = vector<int>({4,16,6,6,6,18,8,11,1,1,9,17,0,1,3,19,3,15,8,14,0,19,1,13,1,11,12,13,4,19,1,3,2,18,9,16,9,13,4,18,3,16,13,16,3,3,4,14,6,19,2,20,0,6,12,14,2,3,1,9,7,15,1,8,10,12,4,11,1,20,6,7,3,14,4,17,2,8,2,4,1,22,10,13,0,16,7,19,8,15,4,13,1,19,2,19,4,4,2,9,1,12,0,7,8,13,12,15,0,21,8,9,8,17,7,12,2,5,4,15,13,13,0,14,1,4,10,10,6,16,11,16,6,14,0,10,9,14,5,14,11,15,11,11,5,20,7,18,13,14,11,13,5,13,2,2,3,6,12,12,1,16,1,17,14,14,0,20,5,6,7,10,6,8,0,12,5,11,4,9,6,15,3,4,1,18,3,10,5,17,10,14,5,8,4,5,1,7,0,3,});
sparse_graph3_ints[0][50] = vector<int>({15,16,6,20,6,13,8,14,2,19,8,17,7,17,3,11,3,20,1,14,4,7,4,4,2,20,5,21,4,17,1,19,12,18,2,18,0,21,4,22,8,8,5,7,4,20,4,11,3,21,7,20,2,9,4,14,13,15,11,18,1,18,11,11,0,20,12,13,1,22,6,18,2,12,0,13,5,10,5,12,16,16,8,18,13,18,2,2,10,19,12,19,0,9,8,13,16,17,0,8,0,1,9,11,1,9,10,15,9,16,1,7,5,22,10,14,10,12,0,17,9,12,2,3,0,18,6,15,11,17,1,10,9,17,3,6,8,10,12,14,2,5,12,12,7,19,3,23,6,21,7,12,4,21,0,16,2,6,1,24,1,11,8,19,7,14,5,18,0,24,6,6,8,16,6,12,11,15,1,23,2,8,1,12,0,14,0,5,7,18,0,0,3,3,10,17,0,10,4,23,});
sparse_graph3_ints[0][55] = vector<int>({8,20,16,19,10,20,1,6,5,10,17,17,11,14,13,16,7,24,6,6,3,21,4,24,5,22,4,14,4,13,7,9,10,22,2,13,7,11,6,18,0,25,11,19,9,10,11,18,0,23,8,15,5,8,8,23,2,8,11,12,18,18,5,5,0,7,6,8,13,20,11,22,4,4,11,17,12,17,4,21,2,7,9,11,5,16,1,17,2,2,0,20,4,22,16,16,10,15,13,13,2,19,2,12,1,14,6,21,6,16,4,19,4,8,0,11,13,15,9,15,3,5,16,17,7,18,12,12,8,11,5,21,9,18,0,3,1,15,8,13,2,22,3,22,10,17,12,21,0,22,7,20,12,16,5,9,0,1,4,25,9,23,2,10,8,17,5,6,6,15,6,17,7,12,5,24,3,14,2,17,2,20,3,4,3,26,3,11,9,22,9,13,8,14,1,4,2,15,17,19,});
sparse_graph3_ints[0][60] = vector<int>({6,25,12,13,2,26,6,10,20,20,5,10,5,6,0,1,4,22,14,22,2,10,6,21,16,22,6,9,15,19,1,22,14,19,5,25,7,22,0,15,5,22,2,28,11,15,1,23,4,15,10,19,8,16,2,23,9,12,4,13,1,20,3,18,19,20,2,16,0,22,1,26,13,17,2,15,4,11,2,2,3,27,3,9,17,18,7,25,18,21,1,4,7,26,8,23,0,2,4,18,0,14,3,16,11,20,18,18,5,8,0,29,1,7,18,20,3,23,6,24,0,3,4,4,2,29,10,24,0,9,1,27,0,0,15,17,5,19,4,24,3,28,12,21,0,4,8,14,11,23,1,24,4,28,8,17,3,4,8,11,13,19,9,18,1,2,13,15,15,20,0,18,6,19,10,13,9,22,3,10,1,25,7,21,0,20,5,7,13,23,1,14,12,18,11,17,5,17,8,8,});
sparse_graph3_ints[0][70] = vector<int>({2,28,11,18,8,20,1,28,14,22,17,19,14,25,20,23,0,26,1,25,14,24,3,15,3,31,6,31,1,17,19,23,11,28,11,12,11,23,1,14,3,25,10,15,0,10,6,15,4,28,6,6,6,7,4,8,6,8,11,16,9,17,13,26,16,18,3,18,1,22,14,15,10,25,6,21,7,18,5,32,3,32,1,26,9,13,3,20,4,18,2,25,19,20,9,24,3,4,14,23,13,16,5,27,0,17,12,13,6,11,5,16,4,12,14,21,0,33,20,20,13,23,10,23,7,11,1,9,3,33,5,6,8,9,6,32,1,1,3,27,4,7,1,32,20,24,10,16,10,18,5,8,12,21,10,14,8,18,3,10,4,16,18,26,2,22,5,28,2,24,7,9,0,15,14,16,2,23,9,25,2,17,7,10,8,25,3,7,1,10,6,19,1,27,17,17,0,16,5,11,});
sparse_graph3_ints[0][80] = vector<int>({4,27,4,4,14,29,10,11,4,11,0,14,18,21,8,19,4,20,4,23,15,28,25,26,0,18,0,40,15,21,7,34,0,10,14,30,8,36,2,35,2,5,6,19,1,15,0,22,4,28,5,36,12,13,3,33,7,29,22,25,1,37,15,27,5,27,12,32,0,0,1,9,6,14,3,17,3,26,3,3,0,39,10,14,1,2,18,28,13,15,2,14,6,10,3,19,12,12,1,5,1,14,4,21,0,7,2,39,17,24,11,32,16,29,1,28,3,29,20,26,12,28,6,35,4,29,6,31,10,34,11,17,8,28,0,23,23,28,21,21,0,24,3,18,8,11,0,5,7,11,8,8,14,14,3,25,4,15,13,16,8,17,12,15,8,21,9,35,18,23,2,33,9,32,15,18,8,15,21,28,7,17,1,7,12,34,5,20,10,25,9,31,13,21,11,26,12,26,4,8,});
sparse_graph3_ints[0][100] = vector<int>({4,27,4,4,14,29,10,11,4,11,0,14,18,21,8,19,4,20,4,23,15,28,25,26,0,18,0,40,15,21,7,34,0,10,14,30,8,36,2,35,2,5,6,19,1,15,0,22,4,28,5,36,12,13,3,33,7,29,22,25,1,37,15,27,5,27,12,32,0,0,1,9,6,14,3,17,3,26,3,3,0,39,10,14,1,2,18,28,13,15,2,14,6,10,3,19,12,12,1,5,1,14,4,21,0,7,2,39,17,24,11,32,16,29,1,28,3,29,20,26,12,28,6,35,4,29,6,31,10,34,11,17,8,28,0,23,23,28,21,21,0,24,3,18,8,11,0,5,7,11,8,8,14,14,3,25,4,15,13,16,8,17,12,15,8,21,9,35,18,23,2,33,9,32,15,18,8,15,21,28,7,17,1,7,12,34,5,20,10,25,9,31,13,21,11,26,12,26,4,8,});


for(int error_rate = 0; error_rate <= 40; ++error_rate){
if(sparse_graph3_ints.count(error_rate) == 0) continue;
for(int num_nodes = 4; num_nodes <= 100; ++num_nodes){
if(sparse_graph3_ints[error_rate].count(num_nodes) == 0) continue;

vector<CompleteGraphSolver::CompleteGraph> v;
// v.push_back(CompleteGraphSolver::CompleteGraph(vector<int>()));
for(int j = 0; j < sz(sparse_graph3_ints[error_rate][num_nodes]); j += 2){
int i0 = sparse_graph3_ints[error_rate][num_nodes][j];
int i1 = sparse_graph3_ints[error_rate][num_nodes][j + 1];
v.push_back(CompleteGraphSolver::CompleteGraph(vector<int>({i0,i1,num_nodes - i0 - i1})));
}
graphs_3_non_flip[error_rate][num_nodes] = v;
}
my_assert(20 == sz(graphs_3_non_flip[error_rate]));
}

// unordered_map<int,unordered_map<int,vector<int>>> sparse_graph3_ints;
sparse_graph3_ints[40][6] = vector<int>({0,0,2,2,0,3,1,2,1,1,0,2,0,1,});
sparse_graph3_ints[40][8] = vector<int>({0,0,2,3,1,3,0,4,2,2,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[40][10] = vector<int>({0,0,3,3,2,4,0,5,1,4,2,3,0,4,1,3,2,2,0,1,0,3,1,2,0,2,1,1,});
sparse_graph3_ints[40][12] = vector<int>({4,4,0,0,0,6,3,4,2,5,1,5,3,3,2,4,0,5,0,1,1,4,2,3,0,2,0,4,1,3,2,2,1,1,0,3,1,2,});
sparse_graph3_ints[40][14] = vector<int>({4,5,0,0,0,7,4,4,0,2,3,5,0,6,2,6,1,6,2,5,3,4,0,1,1,5,3,3,2,4,0,5,1,1,1,4,0,4,2,3,0,3,1,2,2,2,1,3,});
sparse_graph3_ints[40][16] = vector<int>({5,5,0,0,0,3,0,8,4,6,4,5,3,6,0,2,2,7,1,7,0,7,3,5,4,4,2,6,0,4,0,6,0,1,1,6,2,5,3,4,2,2,1,1,1,2,1,5,3,3,2,4,0,5,1,4,1,3,2,3,});
sparse_graph3_ints[40][18] = vector<int>({6,6,0,0,0,4,0,8,5,6,4,7,2,2,0,9,5,5,4,6,3,7,0,3,1,8,2,8,4,5,2,7,3,6,0,2,1,7,1,3,0,7,0,6,0,1,3,5,1,2,2,6,4,4,0,5,1,6,1,1,1,4,2,5,3,4,2,3,2,4,1,5,3,3,});
sparse_graph3_ints[40][20] = vector<int>({6,7,0,0,0,4,0,10,6,6,2,2,5,7,4,8,0,9,5,6,1,4,4,7,1,9,3,8,2,9,0,5,0,8,5,5,0,2,2,8,4,6,0,6,3,7,2,3,1,8,1,3,0,1,2,4,2,7,0,3,4,5,3,6,1,2,1,7,3,3,0,7,4,4,1,5,1,1,3,5,2,6,3,4,1,6,2,5,});
sparse_graph3_ints[40][23] = vector<int>({7,8,0,0,3,3,0,11,7,7,1,3,6,8,1,11,1,5,6,7,5,9,0,10,5,8,0,5,0,1,0,6,3,4,4,9,1,10,6,6,1,4,2,10,1,1,0,7,5,7,3,10,2,4,4,8,3,9,0,9,2,3,1,9,2,5,5,6,0,4,2,9,3,5,4,7,0,3,0,8,3,8,1,6,0,2,4,4,2,2,1,7,5,5,1,2,4,6,2,8,});
sparse_graph3_ints[40][26] = vector<int>({8,9,0,0,0,4,0,12,4,4,8,8,7,9,0,8,0,13,2,2,7,8,4,5,6,10,0,6,1,12,6,9,3,5,7,7,2,12,5,10,0,10,2,4,6,8,0,7,0,2,3,4,0,11,5,9,4,11,1,11,2,6,0,5,4,6,3,11,4,10,6,7,2,11,2,5,1,6,1,4,0,9,5,8,3,6,2,3,3,3,0,1,1,5,5,5,3,10,6,6,});
sparse_graph3_ints[40][30] = vector<int>({10,10,0,0,0,4,0,14,5,5,0,8,9,10,0,15,9,9,4,6,2,2,8,11,0,10,8,10,0,6,6,6,1,14,4,4,7,11,8,9,5,6,2,14,0,12,7,10,4,5,2,4,0,9,0,2,0,7,0,13,6,12,8,8,1,13,4,7,0,5,3,6,6,11,7,9,2,6,5,7,1,4,3,5,0,11,2,13,1,8,5,12,6,7,3,13,3,7,3,4,});
sparse_graph3_ints[40][35] = vector<int>({11,12,0,0,3,5,0,17,7,7,1,3,0,12,11,11,1,17,10,12,5,5,0,5,7,8,0,10,10,11,0,16,9,13,6,7,0,9,3,3,6,6,9,12,0,13,1,16,10,10,6,8,5,6,1,5,0,11,0,8,0,1,2,3,5,7,2,16,0,7,9,11,4,5,5,8,8,13,0,15,8,8,4,6,1,4,1,7,1,1,4,4,1,11,0,6,4,7,1,15,});
sparse_graph3_ints[40][40] = vector<int>({13,13,0,0,0,8,0,20,0,14,0,4,9,9,6,6,12,14,8,8,4,4,12,13,0,12,0,19,2,2,8,9,11,14,1,19,0,10,0,16,7,7,12,12,4,6,8,10,11,13,2,19,7,8,0,6,5,6,10,15,0,18,9,10,6,8,0,15,10,14,2,18,0,13,7,9,4,5,6,7,11,12,0,11,2,4,5,5,7,10,4,18,10,10,6,9,0,9,5,7,});
sparse_graph3_ints[40][45] = vector<int>({15,15,0,0,5,5,0,22,9,9,1,3,0,17,0,12,11,11,0,7,7,7,14,15,1,22,13,16,10,10,14,14,0,14,3,21,0,18,8,8,10,11,3,5,6,7,13,15,1,21,0,16,0,10,9,10,0,4,6,6,9,11,7,19,0,21,0,15,8,9,0,9,5,7,3,3,0,13,7,9,12,16,13,14,2,21,10,12,7,8,1,17,8,10,0,11,4,5,6,8,});
sparse_graph3_ints[40][50] = vector<int>({0,0,16,17,8,8,0,25,4,4,12,12,0,20,0,4,0,12,0,16,10,10,6,22,6,6,16,16,15,17,12,13,0,24,9,9,0,18,0,8,11,11,7,7,2,2,0,14,4,6,7,21,11,12,15,16,8,9,0,10,0,19,1,24,14,18,9,10,11,13,6,8,10,12,8,21,14,17,2,24,0,21,10,11,0,15,7,8,0,6,5,5,8,10,0,17,13,13,6,7,});
sparse_graph3_ints[40][55] = vector<int>({18,18,0,0,0,16,0,27,0,8,13,13,0,22,7,7,1,3,11,11,9,23,9,9,0,12,0,19,17,19,14,14,5,5,17,18,7,24,1,27,10,10,0,23,12,12,0,5,8,8,13,14,0,18,0,14,5,7,3,5,9,10,0,21,11,12,16,19,8,23,13,15,7,8,10,11,0,10,0,15,0,26,0,20,8,9,12,13,9,11,6,7,0,17,17,17,11,13,5,25,});
sparse_graph3_ints[40][60] = vector<int>({0,22,0,0,20,20,4,4,0,30,8,8,12,12,16,16,0,16,0,4,8,26,0,12,0,26,10,10,14,14,6,6,0,20,19,20,10,25,6,20,11,11,15,15,0,8,9,9,0,18,13,13,0,24,7,7,0,14,10,11,18,21,1,29,9,25,15,16,19,19,2,2,4,6,12,13,8,9,14,16,18,20,12,24,6,22,10,12,0,29,12,14,0,21,14,15,6,27,0,25,});
sparse_graph3_ints[40][70] = vector<int>({23,23,8,8,0,0,0,24,16,16,0,8,10,30,0,30,12,12,0,35,0,20,2,2,19,19,0,12,8,24,0,16,10,10,14,14,6,6,14,28,0,28,22,24,18,18,6,20,6,32,8,26,0,34,4,4,15,15,11,11,0,26,0,22,13,13,9,9,0,18,12,29,22,23,18,19,8,22,16,17,7,7,11,12,0,14,9,10,13,14,0,4,4,6,15,16,10,24,18,20,});
sparse_graph3_ints[40][80] = vector<int>({0,40,8,8,0,0,26,27,0,24,19,19,0,6,0,32,12,12,18,31,16,16,0,18,0,10,8,30,22,22,8,36,0,28,8,26,1,2,0,14,10,10,14,14,0,36,6,6,0,22,14,28,8,22,12,34,4,4,17,17,20,20,26,26,22,23,16,32,12,26,13,13,10,11,0,20,9,9,7,7,0,16,18,18,15,15,6,20,11,12,0,26,8,24,0,30,0,34,6,37,});
sparse_graph3_ints[40][100] = vector<int>({33,33,0,0,0,28,0,46,8,8,26,26,18,18,0,38,2,4,14,38,14,14,0,20,22,22,22,39,8,46,12,28,5,5,0,34,7,38,12,12,0,50,0,24,17,31,8,24,30,30,0,16,0,42,10,10,1,2,0,12,14,43,22,34,16,16,8,34,20,20,10,26,0,31,24,24,28,28,7,42,6,22,0,8,14,30,26,37,18,34,6,7,6,32,18,41,12,36,32,34,});
sparse_graph3_ints[39][6] = vector<int>({0,0,2,2,0,3,1,2,1,1,0,2,0,1,});
sparse_graph3_ints[39][8] = vector<int>({0,0,2,3,0,4,1,3,2,2,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[39][10] = vector<int>({3,3,0,0,2,4,0,5,1,4,2,3,0,4,1,3,2,2,0,1,0,3,0,2,1,2,1,1,});
sparse_graph3_ints[39][12] = vector<int>({4,4,0,0,0,6,3,4,2,5,0,2,1,5,3,3,2,4,0,5,1,4,0,1,2,3,0,4,1,3,2,2,1,1,0,3,1,2,});
sparse_graph3_ints[39][14] = vector<int>({4,5,0,0,0,6,0,2,4,4,3,5,0,7,2,6,1,6,3,4,2,5,0,4,1,5,0,1,3,3,2,4,1,1,0,5,0,3,1,4,1,2,2,3,2,2,1,3,});
sparse_graph3_ints[39][16] = vector<int>({5,5,0,0,0,3,0,8,4,6,4,5,0,4,3,6,2,7,1,7,0,2,0,7,4,4,3,5,2,6,2,2,0,6,1,6,1,2,2,5,0,1,3,4,1,3,1,1,1,5,2,4,3,3,0,5,1,4,2,3,});
sparse_graph3_ints[39][18] = vector<int>({6,6,0,0,0,4,0,8,5,6,2,2,4,7,0,9,5,5,4,6,1,3,3,7,1,8,2,8,0,2,4,5,0,6,2,7,3,6,0,3,1,7,0,5,0,7,4,4,1,2,3,5,2,6,1,4,0,1,2,3,2,4,1,6,1,1,3,4,2,5,3,3,1,5,});
sparse_graph3_ints[39][20] = vector<int>({6,7,0,0,0,4,0,10,6,6,2,2,5,7,0,6,4,8,0,9,5,6,4,7,2,4,1,9,0,2,0,5,3,8,0,8,2,9,5,5,1,4,4,6,2,8,2,3,3,7,1,8,0,3,3,3,1,3,4,5,1,5,0,1,2,7,1,2,0,7,3,6,4,4,3,4,1,7,1,6,3,5,2,5,2,6,1,1,});
sparse_graph3_ints[39][23] = vector<int>({7,8,0,0,1,3,0,11,3,4,7,7,6,8,0,7,1,11,0,5,6,7,5,9,0,10,3,3,0,1,1,5,5,8,0,6,1,1,3,5,1,10,6,6,1,4,4,9,2,10,5,7,2,3,0,9,3,10,4,4,2,5,0,3,4,8,2,4,1,9,3,9,0,8,0,4,1,6,5,6,2,9,1,7,2,2,5,5,4,7,4,5,3,8,1,2,2,6,0,2,});
sparse_graph3_ints[39][26] = vector<int>({8,9,0,0,0,4,0,12,4,4,8,8,0,8,7,9,0,13,2,2,4,5,7,8,0,6,6,10,4,6,1,12,0,2,2,4,6,9,0,10,0,7,7,7,2,12,5,5,3,5,5,10,0,5,6,8,0,11,3,4,2,6,1,11,5,9,4,11,0,9,2,5,0,3,3,6,6,7,3,11,1,6,2,11,6,6,1,4,4,10,1,7,5,6,3,3,2,3,1,8,});
sparse_graph3_ints[39][30] = vector<int>({10,10,0,0,0,10,0,4,0,14,6,6,2,4,9,10,0,15,0,8,8,11,5,6,0,2,0,6,9,9,5,5,8,10,1,14,4,4,2,2,4,6,0,12,6,7,7,11,2,14,8,9,4,5,0,9,0,5,5,7,0,7,7,10,4,7,0,13,8,8,1,13,6,12,0,11,3,5,1,4,3,6,2,6,7,7,2,3,1,8,3,4,6,11,3,7,7,9,2,13,});
sparse_graph3_ints[39][35] = vector<int>({11,12,0,0,5,5,0,17,1,3,7,7,0,12,3,3,11,11,0,9,1,17,8,8,10,12,6,6,0,7,0,13,0,3,3,5,7,8,10,11,0,16,9,13,0,11,6,7,2,16,9,12,7,9,4,4,0,10,5,6,0,5,6,8,1,16,0,14,0,8,5,7,10,10,4,5,9,11,5,8,8,13,3,16,0,15,8,9,1,5,1,11,4,6,6,9,1,7,4,7,});
sparse_graph3_ints[39][40] = vector<int>({0,0,13,13,0,12,0,20,0,4,9,9,0,8,6,6,0,16,12,14,8,8,4,4,4,18,12,13,10,10,0,14,7,7,0,10,2,4,0,2,5,5,8,9,1,19,11,14,0,6,6,7,9,10,0,19,12,12,0,15,7,8,4,6,8,10,2,19,11,13,2,2,6,8,4,5,0,13,0,18,10,15,7,9,5,6,0,11,3,4,0,9,5,7,7,10,3,18,10,14,});
sparse_graph3_ints[39][45] = vector<int>({15,15,0,0,3,5,0,22,0,14,11,11,7,7,1,3,0,18,9,9,0,10,7,19,14,15,6,6,0,7,10,11,8,8,0,16,0,12,5,5,0,3,3,3,1,22,10,10,13,16,7,8,0,17,14,14,5,20,11,12,8,9,5,6,0,13,9,10,13,15,1,21,0,19,0,8,6,7,0,15,9,11,6,19,7,9,0,11,10,12,0,21,3,21,1,17,5,7,8,10,});
sparse_graph3_ints[39][50] = vector<int>({0,0,16,17,8,8,0,25,0,6,12,12,0,20,0,10,0,14,8,21,1,2,10,10,6,6,0,18,4,4,16,16,13,13,6,22,0,24,9,9,11,11,0,16,7,7,0,12,0,4,0,8,15,17,5,5,12,13,9,10,0,21,6,7,8,9,7,21,11,12,10,11,7,8,0,19,0,15,15,16,8,10,10,12,12,14,14,18,1,24,9,20,4,18,0,17,2,24,0,22,});
sparse_graph3_ints[39][55] = vector<int>({0,19,0,0,18,18,0,10,0,27,13,13,9,9,3,3,9,23,0,14,0,23,7,7,11,11,0,3,5,5,15,15,7,19,17,19,0,16,7,24,10,10,0,20,8,8,12,12,0,7,6,6,14,14,0,12,1,3,3,5,1,27,0,22,0,17,9,10,7,8,11,12,17,18,10,22,13,14,10,11,5,19,8,9,0,18,5,6,0,13,0,8,6,7,0,15,9,11,7,9,});
sparse_graph3_ints[39][60] = vector<int>({0,0,12,12,20,20,0,30,4,4,0,14,0,22,16,16,8,26,2,2,10,10,6,6,0,18,0,26,8,8,14,14,6,22,0,10,12,24,0,6,0,2,6,18,9,9,0,20,0,16,11,11,7,7,19,20,13,13,15,15,0,24,0,12,5,5,8,20,4,28,16,17,10,25,10,11,8,9,0,8,6,7,13,14,11,12,2,4,9,10,18,21,15,16,12,13,19,19,11,24,});
sparse_graph3_ints[39][70] = vector<int>({0,14,23,23,0,0,0,35,16,16,0,26,4,4,12,12,14,28,0,22,2,2,10,10,0,31,20,20,6,6,0,18,8,22,12,24,6,32,8,8,14,14,0,10,18,18,6,26,6,18,0,28,0,6,0,2,10,30,0,24,0,20,0,16,9,9,11,11,7,7,6,22,13,13,22,24,16,27,14,15,17,17,10,24,19,19,0,12,5,5,15,16,6,20,10,11,8,9,12,13,});
sparse_graph3_ints[39][80] = vector<int>({26,27,12,12,0,0,0,40,4,4,20,20,0,32,0,14,0,24,12,34,16,16,0,20,2,2,10,24,10,10,0,28,23,23,6,6,14,28,0,36,20,30,14,14,8,8,6,30,8,22,18,18,6,37,0,10,0,18,6,18,12,26,8,32,16,32,0,6,0,2,22,22,6,26,6,21,0,16,9,9,7,7,11,11,0,22,0,26,26,26,13,13,0,30,15,15,0,12,5,5,});
sparse_graph3_ints[39][100] = vector<int>({0,42,33,33,14,14,0,0,0,14,14,43,0,32,26,26,0,6,18,18,0,50,10,10,16,30,0,26,0,20,22,22,22,34,0,38,0,10,8,42,12,26,26,37,8,35,8,22,1,2,30,30,12,12,8,8,12,36,0,46,16,16,20,40,0,23,8,46,7,32,20,20,6,6,0,17,19,32,10,24,0,29,24,24,15,38,0,35,14,28,28,28,4,4,6,20,6,40,6,29,});
sparse_graph3_ints[38][6] = vector<int>({0,0,2,2,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[38][8] = vector<int>({0,0,2,3,0,4,1,3,2,2,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[38][10] = vector<int>({3,3,0,0,0,5,2,4,1,4,2,3,0,4,1,3,2,2,0,1,0,2,0,3,1,2,1,1,});
sparse_graph3_ints[38][12] = vector<int>({4,4,0,0,0,6,0,2,3,4,2,5,1,5,3,3,2,4,0,5,0,1,1,4,2,3,0,4,1,1,1,3,2,2,0,3,1,2,});
sparse_graph3_ints[38][14] = vector<int>({4,5,0,0,0,2,0,6,4,4,3,5,0,7,0,3,2,6,1,6,3,4,2,5,1,2,1,5,0,4,3,3,0,1,2,4,0,5,2,2,1,1,1,4,2,3,1,3,});
sparse_graph3_ints[38][16] = vector<int>({5,5,0,0,0,4,0,8,4,6,0,2,4,5,2,2,3,6,0,7,2,7,1,7,0,3,4,4,3,5,2,6,0,6,1,2,1,6,3,4,1,3,2,5,0,5,0,1,1,4,1,1,2,4,1,5,2,3,3,3,});
sparse_graph3_ints[38][18] = vector<int>({6,6,0,0,0,4,0,8,5,6,2,2,0,9,4,7,5,5,0,2,2,3,4,6,1,8,3,7,0,6,2,8,0,5,4,5,0,3,2,7,3,6,1,4,1,7,2,4,1,2,0,7,4,4,1,3,3,5,2,6,3,3,0,1,3,4,1,6,1,1,1,5,2,5,});
sparse_graph3_ints[38][20] = vector<int>({6,7,0,0,0,4,0,10,6,6,2,4,0,2,5,7,0,6,2,2,0,9,5,6,4,8,0,5,1,9,4,7,0,8,3,3,3,8,2,9,5,5,2,3,4,6,1,4,2,8,0,3,3,4,3,7,1,8,4,4,1,5,1,2,0,7,1,3,4,5,2,5,3,6,2,7,1,6,1,7,3,5,2,6,0,1,1,1,});
sparse_graph3_ints[38][23] = vector<int>({0,0,7,8,0,7,0,11,0,3,7,7,3,3,6,8,1,11,3,5,1,3,6,7,4,4,0,10,5,9,0,5,5,8,3,4,0,6,0,1,0,4,0,8,6,6,1,10,1,5,1,1,2,3,2,5,4,9,0,9,2,10,5,7,4,5,1,4,3,10,5,5,2,4,1,6,4,8,1,9,2,2,1,7,3,9,5,6,1,2,2,6,3,6,2,9,4,7,4,6,});
sparse_graph3_ints[38][26] = vector<int>({8,9,0,0,4,4,0,12,0,4,0,8,8,8,0,13,7,9,5,5,0,2,0,6,2,2,4,5,7,8,2,4,0,10,6,10,1,12,4,6,6,9,3,4,2,12,5,6,7,7,0,7,0,3,3,5,0,5,0,9,6,6,5,10,6,8,0,11,2,6,1,4,3,6,1,11,2,5,3,3,1,6,1,2,2,3,1,7,4,11,6,7,1,8,5,9,3,11,2,7,});
sparse_graph3_ints[38][30] = vector<int>({10,10,0,0,0,10,0,4,0,15,4,4,6,6,9,10,2,4,0,2,0,14,6,7,0,8,8,11,9,9,5,6,0,6,0,12,1,14,8,10,5,5,2,2,4,6,2,14,7,7,3,4,7,11,0,11,4,5,5,7,8,9,0,9,3,3,0,7,0,3,0,5,4,7,3,5,6,8,3,6,7,10,0,13,6,12,8,8,1,13,2,6,1,4,1,9,4,8,2,5,});
sparse_graph3_ints[38][35] = vector<int>({11,12,0,0,0,13,3,3,0,17,5,5,8,8,0,3,0,10,7,7,11,11,1,17,0,7,10,12,6,6,1,3,3,5,7,9,0,12,0,8,5,6,7,8,3,16,0,14,10,11,9,13,0,16,6,7,0,11,8,9,2,16,4,4,5,7,0,5,0,9,9,12,6,8,4,5,1,5,1,1,0,6,4,6,9,9,5,15,0,15,1,11,6,9,10,10,1,16,5,8,});
sparse_graph3_ints[38][40] = vector<int>({0,0,13,13,0,10,0,20,8,8,2,2,0,14,4,4,10,10,6,6,4,18,0,16,12,14,0,6,0,2,9,9,7,7,0,12,5,5,0,8,2,4,12,13,9,10,6,7,6,17,0,4,4,5,7,8,8,10,0,15,0,19,11,14,5,6,8,9,1,19,12,12,5,17,6,8,9,11,0,13,0,18,4,6,0,11,7,9,3,4,11,13,1,15,2,19,5,7,8,11,});
sparse_graph3_ints[38][45] = vector<int>({15,15,0,0,0,12,0,22,3,3,11,11,0,16,5,5,9,9,7,19,0,3,7,7,0,19,0,8,14,15,11,12,8,9,0,14,6,6,1,3,3,5,0,10,10,10,7,8,0,17,1,22,8,18,10,11,8,8,5,6,0,13,0,18,14,14,9,10,5,20,13,16,12,12,0,6,6,7,0,15,7,9,9,11,0,11,4,4,5,7,5,15,13,15,6,19,0,21,10,12,});
sparse_graph3_ints[38][50] = vector<int>({0,0,16,17,0,10,0,25,10,10,2,2,0,20,0,14,13,13,6,22,8,8,4,4,6,6,0,18,6,18,10,20,12,12,0,6,0,2,16,16,0,16,9,9,7,7,0,12,11,11,5,5,0,22,13,14,15,17,8,21,0,24,0,8,4,18,9,10,7,8,10,11,12,13,0,4,3,4,5,6,8,9,7,21,11,12,6,7,0,17,8,10,6,16,2,24,0,15,0,19,});
sparse_graph3_ints[38][55] = vector<int>({11,11,0,0,18,18,0,8,0,27,0,20,7,7,1,3,15,15,0,16,11,22,7,19,0,12,9,9,0,23,13,13,5,25,5,5,3,3,1,1,5,17,0,18,0,14,8,8,10,10,6,6,17,19,9,23,12,12,14,14,0,10,4,4,9,10,7,8,0,24,1,27,7,20,14,15,17,18,10,22,12,13,0,21,10,11,0,17,5,19,0,5,8,9,5,7,11,12,7,24,});
sparse_graph3_ints[38][60] = vector<int>({12,12,0,0,20,20,0,30,4,4,0,14,0,22,16,16,8,26,2,2,8,9,0,18,6,22,6,6,0,26,14,14,0,10,12,24,10,10,6,18,0,6,0,2,7,7,0,16,0,20,10,20,0,24,4,28,0,12,5,5,8,8,11,11,4,16,13,13,17,17,19,20,6,20,15,15,9,10,13,23,0,8,8,22,11,12,13,14,6,7,9,9,2,4,0,4,4,6,7,8,});
sparse_graph3_ints[38][70] = vector<int>({23,23,0,20,0,0,0,35,4,4,16,16,0,28,0,14,12,12,14,28,2,2,0,24,10,26,10,10,20,20,6,6,8,20,8,8,6,32,0,32,0,10,18,18,14,14,10,22,0,18,6,26,0,6,0,2,6,19,0,22,0,26,10,30,18,26,14,24,0,16,6,16,0,30,9,9,7,7,0,12,5,5,6,23,6,28,11,11,13,13,15,15,12,22,0,8,17,17,22,24,});
sparse_graph3_ints[38][80] = vector<int>({0,14,26,27,0,0,0,40,18,18,0,23,0,32,4,4,12,34,13,13,22,22,10,10,2,2,10,22,14,26,0,28,0,18,0,36,20,30,8,8,0,10,6,30,16,16,6,20,6,37,6,7,20,20,11,12,12,24,0,20,6,33,17,27,24,24,0,2,0,26,14,15,6,27,0,6,12,30,16,32,6,24,5,17,0,16,5,5,0,12,0,30,9,9,8,20,0,34,10,28,});
sparse_graph3_ints[38][100] = vector<int>({21,21,6,6,0,50,0,0,33,33,13,13,0,34,20,40,0,42,0,20,27,28,1,4,17,17,0,28,12,34,8,8,8,46,13,25,25,25,0,24,12,38,11,11,0,38,4,4,8,24,0,46,19,32,26,37,0,14,13,29,15,15,0,2,30,30,22,34,0,17,14,43,6,32,23,23,7,21,0,31,6,36,9,10,19,19,6,40,0,10,17,29,16,38,7,28,7,43,6,18,});
sparse_graph3_ints[37][6] = vector<int>({0,0,2,2,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[37][8] = vector<int>({2,3,0,0,0,4,1,3,2,2,0,3,1,2,0,2,0,1,1,1,});
sparse_graph3_ints[37][10] = vector<int>({3,3,0,0,0,5,2,4,0,4,2,3,1,4,0,2,1,3,2,2,0,1,0,3,1,2,1,1,});
sparse_graph3_ints[37][12] = vector<int>({4,4,0,0,0,6,0,2,3,4,2,5,1,5,3,3,2,4,0,5,0,4,0,1,1,4,2,3,1,1,0,3,2,2,1,3,1,2,});
sparse_graph3_ints[37][14] = vector<int>({4,5,0,0,0,2,0,6,4,4,0,3,3,5,0,7,2,6,0,4,1,6,3,4,2,5,1,2,1,5,2,2,3,3,2,4,0,1,0,5,1,1,1,4,2,3,1,3,});
sparse_graph3_ints[37][16] = vector<int>({5,5,0,0,0,4,0,8,4,6,0,2,2,2,4,5,3,6,0,7,0,3,2,7,1,7,4,4,0,6,3,5,2,6,1,2,1,3,1,6,0,5,3,4,2,5,2,3,1,4,1,1,2,4,0,1,3,3,1,5,});
sparse_graph3_ints[37][18] = vector<int>({6,6,0,0,0,4,0,8,5,6,0,2,2,4,0,9,4,7,2,2,5,5,0,6,4,6,1,8,0,5,3,7,2,8,0,3,2,3,4,5,1,4,2,7,3,6,1,2,3,3,1,7,0,7,4,4,1,3,3,5,3,4,2,6,1,5,1,6,1,1,2,5,0,1,});
sparse_graph3_ints[37][20] = vector<int>({6,7,0,0,0,4,0,10,2,4,6,6,0,2,0,6,5,7,2,2,3,3,0,9,5,6,4,8,0,8,4,4,1,9,4,7,0,5,0,3,3,4,5,5,2,9,3,8,2,3,1,4,4,6,0,7,2,8,1,2,1,5,2,5,3,7,1,8,1,3,4,5,1,6,3,5,3,6,2,6,2,7,1,7,1,1,0,1,});
sparse_graph3_ints[37][23] = vector<int>({7,8,0,0,0,7,0,11,0,3,3,3,7,7,4,4,1,11,6,8,1,3,3,5,6,7,0,10,0,6,5,5,1,1,0,5,5,9,0,8,3,4,5,8,0,4,4,5,1,5,1,10,6,6,0,9,2,3,0,1,1,4,2,5,1,2,2,10,1,7,4,9,2,4,5,7,1,6,2,2,3,10,3,6,5,6,1,9,2,6,4,8,3,9,4,6,4,7,2,9,});
sparse_graph3_ints[37][26] = vector<int>({8,9,0,0,0,8,2,2,0,13,5,5,8,8,0,6,0,2,0,12,4,4,7,9,6,6,0,4,0,10,2,4,4,5,7,8,1,12,6,10,4,6,3,4,5,6,2,12,6,9,0,9,0,7,0,3,0,5,3,5,3,3,7,7,3,6,2,3,1,2,2,5,1,4,2,6,1,6,0,11,6,8,4,7,5,10,1,7,1,11,1,8,6,7,1,5,1,3,2,7,});
sparse_graph3_ints[37][30] = vector<int>({10,10,0,0,6,6,2,2,0,15,4,4,0,10,0,6,0,2,9,10,7,7,0,14,5,5,0,8,0,12,8,11,6,7,2,14,9,9,0,4,2,4,5,6,8,10,4,5,6,8,1,14,0,11,3,4,4,6,0,9,5,7,3,3,0,3,0,7,0,5,3,5,1,2,2,5,2,3,4,7,7,11,2,6,1,4,3,6,5,8,8,9,1,6,3,13,7,8,1,9,});
sparse_graph3_ints[37][35] = vector<int>({0,0,11,12,5,5,0,17,1,3,0,12,8,8,0,8,3,3,7,7,5,15,11,11,0,14,1,1,6,6,9,9,1,17,0,10,3,5,0,5,10,12,7,9,5,6,0,13,6,7,4,4,0,9,7,8,0,6,0,3,0,11,4,5,5,7,8,9,9,13,3,16,0,16,10,11,0,7,6,8,1,13,2,3,4,6,3,4,2,16,0,15,4,15,6,9,9,12,1,11,});
sparse_graph3_ints[37][40] = vector<int>({0,0,13,13,0,10,0,20,8,8,2,2,0,14,4,4,10,10,6,6,6,17,0,16,0,6,0,2,0,12,7,7,12,14,9,9,5,5,0,8,0,4,2,4,6,7,9,10,4,18,12,13,4,5,7,8,10,11,8,9,5,6,8,16,0,17,0,13,8,10,6,8,0,15,1,19,4,14,11,14,5,17,9,11,0,11,7,9,4,6,3,4,12,12,0,19,5,7,0,9,});
sparse_graph3_ints[37][45] = vector<int>({0,16,0,0,15,15,0,8,0,22,11,11,7,7,1,3,7,19,0,12,9,9,0,19,5,5,3,3,1,1,5,15,12,12,0,14,8,8,6,6,0,10,4,4,9,10,0,6,14,15,5,20,7,8,5,6,9,18,11,12,10,10,0,18,0,15,8,9,1,22,5,16,6,7,0,13,7,9,0,11,4,5,5,7,0,3,3,4,0,9,0,5,0,17,10,11,6,8,8,10,});
sparse_graph3_ints[37][50] = vector<int>({0,10,16,17,0,0,0,18,0,25,12,12,2,2,8,8,10,20,0,14,4,4,0,22,10,10,6,6,6,18,14,14,6,22,0,6,0,2,0,20,0,16,6,16,0,12,7,7,9,9,5,5,11,11,13,13,16,16,0,8,4,14,6,7,8,9,2,4,10,11,9,20,13,14,15,17,11,12,4,16,4,23,7,18,0,4,9,10,7,8,0,24,8,21,12,13,4,6,4,18,});
sparse_graph3_ints[37][55] = vector<int>({0,0,0,20,18,18,0,8,0,27,7,7,11,11,9,23,1,3,0,16,15,15,0,24,0,12,9,9,7,17,13,13,5,5,3,3,9,19,1,1,5,25,13,21,5,15,0,22,0,14,0,18,5,21,8,8,6,6,0,10,4,4,10,10,12,12,0,6,5,17,14,14,17,19,15,16,7,8,9,10,11,22,11,12,7,19,13,14,7,24,1,27,5,7,8,9,10,11,3,15,});
sparse_graph3_ints[37][60] = vector<int>({0,30,0,14,0,0,20,20,14,14,4,4,0,22,10,10,12,24,2,2,0,18,6,6,0,26,8,22,17,17,8,8,12,12,0,10,6,20,8,26,6,16,0,2,0,6,15,15,0,16,0,12,0,20,4,28,0,24,7,7,9,9,5,5,0,8,12,20,4,24,6,18,13,13,11,11,4,14,9,19,14,23,16,16,19,20,3,3,0,4,4,16,8,9,6,8,10,11,12,13,});
sparse_graph3_ints[37][70] = vector<int>({0,0,10,10,23,23,0,32,4,4,14,15,0,24,0,14,14,28,17,18,2,2,8,24,0,18,6,32,6,6,0,28,12,12,8,8,20,21,6,18,0,35,6,22,0,10,14,24,6,28,0,22,0,2,0,6,16,16,18,26,10,30,5,16,12,22,19,19,0,16,0,12,13,13,10,20,0,26,0,30,0,20,10,26,7,7,9,9,6,20,5,5,0,8,4,26,11,11,4,14,});
sparse_graph3_ints[37][80] = vector<int>({26,27,7,7,0,0,18,18,0,24,0,34,12,34,4,4,13,13,0,18,0,40,22,22,2,2,8,22,12,28,10,10,0,30,20,30,16,16,0,10,0,14,8,28,0,21,6,37,20,20,0,27,6,18,8,9,6,32,0,2,12,22,18,27,24,24,0,37,0,6,11,12,5,6,14,15,6,26,14,24,7,20,16,32,0,16,13,30,0,12,4,16,0,32,8,33,16,26,0,8,});
sparse_graph3_ints[37][100] = vector<int>({0,50,0,0,18,18,26,26,7,7,0,38,33,33,0,30,2,4,20,40,14,14,13,36,10,10,14,28,0,24,0,42,5,5,22,22,8,46,8,26,0,19,30,30,0,34,12,12,0,2,0,46,8,35,23,34,0,16,8,22,17,30,16,38,26,37,16,16,6,32,8,42,20,20,0,12,0,27,20,32,14,43,24,24,8,9,12,26,0,4,4,18,6,40,0,8,6,20,28,28,});
sparse_graph3_ints[36][6] = vector<int>({2,2,0,0,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[36][8] = vector<int>({2,3,0,0,0,4,2,2,1,3,0,3,1,2,0,2,0,1,1,1,});
sparse_graph3_ints[36][10] = vector<int>({3,3,0,0,0,5,2,4,0,2,1,4,2,3,0,4,1,3,2,2,0,1,0,3,1,2,1,1,});
sparse_graph3_ints[36][12] = vector<int>({4,4,0,0,0,2,0,6,3,4,2,5,1,5,1,1,3,3,2,4,0,5,0,4,0,3,1,4,2,3,0,1,1,2,2,2,1,3,});
sparse_graph3_ints[36][14] = vector<int>({4,5,0,0,0,3,0,6,4,4,0,2,0,7,3,5,0,4,2,6,1,6,2,2,3,4,2,5,1,2,1,5,3,3,1,1,2,4,0,5,1,3,0,1,2,3,1,4,});
sparse_graph3_ints[36][16] = vector<int>({5,5,0,0,0,4,0,8,0,2,4,6,2,2,4,5,0,6,3,6,0,3,0,7,4,4,2,7,1,7,2,3,3,5,1,2,2,6,1,3,0,5,1,4,1,6,3,4,1,1,2,4,2,5,3,3,1,5,0,1,});
sparse_graph3_ints[36][18] = vector<int>({6,6,0,0,0,4,0,8,0,2,2,4,5,6,0,9,2,2,4,7,0,6,5,5,4,6,3,3,0,3,1,8,0,5,3,7,2,8,2,3,4,5,1,2,1,4,4,4,0,7,2,7,3,6,3,4,1,3,1,7,1,5,3,5,1,1,2,5,2,6,1,6,0,1,});
sparse_graph3_ints[36][20] = vector<int>({6,7,0,0,2,2,0,10,0,6,0,2,6,6,4,4,0,4,5,7,2,4,3,3,0,9,5,6,0,8,4,8,0,3,3,4,1,9,0,5,4,7,5,5,2,3,1,2,0,7,2,9,2,5,3,8,1,4,4,6,1,5,4,5,2,8,1,3,3,5,1,6,1,8,3,7,1,1,2,6,3,6,1,7,2,7,0,1,});
sparse_graph3_ints[36][23] = vector<int>({7,8,0,0,1,3,0,11,5,5,3,3,0,8,7,7,1,1,1,11,4,4,6,8,0,5,0,7,0,3,4,5,0,6,6,7,0,10,3,5,5,9,0,4,3,4,2,3,0,9,5,8,1,5,1,2,2,5,6,6,2,10,1,7,1,4,2,2,0,1,2,4,3,6,1,10,1,6,4,6,4,9,5,7,5,6,3,10,2,6,1,9,3,9,4,8,2,7,1,8,});
sparse_graph3_ints[36][26] = vector<int>({0,0,8,9,4,4,0,13,2,2,0,10,6,6,0,6,0,2,8,8,0,8,5,5,0,12,7,9,0,4,3,4,5,6,2,12,2,4,4,6,7,8,6,10,1,12,0,9,4,5,3,3,0,3,0,7,0,5,3,5,1,2,2,3,5,7,6,9,3,6,2,5,7,7,1,4,2,6,4,7,0,11,1,6,1,5,1,8,6,7,5,10,1,7,6,8,3,11,1,3,});
sparse_graph3_ints[36][30] = vector<int>({10,10,0,0,6,6,2,2,0,15,4,4,0,10,0,6,0,2,7,7,0,12,9,10,2,14,0,8,5,5,0,4,2,4,6,7,0,14,8,11,4,5,6,8,9,9,4,13,5,6,0,11,8,10,7,8,3,4,4,6,1,14,0,9,5,7,3,3,1,2,0,7,3,5,0,5,8,8,7,11,5,8,4,7,3,13,8,9,0,13,6,12,3,6,2,3,0,3,2,5,});
sparse_graph3_ints[36][35] = vector<int>({0,0,11,12,0,8,0,17,7,7,1,3,0,12,9,9,5,5,5,15,3,3,0,14,1,1,0,10,6,6,8,8,11,11,4,4,0,6,5,6,7,8,1,17,10,12,8,9,0,13,6,14,6,7,0,11,0,15,3,5,0,3,0,5,0,9,5,7,4,5,7,9,0,7,2,3,3,4,4,6,6,8,3,16,1,13,9,13,8,10,10,11,4,15,0,16,1,11,6,9,});
sparse_graph3_ints[36][40] = vector<int>({13,13,0,0,0,10,0,20,8,8,2,2,0,14,4,4,10,10,6,6,8,16,0,16,0,6,0,2,4,18,0,12,4,14,0,8,5,5,7,8,12,14,10,11,9,9,6,7,2,4,0,4,4,5,7,7,6,17,9,10,12,13,0,18,8,9,5,6,11,11,6,8,8,10,0,13,5,14,0,15,5,17,0,11,3,4,4,6,7,9,5,7,4,12,0,9,9,11,7,16,});
sparse_graph3_ints[36][45] = vector<int>({0,0,15,15,0,13,0,22,3,3,9,9,0,18,5,5,12,12,7,19,1,1,7,7,0,10,0,16,5,15,1,3,0,8,11,11,0,20,3,21,9,18,6,6,8,8,5,17,10,10,4,4,0,15,0,6,0,12,5,13,14,15,7,8,5,6,11,12,9,10,6,7,8,9,13,13,7,15,0,14,8,18,0,17,10,11,5,20,0,19,3,13,7,9,0,11,4,5,5,7,});
sparse_graph3_ints[36][50] = vector<int>({16,17,0,0,8,8,0,25,4,4,12,12,0,18,0,4,0,14,10,20,6,6,0,22,10,10,0,10,6,16,14,14,6,22,0,2,0,6,8,18,0,16,0,12,0,20,4,14,0,8,7,7,5,5,9,9,4,20,11,11,3,3,13,13,4,16,2,2,8,16,10,11,8,9,6,7,4,12,16,16,12,19,4,23,8,21,13,14,4,18,11,12,9,10,7,8,4,6,6,14,});
sparse_graph3_ints[36][55] = vector<int>({18,18,0,12,0,0,0,20,5,25,3,3,9,9,14,14,0,24,0,16,5,5,12,12,1,1,13,21,7,7,5,19,0,27,5,15,7,21,0,8,1,3,16,16,9,23,11,11,0,14,9,17,0,10,0,18,0,22,6,6,8,8,4,4,5,17,10,10,11,19,0,6,5,13,13,13,15,15,5,21,7,17,17,19,9,19,11,22,7,24,1,27,0,25,7,8,5,7,3,13,});
sparse_graph3_ints[36][60] = vector<int>({0,14,0,0,0,30,20,20,8,18,4,4,10,10,16,16,0,4,12,24,0,18,0,26,6,6,0,22,8,8,12,12,6,24,0,10,14,14,0,2,6,16,8,26,0,6,12,20,17,18,4,22,0,16,0,12,0,20,4,14,4,28,8,21,0,8,7,7,5,5,0,24,15,22,9,9,4,20,0,28,11,11,3,3,13,13,15,15,4,16,10,18,8,16,4,24,10,22,2,2,});
sparse_graph3_ints[36][70] = vector<int>({6,32,0,0,13,13,0,14,23,23,0,28,4,4,18,18,0,22,10,10,2,2,14,28,8,22,0,18,0,32,6,6,8,8,16,16,6,20,10,27,0,35,21,21,0,10,0,2,6,26,0,25,11,12,0,6,5,16,15,24,14,15,10,24,18,26,0,16,0,12,5,29,10,30,6,18,0,20,19,20,0,30,7,7,4,14,12,20,5,5,4,24,14,22,0,8,10,18,9,9,});
sparse_graph3_ints[36][80] = vector<int>({0,0,0,40,26,27,18,19,0,26,9,10,2,4,0,12,6,30,16,32,12,13,22,22,15,16,0,22,0,16,0,32,6,24,10,35,0,2,5,5,16,25,8,8,12,22,7,33,0,19,7,20,0,37,21,29,11,11,0,4,0,29,0,8,13,30,24,24,6,7,12,25,14,14,6,18,6,27,20,20,18,27,5,37,0,34,0,14,17,17,10,29,5,16,0,24,10,20,0,10,});
sparse_graph3_ints[36][100] = vector<int>({5,5,33,33,20,20,0,0,13,36,0,36,0,27,0,50,9,9,16,16,28,28,14,43,0,42,12,12,24,24,2,4,0,22,8,26,26,37,7,7,7,33,16,30,0,2,14,14,0,46,0,16,17,37,0,30,7,46,18,18,20,31,12,26,8,36,0,19,20,40,8,22,22,22,23,34,6,41,30,31,0,12,0,4,6,20,10,11,14,28,0,33,0,8,0,39,26,26,11,41,});
sparse_graph3_ints[35][6] = vector<int>({2,2,0,0,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[35][8] = vector<int>({2,3,0,0,0,4,2,2,1,3,0,3,1,2,0,2,0,1,1,1,});
sparse_graph3_ints[35][10] = vector<int>({3,3,0,0,0,5,0,2,2,4,0,4,2,3,1,4,1,3,0,1,2,2,0,3,1,2,1,1,});
sparse_graph3_ints[35][12] = vector<int>({4,4,0,0,0,2,0,6,3,4,0,3,2,5,3,3,1,5,2,4,1,1,0,5,0,4,1,2,1,4,2,3,0,1,2,2,1,3,});
sparse_graph3_ints[35][14] = vector<int>({4,5,0,0,0,2,0,6,2,2,4,4,0,7,3,5,0,4,2,6,0,3,1,6,3,4,1,2,2,5,3,3,1,1,1,5,0,5,2,4,1,3,2,3,1,4,0,1,});
sparse_graph3_ints[35][16] = vector<int>({5,5,0,0,0,4,0,8,0,2,4,6,2,2,4,5,2,3,0,6,3,6,0,3,0,7,4,4,1,7,2,7,0,5,1,2,3,5,2,4,2,6,1,3,1,4,1,1,3,3,3,4,1,6,2,5,1,5,0,1,});
sparse_graph3_ints[35][18] = vector<int>({6,6,0,0,3,3,0,9,0,2,0,4,5,6,0,6,0,8,2,2,4,7,5,5,2,4,4,6,0,3,0,5,1,8,4,4,2,8,2,3,1,2,3,4,3,7,1,4,4,5,0,7,1,3,1,1,1,5,2,7,3,6,1,7,2,5,3,5,1,6,2,6,0,1,});
sparse_graph3_ints[35][20] = vector<int>({0,0,6,7,2,2,0,10,0,6,0,2,4,4,6,6,0,4,2,4,5,7,0,8,3,4,0,3,3,3,1,9,5,6,4,8,0,9,0,5,0,7,1,2,5,5,2,3,4,7,2,5,4,5,2,9,1,4,1,5,1,1,3,5,1,3,3,8,1,6,4,6,2,8,2,6,1,8,3,7,3,6,1,7,2,7,0,1,});
sparse_graph3_ints[35][23] = vector<int>({0,0,7,8,0,8,1,3,0,11,5,5,3,3,1,1,7,7,4,4,1,11,0,5,6,8,0,7,0,3,4,5,2,3,0,6,3,5,0,10,6,7,5,9,0,9,3,4,5,6,0,4,1,2,1,5,2,4,4,6,2,10,1,7,5,8,6,6,2,5,1,4,3,6,2,2,1,10,1,6,0,2,3,10,5,7,2,6,4,9,1,9,2,7,1,8,3,9,3,7,});
sparse_graph3_ints[35][26] = vector<int>({8,9,0,0,4,4,0,13,2,2,0,10,6,6,0,6,0,2,0,8,8,8,5,5,2,12,2,4,7,9,0,12,5,6,0,4,4,5,3,4,6,10,6,7,0,9,4,6,7,8,1,12,3,3,0,3,0,7,5,7,0,5,3,5,7,7,6,9,0,11,3,11,4,7,3,6,2,3,1,1,1,4,2,5,1,2,2,6,1,5,1,8,1,3,1,6,1,9,4,11,});
sparse_graph3_ints[35][30] = vector<int>({0,0,10,10,0,10,2,2,0,15,4,4,7,7,0,6,0,2,6,6,0,12,4,13,0,8,5,5,9,10,0,4,3,3,7,8,4,5,6,7,0,14,2,14,8,11,5,6,6,8,0,11,9,9,6,12,8,8,3,4,4,6,0,9,5,7,0,7,2,3,1,1,2,4,8,10,3,13,1,11,1,14,3,5,5,8,0,5,0,3,4,7,3,6,0,13,5,12,8,9,});
sparse_graph3_ints[35][35] = vector<int>({11,12,0,0,3,3,7,7,0,17,0,13,5,5,1,1,9,9,0,10,5,15,1,3,0,8,0,15,0,6,6,6,4,4,8,8,0,12,11,11,3,13,1,17,6,14,6,7,8,9,5,6,7,8,0,11,0,14,3,5,0,3,0,5,0,9,5,7,4,5,10,12,7,9,9,10,3,16,0,7,2,3,3,4,6,8,4,6,7,14,5,11,1,11,5,8,8,10,1,13,});
sparse_graph3_ints[35][40] = vector<int>({0,0,13,13,0,10,0,20,8,8,0,4,0,16,4,4,10,10,8,16,6,6,0,14,0,2,0,6,4,14,0,12,4,18,0,8,5,5,7,7,11,11,9,9,3,3,0,18,4,12,6,14,2,2,12,14,6,17,6,7,8,9,10,11,5,6,7,8,4,5,9,10,6,8,3,4,4,6,0,11,0,13,12,13,8,10,7,16,5,14,5,7,7,9,0,15,2,12,0,9,});
sparse_graph3_ints[35][45] = vector<int>({0,0,0,14,15,15,0,22,3,3,10,10,5,5,0,18,7,7,9,18,1,1,12,12,5,17,0,8,0,12,5,13,1,3,8,8,0,10,3,21,0,16,6,6,0,20,4,4,5,15,0,6,7,19,9,9,11,11,13,13,7,15,3,13,11,17,14,15,5,20,7,8,5,6,3,11,9,15,10,11,8,9,6,7,9,10,11,12,3,15,3,17,4,5,0,11,0,13,0,15,});
sparse_graph3_ints[35][50] = vector<int>({8,8,0,0,16,17,0,25,4,4,12,12,0,18,0,4,0,14,10,20,6,6,0,22,10,10,0,10,6,16,14,14,0,2,6,22,0,6,8,18,4,14,0,12,0,20,0,16,0,8,5,5,7,7,4,20,9,9,3,3,11,11,4,12,6,14,8,16,13,13,12,19,4,18,2,2,4,23,4,16,16,16,8,21,14,15,10,17,6,8,8,9,10,11,12,13,9,10,11,12,});
sparse_graph3_ints[35][55] = vector<int>({0,12,0,27,0,0,18,18,11,11,3,3,0,20,14,14,0,16,5,5,9,9,0,23,13,21,1,1,7,7,7,17,5,25,0,8,5,21,5,15,1,3,11,19,16,16,9,23,0,14,12,12,0,10,0,18,0,25,6,6,8,8,4,4,9,17,3,19,5,13,10,10,7,21,0,6,13,13,7,15,15,15,3,13,0,22,5,19,9,19,3,23,3,11,3,17,3,26,7,24,});
sparse_graph3_ints[35][60] = vector<int>({0,0,12,12,20,20,0,30,4,4,0,14,0,22,16,16,0,4,12,24,0,18,6,6,0,26,10,10,10,20,8,8,8,18,0,10,8,26,14,14,0,2,4,18,0,6,6,24,18,18,0,16,0,12,0,24,5,15,0,20,4,28,6,22,16,22,7,7,5,5,0,8,14,20,9,9,0,28,4,20,4,14,11,11,10,22,8,16,13,13,3,3,4,12,10,18,4,22,15,15,});
sparse_graph3_ints[35][70] = vector<int>({16,16,0,0,0,35,23,23,0,22,4,4,0,14,12,12,14,28,6,24,2,2,9,9,19,19,0,18,6,29,0,26,0,10,10,18,7,7,0,29,14,22,0,2,6,32,10,11,5,20,0,6,0,32,21,21,6,6,12,26,11,21,18,26,4,16,0,16,14,15,9,25,0,12,6,17,10,30,17,18,8,8,16,24,13,13,0,20,0,24,4,28,5,5,6,22,0,8,4,14,});
sparse_graph3_ints[35][80] = vector<int>({0,0,17,17,11,11,26,27,6,6,11,34,0,28,0,34,22,22,2,2,0,40,0,18,10,28,14,14,0,22,4,4,8,8,6,21,21,29,0,14,6,25,20,20,6,34,16,26,0,2,0,10,9,10,0,31,16,32,0,6,12,22,0,25,6,18,24,24,6,28,0,37,15,16,12,30,12,13,14,24,6,37,18,28,5,16,0,16,10,20,5,32,18,19,0,12,7,7,0,20,});
sparse_graph3_ints[35][100] = vector<int>({25,25,0,0,9,9,33,33,9,32,5,5,18,18,20,40,6,47,14,14,0,22,0,28,7,40,0,34,21,21,2,4,7,7,30,30,16,38,12,12,0,38,18,30,10,24,0,43,0,2,26,37,0,16,27,28,16,16,5,21,14,25,1,46,21,32,14,43,0,19,11,34,8,22,0,4,0,50,7,30,0,31,23,23,24,34,0,12,14,36,10,11,16,28,0,8,9,42,4,38,});
sparse_graph3_ints[34][6] = vector<int>({2,2,0,0,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[34][8] = vector<int>({2,3,0,0,0,4,2,2,1,3,0,2,0,3,1,2,0,1,1,1,});
sparse_graph3_ints[34][10] = vector<int>({3,3,0,0,0,2,0,5,2,4,0,4,2,3,1,4,0,1,2,2,1,3,1,1,0,3,1,2,});
sparse_graph3_ints[34][12] = vector<int>({4,4,0,0,0,2,0,6,3,4,0,3,2,5,0,4,3,3,1,5,1,1,2,4,0,5,1,2,2,2,1,4,2,3,0,1,1,3,});
sparse_graph3_ints[34][14] = vector<int>({4,5,0,0,0,4,0,7,0,2,4,4,2,2,0,6,3,5,0,3,2,6,3,4,1,6,1,2,2,5,1,3,1,1,3,3,0,5,2,3,1,5,2,4,1,4,0,1,});
sparse_graph3_ints[34][16] = vector<int>({5,5,0,0,0,4,0,8,0,2,2,2,4,6,0,6,4,5,3,3,0,3,3,6,2,3,1,7,4,4,0,7,1,2,0,5,2,7,2,4,3,5,1,1,1,4,1,3,2,6,3,4,1,6,2,5,1,5,0,1,});
sparse_graph3_ints[34][18] = vector<int>({6,6,0,0,3,3,0,9,0,2,0,4,0,6,5,6,0,8,2,2,4,4,4,7,5,5,2,4,0,3,4,6,0,5,1,8,3,4,2,3,1,1,2,8,1,4,1,2,0,7,3,7,4,5,1,3,1,5,2,5,3,5,3,6,2,7,1,7,1,6,2,6,0,1,});
sparse_graph3_ints[34][20] = vector<int>({0,0,6,7,2,2,0,10,4,4,0,6,0,2,6,6,0,4,3,3,0,8,5,7,3,4,0,3,2,4,4,5,1,9,4,8,5,6,0,7,2,3,1,1,0,5,0,9,3,5,1,2,1,4,5,5,2,5,2,9,4,7,1,5,1,3,1,6,4,6,3,8,2,6,2,8,3,6,1,8,1,7,3,7,2,7,0,1,});
sparse_graph3_ints[34][23] = vector<int>({0,0,7,8,0,8,1,3,0,11,5,5,3,3,1,1,0,6,7,7,1,11,4,4,0,3,6,8,4,5,0,5,0,9,3,4,5,6,0,7,2,3,3,5,0,4,5,9,0,10,6,7,4,6,2,10,2,4,0,2,2,2,1,5,1,7,1,2,1,4,6,6,2,5,3,6,3,10,1,6,5,8,5,7,2,6,1,10,1,9,4,9,3,7,1,8,4,7,2,7,});
sparse_graph3_ints[34][26] = vector<int>({8,9,0,0,4,4,0,13,0,4,0,10,6,6,0,2,0,6,0,8,5,5,8,8,2,12,3,3,2,2,5,6,7,9,0,12,6,7,4,5,4,11,3,4,0,9,4,6,2,3,1,1,0,7,2,4,5,7,6,10,7,8,1,12,0,11,3,5,0,5,7,7,4,7,3,11,0,3,3,6,1,9,2,5,1,4,1,2,2,6,1,3,1,5,1,6,1,8,1,7,});
sparse_graph3_ints[34][30] = vector<int>({10,10,0,0,0,10,0,4,0,15,4,4,7,7,0,2,0,12,0,6,6,6,4,13,0,8,5,5,3,3,8,8,2,2,9,10,6,7,4,5,6,12,0,14,4,10,5,6,2,14,7,8,0,11,3,4,4,6,6,8,8,11,0,9,9,9,5,7,5,12,2,3,1,1,0,7,0,13,3,13,2,10,2,4,3,5,8,10,5,8,1,14,4,7,6,9,0,5,8,9,});
sparse_graph3_ints[34][35] = vector<int>({0,12,0,0,11,12,3,3,0,17,5,5,9,9,1,1,7,7,5,15,0,8,0,15,1,3,0,10,0,6,3,13,6,6,4,4,8,8,0,13,7,14,1,17,11,11,5,11,9,10,6,7,5,6,7,8,0,11,4,5,0,9,0,3,0,5,3,4,0,14,3,11,5,7,8,9,3,16,0,7,6,14,2,3,3,5,10,12,4,6,6,8,7,9,5,13,10,10,1,11,});
sparse_graph3_ints[34][40] = vector<int>({13,13,0,10,0,0,0,20,9,9,0,4,0,16,4,4,7,7,8,16,0,14,0,2,11,11,0,6,6,14,6,6,0,12,4,18,4,12,0,8,5,5,0,18,8,8,3,3,10,10,4,14,2,2,2,12,6,7,5,6,7,8,8,9,12,14,6,17,10,11,6,12,9,10,9,15,5,15,4,5,2,10,10,12,2,16,2,19,7,16,12,13,6,8,2,14,8,10,11,12,});
sparse_graph3_ints[34][45] = vector<int>({15,15,0,12,0,0,0,22,3,3,11,11,0,16,5,5,7,19,9,9,1,1,7,7,5,17,0,8,0,18,1,3,13,13,0,14,0,10,7,13,11,17,0,20,3,13,6,6,4,4,3,21,9,15,8,8,0,6,5,13,10,10,3,17,12,12,3,11,7,15,3,15,9,18,5,20,0,3,0,5,5,6,3,4,4,5,0,11,0,9,6,7,7,8,8,9,5,15,9,10,});
sparse_graph3_ints[34][50] = vector<int>({10,10,0,0,16,17,4,4,0,25,0,14,0,4,8,16,6,6,10,20,4,16,8,8,0,22,14,14,0,10,0,2,0,16,6,22,12,12,0,6,0,20,0,12,4,20,0,18,4,14,0,8,5,5,7,7,8,18,9,9,4,12,4,18,11,11,3,3,12,19,13,13,7,15,15,15,4,23,10,18,1,2,0,1,4,10,6,14,2,2,10,16,8,21,6,20,6,18,0,23,});
sparse_graph3_ints[34][55] = vector<int>({18,18,0,0,10,10,0,27,3,3,6,6,14,14,0,22,0,14,0,18,7,21,1,1,8,8,13,21,0,8,12,12,5,19,1,3,5,25,16,16,0,12,7,15,0,25,5,5,5,13,9,23,9,17,0,20,0,16,11,19,0,10,7,7,4,4,5,15,9,9,11,11,4,23,0,6,3,13,13,13,4,18,1,23,15,15,6,20,8,16,11,17,3,11,0,1,0,3,0,5,});
sparse_graph3_ints[34][60] = vector<int>({0,14,20,20,0,30,0,4,0,22,11,11,0,0,0,10,12,24,16,17,8,18,0,18,0,26,9,9,4,4,6,24,7,7,13,13,0,2,11,19,0,6,4,18,8,26,6,6,4,22,15,15,0,16,0,12,16,22,5,15,4,28,18,18,10,22,8,8,0,20,5,5,0,24,8,16,0,8,0,28,10,10,4,14,14,20,8,20,4,20,12,12,14,14,4,12,4,24,2,3,});
sparse_graph3_ints[34][70] = vector<int>({6,32,0,0,0,10,12,12,23,23,0,28,0,20,11,21,2,4,9,9,19,19,14,28,0,24,7,7,0,32,0,16,14,14,0,2,5,27,5,22,17,17,0,4,0,35,6,16,8,28,0,8,14,22,21,21,0,14,6,6,18,26,10,11,9,17,15,16,16,24,0,18,0,12,6,24,10,30,0,22,8,8,10,19,10,24,5,19,12,26,5,5,5,14,0,26,0,30,4,30,});
sparse_graph3_ints[34][80] = vector<int>({26,27,8,8,9,35,0,0,5,28,4,4,14,14,0,22,18,18,16,26,0,36,0,4,11,11,0,27,6,6,0,18,0,40,20,30,10,29,10,20,0,14,24,24,20,20,0,2,12,22,6,18,14,33,16,16,0,30,0,10,9,10,0,6,22,22,6,34,0,33,6,21,15,23,18,28,12,13,5,31,0,24,12,31,0,16,5,16,5,25,10,26,7,7,4,38,0,12,0,20,});
sparse_graph3_ints[34][100] = vector<int>({28,28,7,31,8,46,12,12,7,7,0,6,0,0,23,23,13,36,22,39,10,22,0,43,33,33,0,19,0,26,19,19,4,6,14,30,0,50,0,30,21,32,0,39,0,2,9,9,16,42,1,22,8,42,10,17,4,30,25,26,30,31,17,17,7,35,0,16,14,26,16,38,21,21,24,34,0,4,15,15,18,30,1,46,12,24,0,36,7,39,28,36,0,12,10,11,0,8,6,23,});
sparse_graph3_ints[32][6] = vector<int>({2,2,0,0,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[32][8] = vector<int>({2,3,0,0,0,4,0,2,2,2,1,3,0,3,0,1,1,2,1,1,});
sparse_graph3_ints[32][10] = vector<int>({3,3,0,0,0,2,0,5,2,4,0,4,1,1,2,3,1,4,2,2,0,1,0,3,1,3,1,2,});
sparse_graph3_ints[32][12] = vector<int>({4,4,0,0,0,2,0,6,2,2,3,4,0,4,2,5,1,1,3,3,0,3,1,5,2,4,1,2,0,5,2,3,0,1,1,3,1,4,});
sparse_graph3_ints[32][14] = vector<int>({4,5,0,0,0,4,0,7,0,2,2,2,4,4,0,6,3,5,2,3,1,1,2,6,0,3,3,4,0,5,1,6,1,2,3,3,1,3,2,5,2,4,1,4,0,1,1,5,});
sparse_graph3_ints[32][16] = vector<int>({5,5,0,0,0,4,0,8,0,2,3,3,4,6,0,6,2,2,4,5,1,1,2,3,2,4,0,3,4,4,0,5,1,7,3,6,1,2,0,7,1,4,3,4,2,7,1,3,3,5,2,6,1,5,0,1,2,5,1,6,});
sparse_graph3_ints[32][18] = vector<int>({0,0,6,6,2,2,0,9,0,6,0,2,4,4,0,4,3,3,5,6,0,8,3,4,1,1,2,3,5,5,2,8,4,7,2,4,0,5,0,3,0,7,4,6,1,8,4,5,1,2,1,4,2,5,1,3,3,5,1,5,3,7,1,6,3,6,2,6,1,7,2,7,0,1,});
sparse_graph3_ints[32][20] = vector<int>({6,7,0,0,2,2,0,10,4,4,0,6,0,2,0,8,0,4,3,3,6,6,4,5,4,8,3,4,1,1,2,3,5,7,5,5,1,9,0,7,2,4,0,5,0,3,3,5,5,6,2,9,1,2,1,4,2,5,0,9,4,6,1,3,2,6,1,5,1,6,3,8,3,6,4,7,1,7,2,8,0,1,1,8,3,7,2,7,});
sparse_graph3_ints[32][23] = vector<int>({7,8,0,0,3,3,0,11,5,5,1,1,0,8,1,3,0,6,4,4,3,10,5,6,7,7,0,9,1,11,0,3,0,5,3,4,4,5,0,7,2,3,6,6,6,8,3,5,0,10,5,9,4,6,0,4,0,2,2,2,2,4,1,2,0,1,1,7,1,5,2,10,6,7,3,6,1,9,5,7,1,4,2,5,3,7,4,9,4,7,1,6,2,6,1,8,3,9,5,8,});
sparse_graph3_ints[32][26] = vector<int>({8,9,0,0,4,4,0,13,0,4,0,10,6,6,0,2,0,6,0,8,4,11,5,5,3,3,2,2,6,7,8,8,0,11,2,12,4,5,5,6,7,7,0,9,0,12,6,10,4,8,3,4,4,6,3,11,7,9,0,7,2,4,0,5,1,2,2,3,0,1,0,3,3,5,5,7,2,8,2,10,5,10,7,8,6,8,4,7,3,6,4,10,1,12,1,1,5,8,1,9,});
sparse_graph3_ints[32][30] = vector<int>({6,6,0,0,10,10,0,4,0,15,4,4,0,10,8,8,0,2,0,12,0,6,4,13,0,8,4,10,5,5,3,3,7,7,6,12,2,2,2,10,0,13,2,14,4,5,5,6,6,7,9,10,7,8,0,1,1,2,0,9,3,4,2,3,0,7,4,6,0,11,3,11,0,14,0,5,4,8,5,12,8,9,6,8,2,8,3,5,2,4,0,3,5,7,3,13,8,11,5,10,});
sparse_graph3_ints[32][35] = vector<int>({0,13,0,0,11,12,0,6,0,17,5,5,9,9,1,1,7,7,0,10,7,14,2,2,0,15,0,8,5,13,4,4,6,6,3,11,3,16,3,3,8,8,0,11,10,10,5,11,0,3,0,5,3,9,3,13,0,1,5,15,0,9,0,7,4,5,3,4,5,6,0,12,0,4,0,14,6,7,1,17,0,16,7,8,7,11,9,13,11,11,9,10,8,9,4,10,2,3,4,13,});
sparse_graph3_ints[32][40] = vector<int>({0,0,13,13,0,12,0,20,0,4,9,9,0,8,0,16,7,7,8,16,0,2,5,5,6,14,0,6,11,11,4,12,0,10,4,18,0,14,4,4,0,18,6,6,8,8,4,14,3,3,10,10,4,10,0,1,1,2,4,16,8,14,10,15,2,2,6,12,12,12,2,10,6,17,0,5,0,9,0,7,4,5,3,4,5,6,2,16,2,14,2,12,6,7,0,11,2,3,2,19,});
sparse_graph3_ints[32][45] = vector<int>({0,0,15,15,0,14,3,3,11,11,7,19,5,5,0,18,0,22,7,7,9,9,1,1,5,17,0,8,1,3,0,12,13,13,5,13,11,17,0,10,0,16,0,20,6,6,9,15,4,4,8,8,3,21,7,13,3,11,3,13,3,17,10,10,0,7,0,5,12,12,0,1,0,3,8,14,3,15,3,19,7,17,3,9,5,15,5,11,0,9,9,18,0,6,0,11,5,6,4,5,});
sparse_graph3_ints[32][50] = vector<int>({0,0,0,18,16,17,5,5,0,25,2,3,9,9,13,13,0,14,10,20,7,7,0,22,11,11,1,2,0,8,8,16,0,12,6,20,6,14,0,16,0,20,0,10,0,1,15,15,6,6,0,6,8,8,4,23,3,19,11,17,4,4,4,14,10,10,3,12,6,16,13,18,12,12,2,22,0,2,0,4,7,21,3,17,14,14,3,4,10,16,5,19,5,12,3,10,8,18,8,14,});
sparse_graph3_ints[32][55] = vector<int>({18,18,0,13,0,27,0,0,10,10,0,5,0,21,14,14,0,9,11,22,0,17,6,20,8,8,12,12,0,3,6,6,7,15,0,7,7,24,0,23,16,17,0,15,0,11,5,14,12,19,0,19,10,16,7,7,5,5,0,1,0,25,8,21,9,9,7,17,4,23,4,4,15,20,15,15,3,26,4,13,3,19,11,11,10,18,3,3,1,1,3,21,3,17,1,3,4,11,13,13,});
sparse_graph3_ints[32][60] = vector<int>({0,0,20,20,0,30,0,14,0,22,3,4,15,15,10,12,8,26,0,3,0,18,0,10,0,26,8,21,9,9,13,21,6,7,13,13,2,3,7,19,18,18,5,15,5,25,7,8,0,16,4,5,0,12,8,16,16,22,0,1,0,20,16,17,12,24,0,24,5,6,4,28,4,18,11,17,12,19,4,23,0,28,9,23,10,10,0,8,4,14,4,20,4,12,12,12,0,6,0,2,});
sparse_graph3_ints[32][70] = vector<int>({8,8,2,4,14,28,0,35,14,14,0,0,0,24,19,19,5,27,11,11,23,23,5,5,0,20,0,13,0,2,16,16,5,23,0,17,6,32,14,22,0,28,0,4,0,32,6,17,8,28,0,8,21,21,11,19,12,13,18,26,9,9,6,6,16,24,0,15,12,21,9,18,10,30,4,15,7,7,17,18,0,22,12,26,0,11,5,20,0,26,9,25,10,10,4,30,8,23,0,30,});
sparse_graph3_ints[32][80] = vector<int>({20,20,6,6,0,0,26,27,8,21,5,32,0,21,9,9,17,31,0,4,0,40,12,13,17,17,4,4,0,31,0,25,0,15,12,24,15,28,24,24,6,37,0,2,10,11,0,28,6,28,12,34,14,14,22,22,0,10,0,6,0,37,1,18,0,34,10,32,15,25,11,22,7,19,15,16,22,29,7,16,10,28,19,27,0,13,8,8,18,19,5,26,0,17,5,35,0,23,7,7,});
sparse_graph3_ints[32][100] = vector<int>({17,38,33,33,11,12,0,50,7,8,16,28,0,6,0,0,28,28,0,30,0,44,10,26,0,37,5,5,0,20,26,37,17,17,0,26,8,46,8,39,13,14,25,26,8,35,23,23,19,19,9,10,2,32,21,32,0,2,20,40,30,31,7,22,7,32,24,34,14,33,0,12,1,40,9,42,0,17,21,21,0,23,14,43,14,26,7,25,0,4,15,16,15,36,2,46,20,29,0,8,});
sparse_graph3_ints[30][6] = vector<int>({2,2,0,0,0,3,1,2,0,2,1,1,0,1,});
sparse_graph3_ints[30][8] = vector<int>({2,3,0,0,0,2,0,4,2,2,1,3,0,1,0,3,1,2,1,1,});
sparse_graph3_ints[30][10] = vector<int>({3,3,0,0,0,2,0,4,2,4,0,5,1,1,2,3,2,2,1,4,0,3,0,1,1,2,1,3,});
sparse_graph3_ints[30][12] = vector<int>({4,4,0,0,0,2,0,6,2,2,0,4,3,4,1,2,0,1,0,3,2,5,3,3,1,1,0,5,2,4,1,5,2,3,1,3,1,4,});
sparse_graph3_ints[30][14] = vector<int>({4,5,0,0,0,4,0,7,0,2,2,2,4,4,0,6,3,3,3,5,2,3,1,2,0,1,0,3,2,6,0,5,1,1,1,3,3,4,1,6,2,4,1,4,2,5,1,5,});
sparse_graph3_ints[30][16] = vector<int>({5,5,0,0,0,4,0,8,0,2,3,3,0,6,2,2,4,6,4,4,2,3,1,2,0,1,0,3,0,5,1,1,2,4,4,5,3,4,1,7,1,3,3,6,0,7,1,4,2,7,3,5,1,5,2,6,2,5,1,6,});
sparse_graph3_ints[30][18] = vector<int>({6,6,0,0,0,6,0,2,0,9,0,4,4,4,3,3,2,2,0,8,2,8,5,6,3,4,0,1,1,2,2,3,4,5,0,7,0,5,0,3,2,4,4,7,5,5,1,1,3,5,1,3,1,4,2,5,1,8,4,6,3,7,2,6,1,5,1,6,3,6,1,7,2,7,});
sparse_graph3_ints[30][20] = vector<int>({0,0,6,7,0,4,0,10,4,4,0,2,0,6,0,8,3,3,5,5,2,2,2,9,6,6,4,5,4,8,0,1,1,2,3,4,2,3,0,7,0,5,0,3,2,4,3,5,4,6,5,7,1,1,2,6,5,6,3,8,0,9,1,9,3,6,1,3,2,5,1,4,1,7,1,5,1,6,2,8,4,7,2,7,1,8,3,7,});
sparse_graph3_ints[30][23] = vector<int>({0,0,7,8,0,6,0,11,5,5,1,1,0,4,0,9,3,3,3,10,4,4,6,6,0,8,2,2,0,3,0,1,0,7,0,5,3,4,1,11,0,10,4,5,5,9,5,6,7,7,3,7,2,3,3,5,1,2,0,2,1,3,1,9,1,7,5,7,2,4,3,9,4,6,6,8,4,9,6,7,2,10,4,7,3,6,1,5,2,8,2,5,1,4,2,6,2,7,3,8,});
sparse_graph3_ints[30][26] = vector<int>({0,0,8,9,4,4,0,13,0,4,6,6,0,10,0,2,0,6,0,8,4,11,5,5,3,3,7,7,2,10,2,2,0,1,1,2,2,8,0,12,2,12,6,10,4,8,4,5,0,7,0,5,3,4,0,9,0,11,5,6,6,7,8,8,2,3,4,10,4,6,0,3,2,6,2,4,3,9,6,8,3,11,3,8,5,10,5,7,7,9,7,8,3,5,4,9,1,1,3,6,});
sparse_graph3_ints[30][30] = vector<int>({10,10,0,0,0,6,0,15,6,6,1,2,0,10,8,8,4,4,0,12,6,12,0,8,4,10,0,1,0,4,5,5,3,3,7,7,4,13,0,2,0,13,2,10,2,2,2,8,2,14,4,11,0,5,0,7,4,5,3,4,0,9,4,8,0,11,0,14,8,9,5,6,2,3,2,12,6,7,7,8,6,10,8,11,3,10,4,12,6,8,4,6,2,9,9,9,2,6,0,3,});
sparse_graph3_ints[30][35] = vector<int>({0,15,3,3,0,0,11,12,8,8,0,9,1,1,6,6,7,14,0,17,0,11,2,2,3,11,5,13,5,5,0,7,10,10,4,4,0,13,3,16,7,7,9,9,3,13,0,5,0,1,0,3,3,9,6,11,0,8,0,10,5,15,9,13,0,6,0,12,7,12,0,14,4,11,3,4,4,5,0,16,5,6,1,17,0,4,3,14,2,8,3,10,6,7,7,8,2,3,5,11,});
sparse_graph3_ints[30][40] = vector<int>({0,8,13,13,0,0,0,20,9,9,0,12,1,2,0,16,7,7,8,16,2,3,5,5,11,11,7,13,0,14,4,18,0,10,5,11,0,1,0,18,0,6,6,6,4,4,4,14,4,16,8,8,4,10,10,10,0,2,0,4,8,14,10,15,3,3,12,12,3,9,2,14,6,12,0,7,0,9,2,16,6,17,4,5,0,11,3,12,0,13,5,6,3,4,2,19,6,15,6,7,});
sparse_graph3_ints[30][45] = vector<int>({0,7,0,16,15,15,0,0,11,11,7,19,0,12,0,20,0,3,9,9,5,5,7,7,0,5,13,13,3,21,8,15,6,13,0,14,0,10,3,17,6,6,0,1,4,4,8,8,0,22,0,18,12,16,3,13,3,19,10,10,3,3,0,9,1,1,10,15,3,15,1,3,3,10,12,12,7,17,8,13,4,11,9,18,5,17,6,11,0,8,0,11,5,15,5,6,4,5,5,20,});
sparse_graph3_ints[30][50] = vector<int>({3,4,0,0,16,17,0,25,10,10,0,14,0,20,0,10,1,2,10,20,6,18,14,14,7,7,12,12,5,12,2,3,6,22,0,18,0,12,6,6,10,16,0,23,0,16,0,1,0,8,5,5,8,8,3,17,8,15,13,18,6,13,6,20,9,9,11,11,4,18,10,18,0,6,0,2,3,12,7,14,0,4,15,15,4,5,1,21,3,23,13,13,3,10,3,15,3,21,2,9,});
sparse_graph3_ints[30][55] = vector<int>({5,25,3,4,0,0,12,12,18,18,8,8,0,18,0,24,15,15,0,10,0,3,0,14,12,21,10,10,5,19,6,6,2,3,11,17,6,15,1,27,0,20,9,21,4,13,0,12,5,21,16,17,7,7,0,8,0,1,0,22,13,14,0,16,9,16,5,5,9,9,4,15,5,23,11,11,9,23,15,20,13,18,1,25,4,18,1,1,8,19,0,6,4,11,8,15,1,3,4,5,});
sparse_graph3_ints[30][60] = vector<int>({15,15,6,7,0,0,20,20,8,26,0,24,11,11,3,4,0,16,0,30,0,20,1,2,8,21,0,10,13,21,13,13,8,8,5,18,5,14,0,5,18,18,0,14,5,25,8,18,0,26,0,12,16,22,4,5,16,17,12,24,0,18,0,1,9,9,0,22,4,28,12,19,4,23,5,6,9,23,5,16,10,14,0,28,11,17,10,10,4,21,0,8,4,13,4,11,0,2,2,2,});
sparse_graph3_ints[30][70] = vector<int>({14,28,10,14,0,6,0,35,0,0,8,8,0,29,23,23,0,19,15,15,5,5,0,23,20,20,8,28,0,13,14,22,10,10,0,2,5,23,17,17,0,4,6,32,0,32,5,27,7,19,0,8,18,26,0,15,13,14,16,24,10,24,6,6,0,17,0,25,10,19,10,30,6,15,12,26,13,19,6,12,18,19,0,21,8,23,7,7,21,22,4,30,5,18,0,11,0,27,11,11,});
sparse_graph3_ints[30][80] = vector<int>({16,16,0,40,0,25,26,27,0,4,0,18,16,32,0,34,12,12,0,0,10,31,19,19,4,4,6,25,6,6,22,22,10,10,12,24,14,14,0,14,10,35,16,26,0,28,8,9,21,29,0,2,6,28,8,18,24,24,6,16,0,37,0,10,0,31,0,6,10,28,19,27,5,37,6,22,17,18,0,20,5,34,11,22,1,22,11,19,14,30,0,16,20,21,15,24,5,31,7,8,});
sparse_graph3_ints[30][100] = vector<int>({18,19,0,8,8,8,33,33,8,42,28,28,8,35,13,13,22,25,22,39,0,0,9,23,0,50,0,26,14,43,10,10,0,43,9,27,1,32,0,13,15,31,2,3,15,15,7,32,0,21,13,36,30,31,19,37,7,46,0,36,16,24,1,46,20,30,1,39,1,17,28,36,7,39,2,28,21,22,25,34,5,6,7,21,0,2,11,12,22,32,26,26,13,29,16,18,14,39,4,20,});
sparse_graph3_ints[27][6] = vector<int>({2,2,0,0,0,2,0,3,1,2,0,1,1,1,});
sparse_graph3_ints[27][8] = vector<int>({2,3,0,0,0,2,0,4,2,2,0,1,1,3,1,1,0,3,1,2,});
sparse_graph3_ints[27][10] = vector<int>({3,3,0,0,0,2,0,5,2,2,0,4,2,4,0,1,0,3,1,1,2,3,1,2,1,4,1,3,});
sparse_graph3_ints[27][12] = vector<int>({4,4,0,0,0,2,0,6,2,2,0,4,0,1,0,3,3,4,2,3,1,1,1,2,3,3,2,5,0,5,1,3,2,4,1,5,1,4,});
sparse_graph3_ints[27][14] = vector<int>({4,5,0,0,0,4,0,2,0,7,3,3,2,2,0,6,0,1,4,4,1,2,0,5,2,6,2,3,0,3,2,4,1,1,3,5,3,4,1,3,1,4,1,6,1,5,2,5,});
sparse_graph3_ints[27][16] = vector<int>({5,5,0,0,0,2,0,8,3,3,0,4,0,6,4,4,2,2,0,1,1,2,2,7,0,5,4,6,3,4,2,3,0,3,2,4,0,7,1,1,4,5,2,6,3,6,2,5,1,4,1,3,1,7,1,5,3,5,1,6,});
sparse_graph3_ints[27][18] = vector<int>({0,0,6,6,0,4,0,9,4,4,0,2,0,6,3,3,2,2,2,8,0,7,0,1,1,2,4,5,0,8,0,5,3,4,2,3,4,7,2,6,5,5,0,3,2,4,1,1,5,6,3,7,3,5,4,6,2,5,3,6,1,8,1,6,1,4,1,7,1,5,1,3,2,7,});
sparse_graph3_ints[27][20] = vector<int>({0,0,6,7,0,4,0,10,4,4,0,2,0,6,0,8,5,5,3,3,4,8,2,2,0,1,1,2,2,9,0,7,0,5,3,4,4,5,2,3,2,6,2,8,6,6,0,9,4,6,2,7,0,3,2,4,5,6,1,1,3,8,3,5,3,6,2,5,5,7,3,7,1,7,1,8,1,9,1,6,1,4,1,5,4,7,1,3,});
sparse_graph3_ints[27][23] = vector<int>({7,8,0,0,0,8,2,2,5,5,3,10,1,1,0,6,0,10,4,4,6,6,3,3,1,11,0,1,0,3,0,5,3,7,5,9,0,7,0,9,0,11,0,4,3,4,3,9,4,5,2,3,1,9,5,6,1,7,7,7,5,7,2,9,2,7,6,7,3,5,4,7,1,2,4,9,2,8,4,8,1,5,0,2,4,6,2,5,3,6,3,8,2,10,2,6,1,3,6,8,});
sparse_graph3_ints[27][26] = vector<int>({8,9,0,6,0,0,0,13,5,5,0,2,0,10,2,2,7,7,0,8,4,11,4,4,2,10,6,6,3,3,0,1,1,2,0,5,2,8,6,10,0,12,2,12,0,7,4,8,0,4,0,9,4,5,3,4,0,11,5,9,2,3,5,6,3,10,6,7,7,8,2,6,2,9,2,7,8,8,4,10,3,8,5,8,4,6,0,3,3,9,6,8,5,10,2,11,1,1,3,7,});
sparse_graph3_ints[27][30] = vector<int>({0,0,10,10,6,6,2,2,0,15,4,4,0,10,0,2,8,8,0,12,0,6,6,12,4,10,0,8,5,5,0,1,7,7,3,3,4,13,1,2,2,12,2,10,0,5,6,10,2,8,9,9,0,7,0,13,2,14,0,9,4,5,3,4,0,11,4,8,5,6,0,14,8,11,0,4,4,12,6,7,2,3,7,8,4,9,2,9,5,10,8,9,2,11,3,12,6,11,2,6,});
sparse_graph3_ints[27][35] = vector<int>({11,12,0,8,0,0,1,17,8,8,0,12,0,3,6,6,0,5,3,13,5,15,0,10,10,10,5,5,4,10,0,1,0,14,7,11,4,4,3,3,0,16,7,7,1,1,9,13,0,7,1,3,3,9,4,14,5,11,9,9,7,14,0,9,5,6,4,5,0,11,6,13,3,16,0,6,0,13,3,4,3,12,0,15,6,7,2,15,8,12,2,11,2,8,7,8,5,12,8,9,});
sparse_graph3_ints[27][40] = vector<int>({13,13,0,18,0,0,0,12,0,4,9,9,0,8,8,16,7,7,0,2,5,5,11,11,4,14,3,3,0,14,4,18,0,10,0,20,6,6,8,13,3,13,8,8,0,1,4,4,1,2,1,16,5,16,10,10,5,10,0,7,2,3,10,15,4,9,7,12,12,12,6,11,3,16,0,15,4,5,1,10,3,11,9,14,0,9,6,14,5,6,2,18,6,17,0,6,0,13,7,15,});
sparse_graph3_ints[27][45] = vector<int>({7,7,0,22,11,11,0,0,4,4,15,15,0,17,1,3,6,19,9,9,0,10,0,13,5,15,1,1,13,13,3,3,3,13,0,20,6,6,9,18,6,16,10,15,5,5,8,8,0,15,12,16,0,7,10,10,0,1,3,21,1,18,0,3,1,11,0,5,3,15,7,12,3,18,12,12,8,13,5,10,7,17,0,8,9,14,6,11,4,19,4,9,4,7,4,5,2,12,0,9,});
sparse_graph3_ints[27][50] = vector<int>({0,25,0,10,10,10,16,17,0,0,0,17,0,6,12,14,0,21,10,20,8,8,0,14,0,2,6,6,5,17,2,2,6,22,0,8,9,14,15,15,0,12,4,21,6,13,11,17,0,19,7,7,4,5,2,14,0,23,13,18,9,16,9,9,0,1,8,19,12,12,4,16,3,4,1,2,6,18,2,3,4,11,11,11,5,6,5,12,7,14,3,10,14,14,3,20,3,23,3,15,});
sparse_graph3_ints[27][55] = vector<int>({18,18,0,20,3,4,8,8,0,0,9,23,12,12,0,12,1,27,15,15,7,20,0,3,0,24,0,16,10,10,5,6,5,19,2,3,13,14,0,14,5,25,16,17,4,5,10,20,0,18,7,14,7,7,15,20,0,22,9,16,0,10,0,1,4,15,11,17,5,23,9,9,12,21,4,13,4,18,1,25,13,18,1,1,0,8,11,11,1,3,4,11,3,22,8,15,0,6,6,6,});
sparse_graph3_ints[27][60] = vector<int>({20,20,0,0,14,15,0,5,10,10,5,23,0,30,7,8,0,18,4,5,12,21,0,12,0,23,12,12,8,26,1,2,5,16,0,26,18,18,5,12,7,17,0,14,12,24,4,26,16,17,3,4,10,18,16,22,0,20,5,6,9,21,0,16,11,15,0,1,6,7,4,28,13,18,0,10,3,22,8,24,5,14,0,28,9,13,15,20,2,20,7,19,9,9,0,2,0,8,2,2,});
sparse_graph3_ints[27][70] = vector<int>({0,27,10,11,16,17,0,0,16,27,7,8,4,4,8,31,0,18,6,19,0,4,23,23,0,35,0,12,13,14,6,24,0,21,12,26,13,22,0,32,9,9,12,12,0,2,21,21,10,19,18,18,0,10,0,16,0,6,8,28,15,15,1,29,11,21,19,20,0,23,0,14,12,29,4,30,5,22,9,25,17,24,5,15,7,16,4,33,5,27,20,25,0,25,6,7,15,23,5,6,});
sparse_graph3_ints[27][80] = vector<int>({9,9,21,22,15,16,26,27,6,6,6,27,7,16,12,30,0,4,0,35,0,0,18,19,4,4,20,30,10,35,0,16,0,29,4,38,0,26,9,28,0,21,12,23,16,25,14,14,0,32,7,19,15,32,10,11,7,8,9,20,0,10,1,37,0,2,24,25,2,17,17,17,17,27,12,13,6,32,20,20,10,22,1,23,7,34,0,6,14,24,23,23,0,40,0,14,19,28,5,25,});
sparse_graph3_ints[27][100] = vector<int>({10,45,0,29,22,22,6,38,22,39,33,33,1,6,24,29,15,21,12,12,2,17,0,50,16,32,0,14,6,31,0,3,0,46,5,6,12,38,16,16,0,0,14,14,0,38,24,25,20,20,7,34,7,27,25,34,5,22,16,26,6,44,18,34,8,9,28,36,0,32,28,29,8,23,14,40,20,36,13,31,0,42,0,24,6,41,16,42,19,27,30,31,3,19,0,35,12,27,10,11,});
sparse_graph3_ints[23][6] = vector<int>({2,2,0,0,0,1,0,3,0,2,1,2,1,1,});
sparse_graph3_ints[23][8] = vector<int>({2,3,0,0,0,2,0,4,0,1,2,2,1,1,0,3,1,2,1,3,});
sparse_graph3_ints[23][10] = vector<int>({3,3,0,0,0,2,0,5,2,2,0,1,0,4,0,3,2,4,1,2,1,1,2,3,1,4,1,3,});
sparse_graph3_ints[23][12] = vector<int>({4,4,0,0,0,2,0,6,2,2,0,4,0,1,3,3,1,2,0,5,2,5,2,3,0,3,1,1,3,4,2,4,1,4,1,3,1,5,});
sparse_graph3_ints[23][14] = vector<int>({0,0,4,5,0,4,0,2,0,7,3,3,2,2,0,1,0,6,1,2,2,6,0,5,3,4,2,3,4,4,2,4,0,3,1,1,1,5,3,5,1,4,1,6,1,3,2,5,});
sparse_graph3_ints[23][16] = vector<int>({5,5,0,0,1,2,0,8,3,3,0,1,0,6,0,4,4,4,0,2,2,7,2,2,0,5,0,7,3,4,2,3,4,6,2,6,2,4,4,5,0,3,1,6,1,1,1,5,3,5,3,6,2,5,1,7,1,4,1,3,});
sparse_graph3_ints[23][18] = vector<int>({6,6,0,0,2,2,0,9,4,4,0,2,0,7,0,5,3,3,0,1,2,8,1,2,5,5,0,6,0,8,0,4,2,6,4,7,3,4,2,3,4,5,2,7,4,6,2,4,1,6,0,3,1,1,3,6,5,6,3,7,1,7,3,5,1,5,2,5,1,4,1,8,1,3,});
sparse_graph3_ints[23][20] = vector<int>({6,7,0,0,2,2,0,10,4,4,0,2,0,6,0,8,3,7,3,3,0,1,4,8,1,2,0,5,5,6,0,7,4,5,2,9,0,4,3,4,0,9,2,6,2,3,5,5,6,6,2,8,3,6,1,8,4,6,4,7,1,6,2,4,1,7,0,3,1,1,3,8,5,7,2,7,3,5,1,5,2,5,1,9,1,4,1,3,});
sparse_graph3_ints[23][23] = vector<int>({0,0,0,8,7,8,0,3,0,5,5,6,3,10,4,4,0,10,0,1,3,3,1,1,0,7,1,11,2,2,3,7,6,7,5,9,2,9,0,6,4,5,0,9,4,8,3,4,5,5,0,11,6,6,0,4,2,6,2,3,7,7,2,7,2,8,3,9,4,7,5,8,5,7,1,9,1,5,6,8,1,7,2,5,4,9,3,8,4,6,3,5,2,10,3,6,1,2,0,2,});
sparse_graph3_ints[23][26] = vector<int>({4,4,0,0,0,13,8,9,1,2,0,10,6,7,2,3,0,8,0,1,5,6,0,6,2,10,6,10,0,2,0,4,2,12,4,5,2,8,3,3,5,9,0,7,7,8,0,11,4,11,4,8,0,9,3,4,5,5,0,12,6,6,2,9,7,7,2,11,0,5,2,2,8,8,4,10,2,6,2,7,5,8,6,9,3,10,4,7,3,7,6,8,3,9,7,9,5,10,4,9,});
sparse_graph3_ints[23][30] = vector<int>({6,6,10,10,0,0,0,15,0,4,4,4,6,10,0,2,2,8,0,13,3,3,4,13,0,11,7,7,0,8,4,10,0,1,1,2,0,7,9,9,2,3,4,5,6,12,2,10,5,6,8,8,0,9,2,12,0,6,2,14,5,5,6,7,3,4,0,10,0,12,8,11,4,11,0,14,7,8,4,12,2,11,4,8,6,11,2,9,8,9,4,9,2,13,8,10,6,9,0,5,});
sparse_graph3_ints[23][35] = vector<int>({2,3,11,12,0,0,0,16,7,8,0,12,0,8,5,6,9,10,0,3,7,14,4,14,1,13,3,4,1,9,5,11,0,1,6,7,4,5,1,17,1,1,0,6,1,3,8,9,4,10,3,9,3,16,10,11,7,11,0,11,3,13,5,5,4,4,0,15,0,9,9,13,8,12,7,7,5,15,2,15,2,11,6,6,6,13,3,12,5,12,8,8,3,3,0,7,9,9,0,13,});
sparse_graph3_ints[23][40] = vector<int>({1,2,8,8,13,13,4,18,3,4,0,13,0,0,0,10,7,12,0,18,6,6,11,11,3,14,2,3,10,15,4,5,0,20,0,1,7,15,1,14,0,8,7,7,0,2,9,9,2,2,2,10,0,6,0,16,3,17,5,5,3,11,10,10,4,12,6,14,12,12,2,16,5,6,6,11,8,13,5,8,8,16,4,4,5,10,0,11,9,14,0,9,2,19,5,13,6,17,5,16,});
sparse_graph3_ints[23][45] = vector<int>({0,19,1,11,11,11,0,7,15,15,0,3,6,19,8,9,0,0,1,15,5,5,0,22,6,16,13,13,0,5,6,7,7,12,12,16,0,9,10,15,3,11,0,1,9,18,3,15,1,20,1,1,7,8,4,4,1,3,3,21,3,3,8,13,5,11,5,6,10,10,5,15,7,17,0,17,0,10,0,13,9,14,12,12,4,19,2,13,4,13,6,6,9,9,2,18,7,7,0,11,});
sparse_graph3_ints[23][50] = vector<int>({1,2,5,6,10,20,9,9,13,13,4,19,3,4,0,17,0,25,16,17,0,0,0,14,11,11,6,14,7,7,0,20,4,23,0,12,15,15,7,19,1,4,0,22,10,14,0,10,0,1,13,18,4,5,5,12,4,17,10,18,8,8,0,2,0,8,4,21,10,16,2,2,7,21,4,11,7,15,2,4,10,10,2,14,7,17,3,10,1,18,6,6,3,15,1,23,13,15,7,12,});
sparse_graph3_ints[23][55] = vector<int>({15,15,3,4,8,15,1,27,8,8,11,22,18,18,0,3,7,20,0,10,0,20,0,24,0,14,0,0,10,10,6,7,13,14,4,15,5,25,0,16,16,17,10,20,4,20,2,3,12,12,0,12,15,20,0,22,4,5,13,19,10,16,0,1,0,18,12,17,4,18,5,23,5,6,4,13,8,23,0,26,1,1,9,9,7,14,1,3,4,11,0,8,3,3,4,22,11,11,7,18,});
sparse_graph3_ints[23][60] = vector<int>({0,0,0,6,0,21,10,25,16,17,8,8,20,20,13,14,2,29,0,13,1,26,0,17,5,5,9,13,4,20,0,2,8,19,15,15,16,22,5,25,11,22,18,18,2,2,6,12,0,8,9,16,9,21,0,15,6,27,0,19,10,10,6,6,15,20,12,12,0,23,5,15,5,23,0,11,13,19,13,23,0,25,6,8,4,18,11,18,0,1,8,24,1,28,4,22,1,2,1,4,});
sparse_graph3_ints[23][70] = vector<int>({20,20,0,0,6,29,11,11,2,3,7,7,23,23,10,19,14,21,0,21,0,30,9,9,14,25,1,10,17,26,4,25,0,15,0,26,1,32,5,27,8,18,15,15,0,7,5,32,0,35,1,23,13,19,0,2,13,14,18,19,13,28,9,25,7,13,21,22,0,17,4,5,10,27,8,23,9,15,9,30,0,13,17,17,0,19,5,22,20,25,5,17,18,23,0,28,6,10,16,22,});
sparse_graph3_ints[23][80] = vector<int>({14,33,0,0,9,16,0,40,24,25,6,6,0,28,17,26,15,21,8,12,14,17,0,36,4,4,10,32,0,33,3,14,6,37,4,18,22,23,12,28,26,27,7,8,7,31,11,22,22,29,20,27,19,22,2,22,0,10,0,19,14,14,7,20,6,29,11,26,17,17,11,12,18,31,5,35,14,30,19,19,8,22,6,26,1,25,1,30,0,2,0,4,15,24,0,6,0,14,10,35,});
sparse_graph3_ints[23][100] = vector<int>({8,24,14,26,0,0,0,17,24,25,12,12,24,34,6,47,0,12,9,19,20,40,28,28,10,10,18,36,0,3,4,5,30,31,6,34,12,38,26,37,19,26,14,43,0,24,32,34,0,6,6,40,0,30,15,16,21,21,23,30,18,20,11,25,13,30,7,8,18,32,0,50,6,43,0,21,0,27,6,37,0,47,0,44,13,34,0,38,6,31,0,41,12,41,14,20,0,35,5,29,});
sparse_graph3_ints[20][6] = vector<int>({2,2,0,0,0,2,0,1,0,3,1,2,1,1,});
sparse_graph3_ints[20][8] = vector<int>({2,3,0,0,0,2,0,4,0,1,0,3,2,2,1,1,1,2,1,3,});
sparse_graph3_ints[20][10] = vector<int>({0,5,0,0,0,2,3,3,2,2,0,1,0,4,0,3,2,4,1,2,2,3,1,1,1,3,1,4,});
sparse_graph3_ints[20][12] = vector<int>({4,4,0,0,0,2,0,6,2,2,0,4,0,1,3,3,1,2,0,5,2,5,2,3,0,3,2,4,1,1,3,4,1,4,1,5,1,3,});
sparse_graph3_ints[20][14] = vector<int>({4,5,0,0,1,2,0,7,3,3,0,1,0,4,0,2,0,6,2,2,2,6,0,5,3,4,2,3,4,4,2,4,2,5,0,3,1,1,1,5,3,5,1,4,1,3,1,6,});
sparse_graph3_ints[20][16] = vector<int>({0,8,0,0,2,2,5,5,0,2,0,6,3,3,4,4,0,1,2,7,1,2,0,5,0,4,0,7,3,4,2,3,2,6,4,6,4,5,1,5,1,6,3,5,2,4,0,3,3,6,1,1,2,5,1,7,1,4,1,3,});
sparse_graph3_ints[20][18] = vector<int>({6,6,0,0,2,2,0,9,4,4,0,2,0,7,0,5,0,1,3,3,2,8,1,2,5,5,0,6,2,6,0,8,4,7,3,4,0,4,4,5,2,3,2,7,4,6,1,5,1,6,3,6,5,6,3,7,1,7,3,5,2,4,0,3,1,1,2,5,1,8,1,4,1,3,});
sparse_graph3_ints[20][20] = vector<int>({0,0,6,7,2,2,0,10,4,4,0,2,0,6,0,8,3,7,0,1,3,3,1,2,4,8,0,5,5,6,4,5,0,7,2,9,3,4,0,9,2,6,5,5,0,4,2,3,6,6,2,8,3,6,4,7,1,8,4,6,5,7,1,6,1,7,1,5,3,8,3,5,2,7,2,4,1,9,0,3,2,5,1,1,1,3,1,4,});
sparse_graph3_ints[20][23] = vector<int>({1,11,0,0,0,6,7,8,5,5,1,1,2,2,0,8,4,4,0,10,0,1,6,7,0,3,3,3,5,9,0,5,2,8,4,8,3,4,3,10,5,6,4,5,0,7,0,9,2,9,6,6,0,11,7,7,2,7,2,6,2,3,0,4,3,9,3,7,5,8,4,7,5,7,6,8,1,9,4,9,3,8,4,6,2,10,1,5,1,7,2,5,1,10,3,5,3,6,1,8,});
sparse_graph3_ints[20][26] = vector<int>({8,9,0,8,0,0,2,3,2,12,6,7,1,2,0,11,4,5,0,1,3,4,2,8,0,6,0,2,5,6,6,10,2,2,0,13,2,10,5,9,4,4,7,8,0,9,4,11,4,8,3,3,5,5,0,7,0,10,6,6,0,12,2,9,7,7,2,11,8,8,4,10,2,7,2,6,5,8,6,9,0,5,3,10,0,4,6,8,7,9,4,7,3,9,3,7,5,10,4,9,});
sparse_graph3_ints[20][30] = vector<int>({10,10,0,0,0,6,2,14,0,11,4,5,0,2,6,6,4,10,2,2,6,12,0,13,0,8,0,1,7,7,2,8,6,10,3,4,1,2,1,4,0,15,9,9,5,6,2,10,4,4,8,8,0,9,2,12,5,5,4,13,6,7,8,11,4,11,3,3,7,8,0,12,0,10,0,7,4,12,2,11,6,11,0,14,8,9,2,9,2,13,4,9,4,8,8,10,6,9,7,10,});
sparse_graph3_ints[20][35] = vector<int>({5,6,1,17,0,0,11,12,3,3,7,11,0,12,0,8,7,8,0,15,1,1,7,14,1,3,1,9,4,14,4,5,0,1,6,7,5,11,0,3,1,13,0,5,4,10,0,7,10,11,8,9,3,9,4,4,5,5,3,16,9,10,0,11,2,15,7,7,3,13,6,6,0,9,8,12,9,13,8,8,5,15,0,16,6,13,3,10,3,12,5,12,9,9,0,13,10,10,11,11,});
sparse_graph3_ints[20][40] = vector<int>({8,16,0,11,3,14,0,20,0,0,2,4,7,8,13,13,9,9,0,8,11,11,0,2,0,14,6,6,7,13,2,2,0,18,5,17,5,5,0,1,0,16,4,10,1,2,5,11,1,4,1,6,4,5,6,12,1,12,6,15,9,14,10,10,6,7,0,10,3,17,5,6,12,12,8,8,2,16,2,19,3,12,10,15,5,14,2,10,4,4,8,9,7,7,0,9,9,10,9,12,});
sparse_graph3_ints[20][45] = vector<int>({9,18,3,21,0,3,0,10,11,11,8,9,15,15,0,19,3,4,0,0,3,11,6,16,13,13,7,13,3,14,2,3,6,7,10,15,12,16,4,5,1,20,0,1,1,11,6,19,5,6,1,1,0,8,7,8,1,14,1,3,4,15,0,22,3,3,10,10,9,14,5,11,7,17,6,12,1,17,3,17,0,16,0,13,5,5,12,12,6,6,4,19,9,9,7,7,0,11,8,13,});
sparse_graph3_ints[20][50] = vector<int>({0,17,0,22,13,14,8,21,16,17,7,7,1,2,3,4,0,0,0,25,5,6,10,10,7,17,12,12,0,14,12,19,15,15,4,23,9,18,0,12,0,20,1,4,6,13,4,18,0,10,0,1,4,5,8,8,6,20,10,15,4,16,2,14,0,2,2,2,7,14,12,17,0,8,9,9,4,11,1,23,2,4,6,6,5,12,3,20,11,11,3,10,9,14,1,18,3,15,5,5,});
sparse_graph3_ints[20][55] = vector<int>({0,0,7,7,7,17,7,24,15,15,0,18,18,18,10,10,0,7,0,24,0,11,0,27,5,12,10,20,0,3,13,14,12,12,5,16,16,17,4,23,11,22,0,14,0,9,2,3,0,20,8,19,15,20,0,16,0,22,13,19,4,21,9,13,12,17,6,6,5,14,0,1,4,25,10,16,8,8,5,5,1,1,7,22,1,3,4,19,9,9,3,3,2,11,4,11,4,4,1,25,});
sparse_graph3_ints[20][60] = vector<int>({1,2,18,18,14,14,4,24,4,17,6,10,10,25,0,30,0,0,0,13,16,16,20,20,9,17,0,18,0,22,14,22,8,12,6,6,5,19,0,5,0,26,0,7,4,28,0,9,11,22,0,15,8,15,12,13,15,19,8,20,13,17,0,24,17,21,9,9,0,11,11,11,4,22,0,28,8,23,6,8,4,26,0,1,0,20,7,26,3,14,0,2,5,5,2,2,11,20,4,15,});
sparse_graph3_ints[20][70] = vector<int>({8,8,0,35,0,0,0,11,5,21,20,20,16,16,14,28,12,12,9,23,1,2,0,14,18,18,0,27,0,23,4,5,16,22,14,20,12,24,10,30,0,5,6,32,6,7,14,15,8,14,9,28,7,22,6,18,8,26,4,16,0,7,5,30,17,24,4,28,13,26,0,18,21,22,18,26,1,32,0,30,0,21,12,19,0,16,22,24,7,11,11,15,0,25,10,18,10,10,4,26,});
sparse_graph3_ints[20][80] = vector<int>({5,27,10,35,0,11,15,15,8,8,26,27,0,0,10,12,0,25,6,32,22,22,19,19,16,26,1,2,9,18,4,38,21,29,17,17,16,32,3,17,0,37,0,14,14,22,24,24,10,27,14,30,12,20,19,27,12,13,0,17,5,19,7,23,4,5,1,34,9,10,2,26,11,29,0,5,0,22,9,33,4,30,10,24,0,7,15,24,19,22,5,35,6,7,1,29,0,32,0,40,});
sparse_graph3_ints[20][100] = vector<int>({12,34,4,5,13,14,18,37,9,10,33,33,10,26,28,28,1,2,4,36,25,25,0,33,17,27,22,39,7,8,11,12,0,21,0,0,24,34,16,42,30,31,21,21,2,4,18,20,0,12,14,20,7,25,17,34,28,36,19,29,12,41,11,38,6,44,23,30,5,20,11,31,23,23,14,26,7,22,0,50,15,16,4,39,4,42,0,30,8,46,0,17,0,27,0,47,6,32,0,36,});
sparse_graph3_ints[17][6] = vector<int>({2,2,0,0,0,1,0,2,0,3,1,1,1,2,});
sparse_graph3_ints[17][8] = vector<int>({2,3,0,0,0,2,0,4,0,1,1,2,2,2,0,3,1,1,1,3,});
sparse_graph3_ints[17][10] = vector<int>({0,5,0,0,0,2,2,2,3,3,0,1,1,2,0,4,2,4,2,3,0,3,1,1,1,3,1,4,});
sparse_graph3_ints[17][12] = vector<int>({0,0,4,4,0,4,0,6,0,1,1,2,3,3,0,2,2,2,0,5,2,5,2,3,2,4,0,3,3,4,1,1,1,4,1,5,1,3,});
sparse_graph3_ints[17][14] = vector<int>({0,0,4,5,2,2,0,7,0,2,0,5,0,1,3,4,1,2,2,6,0,6,2,4,0,4,2,3,4,4,3,3,2,5,1,5,3,5,0,3,1,1,1,6,1,4,1,3,});
sparse_graph3_ints[17][16] = vector<int>({0,0,0,8,2,2,5,5,0,2,0,6,3,3,0,1,4,4,1,2,2,7,0,5,0,7,3,4,0,4,2,6,2,3,4,6,4,5,1,5,1,6,3,5,3,6,2,4,2,5,1,7,0,3,1,1,1,3,1,4,});
sparse_graph3_ints[17][18] = vector<int>({6,6,0,0,0,4,0,9,4,4,0,2,0,7,3,3,0,1,1,2,2,8,0,6,5,5,2,3,2,6,3,4,0,8,4,7,4,5,2,7,0,5,2,2,4,6,5,6,3,6,1,5,3,7,1,6,3,5,1,7,2,4,2,5,1,8,0,3,1,1,1,3,1,4,});
sparse_graph3_ints[17][20] = vector<int>({0,0,0,6,6,7,0,10,1,2,4,5,0,8,0,1,2,3,3,4,0,2,0,4,2,8,5,6,4,8,4,4,3,3,0,7,2,9,5,5,0,9,2,6,6,6,3,7,0,5,2,2,4,7,3,6,4,6,5,7,1,8,3,8,1,7,1,6,1,5,3,5,2,7,1,9,2,4,2,5,0,3,1,1,1,3,1,4,});
sparse_graph3_ints[17][23] = vector<int>({0,0,2,2,7,8,0,8,1,11,5,6,0,6,1,1,0,10,4,5,0,1,3,4,0,3,5,9,0,5,2,8,6,7,3,3,4,8,4,4,5,5,3,10,0,7,0,9,2,9,6,6,7,7,0,11,2,7,2,6,3,9,5,8,4,7,3,7,2,3,5,7,6,8,0,4,4,9,1,9,2,10,4,6,3,8,1,10,1,7,3,5,3,6,1,5,2,5,1,8,});
sparse_graph3_ints[17][26] = vector<int>({6,6,2,2,0,0,0,8,8,9,0,11,0,6,4,10,0,2,0,13,5,5,0,1,3,4,2,8,1,2,1,4,7,8,4,4,2,10,4,5,6,7,2,12,6,10,0,9,5,6,3,3,4,11,0,7,0,10,7,7,4,8,2,9,0,12,8,8,2,11,5,9,6,9,5,8,2,7,7,9,6,8,2,6,3,10,5,10,3,9,4,7,4,9,3,7,5,7,3,11,});
sparse_graph3_ints[17][30] = vector<int>({0,11,7,8,1,2,10,10,0,0,2,14,0,8,6,12,5,6,1,4,0,13,0,1,4,10,3,4,4,5,0,2,2,2,2,8,6,7,0,6,6,10,9,9,0,15,4,4,2,10,6,6,5,5,0,9,4,11,7,7,4,13,8,8,8,11,2,12,4,12,6,11,0,12,2,11,8,9,3,3,0,10,0,7,0,14,2,13,2,9,8,10,4,9,4,8,9,10,7,11,});
sparse_graph3_ints[17][35] = vector<int>({11,12,5,12,0,3,2,5,7,7,0,12,0,0,1,17,5,6,5,15,9,10,2,3,1,13,1,9,0,1,4,5,1,1,2,15,0,8,1,3,3,10,4,11,3,3,8,9,0,15,6,13,6,6,9,13,5,5,7,8,0,11,4,4,10,11,6,7,3,13,3,9,0,9,8,8,9,9,4,10,8,12,7,14,3,16,4,14,0,16,7,11,5,11,10,10,0,13,11,11,});
sparse_graph3_ints[17][40] = vector<int>({13,13,5,17,7,8,0,0,4,4,0,14,0,11,7,13,2,2,0,20,9,9,11,11,6,6,0,2,9,14,0,18,2,4,4,15,5,5,0,1,5,11,1,2,1,16,1,4,4,10,3,4,4,5,1,12,5,13,6,7,8,16,2,14,10,10,0,10,5,6,8,8,6,15,3,17,12,12,3,12,8,9,7,7,2,19,10,15,2,10,8,11,5,12,10,11,0,9,0,8,});
sparse_graph3_ints[17][45] = vector<int>({3,13,0,10,0,0,11,11,9,18,3,21,0,3,13,13,9,9,15,15,6,16,7,8,0,17,2,3,3,4,10,15,1,11,12,16,0,21,4,5,0,1,6,19,5,6,8,13,3,11,1,1,6,7,1,3,5,15,0,15,0,8,3,19,3,3,7,12,2,17,7,17,10,10,3,15,9,14,0,13,1,19,6,6,7,9,5,5,12,12,8,9,7,7,4,13,0,11,1,22,});
sparse_graph3_ints[17][50] = vector<int>({0,0,15,15,0,14,7,18,12,12,10,20,0,25,0,10,8,12,1,2,3,18,2,16,8,9,11,17,0,12,1,4,16,17,4,23,3,4,5,8,13,18,13,14,0,23,4,5,4,12,5,6,0,1,8,14,4,20,7,21,7,8,0,19,0,2,2,2,2,4,6,17,1,21,0,8,0,16,9,15,3,22,11,11,6,12,8,19,6,6,4,16,6,8,9,10,10,16,5,14,});
sparse_graph3_ints[17][55] = vector<int>({2,3,0,3,0,11,16,17,0,27,13,14,8,15,6,13,8,8,11,22,0,0,0,7,1,15,7,24,9,17,5,19,0,19,15,15,0,25,18,18,0,23,11,11,0,21,5,5,15,20,13,19,10,20,6,6,7,20,12,17,2,11,4,25,7,7,0,9,5,12,0,1,7,22,4,18,2,16,4,23,9,9,1,1,4,11,7,14,1,3,11,13,3,3,0,14,4,21,10,10,});
sparse_graph3_ints[17][60] = vector<int>({0,0,0,13,8,9,12,12,18,19,1,27,7,19,0,22,13,19,3,16,8,24,15,15,14,23,9,12,0,2,0,4,0,6,6,6,13,14,17,17,3,24,12,22,0,20,6,27,10,25,20,20,8,15,6,8,0,24,0,17,0,11,5,5,10,20,15,20,7,21,9,10,4,20,11,17,4,12,5,25,4,17,3,28,0,9,0,15,9,16,0,1,4,4,0,30,5,13,1,2,});
sparse_graph3_ints[17][70] = vector<int>({0,33,8,24,22,24,14,16,0,0,1,4,0,10,18,18,5,10,11,11,0,25,9,28,13,13,20,20,5,15,12,29,4,4,18,26,0,2,16,22,0,18,10,18,12,24,9,15,6,20,0,13,9,21,16,17,13,26,8,9,13,21,5,23,4,26,8,31,5,30,17,24,6,6,0,16,4,33,5,17,0,27,8,26,21,22,0,23,4,28,0,21,9,10,0,35,0,31,0,29,});
sparse_graph3_ints[17][80] = vector<int>({21,21,2,2,1,27,12,26,5,35,8,10,26,27,17,17,16,32,0,0,7,7,0,24,1,34,15,15,0,11,4,18,0,8,9,25,19,19,0,16,11,34,12,20,10,18,0,19,24,25,14,22,7,23,10,11,0,40,22,29,14,30,19,28,0,30,0,37,0,2,17,26,6,32,17,23,6,37,2,4,2,31,7,29,21,24,13,28,10,32,11,15,5,22,12,12,5,27,6,19,});
sparse_graph3_ints[17][100] = vector<int>({8,9,26,30,2,4,6,44,15,15,0,28,0,0,18,22,20,40,11,34,4,8,1,42,20,30,30,31,33,33,24,24,8,19,14,43,13,29,17,38,28,36,22,22,12,25,10,14,18,28,1,2,1,21,13,36,24,35,26,26,0,46,6,28,3,6,0,35,0,50,18,18,13,20,7,40,8,46,0,25,21,33,6,35,10,11,6,25,1,18,12,40,6,32,0,39,0,32,0,15,});
sparse_graph3_ints[13][6] = vector<int>({2,2,0,0,0,1,0,2,0,3,1,1,1,2,});
sparse_graph3_ints[13][8] = vector<int>({0,0,2,3,0,2,0,4,0,1,1,2,2,2,0,3,1,1,1,3,});
sparse_graph3_ints[13][10] = vector<int>({0,5,0,0,0,2,2,2,3,3,0,1,1,2,0,4,2,4,2,3,0,3,1,1,1,3,1,4,});
sparse_graph3_ints[13][12] = vector<int>({4,4,0,0,0,4,0,6,0,1,1,2,0,2,3,3,2,2,0,5,2,5,2,3,2,4,3,4,1,5,1,4,1,3,0,3,1,1,});
sparse_graph3_ints[13][14] = vector<int>({4,5,0,0,2,2,0,7,0,2,3,3,0,1,0,5,1,2,2,6,0,6,3,4,4,4,0,4,2,3,2,5,2,4,3,5,1,5,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[13][16] = vector<int>({5,5,0,0,0,4,0,8,0,2,0,6,3,3,0,1,4,4,1,2,2,7,2,3,3,4,0,7,2,6,4,6,4,5,0,5,2,2,3,5,1,5,1,6,3,6,2,4,1,7,2,5,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[13][18] = vector<int>({6,6,2,2,0,0,0,9,4,5,0,2,0,7,3,3,0,1,1,2,2,3,2,8,4,4,0,6,3,4,4,6,0,8,4,7,2,6,2,7,5,5,0,5,5,6,0,4,3,7,3,6,3,5,1,7,1,5,1,6,1,8,2,4,2,5,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[13][20] = vector<int>({6,7,0,10,0,6,0,0,1,2,4,5,0,8,0,1,2,3,0,2,3,4,2,2,2,8,5,6,4,4,3,3,4,8,2,9,5,5,0,7,6,6,0,9,2,7,2,6,4,7,4,6,5,7,3,7,0,5,0,4,3,8,1,9,3,6,1,7,3,5,1,8,2,5,1,5,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[13][23] = vector<int>({0,6,7,8,0,3,0,0,0,10,4,5,4,8,0,8,0,1,1,11,2,3,1,1,3,4,2,2,5,6,4,4,5,9,5,5,6,7,3,3,2,8,3,10,0,7,0,9,2,9,7,7,6,6,0,11,3,9,5,8,2,7,6,8,5,7,2,6,4,7,3,7,4,9,2,10,1,9,4,6,3,8,0,5,1,10,0,4,1,7,3,6,3,5,1,8,2,5,1,5,});
sparse_graph3_ints[13][26] = vector<int>({0,13,1,2,2,8,8,9,0,0,6,7,3,4,4,10,1,4,0,1,0,8,4,5,0,2,2,2,2,10,5,6,0,6,4,4,7,8,5,5,6,6,6,10,2,12,0,9,2,9,7,7,4,11,3,3,8,8,0,12,0,10,0,7,4,9,4,8,0,11,2,11,6,9,6,8,7,9,5,9,5,10,3,11,5,8,2,7,3,9,3,10,5,7,2,6,1,11,4,7,});
sparse_graph3_ints[13][30] = vector<int>({6,7,2,14,1,2,0,8,0,0,10,10,6,12,0,10,8,9,1,4,0,13,0,1,4,10,3,4,4,5,0,2,2,2,3,9,5,6,0,6,7,8,5,5,4,4,6,6,0,15,6,10,7,7,0,9,0,12,3,10,8,10,2,9,4,13,8,8,8,11,2,12,4,12,6,11,5,10,9,9,2,13,0,14,2,8,9,10,3,3,0,11,0,7,7,11,7,10,5,11,});
sparse_graph3_ints[13][35] = vector<int>({7,7,11,12,1,17,0,0,0,3,1,9,0,15,0,12,8,9,5,15,2,3,2,5,8,12,4,11,4,5,0,1,5,6,1,1,5,12,1,3,3,3,3,10,1,13,4,14,4,4,6,6,9,10,7,8,5,5,6,7,0,11,8,8,3,9,9,13,9,9,0,9,10,11,5,11,4,10,2,14,0,8,7,14,6,13,0,16,11,11,10,10,3,16,0,13,7,11,2,15,});
sparse_graph3_ints[13][40] = vector<int>({9,9,0,0,13,13,8,14,3,16,2,14,1,4,5,6,7,7,4,18,0,12,0,7,0,20,1,2,0,10,10,15,11,11,3,6,1,13,0,1,7,12,0,18,0,2,2,2,2,4,4,4,4,11,8,8,7,16,6,6,5,12,5,5,4,14,6,7,10,10,1,16,6,14,12,12,8,9,7,8,4,10,0,11,5,16,1,12,9,12,5,11,11,12,0,15,3,9,2,18,});
sparse_graph3_ints[13][45] = vector<int>({0,16,13,13,7,17,7,14,1,18,15,15,4,5,0,3,0,0,9,10,0,7,2,3,3,19,11,12,3,21,0,14,5,6,12,16,6,7,0,10,1,11,7,8,9,18,10,15,8,9,4,12,0,1,1,1,0,22,0,20,4,17,1,3,10,11,6,19,3,3,6,12,4,4,3,14,5,9,6,6,2,16,8,15,5,5,5,14,8,8,9,9,1,12,9,11,0,11,3,12,});
sparse_graph3_ints[13][50] = vector<int>({8,8,4,5,15,15,4,17,12,13,0,12,10,20,0,15,9,10,0,0,1,2,1,4,6,22,8,17,5,6,5,13,16,17,0,20,6,20,12,17,0,10,10,11,1,16,6,14,3,21,12,15,0,22,11,12,3,23,2,12,0,1,8,14,5,18,0,8,4,9,0,24,3,4,0,2,2,2,3,16,9,18,6,11,5,10,2,4,3,13,0,18,13,18,9,15,7,7,6,6,});
sparse_graph3_ints[13][55] = vector<int>({1,3,0,0,1,1,15,20,9,9,12,12,9,23,5,5,0,18,7,15,6,22,0,6,7,19,3,21,0,16,6,6,0,20,16,17,13,18,0,11,18,18,10,19,10,10,12,16,5,12,15,15,12,21,8,8,8,16,11,11,3,23,4,11,0,22,6,14,12,14,6,24,7,7,0,9,0,14,3,25,4,4,5,18,4,17,3,10,0,27,0,25,0,1,3,16,0,3,2,3,});
sparse_graph3_ints[13][60] = vector<int>({4,17,5,6,20,20,17,17,3,4,1,2,0,14,4,24,10,17,12,12,5,19,16,22,12,24,0,1,8,26,1,4,15,16,0,9,4,28,6,7,7,8,0,19,0,26,8,9,0,30,0,17,9,10,18,19,8,15,14,21,10,11,0,28,10,23,0,12,12,20,6,21,2,20,3,13,5,13,7,24,13,13,9,16,3,22,12,18,14,14,6,14,0,23,9,20,4,26,0,0,});
sparse_graph3_ints[13][70] = vector<int>({8,9,12,13,0,12,0,2,10,20,2,4,18,22,20,25,13,23,10,12,17,18,8,26,1,14,6,18,16,27,9,28,14,15,3,7,14,25,12,29,13,20,0,0,2,2,10,17,8,31,7,7,4,4,5,25,5,30,5,21,0,24,4,28,16,16,21,22,4,33,1,32,19,19,0,30,0,22,0,17,10,10,5,16,9,23,23,23,0,35,0,28,5,14,5,23,0,20,0,26,});
sparse_graph3_ints[13][80] = vector<int>({22,24,7,24,19,20,6,30,8,9,5,37,3,4,13,14,0,0,26,27,0,12,16,32,17,18,11,32,0,4,6,19,5,34,10,13,3,18,10,10,0,15,10,25,6,22,22,29,0,36,0,26,0,40,0,18,18,26,5,5,5,28,0,23,12,21,14,28,19,28,1,33,12,34,21,22,15,22,17,24,11,19,16,16,10,30,15,30,13,26,0,31,2,27,24,25,7,7,0,2,});
sparse_graph3_ints[13][100] = vector<int>({22,39,30,31,12,12,2,4,4,11,8,32,0,0,15,15,0,44,6,47,22,22,19,19,1,27,12,38,19,30,12,34,10,10,11,25,33,33,2,33,17,17,9,23,28,36,24,24,25,34,20,36,6,37,26,26,1,37,9,18,0,24,28,28,0,31,23,31,17,24,0,50,1,2,15,29,17,41,2,46,18,34,6,6,6,41,3,6,1,17,0,21,12,44,8,43,15,39,0,41,});
sparse_graph3_ints[10][6] = vector<int>({2,2,0,0,0,1,0,2,0,3,1,1,1,2,});
sparse_graph3_ints[10][8] = vector<int>({0,0,2,3,0,2,0,1,0,4,1,2,2,2,0,3,1,1,1,3,});
sparse_graph3_ints[10][10] = vector<int>({0,5,0,0,0,2,2,2,3,3,0,1,1,2,0,4,2,4,2,3,1,3,1,1,0,3,1,4,});
sparse_graph3_ints[10][12] = vector<int>({4,4,0,0,1,2,0,6,0,1,2,3,0,2,0,4,2,4,2,5,2,2,0,5,3,3,3,4,1,5,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[10][14] = vector<int>({1,2,0,0,4,5,0,6,0,1,2,3,0,2,2,2,3,4,0,7,2,6,3,3,4,4,0,5,0,4,2,5,2,4,3,5,1,5,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[10][16] = vector<int>({1,2,5,5,0,0,0,6,0,8,0,1,2,3,4,4,0,2,2,2,3,3,2,7,3,4,2,6,0,7,4,6,4,5,0,5,0,4,3,5,3,6,2,5,1,5,1,7,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[10][18] = vector<int>({3,3,6,6,0,0,0,9,0,2,4,5,0,7,2,2,0,1,1,2,2,3,4,4,2,8,0,6,3,4,4,6,4,7,0,8,2,6,2,7,5,5,5,6,3,7,0,5,3,6,3,5,1,7,0,4,1,8,2,5,1,5,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[10][20] = vector<int>({4,5,1,2,0,0,0,10,6,7,0,6,0,8,0,1,2,3,0,2,2,2,3,4,5,6,4,4,2,8,3,3,4,8,5,5,0,7,2,9,6,6,0,9,2,7,2,6,5,7,4,7,4,6,3,7,3,8,1,9,0,5,1,7,3,6,3,5,1,8,0,4,2,5,1,5,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[10][23] = vector<int>({4,5,0,0,7,8,0,10,0,3,0,6,4,8,0,8,0,1,1,11,2,3,1,1,2,2,3,4,5,6,4,4,5,5,5,9,6,7,3,3,2,8,3,10,7,7,0,9,0,7,6,6,2,9,6,8,3,9,0,11,5,8,5,7,2,7,4,9,4,7,3,7,2,10,2,6,1,9,3,8,4,6,1,10,1,7,0,5,3,6,3,5,1,8,0,4,2,5,1,5,});
sparse_graph3_ints[10][26] = vector<int>({0,13,1,2,0,0,8,9,4,10,0,8,1,4,6,7,3,4,0,1,4,5,0,2,2,8,2,2,5,6,3,3,2,10,4,4,7,8,5,5,6,6,6,10,0,9,7,7,2,12,2,9,8,8,4,11,0,6,0,12,2,11,0,7,0,10,4,9,7,9,4,8,0,11,6,9,6,8,5,9,5,10,3,11,5,8,3,9,3,10,5,7,1,11,2,7,1,12,4,7,});
sparse_graph3_ints[10][30] = vector<int>({6,7,1,2,0,8,0,0,0,15,1,10,10,10,6,12,1,4,0,13,4,11,8,9,0,1,3,4,4,5,0,2,2,2,5,6,3,3,2,8,3,10,7,8,6,6,5,5,4,4,7,7,8,8,4,13,0,9,3,9,4,10,8,10,0,12,8,11,2,14,6,10,4,12,6,11,9,9,2,12,9,10,2,13,0,14,7,11,7,10,5,11,7,9,0,6,0,10,0,7,});
sparse_graph3_ints[10][35] = vector<int>({6,6,0,0,5,15,0,16,9,10,0,12,1,1,1,3,5,12,3,3,11,12,4,4,5,5,1,13,7,7,0,10,1,17,4,11,0,1,0,3,8,9,3,10,2,3,2,5,6,13,4,5,5,6,7,8,10,11,6,7,8,8,3,9,9,9,9,13,0,11,4,10,5,11,1,14,10,10,0,9,11,11,7,14,2,15,6,12,8,12,0,8,0,13,3,16,4,14,10,12,});
sparse_graph3_ints[10][40] = vector<int>({8,8,5,6,0,1,6,13,0,7,0,20,1,12,10,13,1,2,1,4,0,10,6,7,7,16,3,6,4,18,9,9,3,15,10,10,3,12,0,0,5,12,4,16,1,18,7,14,0,17,0,2,0,14,2,2,4,10,11,14,4,4,2,14,2,4,2,10,5,5,7,8,6,6,13,13,8,9,7,7,10,11,9,10,0,11,12,12,5,11,6,12,11,11,9,15,8,14,7,13,});
sparse_graph3_ints[10][45] = vector<int>({5,5,1,3,0,0,1,1,5,11,7,19,12,12,13,16,4,9,6,15,1,20,3,3,3,5,0,15,3,21,9,11,7,17,10,17,0,12,9,14,2,12,6,13,11,11,13,14,10,15,3,17,3,15,0,22,0,19,0,17,0,1,4,19,0,3,2,3,4,13,5,7,2,5,7,11,6,9,2,7,5,17,7,7,5,6,8,9,15,15,0,10,9,10,8,8,0,11,9,12,});
sparse_graph3_ints[10][50] = vector<int>({1,4,4,9,0,1,11,11,3,17,16,17,10,20,4,23,8,8,13,13,0,22,4,4,7,19,1,2,5,5,6,13,15,15,0,25,9,9,12,17,5,12,10,10,2,16,4,11,1,15,0,14,10,15,7,21,12,12,0,12,4,21,7,17,9,14,11,16,0,9,6,16,8,13,4,19,14,14,13,18,2,9,1,23,1,20,0,0,7,7,0,19,6,6,0,7,2,2,3,3,});
sparse_graph3_ints[10][55] = vector<int>({15,20,1,27,7,19,9,9,1,1,3,3,0,9,14,14,8,15,0,12,16,16,11,22,0,0,7,24,5,11,1,3,5,6,18,18,1,25,10,10,0,15,11,11,13,18,4,18,7,14,6,13,3,11,0,21,3,17,13,13,0,18,6,7,2,15,8,20,11,16,5,22,12,17,4,25,10,15,9,21,12,12,15,15,4,4,0,24,6,18,7,8,14,19,17,17,2,21,4,21,});
sparse_graph3_ints[10][60] = vector<int>({14,15,12,13,10,11,17,17,20,20,0,26,0,2,0,14,8,9,7,24,0,11,0,8,16,22,12,24,4,16,8,26,0,21,6,7,0,0,3,3,4,28,2,2,0,30,18,19,14,21,4,24,7,8,4,14,0,24,0,28,9,10,8,16,9,21,11,12,3,13,9,17,6,21,14,19,5,5,10,23,13,14,7,15,10,18,5,20,13,18,4,19,4,26,16,16,0,19,0,17,});
sparse_graph3_ints[10][70] = vector<int>({20,25,15,16,4,30,6,12,20,20,3,4,0,0,0,12,8,24,13,14,5,22,0,4,16,26,10,14,0,19,5,5,17,17,12,29,16,23,12,26,18,19,14,22,8,29,8,8,10,25,9,21,4,33,21,22,0,2,13,20,1,14,0,26,0,34,23,23,0,32,8,31,5,18,8,18,4,27,6,8,4,25,0,24,12,17,10,12,0,17,10,10,0,30,0,22,5,16,0,28,});
sparse_graph3_ints[10][80] = vector<int>({16,22,0,14,10,12,23,24,0,0,0,9,11,20,9,33,26,26,14,14,21,29,18,26,16,32,2,4,1,2,6,21,10,27,6,6,3,17,12,13,12,23,5,30,7,26,1,16,5,19,18,18,16,17,14,28,0,25,21,22,12,34,8,8,0,33,17,24,5,25,0,28,10,30,1,30,8,36,9,10,20,20,13,31,6,32,17,29,5,35,4,38,0,22,2,34,1,37,0,40,});
sparse_graph3_ints[10][100] = vector<int>({21,22,0,17,17,18,11,41,8,34,26,37,0,50,1,2,28,28,13,13,0,12,2,34,0,25,20,40,22,32,30,31,10,29,16,35,0,41,32,34,14,43,15,16,0,32,8,46,19,20,6,17,17,38,5,44,23,24,19,30,10,23,3,6,4,26,25,26,0,0,23,35,14,32,8,20,2,4,7,8,0,44,11,37,0,20,13,24,2,37,6,39,7,28,9,10,16,25,18,27,});
sparse_graph3_ints[7][6] = vector<int>({2,2,0,0,0,1,0,2,0,3,1,1,1,2,});
sparse_graph3_ints[7][8] = vector<int>({0,0,2,3,0,2,0,1,0,4,1,2,2,2,1,3,1,1,0,3,});
sparse_graph3_ints[7][10] = vector<int>({0,1,0,5,0,0,2,2,0,2,3,3,1,2,0,4,2,4,2,3,1,3,1,4,1,1,0,3,});
sparse_graph3_ints[7][12] = vector<int>({1,2,0,0,4,4,0,6,0,1,2,3,0,2,2,2,2,4,2,5,0,4,0,5,3,3,3,4,1,5,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[7][14] = vector<int>({0,0,1,2,0,6,4,5,0,1,2,3,0,2,2,2,3,4,0,7,3,3,2,6,4,4,3,5,0,5,0,4,2,5,2,4,1,5,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[7][16] = vector<int>({0,8,0,0,1,2,5,5,0,6,0,1,2,3,0,2,4,4,2,2,3,3,3,4,2,7,4,6,2,6,0,7,4,5,3,5,3,6,0,5,1,7,0,4,2,5,1,5,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[7][18] = vector<int>({0,9,3,3,6,6,0,0,0,2,4,5,0,7,2,2,0,1,1,2,2,3,4,4,0,6,2,8,3,4,5,5,4,7,0,8,4,6,2,6,2,7,5,6,3,7,3,6,3,5,1,7,1,8,0,5,0,4,2,5,1,5,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[7][20] = vector<int>({0,6,0,10,4,8,0,0,0,8,1,2,4,5,0,1,2,3,0,2,2,2,3,4,4,4,5,6,3,3,6,7,5,5,2,8,6,6,0,7,2,9,0,9,5,7,3,7,4,7,2,7,2,6,4,6,3,8,1,9,1,7,3,6,3,5,1,8,0,5,0,4,2,5,1,5,2,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[7][23] = vector<int>({2,9,0,1,0,3,2,3,3,4,4,5,0,0,5,9,0,8,1,11,1,1,5,6,2,2,3,3,4,4,6,7,5,5,7,8,6,6,2,8,7,7,0,6,0,9,3,10,0,7,3,9,0,10,6,8,4,8,5,8,0,11,5,7,4,9,2,10,4,7,3,7,2,7,1,9,2,6,3,8,4,6,1,10,1,7,3,6,3,5,1,8,0,5,0,4,2,5,1,5,});
sparse_graph3_ints[7][26] = vector<int>({0,8,0,13,8,9,0,0,1,2,1,4,6,7,2,10,4,5,0,1,5,6,0,2,3,4,2,8,2,2,3,3,4,4,4,10,7,8,5,5,6,6,7,7,6,10,1,8,2,9,8,8,4,11,2,12,7,9,0,12,2,11,0,6,5,9,0,7,6,9,4,9,4,8,0,10,6,8,0,11,5,10,3,11,3,9,5,8,3,10,5,7,1,11,1,12,4,7,3,7,});
sparse_graph3_ints[7][30] = vector<int>({10,10,4,11,1,10,0,2,0,0,2,2,2,4,4,5,7,7,0,15,0,8,6,6,8,8,0,1,2,8,3,10,1,2,1,4,0,13,1,6,5,6,6,12,6,7,9,9,5,5,7,8,4,4,0,9,3,9,8,9,4,10,8,11,2,14,0,12,4,12,6,11,8,10,6,10,4,13,9,10,7,11,2,12,0,14,2,13,7,10,5,11,7,9,5,12,3,13,6,9,});
sparse_graph3_ints[7][35] = vector<int>({9,9,2,3,0,3,0,15,0,0,3,16,0,8,1,9,7,14,6,6,7,7,3,10,11,12,5,12,4,11,8,8,0,17,1,13,0,12,1,1,2,5,0,1,10,11,1,3,3,3,4,14,5,5,4,7,6,7,9,13,0,9,7,8,3,9,9,10,7,13,8,9,0,11,9,11,5,11,4,10,2,14,11,11,6,13,5,15,0,13,4,5,6,12,4,4,10,12,9,12,});
sparse_graph3_ints[7][40] = vector<int>({10,10,10,15,8,8,7,14,5,6,0,2,3,4,0,20,3,15,0,0,6,17,4,10,1,4,4,5,10,13,5,12,7,12,4,16,0,8,0,17,2,14,1,18,2,8,1,12,2,10,0,1,12,12,2,2,12,14,1,2,3,12,2,4,5,8,8,10,3,18,6,6,7,15,7,8,8,16,10,12,8,9,7,13,1,10,7,7,10,14,10,11,5,11,12,13,0,15,0,14,});
sparse_graph3_ints[7][45] = vector<int>({1,3,0,0,2,7,6,6,9,10,3,17,8,8,15,15,13,13,11,17,7,19,10,13,3,21,1,1,0,7,7,7,10,11,4,11,0,18,3,10,0,22,0,14,1,20,6,12,8,17,4,19,3,14,5,17,12,15,6,15,3,3,9,15,14,14,13,16,3,8,4,15,0,16,11,14,0,12,0,1,7,13,5,5,10,16,0,3,12,12,0,10,2,3,9,18,3,5,7,16,});
sparse_graph3_ints[7][50] = vector<int>({14,15,4,5,0,1,10,11,6,7,0,20,1,4,8,9,12,19,8,19,16,17,4,23,1,2,3,16,3,4,0,12,0,25,5,6,8,21,10,16,7,8,12,13,9,10,11,12,9,15,4,12,0,10,7,18,13,14,4,20,6,17,8,14,3,11,5,13,5,16,3,19,11,18,0,15,15,16,7,13,1,23,5,21,0,22,12,16,14,18,0,18,13,17,0,8,2,15,0,0,});
sparse_graph3_ints[7][55] = vector<int>({13,21,8,15,0,9,15,16,1,1,0,20,6,7,3,3,9,9,7,24,11,16,4,21,4,15,8,21,17,17,3,26,0,0,1,3,11,11,5,6,0,24,0,12,4,13,11,20,4,18,8,8,10,10,7,14,13,13,5,22,14,14,12,17,0,22,8,18,10,15,12,12,0,27,0,18,4,11,18,18,6,18,10,22,14,19,0,16,1,25,4,4,4,24,16,19,0,14,9,19,});
sparse_graph3_ints[7][60] = vector<int>({4,28,10,10,13,14,0,20,20,20,0,2,0,8,6,6,8,8,14,18,7,24,15,20,9,16,8,21,16,22,0,0,3,3,0,30,15,15,5,21,2,2,18,19,0,28,12,12,13,23,17,17,5,5,10,24,1,24,5,19,7,7,3,16,8,10,1,12,11,20,9,18,3,14,1,22,8,15,11,11,4,18,7,14,7,26,12,17,3,12,0,11,4,26,12,21,5,23,2,25,});
sparse_graph3_ints[7][70] = vector<int>({10,30,5,22,6,11,0,32,0,2,0,11,16,16,11,12,12,24,0,6,18,18,20,20,0,14,10,10,13,13,14,20,7,18,16,24,14,15,4,5,8,26,4,27,5,17,6,32,11,20,0,20,21,22,2,34,0,0,23,23,12,27,8,29,4,31,2,2,1,24,17,21,4,29,4,25,20,25,4,15,17,26,10,18,0,30,1,22,0,27,14,28,8,24,7,8,6,7,0,16,});
sparse_graph3_ints[7][80] = vector<int>({0,24,14,14,11,28,11,11,20,20,13,33,0,14,16,17,0,20,4,30,0,2,2,28,2,9,22,22,4,4,1,4,24,24,0,17,15,23,8,36,26,26,9,10,22,28,8,8,20,26,12,13,18,24,18,30,16,28,6,23,5,18,11,21,12,30,1,34,11,25,0,27,8,32,18,19,7,26,14,21,6,31,4,38,12,18,9,18,0,0,0,32,0,40,4,35,4,22,0,38,});
sparse_graph3_ints[7][100] = vector<int>({13,23,17,33,7,33,3,42,15,15,30,30,7,7,0,6,0,0,12,12,22,23,33,33,0,10,24,35,10,10,18,38,0,17,12,40,0,37,28,36,7,26,11,37,4,25,6,44,9,18,14,43,12,33,3,19,24,25,17,17,0,50,0,34,15,28,8,46,0,25,18,29,0,47,6,37,1,2,12,27,18,23,0,31,0,40,20,40,25,31,19,19,20,34,22,30,6,30,27,27,});
sparse_graph3_ints[3][6] = vector<int>({2,2,0,0,0,1,0,2,0,3,1,1,1,2,});
sparse_graph3_ints[3][8] = vector<int>({0,4,0,0,0,1,1,2,0,2,2,3,2,2,1,3,1,1,0,3,});
sparse_graph3_ints[3][10] = vector<int>({1,2,0,0,3,3,0,1,0,5,0,2,2,2,2,4,0,4,2,3,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[3][12] = vector<int>({0,1,2,3,4,4,0,6,0,0,1,2,0,2,2,2,3,3,3,4,2,4,2,5,0,5,0,4,1,5,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[3][14] = vector<int>({4,5,0,0,2,3,0,6,0,1,1,2,0,2,2,2,3,4,3,3,4,4,0,7,2,6,3,5,1,5,2,5,2,4,0,5,0,4,1,6,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[3][16] = vector<int>({4,4,0,8,0,0,1,2,0,1,4,6,0,6,2,3,0,2,2,2,3,3,3,4,4,5,5,5,2,6,2,7,0,7,3,5,3,6,1,7,2,5,1,5,2,4,1,6,0,5,0,4,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[3][18] = vector<int>({1,2,2,3,0,9,0,0,6,6,0,6,0,1,3,4,0,2,2,2,4,5,4,4,3,3,0,7,5,5,2,8,5,6,0,8,4,6,4,7,3,7,2,6,2,7,3,6,1,7,3,5,1,8,2,5,1,5,2,4,1,6,0,5,0,4,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[3][20] = vector<int>({4,8,0,2,2,2,4,4,0,0,0,10,0,8,0,6,0,1,2,3,1,2,5,6,3,4,4,5,6,7,5,5,3,3,6,6,2,8,0,7,2,9,5,7,0,9,3,7,4,7,4,6,3,8,2,7,1,9,2,6,1,7,3,6,1,8,3,5,2,5,1,5,2,4,1,6,0,5,0,4,1,3,1,4,0,3,1,1,});
sparse_graph3_ints[3][23] = vector<int>({5,5,0,3,0,0,2,3,7,8,3,10,0,9,4,4,6,7,2,9,1,1,0,1,0,6,1,3,3,4,4,5,5,6,5,9,6,6,7,7,0,8,1,11,0,10,4,8,3,3,0,7,6,8,3,9,2,8,5,8,5,7,0,11,4,9,2,10,4,7,3,7,1,9,3,8,4,6,1,10,2,7,2,6,1,7,3,6,1,8,3,5,2,5,1,5,2,4,1,6,});
sparse_graph3_ints[3][26] = vector<int>({2,11,4,10,3,3,4,4,0,2,0,0,2,2,8,9,7,8,6,6,0,8,6,10,0,1,0,11,1,2,1,4,4,5,3,4,4,11,5,6,5,5,2,10,6,7,7,7,8,8,1,8,0,13,0,10,2,12,4,9,0,12,7,9,4,8,5,9,6,9,6,8,5,10,0,6,0,7,3,11,2,8,2,9,3,9,5,8,3,10,1,11,5,7,1,12,4,7,3,7,});
sparse_graph3_ints[3][30] = vector<int>({8,9,6,12,6,7,0,9,2,2,0,15,0,0,0,2,4,4,0,13,5,6,7,8,10,10,1,10,4,12,8,11,1,6,2,4,3,10,0,1,1,2,6,10,4,13,1,4,4,5,3,11,4,6,6,6,0,8,2,14,3,9,8,8,7,7,0,14,5,9,0,10,0,12,5,10,9,9,3,12,9,10,8,10,6,11,7,11,7,10,5,11,7,9,5,12,3,13,2,12,});
sparse_graph3_ints[3][35] = vector<int>({11,12,6,11,4,4,6,7,0,13,0,5,1,9,0,16,1,1,5,15,0,0,9,9,3,12,1,17,10,11,6,13,9,13,3,9,7,9,8,12,7,14,4,14,1,6,0,1,3,3,4,7,2,7,0,11,1,3,0,3,3,16,2,15,3,11,0,15,5,5,7,8,3,10,6,12,7,7,2,14,6,6,4,13,8,11,8,9,0,14,0,8,5,11,0,12,10,10,11,11,});
sparse_graph3_ints[3][40] = vector<int>({0,20,7,8,0,13,0,16,4,14,9,9,1,2,0,0,3,4,12,12,8,14,4,18,5,6,2,3,4,5,6,7,6,11,5,16,13,13,6,14,9,12,11,14,0,8,3,16,1,14,10,10,9,15,10,13,11,11,7,16,3,9,2,8,0,11,0,6,1,17,2,18,0,2,0,1,2,2,2,19,5,9,2,15,0,19,4,11,6,13,6,10,3,13,8,11,6,12,2,12,});
sparse_graph3_ints[3][45] = vector<int>({5,19,11,17,3,3,0,22,0,12,1,1,5,5,9,10,9,14,13,13,0,0,2,2,6,15,4,4,11,11,7,10,5,10,3,17,1,20,10,15,12,12,0,19,0,9,3,14,14,14,15,15,7,7,4,15,9,17,0,16,12,15,1,17,13,16,2,9,4,9,6,6,8,16,9,12,7,19,1,14,7,13,0,7,6,12,6,17,4,12,2,12,3,21,4,18,8,8,0,1,});
sparse_graph3_ints[3][50] = vector<int>({7,18,6,6,0,2,0,8,0,17,8,13,8,8,0,20,6,22,14,15,2,24,2,4,0,0,12,12,2,2,11,17,5,5,7,7,0,11,4,20,10,10,9,14,3,19,11,11,2,14,7,12,8,19,1,23,9,9,5,21,0,14,4,11,3,10,4,14,15,16,6,11,6,17,0,22,1,18,16,17,12,15,14,18,4,17,13,17,5,15,13,13,11,19,10,16,11,14,9,20,});
sparse_graph3_ints[3][55] = vector<int>({15,19,5,25,3,23,6,7,1,1,3,3,13,13,0,9,9,9,12,18,8,15,4,16,1,27,11,11,0,0,1,3,5,20,9,19,12,21,5,6,9,23,0,12,8,8,9,21,0,19,2,21,5,12,1,20,12,16,6,13,11,13,1,15,6,21,0,24,8,13,4,11,5,17,10,10,8,18,15,17,15,15,4,24,0,26,4,4,0,14,2,16,17,19,6,18,9,16,17,17,});
sparse_graph3_ints[3][60] = vector<int>({3,24,8,15,20,20,0,10,0,2,10,10,16,22,14,16,12,24,8,26,6,12,16,17,3,19,4,28,18,18,0,0,0,30,6,25,1,7,2,4,10,23,2,2,8,22,3,22,6,10,6,8,1,16,6,6,8,20,10,17,12,14,1,14,0,20,7,14,0,13,12,12,3,26,2,17,11,11,0,23,9,16,14,14,11,18,7,19,6,18,12,19,0,28,13,20,0,26,14,21,});
sparse_graph3_ints[3][70] = vector<int>({4,26,4,22,11,11,0,17,14,15,23,23,3,4,0,0,9,28,0,12,18,18,0,4,11,24,5,5,5,28,13,13,0,31,9,19,16,16,10,30,14,20,15,22,16,24,7,13,6,32,19,20,11,20,21,21,1,22,0,20,2,34,8,10,8,24,4,31,7,18,20,24,5,17,0,2,0,33,10,14,0,15,5,9,4,24,17,26,13,25,14,28,0,29,8,8,0,27,0,25,});
sparse_graph3_ints[3][80] = vector<int>({8,8,0,10,16,32,7,23,10,12,0,40,0,33,1,4,15,21,16,26,22,23,10,35,8,19,4,35,0,2,24,25,6,7,10,24,26,27,9,10,2,19,12,13,0,18,12,20,13,31,5,27,5,37,17,17,0,30,1,27,5,32,6,18,2,24,9,32,12,28,6,29,19,22,15,24,19,19,11,26,4,4,0,23,18,27,0,36,20,28,13,17,0,15,0,0,14,14,22,29,});
sparse_graph3_ints[3][100] = vector<int>({7,29,12,44,23,30,14,14,0,46,0,0,6,7,30,31,9,10,16,26,13,39,7,35,0,9,11,12,0,16,17,33,16,16,8,22,18,18,33,33,11,36,1,20,25,25,20,20,28,36,0,40,6,46,7,42,11,23,22,22,25,33,1,30,28,28,20,27,14,31,23,38,14,24,0,37,0,34,2,4,10,30,20,35,0,50,18,40,5,40,7,19,1,2,0,27,0,24,0,43,});
sparse_graph3_ints[0][6] = vector<int>({0,1,1,1,0,0,0,2,2,2,1,2,0,3,});
sparse_graph3_ints[0][8] = vector<int>({0,2,0,3,1,2,0,0,2,2,2,3,1,3,0,4,0,1,1,1,});
sparse_graph3_ints[0][10] = vector<int>({0,1,1,1,0,2,1,4,1,2,0,3,0,5,0,0,2,3,3,3,2,2,2,4,1,3,0,4,});
sparse_graph3_ints[0][12] = vector<int>({0,6,2,3,0,0,2,4,1,3,0,5,0,3,1,5,0,2,3,4,2,5,3,3,0,4,1,1,1,4,2,2,1,2,4,4,0,1,});
sparse_graph3_ints[0][14] = vector<int>({2,5,1,1,1,5,0,7,0,5,3,3,0,3,0,4,3,5,3,4,1,6,2,3,2,2,0,6,2,4,1,3,0,2,1,2,1,4,4,5,2,6,0,1,4,4,0,0,});
sparse_graph3_ints[0][16] = vector<int>({1,4,2,7,2,6,2,5,0,0,1,6,0,1,1,5,3,4,0,8,3,5,3,6,2,3,3,3,0,6,0,5,5,5,2,4,1,7,0,7,0,3,4,4,1,1,4,6,1,3,0,2,0,4,4,5,1,2,2,2,});
sparse_graph3_ints[0][18] = vector<int>({3,7,0,6,0,2,5,5,2,2,2,7,2,8,4,7,4,6,3,4,5,6,1,4,2,6,1,1,0,7,1,6,1,3,0,3,0,0,1,5,0,9,1,8,3,5,0,8,3,6,1,2,6,6,3,3,4,4,4,5,0,5,1,7,2,5,0,1,2,4,2,3,0,4,});
sparse_graph3_ints[0][20] = vector<int>({2,5,1,4,0,3,0,4,2,9,5,6,4,7,3,4,3,7,0,7,1,6,4,5,2,8,6,6,0,9,1,3,0,2,2,6,0,0,1,5,4,6,5,5,4,4,0,8,3,3,2,4,1,9,2,2,1,2,2,7,3,5,2,3,3,8,6,7,1,1,0,10,0,5,0,6,5,7,1,8,4,8,1,7,3,6,0,1,});
sparse_graph3_ints[0][23] = vector<int>({1,9,3,4,3,3,7,8,0,8,6,8,0,7,2,7,3,9,6,7,2,8,2,5,3,8,4,9,1,1,6,6,2,2,2,3,1,4,4,8,5,9,2,6,7,7,0,6,0,10,5,7,2,9,3,5,4,6,5,5,0,11,4,5,2,10,0,9,3,7,0,2,5,6,1,3,4,7,0,0,1,7,0,1,1,10,4,4,0,3,5,8,1,2,0,5,2,4,1,8,});
sparse_graph3_ints[0][26] = vector<int>({3,7,0,3,4,8,3,3,1,12,2,11,1,11,0,11,1,10,4,9,8,8,4,11,6,10,0,9,2,12,1,4,0,13,2,7,0,2,5,7,1,6,1,5,5,8,2,9,6,9,4,7,4,6,2,8,3,11,0,0,0,1,7,7,3,6,0,6,0,12,3,10,2,4,2,6,1,1,1,7,5,10,3,5,2,5,3,9,1,3,6,7,4,4,5,6,2,3,3,4,});
sparse_graph3_ints[0][30] = vector<int>({0,13,4,8,4,4,1,1,2,8,8,8,4,10,0,9,1,4,8,10,6,8,5,11,1,6,3,9,8,9,2,12,1,13,6,12,2,2,3,6,4,6,7,8,1,8,7,10,3,3,5,8,6,6,0,2,2,14,3,8,1,12,9,9,3,11,1,2,1,7,1,11,3,12,5,5,5,9,2,3,0,4,5,6,3,13,1,3,0,11,2,13,2,5,8,11,0,1,6,7,});
sparse_graph3_ints[0][35] = vector<int>({4,15,6,8,6,6,5,10,4,4,3,10,1,7,6,12,5,6,2,6,0,2,0,8,9,9,5,11,2,7,9,13,2,13,0,7,4,12,8,12,4,5,1,14,7,11,1,2,2,12,6,9,9,10,2,4,3,6,3,7,1,13,5,15,4,10,1,15,7,9,2,15,3,16,10,10,1,1,0,1,4,13,6,11,1,4,0,5,2,9,2,3,5,8,0,10,0,13,2,10,});
sparse_graph3_ints[0][40] = vector<int>({11,12,0,13,0,8,6,15,5,13,5,8,5,15,1,19,2,19,5,7,7,14,8,8,8,13,5,12,10,11,9,14,3,17,3,5,5,16,4,10,2,17,2,7,3,4,2,6,5,6,7,7,8,10,6,17,1,6,2,16,0,20,12,12,7,12,0,12,6,14,1,5,1,16,10,14,0,5,11,13,6,9,3,16,5,5,0,6,4,14,9,10,5,14,0,0,8,11,3,10,});
sparse_graph3_ints[0][45] = vector<int>({0,4,9,17,4,4,3,13,4,19,8,15,4,14,8,12,9,13,1,4,0,19,10,11,12,16,6,16,1,9,3,11,1,13,7,13,3,5,6,10,0,5,1,3,0,15,7,7,1,10,5,17,3,21,2,9,10,12,2,6,0,2,14,14,1,6,4,15,9,18,13,14,10,10,5,9,3,8,2,18,2,16,6,12,1,18,0,8,9,9,5,7,4,13,4,10,0,11,2,20,});
sparse_graph3_ints[0][50] = vector<int>({2,14,8,8,10,20,11,15,3,8,9,16,3,22,4,23,1,12,4,20,4,19,0,22,5,18,3,9,14,18,10,10,2,23,4,17,9,20,2,2,7,18,9,14,0,9,1,18,10,13,13,15,3,7,16,17,0,8,15,15,6,19,3,3,1,5,4,22,8,10,0,5,12,13,8,15,8,20,2,20,5,20,3,15,14,15,10,18,3,6,1,22,3,11,8,18,6,10,2,18,});
sparse_graph3_ints[0][55] = vector<int>({14,18,8,8,4,18,3,23,8,21,3,6,3,13,9,17,10,15,15,17,5,18,4,23,14,15,6,9,2,6,1,21,7,9,3,7,7,11,10,11,3,15,13,20,10,17,4,10,7,21,1,26,5,20,5,7,13,21,0,17,0,7,1,19,5,12,4,19,10,12,4,20,14,17,6,11,1,10,3,17,15,18,14,14,8,19,0,10,6,20,12,17,15,16,10,20,17,19,11,14,});
sparse_graph3_ints[0][60] = vector<int>({5,24,12,14,2,23,9,18,12,19,5,26,1,2,3,11,8,23,10,11,9,20,6,12,14,16,2,9,6,9,9,24,8,16,1,1,6,19,11,16,5,16,5,13,5,9,3,5,1,8,10,17,13,13,5,17,2,21,0,22,0,25,3,25,3,15,1,28,10,15,7,26,3,17,3,16,14,14,6,15,2,7,7,12,5,19,0,24,3,27,8,14,4,7,6,27,11,22,10,24,});
sparse_graph3_ints[0][70] = vector<int>({0,5,21,24,15,19,14,19,2,26,14,28,21,21,1,19,0,24,6,15,3,3,6,31,7,29,7,30,7,9,2,8,3,12,18,18,1,17,12,14,7,7,17,25,2,33,4,28,8,24,5,32,11,13,4,6,2,30,9,16,7,27,14,16,17,18,6,7,2,21,2,11,4,33,11,11,3,27,13,25,20,20,3,7,16,19,12,25,2,10,3,26,4,15,2,28,17,22,4,24,});
sparse_graph3_ints[0][80] = vector<int>({19,23,5,24,2,25,4,38,24,28,1,28,16,31,8,25,7,15,0,3,0,29,5,31,5,26,10,10,18,23,0,6,16,30,14,18,4,22,5,21,11,14,5,29,3,32,10,28,5,20,3,20,3,23,17,19,4,8,2,34,12,28,15,28,4,9,4,37,9,11,6,25,2,32,8,30,0,4,4,4,4,23,0,34,3,15,2,38,16,18,2,8,15,31,3,7,9,10,7,20,});
sparse_graph3_ints[0][100] = vector<int>({9,15,4,13,3,16,22,28,3,34,2,16,7,33,22,23,4,11,8,18,3,45,22,24,26,30,26,32,4,27,4,20,14,17,3,42,27,35,16,29,6,32,17,31,1,30,12,15,15,21,20,24,22,35,15,40,13,43,13,21,24,27,3,36,0,36,8,8,11,13,12,25,11,19,23,34,0,42,8,29,7,38,1,6,12,39,6,25,11,29,4,16,16,37,5,20,2,4,3,24,});




for(int error_rate = 0; error_rate <= 40; ++error_rate){
if(sparse_graph3_ints.count(error_rate) == 0) continue;
for(int num_nodes = 4; num_nodes <= 100; ++num_nodes){
if(sparse_graph3_ints[error_rate].count(num_nodes) == 0) continue;

vector<CompleteGraphSolver::CompleteGraph> v;
// v.push_back(CompleteGraphSolver::CompleteGraph(vector<int>()));
for(int j = 0; j < sz(sparse_graph3_ints[error_rate][num_nodes]); j += 2){
int i0 = sparse_graph3_ints[error_rate][num_nodes][j];
int i1 = sparse_graph3_ints[error_rate][num_nodes][j + 1];
v.push_back(CompleteGraphSolver::CompleteGraph(vector<int>({i0,i1,num_nodes - i0 - i1})));
}
graphs_3_flip[error_rate][num_nodes] = v;
}
my_assert(20 == sz(graphs_3_flip[error_rate]));
}
}

{
unordered_map<int,unordered_map<int,vector<int>>> sparse_graph4_ints;
sparse_graph4_ints[40][6] = vector<int>({0,0,0,1,1,2,0,2,2,0,1,2,1,1,1,0,0,3,0,0,2,0,1,1,0,0,1,});
sparse_graph4_ints[40][8] = vector<int>({2,2,2,0,0,0,0,0,4,1,2,2,1,1,3,0,2,3,1,1,2,0,1,3,0,2,2,0,0,3,0,1,2,0,0,2,1,1,1,0,0,1,0,1,1,});
sparse_graph4_ints[40][10] = vector<int>({2,2,3,0,0,0,0,0,4,1,3,3,2,2,2,1,2,3,0,3,3,1,1,4,0,2,4,0,0,5,1,2,2,0,2,3,0,1,4,1,1,3,0,0,2,0,1,3,1,1,2,0,2,2,0,0,1,0,0,3,0,1,2,1,1,1,0,1,1,});
sparse_graph4_ints[40][12] = vector<int>({3,3,3,0,0,0,0,0,6,2,3,3,2,2,4,1,3,4,0,0,2,0,4,4,2,2,3,1,3,3,0,0,4,0,3,4,1,2,4,0,2,5,1,1,5,0,0,5,1,2,3,0,3,3,2,2,2,1,1,4,0,1,5,0,2,4,0,0,1,1,2,2,0,1,4,0,2,3,1,1,3,0,1,1,1,1,1,0,2,2,0,1,3,0,0,3,1,1,2,0,1,2,});
sparse_graph4_ints[40][14] = vector<int>({3,3,4,0,0,0,0,0,6,2,4,4,1,1,1,3,3,3,2,3,4,1,4,4,0,0,7,0,4,5,0,0,2,2,2,5,1,3,5,2,3,3,1,3,4,0,0,4,0,4,4,2,2,4,0,3,5,1,2,5,0,2,6,1,1,6,0,1,6,2,2,3,0,3,4,1,2,4,1,3,3,0,2,5,0,0,5,1,1,5,0,1,1,0,0,1,0,1,5,0,2,2,0,3,3,1,2,3,2,2,2,1,1,4,0,2,4,0,1,2,0,0,3,1,1,3,1,2,2,0,1,4,0,2,3,0,1,3,1,1,2,});
sparse_graph4_ints[40][16] = vector<int>({4,4,4,0,0,0,0,0,8,1,1,1,3,4,4,3,3,5,2,4,5,0,0,6,1,5,5,3,3,4,0,2,2,2,4,4,0,5,5,1,4,5,2,3,5,0,0,4,0,4,6,0,0,7,1,3,6,3,3,3,2,3,4,2,2,6,1,4,4,0,4,5,0,0,2,2,2,5,1,3,5,0,3,6,1,2,6,0,1,7,1,1,2,1,1,7,0,2,7,2,3,3,1,3,4,0,4,4,0,3,5,0,2,6,2,2,4,1,2,5,0,1,2,1,1,6,0,1,6,1,1,3,0,0,1,0,1,1,0,0,5,2,2,3,0,1,3,1,3,3,0,3,4,1,2,4,2,2,2,0,2,5,0,0,3,1,1,5,1,2,2,0,3,3,0,2,4,0,1,5,1,2,3,0,2,3,1,1,4,0,1,4,});
sparse_graph4_ints[40][18] = vector<int>({4,4,5,0,0,0,0,0,8,0,2,2,3,5,5,4,4,4,0,0,4,3,4,5,0,0,9,0,6,6,2,5,5,3,3,6,1,1,1,1,5,6,2,4,6,3,4,4,0,0,6,2,4,5,3,3,5,0,5,6,2,2,2,1,5,5,1,4,6,2,3,6,0,4,7,3,3,4,0,5,5,0,1,8,1,1,3,1,4,5,0,0,7,2,4,4,2,2,7,2,3,5,0,4,6,0,0,2,1,2,2,1,3,7,0,2,8,1,3,6,0,3,7,1,1,2,2,2,6,1,2,7,1,1,8,3,3,3,1,1,7,2,3,4,0,4,5,1,4,4,0,1,7,2,2,5,0,2,7,0,2,3,0,3,6,1,3,5,1,2,6,0,1,3,2,3,3,0,0,5,0,3,3,1,2,3,0,1,2,0,0,1,0,2,4,1,3,3,0,4,4,0,2,6,1,3,4,0,1,1,2,2,4,0,3,5,2,2,3,0,0,3,1,1,6,1,2,5,0,1,6,0,1,4,0,3,4,1,2,4,1,1,4,1,1,5,0,1,5,0,2,5,});
sparse_graph4_ints[40][20] = vector<int>({5,5,5,0,0,0,0,0,10,0,0,4,2,2,2,4,5,5,4,4,6,3,5,6,0,6,6,0,2,2,0,0,8,4,4,5,2,6,6,3,5,5,0,6,7,3,4,6,0,0,9,1,2,2,2,5,6,1,6,6,1,5,7,4,4,4,2,4,7,0,0,6,3,4,5,3,3,7,2,5,5,1,5,6,0,5,7,1,1,3,3,3,6,2,4,6,1,3,3,0,0,2,0,1,9,1,4,7,1,2,3,0,4,8,2,3,7,3,4,4,0,5,6,1,1,9,2,4,5,3,3,5,1,5,5,0,3,3,1,3,8,1,4,6,0,4,7,2,3,6,1,1,1,2,2,3,2,2,8,0,2,8,0,2,9,2,2,7,0,3,8,0,0,7,0,2,3,0,1,8,1,3,7,3,3,4,1,2,8,0,2,4,0,5,5,2,4,4,1,4,5,2,3,5,1,1,2,1,1,8,0,4,6,3,3,3,1,3,6,0,3,7,2,2,4,2,3,3,0,0,5,1,2,7,0,4,4,2,2,6,2,3,4,1,4,4,0,1,3,0,3,4,0,4,5,1,1,7,1,2,4,1,1,4,0,1,4,0,0,1,0,1,2,0,1,7,1,3,4,2,2,5,1,3,5,0,2,7,0,3,6,0,1,5,0,1,1,1,1,5,});
sparse_graph4_ints[40][23] = vector<int>({5,6,6,0,0,0,0,0,11,0,0,5,2,3,3,5,5,6,4,6,6,1,2,2,0,0,10,0,7,8,4,5,7,5,5,5,2,2,3,0,0,1,3,6,7,4,5,6,2,7,7,2,2,2,3,6,6,0,0,9,0,1,11,1,7,7,1,3,3,0,0,3,3,5,7,4,4,7,0,7,7,0,0,7,2,6,7,1,2,3,0,1,10,1,6,8,0,6,8,4,5,5,4,4,6,1,3,4,2,5,8,0,0,8,3,5,6,2,6,6,3,4,8,1,6,7,1,1,2,3,3,3,2,5,7,0,2,10,3,4,7,1,5,8,2,3,4,0,0,6,1,1,10,0,3,3,0,6,7,3,5,5,0,5,9,4,4,5,2,4,8,0,3,4,3,3,8,1,6,6,3,4,6,0,5,8,1,2,4,0,1,1,2,2,4,2,5,6,1,5,7,0,3,10,1,4,9,1,4,4,0,2,3,0,1,9,3,3,7,2,3,9,2,4,7,0,4,4,1,2,10,4,4,4,0,4,9,1,4,8,0,6,6,1,1,9,3,4,5,2,3,8,3,3,4,1,3,9,2,5,5,1,1,4,0,2,9,0,0,4,0,3,5,2,2,9,0,4,5,2,4,6,0,5,7,1,5,6,1,2,9,1,1,3,0,3,9,3,4,4,});
sparse_graph4_ints[40][26] = vector<int>({6,6,7,0,0,0,0,0,12,0,0,6,3,3,3,1,1,1,0,8,8,5,7,7,6,6,6,2,4,4,0,0,13,0,0,8,2,2,2,5,6,7,0,8,9,4,7,7,3,3,4,5,5,8,4,6,8,0,0,4,0,2,12,0,0,10,2,3,3,5,6,6,3,7,8,5,5,7,2,8,8,0,1,12,4,6,7,2,3,4,1,8,8,1,7,9,3,7,7,4,5,8,2,7,8,3,6,8,0,7,9,2,2,4,5,5,6,4,6,6,0,6,10,1,4,4,0,0,11,0,2,2,3,3,5,1,7,8,2,6,9,4,5,7,3,6,7,4,4,9,2,7,7,3,5,9,3,4,4,1,3,3,1,1,11,2,6,8,3,5,8,0,7,8,1,6,9,4,4,8,1,1,12,4,4,4,0,4,4,5,5,5,4,5,6,1,7,7,2,5,9,0,6,9,3,6,6,0,1,11,3,4,9,0,5,5,4,4,7,2,6,7,1,6,8,3,5,7,1,5,10,1,5,5,2,2,3,2,5,8,2,3,5,0,2,11,0,0,9,4,5,5,0,5,10,3,4,8,3,3,10,2,4,10,1,3,4,0,7,7,1,5,9,0,6,8,0,3,11,4,4,6,1,3,11,1,4,5,3,5,6,1,2,11,2,4,9,2,4,5,});
sparse_graph4_ints[40][30] = vector<int>({7,7,8,0,0,0,0,0,14,0,0,6,4,4,4,0,10,10,1,1,1,0,0,10,3,3,3,6,8,8,0,0,15,7,7,7,3,5,5,2,2,2,6,7,8,0,2,14,0,0,8,2,4,4,5,8,8,0,9,10,4,4,5,6,6,9,6,7,7,5,7,9,0,8,10,0,0,12,0,1,14,3,4,4,0,0,4,1,3,3,6,6,8,3,4,5,5,7,8,4,8,9,1,9,9,3,3,5,1,9,10,4,8,8,3,9,9,5,6,9,1,1,14,0,8,11,4,7,9,0,6,6,6,6,7,2,9,9,3,8,9,5,7,7,0,9,9,2,8,10,3,3,4,5,6,8,4,7,8,5,5,10,4,4,6,1,8,10,3,7,10,1,1,13,4,6,10,2,8,9,3,8,8,2,5,5,0,0,13,4,6,9,5,5,9,4,5,5,2,2,4,3,7,9,1,7,11,6,6,6,1,5,5,5,6,7,2,7,10,1,8,9,1,3,13,5,5,5,4,7,7,2,4,5,0,3,13,2,4,6,2,3,3,0,2,2,0,0,9,0,1,13,0,7,11,3,6,10,5,5,8,4,5,10,3,4,6,2,8,8,3,7,8,4,6,8,1,2,13,0,2,13,0,0,11,0,8,9,2,3,4,1,7,10,2,5,6,0,6,12,});
sparse_graph4_ints[40][35] = vector<int>({8,9,9,0,0,0,0,0,9,0,0,17,4,5,5,1,1,2,0,11,12,2,3,3,0,0,12,0,3,16,8,8,9,5,5,5,0,0,5,3,3,4,7,9,9,0,0,16,5,5,6,0,10,12,7,8,10,0,0,11,3,3,3,4,5,6,8,8,8,0,1,17,4,4,5,0,0,13,1,2,16,7,8,9,0,11,11,6,9,10,4,6,6,0,0,7,3,4,4,6,9,9,1,11,11,3,6,6,1,2,2,0,0,10,0,2,16,0,0,1,2,3,4,7,7,10,5,6,6,4,4,4,0,0,15,6,8,10,1,3,15,0,7,8,0,2,3,0,0,8,3,4,5,5,10,10,0,10,11,7,8,8,0,1,16,0,0,14,4,4,6,7,7,9,1,1,16,1,4,4,4,5,7,1,10,12,5,9,10,3,5,6,6,8,9,1,10,11,6,7,11,0,4,15,2,11,11,0,8,8,4,10,10,3,5,5,5,8,11,5,9,9,6,7,10,2,10,11,5,5,7,4,9,11,1,4,15,3,10,11,0,7,7,0,9,12,5,8,10,2,2,3,2,5,5,4,6,7,7,7,8,0,9,13,6,8,8,3,10,10,3,6,7,4,9,10,2,2,15,1,9,12,3,4,6,6,7,9,6,6,6,3,9,11,6,6,11,5,7,11,});
sparse_graph4_ints[40][40] = vector<int>({10,10,10,0,0,0,4,4,4,0,0,20,0,0,4,0,0,14,6,6,7,0,0,8,0,12,14,0,4,18,2,2,2,5,5,5,0,0,12,3,3,3,9,10,10,6,6,6,9,9,11,0,8,8,2,2,18,0,0,16,0,0,10,2,4,4,1,1,1,0,13,13,8,10,11,5,7,7,3,5,5,9,9,10,1,13,13,3,3,17,8,10,10,0,8,10,7,7,7,4,4,6,0,0,18,8,9,11,1,5,17,7,11,11,6,6,8,0,12,12,9,9,9,4,6,6,7,10,11,0,12,13,1,3,18,0,0,19,8,9,10,0,10,10,5,5,7,1,3,3,3,3,5,0,0,6,6,7,7,4,4,5,8,8,12,5,6,6,1,1,19,7,9,12,1,12,13,0,0,13,5,6,7,8,8,11,0,5,17,7,10,10,3,3,4,5,5,6,6,11,11,0,11,14,7,9,11,0,9,9,0,1,19,0,0,15,1,4,17,4,5,5,5,7,8,1,11,13,6,10,12,8,9,9,5,6,8,2,12,12,6,10,11,0,6,17,2,12,13,8,8,10,4,7,7,7,8,12,1,11,14,5,11,12,7,9,10,1,9,9,0,2,19,0,10,14,4,6,7,0,6,6,6,7,8,6,9,12,2,4,17,1,12,12,4,12,12,0,8,9,});
sparse_graph4_ints[40][45] = vector<int>({0,0,22,0,0,0,11,11,11,0,0,8,6,6,6,0,0,17,0,5,20,4,4,5,1,1,2,0,14,15,0,0,12,7,8,8,2,3,3,10,11,12,0,10,11,4,5,5,0,0,5,3,3,3,0,0,14,5,6,6,3,4,19,10,11,11,0,15,15,7,7,8,0,0,10,0,0,16,10,10,12,5,5,5,0,0,21,1,4,20,0,10,10,3,4,4,9,12,12,7,7,7,0,14,14,0,1,22,0,0,18,0,0,13,5,5,6,8,8,8,0,7,19,9,11,12,0,3,15,6,7,7,4,5,6,3,3,4,1,2,2,0,0,9,0,0,15,10,10,11,0,13,16,4,4,4,0,9,10,2,3,20,6,8,8,9,11,11,1,14,15,9,10,13,0,8,8,7,7,9,2,5,19,6,6,7,0,0,20,0,0,11,3,5,5,8,12,12,0,11,12,1,5,19,9,10,12,5,6,7,7,8,9,8,11,13,1,13,15,6,7,8,1,1,21,0,0,19,1,6,19,3,6,6,2,3,4,1,2,15,0,8,9,6,8,9,10,10,10,3,3,19,1,14,14,8,11,12,0,11,11,0,1,21,6,7,9,0,6,19,9,9,13,0,13,15,5,7,7,3,5,6,9,10,11,6,6,8,2,14,14,0,3,21,0,10,12,8,10,13,});
sparse_graph4_ints[40][50] = vector<int>({12,12,13,0,0,0,0,0,24,3,3,3,7,7,7,0,0,16,0,6,22,0,2,2,0,16,16,0,0,12,5,5,5,9,9,9,0,0,20,0,0,8,0,12,12,0,4,18,6,6,6,4,4,4,11,13,13,4,6,20,0,0,25,0,0,18,0,0,14,0,0,4,0,10,10,1,3,3,0,0,10,3,5,5,8,8,8,12,12,12,0,16,17,0,8,8,2,4,22,0,4,16,8,9,9,11,12,13,4,6,6,2,4,4,6,6,8,8,8,10,3,5,21,11,11,14,1,15,17,0,12,14,10,13,13,11,12,12,8,8,9,5,7,7,0,14,18,10,12,14,1,7,21,11,11,13,7,9,9,0,15,17,0,2,24,1,3,17,0,0,22,5,5,20,0,10,12,8,9,10,4,4,6,6,6,7,1,3,23,10,12,13,1,16,16,9,9,10,6,8,8,0,8,21,9,13,14,2,16,16,11,11,12,0,12,13,10,11,14,8,10,10,7,8,8,0,8,10,4,4,21,9,13,13,10,12,12,7,7,9,0,14,14,5,5,7,7,8,9,0,15,16,9,12,14,0,7,21,0,6,16,7,9,10,10,11,13,1,15,16,0,10,20,7,7,8,0,1,24,2,6,21,0,13,13,10,10,15,9,10,10,8,14,14,9,12,13,11,11,11,});
sparse_graph4_ints[40][55] = vector<int>({0,0,0,13,14,14,0,0,27,0,10,10,2,3,3,0,0,16,5,6,22,10,10,10,0,0,22,4,5,5,0,18,18,0,0,12,1,1,2,0,5,19,8,8,8,0,0,8,0,5,25,0,14,14,5,6,6,0,0,18,13,13,14,3,4,4,7,7,8,0,0,14,0,0,20,0,0,26,0,7,24,9,10,10,6,6,7,4,4,5,0,0,5,0,0,10,0,12,12,0,3,16,0,6,19,12,14,14,0,17,18,5,5,6,9,9,9,6,7,21,13,13,13,3,3,3,7,7,7,10,10,11,12,13,15,0,17,19,0,3,26,0,0,23,0,9,23,0,14,15,8,8,9,0,5,18,0,8,8,0,0,17,0,0,13,6,6,6,4,4,4,12,13,14,7,8,8,9,10,11,0,0,21,0,1,27,4,5,23,1,17,18,0,10,11,0,13,14,5,5,5,0,0,15,0,0,19,0,5,20,6,7,7,9,9,10,11,14,15,0,10,22,1,18,18,12,12,15,8,9,9,10,11,11,0,4,17,11,14,14,1,6,24,0,12,13,6,7,8,9,9,11,0,16,19,11,13,15,0,9,10,4,5,6,7,8,9,6,6,21,0,6,18,0,2,26,0,0,24,0,6,24,9,11,11,12,13,13,8,9,10,5,6,7,0,12,14,5,7,21,});
sparse_graph4_ints[40][60] = vector<int>({15,15,15,3,3,3,0,0,30,0,0,0,9,9,9,0,0,22,0,0,14,0,8,26,0,20,20,7,7,7,0,0,4,5,5,5,11,11,12,1,5,21,0,0,18,0,0,8,0,16,16,0,12,12,0,0,26,7,7,23,0,4,16,0,0,12,0,4,28,4,4,4,6,6,6,2,2,2,14,15,15,10,10,10,0,6,18,8,8,8,0,8,8,0,14,14,0,0,16,0,0,20,0,0,24,0,10,10,0,0,10,1,1,1,3,3,5,0,6,22,11,11,11,14,14,16,0,12,24,0,19,20,4,6,6,6,8,8,13,15,16,9,9,10,12,12,12,6,6,24,0,4,18,2,2,28,14,14,15,1,19,19,0,14,16,8,8,9,10,11,11,0,8,20,8,8,22,13,15,15,5,7,7,3,5,5,1,3,3,10,12,12,1,19,20,13,14,16,5,5,25,8,10,10,0,18,20,12,16,16,10,10,12,0,12,14,7,7,8,14,14,14,0,6,20,1,15,15,11,12,12,7,9,9,5,5,7,9,10,10,0,10,12,6,8,23,1,5,27,10,10,11,0,16,18,12,15,16,0,18,21,13,14,15,11,11,13,8,9,9,13,13,17,0,10,25,10,11,12,0,19,19,7,8,22,0,15,16,12,14,17,2,18,20,13,13,16,9,11,11,});
sparse_graph4_ints[38][6] = vector<int>({1,1,2,0,0,0,0,2,2,0,1,2,1,1,1,0,0,3,0,0,2,0,1,1,0,0,1,});
sparse_graph4_ints[38][8] = vector<int>({2,2,2,0,0,0,0,0,4,1,2,2,0,2,3,1,1,3,1,1,2,0,1,3,0,2,2,0,0,3,0,0,2,0,1,2,1,1,1,0,0,1,0,1,1,});
sparse_graph4_ints[38][10] = vector<int>({2,2,3,0,0,0,0,0,4,1,3,3,2,2,2,0,3,3,1,2,3,0,0,2,1,1,4,0,2,4,0,0,5,1,2,2,0,2,3,0,1,4,1,1,3,0,1,3,1,1,2,0,2,2,0,0,1,0,0,3,1,1,1,0,1,1,0,1,2,});
sparse_graph4_ints[38][12] = vector<int>({3,3,3,0,0,0,0,0,6,0,0,2,2,3,3,2,2,4,1,3,4,0,4,4,0,0,4,2,2,3,1,3,3,0,3,4,1,2,4,1,1,1,0,2,5,1,1,5,0,0,5,0,3,3,1,2,3,2,2,2,0,1,5,1,1,4,0,2,4,0,1,1,0,0,1,0,2,3,0,1,4,1,2,2,0,2,2,1,1,3,0,0,3,0,1,2,0,1,3,1,1,2,});
sparse_graph4_ints[38][14] = vector<int>({3,3,4,0,0,0,0,0,6,1,1,1,2,4,4,3,3,3,2,3,4,0,0,2,0,0,7,1,4,4,0,4,5,0,0,4,2,2,5,1,3,5,0,4,4,2,3,3,0,2,2,1,3,4,2,2,4,0,3,5,1,2,5,0,2,6,0,1,6,1,1,6,0,1,2,0,3,4,2,2,3,1,3,3,1,2,4,0,2,5,0,0,5,1,1,5,0,1,1,0,1,5,2,2,2,1,1,2,0,3,3,1,2,3,0,0,3,0,2,4,0,1,3,0,0,1,1,1,4,1,1,3,1,2,2,0,2,3,0,1,4,});
sparse_graph4_ints[38][16] = vector<int>({4,4,4,0,0,0,0,0,8,0,2,2,3,4,4,0,0,2,3,3,5,2,4,5,0,0,6,1,5,5,1,1,2,0,5,5,3,3,4,2,4,4,0,0,4,1,4,5,0,4,6,2,3,5,0,0,7,1,1,1,1,3,6,0,4,5,3,3,3,2,3,4,1,4,4,2,2,6,1,1,3,2,2,5,1,3,5,0,3,6,0,1,7,2,2,2,1,1,7,1,2,6,0,2,7,1,2,2,0,4,4,2,3,3,1,3,4,0,1,2,0,2,6,0,3,5,2,2,4,0,1,3,1,2,5,1,1,6,0,2,3,0,1,6,0,0,5,1,3,3,2,2,3,0,0,3,0,3,3,0,3,4,1,2,4,0,2,4,0,1,1,1,2,3,0,2,5,1,1,5,0,1,4,1,1,4,0,1,5,0,0,1,});
sparse_graph4_ints[38][18] = vector<int>({4,4,5,0,0,0,0,0,8,0,0,4,2,2,2,3,5,5,0,6,6,0,2,2,4,4,4,0,0,9,3,4,5,2,5,5,0,0,2,0,0,6,1,5,6,1,2,2,3,3,6,2,4,6,0,5,6,3,4,4,2,4,5,1,5,5,3,3,5,1,1,3,1,4,6,0,1,8,2,3,6,0,4,7,0,3,3,1,1,1,0,2,8,0,5,5,3,3,4,1,3,3,0,4,6,0,0,7,2,4,4,1,4,5,2,2,7,1,3,7,2,3,5,0,2,3,1,3,6,0,3,7,1,2,3,1,1,8,2,2,6,3,3,3,1,2,7,1,1,7,1,1,2,0,4,5,0,2,4,2,3,4,0,1,7,1,4,4,0,2,7,2,2,3,2,2,5,0,1,3,0,0,5,1,3,5,0,3,6,0,4,4,2,3,3,1,2,6,2,2,4,0,1,2,1,3,4,0,3,4,1,1,4,0,2,6,0,1,4,0,3,5,1,2,4,0,0,3,1,2,5,0,1,6,1,1,6,0,1,5,1,1,5,0,2,5,0,1,1,0,0,1,});
sparse_graph4_ints[38][20] = vector<int>({5,5,5,0,0,0,0,0,10,0,2,2,1,3,3,0,0,6,4,5,5,0,0,2,0,6,6,4,4,6,2,2,2,3,5,6,4,4,5,0,6,7,0,0,8,0,0,4,2,6,6,2,2,3,3,5,5,0,0,9,1,6,6,3,4,6,2,5,6,1,5,7,0,3,3,4,4,4,2,4,7,0,4,4,1,1,1,3,4,5,0,1,9,0,5,7,3,3,7,1,5,6,2,5,5,1,2,3,3,3,6,1,1,9,2,4,6,2,2,4,1,4,7,0,4,8,3,3,3,1,1,3,2,3,7,0,5,6,3,4,4,2,3,3,2,4,5,1,5,5,3,3,5,1,2,2,1,3,8,0,4,7,0,2,9,1,4,6,0,3,4,0,2,8,2,2,8,2,3,6,0,2,4,0,0,7,0,3,8,3,3,4,2,4,4,0,2,3,1,3,7,2,2,7,0,1,8,1,2,8,0,5,5,1,3,4,1,2,4,1,4,5,1,1,2,2,3,4,1,1,8,0,0,5,0,4,6,2,3,5,1,4,4,0,4,5,1,1,4,1,3,6,0,3,5,0,1,3,0,3,7,1,3,5,2,2,6,1,2,7,2,2,5,1,1,5,0,1,2,0,2,5,0,1,4,1,1,7,1,2,5,0,1,5,0,1,7,0,3,6,});
sparse_graph4_ints[38][23] = vector<int>({5,6,6,0,0,0,0,0,11,1,2,2,3,3,3,0,0,7,0,7,8,0,0,3,5,5,6,2,3,3,0,0,10,4,6,6,4,5,7,5,5,5,0,1,11,2,3,4,0,0,6,1,1,2,1,7,7,3,6,7,2,7,7,1,4,4,4,5,6,2,2,3,0,0,1,0,0,5,0,6,8,0,0,9,3,6,6,0,7,7,0,3,3,0,1,10,3,3,4,4,4,7,3,5,7,0,0,8,2,6,7,0,2,10,1,3,4,1,6,8,0,1,2,0,4,4,4,5,5,4,4,6,1,3,3,1,1,10,1,6,7,2,5,8,2,6,6,3,5,6,3,4,8,0,4,5,0,2,3,2,5,7,2,4,4,0,3,4,0,6,7,3,4,7,1,5,8,2,2,4,0,3,10,0,0,4,2,2,2,0,5,5,0,5,9,4,4,5,3,5,5,2,4,8,3,4,4,1,2,3,1,2,10,1,6,6,3,3,8,1,4,5,0,5,8,3,4,6,0,3,5,1,5,7,1,4,9,2,5,6,4,4,4,1,2,4,2,3,5,0,4,9,2,3,9,0,6,6,2,2,5,3,3,7,0,1,9,2,4,7,3,4,5,1,3,9,1,1,9,1,4,8,1,3,5,2,5,5,0,2,2,2,4,5,2,2,9,1,2,5,});
sparse_graph4_ints[38][26] = vector<int>({6,6,7,0,0,0,0,0,8,0,0,12,1,1,1,4,4,4,2,2,2,0,8,8,3,3,3,5,7,7,0,0,13,0,0,6,6,6,6,2,4,4,0,2,12,0,0,10,0,0,4,5,6,7,0,8,9,3,4,4,1,3,3,4,7,7,5,5,8,3,3,4,4,6,8,1,7,9,5,6,6,0,1,12,3,3,5,5,5,7,1,8,8,3,7,8,2,8,8,4,6,7,1,5,5,0,7,9,3,7,7,0,4,4,0,6,10,2,7,8,4,5,8,0,2,2,0,5,5,3,6,8,1,1,12,5,5,6,0,6,6,4,6,6,1,7,8,2,2,4,2,6,9,2,4,5,4,5,7,3,6,7,4,4,9,2,7,7,3,5,9,0,7,8,3,4,5,2,6,8,5,5,5,1,6,9,2,3,4,3,5,8,1,1,11,3,5,5,1,3,11,2,3,3,0,0,11,4,4,8,0,0,9,0,3,11,1,7,7,4,5,6,1,4,4,2,5,5,0,6,9,1,4,5,2,2,3,0,0,2,1,1,3,0,5,6,1,2,11,4,4,5,2,5,9,0,4,5,3,6,6,2,3,5,0,0,7,0,3,3,3,4,9,0,1,11,1,5,10,4,5,5,4,4,7,0,4,6,2,6,7,0,2,11,1,6,8,3,5,7,});
sparse_graph4_ints[38][30] = vector<int>({7,7,8,0,0,0,0,0,14,4,4,4,0,0,4,0,0,10,1,3,3,0,10,10,1,3,13,0,0,8,1,1,1,6,8,8,5,5,5,3,3,3,0,6,6,7,7,7,0,0,15,0,0,6,0,0,12,2,2,2,3,5,5,2,4,4,0,2,14,6,7,8,0,9,10,4,4,6,5,8,8,6,6,9,0,8,10,6,7,7,4,5,5,5,7,9,3,3,5,0,4,13,6,6,8,1,9,9,4,4,5,5,7,8,1,9,10,4,8,9,0,4,4,0,1,14,0,7,7,3,4,4,0,8,11,4,8,8,3,9,9,5,6,9,1,1,14,2,6,6,6,6,7,2,9,9,4,7,9,0,9,9,5,7,7,3,4,5,3,8,9,2,8,10,3,5,6,0,3,13,0,6,7,0,0,11,2,2,4,0,2,2,3,3,4,5,6,8,4,5,6,1,8,10,0,6,12,5,5,10,4,7,8,1,5,5,6,6,6,3,7,10,0,8,8,4,6,10,1,2,13,2,8,9,2,5,5,3,8,8,4,6,6,1,7,11,4,6,9,0,6,8,5,5,9,3,7,9,5,6,7,1,6,6,2,2,13,3,4,6,1,1,13,2,3,3,2,5,6,0,0,9,0,0,13,5,5,6,1,8,9,2,7,10,2,4,5,0,7,11,});
sparse_graph4_ints[38][35] = vector<int>({0,0,17,0,0,0,8,9,9,2,3,3,5,6,6,0,0,10,0,0,3,0,11,12,4,4,5,0,5,15,0,0,13,0,0,6,1,2,2,3,4,4,8,8,9,0,8,8,0,0,8,3,3,3,5,5,5,0,0,12,0,3,16,0,0,16,7,9,9,4,4,4,6,6,6,0,10,12,8,8,8,1,4,15,7,8,10,5,5,6,0,0,14,0,1,17,0,0,11,0,7,8,3,3,4,0,3,3,1,11,11,4,5,5,7,8,9,2,3,15,5,6,7,0,0,9,3,4,5,6,9,10,0,11,11,0,8,9,0,1,2,2,2,3,0,5,5,0,0,7,0,6,7,4,6,6,6,9,9,1,2,16,0,0,15,2,5,5,5,5,7,7,7,10,1,3,15,4,5,6,6,8,10,0,10,11,2,3,4,0,7,7,4,6,7,7,8,8,0,2,16,5,10,10,6,6,7,1,10,12,0,1,12,0,4,15,7,7,9,0,6,6,4,5,7,1,10,11,3,6,6,5,9,10,0,6,14,2,11,11,4,7,7,6,8,9,3,5,5,2,4,4,1,7,8,6,7,11,2,4,14,2,10,11,0,1,16,0,9,9,4,4,6,4,10,10,3,6,7,5,7,7,5,8,11,2,2,15,3,10,11,6,7,10,0,10,10,5,9,9,});
sparse_graph4_ints[38][40] = vector<int>({10,10,10,0,0,0,0,0,20,5,5,5,0,0,12,0,2,2,3,3,3,0,12,14,7,7,7,0,4,18,0,0,16,0,0,6,0,0,10,1,3,13,4,4,4,0,10,10,4,4,16,9,10,10,6,6,6,0,0,14,0,0,2,2,2,2,0,0,8,0,8,8,2,4,4,3,5,5,9,9,11,1,13,13,0,0,4,1,3,3,6,7,7,0,6,6,1,5,17,9,9,10,8,10,11,0,13,13,4,6,6,7,7,8,8,10,10,6,6,7,4,4,6,0,8,10,3,3,17,6,8,8,0,4,14,3,3,5,5,5,6,8,9,11,6,6,8,0,12,12,7,11,11,0,4,4,0,6,8,9,9,9,5,7,7,0,6,17,0,12,13,1,1,19,0,0,18,0,8,16,6,7,8,7,10,11,5,6,6,2,2,18,0,2,14,4,5,5,0,9,10,8,9,10,1,12,13,8,8,12,2,6,16,1,9,9,7,8,8,5,5,7,7,9,12,5,7,8,8,8,11,2,12,12,7,10,10,0,10,12,2,4,17,2,12,13,7,7,9,5,6,7,3,4,4,4,5,6,0,9,9,6,11,11,7,9,11,5,8,8,1,11,13,8,9,9,0,5,17,0,11,14,8,8,8,6,10,12,0,10,11,5,5,15,8,8,10,1,11,14,});
sparse_graph4_ints[38][45] = vector<int>({0,0,0,11,11,11,0,0,14,0,0,22,0,0,6,7,7,7,3,4,4,0,5,20,0,15,15,0,0,18,0,8,8,0,0,10,0,0,3,0,12,12,2,3,3,0,4,16,4,5,18,4,5,5,8,8,9,6,6,6,10,11,12,0,0,12,0,0,16,0,3,3,0,10,10,0,0,8,4,4,4,0,14,15,10,11,11,8,8,8,5,5,6,1,1,2,3,3,3,0,3,13,4,4,5,6,6,7,0,7,19,0,5,15,0,1,22,0,0,19,0,0,15,0,0,11,7,8,8,10,10,12,9,12,12,5,5,17,1,14,15,0,11,12,5,5,5,6,7,7,8,9,9,3,3,4,0,8,9,0,0,13,0,4,15,0,3,21,0,6,7,5,6,6,7,7,8,0,8,18,0,0,17,0,0,21,9,11,12,0,10,11,1,4,20,3,4,5,2,2,3,0,0,9,0,14,14,7,8,9,0,5,16,5,6,7,4,5,6,10,10,11,0,9,10,7,9,9,3,4,19,0,3,14,0,7,8,6,7,8,0,13,16,9,11,11,5,6,17,0,10,12,9,10,13,7,7,9,5,7,7,2,5,5,0,9,18,1,2,21,4,6,6,1,4,15,6,8,8,0,12,13,8,12,12,2,14,14,9,10,12,7,8,10,1,6,19,6,6,8,});
sparse_graph4_ints[38][50] = vector<int>({0,16,17,3,3,3,0,0,24,0,0,0,0,0,16,9,9,9,0,8,8,0,2,2,0,4,18,12,12,13,0,0,12,5,7,19,7,7,7,0,0,20,1,3,23,0,0,6,0,12,12,4,4,4,0,0,10,0,4,14,0,0,2,5,5,5,0,0,14,0,10,20,11,13,13,0,14,14,0,10,10,8,8,8,10,10,10,2,2,2,0,0,8,0,6,18,0,0,18,6,6,6,0,16,16,12,12,12,0,6,22,0,0,22,0,0,25,0,6,6,0,4,16,0,4,4,0,0,4,9,9,10,11,12,13,6,6,19,2,4,22,7,8,8,4,4,16,5,7,7,3,5,5,0,4,12,2,4,4,4,6,6,0,12,14,2,16,16,8,8,9,0,10,12,6,7,7,9,10,10,4,6,20,11,11,14,10,13,13,1,15,17,8,10,10,11,12,12,7,7,8,8,9,9,0,6,16,0,2,24,3,5,21,6,8,18,10,12,14,1,16,16,0,13,13,9,9,11,11,11,13,4,4,6,0,8,10,6,8,8,5,6,6,7,9,9,2,4,18,1,3,15,1,7,21,6,6,8,8,8,10,1,13,13,0,14,18,7,7,18,0,11,11,10,12,13,8,9,10,7,7,9,0,8,21,0,15,17,9,10,11,0,12,13,1,5,17,5,5,20,});
sparse_graph4_ints[38][55] = vector<int>({0,7,24,0,0,0,0,0,14,13,14,14,7,7,7,0,0,6,0,0,22,0,14,14,4,4,5,0,0,27,0,18,18,0,0,18,0,0,10,0,5,18,9,9,9,0,0,3,7,8,20,5,6,6,3,3,3,11,11,11,0,4,15,0,8,19,0,12,12,1,2,2,0,11,22,0,3,26,0,8,8,0,0,24,0,0,20,0,0,16,0,0,12,0,10,10,8,8,8,10,10,10,13,13,14,0,0,8,3,4,4,0,4,21,0,15,16,2,2,3,5,5,5,0,5,16,6,7,7,1,18,18,12,14,14,8,9,9,10,11,11,6,7,21,5,5,18,0,5,25,0,3,13,0,6,7,6,6,6,7,8,8,3,3,4,0,12,13,10,10,11,13,13,13,12,13,15,0,14,15,0,17,19,9,10,10,8,8,19,4,5,23,8,8,9,0,5,17,4,5,5,6,6,7,0,5,20,0,12,21,1,4,25,0,0,26,0,0,23,0,0,19,0,11,12,0,4,14,0,0,13,0,0,17,0,0,21,4,5,18,7,7,8,0,9,10,9,9,10,0,6,17,5,5,6,0,0,15,0,0,11,4,4,4,0,1,27,10,11,12,1,17,18,5,6,22,12,13,14,0,13,14,0,5,21,0,15,15,7,8,9,0,10,11,0,4,17,5,6,7,});
sparse_graph4_ints[38][60] = vector<int>({15,15,15,0,0,14,0,0,0,0,0,30,10,10,10,3,3,3,0,4,20,0,12,24,0,10,10,0,2,2,0,20,20,0,0,18,5,5,5,0,0,26,0,16,16,0,0,10,0,4,28,8,8,8,12,12,12,0,0,22,0,10,20,0,0,6,0,14,14,9,9,21,0,6,16,5,5,25,0,4,24,4,4,4,6,6,6,0,4,14,0,0,2,0,12,12,0,8,18,0,0,12,0,0,16,0,0,20,2,2,2,0,0,8,9,9,9,7,7,7,0,0,24,0,8,8,11,11,11,6,6,20,2,4,4,3,5,5,14,15,15,0,6,22,0,16,18,0,8,26,8,8,22,6,18,18,0,4,16,14,14,16,12,12,13,0,0,4,1,3,3,10,10,11,8,8,9,5,7,7,0,4,12,5,5,17,0,6,6,4,6,6,7,7,8,0,4,18,9,10,10,11,12,12,3,3,27,7,7,23,0,19,20,0,14,23,13,15,16,0,14,16,0,12,14,6,6,18,10,10,20,11,11,12,14,14,15,0,17,17,1,19,20,8,9,9,0,10,12,0,4,22,9,9,10,10,11,11,6,8,8,11,13,13,0,6,18,13,15,15,6,6,7,7,9,9,9,11,11,11,11,13,8,10,10,5,5,19,0,15,15,6,6,24,0,12,20,1,19,19,});
sparse_graph4_ints[36][6] = vector<int>({0,2,2,0,0,0,1,1,2,0,1,2,0,0,3,1,1,1,0,0,2,0,1,1,0,0,1,});
sparse_graph4_ints[36][8] = vector<int>({2,2,2,0,0,0,0,0,4,1,2,2,0,2,3,1,1,3,1,1,2,0,2,2,0,0,2,0,1,3,0,0,3,0,1,2,1,1,1,0,0,1,0,1,1,});
sparse_graph4_ints[36][10] = vector<int>({2,2,3,0,0,0,0,0,4,1,3,3,0,0,2,2,2,2,0,3,3,1,2,3,0,0,5,0,2,4,1,1,4,0,2,3,1,2,2,0,1,4,1,1,3,0,1,1,0,2,2,0,1,3,0,0,1,1,1,2,1,1,1,0,0,3,0,1,2,});
sparse_graph4_ints[36][12] = vector<int>({3,3,3,0,0,0,0,0,6,0,0,2,2,3,3,0,4,4,1,1,1,1,3,4,2,2,4,0,0,4,2,2,3,0,3,4,1,3,3,1,2,4,0,2,5,0,1,1,0,0,5,1,1,5,0,3,3,0,2,2,1,2,3,0,1,5,2,2,2,0,2,4,1,1,4,0,1,2,0,2,3,0,0,3,1,2,2,0,1,4,1,1,3,0,0,1,1,1,2,0,1,3,});
sparse_graph4_ints[36][14] = vector<int>({3,3,4,0,0,0,0,0,6,0,2,2,0,0,2,2,4,4,3,3,3,0,0,7,0,0,4,2,3,4,0,4,5,1,4,4,1,1,1,1,3,5,2,2,5,0,4,4,2,3,3,1,3,4,0,3,5,2,2,4,1,1,2,0,2,6,1,2,5,0,1,6,0,1,2,1,1,6,0,3,4,2,2,3,1,3,3,1,2,4,0,0,5,0,2,5,1,1,3,1,1,5,0,1,1,2,2,2,0,1,3,0,0,3,0,3,3,0,1,5,1,2,2,0,2,4,1,2,3,0,2,3,1,1,4,0,1,4,0,0,1,});
sparse_graph4_ints[36][16] = vector<int>({4,4,4,0,0,0,0,0,4,0,0,8,0,0,2,2,2,2,3,4,4,1,5,5,0,2,2,3,3,5,2,4,5,0,5,5,0,0,6,0,4,6,3,3,4,2,4,4,1,2,2,1,4,5,1,1,1,2,3,5,0,0,7,1,3,6,1,1,3,0,4,5,3,3,3,1,4,4,2,2,6,2,3,4,0,2,3,0,1,7,2,2,5,1,3,5,1,1,7,0,3,6,0,3,3,1,1,2,0,4,4,1,2,6,0,2,7,2,3,3,1,3,3,0,1,2,1,3,4,0,2,6,2,2,4,0,3,5,0,1,3,1,2,3,0,0,5,0,2,4,0,0,3,2,2,3,1,1,6,1,2,5,0,3,4,0,1,6,0,1,1,0,1,4,1,2,4,1,1,4,0,2,5,1,1,5,0,1,5,0,0,1,});
sparse_graph4_ints[36][18] = vector<int>({4,4,5,0,0,0,0,0,8,2,2,2,0,0,2,0,6,6,0,2,2,0,0,6,3,5,5,0,0,9,4,4,4,0,0,4,1,3,3,3,4,5,2,5,5,1,5,6,0,3,3,3,3,6,0,5,6,2,4,6,1,1,1,3,4,4,1,5,5,2,2,3,0,2,8,2,4,5,3,3,5,1,2,3,0,1,8,1,4,6,0,4,7,2,3,6,1,2,2,0,5,5,0,4,4,3,3,4,0,4,6,2,4,4,1,3,7,1,4,5,1,1,3,2,2,7,3,3,3,0,0,7,2,3,5,0,2,3,1,1,8,0,3,4,1,3,6,0,3,7,0,2,4,1,1,7,2,3,3,2,2,6,1,2,7,2,2,4,0,4,5,0,1,2,0,0,5,2,3,4,1,4,4,1,2,4,0,1,7,1,1,2,1,3,4,0,2,7,1,3,5,1,1,4,2,2,5,0,3,6,0,1,3,0,3,5,0,0,3,0,1,4,1,2,6,0,2,5,0,1,5,0,2,6,1,1,5,1,2,5,0,1,1,0,1,6,1,1,6,0,0,1,});
sparse_graph4_ints[36][20] = vector<int>({5,5,5,0,0,0,0,0,10,0,2,2,0,4,4,0,0,6,0,0,2,0,6,6,4,5,5,2,2,2,4,4,6,0,0,4,3,3,3,0,6,7,3,5,6,1,3,3,0,0,8,1,1,9,4,4,5,2,6,6,3,5,5,1,6,6,2,3,3,3,4,6,1,1,1,0,3,3,2,5,6,1,5,7,0,0,9,2,2,3,4,4,4,0,1,9,0,5,7,2,4,7,2,2,4,3,4,5,3,3,7,1,5,6,2,5,5,0,4,8,3,3,6,0,3,4,2,4,6,1,4,7,1,3,4,1,1,3,0,5,6,3,4,4,0,2,9,2,3,7,2,4,4,1,5,5,1,2,3,3,3,5,2,4,5,2,3,4,2,2,8,1,3,8,0,0,7,0,2,8,1,4,4,0,4,7,3,3,4,0,2,4,0,3,8,1,4,6,0,5,5,0,4,5,1,2,2,1,2,4,2,3,6,1,2,8,0,3,5,1,3,7,1,4,5,0,2,3,0,4,6,1,3,5,2,2,7,2,3,5,0,0,5,0,1,2,0,1,8,2,2,5,1,1,8,1,1,4,0,2,5,1,2,5,1,1,2,1,1,5,1,3,6,2,2,6,0,3,7,0,2,6,1,2,7,0,1,5,0,1,3,0,3,6,0,1,6,0,0,3,});
sparse_graph4_ints[36][23] = vector<int>({0,0,0,5,6,6,0,0,6,0,0,11,3,3,4,0,0,3,0,3,3,0,7,8,0,0,8,5,5,6,3,3,3,0,1,11,0,1,2,4,6,6,2,3,3,1,2,2,0,0,10,4,5,7,0,5,5,0,0,7,1,7,7,5,5,5,0,2,10,1,4,4,3,6,7,0,6,8,0,0,5,2,3,4,4,5,6,2,2,3,0,0,9,2,7,7,0,3,10,3,4,4,3,6,6,0,7,7,0,4,5,0,3,4,1,1,2,0,0,1,0,0,4,4,4,7,2,4,4,3,5,7,2,6,7,0,1,10,1,6,8,0,4,4,0,2,3,1,1,10,1,3,4,4,5,5,1,4,5,2,2,2,1,6,7,4,4,6,1,2,10,2,3,5,2,5,8,3,5,6,2,6,6,3,4,8,2,4,5,1,3,3,0,6,7,4,4,4,2,5,7,2,2,4,1,5,8,0,5,9,0,5,6,3,4,7,4,4,5,1,2,3,3,3,5,3,5,5,0,3,5,0,2,2,2,2,5,1,4,9,2,4,8,3,4,5,1,3,5,0,5,8,1,6,6,0,6,6,3,3,8,0,4,9,1,5,5,3,4,6,1,2,4,1,3,9,2,5,6,2,3,9,2,5,5,1,5,7,0,4,6,2,2,9,1,5,6,1,4,6,});
sparse_graph4_ints[36][26] = vector<int>({6,6,7,0,0,0,0,0,12,3,3,3,0,2,2,0,0,8,0,8,8,4,4,4,0,0,6,0,0,2,1,3,11,2,2,2,0,0,10,5,7,7,0,0,13,0,6,6,6,6,6,2,4,4,0,0,4,1,3,3,0,2,12,0,8,9,5,6,7,3,4,4,0,4,4,3,5,5,4,7,7,5,5,8,3,3,5,1,7,9,5,6,6,4,6,8,1,8,8,1,1,12,1,5,5,5,5,7,2,8,8,4,4,5,3,7,8,0,6,10,4,6,7,0,5,5,0,7,9,3,4,5,3,7,7,0,1,12,3,3,4,0,3,3,1,1,1,2,2,4,0,3,11,0,0,9,2,5,5,2,7,8,4,5,8,5,5,6,3,6,8,0,5,6,5,5,5,1,7,8,4,6,6,0,4,11,2,4,5,2,3,3,4,5,5,2,6,9,0,7,8,4,5,7,1,5,6,4,4,9,1,2,11,3,5,9,3,6,7,2,7,7,4,4,6,2,4,6,1,6,9,1,4,4,2,6,8,1,7,7,4,5,6,2,2,11,0,6,7,3,5,8,0,4,6,4,4,8,0,6,8,3,4,6,2,4,10,1,4,5,2,2,3,3,3,6,1,1,11,0,0,11,0,5,10,0,6,9,0,7,7,1,6,6,0,4,5,3,6,6,});
sparse_graph4_ints[36][30] = vector<int>({7,7,8,0,0,0,3,3,3,0,0,14,0,2,2,0,0,10,5,5,5,0,10,10,0,0,6,1,3,13,0,6,6,0,0,2,2,2,2,0,0,8,0,0,12,6,8,8,4,4,4,0,0,4,0,4,4,0,0,15,0,4,13,7,7,7,4,5,5,2,4,4,0,8,8,6,7,8,0,2,10,0,2,14,4,4,6,1,3,3,0,5,5,3,5,5,0,9,10,4,6,6,5,8,8,2,2,13,6,6,9,0,6,8,6,7,7,1,9,9,5,7,9,4,4,5,6,6,8,1,9,10,2,4,12,5,5,6,3,3,5,0,8,10,0,7,7,5,7,8,0,6,12,4,5,6,4,8,9,3,4,4,3,9,9,6,6,7,0,8,11,0,3,13,1,7,7,4,8,8,5,6,9,6,6,6,2,9,9,3,5,6,5,7,7,2,8,10,0,9,9,4,7,9,0,6,7,0,5,12,5,6,6,3,8,9,1,5,5,2,6,6,3,6,6,1,1,14,0,1,14,0,0,11,2,3,3,3,4,5,1,8,10,5,6,8,0,7,8,2,3,12,5,5,7,5,5,10,3,3,4,1,6,6,3,5,7,1,4,12,1,7,11,4,6,10,4,7,8,5,6,7,3,7,10,2,8,9,4,4,7,3,3,12,3,4,6,5,5,9,});
sparse_graph4_ints[36][35] = vector<int>({0,0,0,0,0,17,8,9,9,0,6,7,0,0,6,0,0,10,0,5,15,6,6,6,0,0,3,0,0,14,3,3,3,0,11,12,0,3,3,0,8,9,0,0,12,0,0,8,3,4,4,5,5,5,0,3,12,8,8,9,0,3,16,0,1,2,2,3,3,4,4,4,6,6,7,1,2,2,4,5,5,3,4,14,7,9,9,1,11,11,0,8,8,3,3,4,0,0,11,5,6,6,0,6,6,0,0,9,0,0,13,0,0,16,1,4,15,8,8,8,0,1,17,1,2,12,0,10,12,7,8,10,6,7,7,5,5,6,4,4,5,2,2,3,0,5,5,0,0,4,0,0,7,2,3,15,0,9,10,2,3,4,0,7,8,3,4,5,5,6,7,0,0,15,7,8,9,0,6,14,4,5,6,0,11,11,6,9,10,5,7,7,0,4,12,1,2,16,0,7,7,0,5,6,0,9,9,4,6,6,0,2,11,3,5,5,1,4,4,2,2,2,0,7,14,5,5,7,7,7,10,2,5,5,6,9,9,0,2,16,2,5,14,4,7,7,2,11,11,7,8,8,0,10,11,4,6,7,6,8,10,2,4,14,4,4,6,0,8,10,7,7,7,1,10,12,5,10,10,0,4,11,5,6,8,3,6,6,7,7,9,3,3,14,2,4,4,3,5,6,});
sparse_graph4_ints[36][40] = vector<int>({0,0,0,10,10,10,0,0,12,0,0,20,0,0,4,7,7,7,4,4,4,0,8,16,0,0,8,0,0,16,0,8,8,1,13,13,2,2,2,0,4,14,0,4,18,0,0,2,3,3,3,0,10,10,0,0,10,0,0,14,5,5,5,0,0,6,6,6,6,8,8,8,0,6,6,0,2,12,9,10,10,5,5,15,0,2,2,0,4,4,0,12,14,9,9,11,7,7,8,3,3,17,2,2,14,2,4,4,3,5,5,4,6,6,6,6,7,0,0,18,0,4,12,1,1,19,0,10,12,4,4,16,0,13,13,8,10,11,0,8,10,7,8,8,9,9,10,6,7,7,0,6,8,5,6,6,1,3,3,1,5,5,4,5,5,3,3,13,6,8,8,8,10,10,2,2,18,5,7,7,6,6,14,0,10,11,5,5,6,1,5,17,6,6,8,7,7,9,0,6,14,0,9,9,4,4,6,0,2,10,2,2,12,5,5,7,0,12,12,8,9,11,6,7,8,7,11,11,2,6,16,9,9,9,1,12,13,3,3,4,4,4,5,5,6,7,0,11,11,4,5,15,0,12,13,0,9,10,7,8,9,7,10,11,8,9,10,2,12,13,0,6,17,6,7,9,8,8,12,1,3,15,4,6,15,0,8,9,5,7,8,4,5,6,1,9,9,6,8,9,});
sparse_graph4_ints[36][45] = vector<int>({11,11,11,0,0,12,0,0,0,0,5,20,7,7,7,0,0,18,0,0,4,4,5,5,0,0,8,0,15,15,0,0,22,0,4,14,0,12,12,3,3,4,5,6,17,0,0,14,0,3,3,0,8,8,9,9,9,0,1,1,0,4,17,0,0,10,0,10,10,6,6,6,0,0,16,8,8,8,0,0,6,0,6,6,2,3,3,0,3,12,0,9,18,4,4,15,10,11,12,0,0,20,0,3,21,1,1,2,4,4,4,5,5,6,0,14,15,0,5,14,7,7,8,8,9,9,10,11,11,6,6,16,6,6,7,3,3,3,0,12,13,0,10,11,3,4,19,0,5,16,8,8,9,5,5,5,6,7,7,0,8,9,0,6,7,3,4,4,2,2,3,0,4,13,0,0,11,0,0,13,0,0,15,0,0,17,5,6,6,0,3,11,9,12,12,1,14,15,7,8,8,0,0,19,0,1,22,0,6,16,6,7,16,1,4,20,0,11,12,0,10,17,10,10,12,0,0,9,4,4,5,0,4,15,0,9,10,8,9,10,0,5,5,6,7,8,0,7,8,5,6,7,4,5,6,0,4,12,7,8,9,5,5,17,4,4,14,9,11,12,2,14,14,0,8,18,8,8,10,5,7,7,0,4,16,2,3,20,0,10,12,6,8,8,3,4,5,0,9,9,});
sparse_graph4_ints[36][50] = vector<int>({12,12,13,0,0,12,0,0,0,0,0,25,8,8,8,2,2,2,0,0,18,6,6,6,0,16,17,4,4,4,0,6,22,0,4,18,0,0,8,0,14,14,0,0,2,0,4,12,7,7,18,0,0,22,10,10,10,0,10,10,0,0,14,0,0,4,3,3,3,5,5,15,5,5,5,0,12,12,0,0,10,0,10,20,0,4,16,0,0,16,0,0,20,0,0,6,7,7,7,9,9,9,0,8,8,0,4,14,0,8,18,1,3,23,0,6,6,0,4,20,0,2,2,4,4,14,11,13,13,4,6,20,0,0,24,1,3,3,2,4,4,0,4,10,0,16,16,8,8,17,12,12,12,5,15,15,10,10,11,9,9,10,6,6,7,8,8,9,7,7,8,0,12,14,0,10,12,0,8,16,6,6,16,0,12,19,3,5,21,4,5,5,0,2,24,5,6,6,1,3,19,0,6,14,0,8,10,7,8,8,9,10,10,11,12,13,6,8,18,2,16,16,2,4,22,8,9,9,6,7,7,1,13,13,3,5,17,5,7,19,9,11,11,8,8,10,0,14,15,6,8,8,1,3,13,0,6,16,9,9,11,1,11,11,7,9,9,10,13,13,7,9,17,5,5,17,1,16,16,0,13,13,11,11,14,9,10,11,0,12,18,6,6,19,8,10,10,11,12,12,});
sparse_graph4_ints[36][55] = vector<int>({4,5,5,13,14,14,0,0,0,0,3,26,8,8,8,0,0,18,1,1,2,0,0,10,0,18,18,0,4,20,0,14,14,0,10,22,0,0,14,6,6,7,2,2,3,0,0,24,10,10,10,3,4,14,8,9,19,3,4,4,0,0,27,0,8,20,0,0,20,0,0,6,0,12,12,0,3,14,0,16,16,0,1,1,11,11,12,5,5,16,0,8,8,0,0,12,0,0,8,0,10,10,4,5,23,0,0,16,0,0,22,0,3,18,9,9,9,3,3,3,7,7,16,0,4,22,0,14,20,6,16,16,7,7,8,0,0,4,0,3,11,5,6,6,0,8,16,0,6,6,6,7,21,0,5,25,0,5,14,13,13,14,11,11,11,0,11,18,0,3,3,8,9,9,7,7,7,9,10,10,0,13,14,0,15,16,9,9,18,5,5,5,0,4,5,4,4,4,5,5,19,10,10,11,12,14,14,11,12,12,1,18,18,7,8,20,0,12,21,0,7,24,6,6,6,0,4,12,4,4,15,0,8,18,8,8,9,0,10,11,0,4,19,0,3,16,0,12,13,7,8,8,9,9,10,0,14,15,7,7,17,0,8,10,6,7,7,0,11,12,10,11,11,5,5,17,0,6,16,0,5,22,5,6,22,5,5,6,7,16,16,13,13,13,9,10,18,8,9,10,});
sparse_graph4_ints[36][60] = vector<int>({0,20,20,0,0,14,0,0,0,0,0,30,10,10,10,0,0,22,2,2,2,0,10,10,15,15,15,0,8,22,2,4,4,0,12,24,5,5,5,0,0,18,0,16,16,0,0,26,8,8,8,12,12,12,0,0,2,5,5,17,0,0,8,0,4,28,0,4,18,0,0,12,10,10,20,0,14,14,0,0,4,0,4,22,1,3,15,6,6,6,6,6,24,0,12,12,8,8,18,0,6,6,0,0,10,0,4,12,0,18,18,0,0,16,6,18,18,0,0,20,0,0,6,0,0,24,7,7,7,9,9,9,1,3,25,0,8,26,0,12,20,0,4,4,4,4,14,0,16,22,11,11,11,7,7,17,13,13,13,0,4,20,0,8,8,8,8,22,0,10,18,0,8,16,0,2,2,0,0,28,4,4,4,0,4,10,5,15,15,6,6,20,0,4,14,4,4,26,5,5,19,0,15,15,0,12,14,9,9,10,5,5,15,8,8,9,0,10,12,6,6,7,7,7,8,0,4,24,0,6,20,1,3,17,3,3,3,2,2,28,10,10,11,0,8,10,4,4,6,14,15,15,9,9,21,9,17,17,4,4,22,5,6,6,11,11,12,12,13,13,11,11,19,14,14,16,0,17,17,7,7,23,0,14,23,9,9,17,0,10,22,0,6,14,5,5,25,7,7,15,});
sparse_graph4_ints[34][6] = vector<int>({0,2,2,0,0,0,1,1,2,0,0,2,0,1,2,1,1,1,0,0,3,0,1,1,0,0,1,});
sparse_graph4_ints[34][8] = vector<int>({2,2,2,0,0,0,0,0,4,1,2,2,0,2,3,0,0,2,1,1,3,0,2,2,1,1,2,0,1,3,0,0,3,0,1,2,1,1,1,0,0,1,0,1,1,});
sparse_graph4_ints[34][10] = vector<int>({2,2,3,0,0,0,0,0,4,0,0,2,1,3,3,0,3,3,2,2,2,1,2,3,0,0,5,0,1,1,0,2,4,1,1,4,0,2,3,1,2,2,0,1,4,1,1,1,1,1,3,0,2,2,0,1,3,0,0,3,0,0,1,1,1,2,0,1,2,});
sparse_graph4_ints[34][12] = vector<int>({3,3,3,0,0,0,0,0,6,1,1,1,0,0,2,0,4,4,2,3,3,1,3,4,0,0,4,2,2,4,0,2,2,0,3,4,2,2,3,1,3,3,1,2,4,0,2,5,0,1,1,1,1,5,0,0,5,0,3,3,2,2,2,0,1,2,0,1,5,1,2,3,0,2,4,1,1,4,0,0,3,1,1,2,0,2,3,1,2,2,1,1,3,0,1,3,0,1,4,0,0,1,});
sparse_graph4_ints[34][14] = vector<int>({3,3,4,0,0,0,0,0,6,0,2,2,0,0,2,2,4,4,0,0,4,3,3,3,0,0,7,0,4,5,2,3,4,1,4,4,1,1,1,2,2,2,0,4,4,1,3,5,2,2,5,2,3,3,1,3,4,1,1,2,0,3,5,0,2,6,2,2,4,1,1,3,0,1,6,1,2,5,0,2,3,0,1,1,1,2,2,1,1,6,1,3,3,0,3,4,0,1,2,0,0,5,2,2,3,0,3,3,0,1,3,1,1,5,0,2,5,1,2,4,0,0,3,0,2,4,1,2,3,0,1,5,1,1,4,0,1,4,0,0,1,});
sparse_graph4_ints[34][16] = vector<int>({4,4,4,0,0,0,0,0,4,0,0,8,2,2,2,0,0,2,3,4,4,0,4,6,0,2,2,0,0,6,3,3,5,1,5,5,0,3,3,2,4,5,0,5,5,1,1,1,3,3,4,2,4,4,1,3,3,1,4,5,2,3,5,0,2,3,0,4,5,1,3,6,3,3,3,1,1,7,1,2,2,0,0,7,2,2,6,1,4,4,2,3,4,1,1,3,0,1,7,0,4,4,1,3,5,0,3,6,2,2,5,1,2,3,0,1,1,0,2,4,0,2,7,2,2,3,1,1,2,2,3,3,1,2,6,0,0,5,2,2,4,1,3,4,0,2,6,0,3,4,0,1,3,0,3,5,0,1,2,1,2,4,1,1,4,0,0,3,0,1,4,1,1,6,1,2,5,0,1,6,0,2,5,1,1,5,0,1,5,0,0,1,});
sparse_graph4_ints[34][18] = vector<int>({4,4,5,0,0,0,0,2,2,0,0,8,1,3,3,0,0,2,0,0,6,0,6,6,0,0,4,0,0,9,3,5,5,2,2,2,4,4,4,0,4,4,3,4,5,0,2,8,0,3,3,1,1,1,2,5,5,1,5,6,0,5,6,2,2,3,3,3,6,2,4,6,3,4,4,1,5,5,3,3,3,3,3,5,2,4,5,0,1,8,0,3,4,0,4,7,1,4,6,0,2,3,2,3,3,2,3,6,0,5,5,0,4,6,3,3,4,1,2,3,1,3,7,2,4,4,2,2,4,2,2,7,1,4,5,1,1,8,1,1,3,0,0,7,1,3,4,2,3,5,0,2,4,0,3,7,1,2,2,0,1,1,1,2,4,0,0,5,1,1,7,2,3,4,1,4,4,1,3,6,0,4,5,1,2,7,2,2,6,0,3,5,1,1,2,1,3,5,1,1,4,2,2,5,0,2,7,0,1,7,0,2,5,0,3,6,1,1,5,0,1,3,0,1,5,0,1,2,1,2,5,0,1,4,0,2,6,1,2,6,0,1,6,0,0,3,1,1,6,0,0,1,});
sparse_graph4_ints[34][20] = vector<int>({5,5,5,0,0,0,2,2,2,0,0,10,0,0,2,0,0,6,3,3,3,0,6,6,0,0,4,0,2,2,0,4,4,4,5,5,0,0,8,1,1,9,4,4,6,1,3,3,0,6,7,3,5,6,4,4,5,2,3,3,2,6,6,3,5,5,0,4,8,1,1,1,0,3,3,2,4,4,1,6,6,1,5,7,3,4,6,2,2,4,2,5,6,4,4,4,0,2,9,1,4,4,0,5,7,2,4,7,3,4,5,2,2,3,3,3,7,0,4,5,1,5,6,2,5,5,2,3,4,0,1,9,1,3,4,0,2,3,3,3,6,2,4,6,3,3,4,2,2,8,0,0,9,0,3,4,0,0,7,3,4,4,1,4,7,0,5,5,0,5,6,0,3,8,1,5,5,3,3,5,2,3,7,1,3,8,1,3,5,1,2,3,2,4,5,0,2,8,0,4,6,0,3,5,1,2,8,0,4,7,1,4,5,1,4,6,1,1,3,2,3,5,1,2,4,1,3,7,2,2,5,2,3,6,0,2,4,1,2,2,0,1,1,0,0,5,1,2,5,2,2,7,0,2,6,0,2,5,1,1,2,1,3,6,1,1,8,1,1,5,1,1,4,0,3,6,2,2,6,0,1,8,0,3,7,0,1,6,1,2,6,1,2,7,0,1,7,1,1,7,});
sparse_graph4_ints[34][23] = vector<int>({5,6,6,0,0,0,0,3,3,0,0,11,0,0,3,0,0,8,3,4,4,0,7,8,0,0,6,2,3,3,0,3,10,5,5,6,0,5,5,0,1,2,0,0,4,2,2,3,0,0,9,0,1,11,3,3,4,4,6,6,0,2,3,0,0,7,3,3,3,1,7,7,0,0,10,0,2,10,5,5,5,0,4,5,4,5,7,4,4,4,0,6,8,2,2,2,0,0,1,0,0,5,1,4,4,0,5,6,3,6,7,1,2,10,1,1,2,0,3,4,4,5,6,2,4,4,2,7,7,0,7,7,0,4,4,2,4,5,3,6,6,2,3,4,1,2,2,4,4,7,1,1,10,1,6,8,3,3,5,1,3,4,1,4,5,2,6,7,3,5,7,0,1,1,1,2,3,0,6,6,4,5,5,2,3,5,1,3,3,3,4,5,1,3,9,1,6,7,4,4,6,0,6,7,1,5,5,2,5,8,4,4,5,3,4,8,0,5,9,0,1,10,0,4,6,2,2,4,3,5,6,0,4,9,2,6,6,2,5,5,1,4,9,0,1,8,0,2,2,0,3,5,3,5,5,1,5,8,2,5,7,2,2,9,2,2,5,3,4,7,2,3,9,1,4,6,1,3,5,1,2,4,1,5,6,2,4,8,1,6,6,0,5,8,3,4,6,3,3,8,});
sparse_graph4_ints[34][26] = vector<int>({0,0,0,6,6,7,2,2,2,0,0,12,4,4,4,0,0,8,0,0,2,1,3,11,0,0,4,3,3,3,0,8,8,0,0,10,0,0,6,0,6,6,5,7,7,0,2,2,0,4,4,0,2,12,6,6,6,3,5,5,0,8,9,2,4,4,0,0,13,5,6,7,0,4,11,4,4,5,1,3,3,1,1,9,0,5,5,5,5,5,3,4,4,4,7,7,1,7,9,5,5,8,2,2,11,5,6,6,2,8,8,4,5,5,0,6,10,4,6,8,0,6,7,1,8,8,5,5,7,1,5,5,0,3,11,4,4,6,3,7,8,3,3,5,2,3,3,3,4,5,4,6,7,0,7,9,0,6,8,5,5,6,3,3,4,1,1,12,3,7,7,2,4,10,2,5,5,2,7,8,4,5,8,0,7,7,4,6,6,0,5,6,3,6,8,1,6,6,1,7,8,0,5,10,4,5,6,0,4,6,3,4,6,0,7,8,4,4,9,1,2,11,3,5,6,4,5,7,2,6,9,3,5,9,1,5,6,2,4,5,0,3,3,1,4,4,1,7,7,0,4,8,0,1,12,0,4,10,2,4,6,2,5,6,0,0,9,0,4,5,2,3,10,2,6,6,0,5,7,3,3,10,3,6,7,2,7,7,1,6,9,4,4,8,1,5,7,1,4,10,});
sparse_graph4_ints[34][30] = vector<int>({7,7,8,0,0,0,3,3,3,0,0,14,0,0,4,0,0,10,5,5,5,1,3,13,0,10,10,0,0,8,0,0,2,2,2,2,0,6,6,0,0,12,0,0,6,0,8,8,4,4,4,0,2,10,0,2,2,7,7,7,0,0,15,2,4,12,0,4,4,6,8,8,5,5,6,0,4,10,2,4,4,0,2,14,3,5,5,1,3,3,0,6,12,0,5,5,4,5,5,6,6,6,0,6,8,6,7,8,4,6,6,0,9,10,0,4,13,3,3,5,4,4,5,6,6,9,5,6,6,5,8,8,1,9,10,2,2,10,3,3,12,0,7,7,6,7,7,1,9,9,4,5,6,5,7,9,6,6,8,5,5,7,2,2,13,0,8,10,4,4,6,3,4,4,1,7,7,0,5,12,5,7,8,3,9,9,0,7,8,6,6,7,4,8,9,0,8,11,0,2,8,0,6,7,1,1,11,1,5,5,4,5,7,2,3,12,2,9,9,3,6,6,3,5,11,5,7,7,0,9,9,5,6,9,3,5,6,4,8,8,5,6,7,2,8,10,1,1,14,1,5,12,4,6,7,3,4,5,0,8,9,1,4,12,2,6,6,4,7,9,3,5,7,5,5,10,3,8,9,5,6,8,4,4,11,1,8,10,1,7,11,0,3,13,4,4,7,0,7,9,});
sparse_graph4_ints[34][35] = vector<int>({8,9,9,0,0,0,0,0,17,0,3,3,5,5,5,0,0,13,0,0,8,0,11,12,3,3,4,0,1,1,0,3,16,6,7,7,0,0,10,0,5,12,0,0,4,0,6,6,4,5,13,0,8,8,0,0,15,0,0,6,0,3,10,2,3,3,0,9,10,1,1,2,6,6,6,0,5,15,4,4,4,8,8,9,0,3,12,0,0,11,4,5,5,0,5,5,5,6,6,0,7,14,1,11,11,7,9,9,7,7,7,2,2,3,0,0,9,0,0,12,3,3,3,0,6,7,0,0,2,1,2,2,0,0,7,3,4,4,0,0,14,0,4,12,5,5,6,4,4,5,0,8,9,6,6,7,0,1,17,3,4,14,0,4,4,0,7,8,0,2,10,0,5,6,8,8,8,0,10,10,4,5,6,0,4,11,5,6,7,7,8,10,1,2,16,0,4,13,4,4,13,0,10,12,0,0,16,2,3,15,3,4,5,0,4,5,2,2,2,0,0,5,0,1,2,0,9,9,0,7,7,5,7,7,0,3,11,4,6,6,2,3,4,3,5,5,0,6,14,7,8,9,6,6,8,0,11,11,0,3,13,5,5,7,5,5,12,6,9,10,0,8,10,1,4,15,2,11,11,6,7,8,0,2,9,4,4,6,2,4,4,0,3,4,2,5,5,0,6,8,});
sparse_graph4_ints[34][40] = vector<int>({5,5,5,0,0,0,10,10,10,2,2,2,0,4,18,0,0,12,0,0,16,0,10,10,2,4,4,1,13,13,0,0,2,0,0,20,0,0,8,5,5,15,0,4,12,0,0,4,8,8,8,6,6,6,0,0,10,0,6,14,0,6,6,0,0,14,0,0,6,0,0,18,0,4,4,0,8,8,7,7,7,0,8,16,0,4,14,0,4,10,0,2,2,0,10,12,4,4,4,3,3,3,0,2,10,6,6,14,9,10,10,0,12,14,3,3,17,1,1,19,4,4,12,0,2,16,0,8,10,7,8,8,6,6,7,4,5,5,2,2,12,5,6,6,9,9,11,7,7,8,0,11,11,0,13,13,4,4,16,6,7,7,0,6,8,5,5,6,1,3,3,3,4,4,0,2,8,3,5,5,0,2,12,8,8,9,0,6,12,1,3,15,4,12,12,2,2,18,3,5,13,0,12,12,8,10,11,5,7,7,7,7,9,7,7,13,4,6,6,0,9,9,6,8,8,9,9,10,0,2,14,1,9,15,6,6,8,0,10,11,5,5,7,3,3,13,5,6,14,1,5,17,0,8,14,7,8,9,4,4,6,8,10,10,6,7,8,3,3,11,1,9,9,5,6,7,1,11,11,4,6,15,7,9,9,7,11,11,2,12,13,0,8,9,0,8,12,9,9,9,});
sparse_graph4_ints[34][45] = vector<int>({11,11,11,1,2,2,0,0,20,6,6,7,0,0,0,0,6,6,0,9,18,0,0,16,0,0,8,0,0,12,8,8,8,0,15,15,0,6,16,0,1,1,1,2,21,0,12,12,2,3,3,5,5,5,0,3,12,0,3,16,0,0,22,0,10,11,7,7,15,0,0,10,0,0,6,9,9,9,0,8,9,3,3,4,0,0,14,0,0,18,0,0,4,3,4,19,3,3,12,5,5,14,7,7,7,0,3,10,0,3,18,4,4,13,0,13,13,4,4,4,0,3,14,0,7,8,5,6,6,2,2,3,0,0,3,0,9,10,5,6,17,5,13,13,10,11,12,0,11,17,0,5,20,0,8,16,9,9,10,4,5,5,0,5,5,7,7,8,8,8,9,0,0,1,6,6,6,0,5,14,0,11,12,1,1,2,0,3,3,0,0,9,0,0,11,3,3,3,0,0,7,0,4,12,4,4,5,5,5,6,6,7,7,0,0,13,0,2,9,0,0,15,0,3,11,0,10,10,0,8,8,3,4,4,0,6,7,7,8,8,0,4,4,0,0,17,0,8,14,0,4,16,0,0,19,0,4,18,7,8,15,4,5,18,5,6,14,0,14,15,8,9,9,0,6,13,0,12,13,4,4,14,10,11,11,6,7,16,0,4,13,5,6,7,6,7,8,0,3,21,});
sparse_graph4_ints[34][50] = vector<int>({12,12,13,0,0,20,0,0,0,0,4,4,8,8,8,0,6,22,0,0,12,6,6,6,0,0,4,0,16,17,0,0,16,0,0,25,4,4,4,4,4,16,0,14,14,10,10,10,0,0,2,0,0,10,0,10,10,0,4,12,8,8,17,0,0,6,0,4,16,0,0,22,0,12,12,5,5,5,0,0,8,0,12,19,0,0,14,0,4,20,0,0,18,5,5,20,2,2,2,0,10,16,2,4,4,7,7,7,9,9,9,0,4,14,0,8,8,0,8,14,5,15,15,0,2,10,7,7,15,0,2,2,0,4,18,1,3,23,4,4,18,0,6,8,0,10,20,4,4,12,11,11,11,6,6,14,0,2,12,1,1,1,0,0,1,0,6,6,0,8,10,0,0,24,0,15,15,0,8,18,0,13,13,5,5,13,0,11,11,7,7,18,3,3,3,4,5,5,5,6,6,6,6,7,7,7,8,11,13,13,8,8,9,0,8,16,0,2,24,3,5,21,0,2,8,1,3,3,6,6,16,9,9,10,5,13,13,9,9,16,2,2,20,0,6,14,0,2,18,0,2,16,3,3,11,0,2,14,2,2,10,10,10,11,0,10,18,6,6,19,12,12,12,8,14,14,0,14,18,2,16,16,2,4,16,0,2,20,4,12,12,6,7,7,7,8,8,8,9,9,});
sparse_graph4_ints[34][55] = vector<int>({0,0,8,0,0,23,13,14,14,0,0,0,0,0,16,0,14,14,0,7,24,0,10,10,5,5,5,0,18,18,0,0,3,0,0,27,0,0,12,11,11,11,8,8,8,0,6,20,0,0,19,8,9,19,1,2,2,4,4,4,4,4,15,3,3,3,6,6,18,9,9,10,0,4,12,0,16,16,0,11,22,6,6,6,0,12,12,0,0,10,0,0,6,0,6,14,0,0,21,0,0,14,0,0,25,0,3,26,7,16,16,0,0,1,0,10,17,2,2,3,7,7,7,0,3,20,0,14,20,0,4,23,0,8,8,0,3,11,0,12,18,0,3,16,6,6,14,6,7,21,12,12,12,8,8,16,0,0,4,10,10,10,0,3,18,3,3,12,0,5,5,0,9,20,0,1,1,0,0,17,0,7,7,6,14,14,0,2,10,0,8,15,5,5,17,8,9,9,0,13,13,0,11,11,3,3,18,4,5,23,10,10,17,0,15,15,0,3,22,7,7,15,4,4,21,0,9,9,0,6,6,6,7,7,7,8,8,0,8,17,4,4,13,5,6,6,0,3,14,0,0,9,2,3,3,4,5,5,3,4,4,0,0,11,0,0,15,0,0,13,0,3,3,0,0,7,1,1,2,0,0,18,0,0,20,0,4,17,0,0,22,0,0,5,2,3,25,0,0,24,});
sparse_graph4_ints[34][60] = vector<int>({9,9,9,0,0,30,4,4,5,0,0,0,15,15,15,0,0,22,0,0,6,0,10,10,0,20,20,4,4,22,0,16,16,0,0,18,0,12,24,0,0,10,0,0,14,0,0,26,0,0,2,11,11,11,4,4,18,3,3,3,3,3,27,0,8,8,0,12,12,11,11,19,0,0,4,2,2,18,0,14,14,0,4,14,9,9,17,0,0,12,0,18,18,0,0,8,13,13,13,7,7,23,8,8,8,0,4,22,0,0,16,6,6,7,7,7,15,0,10,22,0,0,20,1,3,3,0,0,24,3,4,4,9,17,17,0,16,22,0,0,28,5,5,13,0,12,18,0,4,12,0,4,16,2,2,26,10,10,10,0,14,20,5,5,5,0,10,16,1,1,3,7,7,7,0,8,26,0,8,20,0,4,20,6,16,16,12,12,12,9,9,21,1,1,11,8,8,16,1,1,1,2,2,24,0,0,1,5,6,6,7,7,20,0,8,16,5,15,15,1,1,29,0,10,12,0,8,10,5,5,25,4,4,12,6,6,18,6,6,14,5,13,13,0,6,6,6,18,18,0,6,8,0,13,13,1,1,9,5,13,21,0,6,16,0,6,24,2,2,2,0,4,4,2,4,4,6,6,20,1,3,11,0,0,9,4,4,20,0,10,18,0,12,20,0,15,15,5,5,23,});
sparse_graph4_ints[32][6] = vector<int>({0,2,2,0,0,0,1,1,2,0,0,2,0,1,2,0,0,3,1,1,1,0,1,1,0,0,1,});
sparse_graph4_ints[32][8] = vector<int>({2,2,2,0,0,0,0,0,4,0,0,2,0,2,3,1,2,2,1,1,3,0,2,2,1,1,2,0,1,3,0,0,1,0,0,3,0,1,1,1,1,1,0,1,2,});
sparse_graph4_ints[32][10] = vector<int>({2,2,3,0,0,0,0,0,4,0,0,2,1,3,3,0,3,3,2,2,2,0,1,1,0,0,5,1,2,3,0,2,4,1,1,1,1,1,4,0,2,3,0,1,4,1,2,2,0,2,2,1,1,3,0,1,2,0,0,3,0,0,1,0,1,3,1,1,2,});
sparse_graph4_ints[32][12] = vector<int>({3,3,3,0,0,0,0,0,6,0,0,2,0,2,2,0,4,4,0,0,4,2,3,3,1,3,4,1,1,1,2,2,4,0,3,4,1,3,3,2,2,3,1,2,4,0,1,1,2,2,2,0,2,5,0,3,3,0,1,2,1,1,5,1,1,2,0,0,5,0,1,5,1,2,3,0,2,4,0,0,3,0,2,3,1,1,4,1,2,2,1,1,3,0,1,3,0,1,4,0,0,1,});
sparse_graph4_ints[32][14] = vector<int>({3,3,4,0,0,0,0,2,2,0,0,6,0,0,2,0,0,4,2,4,4,0,0,7,2,2,2,0,4,5,3,3,3,1,4,4,1,1,1,2,3,4,0,4,4,0,2,3,1,3,5,2,2,5,0,2,6,1,2,2,0,1,1,1,1,3,2,3,3,1,3,4,0,3,5,2,2,4,0,1,6,0,3,3,1,1,2,1,2,5,1,3,3,1,1,6,0,0,5,2,2,3,0,3,4,1,2,3,0,1,2,0,2,4,0,1,3,1,1,5,0,0,3,1,2,4,0,2,5,0,1,4,1,1,4,0,1,5,0,0,1,});
sparse_graph4_ints[32][16] = vector<int>({4,4,4,0,0,0,0,0,8,2,2,2,0,0,2,0,0,4,0,4,6,0,0,6,0,2,2,3,4,4,1,3,3,1,5,5,3,3,5,0,5,5,2,4,5,0,3,3,1,1,1,1,1,7,3,3,4,2,4,4,0,4,4,0,2,3,1,4,5,2,3,5,3,3,3,2,2,3,0,4,5,1,3,6,2,2,6,1,2,3,1,2,2,0,1,1,1,1,3,1,4,4,2,3,4,2,3,3,0,2,7,0,2,4,0,1,7,0,0,7,0,3,6,0,3,4,0,0,5,1,3,5,2,2,4,2,2,5,1,1,2,1,2,6,1,3,4,0,2,6,1,2,4,0,3,5,1,1,4,0,1,3,0,1,2,0,1,4,1,2,5,0,0,3,0,2,5,1,1,6,0,1,5,1,1,5,0,1,6,0,0,1,});
sparse_graph4_ints[32][18] = vector<int>({0,0,0,4,4,5,0,0,8,2,2,2,0,0,2,0,0,4,0,6,6,0,0,6,0,4,4,0,2,8,0,2,2,3,5,5,1,3,3,0,0,9,4,4,4,3,3,3,0,3,3,1,1,1,3,4,5,2,5,5,2,3,3,1,5,6,0,5,6,3,3,6,2,4,6,2,2,4,1,5,5,3,4,4,0,1,8,2,2,3,3,3,5,0,4,7,2,4,5,0,2,3,0,3,4,0,4,6,1,3,7,1,4,6,3,3,4,0,5,5,1,3,4,1,1,8,2,4,4,2,3,6,0,0,7,0,2,4,2,3,4,1,4,4,2,2,7,0,4,5,1,2,3,1,1,3,0,1,1,1,2,2,0,3,7,1,2,4,0,0,5,0,3,5,1,4,5,1,2,7,2,3,5,1,1,7,1,3,5,2,2,6,1,3,6,2,2,5,0,2,6,0,2,5,1,1,2,1,1,5,0,2,7,1,2,5,1,1,4,0,3,6,0,1,7,1,2,6,0,1,6,0,1,5,0,1,3,0,1,4,0,1,2,1,1,6,0,0,3,0,0,1,});
sparse_graph4_ints[32][20] = vector<int>({0,0,0,5,5,5,0,0,6,0,0,10,0,0,2,3,3,3,2,2,2,0,0,4,0,6,6,0,0,8,1,1,9,0,2,2,0,4,4,4,5,5,4,4,6,0,6,7,2,4,4,1,3,3,0,4,8,3,5,6,0,4,5,4,4,5,2,6,6,0,2,9,3,5,5,3,3,4,0,3,3,2,3,3,1,6,6,4,4,4,1,5,7,0,5,5,2,2,8,3,4,6,2,5,6,2,2,4,1,1,1,1,4,4,0,5,7,2,3,4,3,3,7,3,4,5,0,4,6,2,4,7,3,4,4,2,2,3,0,2,3,0,3,8,1,5,6,1,3,4,3,3,5,2,5,5,1,5,5,1,3,5,3,3,6,2,4,6,0,5,6,1,4,7,1,3,8,0,3,4,1,4,5,0,0,7,1,2,8,2,4,5,2,3,7,2,3,5,0,1,9,0,3,5,0,2,8,1,2,3,2,2,5,0,4,7,0,0,9,0,2,6,1,3,7,1,4,6,1,2,2,1,2,4,1,1,3,0,1,1,0,2,4,0,0,5,0,3,6,1,2,5,2,3,6,2,2,6,2,2,7,1,3,6,1,1,7,0,2,5,0,3,7,1,1,5,1,1,2,1,2,6,1,1,8,0,1,7,0,1,6,1,1,4,1,2,7,1,1,6,});
sparse_graph4_ints[32][23] = vector<int>({0,0,11,0,0,0,5,6,6,3,3,3,0,1,2,0,0,8,0,3,3,0,3,10,0,0,6,0,7,8,0,5,6,0,0,4,0,0,1,4,4,4,0,4,4,5,5,6,0,0,9,0,2,3,0,0,3,2,2,3,3,3,4,0,0,7,1,2,2,0,0,5,0,1,1,2,3,3,1,2,10,3,4,4,4,6,6,0,1,11,0,0,10,0,4,5,0,6,6,1,7,7,0,3,4,0,5,5,5,5,5,1,1,2,2,2,2,2,3,4,4,5,7,3,4,5,0,2,10,0,6,8,0,1,8,2,4,4,1,3,3,0,0,2,0,2,2,1,3,9,1,4,4,4,4,5,3,6,7,4,5,6,2,4,5,2,7,7,0,7,7,1,2,3,1,3,4,3,3,5,2,3,9,1,4,5,1,5,5,0,4,9,3,6,6,4,5,5,0,4,6,0,6,7,4,4,7,1,6,8,1,1,10,2,5,5,2,6,7,2,3,5,3,5,7,2,2,9,2,2,4,0,2,8,0,5,9,3,5,5,1,4,9,4,4,6,1,5,6,1,6,7,3,4,8,1,4,6,0,3,5,2,5,8,3,5,6,0,4,7,3,3,6,2,2,5,1,2,4,1,3,5,2,3,6,2,6,6,3,4,6,3,3,8,1,5,8,0,5,8,});
sparse_graph4_ints[32][26] = vector<int>({0,0,12,0,0,0,6,6,7,0,0,4,0,0,8,0,6,6,3,3,3,1,3,11,0,0,2,2,2,2,0,0,10,0,8,8,0,0,6,4,4,4,0,2,2,5,5,5,0,4,4,0,2,8,0,2,12,5,7,7,0,0,13,0,2,10,2,4,10,0,8,9,6,6,6,4,4,5,2,4,4,1,3,3,0,6,8,0,4,11,0,5,5,4,5,5,5,6,7,0,4,8,3,5,5,0,6,10,3,4,4,2,8,8,4,4,6,2,2,11,5,5,8,4,7,7,0,7,7,1,8,8,5,6,6,5,5,7,1,7,9,4,6,8,0,6,7,0,4,6,3,4,5,3,3,10,3,3,5,2,3,3,1,1,9,0,3,11,5,5,6,4,5,6,3,3,4,3,7,8,3,5,6,1,5,5,0,5,10,4,6,7,0,7,9,1,6,6,1,1,12,4,6,6,3,4,6,2,3,10,2,2,8,0,3,3,0,4,5,2,5,5,2,6,6,0,4,10,1,7,7,3,7,7,4,5,8,2,7,8,0,5,6,4,4,9,0,7,8,2,5,6,3,5,9,1,4,10,1,5,7,1,7,8,3,6,8,3,6,6,2,4,5,2,2,4,4,5,7,1,5,6,1,2,11,3,4,9,1,3,9,2,4,6,3,3,9,0,5,7,});
sparse_graph4_ints[32][30] = vector<int>({0,0,0,0,10,10,3,3,3,0,0,15,0,0,4,5,5,5,0,0,12,0,0,8,7,7,8,0,0,2,1,3,13,2,2,2,0,6,6,0,0,10,0,0,6,0,8,8,2,2,10,0,6,12,0,2,2,4,4,4,6,6,6,0,4,4,0,2,8,0,0,14,0,2,10,6,8,8,0,2,14,4,4,11,2,4,4,0,6,8,5,5,6,7,7,7,0,4,10,4,5,5,1,3,3,3,4,4,2,4,12,0,2,12,4,4,5,5,6,6,0,4,6,0,4,8,3,5,5,4,6,6,0,7,7,6,7,8,0,4,13,5,5,7,3,5,11,0,9,10,0,6,10,4,4,6,3,9,9,3,3,12,6,6,9,0,8,10,6,7,7,1,9,10,1,7,7,5,8,8,5,6,7,4,5,6,2,2,13,0,8,9,1,3,11,3,3,5,1,3,9,6,6,8,5,7,9,1,9,9,3,4,11,0,7,8,6,6,7,4,6,7,3,3,9,0,6,7,0,5,12,4,5,7,0,8,11,5,7,7,4,8,9,4,4,10,2,9,9,0,9,9,5,7,8,5,5,10,1,1,14,1,5,12,3,7,7,3,6,6,5,6,9,1,7,11,2,3,12,2,8,10,0,7,9,3,4,5,4,4,8,4,5,10,1,1,11,0,4,12,});
sparse_graph4_ints[32][35] = vector<int>({0,0,0,8,9,9,0,0,17,0,3,3,5,5,5,0,0,12,3,3,4,0,11,12,0,0,8,0,1,1,0,4,13,4,5,13,0,8,9,0,0,4,0,0,14,0,0,10,7,7,7,0,6,6,0,3,16,0,0,6,0,3,10,2,3,3,6,6,6,0,10,10,0,7,14,1,1,2,0,4,11,4,4,4,0,0,15,0,7,8,0,0,1,0,0,3,0,2,8,0,6,12,0,0,13,0,0,11,0,4,5,0,0,9,0,0,7,2,2,3,1,2,2,4,5,5,5,6,6,0,3,12,0,5,15,6,7,7,8,8,9,5,6,12,3,4,4,0,3,9,1,11,11,0,9,10,3,3,11,0,0,5,0,1,17,0,0,16,3,3,3,0,6,7,5,5,6,0,8,8,6,6,7,4,4,5,3,4,14,0,2,14,1,2,16,0,4,10,0,5,6,0,4,4,2,2,2,4,4,11,7,7,8,7,9,9,0,8,13,0,0,2,0,1,2,2,11,11,2,3,15,5,5,12,0,9,9,4,5,6,0,5,12,5,6,7,0,5,5,0,7,7,0,2,10,0,2,12,3,4,5,0,2,3,0,3,4,3,3,10,2,3,4,0,3,8,6,7,8,3,5,5,4,6,6,2,2,13,2,2,10,8,8,8,5,7,7,4,4,6,});
sparse_graph4_ints[32][40] = vector<int>({10,10,10,0,0,0,0,0,20,0,4,4,6,6,6,0,0,12,0,8,16,0,0,4,0,0,16,4,4,4,1,13,13,0,10,10,0,0,2,0,4,14,0,0,10,8,8,8,0,0,6,0,4,10,0,4,18,0,0,8,0,0,14,0,8,8,0,0,18,2,2,2,6,6,14,5,5,5,0,4,6,4,4,12,7,7,7,0,11,11,3,3,11,1,1,1,0,4,16,4,4,16,0,2,10,0,0,1,0,6,6,0,2,14,0,2,2,3,3,3,0,12,12,0,6,14,0,9,9,4,12,12,5,5,13,0,2,12,0,2,16,1,1,19,0,6,8,0,10,15,0,8,14,7,7,13,0,4,12,0,6,12,1,3,3,2,4,4,4,4,5,5,5,6,1,1,9,3,3,17,6,6,7,8,8,9,7,7,8,1,2,2,0,0,9,0,0,7,5,5,15,5,5,11,9,10,10,0,0,5,3,4,4,4,5,5,5,6,6,6,7,7,0,12,14,1,11,11,0,8,12,4,4,14,7,8,8,3,3,13,2,3,3,0,2,8,1,3,9,0,6,10,4,4,10,0,0,11,2,2,10,0,0,13,2,2,12,0,8,10,4,6,6,5,7,7,0,7,7,3,5,5,3,3,5,2,2,16,2,2,18,0,6,17,2,2,14,5,5,7,});
sparse_graph4_ints[32][45] = vector<int>({0,0,18,11,11,11,0,0,0,4,4,5,0,0,5,0,10,10,3,4,19,0,0,14,0,0,10,0,0,22,0,15,15,8,8,8,3,3,3,0,0,3,0,8,8,0,12,12,0,3,14,6,7,16,0,3,18,0,0,8,0,0,12,2,2,3,6,6,6,0,8,14,9,9,9,0,0,1,3,4,4,0,0,16,0,9,18,0,0,20,4,4,12,0,3,12,1,2,21,7,7,7,0,0,6,5,5,5,0,13,14,0,3,10,6,6,14,0,3,16,6,13,13,1,1,2,0,5,18,0,1,1,3,3,11,5,5,13,0,3,3,0,6,6,8,8,14,0,11,17,0,10,15,0,9,9,0,11,11,3,3,15,0,0,9,0,0,11,0,0,7,4,4,16,9,10,10,0,7,7,0,0,4,0,2,8,0,4,4,0,6,12,5,11,11,0,0,13,5,5,6,5,5,17,0,0,15,0,5,5,1,2,2,0,0,17,6,6,7,0,6,14,0,0,19,0,8,16,0,5,20,0,5,10,1,2,9,7,7,8,8,8,9,0,0,21,4,4,4,4,5,5,5,6,6,6,6,13,0,3,11,0,6,16,1,2,15,1,2,13,5,5,12,6,7,7,1,2,19,1,2,17,3,3,13,2,3,3,3,3,4,0,7,19,0,1,22,7,8,8,});
sparse_graph4_ints[32][50] = vector<int>({0,0,25,12,12,13,5,5,5,0,0,0,0,0,6,0,0,16,9,9,9,5,5,20,3,3,17,0,0,12,0,16,17,1,1,1,3,4,4,7,7,8,0,0,22,0,14,14,0,0,8,0,3,16,6,6,6,0,12,12,0,0,20,0,0,10,9,9,16,1,3,23,0,0,14,0,10,10,3,3,3,6,6,16,0,12,19,0,0,1,1,3,11,10,10,10,0,0,4,1,3,3,0,4,20,0,0,18,0,8,14,0,10,16,1,3,9,3,4,12,0,8,8,5,15,15,5,5,15,8,8,8,0,0,2,5,13,13,4,4,5,3,3,11,0,6,22,7,7,18,0,10,18,2,2,2,4,4,18,11,11,11,3,3,15,1,1,15,6,7,7,0,0,5,1,1,9,0,2,20,0,8,10,0,11,11,0,13,13,0,2,22,5,5,13,0,10,20,8,8,14,0,6,6,0,2,18,0,15,15,0,8,16,0,0,9,0,4,4,0,0,24,7,7,13,0,2,2,0,4,6,0,6,8,0,6,20,0,0,7,0,6,14,4,12,12,8,14,14,0,0,13,5,5,6,0,0,15,0,4,14,0,14,18,0,4,12,0,4,18,5,5,11,1,2,2,0,2,24,0,0,11,0,12,16,0,2,12,5,11,17,0,8,18,3,5,21,3,11,11,});
sparse_graph4_ints[32][55] = vector<int>({13,14,14,0,3,26,0,12,12,2,2,2,0,0,16,0,0,0,4,4,5,0,0,23,7,8,20,6,6,7,0,18,18,11,11,11,0,0,12,0,6,19,0,1,1,3,3,3,0,0,19,0,15,16,9,9,9,0,8,8,0,9,23,0,2,2,0,0,8,7,7,16,7,8,8,0,0,25,0,3,18,0,0,10,0,0,14,0,6,22,0,14,14,0,8,14,1,2,15,7,16,16,3,4,4,0,0,27,0,10,10,0,10,16,0,15,20,10,10,17,0,0,21,2,2,12,0,12,19,0,0,6,5,5,5,6,14,14,10,10,10,5,6,14,0,3,22,0,3,11,0,0,1,0,6,13,3,4,24,0,9,20,12,12,12,0,0,3,3,3,17,0,0,5,5,5,17,2,3,3,5,6,6,7,7,7,1,2,9,0,12,21,4,4,13,0,0,18,0,3,20,5,5,20,9,9,15,0,9,9,0,16,17,0,3,24,8,8,9,0,11,11,0,13,13,0,0,7,0,0,9,5,13,13,5,6,22,0,12,17,0,4,16,7,7,14,0,0,11,0,7,7,7,7,18,3,4,20,0,5,5,0,8,16,5,12,19,0,0,13,0,0,15,0,5,12,0,6,6,2,2,3,5,5,13,0,0,17,0,0,4,0,0,20,0,9,18,4,12,12,});
sparse_graph4_ints[32][60] = vector<int>({0,0,8,15,15,15,0,0,24,0,0,16,9,9,9,0,0,0,6,6,24,0,0,30,0,16,16,6,6,6,0,20,20,0,0,12,1,1,1,0,0,20,5,5,19,4,4,14,4,5,5,11,11,11,10,10,20,1,1,3,2,4,4,3,3,27,3,3,19,0,12,12,0,14,14,0,3,24,0,10,22,13,13,13,0,4,14,0,10,10,0,0,14,0,16,22,0,0,10,9,9,17,9,17,17,0,0,28,0,0,1,0,0,22,0,18,18,4,4,4,0,8,8,0,0,18,0,0,6,8,8,8,0,12,24,0,4,4,10,10,10,0,4,26,0,0,26,0,4,16,0,14,20,0,10,18,0,8,16,1,3,11,8,8,16,0,4,18,6,16,16,6,7,7,0,0,4,0,0,2,12,12,12,1,5,23,8,8,22,5,5,17,0,8,26,0,10,12,6,8,20,5,15,15,1,1,11,2,2,2,0,13,13,5,13,13,7,7,8,0,3,22,0,10,20,3,3,11,4,4,24,12,12,18,1,1,29,1,1,9,6,8,14,6,6,13,6,14,20,0,8,10,0,6,21,4,4,12,0,0,7,0,0,9,5,5,21,1,1,21,5,5,6,0,14,16,0,10,16,3,3,3,0,13,19,0,6,7,6,18,18,1,3,3,10,11,17,6,8,16,});
sparse_graph4_ints[30][6] = vector<int>({0,2,2,0,0,0,0,0,2,1,1,2,0,1,2,0,0,3,1,1,1,0,1,1,0,0,1,});
sparse_graph4_ints[30][8] = vector<int>({2,2,2,0,0,0,0,0,4,0,0,2,0,2,3,1,2,2,1,1,3,0,1,1,0,2,2,1,1,2,0,1,3,0,0,1,0,0,3,1,1,1,0,1,2,});
sparse_graph4_ints[30][10] = vector<int>({1,3,3,0,0,0,0,0,4,0,0,2,2,2,3,1,1,1,0,3,3,0,0,5,2,2,2,1,2,3,0,1,1,0,2,4,0,2,2,1,1,4,0,2,3,0,1,4,1,2,2,0,1,2,1,1,3,0,0,3,0,0,1,1,1,2,0,1,3,});
sparse_graph4_ints[30][12] = vector<int>({3,3,3,0,0,0,0,0,6,0,0,2,0,2,2,0,4,4,0,0,4,2,3,3,1,1,1,2,2,4,1,3,4,2,2,2,0,3,4,1,3,3,2,2,3,0,1,1,1,1,2,1,2,4,0,3,3,0,2,5,1,1,5,0,2,3,1,2,2,0,1,2,0,0,5,1,1,3,0,1,5,0,0,3,0,2,4,1,2,3,0,1,3,0,0,1,1,1,4,0,1,4,});
sparse_graph4_ints[30][14] = vector<int>({3,3,4,0,0,0,0,0,6,0,0,2,0,0,4,2,2,2,0,2,2,0,0,7,2,4,4,0,4,5,3,3,3,0,4,4,0,3,3,1,1,1,0,2,6,2,3,4,1,4,4,0,2,3,1,3,5,2,2,5,1,2,2,0,1,1,1,1,3,2,3,3,1,3,3,0,1,6,1,3,4,0,3,5,0,0,5,2,2,4,1,1,2,0,2,4,1,1,6,2,2,3,1,2,5,1,2,3,0,3,4,1,1,5,0,1,3,1,2,4,0,0,3,0,1,2,0,1,4,0,0,1,0,2,5,1,1,4,0,1,5,});
sparse_graph4_ints[30][16] = vector<int>({4,4,4,0,0,0,0,0,4,0,0,8,0,0,2,1,3,3,0,0,6,0,2,2,0,4,6,2,2,2,3,4,4,1,1,7,1,5,5,0,4,4,3,3,5,0,3,3,1,1,1,0,5,5,2,4,5,3,3,3,3,3,4,0,2,3,2,2,3,2,4,4,1,4,5,0,2,7,0,3,4,2,3,5,2,2,6,2,3,3,0,4,5,1,2,2,2,2,4,1,1,3,0,1,1,1,2,3,0,0,5,1,3,6,0,2,4,0,3,6,2,3,4,1,4,4,1,3,4,1,3,5,0,1,7,0,0,7,0,2,6,1,2,4,1,2,6,1,1,2,2,2,5,0,3,5,1,1,4,0,1,2,0,0,1,0,0,3,0,1,3,1,1,5,0,1,4,0,2,5,0,1,5,1,2,5,1,1,6,0,1,6,});
sparse_graph4_ints[30][18] = vector<int>({0,0,0,4,4,5,0,0,4,0,0,8,0,4,4,0,0,2,0,0,6,2,2,2,0,6,6,0,2,8,0,2,2,3,3,3,3,5,5,0,0,9,1,3,3,4,4,4,1,1,1,0,3,3,2,3,3,1,3,7,3,4,5,2,2,4,0,5,6,2,5,5,1,1,8,1,5,6,0,4,5,0,2,3,0,0,7,3,4,4,3,3,6,2,2,3,2,4,6,2,3,4,0,4,6,1,5,5,3,3,5,0,1,8,2,4,4,0,3,4,0,4,7,3,3,4,1,3,4,2,4,5,0,5,5,0,2,6,0,3,7,1,4,4,1,2,2,0,0,5,1,4,6,0,2,4,0,3,5,2,2,7,2,3,6,1,2,7,1,3,5,1,2,3,1,2,4,1,4,5,1,1,7,2,3,5,2,2,6,2,2,5,1,1,5,1,1,3,0,0,1,0,0,3,1,1,2,1,2,5,0,2,5,1,3,6,0,3,6,0,2,7,0,1,6,1,1,4,0,1,1,0,1,2,1,2,6,0,1,3,0,1,5,0,1,4,0,1,7,1,1,6,});
sparse_graph4_ints[30][20] = vector<int>({0,0,0,5,5,5,0,0,6,0,0,10,0,0,2,3,3,3,0,0,4,2,2,2,0,6,6,0,0,8,1,1,9,0,2,2,0,4,4,2,4,4,2,2,8,4,5,5,1,3,3,0,6,7,0,5,5,4,4,6,1,1,1,0,4,8,1,1,7,0,3,3,3,3,4,3,5,6,4,4,4,2,3,3,2,6,6,0,0,1,4,4,5,0,4,6,2,2,4,1,2,2,0,2,9,0,0,7,0,4,5,1,6,6,3,4,4,3,5,5,1,5,7,2,3,4,2,2,3,1,4,4,0,2,6,0,0,5,0,2,3,0,3,4,0,3,8,3,3,5,3,4,6,3,3,7,1,5,5,2,5,6,1,3,4,1,3,5,0,5,7,3,4,5,1,4,5,1,3,8,0,2,8,0,5,6,2,4,7,2,3,5,1,2,8,2,4,5,1,5,6,2,5,5,0,3,5,0,2,4,3,3,6,2,4,6,2,3,7,2,2,5,1,4,7,2,2,6,1,3,7,0,3,6,1,2,3,1,2,4,1,1,3,0,1,2,0,4,7,1,4,6,1,2,5,0,1,9,0,0,9,1,1,5,1,3,6,2,3,6,0,3,7,1,2,6,2,2,7,0,1,7,0,2,7,0,2,5,1,2,7,0,1,6,1,1,8,1,1,6,});
sparse_graph4_ints[30][23] = vector<int>({0,0,11,0,0,0,5,6,6,0,0,4,2,3,3,0,0,8,4,4,4,0,0,2,0,3,10,0,0,6,0,7,8,0,3,3,3,3,4,1,1,2,0,0,9,0,6,6,0,0,1,0,4,5,0,0,3,0,2,8,0,0,7,2,2,3,0,0,5,0,2,3,5,5,6,2,3,9,0,1,11,0,0,10,3,4,4,0,4,4,0,1,2,2,2,2,3,3,3,0,1,1,0,5,6,4,4,5,1,2,10,1,2,2,0,2,7,0,3,4,4,6,6,1,7,7,0,5,5,0,3,8,0,2,2,2,3,4,3,4,5,5,5,5,1,3,3,0,6,8,4,5,7,1,4,9,2,4,4,0,2,10,0,6,7,3,3,5,4,5,5,2,7,7,0,4,9,1,3,4,1,2,3,1,4,4,0,4,6,2,5,5,1,1,8,3,6,7,1,3,9,2,4,5,4,5,6,0,7,7,3,5,5,0,5,9,1,5,5,0,2,6,2,2,9,1,4,5,0,4,7,2,2,4,4,4,7,1,5,6,2,3,5,3,6,6,3,3,6,3,3,8,1,6,8,4,4,6,1,1,10,3,4,8,2,6,7,3,4,6,0,3,7,3,5,7,0,5,7,0,3,6,1,4,6,0,3,5,1,2,8,3,5,6,0,5,8,1,6,7,});
sparse_graph4_ints[30][26] = vector<int>({0,0,0,6,6,7,0,0,8,0,0,13,0,0,4,0,6,6,3,3,3,0,0,2,0,0,10,2,2,2,1,3,11,0,8,9,0,0,6,5,5,5,0,2,10,0,2,2,4,4,4,0,2,8,0,4,4,0,0,12,0,0,1,1,1,1,2,4,10,0,7,7,0,2,12,2,4,4,0,4,8,5,7,7,0,6,10,1,3,3,4,5,5,0,4,6,6,6,6,0,8,8,1,3,9,4,4,5,3,4,4,2,8,8,3,3,10,1,2,2,0,0,7,1,1,7,2,3,3,0,0,5,0,6,8,1,1,9,0,0,9,0,0,11,0,4,11,3,3,4,3,5,5,0,3,3,2,2,8,0,5,5,4,4,6,5,6,7,2,2,4,5,5,6,3,3,5,0,2,6,1,1,12,0,6,7,1,5,5,2,2,3,0,4,5,3,4,5,4,4,9,2,2,11,4,5,6,0,4,10,1,8,8,5,6,6,5,5,8,1,7,7,0,3,9,0,5,6,2,3,4,4,7,7,3,3,9,1,7,9,4,6,6,3,3,7,3,5,6,5,5,7,0,5,10,4,6,8,2,6,6,0,3,8,2,5,5,3,5,9,3,4,6,2,4,5,1,6,6,2,3,10,3,7,8,0,7,8,3,4,9,0,3,11,0,4,9,1,3,7,});
sparse_graph4_ints[30][30] = vector<int>({7,7,8,2,2,2,0,0,0,0,0,15,5,5,5,0,0,8,0,0,2,0,0,12,2,4,4,0,6,12,0,0,4,0,10,10,0,0,10,0,4,10,0,0,6,0,6,6,0,8,8,0,4,4,6,6,6,1,3,13,1,1,1,0,2,10,4,4,4,0,0,1,0,0,14,0,2,2,4,4,11,0,2,12,1,1,7,0,7,7,1,1,9,0,2,14,3,3,9,3,3,3,1,3,3,0,0,7,0,0,5,0,4,6,4,4,5,1,2,2,0,0,9,5,6,6,7,7,7,3,9,9,0,6,10,1,3,9,3,4,4,0,2,8,0,0,11,0,0,13,0,4,8,4,5,5,1,3,11,2,4,12,5,5,6,0,8,9,6,8,8,5,5,10,0,6,8,1,1,11,2,3,3,3,3,4,3,5,5,2,2,8,1,5,5,0,2,6,0,3,3,5,5,7,0,4,13,3,5,11,2,2,10,4,4,6,4,6,6,1,7,7,2,4,10,0,6,7,3,3,5,0,4,5,2,2,3,6,6,7,0,5,5,3,4,5,1,7,11,1,1,14,3,3,12,4,5,6,0,9,9,6,7,8,5,6,7,4,4,8,0,4,12,2,2,4,0,3,8,2,3,4,0,5,6,3,3,7,4,4,10,0,7,8,2,2,13,0,5,10,});
sparse_graph4_ints[30][35] = vector<int>({0,0,0,8,9,9,0,0,17,0,0,6,5,5,5,0,0,10,0,7,14,0,0,3,0,0,14,3,4,4,0,11,12,0,4,12,0,4,4,7,7,7,0,0,12,0,0,8,0,8,8,0,3,16,0,0,1,0,3,3,0,10,10,0,4,10,0,2,2,6,6,6,0,6,6,5,6,12,4,4,11,0,0,5,0,1,1,0,0,15,3,3,3,0,2,10,0,2,8,4,4,5,0,3,14,0,9,9,3,4,14,0,0,7,0,0,4,0,0,13,0,7,7,0,0,9,2,2,3,0,0,11,3,3,10,0,7,12,5,10,10,0,2,12,0,5,5,0,0,16,1,2,2,5,5,6,2,2,9,0,6,11,6,6,7,0,9,13,7,7,8,0,3,8,0,5,15,0,1,17,4,4,4,3,3,12,4,5,13,0,2,14,4,5,5,5,6,6,2,3,3,3,3,4,0,2,7,2,11,11,8,8,9,6,7,7,0,1,2,0,0,2,3,9,9,6,6,11,0,7,8,2,3,15,4,4,10,3,3,9,0,2,13,0,6,7,0,4,14,0,8,9,0,9,10,5,5,10,0,4,11,0,2,11,0,5,6,0,3,9,2,2,2,0,6,12,0,4,5,1,2,16,2,2,12,0,3,4,0,8,13,5,5,12,0,10,11,7,8,8,});
sparse_graph4_ints[30][40] = vector<int>({0,0,20,0,0,8,10,10,10,0,0,0,4,4,5,0,6,14,1,1,1,0,0,16,0,0,12,6,6,6,1,13,13,2,2,2,8,8,8,0,8,16,3,3,3,0,10,10,0,0,10,3,3,11,0,0,14,0,0,1,0,0,18,3,3,17,0,8,8,0,0,4,0,4,10,7,7,13,0,0,6,3,4,4,7,7,7,0,12,12,5,5,5,5,5,13,1,1,15,1,1,19,0,0,2,0,4,4,1,1,9,1,1,13,0,10,12,1,1,17,3,3,13,0,0,5,0,9,9,0,6,6,5,5,15,5,5,11,4,12,12,0,10,15,0,0,9,0,0,7,3,3,15,0,4,6,0,2,2,1,3,3,0,6,12,1,3,9,0,7,7,4,10,10,0,8,14,0,4,18,9,9,9,0,4,12,0,0,13,5,5,6,0,0,11,1,2,2,3,3,9,6,6,7,0,4,14,0,0,15,0,4,16,0,0,17,0,8,12,4,4,10,0,2,10,0,2,12,4,4,4,1,1,7,6,6,12,2,10,10,4,4,16,7,7,8,4,4,12,6,6,14,0,6,10,3,3,4,0,2,8,0,2,14,7,11,11,0,2,16,0,6,17,4,4,14,4,5,5,5,6,6,1,9,9,1,1,11,2,2,8,8,8,9,0,12,14,6,7,7,});
sparse_graph4_ints[30][45] = vector<int>({2,2,2,0,8,8,11,11,11,0,0,22,0,0,0,0,0,10,4,4,14,0,0,14,0,0,18,0,15,15,0,1,1,0,12,12,4,4,5,0,5,20,6,7,16,3,3,3,0,10,10,9,9,9,0,2,2,0,3,14,0,0,8,7,8,8,0,0,12,0,0,20,3,4,4,0,4,16,0,8,18,4,4,11,1,2,11,0,0,16,0,0,6,5,6,6,5,5,15,0,12,16,6,13,13,0,0,1,3,3,17,7,7,7,5,5,5,0,0,3,0,0,5,0,13,13,0,4,4,0,4,12,0,8,13,0,9,9,1,1,20,0,2,10,0,10,15,8,9,14,0,11,11,0,0,7,0,0,9,6,6,7,7,7,13,0,7,7,0,5,14,0,6,6,10,10,10,8,8,9,1,2,21,0,0,11,4,12,12,0,9,14,0,2,18,5,5,17,0,5,5,0,2,16,0,0,15,0,0,13,2,2,3,5,6,12,0,6,12,0,6,16,0,0,4,0,5,10,0,3,8,0,10,17,5,5,11,0,0,17,0,0,19,3,4,19,0,8,16,3,3,11,4,10,10,1,2,2,0,0,21,0,4,9,0,4,18,6,6,13,0,2,13,3,3,14,0,2,8,4,11,11,4,4,4,0,6,18,4,11,15,0,14,14,4,5,5,5,5,6,});
sparse_graph4_ints[30][50] = vector<int>({12,12,13,2,2,2,0,0,22,8,8,8,0,0,0,4,4,5,0,10,20,0,4,14,0,0,10,0,0,14,0,14,14,1,1,1,1,3,23,0,10,10,10,10,10,0,8,8,3,3,3,0,3,18,5,15,15,6,6,16,0,0,18,0,12,12,0,0,8,0,0,12,8,8,17,0,0,25,0,0,16,2,2,20,6,6,6,0,0,1,5,5,13,0,14,18,3,3,5,0,0,4,0,10,16,0,0,20,2,2,10,9,9,9,4,4,21,0,6,14,4,4,18,6,7,7,5,13,13,5,5,5,0,0,2,0,0,7,3,3,11,11,11,11,0,8,16,0,0,5,0,11,11,0,4,4,6,6,19,0,4,8,1,11,17,3,3,17,7,7,13,0,9,9,7,7,8,0,16,17,0,2,14,0,0,6,0,13,13,0,2,22,8,8,14,0,3,16,0,0,9,0,6,20,0,8,14,0,0,11,0,4,6,10,10,15,5,5,15,0,2,20,1,3,11,0,15,15,3,5,11,0,0,13,0,7,7,1,3,3,4,12,12,0,0,24,3,3,15,5,5,11,0,9,18,5,11,17,0,6,22,8,14,14,5,6,6,0,0,17,0,0,15,5,9,11,0,2,24,0,0,19,0,6,18,4,4,4,1,3,9,0,12,19,0,2,2,1,2,2,});
sparse_graph4_ints[30][55] = vector<int>({0,0,16,0,0,0,0,0,7,9,9,9,13,14,14,0,0,24,7,7,20,0,10,10,0,7,7,0,0,20,0,0,3,0,8,19,2,3,25,0,0,12,0,5,5,6,14,14,0,14,14,0,0,5,5,6,6,0,12,12,0,15,20,0,4,17,7,8,8,0,0,14,0,1,27,0,0,10,0,10,20,5,16,17,0,3,21,7,7,15,0,0,18,0,6,6,12,12,12,0,5,12,2,3,3,0,0,1,0,8,15,10,10,10,0,0,22,10,11,17,0,6,22,0,11,22,6,6,7,6,6,14,0,16,17,0,4,11,0,6,14,2,2,2,0,0,9,0,0,26,0,1,1,5,5,22,1,1,2,0,5,18,11,11,11,6,7,18,0,3,15,5,13,13,7,7,7,0,18,18,0,8,8,0,10,16,0,3,10,0,3,24,0,12,17,0,11,11,3,3,20,0,13,19,8,8,9,0,9,9,5,12,19,8,9,15,0,0,8,5,5,5,0,13,13,4,5,13,0,0,11,9,15,15,0,15,16,3,4,4,0,7,18,4,5,20,0,7,24,9,9,17,3,3,3,4,4,5,0,2,20,0,0,6,3,4,12,1,1,17,0,0,15,5,11,12,5,5,17,1,1,23,6,6,12,0,0,13,0,0,17,1,2,13,4,4,16,2,2,3,});
sparse_graph4_ints[30][60] = vector<int>({15,15,15,0,0,0,3,5,17,0,0,30,0,4,4,6,6,6,11,11,11,0,0,14,0,20,20,0,0,4,0,0,18,6,6,20,0,0,26,0,0,22,0,14,14,10,10,20,7,7,8,0,0,10,3,3,27,0,0,2,0,6,8,0,16,16,8,9,9,0,6,14,6,6,24,3,4,21,2,2,2,13,13,13,0,0,12,0,12,12,0,14,20,0,0,16,0,6,6,9,17,17,0,0,20,0,16,22,0,5,11,10,10,10,0,0,24,6,16,16,2,4,4,0,4,26,0,10,18,1,3,19,0,12,24,0,0,28,0,7,15,0,18,18,0,3,23,8,8,15,0,0,7,0,0,1,7,8,20,2,4,12,0,10,11,0,12,19,12,12,12,6,6,15,1,1,1,5,15,15,0,0,9,6,7,7,0,0,5,5,5,5,3,4,23,0,3,18,10,10,17,0,8,26,12,12,18,8,8,8,0,13,13,0,11,22,8,8,22,0,3,25,1,1,11,0,8,9,0,3,16,0,6,20,1,1,29,9,9,16,0,15,15,0,9,10,4,6,14,6,6,17,1,1,15,5,13,21,1,1,13,0,8,24,8,10,10,0,0,8,0,0,11,0,2,22,0,9,21,4,4,5,6,13,13,0,10,16,5,17,19,3,3,5,3,3,3,5,5,23,});
sparse_graph4_ints[25][6] = vector<int>({0,2,2,0,0,0,0,0,2,1,1,2,0,0,3,0,0,1,0,1,2,1,1,1,0,1,1,});
sparse_graph4_ints[25][8] = vector<int>({2,2,2,0,0,0,0,0,2,0,0,4,0,2,3,0,0,1,1,1,1,1,2,2,1,1,3,0,2,2,0,1,1,0,0,3,1,1,2,0,1,3,0,1,2,});
sparse_graph4_ints[25][10] = vector<int>({1,3,3,0,0,0,0,0,2,0,0,4,0,2,2,0,0,5,2,2,3,1,1,1,0,0,1,0,2,4,2,2,2,0,3,3,0,0,3,1,2,3,0,1,1,1,1,2,0,1,2,1,1,4,0,2,3,1,2,2,0,1,4,1,1,3,0,1,3,});
sparse_graph4_ints[25][12] = vector<int>({0,0,0,3,3,3,0,0,6,0,0,2,0,2,2,0,0,4,0,4,4,2,2,2,0,0,1,1,1,1,1,1,5,1,1,3,0,0,3,2,3,3,1,2,2,2,2,4,0,0,5,0,3,3,1,3,4,0,2,3,0,2,5,1,3,3,0,3,4,0,2,4,2,2,3,1,1,2,0,1,1,0,1,2,1,2,4,1,2,3,0,1,3,0,1,5,0,1,4,1,1,4,});
sparse_graph4_ints[25][14] = vector<int>({3,3,4,0,0,0,0,0,4,0,0,7,0,0,2,2,2,2,0,2,2,0,0,6,0,4,4,0,0,1,1,1,1,0,2,6,0,0,5,2,4,4,1,3,3,1,2,2,0,4,5,3,3,3,0,2,4,0,0,3,0,3,3,1,1,3,2,2,3,1,3,5,1,1,6,2,3,4,1,4,4,1,1,5,2,3,3,0,2,3,0,3,4,2,2,5,2,2,4,1,2,3,0,3,5,0,1,6,1,1,2,0,1,1,1,3,4,1,2,5,1,2,4,0,1,4,0,1,2,0,1,5,1,1,4,0,2,5,0,1,3,});
sparse_graph4_ints[25][16] = vector<int>({0,0,8,0,0,0,4,4,4,2,2,2,0,0,2,0,0,4,0,0,6,0,4,4,0,4,6,0,2,2,0,0,1,1,1,7,1,1,1,3,3,3,0,0,5,1,3,3,1,5,5,1,1,5,1,2,2,2,2,6,0,0,7,0,2,4,0,0,3,2,3,3,0,2,6,3,4,4,1,1,3,0,3,3,2,2,3,3,3,5,0,5,5,0,2,7,2,2,4,0,3,4,0,2,3,0,1,1,1,1,2,3,3,4,2,4,5,0,4,5,2,4,4,0,3,6,2,3,4,1,2,3,1,3,4,0,3,5,1,2,4,0,1,6,1,4,4,1,2,6,2,3,5,1,4,5,1,3,6,1,3,5,0,2,5,0,1,5,1,2,5,2,2,5,0,1,7,0,1,4,0,1,2,1,1,4,1,1,6,0,1,3,});
sparse_graph4_ints[25][18] = vector<int>({0,0,8,0,0,0,4,4,5,1,1,1,0,4,4,0,0,6,2,2,2,0,2,8,0,0,1,0,0,4,0,6,6,0,0,2,3,3,3,0,2,2,0,2,6,0,0,9,1,3,7,0,0,5,1,3,3,0,0,7,1,2,2,0,4,6,4,4,4,3,3,4,1,1,5,0,0,3,0,2,4,2,3,3,1,1,7,3,5,5,0,4,5,2,4,4,1,1,3,2,2,3,2,2,7,0,3,3,2,2,4,0,5,5,0,4,7,3,4,4,1,3,5,1,1,8,2,2,6,3,3,6,3,4,5,1,5,5,2,3,4,0,3,4,3,3,5,0,3,6,2,4,6,1,4,4,2,5,5,0,5,6,1,5,6,0,3,7,0,1,6,0,3,5,0,1,7,1,3,4,2,2,5,1,2,7,2,3,5,2,3,6,0,2,3,0,1,1,1,1,2,1,2,3,1,2,6,1,2,4,1,2,5,1,4,5,0,1,5,0,2,5,2,4,5,0,2,7,1,1,6,0,1,8,1,4,6,1,3,6,0,1,4,0,1,2,1,1,4,0,1,3,});
sparse_graph4_ints[25][20] = vector<int>({0,0,0,5,5,5,2,2,2,0,0,10,1,1,1,0,4,6,0,0,8,0,0,6,0,0,1,0,0,4,0,4,4,0,6,7,1,1,9,0,0,2,4,4,4,0,2,2,3,3,3,0,4,8,1,1,7,0,0,7,0,0,5,1,3,3,1,2,2,3,3,7,1,5,5,0,2,6,0,0,9,3,3,4,2,2,8,0,6,6,0,2,8,1,1,5,2,6,6,2,2,6,0,2,4,2,3,3,0,0,3,0,4,5,3,4,4,4,4,6,0,5,5,1,1,3,2,2,3,2,4,4,2,2,4,0,3,3,3,3,5,4,5,5,1,3,7,0,1,1,0,3,6,0,2,9,0,5,6,1,5,7,4,4,5,2,3,4,0,3,7,0,3,4,1,3,5,0,1,6,0,1,7,0,2,7,1,4,4,3,4,5,1,3,8,0,1,8,0,3,8,3,5,5,3,5,6,2,3,7,2,4,5,1,6,6,0,4,7,2,3,5,0,3,5,1,2,7,0,5,7,3,3,6,1,2,6,1,4,5,2,3,6,1,3,4,2,2,5,3,4,6,2,5,5,2,4,7,2,2,7,2,4,6,0,2,3,1,1,2,1,2,8,1,2,3,1,2,4,1,1,6,0,1,5,1,2,5,1,3,6,1,4,6,2,5,6,1,4,7,});
sparse_graph4_ints[25][23] = vector<int>({0,0,8,0,0,0,5,6,6,0,2,2,0,3,10,3,3,3,0,1,1,0,0,6,0,0,11,4,4,4,0,7,8,0,3,3,0,2,8,0,0,1,0,6,6,0,0,5,0,0,3,0,2,6,0,0,10,0,5,5,2,2,3,0,5,9,0,0,4,0,0,7,4,5,5,0,0,9,0,4,8,0,4,4,1,2,2,0,1,11,2,2,7,3,4,8,0,2,7,3,3,4,1,1,9,0,6,7,2,3,9,2,7,7,0,5,6,4,4,5,3,4,4,2,3,3,0,4,5,2,2,8,1,1,1,0,0,2,0,3,4,5,5,5,2,2,2,2,2,6,3,3,7,1,2,10,1,1,5,0,4,7,0,7,7,5,5,6,0,2,3,1,1,2,0,2,4,1,1,8,0,2,9,3,3,6,0,4,9,3,3,8,1,1,7,0,4,6,0,6,8,4,4,7,3,5,5,3,3,5,0,3,8,2,3,4,0,1,7,1,7,7,0,5,8,3,4,5,2,4,4,0,3,6,0,1,9,1,5,5,4,6,6,2,6,6,2,3,7,0,2,10,2,2,9,4,4,6,0,3,7,2,2,4,3,6,7,1,4,9,0,5,7,0,3,9,1,6,6,4,5,7,1,3,3,1,1,6,0,1,8,2,5,8,3,4,6,2,5,5,});
sparse_graph4_ints[25][26] = vector<int>({0,0,0,6,6,7,0,0,6,0,0,13,4,4,4,1,1,1,0,0,10,0,4,4,0,6,8,0,0,8,0,0,1,1,3,11,0,0,4,2,2,2,0,2,10,0,5,5,2,8,8,0,0,2,5,5,6,0,6,10,1,1,7,0,6,6,0,0,5,0,0,12,1,3,7,0,0,7,4,5,5,3,3,3,1,3,9,0,2,2,1,3,3,0,2,12,3,5,9,0,0,9,0,8,8,0,0,11,1,2,2,0,2,8,3,3,9,1,1,9,3,3,7,0,2,6,3,4,4,1,7,7,0,4,8,0,4,10,5,5,5,1,1,11,0,6,7,3,3,10,0,8,9,2,2,6,4,4,5,1,1,5,2,3,3,0,4,5,2,2,8,3,3,4,0,5,6,5,5,8,3,3,8,0,2,4,4,4,8,0,0,3,6,6,6,2,6,6,3,7,7,5,7,7,2,2,10,0,4,11,4,6,6,0,6,9,4,4,9,0,1,1,0,7,7,0,5,8,3,5,5,4,4,6,0,4,6,3,3,5,5,6,6,2,4,10,3,5,7,0,5,9,1,7,9,0,3,10,1,5,5,0,3,8,3,3,6,2,4,8,2,2,11,0,7,8,4,6,8,2,4,4,0,3,6,2,2,3,0,3,9,4,4,7,5,5,7,2,7,7,});
sparse_graph4_ints[25][30] = vector<int>({0,0,6,7,7,8,0,0,0,0,0,15,4,4,4,0,0,10,1,1,1,0,6,12,0,4,4,0,0,12,5,6,6,0,10,10,0,6,8,0,0,8,0,0,1,0,0,4,0,2,12,0,5,5,2,2,2,0,8,8,0,2,10,0,0,2,2,2,13,5,5,10,5,5,5,0,0,5,0,2,8,0,6,10,0,0,7,0,6,6,0,0,14,3,3,3,0,4,10,2,8,8,1,3,3,0,0,9,0,2,14,3,3,7,0,2,2,3,9,9,0,0,11,5,7,7,4,4,8,3,4,4,0,0,13,1,3,11,1,7,11,1,2,2,3,3,11,1,1,7,1,1,9,0,4,8,4,5,5,1,1,11,3,3,9,1,7,7,4,4,11,0,2,6,1,3,7,0,4,12,1,3,9,3,5,9,0,8,10,1,1,13,6,6,6,0,6,7,3,3,12,4,4,5,3,3,4,4,6,6,0,8,9,4,4,10,6,7,7,3,3,8,3,7,7,1,9,9,0,5,6,3,3,10,0,6,9,2,2,8,0,4,5,0,4,13,2,3,3,5,5,9,6,6,9,2,2,10,0,6,11,2,2,6,1,1,5,4,8,8,2,2,12,0,0,3,2,6,6,0,2,4,6,8,8,0,8,11,3,7,10,4,6,8,0,9,9,7,7,7,});
sparse_graph4_ints[25][35] = vector<int>({0,0,0,7,7,7,3,4,4,0,0,17,0,0,5,0,0,13,0,0,8,0,11,12,0,5,15,3,3,9,0,0,3,8,9,9,0,0,11,0,6,12,0,0,15,5,6,6,2,3,3,0,0,1,0,10,10,4,4,5,4,5,13,2,2,2,0,2,8,0,8,8,0,1,1,0,0,7,0,4,12,0,2,2,0,2,10,1,1,15,6,7,11,3,3,3,0,7,7,5,5,10,0,2,12,0,0,9,0,3,16,2,8,8,0,6,6,5,5,5,0,0,6,0,9,13,5,10,10,0,4,9,2,2,3,6,6,7,0,0,10,4,4,10,0,0,12,3,3,13,0,0,14,4,9,9,0,8,12,0,0,16,3,3,4,0,7,14,0,6,10,7,8,8,2,2,13,1,1,13,0,4,10,0,1,17,0,0,4,1,2,2,0,2,9,0,4,14,4,4,4,0,8,11,0,2,7,0,2,14,4,5,5,4,8,8,4,4,9,0,7,8,2,2,8,5,5,6,4,4,12,1,1,11,5,6,12,0,8,9,3,4,14,6,6,10,2,11,11,0,5,6,0,2,6,0,10,11,0,4,11,0,6,7,6,6,6,0,6,11,0,6,13,1,9,9,3,7,11,3,3,10,3,3,12,3,3,8,0,4,6,0,8,10,4,4,11,3,8,12,});
sparse_graph4_ints[25][40] = vector<int>({0,0,0,0,0,14,7,11,11,0,0,20,3,4,4,7,7,7,2,2,2,5,5,5,3,3,13,0,0,2,0,0,8,0,12,12,6,6,14,0,0,4,0,4,18,0,0,10,0,0,16,0,9,9,0,2,10,0,8,14,0,4,12,8,8,8,0,0,18,3,3,3,0,10,10,1,13,13,3,5,5,4,4,16,0,0,1,1,1,1,0,8,8,0,3,11,0,10,15,1,1,3,0,0,7,3,3,15,5,5,7,1,1,17,1,1,15,9,9,9,0,4,4,5,5,13,0,11,11,0,0,13,5,7,7,0,6,12,0,6,8,0,0,9,0,0,11,1,1,19,10,10,10,2,4,4,0,6,6,0,0,6,6,6,10,7,7,11,0,8,12,1,1,13,0,8,16,5,9,13,1,3,3,0,0,12,5,5,10,0,4,16,3,3,9,3,3,11,1,3,9,3,11,11,5,9,9,0,4,14,0,3,8,8,8,12,0,0,17,0,0,15,0,6,14,0,6,16,4,5,12,4,12,12,4,4,10,4,8,12,0,2,8,0,0,19,3,3,17,6,6,6,3,9,9,6,10,10,4,5,5,5,5,6,0,2,16,0,2,14,0,10,13,4,4,4,0,6,10,5,5,15,0,12,14,1,7,11,1,7,7,4,4,14,0,2,18,1,3,15,});
sparse_graph4_ints[25][45] = vector<int>({8,8,8,2,2,2,0,0,0,0,0,17,11,11,11,0,6,18,5,6,6,0,0,13,0,0,9,0,15,15,0,0,22,0,10,10,0,1,1,6,7,16,4,4,5,0,4,16,1,1,2,0,9,14,0,0,11,0,0,15,3,3,12,3,3,4,0,0,19,1,13,13,9,9,9,2,3,3,0,9,18,2,2,10,6,7,7,6,13,13,0,0,7,0,0,1,1,2,21,5,6,15,0,12,12,5,5,5,2,3,11,0,0,3,0,0,5,1,2,9,8,9,14,0,9,9,4,4,12,0,3,18,0,12,16,0,11,11,3,4,16,6,7,12,10,10,10,0,5,6,0,8,8,0,2,20,1,1,16,7,7,8,0,0,10,0,0,8,5,12,12,0,8,13,0,0,20,3,3,3,4,5,18,4,11,11,0,5,20,0,7,7,0,3,14,0,0,14,8,8,12,6,6,11,0,0,12,4,4,4,0,8,16,0,11,14,0,7,12,0,0,16,3,3,15,6,6,6,0,0,6,0,3,12,4,11,15,5,5,14,4,9,14,1,3,3,2,3,14,0,6,11,4,5,13,0,0,18,2,2,19,0,3,10,0,10,16,4,10,10,4,4,17,5,6,10,5,5,9,0,0,21,1,1,17,0,5,17,0,7,19,0,2,14,0,6,9,5,5,6,});
sparse_graph4_ints[25][50] = vector<int>({0,10,20,5,15,15,0,0,0,0,0,10,0,0,17,0,0,25,0,0,6,10,10,10,6,7,7,1,7,7,1,13,13,0,0,13,0,0,22,0,0,2,8,8,8,4,4,4,0,8,18,12,12,13,0,0,4,3,3,3,2,3,14,0,6,22,0,4,20,5,5,13,0,0,15,5,9,15,0,8,9,1,1,19,0,0,8,0,12,12,1,15,15,0,10,11,9,9,9,0,4,12,4,10,18,11,11,11,1,3,3,0,0,1,0,1,18,0,8,16,1,1,1,3,3,15,2,2,10,1,1,3,5,5,18,0,0,11,10,10,15,6,6,6,2,4,4,7,7,13,3,3,17,0,16,17,1,9,9,0,11,17,7,13,13,0,1,23,0,6,7,1,3,23,5,5,20,7,7,8,6,12,12,8,8,17,0,0,20,0,6,12,2,4,20,5,5,6,0,8,14,1,5,5,0,11,15,0,0,12,8,9,14,0,0,9,5,7,13,0,4,4,0,7,13,0,14,18,1,14,14,0,0,14,1,1,21,4,11,11,4,4,17,4,5,5,0,2,16,3,10,10,5,5,11,0,2,24,6,12,16,1,3,13,0,0,16,8,14,14,0,3,17,6,7,17,2,12,16,0,3,22,0,4,18,0,8,20,1,3,11,5,6,16,6,11,11,0,3,9,});
sparse_graph4_ints[25][55] = vector<int>({3,12,17,0,1,19,0,0,7,0,1,27,13,14,14,0,6,8,8,8,9,0,1,2,0,9,9,0,8,18,0,0,11,0,18,18,0,0,0,0,0,23,0,0,15,0,7,22,0,14,14,0,11,11,9,10,10,0,0,5,6,6,21,12,12,12,0,0,9,0,0,17,0,6,14,0,0,13,10,11,17,4,4,16,0,6,6,7,7,16,5,15,15,0,1,24,6,7,7,3,3,4,0,0,1,0,8,8,1,12,21,3,4,24,9,9,16,0,2,20,0,12,12,0,11,18,11,11,11,0,5,12,6,6,14,2,3,3,0,1,1,0,7,15,0,3,24,1,1,2,0,3,3,5,5,18,5,16,17,0,16,17,0,3,16,1,2,21,8,8,19,5,12,13,0,0,26,7,7,8,0,11,20,0,13,13,0,6,21,5,13,18,0,9,23,1,15,15,6,10,11,1,3,11,5,5,20,1,3,17,5,6,12,0,10,10,7,13,13,0,15,20,5,6,6,0,11,16,5,5,5,4,4,5,7,7,18,4,4,11,10,15,15,0,0,10,0,3,26,3,4,4,0,0,12,0,0,18,9,9,13,0,10,15,0,5,18,0,0,21,3,3,19,0,2,14,7,7,14,5,5,11,0,1,13,9,14,14,3,3,3,0,0,8,2,2,22,2,2,14,});
sparse_graph4_ints[25][60] = vector<int>({9,9,21,0,0,30,0,8,8,15,15,15,11,11,11,0,0,7,0,12,14,0,0,18,0,0,25,1,1,1,0,10,22,4,4,4,0,17,17,0,0,14,0,10,10,6,18,18,0,0,0,0,0,21,5,5,25,0,16,22,7,8,8,13,13,13,0,15,15,7,7,17,2,2,26,0,0,9,1,2,2,0,10,18,7,11,17,0,0,16,3,4,14,12,12,18,0,3,21,10,10,10,0,0,12,8,9,9,0,12,24,0,6,21,0,7,7,7,15,15,0,0,28,0,0,1,0,8,15,0,20,20,0,4,13,1,5,27,2,4,4,6,6,15,0,4,24,12,12,12,0,8,10,6,14,20,0,13,21,0,12,12,0,4,18,0,0,2,7,7,23,0,2,18,0,14,18,0,4,4,0,0,4,0,7,23,0,5,14,7,7,19,6,11,11,10,11,17,0,3,12,2,18,18,0,0,23,7,7,7,0,0,6,5,5,19,6,14,14,1,1,9,0,11,11,0,8,18,8,16,16,4,4,24,0,3,10,6,6,13,6,6,7,8,8,20,3,14,19,6,12,13,0,1,26,0,10,20,6,6,22,0,2,29,3,4,17,0,14,14,0,3,23,5,6,6,10,16,17,5,9,15,9,9,15,5,5,5,0,0,10,4,12,22,14,14,14,5,5,21,});
sparse_graph4_ints[20][6] = vector<int>({0,2,2,0,0,0,0,0,2,0,0,1,0,0,3,1,1,2,1,1,1,0,1,2,0,1,1,});
sparse_graph4_ints[20][8] = vector<int>({2,2,2,0,0,0,0,0,2,0,0,4,0,0,1,1,1,1,0,2,2,0,2,3,0,0,3,1,1,3,0,1,1,1,2,2,0,1,2,0,1,3,1,1,2,});
sparse_graph4_ints[20][10] = vector<int>({2,2,3,0,0,0,0,0,2,0,0,5,0,2,2,0,0,1,0,0,4,1,1,1,0,2,4,1,3,3,0,0,3,1,1,3,2,2,2,0,3,3,1,2,2,0,1,1,1,1,2,0,1,2,0,2,3,1,1,4,0,1,3,0,1,4,1,2,3,});
sparse_graph4_ints[20][12] = vector<int>({0,4,4,0,0,0,1,1,1,0,0,6,0,0,4,0,0,1,2,2,2,0,0,2,3,3,3,0,2,2,1,1,5,0,0,5,1,1,3,0,0,3,0,2,4,1,3,3,2,2,4,1,2,2,0,1,1,0,3,3,1,1,2,0,1,4,0,2,5,0,2,3,2,3,3,0,1,2,0,1,3,2,2,3,0,1,5,0,3,4,1,2,3,1,3,4,1,2,4,1,1,4,});
sparse_graph4_ints[20][14] = vector<int>({3,3,4,0,0,0,1,1,1,0,0,6,2,2,2,0,0,1,0,0,4,0,2,6,0,0,2,0,2,2,0,4,4,0,0,7,0,0,5,1,3,3,1,1,3,0,4,5,0,2,4,1,1,5,0,0,3,3,3,3,2,2,4,2,4,4,1,3,5,0,1,1,1,2,2,2,2,3,0,3,3,0,1,5,2,3,3,0,3,4,1,1,6,2,2,5,0,1,4,1,1,2,0,2,3,0,1,3,0,1,2,0,2,5,0,3,5,1,2,3,0,1,6,1,2,4,1,4,4,2,3,4,1,2,5,1,1,4,1,3,4,});
sparse_graph4_ints[20][16] = vector<int>({0,0,0,0,0,8,4,4,4,1,1,1,2,2,2,0,0,6,0,4,4,0,0,1,0,0,4,1,1,7,0,0,2,1,5,5,0,2,2,3,3,3,1,1,5,0,0,5,2,2,6,0,0,7,1,2,2,1,3,3,0,2,6,0,4,6,0,2,4,0,0,3,2,2,4,1,3,5,3,3,5,2,4,4,0,1,1,1,1,3,0,4,5,2,3,3,0,3,3,2,2,3,0,1,5,0,1,6,0,5,5,0,3,4,0,2,7,0,3,5,3,3,4,0,1,4,1,1,2,0,3,6,0,1,7,2,2,5,3,4,4,1,4,4,1,2,5,0,2,3,0,2,5,2,3,4,1,3,6,1,1,6,2,4,5,0,1,3,0,1,2,1,2,4,1,2,3,1,2,6,2,3,5,1,3,4,1,1,4,1,4,5,});
sparse_graph4_ints[20][18] = vector<int>({0,6,6,0,0,0,0,0,6,1,1,1,3,3,3,0,0,8,2,2,2,0,0,1,0,0,4,0,2,8,0,0,2,0,2,6,3,5,5,0,0,5,1,3,3,0,0,9,0,4,6,0,2,2,0,0,7,0,4,4,2,2,7,1,2,2,3,3,4,1,1,7,1,1,5,4,4,4,1,5,5,0,2,4,0,4,5,2,3,3,0,4,7,2,2,6,0,0,3,3,3,5,3,3,6,0,5,5,4,4,5,0,3,6,0,1,1,2,4,4,2,2,5,3,4,4,2,4,6,0,1,7,2,2,4,1,3,7,0,1,6,2,2,3,0,1,5,1,1,3,0,3,3,0,3,4,0,3,5,0,3,7,1,3,5,1,1,8,0,5,6,0,1,4,2,5,5,0,1,8,1,1,2,1,2,6,1,4,4,1,1,6,2,3,5,0,2,7,0,2,5,0,2,3,2,3,6,1,2,5,2,3,4,0,1,3,0,1,2,1,2,7,1,2,4,1,2,3,2,4,5,1,5,6,1,4,5,3,4,5,1,3,6,1,3,4,1,1,4,1,4,6,});
sparse_graph4_ints[20][20] = vector<int>({0,0,0,5,5,5,0,0,4,0,0,10,0,4,4,0,0,6,0,0,8,0,0,1,0,6,6,1,1,1,2,2,2,0,4,6,1,1,9,0,0,2,4,4,4,0,4,8,1,1,7,0,0,5,3,3,3,0,0,7,0,2,2,2,2,3,3,3,7,0,0,9,1,2,2,2,6,6,0,2,6,1,5,5,0,2,8,3,3,4,1,1,5,2,2,8,0,6,7,2,2,6,0,4,5,0,2,4,3,3,5,0,4,7,2,3,3,4,4,6,3,5,5,0,0,3,2,2,7,0,5,5,0,3,6,1,5,7,3,3,6,3,4,4,0,3,4,0,3,7,0,1,7,0,1,8,0,1,1,0,5,6,2,2,5,2,4,4,0,3,8,2,5,5,2,2,4,4,4,5,0,1,6,0,5,7,0,1,5,0,1,9,0,3,3,1,1,3,1,3,3,2,4,6,0,3,5,1,3,7,2,3,6,0,2,9,2,4,7,1,2,7,1,3,5,0,1,4,3,5,6,2,3,7,1,1,6,0,2,7,1,4,4,2,3,5,1,1,2,1,1,8,1,2,6,4,5,5,3,4,5,1,6,6,3,4,6,0,2,3,0,2,5,1,2,5,1,3,8,2,3,4,2,4,5,0,1,3,0,1,2,1,2,8,1,5,6,1,4,6,});
sparse_graph4_ints[20][23] = vector<int>({0,0,0,0,3,10,0,0,6,5,6,6,3,3,3,0,1,1,0,2,2,0,0,8,4,4,4,0,0,10,0,7,8,2,2,2,0,0,1,0,2,8,0,0,3,0,0,5,0,6,6,0,1,11,0,2,6,0,5,5,2,2,3,4,4,7,0,0,7,2,2,8,0,0,9,0,5,9,0,4,7,0,0,4,0,0,11,1,2,2,2,3,3,1,1,9,2,6,6,1,1,7,3,3,7,3,4,4,5,5,5,2,7,7,2,2,6,0,4,5,0,6,7,4,4,5,2,3,9,1,1,5,0,4,8,3,3,4,0,4,4,4,5,5,2,2,7,1,5,5,3,3,6,0,7,7,2,5,8,0,1,5,0,5,8,0,3,9,4,5,7,0,1,9,0,1,10,0,3,6,3,3,8,1,1,1,0,3,8,0,4,9,0,6,8,0,4,6,2,2,9,3,6,6,4,4,6,0,1,8,0,1,7,0,5,7,1,1,10,0,3,4,1,2,7,3,5,5,0,5,6,3,4,8,2,5,5,0,0,2,3,3,5,3,4,7,2,2,5,5,5,6,2,3,7,1,5,7,0,3,3,2,3,8,1,2,5,0,1,6,3,6,7,1,6,6,2,4,4,1,7,7,2,3,6,3,4,6,4,6,6,0,2,4,1,2,10,0,2,3,});
sparse_graph4_ints[20][26] = vector<int>({0,5,5,0,0,0,0,2,12,5,7,7,0,0,6,1,1,1,0,0,10,4,5,5,0,0,8,0,6,10,0,0,1,2,3,3,1,2,2,0,2,10,0,6,6,0,0,2,0,0,4,0,0,12,0,6,8,2,8,8,0,2,8,2,2,2,4,6,6,0,0,7,0,4,10,1,1,7,3,3,7,3,4,4,1,3,3,0,0,13,3,5,9,3,3,3,0,0,11,3,3,9,0,4,8,0,8,8,4,4,4,0,0,9,1,1,9,2,2,11,0,0,5,1,1,11,0,2,6,1,7,7,0,2,2,2,2,7,5,5,5,2,2,9,6,6,6,0,6,7,4,4,8,3,3,10,5,5,7,0,8,9,3,3,8,2,2,6,3,5,5,0,4,5,4,6,8,3,7,7,0,6,9,1,5,5,2,6,6,3,3,4,1,1,5,4,4,7,2,2,8,0,4,11,2,2,10,0,4,4,2,6,8,0,5,8,0,2,4,3,3,6,1,7,9,0,3,10,0,7,7,0,5,9,6,6,7,0,1,11,0,4,7,5,5,8,5,6,6,0,1,10,0,1,9,0,3,9,4,4,9,0,3,8,3,6,6,4,4,6,2,5,8,2,7,7,5,5,6,0,0,3,0,4,9,0,4,6,0,3,6,0,5,10,0,3,11,0,7,8,});
sparse_graph4_ints[20][30] = vector<int>({0,0,15,0,6,6,1,1,1,6,8,8,0,0,8,0,0,0,0,6,12,0,0,11,0,8,8,3,3,3,1,2,2,0,10,10,1,3,11,6,6,6,0,0,1,1,3,3,0,0,13,0,2,8,0,0,6,1,3,9,0,0,2,0,0,4,5,5,10,1,3,13,0,6,8,3,4,4,4,4,9,4,4,5,5,5,6,0,0,9,2,8,8,2,3,3,0,0,7,0,2,14,2,2,2,0,2,12,0,6,10,0,0,10,0,0,12,2,2,8,3,3,4,3,9,9,0,8,11,0,0,14,3,3,9,4,5,5,1,1,9,1,1,11,1,7,7,4,4,11,5,7,7,4,4,4,0,4,10,0,2,10,1,1,13,1,1,7,3,7,7,5,5,5,2,2,12,3,3,11,0,6,7,1,3,7,0,0,5,0,2,6,7,7,7,0,2,2,4,4,10,1,9,9,4,4,8,3,3,8,3,3,12,4,8,8,0,4,6,0,8,10,1,7,9,5,5,9,3,3,10,5,7,9,0,6,9,0,6,11,6,6,8,2,8,10,3,3,7,5,6,6,5,5,8,3,7,9,0,4,5,0,5,6,2,6,11,2,6,6,4,4,7,2,7,7,2,6,10,0,5,11,0,9,9,2,2,10,0,1,13,0,4,8,0,5,12,7,7,8,});
sparse_graph4_ints[20][35] = vector<int>({3,4,4,0,0,0,2,2,2,0,11,12,0,0,13,0,5,15,7,7,7,5,5,5,0,0,16,0,0,8,0,1,1,0,4,12,8,9,9,0,0,10,1,1,2,0,10,10,6,6,6,4,5,13,3,3,3,0,8,8,0,2,10,0,1,17,0,0,1,0,0,3,0,0,5,6,7,11,4,4,5,0,0,7,5,5,10,0,4,14,0,9,9,0,2,12,4,4,9,2,3,3,0,7,7,0,0,14,0,6,6,5,9,9,0,0,9,3,3,9,0,6,13,0,0,11,0,5,5,0,2,14,0,9,13,0,3,16,1,3,9,0,8,12,0,6,11,1,3,3,0,0,6,0,0,12,5,10,10,4,8,8,0,7,14,0,3,8,0,0,15,3,3,12,5,5,6,1,1,15,6,6,10,0,4,10,8,8,8,0,2,8,2,10,10,4,4,12,1,1,13,6,6,7,0,0,17,2,2,13,4,4,4,4,5,5,4,4,11,5,6,6,0,8,11,3,3,10,0,7,8,1,1,11,0,6,7,5,6,12,3,9,9,5,5,11,0,4,11,2,11,11,4,4,10,2,2,8,0,8,9,3,4,14,3,8,11,1,1,7,3,3,13,3,7,9,3,3,8,0,6,10,7,7,8,0,10,11,0,7,11,3,7,8,2,9,12,3,3,11,});
sparse_graph4_ints[20][40] = vector<int>({0,12,12,0,8,8,3,3,3,10,10,10,0,0,0,0,0,15,4,5,11,0,0,6,0,0,18,3,5,5,6,6,14,0,4,18,8,8,8,0,0,2,0,0,10,0,0,4,0,0,13,0,8,14,2,3,12,0,0,20,0,0,8,4,12,12,7,7,7,4,4,16,0,9,9,3,4,4,0,0,1,1,3,3,0,10,15,0,4,11,1,1,1,0,3,9,6,10,10,1,1,3,5,5,5,0,3,17,0,10,10,5,5,7,8,8,12,0,2,16,4,8,12,0,6,14,0,11,11,0,0,11,2,4,4,6,6,7,0,5,9,0,4,4,0,0,9,0,2,14,9,9,9,3,3,13,5,9,9,1,1,19,0,12,14,0,5,16,4,4,10,4,5,14,0,0,16,7,11,11,5,9,13,4,4,4,0,0,12,0,2,12,7,7,11,0,6,17,4,4,13,3,3,10,0,3,10,0,0,14,0,8,16,0,9,12,6,6,11,0,8,11,4,8,8,0,0,17,3,11,11,6,6,6,0,10,13,4,5,5,0,0,7,1,3,15,0,6,12,5,5,6,0,7,10,2,2,14,0,7,8,2,2,2,1,1,15,0,0,19,3,9,9,7,7,8,3,7,15,1,13,13,3,4,15,0,3,8,1,1,13,4,6,12,1,1,17,5,6,10,});
sparse_graph4_ints[20][45] = vector<int>({8,12,12,5,5,5,0,1,22,0,0,18,0,0,3,0,0,0,0,11,11,0,9,9,9,9,9,3,4,4,3,4,19,0,13,13,0,0,12,0,5,17,0,4,4,0,15,15,0,5,14,0,0,5,1,3,12,7,8,15,0,11,17,0,0,10,0,0,14,6,7,7,8,8,8,4,5,15,0,1,19,0,0,1,0,0,16,0,0,7,6,7,14,0,1,1,4,4,5,5,5,11,10,10,10,1,1,2,2,2,10,0,0,21,0,10,15,2,2,2,5,6,6,0,0,9,0,8,14,0,10,10,5,6,17,0,8,8,3,3,4,5,10,10,0,6,18,11,11,11,0,12,12,0,3,21,3,3,15,4,13,13,0,3,14,0,2,20,0,0,11,8,8,12,4,5,10,4,4,4,5,5,13,7,7,8,9,9,13,0,9,18,3,3,3,0,6,11,4,11,12,0,0,13,0,0,8,1,1,17,0,7,7,5,5,16,7,11,11,0,7,13,0,14,14,6,6,13,3,3,12,0,0,15,0,3,18,6,6,6,2,2,11,0,4,19,4,10,14,0,8,17,5,9,9,0,7,16,0,4,16,1,2,9,6,11,14,0,5,20,7,8,11,0,0,17,1,2,15,4,8,12,0,13,16,4,9,16,3,9,9,0,3,10,0,12,15,0,10,13,});
sparse_graph4_ints[20][50] = vector<int>({7,7,13,3,3,22,6,6,6,0,0,6,12,12,13,0,0,17,0,0,0,4,4,4,0,0,14,0,0,10,0,12,12,5,15,15,0,5,18,10,10,10,0,14,14,0,0,21,0,0,24,3,3,13,8,9,16,1,10,10,0,10,18,0,0,2,0,14,18,0,6,8,0,0,4,3,3,3,8,8,9,0,0,19,5,5,20,0,8,8,0,0,8,7,7,8,7,13,13,0,0,1,0,10,15,0,1,22,1,1,11,0,4,16,1,1,1,8,8,14,4,4,19,1,1,3,1,3,11,6,6,12,1,15,15,1,9,9,1,1,5,6,6,16,2,2,13,0,0,11,0,9,14,11,11,12,7,7,17,0,3,22,4,13,13,0,11,19,3,4,14,0,13,13,3,4,4,5,5,5,9,11,15,4,4,16,6,6,8,0,6,19,0,2,24,2,4,4,1,12,16,0,0,15,0,8,21,0,0,12,3,5,5,9,9,9,9,10,13,0,3,18,4,10,18,2,2,20,1,16,16,7,11,11,5,6,15,0,8,12,5,11,15,5,5,7,5,10,11,8,14,14,0,6,16,0,2,16,0,6,12,1,11,15,4,9,10,6,12,16,0,2,20,0,5,21,0,9,17,0,0,9,2,2,12,4,6,12,5,6,18,0,0,16,4,12,12,0,4,4,});
sparse_graph4_ints[20][55] = vector<int>({11,11,11,0,0,0,0,12,12,4,4,5,0,3,17,0,1,27,0,11,22,0,0,24,0,1,15,11,11,16,4,11,16,6,6,7,9,9,9,5,15,15,0,8,21,0,5,5,0,14,14,0,0,12,1,18,18,0,1,1,0,4,20,13,14,14,1,1,2,1,4,25,1,1,4,6,7,19,5,5,13,0,0,21,6,6,15,2,3,3,0,15,20,0,0,14,10,10,10,0,10,10,3,4,20,0,2,16,0,16,16,4,5,6,0,7,14,0,0,9,10,10,15,4,5,23,6,13,18,4,4,17,7,7,11,0,0,26,1,10,17,0,0,1,0,1,2,1,4,23,0,11,15,0,13,19,0,8,10,0,0,5,0,0,19,0,11,11,0,0,7,7,8,8,8,8,13,0,6,10,12,12,13,5,12,17,9,9,14,1,5,13,1,2,19,6,7,21,0,0,11,0,1,22,5,6,18,2,2,22,3,4,5,6,13,14,6,6,9,9,9,18,10,15,15,5,5,7,5,12,13,0,6,17,1,3,13,1,4,9,5,16,17,0,10,21,0,3,12,0,12,18,3,4,4,8,8,17,0,8,18,0,17,17,0,0,13,3,12,12,0,9,15,4,11,20,4,4,22,0,0,10,0,15,15,7,7,16,9,13,13,0,2,25,8,14,15,0,0,17,});
sparse_graph4_ints[20][60] = vector<int>({0,0,7,15,15,15,9,9,9,6,18,18,0,0,30,0,4,10,0,5,21,11,11,12,0,7,16,7,7,7,1,1,1,10,10,20,0,0,10,0,0,21,0,16,22,0,0,0,0,5,14,7,7,21,8,15,15,0,3,25,0,12,20,5,13,13,0,12,24,7,7,17,0,14,14,0,0,18,0,0,12,2,2,28,1,1,3,1,18,18,0,0,15,0,10,17,10,10,10,3,3,3,1,6,22,0,1,24,10,10,16,0,10,12,0,20,20,1,5,5,0,15,20,2,12,12,0,6,10,5,5,22,4,6,25,0,0,23,13,14,14,0,0,1,0,8,10,7,7,11,2,2,20,7,14,14,0,13,17,6,8,15,0,0,28,0,3,14,0,10,10,12,12,18,0,17,17,0,6,15,0,0,2,4,4,5,12,12,13,0,0,4,4,5,19,4,12,17,1,1,18,6,8,23,0,7,18,5,13,21,9,9,14,2,2,2,5,5,5,0,0,26,1,11,22,6,8,9,7,15,19,1,2,27,5,6,15,0,11,13,3,4,25,0,4,4,1,9,20,0,4,18,6,6,7,11,12,16,12,16,16,8,9,20,5,6,6,0,0,13,5,17,17,6,8,19,4,14,19,0,6,6,2,4,24,2,15,15,6,11,11,9,16,16,0,9,24,7,10,18,});
sparse_graph4_ints[15][6] = vector<int>({0,2,2,0,0,0,0,0,2,0,0,1,0,0,3,1,1,1,1,1,2,0,1,1,0,1,2,});
sparse_graph4_ints[15][8] = vector<int>({2,2,2,0,0,0,0,0,4,0,0,1,1,1,1,0,0,2,0,2,2,1,1,3,0,0,3,0,1,1,0,2,3,0,1,2,0,1,3,1,2,2,1,1,2,});
sparse_graph4_ints[15][10] = vector<int>({1,3,3,0,0,0,1,1,1,0,0,5,0,0,1,0,0,4,0,0,2,0,2,2,0,2,4,1,1,3,2,2,2,0,0,3,0,1,1,2,2,3,1,2,2,0,3,3,0,1,3,0,1,4,1,1,4,1,1,2,0,2,3,0,1,2,1,2,3,});
sparse_graph4_ints[15][12] = vector<int>({0,0,0,3,3,3,0,0,4,0,0,6,0,0,1,1,1,1,2,2,2,0,0,2,1,1,5,0,4,4,0,2,2,0,0,5,1,1,3,0,2,4,1,3,3,0,0,3,2,2,4,0,1,1,1,2,2,0,3,3,0,1,4,0,1,5,0,1,3,0,2,5,2,2,3,0,3,4,2,3,3,1,1,2,0,2,3,1,1,4,0,1,2,1,2,3,1,3,4,1,2,4,});
sparse_graph4_ints[15][14] = vector<int>({3,3,3,0,0,0,0,0,4,0,0,7,2,2,2,0,0,1,1,1,1,0,0,2,0,0,5,0,2,6,0,4,4,0,0,6,0,2,2,1,1,5,1,2,2,2,4,4,2,2,3,0,2,4,0,4,5,2,2,4,2,2,5,0,0,3,0,3,4,3,3,4,0,1,5,2,3,3,0,1,4,0,3,3,1,3,5,0,1,1,1,3,3,0,3,5,1,1,3,0,1,6,0,1,3,1,1,6,0,2,5,1,4,4,1,1,2,1,1,4,0,2,3,0,1,2,1,2,4,1,2,5,2,3,4,1,3,4,1,2,3,});
sparse_graph4_ints[15][16] = vector<int>({1,1,1,1,5,5,0,0,0,0,0,8,2,2,2,0,0,1,0,0,6,3,3,3,0,0,4,0,0,2,1,1,7,4,4,4,1,1,5,0,0,5,0,4,4,2,2,6,0,2,2,0,0,7,1,2,2,0,2,6,2,2,3,0,4,6,0,2,4,0,4,5,3,3,5,2,4,4,2,3,3,2,2,5,0,0,3,0,3,4,0,3,5,0,3,6,0,5,5,0,1,6,0,1,5,0,1,7,3,3,4,2,2,4,0,3,3,0,1,4,0,1,1,1,3,5,1,3,3,1,4,4,3,4,4,1,1,3,2,3,5,0,1,3,0,2,7,1,2,5,2,4,5,1,1,6,1,3,6,0,2,5,2,3,4,1,2,6,1,1,2,1,1,4,0,2,3,0,1,2,1,2,4,1,4,5,1,3,4,1,2,3,});
sparse_graph4_ints[15][18] = vector<int>({0,0,4,3,5,5,0,0,8,0,0,0,2,2,3,0,0,1,0,0,6,0,5,5,1,1,1,0,2,8,0,0,2,1,2,2,0,2,6,3,3,3,0,6,6,0,0,9,2,2,2,0,0,7,0,4,7,0,4,4,0,0,5,0,2,2,1,1,7,3,3,5,2,2,6,4,4,4,1,1,5,0,4,5,3,3,4,2,4,6,0,2,4,2,2,7,2,3,3,1,5,5,0,4,6,0,3,6,4,4,5,0,1,7,3,4,4,2,2,5,3,3,6,0,0,3,0,3,7,0,3,4,1,2,5,0,1,6,2,4,4,0,1,8,0,1,5,2,5,5,0,5,6,2,2,4,0,3,3,0,1,4,1,3,7,2,3,5,0,1,1,1,4,4,2,3,6,1,3,3,1,3,5,1,1,3,1,1,8,2,4,5,1,2,6,0,3,5,0,1,3,3,4,5,0,2,7,1,2,7,1,1,6,1,5,6,0,2,5,2,3,4,1,3,6,1,4,6,1,1,2,1,4,5,1,1,4,0,2,3,0,1,2,1,2,4,1,3,4,1,2,3,});
sparse_graph4_ints[15][20] = vector<int>({0,0,10,0,0,0,0,0,4,5,5,5,0,4,4,0,4,8,0,0,6,0,0,8,0,0,1,1,5,5,1,1,1,2,2,2,0,0,2,1,1,7,2,6,6,1,2,2,3,3,3,1,1,9,4,4,4,0,0,7,2,2,3,3,3,7,0,0,9,3,3,4,0,2,6,0,2,8,0,0,5,0,6,6,2,2,7,0,2,2,2,2,6,0,4,5,0,6,7,1,1,5,3,5,5,4,4,6,0,5,5,3,3,5,2,2,8,3,3,6,2,3,3,0,1,5,0,4,7,0,3,6,0,4,6,1,5,7,0,1,8,0,1,9,0,3,7,0,1,7,0,3,8,3,4,4,4,4,5,2,5,5,0,5,7,0,5,6,2,2,5,1,2,5,0,3,4,2,4,6,0,0,3,0,1,6,2,3,6,3,5,6,2,4,4,0,2,4,2,3,7,3,4,6,4,5,5,2,4,7,2,2,4,2,3,5,0,3,3,1,6,6,0,1,4,3,4,5,0,1,1,1,4,4,1,3,7,1,2,7,1,3,5,1,3,3,1,2,8,2,5,6,2,4,5,1,1,3,1,5,6,1,2,6,1,1,8,0,2,9,1,3,8,0,3,5,0,1,3,0,2,7,1,4,7,1,1,6,0,2,5,1,4,6,2,3,4,1,3,6,});
sparse_graph4_ints[15][23] = vector<int>({0,7,8,0,0,0,0,0,6,0,1,11,4,4,4,0,0,9,0,1,1,0,0,4,3,3,3,5,6,6,0,0,1,2,2,3,0,0,3,0,2,8,0,2,6,0,6,6,0,0,5,0,3,10,4,4,7,0,0,10,0,5,5,2,2,2,0,4,8,0,0,7,0,5,9,0,0,8,2,2,7,2,6,6,3,3,7,1,1,9,2,3,3,0,0,11,1,1,7,3,4,4,1,2,2,5,5,5,2,7,7,4,4,5,0,6,7,0,2,2,2,2,6,3,3,8,0,4,5,0,5,6,1,2,9,3,3,4,4,5,5,3,3,6,0,7,7,1,1,5,2,2,9,2,5,8,4,5,7,0,5,8,0,1,10,0,1,9,0,3,8,3,6,6,4,4,6,0,4,4,2,2,8,0,1,5,0,6,8,1,1,10,4,6,6,0,3,6,3,4,7,2,3,9,0,5,7,2,5,5,0,4,9,0,1,8,1,2,7,3,5,5,0,1,7,5,5,6,0,4,7,3,3,5,0,4,6,2,3,8,2,6,7,3,4,8,3,4,6,2,3,7,1,5,5,1,5,7,2,2,5,1,2,5,2,5,7,2,3,6,1,6,6,0,3,4,4,5,6,0,1,6,1,1,1,3,6,7,2,4,6,3,5,7,2,4,8,2,4,4,});
sparse_graph4_ints[15][26] = vector<int>({0,8,8,1,3,3,0,0,0,0,0,13,1,1,1,0,0,10,0,5,5,4,5,5,1,3,11,6,6,6,0,0,8,0,4,8,0,0,1,1,2,2,0,0,6,0,0,2,4,4,9,0,0,4,2,3,3,0,5,9,4,4,4,1,1,9,3,3,3,0,6,7,0,6,10,1,1,11,2,2,2,0,0,7,0,0,9,0,0,12,1,7,7,0,2,8,3,4,4,5,5,6,0,2,12,3,7,7,0,0,11,1,1,7,0,6,6,1,3,9,4,4,5,3,3,9,2,8,8,5,5,5,2,2,10,0,2,6,2,2,7,1,7,9,5,7,7,3,3,10,0,1,11,4,4,8,3,3,8,0,0,5,0,7,7,4,6,8,3,3,7,5,5,7,2,2,6,0,6,9,4,4,7,1,2,9,0,5,6,0,2,2,0,4,5,2,6,8,6,6,7,3,3,4,1,2,10,0,5,10,3,3,6,2,6,6,5,6,6,0,2,10,5,5,8,0,5,8,2,7,7,1,1,5,0,8,9,0,6,8,0,1,12,1,2,11,3,6,6,2,5,8,4,7,7,0,1,10,4,6,6,0,1,9,3,6,8,2,5,9,0,3,8,4,4,6,0,4,10,2,2,8,2,5,6,0,7,9,0,4,4,3,4,7,4,5,7,3,5,9,});
sparse_graph4_ints[15][30] = vector<int>({3,9,9,1,3,9,0,0,0,0,0,8,1,1,1,0,6,12,5,6,6,3,3,4,0,2,14,1,3,3,0,0,11,0,0,13,0,0,1,1,2,2,1,7,7,7,7,7,0,0,6,0,0,2,4,4,4,0,6,10,0,0,4,5,5,10,2,8,8,0,0,9,4,5,5,2,3,3,0,0,15,0,2,12,3,3,3,0,8,11,4,4,8,0,4,11,0,4,13,0,6,7,0,0,10,3,4,4,0,4,8,0,0,7,0,0,12,0,10,10,2,2,2,4,4,10,0,2,8,6,6,7,5,5,5,0,8,8,0,2,10,0,0,14,1,1,9,4,4,5,3,3,11,2,2,10,3,3,9,1,1,13,2,2,12,1,1,11,4,8,8,1,7,9,0,6,6,0,7,7,6,6,6,4,4,11,1,9,9,1,1,7,6,8,8,3,3,8,4,4,9,3,3,12,5,5,9,6,6,9,5,5,6,1,7,10,2,2,7,3,3,10,0,2,6,2,8,10,0,6,9,5,5,8,0,6,11,6,6,8,0,4,6,3,7,7,3,7,9,0,5,9,0,9,9,1,2,12,0,0,5,0,1,13,2,7,7,3,3,7,7,7,8,6,7,7,4,4,7,0,5,6,2,6,11,5,7,9,3,8,8,1,2,13,0,1,14,5,8,8,});
sparse_graph4_ints[15][35] = vector<int>({7,7,7,0,0,0,0,0,12,0,1,17,0,8,8,0,0,15,2,2,2,8,9,9,0,10,11,2,4,4,4,5,5,0,1,1,4,5,10,0,0,9,2,3,15,1,1,2,0,4,4,1,3,9,0,0,1,6,7,11,5,6,6,0,0,7,0,0,3,0,6,11,0,2,12,0,0,5,0,7,14,4,4,4,2,11,11,0,0,10,5,9,9,1,1,15,4,4,12,2,2,12,2,8,8,3,3,3,0,0,8,3,4,4,4,5,13,0,0,11,5,5,5,0,9,10,0,9,13,6,6,10,6,6,7,0,0,13,3,4,9,3,3,12,0,0,16,4,4,5,0,4,11,0,7,12,4,8,9,0,4,8,0,3,16,2,3,9,5,10,10,0,0,14,1,7,7,8,8,8,5,5,6,0,3,8,0,4,13,1,3,3,4,4,9,6,6,6,2,2,14,3,8,11,0,2,13,0,2,14,0,0,6,1,8,11,0,0,17,6,7,7,3,3,13,0,6,7,4,5,9,0,2,8,0,2,10,2,9,12,0,7,7,5,5,11,0,8,9,5,6,12,3,3,9,2,7,12,5,5,10,1,1,11,3,8,8,2,10,10,7,7,8,0,7,11,1,6,13,0,6,6,0,5,14,2,9,9,0,6,10,3,4,14,0,11,12,7,8,10,});
sparse_graph4_ints[15][40] = vector<int>({5,5,5,0,0,12,1,1,1,10,10,10,0,0,8,0,0,0,0,0,18,0,12,14,7,7,7,0,9,9,3,3,5,0,8,16,4,5,13,0,4,18,0,0,14,0,0,16,0,0,10,1,1,3,8,8,8,3,3,11,1,3,15,0,0,20,1,3,3,4,9,12,0,0,1,1,10,10,3,3,3,0,0,2,4,12,12,9,9,9,0,0,4,0,8,8,0,3,12,3,5,5,5,5,15,0,0,6,5,5,10,8,8,12,5,6,6,1,2,10,6,6,11,0,12,12,2,4,4,6,10,10,0,8,12,1,2,14,4,4,4,7,11,11,3,3,15,3,4,12,0,0,9,0,0,11,6,6,7,4,5,5,5,9,9,4,8,14,0,4,4,0,10,14,0,10,12,0,6,17,1,1,19,1,2,9,0,6,14,2,4,10,0,5,13,6,6,6,7,7,11,0,7,15,0,2,16,5,6,13,0,6,9,0,0,13,0,0,15,4,4,10,5,5,6,0,2,14,2,3,9,4,10,13,1,1,17,7,7,8,0,0,17,5,5,11,2,8,13,4,10,10,1,8,10,0,4,16,1,13,13,6,7,7,0,2,12,1,7,9,4,8,10,1,11,11,7,7,13,4,4,15,1,9,15,0,0,19,2,9,9,1,7,13,0,2,18,1,11,13,});
sparse_graph4_ints[15][45] = vector<int>({9,9,9,0,1,22,5,5,17,4,4,5,11,11,11,0,0,11,0,0,1,0,13,13,0,8,8,8,8,14,7,7,8,0,8,12,0,0,3,0,0,5,0,10,13,3,3,4,1,2,16,0,0,7,6,6,7,0,5,20,0,0,13,6,13,13,0,0,16,0,15,15,0,0,9,3,3,12,4,4,14,7,11,11,0,10,16,7,7,11,0,0,21,0,0,0,0,4,4,0,1,1,5,5,15,1,1,2,0,9,9,1,2,3,5,5,5,0,7,16,6,7,16,1,2,11,0,0,19,5,5,11,2,2,12,3,4,4,10,10,10,0,6,18,4,10,10,4,11,15,0,11,17,0,2,10,5,6,6,0,1,14,9,10,13,5,9,14,0,7,7,1,10,10,5,6,13,0,0,10,4,11,12,2,3,16,0,3,14,0,6,6,0,2,20,0,14,14,0,7,19,0,12,12,8,8,12,3,3,14,0,2,18,0,6,11,0,5,10,8,8,8,3,3,3,5,8,13,0,11,14,0,6,13,5,6,9,0,8,17,6,6,11,0,4,19,6,6,6,0,3,21,0,9,18,0,0,17,0,0,14,2,8,12,6,10,10,4,12,13,0,9,15,0,13,16,0,12,15,7,7,7,0,0,8,0,7,14,0,1,17,0,9,10,0,5,17,5,5,6,});
sparse_graph4_ints[15][50] = vector<int>({0,0,10,8,8,9,0,0,0,8,8,17,4,4,21,1,3,19,0,0,25,12,12,13,1,9,17,10,10,11,2,2,16,5,15,15,1,1,1,7,7,7,0,9,13,0,6,6,0,9,10,0,0,17,1,1,3,3,3,3,5,11,15,0,0,15,0,2,22,1,14,14,2,2,23,6,6,19,0,6,8,10,10,15,6,6,6,0,0,12,0,0,19,7,8,14,0,8,8,3,13,17,9,9,10,0,0,8,1,9,15,4,4,15,0,0,1,0,3,4,2,2,12,6,6,12,0,16,16,0,0,2,6,6,14,4,4,19,0,0,4,11,11,12,6,12,12,0,0,6,5,5,5,1,9,20,0,1,21,3,11,11,9,9,14,6,6,16,0,13,17,2,3,21,1,2,15,5,5,10,4,14,14,3,4,4,0,7,17,0,12,14,4,10,18,9,13,14,0,12,16,1,1,11,3,4,14,3,3,13,1,4,17,1,10,10,8,12,13,0,0,14,4,4,5,5,8,17,4,11,13,1,1,19,0,5,5,5,5,7,7,11,16,0,6,19,6,6,10,7,8,12,1,6,11,1,3,11,2,10,11,4,5,17,0,12,19,0,9,19,1,3,13,0,6,22,6,6,7,0,1,10,0,0,23,0,0,16,0,11,13,0,11,18,6,10,10,7,7,9,});
sparse_graph4_ints[15][55] = vector<int>({0,3,18,8,15,16,0,0,8,2,5,22,0,9,9,6,6,11,8,9,16,0,0,13,2,11,16,3,14,19,6,7,21,0,0,3,9,11,11,0,0,0,0,0,5,0,12,14,5,5,6,1,16,16,11,11,15,13,14,14,0,0,15,0,5,21,7,7,7,0,7,24,1,1,8,0,3,24,9,9,18,9,9,10,0,0,19,0,13,19,0,1,16,7,13,13,4,4,5,1,7,13,8,8,14,0,15,15,0,11,22,0,10,10,6,6,13,0,4,19,11,12,12,0,11,11,5,6,18,0,12,12,0,3,4,0,0,1,0,8,20,0,0,26,0,2,23,1,9,21,2,3,15,6,7,19,1,3,25,0,7,12,0,1,1,7,13,17,0,17,18,6,7,15,2,3,4,0,1,21,8,8,8,0,3,9,0,9,16,1,1,2,0,6,11,0,14,14,0,8,15,0,6,9,0,1,27,4,17,17,4,13,17,12,13,13,0,10,17,1,1,4,0,4,14,5,10,12,5,9,17,0,0,11,5,5,22,4,11,20,1,2,13,0,11,19,8,12,15,6,6,7,3,4,17,5,12,12,2,4,16,0,2,12,4,4,19,4,5,5,5,5,20,5,12,15,5,5,16,0,0,23,9,14,14,5,15,16,10,10,14,1,5,5,0,6,19,0,3,21,});
sparse_graph4_ints[15][60] = vector<int>({0,0,0,0,0,2,0,0,4,0,0,15,8,8,12,0,2,17,0,0,8,0,0,25,10,10,16,4,4,26,7,7,7,4,4,17,1,16,16,12,14,15,1,6,7,11,11,11,5,10,21,3,11,17,8,8,9,9,17,17,4,4,4,0,9,21,1,2,21,4,5,19,0,14,20,0,12,16,0,0,11,4,4,12,2,2,28,0,15,22,4,4,14,5,17,17,0,1,12,10,10,10,3,19,19,1,9,9,0,0,22,2,3,13,11,11,17,6,6,24,6,7,10,7,8,15,0,0,20,1,11,13,0,17,18,6,6,14,0,5,18,0,6,20,8,8,22,8,14,14,6,6,6,7,13,18,0,12,23,0,11,11,6,11,18,0,18,20,1,3,3,9,9,19,5,7,22,0,1,16,0,7,10,0,1,28,7,12,13,6,6,19,0,4,17,5,13,21,12,12,12,5,5,6,3,12,12,9,9,15,0,0,30,3,4,22,1,2,7,2,8,17,9,11,20,13,15,16,1,8,15,0,0,1,3,13,13,3,5,23,4,9,20,1,1,1,6,15,19,1,8,24,10,14,18,13,13,13,2,2,24,1,10,18,0,1,26,0,0,6,0,14,16,6,13,15,0,2,3,0,3,13,0,14,18,0,10,22,2,3,26,9,15,15,1,2,23,1,4,4,});
sparse_graph4_ints[10][6] = vector<int>({0,2,2,0,0,0,0,0,1,0,0,2,0,0,3,1,1,1,0,1,1,1,1,2,0,1,2,});
sparse_graph4_ints[10][8] = vector<int>({0,0,0,2,2,2,0,0,4,0,0,1,1,1,1,0,0,2,0,2,2,1,1,3,0,0,3,0,1,1,0,2,3,0,1,3,1,1,2,0,1,2,1,2,2,});
sparse_graph4_ints[10][10] = vector<int>({1,3,3,0,0,0,0,0,2,0,0,5,0,2,2,0,0,1,1,1,1,0,0,4,0,2,4,1,1,3,2,2,2,0,0,3,1,2,2,2,2,3,0,3,3,0,1,3,0,1,1,0,1,4,1,1,4,1,1,2,0,1,2,0,2,3,1,2,3,});
sparse_graph4_ints[10][12] = vector<int>({0,0,0,0,4,4,0,0,4,0,0,6,0,0,1,2,2,2,1,1,1,0,0,2,3,3,3,1,1,5,0,2,2,0,0,5,1,2,2,1,3,3,0,2,4,2,2,4,0,0,3,0,1,5,0,1,4,0,3,3,2,2,3,0,3,4,1,1,3,0,1,3,2,3,3,0,1,1,0,2,5,1,1,4,1,3,4,1,2,4,1,1,2,0,1,2,0,2,3,1,2,3,});
sparse_graph4_ints[10][14] = vector<int>({0,0,0,3,3,3,0,0,4,0,0,6,0,0,1,2,2,2,1,1,1,0,2,6,0,0,2,0,0,7,0,4,4,1,2,2,0,0,5,2,2,3,0,2,2,0,4,5,1,1,5,2,4,4,0,1,5,2,2,4,2,2,5,3,3,4,0,3,4,2,3,3,0,2,4,0,3,5,0,0,3,0,1,6,0,1,4,0,3,3,1,3,5,1,3,3,1,1,3,0,1,3,2,3,4,1,1,6,0,1,1,1,4,4,0,2,5,1,2,5,1,1,4,1,2,4,1,3,4,1,1,2,0,1,2,0,2,3,1,2,3,});
sparse_graph4_ints[10][16] = vector<int>({4,4,4,0,0,0,0,0,4,0,0,8,0,4,4,0,0,6,0,0,1,2,2,6,2,2,2,1,1,1,0,0,2,1,5,5,1,2,2,3,3,3,2,2,3,1,1,7,0,0,7,0,2,6,0,0,5,0,2,2,1,1,5,0,4,5,2,4,4,0,1,5,3,3,5,0,4,6,2,3,3,1,2,5,0,1,7,0,3,6,2,2,5,3,3,4,0,1,6,0,5,5,0,3,4,3,4,4,0,2,4,2,2,4,2,3,5,0,0,3,1,4,4,0,1,4,0,3,3,2,4,5,1,3,5,1,3,3,1,3,6,1,1,3,2,3,4,0,1,3,1,2,6,0,3,5,1,1,6,0,2,7,0,1,1,0,2,5,1,4,5,1,1,4,1,3,4,1,2,4,1,1,2,0,1,2,0,2,3,1,2,3,});
sparse_graph4_ints[10][18] = vector<int>({3,5,5,0,0,4,0,0,0,0,0,9,2,2,3,0,0,2,0,0,6,0,5,5,0,0,1,1,1,1,0,2,6,1,2,2,0,2,8,3,3,3,2,2,2,0,6,6,0,0,7,0,4,7,0,0,8,2,3,3,1,1,7,3,3,5,0,0,5,2,2,6,4,4,4,0,2,2,0,4,5,3,3,6,3,3,4,1,1,5,1,5,5,0,1,5,4,4,5,2,2,7,0,4,4,0,3,6,3,4,4,0,1,7,0,1,8,0,4,6,1,2,7,2,5,5,2,4,6,1,2,5,2,2,5,0,1,6,2,4,4,0,5,6,2,3,6,0,3,4,2,3,5,2,2,4,0,2,4,3,4,5,2,4,5,0,0,3,2,3,4,0,1,4,0,3,3,1,3,7,1,5,6,1,3,5,1,3,3,1,3,6,1,4,4,1,1,3,1,4,6,0,3,7,0,1,3,1,2,6,1,1,8,1,4,5,0,3,5,0,2,7,1,1,6,0,2,5,0,1,1,1,3,4,1,1,4,1,2,4,1,1,2,0,1,2,0,2,3,1,2,3,});
sparse_graph4_ints[10][20] = vector<int>({0,0,4,0,0,10,0,0,0,0,5,5,2,2,3,5,5,5,0,0,2,0,0,8,3,3,7,0,0,6,0,0,1,1,1,1,4,4,4,1,2,2,2,6,6,3,3,3,1,1,9,1,1,7,2,2,2,0,4,8,2,3,3,0,0,7,0,2,6,0,2,8,0,0,9,0,6,6,3,4,4,2,2,6,0,0,5,0,4,7,3,5,5,0,6,7,0,4,5,4,4,6,3,3,4,2,4,6,0,2,2,1,5,5,1,2,8,1,1,5,0,1,9,2,3,6,1,2,7,2,3,7,4,4,5,0,1,8,0,1,5,0,3,6,2,5,5,2,2,8,0,4,4,0,1,7,1,5,7,4,5,5,3,3,5,3,4,6,0,4,6,0,5,7,1,2,5,2,2,7,3,5,6,0,5,6,3,3,6,2,2,5,0,1,6,2,4,4,2,5,6,0,3,4,2,4,7,2,3,5,3,4,5,2,2,4,0,2,4,2,4,5,1,6,6,2,3,4,1,5,6,0,0,3,0,1,4,0,3,3,1,3,7,1,3,8,1,3,5,1,4,7,1,3,6,1,3,3,1,4,6,1,4,4,0,3,8,1,1,3,0,3,7,1,1,8,0,2,9,1,2,6,0,1,3,1,4,5,0,2,7,0,3,5,1,1,6,0,2,5,});
sparse_graph4_ints[10][23] = vector<int>({0,0,7,0,0,4,0,0,0,0,6,6,0,3,10,5,6,6,0,1,1,0,0,10,2,2,2,3,4,8,0,0,1,2,3,3,0,5,5,0,0,3,4,5,5,0,0,5,0,0,8,0,1,11,2,7,7,3,3,7,0,2,8,4,4,4,3,3,3,0,0,6,1,3,3,0,0,9,0,6,8,0,4,8,3,4,4,1,1,9,0,4,7,1,1,7,0,0,11,2,3,9,0,2,6,5,5,5,4,4,5,1,6,6,2,2,6,1,2,9,1,2,2,4,5,7,3,4,7,0,4,5,3,3,6,2,6,7,0,1,10,2,3,8,0,7,8,3,3,4,0,7,7,0,3,8,4,6,6,0,1,9,2,6,6,1,1,10,4,4,7,2,2,9,0,2,2,4,4,6,1,5,5,1,1,5,3,3,8,5,5,6,1,5,7,4,5,6,2,3,6,2,3,7,1,2,7,0,1,8,3,5,5,0,3,6,0,1,5,2,5,5,0,5,9,0,5,8,3,5,7,3,4,6,2,2,8,0,4,9,0,1,7,3,6,6,0,4,4,0,5,7,3,3,5,2,5,8,3,5,6,3,6,7,2,2,7,0,4,6,2,4,8,2,5,7,0,5,6,1,7,7,2,4,6,1,2,5,0,6,7,2,2,5,0,1,6,2,4,4,});
sparse_graph4_ints[10][26] = vector<int>({6,6,6,1,3,11,3,3,3,1,1,1,0,0,0,1,1,9,0,0,8,0,0,12,0,6,6,0,0,6,5,5,5,0,0,1,1,2,2,0,6,8,0,0,2,1,3,3,0,0,4,0,6,10,3,7,7,3,4,4,1,3,9,4,4,9,0,0,9,2,3,3,0,5,5,3,5,5,0,8,8,0,0,10,0,2,12,0,0,7,0,0,13,2,2,2,4,4,4,0,2,8,1,7,7,4,5,5,0,0,11,1,1,11,4,6,6,3,3,9,2,3,7,2,2,10,2,8,8,1,1,7,4,4,8,3,3,8,5,7,7,0,1,11,3,3,10,2,2,7,5,5,8,0,6,7,0,2,6,1,2,9,0,8,9,5,5,7,3,3,7,0,6,9,3,4,8,6,6,7,5,6,6,0,5,6,1,2,10,2,6,9,1,2,11,2,2,6,0,1,12,2,3,10,2,6,6,4,7,7,1,4,9,0,0,5,0,2,10,4,5,7,2,6,8,4,6,8,0,4,5,3,5,7,3,4,9,0,1,10,2,7,7,0,5,8,0,7,7,0,4,8,4,5,8,3,4,7,3,6,6,3,3,4,5,5,6,2,7,8,0,1,9,0,7,9,0,3,8,4,4,6,5,6,7,0,2,2,4,6,7,1,1,5,3,4,6,0,7,8,});
sparse_graph4_ints[10][30] = vector<int>({0,0,8,3,3,4,0,2,12,2,8,8,0,0,0,0,0,4,0,0,2,0,0,6,5,5,10,4,6,6,0,0,15,0,0,10,7,7,7,0,3,9,0,6,12,4,4,5,3,9,9,0,0,1,1,3,3,1,1,1,1,1,3,0,6,10,1,3,13,0,0,12,3,3,8,3,3,3,0,6,6,1,7,7,2,3,3,3,4,4,0,8,11,0,0,9,1,1,9,2,3,10,5,5,5,0,10,10,6,6,7,0,2,14,1,1,11,4,4,10,0,0,14,4,5,5,0,8,8,5,6,6,4,4,4,2,3,8,0,0,7,0,0,13,1,7,9,4,8,8,2,2,12,1,1,13,2,2,2,0,7,7,3,3,11,6,6,6,0,2,8,3,5,11,3,4,9,0,0,11,1,9,9,6,8,8,5,5,9,0,6,7,1,7,10,3,3,12,6,6,9,3,4,8,0,4,7,2,2,10,4,5,8,0,3,6,6,6,8,1,2,12,0,1,11,2,8,10,4,5,9,0,6,9,2,3,11,0,6,11,7,7,8,3,4,10,6,7,7,4,7,8,0,1,13,1,2,13,0,1,14,0,5,9,2,3,12,0,9,9,0,3,10,5,8,8,0,2,6,4,5,10,5,7,9,0,4,6,2,3,7,1,3,11,4,4,8,3,7,7,});
sparse_graph4_ints[10][35] = vector<int>({0,4,10,7,8,8,6,6,7,0,8,8,0,1,17,0,0,0,0,0,7,0,0,10,3,3,3,0,1,1,1,10,10,1,1,2,0,0,14,5,5,5,0,4,12,1,2,9,4,4,10,2,11,11,3,4,4,0,0,16,2,2,2,8,9,9,0,0,1,3,8,12,0,0,3,4,4,13,0,5,15,6,6,10,0,0,5,1,4,13,4,4,5,0,7,7,0,4,4,0,0,9,0,0,12,2,4,4,0,9,10,5,6,6,4,4,4,2,6,12,0,0,11,0,6,11,0,0,8,0,2,15,0,3,10,3,7,8,6,7,11,0,3,16,3,4,9,7,7,7,3,8,9,0,0,13,0,7,14,0,10,12,5,5,6,4,9,9,5,5,10,0,0,15,0,4,11,6,6,6,4,4,9,3,4,8,6,7,7,0,3,8,1,1,14,4,5,10,0,2,12,4,5,5,0,7,11,5,9,10,0,8,9,0,0,17,0,2,13,2,3,14,0,6,7,2,2,13,5,5,12,0,6,6,0,6,13,4,8,9,3,8,8,5,5,11,2,7,13,1,2,15,1,7,7,8,8,8,1,8,12,1,3,3,7,7,8,3,10,10,1,2,7,0,9,11,3,8,11,0,0,6,7,8,10,6,6,11,3,4,13,3,4,12,3,3,8,1,7,11,});
sparse_graph4_ints[10][40] = vector<int>({0,8,8,4,5,10,0,0,4,0,9,12,0,0,2,0,0,0,0,0,18,4,10,13,10,10,10,0,0,12,1,5,5,1,3,3,5,5,15,0,0,14,1,3,13,0,0,20,0,0,10,7,7,11,3,3,3,3,3,17,4,4,14,0,0,8,3,5,5,5,5,5,8,8,12,1,12,12,0,8,16,1,2,12,5,6,6,0,0,1,8,8,8,9,9,9,1,1,1,0,0,6,0,6,14,1,2,2,7,11,11,1,9,13,0,6,12,0,1,16,5,9,9,2,8,8,2,4,4,1,2,10,0,10,10,0,1,10,4,4,4,3,3,10,5,6,13,5,6,10,0,10,15,4,5,5,0,0,9,2,2,10,4,8,8,0,6,16,4,4,12,7,7,7,0,0,15,0,12,14,6,6,7,2,10,10,4,9,11,0,2,17,0,3,9,0,3,15,3,4,10,1,1,19,0,6,9,4,12,12,6,6,11,0,4,16,5,5,6,0,0,13,1,13,13,6,6,6,7,7,8,1,2,13,4,11,11,1,11,11,0,0,16,0,4,4,0,4,18,3,7,12,4,4,10,6,7,13,3,9,9,1,9,9,2,3,14,1,7,15,0,7,12,0,11,13,1,8,8,4,4,13,5,10,10,3,8,14,0,0,19,1,5,15,8,9,9,1,3,17,});
sparse_graph4_ints[10][45] = vector<int>({0,0,15,8,8,8,2,2,13,0,1,1,2,6,17,0,0,0,1,2,21,5,5,11,0,6,8,9,9,13,0,15,15,0,0,7,1,2,17,1,1,2,0,0,10,5,6,6,3,3,3,3,8,13,3,4,4,5,9,14,11,11,11,0,2,10,0,12,14,9,9,9,6,6,7,0,0,17,5,11,11,2,2,18,0,0,22,5,6,17,0,8,12,0,0,19,0,2,12,0,8,8,0,3,3,0,0,1,0,1,20,4,4,5,10,10,10,0,0,3,7,8,15,0,0,5,2,3,10,8,12,12,7,7,8,1,3,4,3,4,12,1,9,12,2,11,12,0,13,16,6,6,11,0,9,10,0,14,14,0,6,6,0,6,16,2,4,4,5,13,13,6,8,11,5,5,5,3,9,9,2,8,8,6,6,15,1,11,11,0,11,17,1,3,15,0,0,11,5,6,14,0,0,13,5,12,12,7,8,12,3,3,15,5,6,9,0,5,20,0,1,15,4,9,13,4,9,16,2,10,15,3,4,11,4,10,10,4,5,13,0,8,16,2,7,18,0,7,10,0,1,17,5,11,14,0,4,12,3,3,18,3,5,15,0,0,9,7,7,7,5,9,9,8,9,12,1,5,14,0,9,14,2,2,14,3,4,19,6,10,10,1,9,15,1,13,13,0,9,17,});
sparse_graph4_ints[10][50] = vector<int>({1,1,3,0,0,1,0,12,14,7,7,11,0,16,16,6,6,6,3,3,19,3,3,3,12,12,12,0,0,21,2,2,23,10,10,11,0,4,12,1,1,1,0,10,13,7,7,18,0,1,12,0,13,17,8,8,12,0,0,25,0,0,11,10,10,15,5,5,20,8,14,14,0,7,17,6,7,7,5,11,11,1,11,16,3,3,12,0,0,8,6,8,8,3,4,8,8,8,15,0,3,4,0,8,18,4,4,5,0,3,20,4,4,12,0,0,16,1,9,15,0,0,23,0,4,16,0,0,0,0,2,16,5,6,17,0,2,8,0,4,18,6,13,14,0,14,14,3,5,9,6,9,14,8,8,8,9,9,9,3,9,19,0,1,1,4,8,15,4,11,17,5,6,13,0,10,10,1,2,22,0,0,4,4,15,15,1,10,18,4,4,15,1,2,14,5,6,10,2,2,2,0,14,18,0,6,22,0,8,8,0,6,6,7,11,12,10,10,13,1,7,11,4,5,13,3,4,4,0,7,7,2,11,12,7,11,16,0,1,19,2,3,14,0,7,14,5,5,16,5,8,17,5,7,11,4,10,10,0,8,20,3,14,14,9,10,10,0,11,11,3,5,19,4,11,15,1,1,13,0,5,21,5,8,13,0,0,14,0,1,11,8,12,12,0,9,9,4,12,13,});
sparse_graph4_ints[10][55] = vector<int>({1,1,2,0,0,1,0,4,4,4,11,12,3,3,17,5,5,5,1,3,24,0,0,16,8,8,8,0,0,19,13,14,14,1,1,11,10,11,17,0,0,22,10,10,10,4,12,16,6,6,21,7,14,15,1,9,20,0,0,25,4,4,18,1,1,4,4,4,4,4,4,23,8,8,15,6,6,17,4,13,13,5,6,6,7,10,19,11,11,11,0,1,10,6,6,7,0,5,5,7,7,7,12,12,12,1,9,10,0,4,21,1,1,16,2,2,16,0,0,9,8,15,16,2,2,13,5,7,11,3,3,13,6,6,14,8,11,15,0,7,15,0,2,19,0,8,10,3,15,16,1,15,15,0,1,26,0,14,14,8,9,9,2,9,16,4,4,21,1,8,19,4,10,11,4,12,18,0,11,11,0,12,21,0,6,10,4,4,13,0,5,25,4,17,17,0,17,18,7,13,13,1,2,12,0,1,23,11,13,14,4,10,16,7,8,18,3,10,19,9,9,17,0,6,21,2,13,20,6,8,14,9,9,10,6,13,18,1,9,22,0,5,19,0,6,8,0,0,0,0,13,18,0,0,14,0,0,3,0,1,1,2,8,15,0,12,12,0,3,23,9,10,13,0,12,17,7,10,12,0,0,5,5,5,14,0,13,13,5,9,20,2,2,3,2,15,19,0,14,19,});
sparse_graph4_ints[10][60] = vector<int>({0,5,21,0,2,26,5,11,13,0,0,30,6,8,8,0,13,23,0,0,9,0,0,15,0,0,21,0,0,2,2,2,2,12,12,12,0,0,12,0,0,18,3,19,19,4,4,26,0,10,15,10,10,20,15,15,15,7,7,20,5,7,7,0,0,0,9,9,15,5,14,19,8,9,10,5,5,5,0,2,2,0,0,25,5,6,18,2,2,28,0,0,23,0,10,20,5,6,6,13,13,17,4,4,24,12,13,14,6,6,24,8,8,21,3,4,17,0,6,12,7,8,9,0,3,11,5,10,16,3,5,5,9,17,17,2,3,11,0,2,18,1,13,13,9,9,17,3,11,19,0,11,17,0,10,11,0,2,15,7,12,13,8,15,15,2,2,18,0,3,8,3,8,21,4,16,16,2,2,15,6,7,13,4,9,22,0,4,4,0,12,12,8,13,14,5,5,17,4,16,20,5,17,17,5,6,12,4,4,22,0,13,21,10,11,16,3,14,18,0,10,23,7,7,14,6,11,19,2,14,16,0,5,26,0,1,28,0,7,13,6,6,19,6,12,21,0,10,25,11,11,11,0,8,14,0,17,17,2,13,15,3,4,21,14,14,14,3,3,15,8,8,14,10,13,18,0,3,24,1,15,22,0,3,21,0,5,24,2,4,4,1,17,19,6,12,16,0,8,18,});
sparse_graph4_ints[5][6] = vector<int>({0,2,2,0,0,0,0,0,1,0,0,2,0,0,3,1,1,1,0,1,1,1,1,2,0,1,2,});
sparse_graph4_ints[5][8] = vector<int>({0,0,0,0,0,4,2,2,2,0,0,1,1,1,1,0,0,2,0,2,2,1,1,3,0,0,3,0,1,3,1,2,2,0,1,1,1,1,2,0,1,2,0,2,3,});
sparse_graph4_ints[5][10] = vector<int>({0,0,0,1,3,3,0,2,2,0,0,1,0,0,5,0,0,2,1,1,1,0,0,4,0,2,4,1,1,3,2,2,2,2,2,3,0,0,3,1,2,2,0,3,3,0,1,4,0,1,3,1,1,4,0,1,1,1,1,2,0,1,2,1,2,3,0,2,3,});
sparse_graph4_ints[5][12] = vector<int>({0,0,0,0,0,6,0,4,4,0,0,4,0,0,1,2,2,2,1,1,1,0,0,2,3,3,3,1,1,5,1,2,2,0,0,5,0,2,2,2,2,3,0,2,4,2,2,4,0,1,5,0,3,4,2,3,3,0,1,4,0,3,3,1,3,3,0,0,3,1,1,3,0,1,3,1,3,4,0,2,5,1,1,4,1,2,4,1,2,3,0,1,1,1,1,2,0,2,3,0,1,2,});
sparse_graph4_ints[5][14] = vector<int>({0,0,4,0,4,5,0,0,1,0,0,6,0,0,0,2,2,3,1,1,1,0,0,2,1,2,2,0,2,6,0,0,7,2,2,2,3,3,3,0,0,5,2,3,3,0,1,5,0,2,2,2,4,4,1,1,5,0,4,4,1,2,5,3,3,4,2,2,5,0,1,6,2,2,4,0,3,4,0,2,4,2,3,4,0,1,4,1,3,5,0,3,3,1,3,3,0,0,3,1,1,3,1,4,4,1,3,4,0,1,3,1,1,6,0,3,5,0,2,5,1,2,4,1,1,4,1,2,3,0,1,1,1,1,2,0,2,3,0,1,2,});
sparse_graph4_ints[5][16] = vector<int>({2,2,2,4,4,4,0,0,0,0,0,4,0,0,2,0,4,4,0,0,8,2,2,6,0,0,6,0,0,1,1,1,1,1,5,5,1,2,2,3,3,3,2,2,3,0,0,7,1,1,7,0,0,5,2,4,4,0,4,5,0,1,7,3,3,5,1,1,5,1,2,5,0,1,5,0,2,2,1,2,6,0,2,6,3,4,4,0,4,6,3,3,4,2,3,3,0,1,6,0,5,5,2,3,5,2,2,5,2,2,4,0,3,4,2,4,5,2,3,4,0,2,4,1,3,5,0,1,4,0,3,3,1,3,6,1,3,3,1,4,4,0,0,3,1,4,5,1,1,3,1,3,4,0,1,3,0,3,6,1,1,6,0,2,7,0,3,5,0,2,5,1,2,4,1,1,4,1,2,3,0,1,1,1,1,2,0,2,3,0,1,2,});
sparse_graph4_ints[5][18] = vector<int>({0,0,7,2,2,2,3,5,5,0,0,0,0,0,9,0,4,4,0,0,4,0,0,2,2,2,7,0,0,1,1,1,1,0,5,5,1,2,2,0,0,6,3,3,3,0,6,6,2,2,3,3,4,4,0,0,8,1,1,7,0,2,8,2,2,6,4,4,4,0,1,7,0,0,5,0,4,5,3,3,4,2,4,6,4,4,5,1,5,5,1,2,5,3,3,5,1,2,7,0,1,8,1,1,5,0,1,5,0,3,6,2,3,6,0,2,2,2,5,5,3,3,6,0,2,6,2,4,4,0,4,6,2,3,3,0,1,6,0,4,7,3,4,5,2,2,5,0,5,6,2,3,5,2,4,5,2,2,4,2,3,4,0,3,4,1,5,6,0,2,4,1,3,7,1,3,5,0,1,4,1,3,6,0,3,3,1,4,6,1,3,3,1,4,4,1,4,5,0,0,3,1,1,3,1,3,4,0,1,3,1,2,6,0,3,7,1,1,8,0,2,7,1,1,6,0,3,5,0,2,5,1,2,4,1,1,4,1,2,3,0,1,1,1,1,2,0,2,3,0,1,2,});
sparse_graph4_ints[5][20] = vector<int>({3,3,7,2,2,2,0,0,4,0,0,0,0,0,2,0,0,10,0,5,5,0,4,4,0,0,7,5,5,5,0,0,1,1,1,1,4,4,4,0,6,6,1,1,7,1,2,2,0,0,6,3,3,3,2,2,3,2,3,6,1,1,9,0,0,8,0,2,8,2,2,8,0,0,9,3,5,5,4,4,6,2,2,6,0,6,7,1,5,5,0,1,7,0,4,5,2,6,6,2,3,7,1,2,8,0,0,5,3,3,4,0,1,9,4,5,5,3,4,6,1,2,7,3,4,4,0,1,8,2,4,6,2,2,7,4,4,5,2,5,5,1,2,5,0,3,6,1,1,5,3,3,5,0,1,5,3,5,6,0,2,2,1,5,7,3,3,6,2,5,6,0,2,6,0,4,8,0,5,7,3,4,5,2,4,7,0,5,6,0,4,6,2,4,4,2,3,3,0,4,7,0,1,6,2,2,5,2,3,5,2,4,5,1,6,6,1,5,6,2,2,4,2,3,4,0,3,4,0,2,4,1,3,7,1,3,8,1,4,7,1,3,5,1,3,6,0,1,4,1,4,6,0,3,3,1,3,3,1,4,4,1,4,5,0,0,3,1,1,3,1,3,4,0,1,3,0,3,8,1,1,8,0,2,9,0,3,7,1,2,6,0,2,7,1,1,6,0,3,5,});
sparse_graph4_ints[5][23] = vector<int>({2,2,2,0,0,3,0,0,0,0,6,6,0,0,11,2,2,6,5,6,6,3,4,8,0,0,1,0,0,5,0,1,1,0,2,2,0,3,10,0,0,7,0,0,9,4,5,5,3,3,3,0,3,8,2,7,7,4,4,4,0,4,4,0,0,8,1,3,3,0,0,6,1,5,5,0,1,9,0,1,11,2,3,7,3,5,5,5,5,5,2,3,9,2,3,8,1,1,7,1,2,9,0,0,10,0,6,7,3,4,7,5,5,6,0,1,10,2,6,7,0,5,5,3,3,7,0,7,7,1,1,9,1,1,10,2,2,8,0,7,8,0,4,5,2,2,9,3,3,8,4,5,7,4,6,6,0,1,7,2,3,6,4,4,7,4,4,6,2,6,6,4,5,6,0,2,8,1,2,2,3,3,4,0,6,8,3,4,6,1,3,7,3,3,6,3,5,7,1,2,7,0,1,8,3,4,4,4,4,5,3,6,6,1,2,6,1,1,5,3,6,7,3,5,6,2,5,5,1,2,5,3,3,5,1,7,7,0,1,5,0,0,4,2,5,8,1,5,7,0,5,8,0,5,9,2,5,7,2,5,6,2,4,8,2,4,6,0,5,7,0,4,8,0,4,9,0,2,6,1,5,8,2,4,7,3,4,5,0,5,6,1,6,8,0,4,6,2,4,4,});
sparse_graph4_ints[5][26] = vector<int>({5,7,7,3,3,3,0,0,10,2,2,2,0,0,2,0,0,0,0,0,4,0,0,8,0,6,6,0,0,13,1,3,11,0,6,8,0,6,10,3,4,4,0,0,1,1,1,1,0,2,10,1,2,2,0,8,8,4,5,5,0,3,8,2,3,3,4,4,9,0,0,7,4,6,6,3,3,4,1,1,7,4,4,4,3,4,8,0,0,11,4,4,5,0,2,12,1,7,7,5,5,5,1,3,3,0,0,6,0,0,12,1,1,11,2,2,10,0,4,8,5,6,6,0,3,7,4,6,8,0,5,8,6,6,6,4,4,8,3,3,10,1,2,10,1,6,6,2,8,8,1,4,9,3,7,7,6,6,7,3,3,9,0,0,9,0,5,6,0,6,9,0,8,9,1,2,11,0,1,12,5,5,7,2,3,10,3,4,7,0,1,9,0,2,6,4,5,8,0,1,11,3,4,9,4,7,7,3,3,7,0,1,10,1,3,9,4,5,7,5,5,8,1,2,9,4,4,7,1,1,9,5,6,7,3,3,8,5,5,6,2,6,8,2,7,7,2,6,9,2,6,6,4,6,7,0,4,5,2,7,8,3,3,6,3,6,6,0,7,7,4,4,6,0,7,9,0,5,5,3,7,8,0,0,5,1,3,7,3,5,7,0,2,8,0,7,8,3,5,9,});
sparse_graph4_ints[5][30] = vector<int>({5,5,10,3,3,12,2,2,8,4,5,5,2,2,2,0,0,8,0,0,2,0,0,0,0,0,4,0,0,10,3,3,3,1,6,9,3,4,4,6,6,6,6,8,8,1,2,12,0,0,13,0,0,15,0,0,1,0,6,7,1,1,1,2,2,10,1,1,3,3,9,9,1,7,7,2,3,3,3,8,8,0,0,7,0,1,11,3,7,7,0,6,11,4,5,9,1,2,7,0,8,11,0,0,9,4,6,6,0,10,10,0,2,14,5,5,5,1,2,8,0,6,6,0,0,11,6,7,7,4,4,5,4,4,4,3,4,8,2,3,11,1,7,10,0,3,10,0,0,14,0,8,9,0,7,7,1,3,13,2,4,4,6,6,7,3,5,11,7,7,7,3,4,10,1,3,3,0,2,12,0,0,6,0,8,8,5,5,9,1,1,13,5,7,9,2,4,8,2,2,12,4,4,8,7,7,8,4,4,10,4,8,8,0,6,9,2,3,12,2,8,8,2,3,10,1,2,13,0,0,12,0,6,12,0,1,14,0,3,6,6,6,8,4,5,8,3,3,11,4,5,10,5,6,9,0,1,13,3,4,11,5,8,8,0,1,12,2,8,10,2,3,8,2,4,10,6,6,9,0,9,9,1,3,11,5,5,8,3,5,9,1,2,11,1,1,11,5,6,6,});
sparse_graph4_ints[5][35] = vector<int>({8,9,9,6,8,8,5,5,10,0,0,15,3,3,9,0,1,17,0,6,7,0,9,12,0,0,3,0,1,6,0,0,1,1,3,7,4,5,13,6,6,6,0,3,14,4,4,9,4,6,6,0,5,14,0,0,0,2,2,2,0,10,10,0,5,5,0,0,5,0,1,1,1,1,2,0,0,11,2,8,8,0,7,14,1,2,10,0,1,9,3,3,3,6,7,7,0,4,4,0,0,9,0,0,8,1,2,12,3,9,10,4,7,12,0,8,8,0,0,13,0,7,11,4,10,10,1,2,9,1,7,7,0,3,16,5,5,5,0,6,6,2,3,9,8,8,8,0,0,12,1,10,12,3,3,12,5,6,6,0,1,16,1,6,13,4,5,9,1,4,15,3,4,9,4,5,12,6,6,7,0,0,14,0,7,7,1,2,15,0,1,15,6,7,10,0,11,11,0,5,12,4,5,10,4,8,9,5,6,11,7,7,7,0,3,13,2,7,11,0,7,10,5,8,11,3,8,8,4,4,6,0,2,12,1,8,13,4,4,5,7,8,8,3,8,9,4,4,4,5,6,10,0,9,10,0,7,12,8,8,9,0,0,16,4,4,12,0,0,17,1,8,8,6,9,10,3,3,13,7,7,10,6,6,10,0,5,11,2,2,14,4,4,13,5,9,9,1,1,15,});
sparse_graph4_ints[5][40] = vector<int>({7,7,7,0,4,14,0,0,14,0,4,18,0,2,2,0,1,6,10,10,10,0,0,0,0,8,16,0,0,20,0,0,2,6,6,11,3,4,7,0,10,10,5,5,7,0,0,17,8,8,12,4,4,4,4,4,12,0,4,16,8,9,9,4,12,12,4,4,15,0,8,8,2,8,8,0,1,15,1,4,11,2,2,2,6,10,11,3,8,14,8,8,8,1,3,9,0,0,1,4,8,10,3,10,13,6,8,11,4,5,13,0,5,6,3,3,4,0,1,2,0,0,5,4,4,10,0,6,16,0,10,12,0,8,13,0,1,10,0,7,10,3,3,3,5,6,13,0,4,4,5,6,9,0,3,11,1,1,19,0,6,14,0,2,13,3,11,11,0,0,10,0,0,9,1,9,9,1,7,7,3,5,5,2,2,13,0,0,19,5,5,15,0,12,12,5,7,7,7,11,11,0,2,18,0,0,18,6,6,14,3,9,9,1,5,17,0,3,9,1,3,17,0,2,16,3,3,9,1,5,15,7,7,13,5,5,11,5,5,6,1,11,11,2,8,13,1,5,13,0,0,8,6,6,6,1,1,17,0,0,12,0,0,13,4,10,10,1,3,15,1,7,11,0,10,14,5,9,9,0,6,12,0,12,14,1,13,13,7,7,12,8,10,10,5,5,5,7,7,8,});
sparse_graph4_ints[5][45] = vector<int>({0,9,16,6,13,13,0,1,18,1,3,10,0,0,5,5,5,6,7,7,8,0,1,22,0,0,1,0,12,12,8,8,14,0,10,10,0,2,6,4,5,18,0,0,3,0,5,20,10,10,10,6,6,6,3,12,15,0,1,11,0,0,17,6,7,14,5,11,12,0,0,21,7,7,10,4,5,15,8,9,9,4,4,4,6,7,16,0,11,11,0,6,18,1,2,8,0,5,5,1,2,18,0,1,15,9,11,12,4,6,13,4,5,11,2,10,14,0,5,8,0,2,13,3,10,15,9,9,10,3,13,13,0,0,0,0,0,10,0,8,14,4,4,17,0,7,16,6,7,7,0,9,9,2,5,8,0,1,1,5,9,9,1,1,2,1,3,3,0,7,12,6,12,12,4,11,11,2,6,13,3,8,17,4,8,9,0,4,19,7,7,12,4,5,5,1,1,4,3,7,7,2,12,13,0,3,21,1,4,17,1,9,14,0,2,20,0,5,13,3,14,14,4,9,12,6,11,14,0,4,12,7,8,13,0,0,9,9,9,13,4,6,16,8,9,12,5,5,9,5,6,14,0,0,14,3,3,14,0,5,16,0,3,14,0,13,16,3,3,3,0,8,18,0,14,14,0,11,16,3,4,19,0,0,20,1,1,16,1,2,17,0,6,11,0,0,13,5,5,12,});
sparse_graph4_ints[5][50] = vector<int>({10,10,14,4,10,18,1,2,18,0,1,3,9,9,9,2,8,8,0,0,0,0,1,24,0,2,12,2,3,3,3,4,13,6,7,15,0,0,18,0,0,2,4,13,13,10,10,11,12,12,13,2,2,2,4,5,17,7,7,11,5,14,14,5,6,19,0,0,12,0,5,21,1,1,15,7,7,9,0,8,20,7,7,7,0,0,23,4,4,4,8,8,14,1,3,23,8,9,16,5,5,12,2,2,12,0,0,20,0,0,15,0,1,21,4,10,10,2,6,6,4,4,21,4,12,12,1,3,9,0,9,18,0,6,19,0,3,8,5,5,7,4,11,16,0,8,8,5,15,15,3,3,9,9,9,11,5,9,13,2,2,19,3,4,17,1,16,16,0,5,5,5,6,13,5,13,16,7,7,17,3,8,9,0,12,15,0,9,12,0,0,7,11,11,11,2,2,15,7,11,14,1,2,21,1,1,7,0,10,13,2,6,11,0,10,20,8,12,15,2,6,15,1,4,13,6,6,14,2,11,12,4,11,14,0,10,16,6,10,10,8,13,13,0,12,19,4,8,17,8,14,14,0,6,22,0,13,13,0,0,1,0,11,11,2,8,15,5,5,18,1,1,1,3,3,16,0,6,15,0,14,17,0,13,16,0,14,14,7,12,12,0,0,10,0,15,15,1,1,3,});
sparse_graph4_ints[5][55] = vector<int>({9,9,17,5,5,21,7,7,12,13,14,14,0,5,5,0,0,12,4,9,10,0,0,24,1,1,2,0,0,0,0,0,17,4,17,17,4,5,11,5,14,14,3,5,17,9,10,10,0,5,25,5,6,18,10,13,13,0,1,1,5,12,19,0,0,15,1,1,21,2,3,3,0,8,10,1,3,18,4,12,12,0,8,8,0,8,19,1,2,11,1,1,26,1,4,22,2,14,14,0,1,25,5,5,5,11,11,12,0,11,15,0,3,17,7,8,20,8,13,17,0,15,20,9,9,15,7,7,14,11,11,15,5,10,10,2,3,14,9,9,9,6,6,12,0,0,19,1,8,13,6,9,17,5,6,22,10,15,15,2,3,12,1,10,10,1,4,19,0,8,21,0,0,21,6,6,7,5,11,18,8,11,13,1,12,18,0,5,8,7,7,8,2,5,16,1,12,21,1,5,15,0,2,9,3,3,20,7,10,13,0,12,12,0,18,18,3,3,23,4,16,16,2,9,17,1,15,16,4,14,17,0,1,8,4,4,4,5,6,16,2,9,19,10,10,11,4,15,18,9,10,18,0,16,18,5,6,6,8,12,15,1,13,19,0,8,23,4,10,15,8,8,15,6,8,19,0,3,22,0,9,15,0,0,7,3,5,20,2,10,20,0,3,3,11,12,16,10,14,14,});
sparse_graph4_ints[5][60] = vector<int>({10,11,16,8,8,18,0,16,22,15,15,15,7,7,23,1,16,16,4,5,22,0,0,13,0,0,10,0,0,22,0,0,4,0,0,0,6,6,6,9,9,9,11,11,11,3,3,15,1,12,12,10,16,16,1,3,26,0,2,6,4,4,17,6,6,18,0,0,30,0,0,28,0,14,20,1,6,21,0,0,2,3,3,27,7,7,7,0,11,11,8,14,19,2,2,2,1,4,12,0,18,19,13,13,17,0,14,16,12,12,12,0,3,24,13,13,13,0,3,22,7,12,12,0,12,24,1,8,18,0,0,16,4,6,25,10,10,20,4,6,6,5,6,13,6,18,18,1,3,11,9,15,15,6,15,15,0,7,7,8,8,16,4,5,11,14,14,14,7,7,9,8,8,14,0,0,26,0,2,18,8,9,20,5,17,17,1,10,23,4,16,20,1,9,9,12,12,16,1,2,28,0,0,18,0,7,16,0,0,24,3,11,13,1,3,15,6,14,18,4,14,14,1,2,9,6,7,20,3,3,23,3,4,25,5,13,21,7,7,12,0,11,21,5,12,19,5,11,17,0,6,20,1,12,18,6,8,21,0,4,18,0,12,16,8,10,11,10,10,14,0,10,16,7,7,14,0,20,20,2,2,20,3,4,4,7,12,16,3,13,13,0,17,18,1,9,19,0,15,21,});

sparse_graph4_ints[40][100] = vector<int>({0,0,0,0,0,22,0,0,50,25,25,25,0,22,22,0,0,34,11,11,39,2,3,3,11,11,11,0,32,34,0,8,38,6,7,7,0,26,26,0,0,16,2,4,26,0,0,42,21,21,21,17,17,17,5,5,45,7,7,25,0,14,14,5,5,5,0,0,26,0,16,36,0,0,4,0,24,38,14,14,15,0,0,12,0,6,34,10,10,28,0,18,18,18,18,32,0,30,30,0,6,20,15,15,29,0,0,30,0,0,38,0,0,46,8,8,8,13,29,29,19,19,19,0,4,16,0,0,8,0,16,42,13,13,13,0,14,28,4,4,4,2,4,42,0,22,34,3,3,31,0,4,48,0,18,30,9,9,33,6,6,22,0,10,34,9,10,10,13,15,36,0,28,28,0,24,24,0,20,20,0,0,18,16,16,16,13,13,27,0,16,16,0,6,22,0,12,12,5,7,39,6,6,6,2,2,2,0,0,2,12,12,12,0,4,38,8,8,42,12,12,32,11,25,25,9,9,25,0,0,14,9,9,9,3,3,17,0,0,10,0,8,8,3,4,4,18,18,19,0,20,40,24,25,25,21,22,22,16,16,34,10,30,30,0,10,10,0,26,37,1,33,33,0,0,20,0,0,24,3,3,29,0,0,28,0,0,32,0,0,36,1,7,23,0,10,26,14,15,15,});
sparse_graph4_ints[38][100] = vector<int>({25,25,25,15,15,15,0,0,38,0,0,10,10,10,40,9,9,10,0,0,0,0,0,22,1,33,33,0,0,46,0,26,26,0,0,29,6,6,36,6,7,7,12,12,12,5,7,29,16,16,29,0,0,6,3,3,47,0,0,17,18,18,18,0,24,38,5,5,5,0,20,20,2,4,28,1,5,21,0,16,30,0,0,42,0,30,30,0,0,34,0,16,38,0,16,16,22,22,22,0,0,50,0,0,2,1,23,23,0,0,14,9,11,32,16,28,28,8,8,8,2,3,4,0,0,26,7,7,21,14,14,36,0,8,42,0,5,35,0,23,34,0,12,44,12,12,25,20,20,20,0,4,16,4,4,4,13,13,14,0,5,39,11,26,26,0,12,26,0,20,32,0,2,2,0,6,18,0,18,18,13,15,27,20,20,30,10,11,23,1,4,25,10,11,11,0,18,41,16,16,17,0,28,28,0,12,14,10,22,34,0,10,34,1,5,31,0,0,32,0,0,20,12,12,34,0,8,10,11,23,23,6,6,18,0,22,22,0,0,12,0,16,26,0,0,8,7,8,38,0,28,36,9,30,30,1,3,43,0,10,12,8,8,30,0,0,24,0,14,36,9,9,21,8,22,22,2,2,2,0,10,22,17,17,33,0,0,36,0,20,28,0,0,40,0,14,15,6,6,44,});
sparse_graph4_ints[36][100] = vector<int>({0,0,12,0,0,0,1,9,45,17,17,17,7,7,9,0,18,18,0,0,34,25,25,25,1,33,33,0,0,44,0,16,34,0,0,27,0,2,4,0,28,28,0,0,18,6,7,28,0,22,39,20,20,21,0,22,22,0,0,50,4,6,20,0,0,22,1,7,7,2,5,37,13,14,14,19,19,31,14,14,28,0,0,38,3,3,3,11,11,11,0,25,25,11,11,39,0,0,2,9,9,9,10,10,26,11,26,26,0,0,30,0,8,10,6,6,40,0,12,25,0,18,37,0,6,18,0,0,4,0,6,26,15,15,16,14,16,35,2,4,30,0,0,16,0,28,36,0,20,20,0,22,32,17,27,27,0,6,34,1,5,15,0,0,41,12,12,27,0,12,32,0,16,16,12,12,13,0,0,47,22,22,23,18,19,19,0,0,10,3,5,5,0,10,38,0,0,24,0,30,30,0,18,28,0,14,14,0,0,14,9,10,34,6,7,21,1,5,21,0,4,48,10,24,24,0,0,8,17,17,28,0,17,41,10,24,33,0,4,42,0,16,26,12,12,34,0,25,34,8,8,32,0,0,20,8,20,20,6,7,7,10,30,30,3,3,5,9,9,21,9,22,22,9,10,11,0,6,44,8,8,42,0,12,13,11,11,21,0,20,30,0,0,32,0,0,36,8,20,32,});
sparse_graph4_ints[34][100] = vector<int>({0,0,0,25,25,25,13,14,14,2,3,4,0,14,16,19,19,19,7,7,8,6,6,44,0,0,30,16,16,34,0,24,24,0,0,16,0,0,46,0,32,34,0,1,39,0,0,24,5,7,37,0,28,29,0,12,26,0,0,5,13,13,31,6,7,30,11,12,12,0,0,12,0,0,20,0,0,34,12,26,26,6,6,6,13,14,22,1,6,20,0,1,49,0,26,37,0,20,21,0,22,32,3,3,27,0,1,2,10,10,38,21,21,21,1,12,12,10,10,30,3,6,6,1,5,33,20,20,30,0,17,28,0,14,39,15,15,15,0,0,43,0,1,6,8,22,22,0,16,18,9,21,31,9,9,10,0,6,24,9,30,30,3,3,17,1,9,35,0,16,42,10,24,24,0,0,26,3,4,4,0,0,14,0,0,37,14,14,26,0,9,42,0,20,30,10,10,22,6,7,22,1,21,39,0,6,30,0,1,17,0,18,20,17,17,27,10,10,12,17,17,18,1,4,40,3,3,47,17,27,28,2,30,30,0,10,10,5,20,20,9,10,34,13,13,37,22,23,23,4,4,5,1,19,36,0,0,22,0,26,27,9,24,33,0,0,28,10,10,26,0,4,44,0,15,33,10,10,18,0,0,10,0,0,1,0,6,36,0,11,21,0,0,32,0,14,27,0,25,34,});
sparse_graph4_ints[32][100] = vector<int>({10,30,30,15,16,16,21,21,22,10,11,31,14,14,36,0,0,26,3,3,47,0,0,3,1,6,37,9,11,11,0,0,20,0,0,36,12,13,14,3,4,4,3,7,7,0,23,34,1,10,29,8,8,8,25,25,25,0,0,46,1,3,3,16,16,26,0,0,14,0,18,41,0,0,32,0,19,29,0,0,0,0,20,21,0,0,40,5,8,40,17,18,18,5,25,35,20,20,30,4,4,20,10,11,23,9,27,27,0,28,28,11,11,35,1,5,25,11,12,12,0,0,23,6,7,7,6,8,34,2,3,30,1,17,19,1,22,22,0,32,34,0,0,5,9,9,9,0,0,9,0,0,43,17,17,29,0,0,16,1,11,44,0,22,31,0,14,31,9,23,24,5,22,22,1,16,16,0,7,15,0,16,35,0,1,48,14,14,15,0,0,28,0,0,18,2,8,24,3,4,18,0,30,31,0,5,33,1,4,41,0,6,44,5,5,5,0,0,12,8,22,30,14,14,22,0,14,15,0,23,38,17,27,28,10,11,39,0,26,26,10,10,27,1,17,37,19,19,19,2,9,26,23,23,23,1,9,39,2,4,35,12,24,32,7,11,22,15,23,23,4,4,5,0,17,26,0,0,30,9,17,30,13,14,26,16,24,26,17,17,33,0,0,1,6,7,30,6,7,19,});
sparse_graph4_ints[30][100] = vector<int>({10,30,30,15,16,16,21,21,22,10,11,31,14,14,36,0,0,26,3,3,47,0,0,3,1,6,37,9,11,11,0,0,20,0,0,36,12,13,14,3,4,4,3,7,7,0,23,34,1,10,29,8,8,8,25,25,25,0,0,46,1,3,3,16,16,26,0,0,14,0,18,41,0,0,32,0,19,29,0,0,0,0,20,21,0,0,40,5,8,40,17,18,18,5,25,35,20,20,30,4,4,20,10,11,23,9,27,27,0,28,28,11,11,35,1,5,25,11,12,12,0,0,23,6,7,7,6,8,34,2,3,30,1,17,19,1,22,22,0,32,34,0,0,5,9,9,9,0,0,9,0,0,43,17,17,29,0,0,16,1,11,44,0,22,31,0,14,31,9,23,24,5,22,22,1,16,16,0,7,15,0,16,35,0,1,48,14,14,15,0,0,28,0,0,18,2,8,24,3,4,18,0,30,31,0,5,33,1,4,41,0,6,44,5,5,5,0,0,12,8,22,30,14,14,22,0,14,15,0,23,38,17,27,28,10,11,39,0,26,26,10,10,27,1,17,37,19,19,19,2,9,26,23,23,23,1,9,39,2,4,35,12,24,32,7,11,22,15,23,23,4,4,5,0,17,26,0,0,30,9,17,30,13,14,26,16,24,26,17,17,33,0,0,1,6,7,30,6,7,19,});
sparse_graph4_ints[25][100] = vector<int>({10,30,30,15,16,16,21,21,22,10,11,31,14,14,36,0,0,26,3,3,47,0,0,3,1,6,37,9,11,11,0,0,20,0,0,36,12,13,14,3,4,4,3,7,7,0,23,34,1,10,29,8,8,8,25,25,25,0,0,46,1,3,3,16,16,26,0,0,14,0,18,41,0,0,32,0,19,29,0,0,0,0,20,21,0,0,40,5,8,40,17,18,18,5,25,35,20,20,30,4,4,20,10,11,23,9,27,27,0,28,28,11,11,35,1,5,25,11,12,12,0,0,23,6,7,7,6,8,34,2,3,30,1,17,19,1,22,22,0,32,34,0,0,5,9,9,9,0,0,9,0,0,43,17,17,29,0,0,16,1,11,44,0,22,31,0,14,31,9,23,24,5,22,22,1,16,16,0,7,15,0,16,35,0,1,48,14,14,15,0,0,28,0,0,18,2,8,24,3,4,18,0,30,31,0,5,33,1,4,41,0,6,44,5,5,5,0,0,12,8,22,30,14,14,22,0,14,15,0,23,38,17,27,28,10,11,39,0,26,26,10,10,27,1,17,37,19,19,19,2,9,26,23,23,23,1,9,39,2,4,35,12,24,32,7,11,22,15,23,23,4,4,5,0,17,26,0,0,30,9,17,30,13,14,26,16,24,26,17,17,33,0,0,1,6,7,30,6,7,19,});
sparse_graph4_ints[20][100] = vector<int>({10,30,30,15,16,16,21,21,22,10,11,31,14,14,36,0,0,26,3,3,47,0,0,3,1,6,37,9,11,11,0,0,20,0,0,36,12,13,14,3,4,4,3,7,7,0,23,34,1,10,29,8,8,8,25,25,25,0,0,46,1,3,3,16,16,26,0,0,14,0,18,41,0,0,32,0,19,29,0,0,0,0,20,21,0,0,40,5,8,40,17,18,18,5,25,35,20,20,30,4,4,20,10,11,23,9,27,27,0,28,28,11,11,35,1,5,25,11,12,12,0,0,23,6,7,7,6,8,34,2,3,30,1,17,19,1,22,22,0,32,34,0,0,5,9,9,9,0,0,9,0,0,43,17,17,29,0,0,16,1,11,44,0,22,31,0,14,31,9,23,24,5,22,22,1,16,16,0,7,15,0,16,35,0,1,48,14,14,15,0,0,28,0,0,18,2,8,24,3,4,18,0,30,31,0,5,33,1,4,41,0,6,44,5,5,5,0,0,12,8,22,30,14,14,22,0,14,15,0,23,38,17,27,28,10,11,39,0,26,26,10,10,27,1,17,37,19,19,19,2,9,26,23,23,23,1,9,39,2,4,35,12,24,32,7,11,22,15,23,23,4,4,5,0,17,26,0,0,30,9,17,30,13,14,26,16,24,26,17,17,33,0,0,1,6,7,30,6,7,19,});
sparse_graph4_ints[15][100] = vector<int>({10,30,30,15,16,16,21,21,22,10,11,31,14,14,36,0,0,26,3,3,47,0,0,3,1,6,37,9,11,11,0,0,20,0,0,36,12,13,14,3,4,4,3,7,7,0,23,34,1,10,29,8,8,8,25,25,25,0,0,46,1,3,3,16,16,26,0,0,14,0,18,41,0,0,32,0,19,29,0,0,0,0,20,21,0,0,40,5,8,40,17,18,18,5,25,35,20,20,30,4,4,20,10,11,23,9,27,27,0,28,28,11,11,35,1,5,25,11,12,12,0,0,23,6,7,7,6,8,34,2,3,30,1,17,19,1,22,22,0,32,34,0,0,5,9,9,9,0,0,9,0,0,43,17,17,29,0,0,16,1,11,44,0,22,31,0,14,31,9,23,24,5,22,22,1,16,16,0,7,15,0,16,35,0,1,48,14,14,15,0,0,28,0,0,18,2,8,24,3,4,18,0,30,31,0,5,33,1,4,41,0,6,44,5,5,5,0,0,12,8,22,30,14,14,22,0,14,15,0,23,38,17,27,28,10,11,39,0,26,26,10,10,27,1,17,37,19,19,19,2,9,26,23,23,23,1,9,39,2,4,35,12,24,32,7,11,22,15,23,23,4,4,5,0,17,26,0,0,30,9,17,30,13,14,26,16,24,26,17,17,33,0,0,1,6,7,30,6,7,19,});
sparse_graph4_ints[10][100] = vector<int>({10,30,30,15,16,16,21,21,22,10,11,31,14,14,36,0,0,26,3,3,47,0,0,3,1,6,37,9,11,11,0,0,20,0,0,36,12,13,14,3,4,4,3,7,7,0,23,34,1,10,29,8,8,8,25,25,25,0,0,46,1,3,3,16,16,26,0,0,14,0,18,41,0,0,32,0,19,29,0,0,0,0,20,21,0,0,40,5,8,40,17,18,18,5,25,35,20,20,30,4,4,20,10,11,23,9,27,27,0,28,28,11,11,35,1,5,25,11,12,12,0,0,23,6,7,7,6,8,34,2,3,30,1,17,19,1,22,22,0,32,34,0,0,5,9,9,9,0,0,9,0,0,43,17,17,29,0,0,16,1,11,44,0,22,31,0,14,31,9,23,24,5,22,22,1,16,16,0,7,15,0,16,35,0,1,48,14,14,15,0,0,28,0,0,18,2,8,24,3,4,18,0,30,31,0,5,33,1,4,41,0,6,44,5,5,5,0,0,12,8,22,30,14,14,22,0,14,15,0,23,38,17,27,28,10,11,39,0,26,26,10,10,27,1,17,37,19,19,19,2,9,26,23,23,23,1,9,39,2,4,35,12,24,32,7,11,22,15,23,23,4,4,5,0,17,26,0,0,30,9,17,30,13,14,26,16,24,26,17,17,33,0,0,1,6,7,30,6,7,19,});
sparse_graph4_ints[5][100] = vector<int>({10,30,30,15,16,16,21,21,22,10,11,31,14,14,36,0,0,26,3,3,47,0,0,3,1,6,37,9,11,11,0,0,20,0,0,36,12,13,14,3,4,4,3,7,7,0,23,34,1,10,29,8,8,8,25,25,25,0,0,46,1,3,3,16,16,26,0,0,14,0,18,41,0,0,32,0,19,29,0,0,0,0,20,21,0,0,40,5,8,40,17,18,18,5,25,35,20,20,30,4,4,20,10,11,23,9,27,27,0,28,28,11,11,35,1,5,25,11,12,12,0,0,23,6,7,7,6,8,34,2,3,30,1,17,19,1,22,22,0,32,34,0,0,5,9,9,9,0,0,9,0,0,43,17,17,29,0,0,16,1,11,44,0,22,31,0,14,31,9,23,24,5,22,22,1,16,16,0,7,15,0,16,35,0,1,48,14,14,15,0,0,28,0,0,18,2,8,24,3,4,18,0,30,31,0,5,33,1,4,41,0,6,44,5,5,5,0,0,12,8,22,30,14,14,22,0,14,15,0,23,38,17,27,28,10,11,39,0,26,26,10,10,27,1,17,37,19,19,19,2,9,26,23,23,23,1,9,39,2,4,35,12,24,32,7,11,22,15,23,23,4,4,5,0,17,26,0,0,30,9,17,30,13,14,26,16,24,26,17,17,33,0,0,1,6,7,30,6,7,19,});


for(int error_rate = 0; error_rate <= 40; ++error_rate){
if(sparse_graph4_ints.count(error_rate) == 0) continue;
for(int num_nodes = 4; num_nodes <= 100; ++num_nodes){
if(sparse_graph4_ints[error_rate].count(num_nodes) == 0) continue;

vector<CompleteGraphSolver::CompleteGraph> v;
for(int j = 0; j < sz(sparse_graph4_ints[error_rate][num_nodes]); j += 3){
int i0 = sparse_graph4_ints[error_rate][num_nodes][j + 0];
int i1 = sparse_graph4_ints[error_rate][num_nodes][j + 1];
int i2 = sparse_graph4_ints[error_rate][num_nodes][j + 2];
v.push_back(CompleteGraphSolver::CompleteGraph(vector<int>({i0,i1,i2,num_nodes - i0 - i1 - i2})));
}
graphs_4_non_flip[error_rate][num_nodes] = v;
}
my_assert(18 == sz(graphs_4_non_flip[error_rate]));
}

/*
sparse_graph4_ints[40][6] = vector<int>({0,0,0,1,1,2,0,2,2,0,1,2,1,1,1,0,0,3,0,0,2,0,1,1,0,0,1,});
sparse_graph4_ints[40][8] = vector<int>({2,2,2,0,0,0,0,0,4,1,2,2,1,1,3,0,2,3,1,1,2,0,1,3,0,2,2,0,0,3,0,1,2,0,0,2,1,1,1,0,0,1,0,1,1,});
sparse_graph4_ints[40][10] = vector<int>({2,2,3,0,0,0,0,0,4,1,3,3,2,2,2,1,2,3,0,3,3,1,1,4,0,2,4,0,0,5,1,2,2,0,2,3,0,1,4,1,1,3,0,0,2,0,1,3,1,1,2,0,2,2,0,0,1,0,0,3,0,1,2,1,1,1,0,1,1,});
sparse_graph4_ints[40][12] = vector<int>({3,3,3,0,0,0,0,0,6,2,3,3,2,2,4,1,3,4,0,0,2,0,4,4,2,2,3,1,3,3,0,0,4,0,3,4,1,2,4,0,2,5,1,1,5,0,0,5,1,2,3,0,3,3,2,2,2,1,1,4,0,1,5,0,2,4,0,0,1,1,2,2,0,1,4,0,2,3,1,1,3,0,1,1,1,1,1,0,2,2,0,1,3,0,0,3,1,1,2,0,1,2,});
sparse_graph4_ints[40][14] = vector<int>({3,3,4,0,0,0,0,0,6,2,4,4,1,1,1,3,3,3,2,3,4,1,4,4,0,0,7,0,4,5,0,0,2,2,2,5,1,3,5,2,3,3,1,3,4,0,0,4,0,4,4,2,2,4,0,3,5,1,2,5,0,2,6,1,1,6,0,1,6,2,2,3,0,3,4,1,2,4,1,3,3,0,2,5,0,0,5,1,1,5,0,1,1,0,0,1,0,1,5,0,2,2,0,3,3,1,2,3,2,2,2,1,1,4,0,2,4,0,1,2,0,0,3,1,1,3,1,2,2,0,1,4,0,2,3,0,1,3,1,1,2,});
sparse_graph4_ints[40][16] = vector<int>({4,4,4,0,0,0,0,0,8,1,1,1,3,4,4,3,3,5,2,4,5,0,0,6,1,5,5,3,3,4,0,2,2,2,4,4,0,5,5,1,4,5,2,3,5,0,0,4,0,4,6,0,0,7,1,3,6,3,3,3,2,3,4,2,2,6,1,4,4,0,4,5,0,0,2,2,2,5,1,3,5,0,3,6,1,2,6,0,1,7,1,1,2,1,1,7,0,2,7,2,3,3,1,3,4,0,4,4,0,3,5,0,2,6,2,2,4,1,2,5,0,1,2,1,1,6,0,1,6,1,1,3,0,0,1,0,1,1,0,0,5,2,2,3,0,1,3,1,3,3,});
sparse_graph4_ints[40][18] = vector<int>({4,4,5,0,0,0,0,0,8,0,2,2,3,5,5,4,4,4,0,0,4,3,4,5,0,0,9,0,6,6,2,5,5,3,3,6,1,1,1,1,5,6,2,4,6,3,4,4,0,0,6,2,4,5,3,3,5,0,5,6,2,2,2,1,5,5,1,4,6,2,3,6,0,4,7,3,3,4,0,5,5,0,1,8,1,1,3,1,4,5,0,0,7,2,4,4,2,2,7,2,3,5,0,4,6,0,0,2,1,2,2,1,3,7,0,2,8,1,3,6,0,3,7,1,1,2,2,2,6,1,2,7,1,1,8,3,3,3,1,1,7,2,3,4,0,4,5,1,4,4,});
sparse_graph4_ints[40][20] = vector<int>({5,5,5,0,0,0,0,0,10,0,0,4,2,2,2,4,5,5,4,4,6,3,5,6,0,6,6,0,2,2,0,0,8,4,4,5,2,6,6,3,5,5,0,6,7,3,4,6,0,0,9,1,2,2,2,5,6,1,6,6,1,5,7,4,4,4,2,4,7,0,0,6,3,4,5,3,3,7,2,5,5,1,5,6,0,5,7,1,1,3,3,3,6,2,4,6,1,3,3,0,0,2,0,1,9,1,4,7,1,2,3,0,4,8,2,3,7,3,4,4,0,5,6,1,1,9,2,4,5,3,3,5,1,5,5,0,3,3,1,3,8,1,4,6,0,4,7,2,3,6,});
sparse_graph4_ints[40][23] = vector<int>({5,6,6,0,0,0,0,0,11,0,0,5,2,3,3,5,5,6,4,6,6,1,2,2,0,0,10,0,7,8,4,5,7,5,5,5,2,2,3,0,0,1,3,6,7,4,5,6,2,7,7,2,2,2,3,6,6,0,0,9,0,1,11,1,7,7,1,3,3,0,0,3,3,5,7,4,4,7,0,7,7,0,0,7,2,6,7,1,2,3,0,1,10,1,6,8,0,6,8,4,5,5,4,4,6,1,3,4,2,5,8,0,0,8,3,5,6,2,6,6,3,4,8,1,6,7,1,1,2,3,3,3,2,5,7,0,2,10,3,4,7,1,5,8,2,3,4,0,0,6,});
sparse_graph4_ints[40][26] = vector<int>({6,6,7,0,0,0,0,0,12,0,0,6,3,3,3,1,1,1,0,8,8,5,7,7,6,6,6,2,4,4,0,0,13,0,0,8,2,2,2,5,6,7,0,8,9,4,7,7,3,3,4,5,5,8,4,6,8,0,0,4,0,1,12,0,0,10,2,3,4,5,6,6,3,7,8,5,5,7,2,8,8,4,6,7,0,2,12,1,3,3,1,8,8,1,7,9,3,7,7,3,3,5,4,5,8,2,7,8,3,6,8,0,7,9,2,3,3,5,5,6,4,6,6,1,7,8,2,6,9,4,5,7,0,6,10,0,0,11,3,6,7,1,4,4,4,4,9,2,7,7,});
sparse_graph4_ints[40][30] = vector<int>({7,7,8,0,0,0,0,0,14,0,0,6,4,4,4,0,10,10,1,1,1,0,0,10,3,3,3,6,8,8,0,0,15,7,7,7,3,5,5,2,2,2,6,7,8,0,2,14,0,0,8,2,4,4,5,8,8,0,9,10,4,4,5,6,6,9,6,7,7,5,7,9,0,8,10,0,0,12,0,1,14,3,4,4,0,0,4,1,3,3,6,6,8,3,4,5,5,7,8,4,8,9,1,9,9,3,3,5,1,9,10,4,8,8,3,9,9,5,6,9,1,1,14,0,8,11,4,7,9,0,6,6,6,6,7,2,9,9,3,8,9,5,7,7,0,9,9,2,8,10,});
sparse_graph4_ints[40][35] = vector<int>({8,9,9,0,0,0,0,0,17,2,3,3,5,5,5,0,0,12,1,1,2,0,11,12,0,0,9,0,3,16,3,4,4,8,8,9,0,0,5,5,5,6,7,9,9,0,0,16,3,3,4,0,0,11,4,5,6,0,10,12,7,8,10,8,8,8,4,5,5,0,1,17,0,0,13,1,2,16,7,8,9,0,11,11,6,9,10,5,6,6,4,4,4,0,0,7,1,2,2,0,0,1,2,2,3,4,4,5,0,0,10,3,3,3,3,6,6,1,11,11,6,9,9,0,2,16,7,7,10,4,6,6,0,0,15,6,8,10,1,3,15,3,4,5,5,10,10,0,10,11,});
sparse_graph4_ints[40][40] = vector<int>({0,0,20,0,0,0,10,10,10,3,3,3,6,6,6,0,0,14,0,0,4,0,12,14,0,0,10,0,4,18,3,5,5,9,10,10,0,2,2,5,7,7,0,0,8,4,4,4,1,3,3,0,8,8,0,0,12,0,0,16,2,2,18,9,9,11,0,13,13,6,6,7,8,10,11,5,5,5,9,9,10,1,13,13,3,3,17,8,10,10,0,8,10,2,4,4,7,7,7,4,6,6,8,9,11,0,0,18,1,5,17,7,11,11,4,4,5,6,6,8,0,12,12,9,9,9,7,10,11,0,12,13,1,3,18,0,0,19,5,6,6,8,9,10,0,10,10,8,8,12,});
sparse_graph4_ints[40][45] = vector<int>({0,0,22,0,0,0,11,11,11,0,0,8,7,8,8,0,0,17,4,4,4,0,14,15,1,1,2,0,5,20,5,6,6,0,0,12,2,3,3,0,10,10,10,11,12,4,5,5,7,7,8,10,11,11,0,15,15,3,4,19,0,0,14,0,0,5,3,3,3,5,5,6,0,0,10,0,0,16,10,10,12,7,7,7,4,4,5,0,0,21,2,5,19,0,10,11,9,12,12,6,6,6,0,14,14,9,11,12,0,1,22,0,0,18,1,4,20,8,8,8,0,0,13,0,4,15,5,5,5,3,3,4,6,7,7,1,14,15,10,10,11,0,8,8,6,8,8,0,7,19,});
sparse_graph4_ints[40][50] = vector<int>({0,0,0,12,12,13,0,0,24,3,3,3,7,7,7,0,0,16,0,6,22,0,0,4,0,16,16,0,0,12,5,5,5,9,9,9,0,0,20,0,0,8,0,12,12,0,4,18,6,6,6,4,4,4,11,13,13,2,2,2,3,5,21,0,0,14,0,0,18,0,0,25,0,8,8,8,8,8,12,12,12,0,16,17,0,10,10,0,0,10,1,1,1,2,4,4,4,6,20,11,12,13,8,9,9,0,4,16,4,6,6,3,5,5,1,3,23,6,6,8,8,8,10,11,11,14,1,15,17,0,12,14,10,13,13,1,7,21,11,12,12,8,8,9,0,14,18,10,12,14,});
sparse_graph4_ints[38][6] = vector<int>({1,1,2,0,0,0,0,2,2,0,1,2,1,1,1,0,0,3,0,0,2,0,1,1,0,0,1,});
sparse_graph4_ints[38][8] = vector<int>({2,2,2,0,0,0,0,0,4,1,2,2,0,2,3,1,1,3,1,1,2,0,1,3,0,2,2,0,0,3,0,0,2,0,1,2,1,1,1,0,0,1,0,1,1,});
sparse_graph4_ints[38][10] = vector<int>({2,2,3,0,0,0,0,0,4,1,3,3,2,2,2,0,3,3,1,2,3,0,0,2,1,1,4,0,2,4,0,0,5,1,2,2,0,2,3,0,1,4,1,1,3,0,1,3,1,1,2,0,2,2,0,0,1,0,0,3,1,1,1,0,1,1,0,1,2,});
sparse_graph4_ints[38][12] = vector<int>({3,3,3,0,0,0,0,0,6,0,0,2,2,3,3,2,2,4,1,3,4,0,4,4,0,0,4,2,2,3,1,3,3,0,3,4,1,2,4,1,1,1,0,2,5,1,1,5,0,0,5,0,3,3,1,2,3,2,2,2,0,1,5,1,1,4,0,2,4,0,1,1,0,0,1,0,2,3,0,1,4,1,2,2,0,2,2,1,1,3,0,0,3,0,1,2,0,1,3,1,1,2,});
sparse_graph4_ints[38][14] = vector<int>({3,3,4,0,0,0,0,0,6,1,1,1,2,4,4,3,3,3,2,3,4,0,0,2,0,0,7,1,4,4,0,4,5,0,0,4,2,2,5,1,3,5,0,4,4,2,3,3,0,2,2,1,3,4,2,2,4,0,3,5,1,2,5,0,2,6,0,1,6,1,1,6,0,1,2,0,3,4,2,2,3,1,3,3,1,2,4,0,2,5,0,0,5,1,1,5,0,1,1,0,1,5,2,2,2,1,1,2,0,3,3,1,2,3,0,0,3,0,2,4,0,1,3,0,0,1,1,1,4,1,1,3,1,2,2,0,2,3,0,1,4,});
sparse_graph4_ints[38][16] = vector<int>({4,4,4,0,0,0,0,0,8,0,2,2,3,4,4,0,0,2,3,3,5,2,4,5,0,0,6,1,5,5,1,1,2,0,5,5,3,3,4,2,4,4,0,0,4,1,4,5,0,4,6,2,3,5,0,0,7,1,1,1,1,3,6,0,4,5,3,3,3,2,3,4,1,4,4,2,2,6,1,1,3,2,2,5,1,3,5,0,3,6,0,1,7,2,2,2,1,1,7,1,2,6,0,2,7,1,2,2,0,4,4,2,3,3,1,3,4,0,1,2,0,2,6,0,3,5,2,2,4,0,1,3,1,2,5,1,1,6,0,2,3,0,1,6,0,0,5,1,3,3,});
sparse_graph4_ints[38][18] = vector<int>({4,4,5,0,0,0,0,0,8,0,0,4,2,2,2,3,5,5,0,6,6,0,2,2,4,4,4,0,0,9,3,4,5,2,5,5,0,0,2,0,0,6,1,5,6,1,2,2,3,3,6,2,4,6,0,5,6,3,4,4,2,4,5,1,5,5,3,3,5,1,1,3,1,4,6,0,1,8,2,3,6,0,4,7,0,3,3,1,1,1,0,2,8,0,5,5,3,3,4,1,3,3,0,4,6,0,0,7,2,4,4,1,4,5,2,2,7,1,3,7,2,3,5,0,2,3,1,3,6,0,3,7,1,2,3,1,1,8,2,2,6,3,3,3,1,2,7,1,1,7,});
sparse_graph4_ints[38][20] = vector<int>({5,5,5,0,0,0,0,0,10,0,2,2,1,3,3,0,0,6,4,5,5,0,0,2,0,6,6,4,4,6,2,2,2,3,5,6,4,4,5,0,6,7,0,0,8,0,0,4,2,6,6,2,2,3,3,5,5,0,0,9,1,6,6,3,4,6,2,5,6,1,5,7,0,3,3,4,4,4,2,4,7,0,4,4,1,1,1,3,4,5,0,1,9,0,5,7,3,3,7,1,5,6,2,5,5,1,2,3,3,3,6,1,1,9,2,4,6,2,2,4,1,4,7,0,4,8,3,3,3,1,1,3,2,3,7,0,5,6,3,4,4,2,3,3,2,4,5,1,5,5,});
sparse_graph4_ints[38][23] = vector<int>({5,6,6,0,0,0,0,0,11,1,2,2,3,3,3,0,0,7,0,7,8,0,0,3,5,5,6,2,3,3,0,0,10,4,6,6,4,5,7,5,5,5,0,1,11,2,3,4,0,0,6,1,1,2,1,7,7,3,6,7,2,7,7,1,4,4,4,5,6,2,2,3,0,0,1,0,0,5,0,6,8,0,0,9,3,6,6,0,7,7,0,3,3,0,1,10,3,3,4,4,4,7,3,5,7,0,0,8,2,6,7,0,2,10,1,3,4,1,6,8,0,1,2,0,4,4,4,5,5,4,4,6,1,3,3,1,1,10,1,6,7,2,5,8,2,6,6,3,5,6,});
sparse_graph4_ints[38][26] = vector<int>({6,6,7,0,0,0,0,0,8,0,0,12,1,1,1,4,4,4,2,2,2,0,8,8,3,3,3,5,7,7,0,0,13,0,0,6,6,6,6,2,4,4,0,2,12,0,0,10,0,0,4,5,6,7,0,8,9,3,4,4,1,3,3,4,7,7,5,5,8,3,3,4,4,6,8,1,7,9,5,6,6,0,1,12,3,3,5,5,5,7,1,8,8,3,7,8,2,8,8,4,6,7,1,5,5,0,7,9,3,7,7,0,4,4,0,6,10,2,7,8,4,5,8,0,2,2,0,5,5,3,6,8,1,1,12,5,5,6,0,6,6,4,6,6,1,7,8,2,3,3,});
sparse_graph4_ints[38][30] = vector<int>({7,7,8,0,0,0,0,0,14,4,4,4,0,0,4,0,0,10,1,3,3,0,10,10,1,3,13,0,0,8,1,1,1,6,8,8,5,5,5,3,3,3,0,6,6,7,7,7,0,0,15,0,0,6,0,0,12,2,2,2,3,5,5,2,4,4,0,2,14,6,7,8,0,9,10,4,4,6,5,8,8,6,6,9,0,8,10,6,7,7,4,5,5,5,7,9,3,3,5,0,4,13,6,6,8,1,9,9,4,4,5,5,7,8,1,9,10,4,8,9,0,4,4,0,1,14,0,7,7,0,8,11,4,8,8,3,4,5,3,9,9,5,6,9,1,1,14,6,6,7,});
sparse_graph4_ints[38][35] = vector<int>({0,0,17,0,0,0,8,9,9,2,3,3,5,6,6,0,0,10,0,0,3,0,11,12,4,4,5,0,5,15,0,0,13,0,0,6,8,8,9,3,4,4,0,8,8,0,3,3,0,0,8,1,1,2,3,3,4,5,5,5,0,0,12,0,3,16,0,0,16,7,9,9,6,6,6,4,4,4,0,10,12,8,8,8,1,4,15,7,8,10,5,5,6,0,0,14,0,1,17,0,0,11,0,7,8,3,3,3,1,11,11,4,5,5,7,8,9,2,3,15,5,6,7,0,0,9,3,4,5,2,2,3,6,9,10,0,11,11,0,8,9,0,6,6,4,6,6,6,9,9,});
sparse_graph4_ints[38][40] = vector<int>({10,10,10,0,0,0,4,4,4,0,0,20,0,0,14,0,2,2,7,7,7,0,0,10,0,12,14,0,4,18,0,8,8,0,0,6,3,3,3,0,0,16,5,5,5,0,10,10,4,4,16,9,10,10,0,0,12,0,4,14,0,0,2,2,2,2,0,0,8,6,6,6,2,4,4,3,5,5,9,9,11,1,13,13,0,0,4,1,3,3,6,7,7,1,5,17,9,9,10,8,10,11,0,13,13,0,6,6,4,6,6,7,7,8,8,10,10,0,4,12,5,7,7,3,3,17,4,4,6,3,3,4,0,8,10,6,8,8,4,5,5,6,6,7,8,9,11,0,12,12,});
sparse_graph4_ints[38][45] = vector<int>({0,0,0,11,11,11,0,0,22,6,6,6,0,0,14,1,1,2,4,4,4,0,5,20,0,15,15,0,0,10,8,8,8,0,0,18,2,3,3,0,5,16,0,0,6,0,8,8,0,12,12,5,5,17,10,11,12,7,7,7,0,3,14,5,5,5,3,4,4,0,0,12,0,0,3,0,0,8,0,10,10,2,2,3,0,14,15,10,11,11,8,8,9,0,0,16,4,5,5,3,3,4,6,6,7,0,8,18,0,1,22,0,0,19,0,5,15,1,4,20,10,10,12,7,8,8,0,0,13,5,5,6,9,12,12,1,14,15,0,11,12,4,5,18,8,9,9,6,7,7,});
sparse_graph4_ints[38][50] = vector<int>({12,12,13,0,0,14,0,0,0,1,3,23,7,7,7,4,4,4,0,0,20,0,2,2,0,16,17,9,9,9,0,0,10,0,0,24,0,4,16,0,8,8,5,7,19,0,0,6,0,12,12,3,3,3,0,0,16,0,6,18,0,0,2,5,5,5,0,0,12,0,10,10,0,14,14,11,13,13,0,10,20,8,8,8,10,10,10,6,6,6,1,3,3,0,0,8,1,3,13,0,0,18,0,6,6,0,16,16,12,12,12,0,6,22,0,0,22,0,0,25,3,3,17,0,0,4,2,4,4,2,2,2,4,6,20,9,9,10,11,12,13,7,8,8,4,4,6,5,7,7,});
sparse_graph4_ints[36][6] = vector<int>({0,2,2,0,0,0,1,1,2,0,1,2,0,0,3,1,1,1,0,0,2,0,1,1,0,0,1,});
sparse_graph4_ints[36][8] = vector<int>({2,2,2,0,0,0,0,0,4,1,2,2,0,2,3,1,1,3,1,1,2,0,2,2,0,0,2,0,1,3,0,0,3,0,1,2,1,1,1,0,0,1,0,1,1,});
sparse_graph4_ints[36][10] = vector<int>({2,2,3,0,0,0,0,0,4,1,3,3,0,0,2,2,2,2,0,3,3,1,2,3,0,0,5,0,2,4,1,1,4,0,2,3,1,2,2,0,1,4,1,1,3,0,1,1,0,2,2,0,1,3,0,0,1,1,1,2,1,1,1,0,0,3,0,1,2,});
sparse_graph4_ints[36][12] = vector<int>({3,3,3,0,0,0,0,0,6,0,0,2,2,3,3,0,4,4,1,1,1,1,3,4,2,2,4,0,0,4,2,2,3,0,3,4,1,3,3,1,2,4,0,2,5,0,1,1,0,0,5,1,1,5,0,3,3,0,2,2,1,2,3,0,1,5,2,2,2,0,2,4,1,1,4,0,1,2,0,2,3,0,0,3,1,2,2,0,1,4,1,1,3,0,0,1,1,1,2,0,1,3,});
sparse_graph4_ints[36][14] = vector<int>({3,3,4,0,0,0,0,0,6,0,2,2,0,0,2,2,4,4,3,3,3,0,0,7,0,0,4,2,3,4,0,4,5,1,4,4,1,1,1,1,3,5,2,2,5,0,4,4,2,3,3,1,3,4,0,3,5,2,2,4,1,1,2,0,2,6,1,2,5,0,1,6,0,1,2,1,1,6,0,3,4,2,2,3,1,3,3,1,2,4,0,0,5,0,2,5,1,1,3,1,1,5,0,1,1,2,2,2,0,1,3,0,0,3,0,3,3,0,1,5,1,2,2,0,2,4,1,2,3,0,2,3,1,1,4,0,1,4,0,0,1,});
sparse_graph4_ints[36][16] = vector<int>({4,4,4,0,0,0,0,0,4,0,0,8,0,0,2,2,2,2,3,4,4,1,5,5,0,2,2,3,3,5,2,4,5,0,5,5,0,0,6,0,4,6,3,3,4,2,4,4,1,2,2,1,4,5,1,1,1,2,3,5,0,0,7,1,3,6,1,1,3,0,4,5,3,3,3,1,4,4,2,2,6,2,3,4,0,2,3,0,1,7,2,2,5,1,3,5,1,1,7,0,3,6,0,3,3,1,1,2,0,4,4,1,2,6,0,2,7,2,3,3,1,3,3,0,1,2,1,3,4,0,2,6,2,2,4,0,3,5,0,1,3,1,2,3,0,0,5,0,2,4,});
sparse_graph4_ints[36][18] = vector<int>({4,4,5,0,0,0,0,0,8,2,2,2,0,0,2,0,6,6,0,2,2,0,0,6,3,5,5,0,0,9,4,4,4,0,0,4,1,3,3,3,4,5,2,5,5,1,5,6,0,3,3,3,3,6,0,5,6,2,4,6,1,1,1,3,4,4,1,5,5,2,2,3,0,2,8,2,4,5,3,3,5,1,2,3,0,1,8,1,4,6,0,4,7,2,3,6,1,2,2,0,5,5,0,4,4,3,3,4,0,4,6,2,4,4,1,3,7,1,4,5,1,1,3,2,2,7,3,3,3,0,0,7,2,3,5,0,2,3,1,1,8,0,3,4,1,3,6,0,3,7,});
sparse_graph4_ints[36][20] = vector<int>({5,5,5,0,0,0,0,0,10,0,2,2,0,4,4,0,0,6,0,0,2,0,6,6,4,5,5,2,2,2,4,4,6,0,0,4,3,3,3,0,6,7,3,5,6,1,3,3,0,0,8,1,1,9,4,4,5,2,6,6,3,5,5,1,6,6,2,3,3,3,4,6,1,1,1,0,3,3,2,5,6,1,5,7,0,0,9,2,2,3,4,4,4,0,1,9,0,5,7,2,4,7,2,2,4,3,4,5,3,3,7,1,5,6,2,5,5,0,4,8,3,3,6,0,3,4,2,4,6,1,4,7,1,3,4,1,1,3,0,5,6,3,4,4,0,2,9,2,3,7,});
sparse_graph4_ints[36][23] = vector<int>({0,0,0,5,6,6,0,0,6,0,0,11,3,3,4,0,0,3,0,3,3,0,7,8,0,0,8,5,5,6,3,3,3,0,1,11,0,1,2,4,6,6,2,3,3,1,2,2,0,0,10,4,5,7,0,5,5,0,0,7,1,7,7,5,5,5,0,2,10,1,4,4,3,6,7,0,6,8,0,0,5,2,3,4,4,5,6,2,2,3,0,0,9,2,7,7,0,3,10,3,4,4,3,6,6,0,7,7,0,4,5,4,4,7,3,5,7,2,6,7,1,1,10,1,6,8,2,4,4,0,2,3,0,4,4,0,1,10,4,5,5,1,6,7,1,4,5,4,4,6,});
sparse_graph4_ints[36][26] = vector<int>({6,6,7,0,0,0,0,0,12,3,3,3,0,2,2,0,0,8,0,8,8,4,4,4,0,0,6,0,0,2,1,3,11,2,2,2,0,0,10,5,7,7,0,0,13,0,6,6,6,6,6,2,4,4,0,0,4,1,3,3,0,2,12,0,8,9,5,6,7,3,4,4,0,4,4,3,5,5,4,7,7,5,5,8,3,3,5,1,7,9,5,6,6,4,6,8,1,8,8,1,1,12,1,5,5,5,5,7,2,8,8,4,4,5,3,7,8,0,6,10,4,6,7,0,5,5,0,7,9,3,4,5,3,7,7,2,7,8,0,1,12,4,5,8,5,5,6,3,3,4,});
sparse_graph4_ints[36][30] = vector<int>({7,7,8,0,0,0,3,3,3,0,0,14,0,2,2,0,0,10,5,5,5,0,10,10,0,0,6,1,3,13,0,6,6,0,0,2,2,2,2,0,0,8,0,0,12,6,8,8,4,4,4,0,0,4,0,4,4,0,0,15,0,4,13,7,7,7,4,5,5,2,4,4,0,8,8,6,7,8,0,2,10,0,2,14,4,4,6,1,3,3,3,3,4,3,5,5,0,9,10,4,6,6,5,8,8,2,2,13,6,6,9,0,6,8,6,7,7,1,9,9,5,7,9,6,6,8,1,9,10,4,4,5,2,4,12,5,5,6,3,3,5,0,8,10,0,7,7,5,7,8,});
sparse_graph4_ints[36][35] = vector<int>({0,0,17,0,0,0,8,9,9,0,6,7,0,0,6,0,0,10,0,5,15,6,6,6,0,0,3,0,0,14,3,3,3,0,11,12,0,3,3,0,8,9,0,0,12,0,0,8,3,4,4,5,5,5,0,3,12,8,8,9,0,3,16,0,1,2,2,3,3,4,4,4,6,6,7,1,2,2,4,5,5,3,4,14,7,9,9,1,11,11,0,8,8,3,3,4,0,0,11,5,6,6,0,6,6,0,0,9,0,0,13,0,0,16,1,4,15,8,8,8,0,1,17,1,2,12,0,10,12,7,8,10,6,7,7,5,5,6,4,4,5,1,4,4,0,6,14,0,9,9,});
sparse_graph4_ints[36][40] = vector<int>({10,10,10,0,0,0,0,0,12,0,0,20,0,0,4,7,7,7,4,4,4,0,8,16,0,0,8,0,0,16,0,8,8,1,13,13,2,2,2,0,4,14,0,4,18,0,0,2,3,3,3,0,10,10,0,0,10,0,0,14,5,5,5,0,0,6,6,6,6,8,8,8,0,6,6,0,4,12,9,10,10,5,5,15,0,2,2,0,4,4,0,12,14,9,9,11,2,2,18,0,0,18,7,7,8,4,4,16,2,2,14,0,4,10,6,6,7,0,10,12,2,4,4,4,6,6,3,5,5,0,13,13,8,10,11,0,8,10,7,8,8,9,9,10,6,7,7,5,5,6,});
sparse_graph4_ints[36][45] = vector<int>({3,4,4,11,11,11,0,0,22,0,0,0,0,0,14,6,6,6,0,0,4,0,9,18,0,0,18,8,8,8,0,0,8,0,15,15,0,6,15,0,0,12,4,5,5,0,3,3,0,5,20,0,10,10,0,4,13,0,12,13,0,1,1,0,0,10,2,3,3,0,0,16,9,9,9,0,8,8,7,7,7,0,0,6,6,7,16,0,4,17,10,11,12,0,6,6,1,1,2,3,3,3,0,3,11,5,5,5,0,0,20,0,3,21,4,5,18,0,4,14,0,11,12,0,14,15,6,7,7,8,9,9,4,4,15,10,11,11,7,8,8,5,6,6,4,4,4,0,9,10,});
sparse_graph4_ints[36][50] = vector<int>({12,12,13,0,0,0,5,5,5,1,3,23,2,2,2,0,0,18,8,8,8,0,0,14,0,16,17,0,0,8,0,0,22,0,10,20,3,3,15,0,0,2,4,4,4,10,10,10,0,10,10,0,14,14,0,0,12,0,0,25,4,6,16,1,3,13,0,0,4,3,3,3,6,6,6,0,12,12,7,7,18,0,0,10,0,4,20,0,0,20,0,0,16,0,0,6,7,7,7,0,8,8,3,5,21,9,9,9,0,4,18,0,6,6,0,3,11,0,2,2,1,3,15,0,8,16,11,13,13,1,3,3,2,4,4,0,0,24,0,6,22,2,16,16,6,8,18,12,12,12,});
sparse_graph4_ints[34][6] = vector<int>({0,2,2,0,0,0,1,1,2,0,0,2,0,1,2,1,1,1,0,0,3,0,1,1,0,0,1,});
sparse_graph4_ints[34][8] = vector<int>({2,2,2,0,0,0,0,0,4,1,2,2,0,2,3,0,0,2,1,1,3,0,2,2,1,1,2,0,1,3,0,0,3,0,1,2,1,1,1,0,0,1,0,1,1,});
sparse_graph4_ints[34][10] = vector<int>({2,2,3,0,0,0,0,0,4,0,0,2,1,3,3,0,3,3,2,2,2,1,2,3,0,0,5,0,1,1,0,2,4,1,1,4,0,2,3,1,2,2,0,1,4,1,1,1,1,1,3,0,2,2,0,1,3,0,0,3,0,0,1,1,1,2,0,1,2,});
sparse_graph4_ints[34][12] = vector<int>({3,3,3,0,0,0,0,0,6,1,1,1,0,0,2,0,4,4,2,3,3,1,3,4,0,0,4,2,2,4,0,2,2,0,3,4,2,2,3,1,3,3,1,2,4,0,2,5,0,1,1,1,1,5,0,0,5,0,3,3,2,2,2,0,1,2,0,1,5,1,2,3,0,2,4,1,1,4,0,0,3,1,1,2,0,2,3,1,2,2,1,1,3,0,1,3,0,1,4,0,0,1,});
sparse_graph4_ints[34][14] = vector<int>({3,3,4,0,0,0,0,0,6,0,2,2,0,0,2,2,4,4,0,0,4,3,3,3,0,0,7,0,4,5,2,3,4,1,4,4,1,1,1,2,2,2,0,4,4,1,3,5,2,2,5,2,3,3,1,3,4,1,1,2,0,3,5,0,2,6,2,2,4,1,1,3,0,1,6,1,2,5,0,2,3,0,1,1,1,2,2,1,1,6,1,3,3,0,3,4,0,1,2,0,0,5,2,2,3,0,3,3,0,1,3,1,1,5,0,2,5,1,2,4,0,0,3,0,2,4,1,2,3,0,1,5,1,1,4,0,1,4,0,0,1,});
sparse_graph4_ints[34][16] = vector<int>({4,4,4,0,0,0,0,0,4,0,0,8,2,2,2,0,0,2,3,4,4,0,4,6,0,2,2,0,0,6,3,3,5,1,5,5,0,3,3,2,4,5,0,5,5,1,1,1,3,3,4,2,4,4,1,3,3,1,4,5,2,3,5,0,2,3,0,4,5,1,3,6,3,3,3,1,1,7,1,2,2,0,0,7,2,2,6,1,4,4,2,3,4,1,1,3,0,1,7,0,4,4,1,3,5,0,3,6,2,2,5,1,2,3,0,1,1,0,2,4,0,2,7,2,2,3,1,1,2,2,3,3,1,2,6,0,0,5,2,2,4,1,3,4,0,2,6,0,3,4,});
sparse_graph4_ints[34][18] = vector<int>({4,4,5,0,0,0,0,2,2,0,0,8,1,3,3,0,0,2,0,0,6,0,6,6,0,0,4,0,0,9,3,5,5,2,2,2,4,4,4,0,4,4,3,4,5,0,2,8,0,3,3,1,1,1,2,5,5,1,5,6,0,5,6,2,2,3,3,3,6,2,4,6,3,4,4,1,5,5,3,3,3,3,3,5,2,4,5,0,1,8,0,3,4,0,4,7,1,4,6,0,2,3,2,3,3,2,3,6,0,5,5,0,4,6,3,3,4,1,2,3,1,3,7,2,4,4,2,2,4,2,2,7,1,4,5,1,1,8,1,1,3,0,0,7,1,3,4,2,3,5,});
sparse_graph4_ints[34][20] = vector<int>({5,5,5,0,0,0,2,2,2,0,0,10,0,0,2,0,0,6,3,3,3,0,6,6,0,0,4,0,2,2,0,4,4,4,5,5,0,0,8,1,1,9,4,4,6,1,3,3,0,6,7,3,5,6,4,4,5,2,3,3,2,6,6,3,5,5,0,4,8,1,1,1,0,3,3,2,4,4,1,6,6,1,5,7,3,4,6,2,2,4,2,5,6,4,4,4,0,2,9,1,4,4,0,5,7,2,4,7,3,4,5,2,2,3,3,3,7,0,4,5,1,5,6,2,5,5,2,3,4,0,1,9,1,3,4,0,2,3,3,3,6,2,4,6,3,3,4,2,2,8,});
sparse_graph4_ints[34][23] = vector<int>({5,6,6,0,0,0,0,0,11,0,2,3,3,4,4,0,0,8,0,0,3,0,7,8,2,3,3,0,0,6,0,3,10,5,5,6,3,3,4,0,3,3,0,1,1,0,0,4,0,0,9,0,1,11,0,5,5,4,6,6,2,2,3,0,0,7,3,3,3,1,7,7,0,0,10,0,2,10,5,5,5,1,2,2,0,4,5,4,5,7,4,4,4,0,1,2,0,0,5,0,3,4,0,6,8,1,4,4,0,5,6,3,6,7,1,2,10,4,5,6,2,4,4,2,7,7,2,2,2,2,3,4,0,7,7,0,4,4,3,6,6,2,4,5,4,4,7,1,4,5,});
sparse_graph4_ints[34][26] = vector<int>({0,0,0,6,6,7,2,2,2,0,0,12,4,4,4,0,0,8,0,0,2,1,3,11,0,0,4,3,3,3,0,8,8,0,0,10,0,0,6,0,6,6,5,7,7,0,2,2,0,4,4,0,2,12,6,6,6,3,5,5,0,8,9,2,4,4,0,0,13,5,6,7,0,4,11,4,4,5,1,3,3,1,1,9,0,5,5,5,5,5,3,4,4,4,7,7,1,7,9,5,5,8,2,2,11,5,6,6,2,8,8,4,5,5,0,6,10,4,6,8,0,6,7,1,8,8,5,5,7,1,5,5,0,3,11,4,4,6,3,7,8,3,3,5,4,6,7,0,7,9,});
sparse_graph4_ints[34][30] = vector<int>({7,7,8,0,0,0,3,3,3,0,0,14,0,0,4,0,0,10,5,5,5,1,3,13,0,10,10,0,0,8,0,0,2,2,2,2,0,6,6,0,0,12,0,0,6,0,8,8,4,4,4,0,2,10,0,2,2,7,7,7,0,0,15,2,4,12,0,4,4,6,8,8,5,5,6,0,4,10,2,4,4,0,2,14,3,5,5,1,3,3,0,6,12,4,6,6,3,4,4,0,6,8,6,7,8,0,4,6,4,5,5,6,6,6,0,9,10,0,4,13,4,4,5,6,6,9,5,6,6,5,8,8,1,9,10,2,2,10,3,3,12,6,7,7,0,7,7,1,9,9,});
sparse_graph4_ints[34][35] = vector<int>({0,0,12,0,0,0,8,9,9,0,3,3,0,3,16,5,5,5,0,0,8,0,11,12,3,3,4,0,0,15,0,1,1,6,7,7,0,0,17,0,0,4,0,8,8,4,5,13,0,0,10,0,3,12,0,6,6,0,0,6,2,3,3,0,9,10,0,5,15,1,1,2,6,6,6,4,4,4,0,3,10,0,5,12,8,8,9,4,5,5,0,0,13,0,5,5,0,7,14,6,6,7,7,9,9,1,11,11,5,5,6,2,2,3,0,0,11,0,0,9,3,3,3,0,6,7,0,0,14,0,4,12,0,1,17,4,4,5,0,8,9,7,7,7,5,6,6,3,4,14,});
sparse_graph4_ints[34][40] = vector<int>({5,5,5,0,0,0,10,10,10,2,2,2,0,4,18,0,0,12,0,0,16,0,10,10,2,4,4,1,13,13,0,0,2,0,0,20,0,0,8,5,5,15,0,4,12,0,0,4,8,8,8,6,6,6,0,0,10,0,6,14,0,6,6,0,0,14,0,0,6,0,0,18,0,4,4,0,8,8,7,7,7,0,8,16,0,4,14,0,4,10,0,2,2,0,11,11,4,4,4,3,3,3,0,2,10,6,6,14,9,10,10,0,12,14,3,3,17,1,1,19,4,4,12,0,2,16,0,8,10,4,5,5,5,6,6,7,8,8,6,7,7,2,2,12,9,9,11,1,3,3,});
sparse_graph4_ints[34][45] = vector<int>({0,15,15,4,4,4,0,0,0,0,0,18,0,0,6,8,8,8,0,5,20,6,6,6,0,0,10,11,11,11,0,3,12,0,0,22,0,1,1,0,12,12,2,3,3,0,4,14,7,7,15,0,0,12,0,7,16,0,0,4,0,0,16,0,10,11,5,5,5,0,0,8,0,10,17,0,0,20,9,9,10,0,0,14,0,3,3,0,8,8,3,3,4,0,3,18,4,4,13,7,7,8,4,5,18,0,3,10,0,9,10,1,1,2,0,6,6,0,3,21,5,5,14,0,13,13,6,7,7,0,4,16,0,5,12,8,9,9,5,13,13,10,11,12,4,5,5,5,6,6,});
sparse_graph4_ints[34][50] = vector<int>({0,0,24,12,12,13,0,0,0,5,5,5,2,2,2,8,8,8,0,0,18,0,14,14,0,0,14,2,4,4,2,4,22,0,4,18,0,10,10,0,0,2,0,16,17,0,0,8,6,6,19,0,0,12,0,4,14,10,10,11,0,0,4,0,0,22,0,12,12,6,6,6,4,4,18,0,0,10,0,6,6,4,4,14,0,0,16,9,9,9,0,0,20,0,12,19,0,0,6,0,4,10,0,4,4,7,7,7,0,8,8,8,8,17,0,2,24,6,6,14,0,4,20,5,15,15,0,4,12,0,8,16,0,4,16,0,8,21,0,2,2,0,10,17,4,4,4,7,7,15,});
sparse_graph4_ints[32][6] = vector<int>({0,2,2,0,0,0,1,1,2,0,0,2,0,1,2,0,0,3,1,1,1,0,1,1,0,0,1,});
sparse_graph4_ints[32][8] = vector<int>({2,2,2,0,0,0,0,0,4,0,0,2,0,2,3,1,2,2,1,1,3,0,2,2,1,1,2,0,1,3,0,0,1,0,0,3,0,1,1,1,1,1,0,1,2,});
sparse_graph4_ints[32][10] = vector<int>({2,2,3,0,0,0,0,0,4,0,0,2,1,3,3,0,3,3,2,2,2,0,1,1,0,0,5,1,2,3,0,2,4,1,1,1,1,1,4,0,2,3,0,1,4,1,2,2,0,2,2,1,1,3,0,1,2,0,0,3,0,0,1,0,1,3,1,1,2,});
sparse_graph4_ints[32][12] = vector<int>({3,3,3,0,0,0,0,0,6,0,0,2,0,2,2,0,4,4,0,0,4,2,3,3,1,3,4,1,1,1,2,2,4,0,3,4,1,3,3,2,2,3,1,2,4,0,1,1,2,2,2,0,2,5,0,3,3,0,1,2,1,1,5,1,1,2,0,0,5,0,1,5,1,2,3,0,2,4,0,0,3,0,2,3,1,1,4,1,2,2,1,1,3,0,1,3,0,1,4,0,0,1,});
sparse_graph4_ints[32][14] = vector<int>({3,3,4,0,0,0,0,2,2,0,0,6,0,0,2,0,0,4,2,4,4,0,0,7,2,2,2,0,4,5,3,3,3,1,4,4,1,1,1,2,3,4,0,4,4,0,2,3,1,3,5,2,2,5,0,2,6,1,2,2,0,1,1,1,1,3,2,3,3,1,3,4,0,3,5,2,2,4,0,1,6,0,3,3,1,1,2,1,2,5,1,3,3,1,1,6,0,0,5,2,2,3,0,3,4,1,2,3,0,1,2,0,2,4,0,1,3,1,1,5,0,0,3,1,2,4,0,2,5,0,1,4,1,1,4,0,1,5,0,0,1,});
sparse_graph4_ints[32][16] = vector<int>({4,4,4,0,0,0,0,0,8,2,2,2,0,0,2,0,0,4,0,4,6,0,0,6,0,2,2,3,4,4,1,3,3,1,5,5,3,3,5,0,5,5,2,4,5,0,3,3,1,1,1,1,1,7,3,3,4,2,4,4,0,4,4,0,2,3,1,4,5,2,3,5,3,3,3,2,2,3,0,4,5,1,3,6,2,2,6,1,2,3,1,2,2,0,1,1,1,1,3,1,4,4,2,3,4,2,3,3,0,2,7,0,2,4,0,1,7,0,0,7,0,3,6,0,3,4,0,0,5,1,3,5,2,2,4,2,2,5,1,1,2,1,2,6,1,3,4,0,2,6,});
sparse_graph4_ints[32][18] = vector<int>({0,0,0,4,4,5,0,0,8,2,2,2,0,0,2,0,0,4,0,6,6,0,0,6,0,4,4,0,2,8,0,2,2,3,5,5,1,3,3,0,0,9,4,4,4,3,3,3,0,3,3,1,1,1,3,4,5,2,5,5,2,3,3,1,5,6,0,5,6,3,3,6,2,4,6,2,2,4,1,5,5,3,4,4,0,1,8,2,2,3,3,3,5,0,4,7,2,4,5,0,2,3,0,3,4,0,4,6,1,3,7,1,4,6,3,3,4,0,5,5,1,3,4,1,1,8,2,4,4,2,3,6,0,0,7,0,2,4,2,3,4,1,4,4,2,2,7,0,3,7,});
sparse_graph4_ints[32][20] = vector<int>({0,0,0,5,5,5,0,0,6,0,0,10,0,0,2,3,3,3,2,2,2,0,0,4,0,6,6,0,0,8,1,1,9,0,2,2,0,4,4,4,5,5,4,4,6,0,6,7,2,4,4,1,3,3,0,4,8,3,5,6,0,4,5,4,4,5,2,6,6,0,2,9,3,5,5,3,3,4,0,3,3,2,3,3,1,6,6,4,4,4,1,5,7,0,5,5,2,2,8,3,4,6,2,5,6,2,2,4,1,1,1,1,4,4,0,5,7,2,3,4,3,3,7,3,4,5,0,4,6,2,4,7,3,4,4,2,2,3,0,2,3,0,3,8,1,5,6,1,3,4,});
sparse_graph4_ints[32][23] = vector<int>({0,0,11,0,0,0,5,6,6,3,3,3,0,1,2,0,0,8,0,3,3,0,3,10,0,0,6,0,7,8,0,5,6,0,0,4,0,0,1,4,4,4,0,4,4,5,5,6,0,0,9,0,2,3,0,0,3,2,2,3,3,3,4,0,0,7,1,2,2,0,0,5,0,1,1,2,3,3,1,2,10,3,4,4,4,6,6,0,1,11,0,0,10,0,4,5,0,6,6,1,7,7,0,3,4,0,5,5,5,5,5,1,1,2,2,2,2,2,3,4,4,5,7,3,4,5,0,2,10,0,6,8,0,1,8,2,4,4,1,3,3,1,3,9,1,4,4,4,4,5,});
sparse_graph4_ints[32][26] = vector<int>({0,0,12,0,0,0,6,6,7,0,0,4,0,0,8,0,6,6,3,3,3,1,3,11,0,0,2,2,2,2,0,0,10,0,8,8,0,0,6,4,4,4,0,2,2,5,5,5,0,4,4,0,2,8,0,2,12,5,7,7,0,0,13,0,2,10,2,4,10,0,8,9,6,6,6,4,4,5,2,4,4,1,3,3,0,6,8,0,4,11,3,4,4,4,5,5,5,6,7,0,4,6,0,4,8,3,5,5,0,6,10,2,8,8,4,4,6,2,2,11,5,5,8,4,7,7,1,8,8,5,6,6,0,7,7,3,3,5,5,5,7,1,7,9,4,6,8,0,6,7,});
sparse_graph4_ints[32][30] = vector<int>({0,0,0,0,10,10,3,3,3,0,0,15,0,0,4,5,5,5,0,0,12,0,0,8,7,7,8,0,0,2,1,3,13,2,2,2,0,6,6,0,0,10,0,0,6,0,8,8,2,2,10,0,6,12,0,2,2,4,4,4,6,6,6,0,4,4,0,2,8,0,0,14,0,2,10,6,8,8,0,2,14,4,4,11,2,4,4,0,6,8,5,5,6,4,4,5,0,4,10,7,7,7,1,1,1,0,3,3,2,4,12,3,4,4,2,2,4,0,4,6,4,5,5,0,2,12,5,6,6,0,0,1,0,4,8,3,5,5,3,3,4,1,3,3,4,6,6,0,7,7,});
sparse_graph4_ints[32][35] = vector<int>({8,9,9,0,0,6,0,0,17,0,0,0,5,5,5,0,0,10,0,7,14,3,4,4,0,1,1,0,3,12,0,11,12,2,3,3,0,0,4,7,7,7,0,0,12,0,8,8,0,0,8,0,0,15,0,3,16,0,3,3,0,6,12,0,3,10,6,6,6,0,10,10,0,6,6,1,1,2,5,6,12,0,4,5,0,0,13,0,0,1,0,0,3,4,4,5,0,2,9,3,4,14,3,3,11,0,3,14,0,9,9,0,7,7,3,3,4,0,0,9,0,0,7,2,2,3,1,2,2,0,0,11,0,0,14,5,5,6,0,4,10,6,6,7,8,8,9,0,0,5,});
sparse_graph4_ints[32][40] = vector<int>({5,5,5,0,0,0,10,10,10,0,4,18,2,2,2,0,0,12,0,10,10,0,0,16,2,4,4,0,12,14,0,0,2,0,0,8,0,0,20,8,8,8,3,3,11,0,0,4,0,8,16,0,0,10,0,0,14,0,6,6,6,6,6,4,4,12,0,0,6,0,0,18,2,2,10,6,6,14,0,4,4,0,3,16,0,8,8,7,7,7,1,11,11,0,2,14,1,1,1,4,12,12,4,4,16,4,4,4,0,0,1,0,2,10,0,8,14,0,2,2,0,4,14,0,8,10,0,2,8,0,6,8,0,6,14,0,2,12,1,1,19,3,3,3,0,4,12,5,5,13,});
sparse_graph4_ints[32][45] = vector<int>({0,15,15,1,2,2,6,6,6,0,0,18,0,0,0,0,0,12,3,4,19,8,8,8,0,0,8,0,0,22,11,11,11,4,4,5,0,7,14,2,2,12,0,1,2,3,3,3,0,13,13,7,8,15,0,0,16,0,0,10,0,4,18,0,0,6,0,11,11,0,8,8,0,0,14,9,9,10,0,0,1,0,8,16,0,0,20,0,9,10,2,2,3,0,11,17,0,3,15,3,4,4,7,7,7,0,4,10,0,3,21,5,5,5,6,6,13,0,0,4,5,6,17,6,13,13,1,2,9,4,4,12,0,3,17,0,0,2,0,12,12,4,4,15,0,6,12,0,8,18,});
sparse_graph4_ints[32][50] = vector<int>({12,12,13,0,0,25,0,8,8,0,0,0,1,2,2,0,0,18,0,0,10,9,9,9,1,7,21,0,16,17,0,0,14,7,7,7,3,3,3,0,0,22,5,5,15,3,5,5,1,1,1,0,4,14,0,14,15,0,4,18,1,11,11,0,0,8,10,10,11,0,0,12,0,12,19,0,0,6,0,0,16,0,0,1,6,6,6,0,0,20,1,3,23,9,9,16,0,8,18,1,5,5,0,10,10,0,13,13,1,3,3,7,7,15,0,4,10,3,5,13,0,4,20,8,8,8,0,4,12,6,6,19,0,0,4,5,15,15,5,5,5,0,4,16,0,0,2,0,8,15,});
sparse_graph4_ints[30][6] = vector<int>({0,2,2,0,0,0,0,0,2,1,1,2,0,1,2,0,0,3,1,1,1,0,1,1,0,0,1,});
sparse_graph4_ints[30][8] = vector<int>({2,2,2,0,0,0,0,0,4,0,0,2,0,2,3,1,2,2,1,1,3,0,1,1,0,2,2,1,1,2,0,1,3,0,0,1,0,0,3,1,1,1,0,1,2,});
sparse_graph4_ints[30][10] = vector<int>({1,3,3,0,0,0,0,0,4,0,0,2,2,2,3,1,1,1,0,3,3,0,0,5,2,2,2,1,2,3,0,1,1,0,2,4,0,2,2,1,1,4,0,2,3,0,1,4,1,2,2,0,1,2,1,1,3,0,0,3,0,0,1,1,1,2,0,1,3,});
sparse_graph4_ints[30][12] = vector<int>({3,3,3,0,0,0,0,0,6,0,0,2,0,2,2,0,4,4,0,0,4,2,3,3,1,1,1,2,2,4,1,3,4,2,2,2,0,3,4,1,3,3,2,2,3,0,1,1,1,1,2,1,2,4,0,3,3,0,2,5,1,1,5,0,2,3,1,2,2,0,1,2,0,0,5,1,1,3,0,1,5,0,0,3,0,2,4,1,2,3,0,1,3,0,0,1,1,1,4,0,1,4,});
sparse_graph4_ints[30][14] = vector<int>({3,3,4,0,0,0,0,0,6,0,0,2,0,0,4,2,2,2,0,2,2,0,0,7,2,4,4,0,4,5,3,3,3,0,4,4,0,3,3,1,1,1,0,2,6,2,3,4,1,4,4,0,2,3,1,3,5,2,2,5,1,2,2,0,1,1,1,1,3,2,3,3,1,3,3,0,1,6,1,3,4,0,3,5,0,0,5,2,2,4,1,1,2,0,2,4,1,1,6,2,2,3,1,2,5,1,2,3,0,3,4,1,1,5,0,1,3,1,2,4,0,0,3,0,1,2,0,1,4,0,0,1,0,2,5,1,1,4,0,1,5,});
sparse_graph4_ints[30][16] = vector<int>({4,4,4,0,0,0,0,0,4,0,0,8,0,0,2,1,3,3,0,0,6,0,2,2,0,4,6,2,2,2,3,4,4,1,1,7,1,5,5,0,4,4,3,3,5,0,3,3,1,1,1,0,5,5,2,4,5,3,3,3,3,3,4,0,2,3,2,2,3,2,4,4,1,4,5,0,2,7,0,3,4,2,3,5,2,2,6,2,3,3,0,4,5,1,2,2,2,2,4,1,1,3,0,1,1,1,2,3,0,0,5,1,3,6,0,2,4,0,3,6,2,3,4,1,4,4,1,3,4,1,3,5,0,1,7,0,0,7,0,2,6,1,2,4,1,2,6,1,1,2,});
sparse_graph4_ints[30][18] = vector<int>({0,0,0,4,4,5,0,0,4,0,0,8,0,4,4,0,0,2,0,0,6,2,2,2,0,6,6,0,2,8,0,2,2,3,3,3,3,5,5,0,0,9,1,3,3,4,4,4,1,1,1,0,3,3,2,3,3,1,3,7,3,4,5,2,2,4,0,5,6,2,5,5,1,1,8,1,5,6,0,4,5,1,2,2,0,0,7,3,4,4,2,2,3,3,3,6,2,4,6,2,3,4,0,4,6,1,5,5,3,3,5,0,1,8,2,4,4,1,3,4,0,2,4,0,4,7,3,3,4,0,2,6,2,4,5,0,3,4,0,5,5,1,4,4,0,3,7,1,4,6,});
sparse_graph4_ints[30][20] = vector<int>({0,0,0,5,5,5,0,0,6,0,0,10,0,0,2,3,3,3,0,0,4,2,2,2,0,6,6,0,0,8,1,1,9,0,2,2,0,4,4,2,4,4,2,2,8,4,5,5,1,3,3,0,6,7,0,5,5,4,4,6,1,1,1,0,4,8,1,1,7,0,3,3,3,3,4,3,5,6,4,4,4,2,3,3,2,6,6,0,0,1,4,4,5,0,4,6,2,2,4,1,2,2,0,2,9,0,0,7,0,4,5,1,6,6,3,4,4,3,5,5,1,5,7,2,3,4,2,2,3,3,3,5,0,3,8,0,2,6,1,4,4,3,4,6,3,3,7,0,2,4,});
sparse_graph4_ints[30][23] = vector<int>({0,0,11,0,0,0,5,6,6,0,0,4,2,3,3,0,0,8,4,4,4,0,0,2,0,3,10,0,0,6,0,7,8,0,3,3,3,3,4,1,1,2,0,0,9,0,6,6,0,0,1,0,4,5,0,0,3,0,2,8,0,0,7,2,2,3,0,0,5,0,2,3,5,5,6,2,3,9,0,1,11,0,0,10,3,4,4,0,4,4,0,1,2,2,2,2,3,3,3,0,1,1,0,5,6,4,4,5,1,2,10,1,2,2,0,2,7,0,3,4,4,6,6,1,7,7,0,5,5,0,3,8,0,2,2,2,3,4,3,4,5,5,5,5,0,5,9,1,3,3,});
sparse_graph4_ints[30][26] = vector<int>({0,0,0,6,6,7,0,0,8,0,0,13,0,0,4,0,6,6,3,3,3,0,0,2,0,0,10,2,2,2,1,3,11,0,8,9,0,0,6,5,5,5,0,2,10,0,2,2,4,4,4,0,2,8,0,4,4,0,0,12,0,0,1,1,1,1,2,4,10,0,7,7,0,2,12,2,4,4,0,4,8,5,7,7,0,6,10,1,3,3,4,5,5,0,4,6,6,6,6,0,8,8,1,3,9,4,4,5,3,4,4,2,8,8,3,3,10,1,2,2,0,0,7,1,1,7,2,3,3,0,0,5,0,6,8,1,1,9,0,4,11,4,4,6,5,6,7,3,3,5,});
sparse_graph4_ints[30][30] = vector<int>({0,0,0,7,7,8,3,3,3,0,0,15,0,0,4,5,5,5,0,0,12,0,0,8,0,6,12,0,0,2,2,2,2,0,6,6,0,10,10,0,0,10,0,4,10,0,0,6,0,8,8,6,6,6,1,3,13,0,2,8,4,4,4,1,1,1,0,4,4,0,0,1,0,2,10,0,0,14,0,2,2,4,4,11,0,2,12,0,5,5,0,7,7,0,2,14,3,3,9,1,3,3,2,2,8,0,0,7,0,0,5,1,2,2,4,4,5,3,3,4,1,1,7,5,6,6,7,7,7,3,9,9,0,6,10,2,4,12,0,8,10,6,8,8,5,5,10,5,5,6,});
sparse_graph4_ints[30][35] = vector<int>({0,0,0,8,9,9,0,0,17,0,0,6,0,8,8,0,0,10,0,0,3,3,4,4,0,0,14,0,11,12,0,6,12,0,4,4,0,3,10,7,7,7,4,5,13,0,0,8,0,3,16,0,0,1,4,4,5,0,3,3,0,0,12,5,6,6,0,10,10,0,2,2,3,3,10,0,3,14,0,0,5,0,7,7,0,1,1,0,3,8,3,3,3,0,0,15,0,8,13,0,9,9,5,5,5,0,5,15,6,6,7,1,2,11,0,0,9,0,0,7,0,6,6,6,6,11,0,0,4,4,4,11,0,4,12,0,0,11,0,0,13,2,2,3,4,10,10,0,5,5,});
sparse_graph4_ints[30][40] = vector<int>({10,10,10,0,0,0,0,0,16,3,3,3,6,6,6,3,3,17,0,0,12,1,1,1,0,0,20,0,0,6,1,13,13,4,4,5,8,8,8,0,10,10,0,4,14,0,0,10,1,3,11,0,0,8,1,2,2,5,5,15,0,0,14,0,0,1,0,0,18,0,8,8,0,8,14,1,3,3,3,4,4,7,7,7,0,4,16,4,4,12,0,0,4,7,7,13,5,5,5,0,11,11,0,3,10,0,10,15,0,0,2,3,3,11,1,1,19,1,1,9,0,8,10,0,2,14,0,6,6,0,12,12,5,5,13,0,6,8,4,12,12,0,8,16,0,4,6,0,4,4,});
sparse_graph4_ints[30][45] = vector<int>({0,0,0,0,0,10,11,11,11,0,0,18,7,7,7,1,2,2,0,7,19,0,0,14,0,0,22,0,8,8,0,15,15,0,12,12,3,3,3,4,4,5,0,0,3,9,9,10,0,4,18,0,0,8,0,8,14,0,0,12,2,2,12,7,8,15,2,2,3,0,9,10,0,0,16,0,0,1,8,8,8,0,11,17,3,4,4,0,0,20,1,2,15,0,0,6,6,6,6,0,3,21,0,4,10,6,7,13,4,4,12,5,5,5,4,4,16,1,13,13,0,4,16,6,13,13,0,0,4,0,1,1,0,10,11,0,3,9,5,6,17,0,10,15,0,6,12,5,5,13,});
sparse_graph4_ints[30][50] = vector<int>({12,12,13,7,7,7,0,0,12,0,0,0,0,0,25,1,2,2,0,0,20,4,4,18,0,16,16,3,3,3,0,0,16,9,9,10,0,8,8,3,5,5,1,1,1,0,12,13,8,8,17,4,4,14,0,0,10,0,4,14,4,6,20,0,0,8,0,0,22,0,0,14,0,14,14,0,4,23,1,3,3,8,8,8,0,0,1,0,0,18,6,6,6,1,5,5,0,4,18,0,12,19,10,11,11,0,11,11,7,7,15,0,4,10,6,14,15,0,0,6,0,9,10,0,8,16,0,3,21,0,4,12,0,4,16,0,10,18,0,0,4,5,5,5,6,6,14,0,0,2,});
sparse_graph4_ints[25][6] = vector<int>({0,2,2,0,0,0,0,0,2,1,1,2,0,0,3,0,0,1,0,1,2,1,1,1,0,1,1,});
sparse_graph4_ints[25][8] = vector<int>({2,2,2,0,0,0,0,0,2,0,0,4,0,2,3,0,0,1,1,1,1,1,2,2,1,1,3,0,2,2,0,1,1,0,0,3,1,1,2,0,1,3,0,1,2,});
sparse_graph4_ints[25][10] = vector<int>({1,3,3,0,0,0,0,0,2,0,0,4,0,2,2,0,0,5,2,2,3,1,1,1,0,0,1,0,2,4,2,2,2,0,3,3,0,0,3,1,2,3,0,1,1,1,1,2,0,1,2,1,1,4,0,2,3,1,2,2,0,1,4,1,1,3,0,1,3,});
sparse_graph4_ints[25][12] = vector<int>({0,0,0,3,3,3,0,0,6,0,0,2,0,2,2,0,0,4,0,4,4,2,2,2,0,0,1,1,1,1,1,1,5,1,1,3,0,0,3,2,3,3,1,2,2,2,2,4,0,0,5,0,3,3,1,3,4,0,2,3,0,2,5,1,3,3,0,3,4,0,2,4,2,2,3,1,1,2,0,1,1,0,1,2,1,2,4,1,2,3,0,1,3,0,1,5,0,1,4,1,1,4,});
sparse_graph4_ints[25][14] = vector<int>({3,3,4,0,0,0,0,0,4,0,0,7,0,0,2,2,2,2,0,2,2,0,0,6,0,4,4,0,0,1,1,1,1,0,2,6,0,0,5,2,4,4,1,3,3,1,2,2,0,4,5,3,3,3,0,2,4,0,0,3,0,3,3,1,1,3,2,2,3,1,3,5,1,1,6,2,3,4,1,4,4,1,1,5,2,3,3,0,2,3,0,3,4,2,2,5,2,2,4,1,2,3,0,3,5,0,1,6,1,1,2,0,1,1,1,3,4,1,2,5,1,2,4,0,1,4,0,1,2,0,1,5,1,1,4,0,2,5,0,1,3,});
sparse_graph4_ints[25][16] = vector<int>({0,0,8,0,0,0,4,4,4,2,2,2,0,0,2,0,0,4,0,0,6,0,4,4,0,4,6,0,2,2,0,0,1,1,1,7,1,1,1,3,3,3,0,0,5,1,3,3,1,5,5,1,1,5,1,2,2,2,2,6,0,0,7,0,2,4,0,0,3,2,3,3,0,2,6,3,4,4,1,1,3,0,3,3,2,2,3,3,3,5,0,5,5,0,2,7,2,2,4,0,3,4,0,2,3,0,1,1,1,1,2,3,3,4,2,4,5,0,4,5,2,4,4,0,3,6,2,3,4,1,2,3,1,3,4,0,2,5,0,3,5,1,2,4,1,4,4,2,3,5,});
sparse_graph4_ints[25][18] = vector<int>({0,0,8,0,0,0,4,4,5,1,1,1,0,4,4,0,0,6,2,2,2,0,2,8,0,0,1,0,0,4,0,6,6,0,0,2,3,3,3,0,2,2,0,2,6,0,0,9,1,3,7,0,0,5,1,3,3,0,0,7,1,2,2,0,4,6,4,4,4,3,3,4,1,1,5,0,0,3,0,2,4,2,3,3,1,1,7,3,5,5,0,4,5,2,4,4,1,1,3,2,2,3,2,2,7,0,3,3,2,2,4,0,5,5,0,4,7,3,4,4,1,3,5,1,1,8,2,2,6,3,3,6,3,4,5,1,5,5,2,3,4,0,3,4,3,3,5,0,3,6,});
sparse_graph4_ints[25][20] = vector<int>({5,5,5,0,0,0,2,2,2,0,0,10,1,1,1,0,4,6,0,0,8,0,0,6,0,0,1,0,0,4,0,4,4,0,6,7,1,1,9,0,0,2,4,4,4,0,2,2,3,3,3,0,4,8,1,1,7,0,0,7,0,0,5,1,3,3,1,2,2,3,3,7,1,5,5,0,2,6,0,0,9,3,3,4,2,2,8,0,6,6,0,2,8,1,1,5,2,6,6,2,2,6,0,2,4,2,3,3,0,0,3,0,4,5,3,4,4,4,4,6,0,5,5,1,1,3,2,2,3,2,4,4,2,2,4,0,3,3,3,3,5,4,5,5,1,3,7,0,1,1,});
sparse_graph4_ints[25][23] = vector<int>({0,0,0,0,0,8,5,6,6,0,2,2,0,3,10,3,3,3,0,1,1,0,0,6,0,0,11,4,4,4,0,7,8,2,2,2,0,2,8,0,0,1,0,6,6,0,0,5,0,0,3,0,2,6,0,0,10,0,5,5,0,5,9,2,2,3,0,0,4,0,0,7,4,5,5,0,0,9,0,4,8,0,4,4,0,1,11,1,2,2,2,2,7,3,4,8,0,2,7,3,3,4,0,2,9,0,6,7,2,3,9,2,7,7,0,5,6,4,4,5,3,4,4,2,3,3,0,4,5,0,2,4,2,2,8,0,3,4,1,1,1,0,0,2,5,5,5,0,3,7,});
sparse_graph4_ints[25][26] = vector<int>({2,8,8,0,0,6,0,0,0,0,0,10,4,4,4,1,1,1,1,3,11,0,4,4,0,0,12,0,0,8,5,5,5,0,0,1,0,0,4,2,2,2,0,4,6,6,6,7,0,6,8,1,1,9,0,0,2,0,6,10,1,3,9,0,0,13,0,6,6,0,0,5,3,3,3,0,0,7,1,1,7,4,4,9,0,2,2,1,3,3,0,2,12,0,0,11,0,0,9,0,8,8,1,2,2,0,2,10,3,3,7,3,3,4,0,2,8,0,4,8,0,2,6,3,3,9,1,7,7,1,5,5,4,5,5,4,6,6,0,4,10,1,3,7,1,1,11,0,6,7,});
sparse_graph4_ints[25][30] = vector<int>({0,0,0,7,7,8,0,0,6,0,0,15,0,0,10,1,1,1,3,3,9,3,4,4,0,6,12,0,4,4,0,0,12,0,10,10,0,0,8,0,0,1,5,5,7,0,0,4,4,4,5,0,8,8,2,2,2,1,3,13,2,2,8,0,0,2,1,1,11,5,5,10,3,3,3,0,2,8,0,7,7,0,0,5,0,0,14,0,3,12,5,5,5,0,0,7,0,6,6,1,3,3,0,9,9,0,6,10,0,0,9,0,0,11,6,6,7,0,5,5,0,2,14,0,4,10,3,9,9,0,2,2,0,0,13,4,4,10,1,2,2,1,1,9,1,7,11,1,1,7,});
sparse_graph4_ints[25][35] = vector<int>({8,9,9,0,0,17,0,0,0,5,5,5,0,0,5,0,0,12,3,4,14,0,0,8,3,4,4,0,0,3,6,6,7,0,11,12,0,3,13,0,0,10,3,3,3,0,10,10,0,0,1,0,8,8,0,3,10,2,2,2,0,0,15,0,3,16,7,7,8,5,6,12,0,1,1,0,6,11,0,0,7,4,4,5,1,1,2,0,2,12,0,6,13,0,4,4,0,5,10,0,3,8,0,9,9,0,8,13,0,7,7,5,6,6,4,10,10,5,5,10,0,0,9,0,0,6,0,2,15,0,6,6,0,4,6,0,0,14,0,0,11,2,2,3,3,3,12,0,0,13,});
sparse_graph4_ints[25][40] = vector<int>({1,2,2,1,13,13,0,0,0,4,4,4,0,0,16,1,5,17,7,7,8,0,0,8,0,0,20,10,10,10,0,0,12,3,3,11,1,1,1,5,5,5,0,10,10,0,0,10,7,7,13,0,0,14,1,3,3,0,8,14,0,0,18,0,0,1,2,4,4,6,6,7,0,6,13,0,3,10,0,12,12,0,8,8,8,8,9,0,0,6,5,5,15,0,0,2,0,3,15,0,0,4,1,3,11,3,3,15,0,10,15,1,1,19,5,5,11,0,4,4,1,1,9,0,7,7,0,9,9,6,6,12,0,2,14,6,11,11,0,11,11,1,1,17,1,1,7,5,6,6,});
sparse_graph4_ints[25][45] = vector<int>({0,0,15,0,0,22,1,2,2,0,0,0,11,11,11,0,0,10,0,12,12,3,4,13,0,8,8,3,4,19,0,15,15,0,0,18,4,4,5,8,9,9,3,3,3,0,0,3,7,7,7,7,8,15,0,0,13,0,6,17,1,3,3,0,11,17,6,6,6,0,0,8,0,0,20,1,5,5,6,6,14,0,0,1,1,1,15,0,10,11,0,4,10,6,6,11,9,10,10,2,2,17,1,13,13,0,3,21,6,13,13,0,4,12,0,0,6,0,1,1,0,2,10,1,9,9,0,9,16,5,5,5,0,0,4,0,8,14,0,6,12,5,6,17,0,3,16,0,2,20,});
sparse_graph4_ints[25][50] = vector<int>({0,8,16,0,0,4,0,16,17,0,0,18,3,7,7,1,5,5,0,0,0,10,10,10,0,8,21,0,0,25,0,0,14,0,0,8,12,12,13,0,14,14,8,8,8,0,0,22,0,0,2,0,0,6,0,0,12,0,11,11,9,9,16,2,2,18,7,7,15,0,4,14,3,4,19,0,0,10,0,12,19,0,5,8,0,0,16,7,7,7,0,0,20,1,3,23,0,9,10,3,3,3,0,12,13,5,5,5,9,9,9,5,13,13,0,4,16,0,10,17,0,4,12,6,6,19,4,4,14,6,6,14,1,3,3,7,14,14,0,3,21,0,0,1,11,11,11,1,1,1,});
sparse_graph4_ints[20][6] = vector<int>({0,2,2,0,0,0,0,0,2,0,0,1,0,0,3,1,1,2,1,1,1,0,1,2,0,1,1,});
sparse_graph4_ints[20][8] = vector<int>({2,2,2,0,0,0,0,0,2,0,0,4,0,0,1,1,1,1,0,2,2,0,2,3,0,0,3,1,1,3,0,1,1,1,2,2,0,1,2,0,1,3,1,1,2,});
sparse_graph4_ints[20][10] = vector<int>({2,2,3,0,0,0,0,0,2,0,0,5,0,2,2,0,0,1,0,0,4,1,1,1,0,2,4,1,3,3,0,0,3,1,1,3,2,2,2,0,3,3,1,2,2,0,1,1,1,1,2,0,1,2,0,2,3,1,1,4,0,1,3,0,1,4,1,2,3,});
sparse_graph4_ints[20][12] = vector<int>({0,4,4,0,0,0,1,1,1,0,0,6,0,0,4,0,0,1,2,2,2,0,0,2,3,3,3,0,2,2,1,1,5,0,0,5,1,1,3,0,0,3,0,2,4,1,3,3,2,2,4,1,2,2,0,1,1,0,3,3,1,1,2,0,1,4,0,2,5,0,2,3,2,3,3,0,1,2,0,1,3,2,2,3,0,1,5,0,3,4,1,2,3,1,3,4,1,2,4,1,1,4,});
sparse_graph4_ints[20][14] = vector<int>({3,3,4,0,0,0,1,1,1,0,0,6,2,2,2,0,0,1,0,0,4,0,2,6,0,0,2,0,2,2,0,4,4,0,0,7,0,0,5,1,3,3,1,1,3,0,4,5,0,2,4,1,1,5,0,0,3,3,3,3,2,2,4,2,4,4,1,3,5,0,1,1,1,2,2,2,2,3,0,3,3,0,1,5,2,3,3,0,3,4,1,1,6,2,2,5,0,1,4,1,1,2,0,2,3,0,1,3,0,1,2,0,2,5,0,3,5,1,2,3,0,1,6,1,2,4,1,4,4,2,3,4,1,2,5,1,1,4,1,3,4,});
sparse_graph4_ints[20][16] = vector<int>({0,0,0,0,0,8,4,4,4,1,1,1,2,2,2,0,0,6,0,4,4,0,0,1,0,0,4,1,1,7,0,0,2,1,5,5,0,2,2,3,3,3,1,1,5,0,0,5,2,2,6,0,0,7,1,2,2,1,3,3,0,2,6,0,4,6,0,2,4,0,0,3,2,2,4,1,3,5,3,3,5,2,4,4,0,1,1,1,1,3,0,4,5,2,3,3,0,3,3,2,2,3,0,1,5,0,1,6,0,5,5,0,3,4,0,2,7,0,3,5,3,3,4,0,1,4,1,1,2,0,3,6,0,1,7,2,2,5,3,4,4,1,4,4,1,2,5,0,2,3,});
sparse_graph4_ints[20][18] = vector<int>({0,0,0,0,6,6,0,0,6,1,1,1,3,3,3,0,0,8,2,2,2,0,0,1,0,0,4,0,2,8,0,0,2,0,2,6,3,5,5,0,0,5,1,3,3,0,0,9,0,4,6,0,2,2,0,0,7,0,4,4,2,2,7,1,2,2,3,3,4,1,1,7,1,1,5,4,4,4,1,5,5,0,2,4,0,4,5,2,3,3,0,4,7,2,2,6,0,0,3,3,3,5,3,3,6,0,5,5,4,4,5,0,3,6,0,1,1,2,4,4,2,2,5,3,4,4,2,4,6,0,1,7,2,2,4,1,3,7,0,1,6,2,2,3,0,1,5,1,1,3,});
sparse_graph4_ints[20][20] = vector<int>({0,0,0,5,5,5,0,0,4,0,0,10,0,4,4,0,0,6,0,0,8,0,0,1,0,6,6,1,1,1,2,2,2,0,4,6,1,1,9,0,0,2,4,4,4,0,4,8,1,1,7,0,0,5,3,3,3,0,0,7,1,3,3,0,2,2,3,3,7,0,0,9,1,2,2,2,6,6,0,2,6,1,5,5,0,2,8,3,3,4,1,1,5,2,2,8,0,6,7,2,2,6,0,4,5,0,2,4,2,3,3,3,3,5,0,4,7,4,4,6,3,5,5,0,0,3,2,2,7,0,5,5,2,4,6,0,3,6,1,5,7,3,4,4,0,3,7,0,1,1,});
sparse_graph4_ints[20][23] = vector<int>({0,0,6,0,3,10,0,0,0,5,6,6,3,3,3,0,1,1,0,2,2,0,0,8,4,4,4,0,0,10,0,7,8,0,3,3,0,0,1,0,2,8,0,0,3,0,0,5,0,6,6,0,1,11,0,2,6,3,4,8,0,5,5,2,2,3,0,0,7,0,4,8,0,0,9,5,5,5,2,2,7,0,0,4,0,0,11,0,5,9,1,2,2,2,3,3,3,3,7,0,2,9,1,1,7,2,6,6,3,4,4,2,7,7,0,4,5,0,6,7,4,4,5,2,2,6,0,5,6,2,3,9,3,3,4,1,1,5,1,2,9,0,4,4,4,5,5,3,3,6,});
sparse_graph4_ints[20][26] = vector<int>({0,0,0,2,8,8,1,3,3,0,0,13,1,1,1,0,0,10,3,3,4,5,5,5,0,0,8,1,3,11,0,6,6,1,2,2,0,0,1,0,0,6,0,2,10,6,6,7,0,6,8,0,0,2,0,0,4,0,6,10,0,4,4,0,2,8,3,3,7,1,1,7,0,0,7,0,0,12,4,4,4,2,2,2,0,2,12,3,5,9,3,3,3,1,5,5,0,0,11,1,7,7,3,3,9,0,4,8,0,0,9,1,1,9,0,4,10,5,6,6,0,0,5,1,1,11,0,8,8,4,5,5,0,2,2,0,2,6,1,3,9,1,3,7,4,4,8,0,4,6,});
sparse_graph4_ints[20][30] = vector<int>({3,4,4,0,0,0,0,10,10,0,2,14,1,1,3,0,0,8,0,0,12,5,6,6,0,4,10,7,7,8,1,1,1,0,6,12,0,0,10,1,3,3,0,0,1,0,7,7,3,3,3,0,0,6,0,0,14,0,0,2,0,8,8,5,5,10,0,0,4,0,6,6,0,2,10,0,5,11,5,5,5,0,2,8,0,4,4,0,9,9,4,4,5,1,2,12,5,7,7,0,0,9,2,2,2,0,0,7,1,3,13,0,0,15,0,5,5,3,3,9,3,9,9,0,0,11,1,1,11,1,7,11,2,3,8,4,4,10,0,3,8,0,0,13,4,4,8,0,7,10,});
sparse_graph4_ints[20][35] = vector<int>({0,5,10,0,0,0,2,2,2,0,1,17,3,4,4,0,11,12,7,7,7,0,0,8,0,0,13,0,1,1,8,9,9,4,5,13,0,0,10,5,6,6,1,1,2,2,3,13,0,10,10,0,1,14,3,3,3,1,3,9,0,5,15,0,0,1,0,7,7,0,0,16,0,0,3,0,9,9,0,0,5,6,7,11,0,0,7,5,5,10,0,6,11,0,6,6,0,8,8,0,1,11,2,3,3,5,5,5,0,8,13,4,9,9,0,7,12,0,2,12,0,0,9,0,4,6,2,2,12,0,2,15,5,10,10,0,2,9,4,4,5,0,3,16,6,6,7,4,4,10,});
sparse_graph4_ints[20][40] = vector<int>({3,3,17,3,3,3,1,9,9,10,10,10,0,0,16,0,0,0,0,12,12,0,0,6,0,0,20,0,0,13,5,5,6,7,8,8,0,0,2,0,0,10,3,5,5,0,0,4,0,8,14,1,2,16,0,0,8,5,5,15,5,5,11,5,11,12,1,5,5,2,2,10,4,4,10,0,0,1,1,3,3,7,7,13,0,0,18,0,3,14,1,1,1,2,10,10,1,1,3,6,7,7,0,3,9,2,10,14,0,1,14,6,6,12,0,6,13,9,9,9,1,4,11,3,3,15,0,7,8,0,2,19,0,8,16,5,10,10,1,13,13,0,0,11,0,8,9,2,4,4,});
sparse_graph4_ints[20][45] = vector<int>({0,0,3,7,7,7,0,0,8,0,5,20,11,11,11,0,5,13,0,0,0,0,0,20,9,9,9,0,7,15,0,0,15,7,8,15,0,8,8,0,13,13,1,6,6,0,15,15,0,0,10,0,1,22,1,2,2,0,11,17,0,0,13,0,11,11,6,6,14,0,0,18,6,6,6,3,4,4,8,8,8,2,2,16,1,3,3,0,9,10,4,5,18,6,13,13,0,0,1,0,9,16,3,3,3,0,4,10,0,3,19,0,1,1,2,2,12,5,11,12,5,6,12,10,10,10,1,1,2,0,2,10,5,5,5,4,4,12,0,1,16,0,0,6,4,10,11,3,4,17,});
sparse_graph4_ints[20][50] = vector<int>({0,0,3,3,3,3,0,0,25,8,14,14,0,8,21,0,0,15,8,8,8,0,0,21,1,6,6,10,10,10,0,0,0,0,10,10,0,15,15,0,8,16,6,6,6,0,0,12,0,5,14,7,7,15,2,12,18,0,0,5,4,5,13,4,12,12,1,3,23,0,0,10,0,0,17,1,3,19,0,8,8,8,9,16,2,16,16,11,11,12,1,11,11,6,6,14,1,3,3,9,9,9,0,0,19,3,5,19,7,7,7,0,0,1,0,11,17,6,6,19,1,3,17,3,3,5,0,0,23,0,5,12,6,13,13,0,3,11,0,0,8,0,12,13,1,3,21,0,0,2,});
sparse_graph4_ints[15][6] = vector<int>({0,2,2,0,0,0,0,0,2,0,0,1,0,0,3,1,1,1,1,1,2,0,1,1,0,1,2,});
sparse_graph4_ints[15][8] = vector<int>({2,2,2,0,0,0,0,0,4,0,0,1,1,1,1,0,0,2,0,2,2,1,1,3,0,0,3,0,1,1,0,2,3,0,1,2,0,1,3,1,2,2,1,1,2,});
sparse_graph4_ints[15][10] = vector<int>({1,3,3,0,0,0,1,1,1,0,0,5,0,0,1,0,0,4,0,0,2,0,2,2,0,2,4,1,1,3,2,2,2,0,0,3,0,1,1,2,2,3,1,2,2,0,3,3,0,1,3,0,1,4,1,1,4,1,1,2,0,2,3,0,1,2,1,2,3,});
sparse_graph4_ints[15][12] = vector<int>({0,0,0,3,3,3,0,0,4,0,0,6,0,0,1,1,1,1,2,2,2,0,0,2,1,1,5,0,4,4,0,2,2,0,0,5,1,1,3,0,2,4,1,3,3,0,0,3,2,2,4,0,1,1,1,2,2,0,3,3,0,1,4,0,1,5,0,1,3,0,2,5,2,2,3,0,3,4,2,3,3,1,1,2,0,2,3,1,1,4,0,1,2,1,2,3,1,3,4,1,2,4,});
sparse_graph4_ints[15][14] = vector<int>({3,3,3,0,0,0,0,0,4,0,0,7,2,2,2,0,0,1,1,1,1,0,0,2,0,0,5,0,2,6,0,4,4,0,0,6,0,2,2,1,1,5,1,2,2,2,4,4,2,2,3,0,2,4,0,4,5,2,2,4,2,2,5,0,0,3,0,3,4,3,3,4,0,1,5,2,3,3,0,1,4,0,3,3,1,3,5,0,1,1,1,3,3,0,3,5,1,1,3,0,1,6,0,1,3,1,1,6,0,2,5,1,4,4,1,1,2,1,1,4,0,2,3,0,1,2,1,2,4,1,2,5,2,3,4,1,3,4,1,2,3,});
sparse_graph4_ints[15][16] = vector<int>({1,1,1,1,5,5,0,0,0,0,0,8,2,2,2,0,0,1,0,0,6,3,3,3,0,0,4,0,0,2,1,1,7,4,4,4,1,1,5,0,0,5,0,4,4,2,2,6,0,2,2,0,0,7,1,2,2,0,2,6,2,2,3,0,4,6,0,2,4,0,4,5,3,3,5,2,4,4,2,3,3,2,2,5,0,0,3,0,3,4,0,3,5,0,3,6,0,5,5,0,1,6,0,1,5,0,1,7,3,3,4,2,2,4,0,3,3,0,1,4,0,1,1,1,3,5,1,3,3,1,4,4,3,4,4,1,1,3,2,3,5,0,1,3,0,2,7,1,2,5,});
sparse_graph4_ints[15][18] = vector<int>({3,5,5,0,0,0,0,0,4,0,0,8,2,2,3,0,0,1,0,0,6,0,5,5,1,1,1,0,2,8,0,0,2,1,2,2,0,2,6,3,3,3,0,6,6,0,0,9,2,2,2,0,0,7,0,4,7,0,4,4,0,0,5,0,2,2,1,1,7,3,3,5,2,2,6,4,4,4,1,1,5,0,4,5,3,3,4,2,4,6,0,2,4,2,2,7,2,3,3,1,5,5,0,4,6,0,3,6,4,4,5,0,1,7,3,4,4,2,2,5,3,3,6,0,0,3,0,3,7,0,3,4,1,2,5,0,1,6,2,4,4,0,1,8,0,1,5,2,5,5,});
sparse_graph4_ints[15][20] = vector<int>({0,0,10,5,5,5,0,0,0,0,0,6,1,1,1,3,3,3,0,4,8,0,0,8,0,0,1,1,2,2,1,5,5,1,3,3,0,0,2,0,0,4,2,6,6,1,1,7,1,1,9,4,4,4,0,4,4,2,2,2,0,0,7,3,3,7,0,0,9,3,3,4,0,2,6,0,2,8,0,0,5,0,6,6,2,2,7,0,2,2,2,2,6,0,4,5,0,6,7,1,1,5,3,5,5,4,4,6,0,5,5,3,3,5,2,2,8,2,4,6,2,3,3,0,1,5,0,4,7,1,2,6,0,4,6,1,5,7,0,1,8,0,1,9,0,3,7,0,1,7,});
sparse_graph4_ints[15][23] = vector<int>({0,0,0,0,1,11,3,3,3,5,6,6,0,0,6,0,1,1,0,6,6,0,0,9,0,0,4,0,7,8,0,0,1,1,3,3,0,0,3,4,4,5,0,2,8,0,2,6,0,0,5,0,3,10,3,4,4,0,0,10,3,4,8,2,2,2,0,0,7,0,5,5,0,0,8,0,2,9,2,6,6,2,3,8,2,3,3,2,2,7,5,5,5,0,0,11,1,1,7,2,3,9,4,4,4,1,2,2,2,7,7,2,3,7,1,2,9,0,6,7,0,2,2,2,2,6,0,4,5,0,5,6,3,4,7,0,6,8,4,5,7,3,3,4,4,5,5,1,1,5,});
sparse_graph4_ints[15][26] = vector<int>({0,0,6,0,6,6,1,1,1,0,0,0,0,0,11,6,6,7,3,3,3,1,3,11,0,8,8,0,0,8,5,5,5,0,0,13,0,0,1,1,1,3,0,2,8,1,3,9,1,3,3,0,0,2,0,0,4,0,6,8,3,4,4,4,4,8,0,6,10,2,3,3,0,0,9,4,4,5,2,2,2,0,5,5,0,0,7,0,2,12,0,2,10,3,7,8,0,0,10,0,0,12,0,4,8,1,1,9,4,4,4,1,7,7,5,6,6,1,1,7,1,1,11,0,4,10,1,3,7,3,3,8,3,5,9,1,5,9,4,5,5,1,5,7,0,2,6,3,3,10,});
sparse_graph4_ints[15][30] = vector<int>({1,1,1,0,6,6,1,3,13,0,0,10,0,0,0,0,8,8,3,3,3,2,2,10,3,9,9,6,6,6,0,0,14,0,0,8,1,1,3,5,5,5,0,0,1,4,4,11,1,3,3,7,7,8,0,0,2,0,0,6,0,0,4,0,1,12,0,4,6,0,9,9,0,2,10,0,7,7,4,4,9,0,3,12,0,6,12,0,4,4,4,4,5,0,1,8,5,6,9,3,4,4,0,5,11,0,0,11,1,5,9,0,2,14,0,3,8,0,8,11,2,3,8,0,0,15,0,0,7,0,10,10,2,2,2,3,7,7,0,0,12,0,7,10,5,5,6,4,4,4,});
sparse_graph4_ints[15][35] = vector<int>({0,6,6,0,6,14,2,2,2,0,0,0,0,1,14,8,9,9,0,0,9,0,1,17,0,0,12,0,10,10,4,4,10,0,1,1,6,7,7,3,3,4,0,5,10,1,1,2,0,9,13,5,6,6,0,4,4,0,0,1,6,7,11,0,0,16,1,1,9,0,0,7,0,7,7,0,0,3,1,11,11,0,3,10,0,0,5,0,8,8,7,8,8,0,3,14,5,6,10,2,2,15,0,9,9,5,5,5,2,2,12,4,4,12,5,10,10,4,4,5,0,2,12,3,3,3,3,4,4,0,0,8,1,7,11,0,0,10,4,9,9,0,5,13,4,5,13,0,6,11,});
sparse_graph4_ints[15][40] = vector<int>({0,0,0,10,10,10,6,6,14,0,0,9,0,0,20,0,8,8,0,0,4,7,8,8,3,3,17,0,12,12,1,6,6,0,0,13,1,2,16,0,0,2,5,5,11,2,2,2,4,4,10,0,0,11,0,8,14,0,0,16,5,11,12,0,4,4,0,10,15,2,4,4,0,4,10,1,9,9,2,10,10,5,5,5,0,0,1,0,0,18,6,7,7,1,1,1,0,3,14,9,9,9,5,7,12,0,0,7,1,1,3,1,2,18,1,13,13,0,1,14,5,10,10,0,3,9,3,3,15,0,8,12,2,3,11,5,6,6,4,5,15,3,4,4,8,8,12,3,3,13,});
sparse_graph4_ints[15][45] = vector<int>({5,10,15,1,3,3,0,0,19,4,4,5,0,0,10,5,6,13,11,11,11,7,7,7,0,0,1,5,6,6,0,9,18,9,9,10,1,13,13,2,2,12,0,0,3,0,5,20,0,0,5,1,9,9,0,6,17,0,1,22,0,0,12,0,0,17,6,13,13,0,10,15,0,5,6,8,8,8,5,5,16,1,7,7,3,3,3,5,10,11,2,2,10,0,8,14,0,0,8,2,2,15,0,0,15,0,12,12,0,10,11,0,15,15,2,5,11,0,0,0,0,3,19,7,7,14,1,4,16,0,12,16,0,1,1,4,4,12,9,9,13,1,1,2,0,0,21,6,11,12,});
sparse_graph4_ints[15][50] = vector<int>({0,0,8,1,3,13,4,5,17,0,6,6,9,10,10,0,0,0,8,14,14,1,1,1,1,3,17,2,12,18,6,8,16,0,8,8,0,0,5,1,3,21,0,0,18,11,11,11,4,4,12,5,13,13,2,16,16,0,0,22,7,7,13,7,9,9,0,12,12,0,0,11,0,0,24,3,3,4,0,0,13,5,5,19,7,7,8,0,0,20,0,11,17,7,11,16,4,5,5,0,6,22,2,2,19,5,5,14,6,6,7,1,3,11,1,13,13,0,0,16,0,14,16,0,8,14,9,9,14,12,12,13,1,3,3,0,8,10,0,10,20,0,10,11,0,10,15,1,3,15,});
sparse_graph4_ints[10][6] = vector<int>({0,2,2,0,0,0,0,0,1,0,0,2,0,0,3,1,1,1,0,1,1,1,1,2,0,1,2,});
sparse_graph4_ints[10][8] = vector<int>({0,0,0,2,2,2,0,0,4,0,0,1,1,1,1,0,0,2,0,2,2,1,1,3,0,0,3,0,1,1,0,2,3,0,1,3,1,1,2,0,1,2,1,2,2,});
sparse_graph4_ints[10][10] = vector<int>({1,3,3,0,0,0,0,0,2,0,0,5,0,2,2,0,0,1,1,1,1,0,0,4,0,2,4,1,1,3,2,2,2,0,0,3,1,2,2,2,2,3,0,3,3,0,1,3,0,1,1,0,1,4,1,1,4,1,1,2,0,1,2,0,2,3,1,2,3,});
sparse_graph4_ints[10][12] = vector<int>({0,0,0,0,4,4,0,0,4,0,0,6,0,0,1,2,2,2,1,1,1,0,0,2,3,3,3,1,1,5,0,2,2,0,0,5,1,2,2,1,3,3,0,2,4,2,2,4,0,0,3,0,1,5,0,1,4,0,3,3,2,2,3,0,3,4,1,1,3,0,1,3,2,3,3,0,1,1,0,2,5,1,1,4,1,3,4,1,2,4,1,1,2,0,1,2,0,2,3,1,2,3,});
sparse_graph4_ints[10][14] = vector<int>({0,0,0,3,3,3,0,0,4,0,0,6,0,0,1,2,2,2,1,1,1,0,2,6,0,0,2,0,0,7,0,4,4,1,2,2,0,0,5,2,2,3,0,2,2,0,4,5,1,1,5,2,4,4,0,1,5,2,2,4,2,2,5,3,3,4,0,3,4,2,3,3,0,2,4,0,3,5,0,0,3,0,1,6,0,1,4,0,3,3,1,3,5,1,3,3,1,1,3,0,1,3,2,3,4,1,1,6,0,1,1,1,4,4,0,2,5,1,2,5,1,1,4,1,2,4,1,3,4,1,1,2,0,1,2,0,2,3,1,2,3,});
sparse_graph4_ints[10][16] = vector<int>({4,4,4,0,0,8,0,0,0,0,0,4,0,4,4,0,0,6,0,0,1,2,2,6,2,2,2,1,1,1,0,0,2,1,5,5,1,2,2,3,3,3,2,2,3,1,1,7,0,0,7,0,2,6,0,0,5,0,2,2,1,1,5,0,4,5,2,4,4,0,1,5,3,3,5,0,4,6,2,3,3,1,2,5,0,1,7,0,3,6,2,2,5,3,3,4,0,1,6,0,5,5,0,3,4,3,4,4,0,2,4,2,2,4,2,3,5,0,0,3,1,4,4,0,1,4,0,3,3,2,4,5,1,3,5,1,3,3,1,3,6,1,1,3,2,3,4,0,1,3,});
sparse_graph4_ints[10][18] = vector<int>({3,5,5,0,0,9,0,0,0,2,2,3,0,0,4,0,0,2,0,0,6,0,5,5,0,0,1,1,1,1,0,2,6,1,2,2,0,2,8,3,3,3,2,2,2,0,6,6,0,0,7,0,4,7,0,0,8,2,3,3,1,1,7,3,3,5,0,0,5,2,2,6,4,4,4,0,2,2,0,4,5,3,3,6,3,3,4,1,1,5,1,5,5,0,1,5,4,4,5,2,2,7,0,4,4,0,3,6,3,4,4,0,1,7,0,3,7,0,1,8,0,4,6,2,5,5,2,4,6,1,2,5,2,2,5,0,1,6,2,4,4,0,5,6,2,3,6,0,3,4,});
sparse_graph4_ints[10][20] = vector<int>({1,3,3,0,0,0,0,0,4,0,0,10,0,5,5,5,5,5,0,0,2,0,0,6,3,3,7,1,1,7,0,0,1,1,1,1,1,2,2,4,4,4,2,6,6,3,3,3,0,0,7,1,1,9,2,2,2,0,4,8,2,3,3,0,0,8,1,3,7,0,2,6,0,2,8,0,0,9,0,6,6,3,4,4,2,2,6,0,0,5,3,5,5,0,6,7,0,4,5,4,4,6,3,3,6,3,3,4,0,2,2,1,5,5,1,1,5,0,1,9,1,2,8,2,3,6,1,2,6,0,1,8,4,4,5,1,2,7,0,1,5,2,2,8,2,5,5,0,4,4,});
sparse_graph4_ints[10][23] = vector<int>({0,7,8,0,4,6,0,0,0,0,3,10,0,0,7,0,1,1,4,4,5,1,1,2,0,0,10,5,6,6,2,3,3,0,0,1,2,2,2,0,0,3,0,0,5,0,6,6,0,0,8,0,2,9,3,3,3,0,1,11,3,4,8,3,4,4,0,0,6,1,3,3,1,1,7,0,5,8,2,3,7,5,5,5,0,0,9,4,4,4,1,6,7,0,0,11,0,2,8,0,2,6,3,3,4,0,5,9,2,2,7,4,5,5,4,5,7,1,2,9,3,3,7,0,5,6,1,2,2,3,3,8,2,7,7,0,4,5,1,6,6,3,6,6,0,1,10,2,2,9,});
sparse_graph4_ints[10][26] = vector<int>({5,7,7,1,1,1,0,0,0,0,0,13,2,4,4,5,5,5,0,6,6,1,3,11,0,0,9,1,1,3,0,0,11,0,0,1,1,3,3,4,4,9,1,3,9,0,0,6,0,0,2,0,0,4,0,6,8,0,4,4,1,3,7,2,8,8,0,6,10,0,0,8,3,3,3,4,4,5,0,0,7,0,2,8,0,2,12,2,2,2,4,4,4,0,2,10,1,7,7,5,6,6,0,0,10,3,4,4,0,0,12,1,5,9,4,5,5,1,7,9,0,4,10,0,4,8,1,1,11,4,4,8,1,5,7,3,3,8,0,3,6,0,8,8,1,1,9,4,6,6,});
sparse_graph4_ints[10][30] = vector<int>({0,4,4,0,0,0,0,0,4,1,2,9,2,8,8,0,5,11,0,0,2,0,0,6,5,5,10,0,1,13,1,7,7,4,4,5,0,1,8,7,7,8,0,10,10,2,4,4,0,0,1,0,0,15,1,1,1,1,1,3,6,6,6,1,7,11,1,3,3,0,4,13,1,1,11,0,0,10,3,3,9,4,8,9,0,0,8,0,6,6,3,4,4,3,3,3,0,0,11,5,5,5,0,7,10,0,4,10,0,2,14,4,5,9,0,3,12,2,6,6,3,7,7,0,6,12,0,0,12,0,5,8,1,2,8,5,5,6,0,8,8,5,7,7,4,4,4,1,9,9,});
sparse_graph4_ints[10][35] = vector<int>({0,0,0,0,1,14,0,5,6,2,3,15,0,8,8,0,0,7,7,8,8,0,1,1,2,11,11,1,1,2,3,3,3,0,0,10,0,0,12,0,1,17,5,6,10,5,5,5,0,9,10,6,6,7,4,4,10,4,5,13,0,3,3,0,3,10,8,9,9,0,0,16,0,0,1,0,5,10,1,7,12,0,0,3,0,0,5,6,7,11,0,7,7,3,3,13,0,10,11,1,3,12,2,3,3,4,4,5,0,0,9,3,3,4,0,3,14,0,2,12,4,9,9,0,7,14,5,5,7,4,4,4,0,6,11,3,8,12,0,0,11,4,8,8,1,2,9,5,10,10,});
sparse_graph4_ints[10][40] = vector<int>({0,0,2,0,0,0,6,11,11,2,2,18,1,2,15,0,0,12,7,7,7,0,9,9,2,2,2,0,0,4,4,4,13,4,4,4,0,0,10,2,10,14,0,0,14,2,3,3,0,0,20,0,12,12,5,5,15,7,7,13,2,2,11,8,8,8,0,5,12,5,5,5,0,0,16,6,6,12,9,9,9,0,4,16,5,5,10,5,10,10,5,5,7,4,8,10,1,9,13,2,4,4,0,6,8,0,8,16,1,13,13,0,8,8,1,6,15,0,1,18,0,3,10,0,7,12,0,0,1,10,10,10,0,3,8,0,0,7,1,1,7,1,1,1,1,1,3,0,10,10,});
sparse_graph4_ints[10][45] = vector<int>({0,4,14,6,13,13,0,0,22,0,0,3,0,0,7,1,5,5,7,7,7,6,7,16,0,0,14,9,10,10,4,5,5,0,9,13,4,4,17,0,0,1,0,9,10,0,2,3,3,3,3,2,2,18,11,11,11,0,0,10,0,0,20,8,8,8,7,7,13,2,2,20,1,1,17,5,5,8,3,3,15,4,6,6,8,9,14,3,12,15,4,4,12,5,5,14,6,10,10,2,2,9,0,5,10,0,11,17,2,11,11,1,9,16,0,7,16,5,11,12,0,0,12,0,6,11,6,6,11,0,14,14,0,7,19,1,2,13,1,12,13,0,1,16,0,0,0,5,8,12,});
sparse_graph4_ints[10][50] = vector<int>({4,4,14,2,2,22,0,0,12,0,2,2,0,4,4,0,0,0,12,12,13,7,9,9,0,4,12,4,10,14,6,6,8,9,9,16,2,12,18,0,0,17,0,0,2,3,4,4,0,0,6,5,5,5,0,8,10,5,5,17,0,10,11,10,10,12,0,0,24,7,14,14,0,0,22,0,0,10,2,12,12,5,7,19,2,2,20,1,1,19,2,2,10,0,13,16,4,4,21,2,9,13,0,6,17,2,16,16,9,10,10,5,5,15,2,2,16,5,5,7,1,5,13,3,5,5,6,12,12,0,9,19,6,7,10,7,8,12,7,8,15,0,0,15,3,14,14,0,10,16,});
sparse_graph4_ints[5][6] = vector<int>({0,2,2,0,0,0,0,0,1,0,0,2,0,0,3,1,1,1,0,1,1,1,1,2,0,1,2,});
sparse_graph4_ints[5][8] = vector<int>({0,0,0,0,0,4,2,2,2,0,0,1,1,1,1,0,0,2,0,2,2,1,1,3,0,0,3,0,1,3,1,2,2,0,1,1,1,1,2,0,1,2,0,2,3,});
sparse_graph4_ints[5][10] = vector<int>({0,0,0,1,3,3,0,2,2,0,0,1,0,0,5,0,0,2,1,1,1,0,0,4,0,2,4,1,1,3,2,2,2,2,2,3,0,0,3,1,2,2,0,3,3,0,1,4,0,1,3,1,1,4,0,1,1,1,1,2,0,1,2,1,2,3,0,2,3,});
sparse_graph4_ints[5][12] = vector<int>({0,0,0,0,0,6,0,4,4,0,0,4,0,0,1,2,2,2,1,1,1,0,0,2,3,3,3,1,1,5,1,2,2,0,0,5,0,2,2,2,2,3,0,2,4,2,2,4,0,1,5,0,3,4,2,3,3,0,1,4,0,3,3,1,3,3,0,0,3,1,1,3,0,1,3,1,3,4,0,2,5,1,1,4,1,2,4,1,2,3,0,1,1,1,1,2,0,2,3,0,1,2,});
sparse_graph4_ints[5][14] = vector<int>({0,0,4,0,4,5,0,0,1,0,0,6,0,0,0,2,2,3,1,1,1,0,0,2,1,2,2,0,2,6,0,0,7,2,2,2,3,3,3,0,0,5,2,3,3,0,1,5,0,2,2,2,4,4,1,1,5,0,4,4,1,2,5,3,3,4,2,2,5,0,1,6,2,2,4,0,3,4,0,2,4,2,3,4,0,1,4,1,3,5,0,3,3,1,3,3,0,0,3,1,1,3,1,4,4,1,3,4,0,1,3,1,1,6,0,3,5,0,2,5,1,2,4,1,1,4,1,2,3,0,1,1,1,1,2,0,2,3,0,1,2,});
sparse_graph4_ints[5][16] = vector<int>({2,2,2,4,4,4,0,0,8,0,0,0,0,0,2,0,0,4,0,4,4,2,2,6,0,0,6,0,0,1,1,1,1,1,5,5,1,2,2,3,3,3,2,2,3,0,0,7,1,1,7,0,0,5,2,4,4,0,4,5,0,1,7,3,3,5,1,1,5,1,2,5,0,1,5,0,2,2,1,2,6,0,2,6,3,4,4,0,4,6,3,3,4,2,3,3,0,1,6,0,5,5,2,3,5,2,2,5,2,2,4,0,3,4,2,4,5,2,3,4,0,2,4,1,3,5,0,1,4,0,3,3,1,3,6,1,3,3,1,4,4,0,0,3,1,4,5,1,1,3,});
sparse_graph4_ints[5][18] = vector<int>({3,5,5,0,0,6,0,0,0,0,0,2,0,0,4,2,2,7,0,0,9,1,3,3,0,0,1,0,5,5,1,1,1,1,2,2,3,3,3,0,0,7,2,2,2,0,4,4,0,6,6,3,4,4,0,0,8,1,1,7,0,2,8,2,2,6,4,4,4,0,1,7,0,0,5,0,4,5,3,3,4,2,4,6,1,1,5,4,4,5,1,2,6,1,5,5,3,3,5,1,2,5,1,2,7,0,1,8,0,1,5,2,3,6,0,2,2,2,5,5,3,3,6,0,2,6,2,3,3,2,4,4,0,4,6,0,1,6,0,4,7,3,4,5,0,5,6,2,3,5,});
sparse_graph4_ints[5][20] = vector<int>({0,5,5,1,1,7,0,0,0,1,1,1,1,3,3,0,0,10,0,0,6,0,0,1,4,4,4,1,2,2,5,5,5,0,6,7,0,0,2,0,0,4,3,3,3,3,3,7,2,2,2,0,0,7,2,3,3,1,1,9,0,0,8,3,4,4,1,3,7,0,2,8,2,2,8,0,2,6,0,0,9,3,5,5,4,4,6,0,5,6,2,2,6,0,4,5,2,6,6,2,3,7,0,0,5,0,6,6,3,3,4,0,1,9,1,2,8,4,5,5,3,4,6,2,4,6,1,2,7,0,1,8,1,1,5,1,2,6,4,4,5,2,5,5,3,3,5,0,1,5,});
sparse_graph4_ints[5][23] = vector<int>({0,0,8,0,0,3,2,2,2,0,0,0,4,5,5,0,7,8,0,0,10,0,6,6,0,1,11,5,6,6,0,3,6,0,0,1,0,0,5,0,1,1,1,1,2,0,3,8,2,3,3,0,0,7,2,4,4,0,5,8,4,4,4,1,3,3,0,0,6,0,3,10,0,0,9,3,4,4,3,4,8,3,3,3,4,4,5,1,6,7,0,3,7,0,0,11,1,2,9,5,5,5,1,4,7,0,5,9,3,6,7,0,5,6,0,2,6,0,1,9,3,3,8,0,1,10,3,3,7,1,6,6,5,5,6,1,1,9,2,2,9,3,6,6,2,2,8,4,4,7,});
sparse_graph4_ints[5][26] = vector<int>({1,3,11,0,0,2,5,7,7,0,0,0,0,0,4,0,0,7,0,0,10,3,3,3,0,0,13,4,4,9,4,4,5,0,7,7,2,2,2,1,2,8,0,0,1,0,6,6,1,1,1,0,6,10,0,2,10,1,1,3,2,8,8,2,2,4,5,5,5,3,4,4,0,0,8,2,4,4,3,3,8,0,0,9,4,4,4,0,0,11,1,2,7,2,7,7,0,2,12,5,6,6,1,7,9,4,5,5,1,3,3,0,0,6,0,4,10,0,0,12,1,5,7,1,6,8,0,4,8,1,1,11,1,3,9,5,5,6,3,3,9,0,3,6,6,6,6,4,4,8,});
sparse_graph4_ints[5][30] = vector<int>({0,0,8,3,3,3,0,0,15,0,9,9,0,0,2,0,5,11,7,7,8,0,0,0,0,2,2,0,0,10,1,7,7,1,1,11,2,2,2,3,5,5,5,5,10,2,2,8,3,4,4,6,6,7,0,10,10,0,1,13,0,1,6,0,0,1,1,1,1,0,0,5,4,8,8,0,4,10,5,5,5,0,7,11,2,2,6,2,3,3,0,1,8,5,5,7,0,6,6,3,3,9,0,7,9,4,5,9,3,3,12,0,3,12,4,4,6,0,0,12,2,3,8,0,2,14,1,2,8,4,6,6,0,0,11,1,8,10,3,9,9,0,4,13,0,6,8,1,8,8,});
sparse_graph4_ints[5][35] = vector<int>({0,0,3,4,4,12,0,0,14,0,0,17,8,8,8,2,3,3,5,5,5,0,0,1,6,7,7,1,7,10,3,3,7,0,5,6,6,6,11,5,10,10,0,0,11,4,5,8,1,2,16,0,0,7,0,3,3,0,5,12,0,1,15,5,8,9,1,3,14,1,11,11,0,1,12,0,1,9,1,10,10,0,0,0,5,5,9,1,2,9,3,3,10,0,6,13,1,9,12,6,6,6,4,4,4,0,3,12,0,1,1,4,5,13,0,0,5,1,1,2,0,6,9,0,2,7,2,2,10,0,5,15,1,9,9,0,8,8,1,8,11,0,7,7,3,3,4,0,7,14,});
sparse_graph4_ints[5][40] = vector<int>({5,5,15,10,10,10,0,0,20,0,0,0,0,0,4,1,3,11,4,9,10,0,4,4,0,0,12,0,0,2,0,0,18,2,2,2,3,3,17,4,4,6,0,12,12,5,5,11,4,4,4,5,5,6,6,11,11,0,0,14,1,13,13,5,5,13,6,6,6,1,7,11,1,5,14,7,7,8,5,8,12,9,9,9,2,10,14,4,8,8,0,3,8,1,2,18,1,3,9,0,8,16,0,8,14,1,8,8,2,2,14,2,4,4,7,7,13,0,5,12,0,10,11,0,0,7,0,1,16,0,3,16,0,2,14,1,1,7,8,8,8,1,7,7,0,0,10,6,10,10,});
sparse_graph4_ints[5][45] = vector<int>({1,9,14,0,1,22,9,12,12,9,9,9,2,3,14,4,5,5,0,0,0,0,0,4,0,5,5,6,6,6,0,3,4,5,6,17,5,5,12,0,0,18,0,1,1,0,6,17,0,8,8,0,0,16,1,9,9,0,5,12,0,7,19,4,4,4,10,10,10,5,10,10,1,3,11,0,4,20,4,10,13,0,0,9,0,6,15,0,0,21,5,12,12,1,3,9,1,2,17,0,0,14,6,8,9,7,7,7,0,3,19,4,13,14,4,5,11,6,8,15,6,6,14,0,10,16,0,0,12,9,9,13,0,2,9,0,11,12,0,14,14,4,5,16,1,12,16,1,10,10,});
sparse_graph4_ints[5][50] = vector<int>({0,7,8,0,0,10,0,0,25,0,0,0,0,0,4,5,5,15,8,8,17,0,4,16,4,4,20,11,11,11,0,0,14,7,7,15,6,6,8,0,14,18,0,0,17,0,2,6,7,8,8,12,12,13,0,0,2,0,8,18,2,10,12,0,0,23,0,0,21,1,4,18,0,14,15,1,3,23,2,2,2,4,5,13,1,3,21,1,6,11,9,9,13,0,0,12,2,16,16,6,6,19,3,4,6,5,9,13,0,10,11,4,11,16,2,2,12,8,14,14,3,4,4,0,0,19,10,10,15,6,12,12,8,9,9,5,13,15,5,6,6,2,9,18,9,10,10,0,11,16,});
*/

for(int error_rate = 0; error_rate <= 40; ++error_rate){
if(sparse_graph4_ints.count(error_rate) == 0) continue;
for(int num_nodes = 4; num_nodes <= 100; ++num_nodes){
if(sparse_graph4_ints[error_rate].count(num_nodes) == 0) continue;

vector<CompleteGraphSolver::CompleteGraph> v;
for(int j = 0; j < sz(sparse_graph4_ints[error_rate][num_nodes]); j += 3){
int i0 = sparse_graph4_ints[error_rate][num_nodes][j + 0];
int i1 = sparse_graph4_ints[error_rate][num_nodes][j + 1];
int i2 = sparse_graph4_ints[error_rate][num_nodes][j + 2];
v.push_back(CompleteGraphSolver::CompleteGraph(vector<int>({i0,i1,i2,num_nodes - i0 - i1 - i2})));
}
graphs_4_flip[error_rate][num_nodes] = v;
}
my_assert(18 == sz(graphs_4_flip[error_rate]));
}
}
}

vector<CompleteGraphSolver::CompleteGraph> get_graphs_3_non_flip(int num_nodes,double error_rate,int num_graphs)
{
my_assert(num_graphs <= get_graph_size_3_non_flip(num_nodes,error_rate));
vector<CompleteGraphSolver::CompleteGraph> v = get_graphs_3_non_flip(num_nodes,error_rate);
while(num_graphs < sz(v)) v.pop_back();
return v;
}

vector<CompleteGraphSolver::CompleteGraph> get_graphs_4_non_flip(int num_nodes,double error_rate,int num_graphs)
{
my_assert(num_graphs <= get_graph_size_4_non_flip(num_nodes,error_rate));
vector<CompleteGraphSolver::CompleteGraph> v = get_graphs_4_non_flip(num_nodes,error_rate);
while(num_graphs < sz(v)) v.pop_back();
return v;
}

vector<CompleteGraphSolver::CompleteGraph> get_graphs_3_flip(int num_nodes,double error_rate,int num_graphs)
{
my_assert(num_graphs <= get_graph_size_3_flip(num_nodes,error_rate));
vector<CompleteGraphSolver::CompleteGraph> v = get_graphs_3_flip(num_nodes,error_rate);
while(num_graphs < sz(v)) v.pop_back();
return v;
}

vector<CompleteGraphSolver::CompleteGraph> get_graphs_4_flip(int num_nodes,double error_rate,int num_graphs)
{
my_assert(num_graphs <= get_graph_size_4_flip(num_nodes,error_rate));
vector<CompleteGraphSolver::CompleteGraph> v = get_graphs_4_flip(num_nodes,error_rate);
while(num_graphs < sz(v)) v.pop_back();
return v;
}

int get_graph_size_3_non_flip(int n,double error_rate)
{
return sz(get_graphs_3_non_flip(n,error_rate));
}

int get_graph_size_4_non_flip(int n,double error_rate)
{
return sz(get_graphs_4_non_flip(n,error_rate));
}

int get_graph_size_3_flip(int n,double error_rate)
{
return sz(get_graphs_3_flip(n,error_rate));
}

int get_graph_size_4_flip(int n,double error_rate)
{
return sz(get_graphs_4_flip(n,error_rate));
}

vector<int> error_rates;
unordered_map<int,unordered_map<int,vector<CompleteGraphSolver::CompleteGraph>>> graphs_3_non_flip;
unordered_map<int,unordered_map<int,vector<CompleteGraphSolver::CompleteGraph>>> graphs_4_non_flip;
unordered_map<int,unordered_map<int,vector<CompleteGraphSolver::CompleteGraph>>> graphs_3_flip;
unordered_map<int,unordered_map<int,vector<CompleteGraphSolver::CompleteGraph>>> graphs_4_flip;

// private:
vector<CompleteGraphSolver::CompleteGraph> get_graphs_3_non_flip(int num_nodes,double error_rate)
{
int error_rate_int = (int) (error_rate * 100 + 0.5);
while(graphs_3_non_flip.count(error_rate_int) == 0) ++error_rate_int;
while(graphs_3_non_flip[error_rate_int].count(num_nodes) == 0) ++num_nodes;
return graphs_3_non_flip[error_rate_int][num_nodes];
}

vector<CompleteGraphSolver::CompleteGraph> get_graphs_3_flip(int num_nodes,double error_rate)
{
int error_rate_int = (int) (error_rate * 100 + 0.5);
while(graphs_3_flip.count(error_rate_int) == 0) ++error_rate_int;
while(graphs_3_flip[error_rate_int].count(num_nodes) == 0) ++num_nodes;
return graphs_3_flip[error_rate_int][num_nodes];
}

vector<CompleteGraphSolver::CompleteGraph> get_graphs_4_non_flip(int num_nodes,double error_rate)
{
int error_rate_int = (int) (error_rate * 100 + 0.5);
while(graphs_4_non_flip.count(error_rate_int) == 0) ++error_rate_int;
while(graphs_4_non_flip[error_rate_int].count(num_nodes) == 0) ++num_nodes;
return graphs_4_non_flip[error_rate_int][num_nodes];
}

vector<CompleteGraphSolver::CompleteGraph> get_graphs_4_flip(int num_nodes,double error_rate)
{
int error_rate_int = (int) (error_rate * 100 + 0.5);
while(graphs_4_flip.count(error_rate_int) == 0) ++error_rate_int;
while(graphs_4_flip[error_rate_int].count(num_nodes) == 0) ++num_nodes;
return graphs_4_flip[error_rate_int][num_nodes];
}
};

static Solver* create_large(int num_graphs,double error_rate,int num_nodes)
{
SparseGraphs graphs;
if(graphs.get_graph_size_3_non_flip(num_nodes,error_rate) < num_graphs){
return BayesSolver::create_sloth(num_graphs,error_rate,4);
}
return new CompleteGraphSolver(graphs.get_graphs_3_non_flip(num_nodes,error_rate,num_graphs),error_rate);
}

static Solver* create_large_flip(int num_graphs,double error_rate,int num_nodes,int dup)
{
SparseGraphs graphs;
if(graphs.get_graph_size_3_flip(num_nodes,error_rate) * (dup ? 2 : 1) < num_graphs){
return BayesSolver::create_sloth(num_graphs,error_rate,4);
}
vector<CompleteGraph> complete_graphs = graphs.get_graphs_3_flip(num_nodes,error_rate);
vector<CompleteGraph> mixed;
for(CompleteGraph g: complete_graphs){
if(dup){
mixed.push_back(g);
g.flip_all = 1;
mixed.push_back(g);
}else{
if(num_graphs <= sz(mixed) * 2) g.flip_all = 1;
mixed.push_back(g);
}
} 
while(num_graphs < sz(mixed)) mixed.pop_back();
return new CompleteGraphSolver(mixed,error_rate);
}

static Solver* create_large_4(int num_graphs,double error_rate,int num_nodes)
{
SparseGraphs graphs;
if(graphs.get_graph_size_4_non_flip(num_nodes,error_rate) < num_graphs){
return BayesSolver::create_sloth(num_graphs,error_rate,4);
}
return new CompleteGraphSolver(graphs.get_graphs_4_non_flip(num_nodes,error_rate,num_graphs),error_rate);
}

static Solver* create_large_4_flip(int num_graphs,double error_rate,int num_nodes,int dup)
{
SparseGraphs graphs;
if(graphs.get_graph_size_4_flip(num_nodes,error_rate) * (dup ? 2 : 1) < num_graphs){
return BayesSolver::create_sloth(num_graphs,error_rate,4);
}
vector<CompleteGraph> complete_graphs = graphs.get_graphs_4_flip(num_nodes,error_rate);
vector<CompleteGraph> mixed;
for(CompleteGraph g: complete_graphs){
if(dup){
mixed.push_back(g);
g.flip_all = true;
mixed.push_back(g);
}else{
if(num_graphs <= sz(mixed) * 2) g.flip_all = 1;
mixed.push_back(g);
}
}
while(num_graphs < sz(mixed)) mixed.pop_back();
return new CompleteGraphSolver(mixed,error_rate);
}

vector<vector<double>> get_cluster_weights(Graph &graph,vector<int> &cluster_indexes,int num_clusters,int num_propagates)
{
vector<vector<double>> cluster_weights;

rep(node_index,graph.get_n()){
vector<double> v(num_clusters,0);
v[cluster_indexes[node_index]] = 1;
cluster_weights.push_back(v);
}

rep(loop,num_propagates){
vector<vector<double>> next_weights;
rep(node_index,graph.get_n()){
vector<double> num_flips;
rep(cluster,num_clusters){
double cnt = 0.0;
rep(i,graph.get_n())if(i != node_index){
cnt += graph.has_edge(i,node_index) ? (1 - cluster_weights[i][cluster]) : cluster_weights[i][cluster];
}
num_flips.push_back(cnt);
}
double min_flips = num_flips[0];
rep(i,num_clusters) min_flips = min(min_flips,num_flips[i]);

vector<double> weights;
rep(i,num_clusters) weights.push_back(pow(error_rate / (1 - error_rate),num_flips[i] - min_flips));

double total_weight = 0;
rep(i,num_clusters) total_weight += weights[i];
rep(i,num_clusters) weights[i] /= total_weight;
next_weights.push_back(weights);
}
cluster_weights = next_weights;
}
return cluster_weights;
}

unordered_map<int,ExpNum> run_dp2(Graph &graph,vector<int> &cluster_indexes,vector<vector<double>> &cluster_weights)
{
const int W = 30;
const int N = 2 * W + 1;
ExpNum dp[N];
rep(i,N) dp[i] = create_exp_num(); 
dp[W].add(1,0);

rep(node_index,graph.get_n()){
ExpNum next_dp[N];
rep(i,N) next_dp[i] = create_exp_num(); 

rep(next_cluster,2){
double cnt_flips = 0;
rep(i,graph.get_n())if(i != node_index){
double w = cluster_weights[i][next_cluster];
cnt_flips += graph.has_edge(i,node_index) ? (1 - w) : w;
}

rep(dp_index,N){
int next_index = dp_index;
if(cluster_indexes[node_index] == 0) --next_index;
if(next_cluster == 0) ++next_index;
if(next_index < 0 || N <= next_index) continue;

ExpNum num = dp[dp_index];
num.multiply(1,cnt_flips);
next_dp[next_index].add(num);
}
}

rep(i,N) dp[i] = next_dp[i];
}

unordered_map<int,ExpNum> res;
int counts[2] = {
(int) count(cluster_indexes.begin(),cluster_indexes.end(),0),
(int) count(cluster_indexes.begin(),cluster_indexes.end(),1)
};
my_assert(counts[0] + counts[1] == sz(cluster_indexes));

rep(i,N)if(dp[i].mul != 0){ 
int diff0 = i - W;
int diff1 = 0 - diff0;
int h = calc_hash(vector<int>({counts[0] + diff0,counts[1] + diff1}));
if(res.count(h) == 0) res[h] = create_exp_num();
res[h].add(dp[i]);
}
return res;
}

unordered_map<int,ExpNum> run_dp3(Graph &graph,vector<int> &cluster_indexes,vector<vector<double>> &cluster_weights)
{
const int W = 20;
const int N = 2 * W + 1;

double dp[N][N];
ExpNum base = create_exp_num();
base.add(1,0);

rep(i,N)rep(j,N) dp[i][j] = 0;
dp[W][W] = 1;

rep(node_index,graph.get_n()){
double next_dp[N][N];
rep(i,N)rep(j,N) next_dp[i][j] = 0;

rep(next_cluster,3){
double cnt_flips = 0;
rep(i,graph.get_n())if(i != node_index){
double w = cluster_weights[i][next_cluster];
cnt_flips += graph.has_edge(i,node_index) ? (1 - w) : w;
}

double e = pow(error_rate / (1 - error_rate),cnt_flips);

rep(dp_index_0,N)rep(dp_index_1,N){
int next_index_0 = dp_index_0;
int next_index_1 = dp_index_1;
if(cluster_indexes[node_index] == 0) --next_index_0;
if(cluster_indexes[node_index] == 1) --next_index_1;
if(next_cluster == 0) ++next_index_0;
if(next_cluster == 1) ++next_index_1;
if(next_index_0 < 0 || N <= next_index_0) continue;
if(next_index_1 < 0 || N <= next_index_1) continue;

double d = dp[dp_index_0][dp_index_1];
next_dp[next_index_0][next_index_1] += d * e;
}
}

double maxi = 1e-100;
rep(i,N)rep(j,N) maxi = max(maxi,dp[i][j]);
base.multiply(ExpNum::from_double(maxi,error_rate / (1 - error_rate)));
rep(i,N)rep(j,N) dp[i][j] = next_dp[i][j] / maxi;
}

int counts[3] = {
(int) count(cluster_indexes.begin(),cluster_indexes.end(),0),
(int) count(cluster_indexes.begin(),cluster_indexes.end(),1),
(int) count(cluster_indexes.begin(),cluster_indexes.end(),2)
};
my_assert(counts[0] + counts[1] + counts[2] == sz(cluster_indexes));

unordered_map<int,double> res;
rep(i0,N)rep(i1,N)if(dp[i0][i1] != 0){ 
int diff0 = i0 - W;
int diff1 = i1 - W;
int diff2 = 0 - diff0 - diff1;
int h = calc_hash(vector<int>({counts[0] + diff0,counts[1] + diff1,counts[2] + diff2}));
res[h] += dp[i0][i1];
}

unordered_map<int,ExpNum> scaled_res;
for(auto p: res){
ExpNum num = ExpNum::from_double(p.second,error_rate / (1 - error_rate));
num.multiply(base);
scaled_res[p.first] = num;
}

return scaled_res;
}

unordered_map<int,ExpNum> run_dp4(Graph &graph,vector<int> &cluster_indexes,vector<vector<double>> &cluster_weights)
{
const int W = 10;
const int N = 2 * W + 1;
double dp[N][N][N];
ExpNum base = create_exp_num();
base.add(1,0);

rep(i,N)rep(j,N)rep(k,N) dp[i][j][k] = 0;
dp[W][W][W] = 1;

double next_dp[N][N][N];
rep(node_index,graph.get_n()){
rep(i,N)rep(j,N)rep(k,N) next_dp[i][j][k] = 0;

rep(next_cluster,4){
double cnt_flips = 0;
rep(i,graph.get_n())if(i != node_index){
double w = cluster_weights[i][next_cluster];
cnt_flips += graph.has_edge(i,node_index) ? (1 - w) : w;
}

int diff_0 = 0;
int diff_1 = 0;
int diff_2 = 0;
if(cluster_indexes[node_index] == 0) --diff_0;
if(cluster_indexes[node_index] == 1) --diff_1;
if(cluster_indexes[node_index] == 2) --diff_2;
if(next_cluster == 0) ++diff_0;
if(next_cluster == 1) ++diff_1;
if(next_cluster == 2) ++diff_2;

double e = pow(error_rate / (1 - error_rate),cnt_flips);

rep(dp_index_0,N)rep(dp_index_1,N)rep(dp_index_2,N){
int next_index_0 = dp_index_0 + diff_0;
int next_index_1 = dp_index_1 + diff_1;
int next_index_2 = dp_index_2 + diff_2;
if(next_index_0 < 0 || N <= next_index_0) continue;
if(next_index_1 < 0 || N <= next_index_1) continue;
if(next_index_2 < 0 || N <= next_index_2) continue;

double d = dp[dp_index_0][dp_index_1][dp_index_2];
next_dp[next_index_0][next_index_1][next_index_2] += d * e;
}
}

double maxi = 1e-100;
rep(i,N)rep(j,N)rep(k,N) maxi = max(maxi,dp[i][j][k]);
base.multiply(ExpNum::from_double(maxi,error_rate / (1 - error_rate)));
rep(i,N)rep(j,N)rep(k,N) dp[i][j][k] = next_dp[i][j][k] / maxi;
}

int counts[4] = {
(int) count(cluster_indexes.begin(),cluster_indexes.end(),0),
(int) count(cluster_indexes.begin(),cluster_indexes.end(),1),
(int) count(cluster_indexes.begin(),cluster_indexes.end(),2),
(int) count(cluster_indexes.begin(),cluster_indexes.end(),3)
};
my_assert(counts[0] + counts[1] + counts[2] + counts[3] == sz(cluster_indexes));

unordered_map<int,double> res;
rep(i0,N)rep(i1,N)rep(i2,N)if(dp[i0][i1][i2] != 0){ 
int diff0 = i0 - W;
int diff1 = i1 - W;
int diff2 = i2 - W;
int diff3 = 0 - diff0 - diff1 - diff2;
int h = calc_hash(vector<int>({counts[0] + diff0,counts[1] + diff1,counts[2] + diff2,counts[3] + diff3}));
res[h] += dp[i0][i1][i2];
}

unordered_map<int,ExpNum> scaled_res;
for(auto p: res){
ExpNum num = ExpNum::from_double(p.second,error_rate / (1 - error_rate));
num.multiply(base);
scaled_res[p.first] = num;
}

return scaled_res;
}

int get_n()
{
return graphs.back().get_n();
}

vector<Graph> get_graphs()
{
return graphs;
}

ExpNum create_exp_num()
{
return ExpNum(error_rate / (1 - error_rate));
}

int count_flip(Graph &graph,vector<int> cluster_indexes)
{
int res = 0;
rep(i,graph.get_n())rep(j,i)if(graph.has_edge(i,j) != (cluster_indexes[i] == cluster_indexes[j])) ++res;
return res;
}

vector<pair<ExpNum,int>> score0(Graph &graph,int flip_all)
{
if(complete_graphs[flip_all].count(0) == 0) return vector<pair<ExpNum,int>>();

my_assert(sz(complete_graphs[flip_all][0]) == 1);

for(auto &p: complete_graphs[flip_all][0]){
int num_flips = 0;
rep(i,graph.get_n())rep(j,i)if(graph.has_edge(i,j)) ++num_flips;

ExpNum res = create_exp_num();
res.add(1,0);
res.multiply(1,num_flips * 2);
return vector<pair<ExpNum,int>>({make_pair(res,p.second)});
}
}

vector<pair<ExpNum,int>> score1(Graph &graph,int flip_all)
{
if(complete_graphs[flip_all].count(1) == 0) return vector<pair<ExpNum,int>>();

my_assert(sz(complete_graphs[flip_all][1]) == 1);

for(auto &p: complete_graphs[flip_all][1]){
int num_flips = 0;
rep(i,graph.get_n())rep(j,i)if(!graph.has_edge(i,j)) ++num_flips;

ExpNum res = create_exp_num();
res.add(1,0);
res.multiply(1,num_flips * 2);
return vector<pair<ExpNum,int>>({make_pair(res,p.second)});
}
}

vector<pair<ExpNum,int>> score2(Graph &graph,bool use_simulated_annealing,int num_attempts,int seed,int num_propagates,int flip_all)
{
int num_clusters = 2;

vector<int> cluster_indexes = ClusteringComplete::cluster_complete_graphs(graph,num_clusters,use_simulated_annealing,num_attempts,seed);
vector<vector<double>> cluster_weights = get_cluster_weights(graph,cluster_indexes,num_clusters,num_propagates);
unordered_map<int,ExpNum> total = run_dp2(graph,cluster_indexes,cluster_weights);

vector<pair<ExpNum,int>> res;
for(auto &p: complete_graphs[flip_all][num_clusters]){
int h = p.first.get_hash();
if(total.count(h)) res.push_back(make_pair(total[h],p.second));
}
return res;
}

vector<pair<ExpNum,int>> score3(Graph &graph,bool use_simulated_annealing,int num_attempts,int seed,int num_propagates,int flip_all)
{
int num_clusters = 3;

vector<int> cluster_indexes = ClusteringComplete::cluster_complete_graphs(graph,num_clusters,use_simulated_annealing,num_attempts,seed);
vector<vector<double>> cluster_weights = get_cluster_weights(graph,cluster_indexes,num_clusters,num_propagates);
unordered_map<int,ExpNum> total = run_dp3(graph,cluster_indexes,cluster_weights);

vector<pair<ExpNum,int>> res;
for(auto &p: complete_graphs[flip_all][num_clusters]){
int h = p.first.get_hash();
if(total.count(h)) res.push_back(make_pair(total[h],p.second));
}
return res;
}

vector<pair<ExpNum,int>> score4(Graph &graph,bool use_simulated_annealing,int num_attempts,int seed,int num_propagates,int flip_all)
{
if(complete_graphs[flip_all].count(4) == 0) return vector<pair<ExpNum,int>>();

int num_clusters = 4;

vector<int> cluster_indexes = ClusteringComplete::cluster_complete_graphs(graph,num_clusters,use_simulated_annealing,num_attempts,seed);
vector<vector<double>> cluster_weights = get_cluster_weights(graph,cluster_indexes,num_clusters,num_propagates);
unordered_map<int,ExpNum> total = run_dp4(graph,cluster_indexes,cluster_weights);

vector<pair<ExpNum,int>> res;
for(auto &p: complete_graphs[flip_all][num_clusters]){
int h = p.first.get_hash();
if(total.count(h)) res.push_back(make_pair(total[h],p.second));
}
return res;
}

int predict_max_4(Graph graph,bool use_simulated_annealing,int num_attempts,int seed,int num_propagates)
{
vector<pair<ExpNum,int>> scores;
for(int flip_all: {0,1}){
for(pair<ExpNum,int> p: score0(graph,flip_all)) scores.push_back(p);
for(pair<ExpNum,int> p: score1(graph,flip_all)) scores.push_back(p);
for(pair<ExpNum,int> p: score2(graph,use_simulated_annealing,num_attempts,seed,num_propagates,flip_all)) scores.push_back(p);
for(pair<ExpNum,int> p: score3(graph,use_simulated_annealing,num_attempts,seed,num_propagates,flip_all)) scores.push_back(p);
for(pair<ExpNum,int> p: score4(graph,use_simulated_annealing,num_attempts,seed,num_propagates,flip_all)) scores.push_back(p);
graph.flip_all();
}

if(sz(scores) == 0) return 0;

int best = 0;
rep(i,sz(scores))if(scores[i].first.is_bigger_than(scores[best].first)) best = i;
return scores[best].second;
}


vector<vector<ExpNum>> run_independent_dp(Graph &graph,vector<int> &cluster_indexes,int num_clusters,vector<vector<double>> &cluster_weights)
{
vector<vector<ExpNum>> dp;
rep(next_cluster,num_clusters){
vector<ExpNum> probs;
rep(i,graph.get_n() + 1) probs.push_back(create_exp_num());
probs[0].add(1,0);
dp.push_back(probs);
}

rep(node_index,sz(cluster_indexes)){
vector<ExpNum> flip_probs;
rep(cluster_index,num_clusters){
double cnt_flips = 0.0;
rep(i,sz(cluster_indexes))if(i != node_index){
double w = cluster_weights[i][cluster_index];
cnt_flips += graph.has_edge(i,node_index) ? (1 - w) : w;
}
ExpNum flip_prob = create_exp_num();
flip_prob.add(1,cnt_flips);
flip_probs.push_back(flip_prob);
}

rep(cluster_index,num_clusters){
ExpNum same = create_exp_num();
ExpNum others = create_exp_num();
rep(i,num_clusters){
if(i == cluster_index) same.add(flip_probs[i]);
else others.add(flip_probs[i]);
}

for(int i = graph.get_n(); 0 <= i; --i){
dp[cluster_index][i].multiply(others);
if(0 < i){
ExpNum d = dp[cluster_index][i - 1];
d.multiply(same);
dp[cluster_index][i].add(d);
}
}
}
}

rep(i,sz(dp))rep(j,sz(dp[i])) dp[i][j].expo /= num_clusters;

return dp;
}

ExpNum agg_independent_dp(vector<vector<ExpNum>> &probs,CompleteGraph &graph,int depth = 0,int used_mask = 0)
{
if(depth == sz(probs)){
ExpNum res = create_exp_num();
res.add(1,0);
return res;
}

ExpNum total = create_exp_num();
unordered_set<int> used_sizes;
rep(i,sz(graph.sizes)){
if(used_mask & bit(i)) continue;

int size = graph.sizes[i];
if(used_sizes.count(size)) continue;

ExpNum num = agg_independent_dp(probs,graph,depth + 1,used_mask | bit(i));
num.multiply(probs[depth][size]);
total.add(num);

used_sizes.insert(size);
}
return total;
}

int predict_infinite(Graph &graph,bool use_simulated_annealing,int num_attempts,int seed,int num_propagates)
{
int best_index = 0;
ExpNum best_score = create_exp_num();

for(int flip_all: {0,1}){
for(auto &p: complete_graphs[flip_all]){
int num_clusters = p.first;
vector<int> cluster_indexes = ClusteringComplete::cluster_complete_graphs(graph,num_clusters,use_simulated_annealing,num_attempts,seed);
vector<vector<double>> cluster_weights = get_cluster_weights(graph,cluster_indexes,num_clusters,num_propagates);
vector<vector<ExpNum>> probs = run_independent_dp(graph,cluster_indexes,num_clusters,cluster_weights);
for(auto &g: p.second){
ExpNum score = agg_independent_dp(probs,g.first);
if(score.is_bigger_than(best_score)){
best_score = score;
best_index = g.second;
}
}
}
graph.flip_all();
}

return best_index;
}

int predict(Graph graph)
{
bool use_simulated_annealing = false;
int num_attempts = 10;
int seed = 1234;
int num_propagates = 5;

int maxi_num_complete_graphs = 0;
for(auto &p: complete_graphs[0]) maxi_num_complete_graphs = max(maxi_num_complete_graphs,p.first);
for(auto &p: complete_graphs[1]) maxi_num_complete_graphs = max(maxi_num_complete_graphs,p.first);

if(maxi_num_complete_graphs <= 3) return predict_max_4(graph,use_simulated_annealing,num_attempts,seed,num_propagates);
return predict_infinite(graph,use_simulated_annealing,num_attempts,seed,num_propagates);
}

vector<Graph> graphs;
unordered_map<int,vector<pair<CompleteGraph,int>>> complete_graphs[2];

double error_rate;
};


class Conf
{
public:
Conf(string name) : name(name) {}
Conf(string name,int arg0) : name(name),arg0(arg0) {}
Conf(string name,int arg0,int arg1) : name(name),arg0(arg0),arg1(arg1) {}

Solver* create(int num_graphs,double error_rate)
{
if(name == "optimal") return BayesSolver::create_optimal(num_graphs,error_rate);
if(name == "sloth") return BayesSolver::create_sloth(num_graphs,error_rate,arg0);
if(name == "sparse_8") return BayesSolver::create_sparse_8(num_graphs,error_rate);
if(name == "packed") return CompleteGraphSolver::create_packed(num_graphs,error_rate,arg0,arg1);
if(name == "large") return CompleteGraphSolver::create_large(num_graphs,error_rate,arg0);
if(name == "large_4") return CompleteGraphSolver::create_large_4(num_graphs,error_rate,arg0);
if(name == "large_flip") return CompleteGraphSolver::create_large_flip(num_graphs,error_rate,arg0,1);
if(name == "large_4_flip") return CompleteGraphSolver::create_large_4_flip(num_graphs,error_rate,arg0,1);
my_assert(false);
}

string to_string()
{
stringstream ss;
ss << "Conf(\"" << name << "\")";
return ss.str();
}

string name;
int arg0 = -1;
int arg1 = -1;
};


































































class LoadBalancer
{
public:
static Solver* create(string name,int num_graphs,double error_rate)
{
if(name == "optimal") return Conf("optimal").create(num_graphs,error_rate);
if(name == "sloth") return Conf("sloth",4).create(num_graphs,error_rate);
if(name == "sloth_4") return Conf("sloth",4).create(num_graphs,error_rate);
if(name == "sloth_5") return Conf("sloth",5).create(num_graphs,error_rate);
if(name == "sloth_6") return Conf("sloth",6).create(num_graphs,error_rate);
if(name == "sloth_7") return Conf("sloth",7).create(num_graphs,error_rate);
if(name == "sparse_8") return Conf("sparse_8").create(num_graphs,error_rate);
if(name == "packed_1_9") return Conf("packed",1,9).create(num_graphs,error_rate);
if(name == "packed_1_8") return Conf("packed",1,8).create(num_graphs,error_rate);
if(name == "packed_1_7") return Conf("packed",1,7).create(num_graphs,error_rate);
if(name == "packed_1_6") return Conf("packed",1,6).create(num_graphs,error_rate);
if(name == "packed_1_5") return Conf("packed",1,5).create(num_graphs,error_rate);
if(name == "packed_1_4") return Conf("packed",1,4).create(num_graphs,error_rate);
if(name == "packed_1_3") return Conf("packed",1,3).create(num_graphs,error_rate);
if(name == "packed_1_2") return Conf("packed",1,2).create(num_graphs,error_rate);
if(name == "packed_2_9") return Conf("packed",2,9).create(num_graphs,error_rate);
if(name == "packed_2_8") return Conf("packed",2,8).create(num_graphs,error_rate);
if(name == "packed_2_7") return Conf("packed",2,7).create(num_graphs,error_rate);
if(name == "packed_2_6") return Conf("packed",2,6).create(num_graphs,error_rate);
if(name == "packed_2_5") return Conf("packed",2,5).create(num_graphs,error_rate);
if(name == "packed_2_4") return Conf("packed",2,4).create(num_graphs,error_rate);
if(name == "packed_2_3") return Conf("packed",2,3).create(num_graphs,error_rate);

if(name == "large_8") return Conf("large",8).create(num_graphs,error_rate);
if(name == "large_10") return Conf("large",10).create(num_graphs,error_rate);
if(name == "large_12") return Conf("large",12).create(num_graphs,error_rate);
if(name == "large_14") return Conf("large",14).create(num_graphs,error_rate);
if(name == "large_16") return Conf("large",16).create(num_graphs,error_rate);
if(name == "large_18") return Conf("large",18).create(num_graphs,error_rate);
if(name == "large_20") return Conf("large",20).create(num_graphs,error_rate);
if(name == "large_23") return Conf("large",23).create(num_graphs,error_rate);
if(name == "large_26") return Conf("large",26).create(num_graphs,error_rate);
if(name == "large_30") return Conf("large",30).create(num_graphs,error_rate);
if(name == "large_35") return Conf("large",35).create(num_graphs,error_rate);
if(name == "large_40") return Conf("large",40).create(num_graphs,error_rate);
if(name == "large_45") return Conf("large",45).create(num_graphs,error_rate);
if(name == "large_50") return Conf("large",50).create(num_graphs,error_rate);
if(name == "large_55") return Conf("large",55).create(num_graphs,error_rate);
if(name == "large_60") return Conf("large",60).create(num_graphs,error_rate);
if(name == "large_70") return Conf("large",70).create(num_graphs,error_rate);
if(name == "large_80") return Conf("large",80).create(num_graphs,error_rate);
if(name == "large_100") return Conf("large",100).create(num_graphs,error_rate);

if(name == "large_8_flip") return Conf("large_flip",8).create(num_graphs,error_rate);
if(name == "large_10_flip") return Conf("large_flip",10).create(num_graphs,error_rate);
if(name == "large_12_flip") return Conf("large_flip",12).create(num_graphs,error_rate);
if(name == "large_14_flip") return Conf("large_flip",14).create(num_graphs,error_rate);
if(name == "large_16_flip") return Conf("large_flip",16).create(num_graphs,error_rate);
if(name == "large_18_flip") return Conf("large_flip",18).create(num_graphs,error_rate);
if(name == "large_20_flip") return Conf("large_flip",20).create(num_graphs,error_rate);
if(name == "large_23_flip") return Conf("large_flip",23).create(num_graphs,error_rate);
if(name == "large_26_flip") return Conf("large_flip",26).create(num_graphs,error_rate);
if(name == "large_30_flip") return Conf("large_flip",30).create(num_graphs,error_rate);
if(name == "large_35_flip") return Conf("large_flip",35).create(num_graphs,error_rate);
if(name == "large_40_flip") return Conf("large_flip",40).create(num_graphs,error_rate);
if(name == "large_45_flip") return Conf("large_flip",45).create(num_graphs,error_rate);
if(name == "large_50_flip") return Conf("large_flip",50).create(num_graphs,error_rate);
if(name == "large_55_flip") return Conf("large_flip",55).create(num_graphs,error_rate);
if(name == "large_60_flip") return Conf("large_flip",60).create(num_graphs,error_rate);
if(name == "large_70_flip") return Conf("large_flip",70).create(num_graphs,error_rate);
if(name == "large_80_flip") return Conf("large_flip",80).create(num_graphs,error_rate);
if(name == "large_100_flip") return Conf("large_flip",100).create(num_graphs,error_rate);

if(name == "large_4_8") return Conf("large_4",8).create(num_graphs,error_rate);
if(name == "large_4_10") return Conf("large_4",10).create(num_graphs,error_rate);
if(name == "large_4_12") return Conf("large_4",12).create(num_graphs,error_rate);
if(name == "large_4_14") return Conf("large_4",14).create(num_graphs,error_rate);
if(name == "large_4_16") return Conf("large_4",16).create(num_graphs,error_rate);
if(name == "large_4_18") return Conf("large_4",18).create(num_graphs,error_rate);
if(name == "large_4_20") return Conf("large_4",20).create(num_graphs,error_rate);
if(name == "large_4_23") return Conf("large_4",23).create(num_graphs,error_rate);
if(name == "large_4_26") return Conf("large_4",26).create(num_graphs,error_rate);
if(name == "large_4_30") return Conf("large_4",30).create(num_graphs,error_rate);
if(name == "large_4_35") return Conf("large_4",35).create(num_graphs,error_rate);
if(name == "large_4_40") return Conf("large_4",40).create(num_graphs,error_rate);
if(name == "large_4_45") return Conf("large_4",45).create(num_graphs,error_rate);
if(name == "large_4_50") return Conf("large_4",50).create(num_graphs,error_rate);
if(name == "large_4_55") return Conf("large_4",55).create(num_graphs,error_rate);
if(name == "large_4_60") return Conf("large_4",60).create(num_graphs,error_rate);
if(name == "large_4_100") return Conf("large_4",100).create(num_graphs,error_rate);

if(name == "large_4_8_flip") return Conf("large_4_flip",8).create(num_graphs,error_rate);
if(name == "large_4_10_flip") return Conf("large_4_flip",10).create(num_graphs,error_rate);
if(name == "large_4_12_flip") return Conf("large_4_flip",12).create(num_graphs,error_rate);
if(name == "large_4_14_flip") return Conf("large_4_flip",14).create(num_graphs,error_rate);
if(name == "large_4_16_flip") return Conf("large_4_flip",16).create(num_graphs,error_rate);
if(name == "large_4_18_flip") return Conf("large_4_flip",18).create(num_graphs,error_rate);
if(name == "large_4_20_flip") return Conf("large_4_flip",20).create(num_graphs,error_rate);
if(name == "large_4_23_flip") return Conf("large_4_flip",23).create(num_graphs,error_rate);
if(name == "large_4_26_flip") return Conf("large_4_flip",26).create(num_graphs,error_rate);
if(name == "large_4_30_flip") return Conf("large_4_flip",30).create(num_graphs,error_rate);
if(name == "large_4_35_flip") return Conf("large_4_flip",35).create(num_graphs,error_rate);
if(name == "large_4_40_flip") return Conf("large_4_flip",40).create(num_graphs,error_rate);
if(name == "large_4_45_flip") return Conf("large_4_flip",45).create(num_graphs,error_rate);
if(name == "large_4_50_flip") return Conf("large_4_flip",50).create(num_graphs,error_rate);
if(name == "large_4_55_flip") return Conf("large_4_flip",55).create(num_graphs,error_rate);
if(name == "large_4_60_flip") return Conf("large_4_flip",60).create(num_graphs,error_rate);
if(name == "large_4_100_flip") return Conf("large_4_flip",100).create(num_graphs,error_rate);

DEBUG(name);
assert(false);
}

static Solver* create(int num_graphs,double error_rate)
{

/*    

vector<int> error_rates({0,1,2,3,4,5,6,7,10,13,16,20,23,26,30,32,34,36,37,38,39,40,});
vector<int> num_graphs_list({10,13,16,20,23,26,30,35,40,50,60,70,80,90,100,});
vector<vector<string>> best_solvers({
vector<string>({"optimal","optimal","optimal","optimal","optimal","optimal","optimal","optimal","optimal","optimal","optimal","optimal","optimal","optimal","optimal",}),
vector<string>({"optimal","sloth_5","sloth_5","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8",}),
vector<string>({"sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8",}),
vector<string>({"sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","large_4_20","sparse_8","sparse_8",}),
vector<string>({"sparse_8","sparse_8","large_12","sparse_8","sparse_8","large_4_16","sparse_8","large_4_16","large_4_16","large_4_20","large_4_20","large_4_20","large_4_20","large_4_20_flip","large_4_20_flip",}),
vector<string>({"large_12","large_12","packed_1_3","packed_1_3","large_16","large_4_16","large_4_16","large_4_16","large_20","large_4_20","large_4_20","large_4_20","large_4_20","large_4_23","large_4_23",}),
vector<string>({"large_12","large_12","large_16","packed_1_3","large_16","large_4_16","large_4_16","large_20","large_20","large_4_23","large_4_20_flip","large_4_20_flip","large_4_26","large_4_23_flip","large_4_26",}),
vector<string>({"large_12","large_12","large_16","large_4_16","large_16","large_4_16","large_20","large_20_flip","large_20","large_4_23","large_4_20_flip","large_4_23","large_4_26","large_4_23","large_4_26",}),
vector<string>({"large_16","large_12","large_16","large_16_flip","large_16_flip","large_20","large_20","large_23","large_4_20","large_4_23","large_26_flip","large_4_23_flip","large_4_26","large_4_26_flip","large_4_23_flip",}),
vector<string>({"packed_1_4","large_16_flip","large_20","large_20","large_20","large_23_flip","large_20_flip","large_4_23_flip","large_4_23_flip","large_4_23_flip","large_4_26_flip","large_4_26_flip","large_4_26_flip","large_4_26_flip","large_4_26_flip",}),
vector<string>({"large_20","large_16_flip","large_20","large_20","large_20_flip","large_23_flip","large_4_20_flip","large_20_flip","large_26_flip","large_4_30_flip","large_35","large_35","large_35","large_35","large_4_35",}),
vector<string>({"large_20_flip","large_20_flip","large_23_flip","large_26_flip","large_26_flip","large_26_flip","large_30","large_26_flip","large_35","large_30_flip","large_35","large_26_flip","large_35_flip","large_40_flip","large_35_flip",}),
vector<string>({"large_26_flip","large_26","large_23_flip","large_26_flip","large_40","large_26_flip","large_35_flip","large_40_flip","large_35","large_45_flip","large_40","large_50","large_40","large_55","large_50_flip",}),
vector<string>({"large_35_flip","large_40_flip","large_35_flip","large_35_flip","large_45","large_40_flip","large_40_flip","large_4_50","large_50_flip","large_50_flip","large_50_flip","large_50_flip","large_60","large_50_flip","large_50_flip",}),
vector<string>({"large_40_flip","large_50_flip","large_45_flip","large_60_flip","large_60_flip","large_60_flip","large_70","large_80","large_70_flip","large_60_fli p","large_70","large_70_flip","large_80_flip","large_70_flip","large_80_flip",}),
vector<string>({"large_60_flip","large_4_60","large_60_flip","large_70_flip","large_70_flip","large_80","large_70_flip","large_100","large_80","large_80_flip","large_80_flip","large_80_flip","large_100_flip","large_80_flip","large_100",}),
vector<string>({"large_80","large_70_flip","large_70_flip","large_100_flip","large_80_flip","large_80_flip","large_80_flip","large_80_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip",}),
vector<string>({"large_80_flip","large_80_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip",}),
vector<string>({"large_100","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip",}),
vector<string>({"large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","sloth_4",}),
vector<string>({"large_100","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","sloth_5","sloth_4","sloth_4","sloth_4","sloth_4",}),
vector<string>({"large_100_flip","large_100_flip","large_4_100_flip","large_100_flip","large_100_flip","sloth_5","sloth_4","sloth_4","sloth_4","sloth_4","sloth_5","sloth_4","sloth_4","sloth_4","sloth_4",}),
});

*/

vector<int> error_rates({0,1,2,3,4,5,6,7,10,13,16,20,23,26,30,32,34,36,37,38,39,40,});
vector<int> num_graphs_list({10,13,16,20,23,26,30,35,40,50,60,70,80,90,100,});
vector<vector<string>> best_solvers({
vector<string>({"optimal","optimal","optimal","optimal","optimal","optimal","optimal","optimal","optimal","optimal","optimal","optimal","optimal","optimal","optimal",}),
vector<string>({"optimal","sloth_5","sloth_5","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8",}),
vector<string>({"sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8",}),
vector<string>({"sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","sparse_8","large_4_12_flip","sparse_8","sparse_8","sparse_8","large_4_20","sparse_8","sparse_8",}),
vector<string>({"sparse_8","sparse_8","large_12","large_4_14","sparse_8","large_4_16","sparse_8","large_4_16","large_4_16","large_4_20","large_4_20","large_4_20","large_4_16_flip","large_4_23","large_4_18_flip",}),
vector<string>({"large_10","large_12","packed_1_3","large_4_14","large_16","large_4_16","large_4_16","large_4_16","large_20","large_4_20","large_4_20","large_4_20","large_4_16_flip","large_4_23","large_4_18_flip",}),
vector<string>({"large_10","large_12","large_4_14","packed_1_3","large_16","large_4_16","large_4_16","large_20","large_20","large_4_18","large_4_20","large_4_23","large_4_26","large_4_23","large_4_26",}),
vector<string>({"large_10","large_12","large_16","large_4_16","large_16","large_4_16","large_20","large_20","large_20","large_4_23","large_4_23","large_4_23","large_23_flip","large_4_23","large_4_26",}),
vector<string>({"large_14","large_12","large_14","large_18","large_20","large_4_18","large_16","large_4_18_flip","large_18_flip","large_18_flip","large_26_flip","large_23_flip","large_23_flip","large_4_23_flip","large_4_23_flip",}),
vector<string>({"large_14","large_16","large_18_flip","large_20","large_18_flip","large_16_flip","large_20_flip","large_20_flip","large_16_flip","large_23_flip","large_23_flip","large_23_flip","large_4_26_flip","large_26_flip","large_4_26_flip",}),
vector<string>({"large_14_flip","large_20","large_18_flip","large_18_flip","large_20_flip","large_20_flip","large_20_flip","large_20_flip","large_20_flip","large_23_flip","large_23_flip","large_23_flip","large_4_30_flip","large_26_flip","large_4_30_flip",}),
vector<string>({"large_18_flip","large_23_flip","large_20_flip","large_23_flip","large_26","large_30_flip","large_30","large_23_flip","large_30_flip","large_30_flip","large_30_flip","large_35_flip","large_30_flip","large_35_flip","large_30_flip",}),
vector<string>({"large_23_flip","large_23_flip","large_26_flip","large_26_flip","large_26_flip","large_26","large_26_flip","large_35_flip","large_30_flip","large_35_flip","large_40_flip","large_40_flip","large_45_flip","large_40_flip","large_40_flip",}),
vector<string>({"large_35_flip","large_30_flip","large_40","large_35_flip","large_40_flip","large_35_flip","large_35_flip","large_40_flip","large_35_flip","large_45_flip","large_45_flip","large_50_flip","large_60","large_55_flip","large_45_flip",}),
vector<string>({"large_55_flip","large_55_flip","large_40_flip","large_60_flip","large_50_flip","large_60_flip","large_55_flip","large_60_flip","large_60_flip","large_60_flip","large_70_flip","large_80_flip","large_70_flip","large_80","large_80",}),
vector<string>({"large_60_flip","large_60_flip","large_60_flip","large_70_flip","large_60_flip","large_70_flip","large_80","large_70_flip","large_80_flip","large_80_flip","large_80_flip","large_100_flip","large_80_flip","large_100_flip","large_100_flip",}),
vector<string>({"large_60_flip","large_70_flip","large_80_flip","large_80_flip","large_80_flip","large_80_flip","large_80_flip","large_100","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip",}),
vector<string>({"large_100","large_100","large_100_flip","large_100","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip",}),
vector<string>({"large_100_flip","large_100_flip","large_4_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip",}),
vector<string>({"large_100","large_100","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip",}),
vector<string>({"large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","large_100_flip","sloth_5","sloth_4","sloth_4","sloth_4","sloth_4",}),
vector<string>({"large_100_flip","large_100_flip","large_100_flip","large_100_flip","sloth_4","sloth_5","sloth_4","sloth_4","sloth_4","sloth_4","sloth_5","sloth_4","sloth_4","sloth_4","sloth_4",}),
});




my_assert(sz(best_solvers) == sz(error_rates));


int error_index = 0;
int num_graphs_index = 0;
while(error_rates[error_index] < error_rate * 100 - 0.5) ++error_index;
while(num_graphs_list[num_graphs_index] < num_graphs) ++num_graphs_index;
return create(best_solvers[error_index][num_graphs_index],num_graphs,error_rate);
}
};

#ifndef LOCAL_TEST


int main()
{
DEBUG("");
Timer timer;

int num_graphs;
double error_rate;
cin >> num_graphs >> error_rate;

Solver *solver = LoadBalancer::create(num_graphs,error_rate);
{
cout << solver->get_n() << endl;
vector<Graph> graphs = solver->get_graphs();
rep(i,sz(graphs)) cout << graphs[i].flat_str() << endl;

rep(i,100){
string s;
cin >> s;
cout << solver->predict(Graph::parse(s)) << endl;
}
}
delete solver;

#ifdef LOCAL
ofstream ofs(getenv("STATS_PATH"));
ofs << endl;
#endif

}

#endif


