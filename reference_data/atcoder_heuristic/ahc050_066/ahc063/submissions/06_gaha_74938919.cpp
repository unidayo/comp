// TEMPLATE VERSION: 2026.02.27

// ReSharper disable CppDFANotInitializedField
// ReSharper disable CppDFANullDereference

#ifndef LOCAL
#define LOCAL 0
#endif

#ifndef DEBUG
#ifndef __clang__
#pragma GCC optimize("O2,inline")

#ifndef STABLE
#pragma GCC optimize("fast-math")
#endif

// selected O3 options
#pragma GCC optimize("vect-cost-model=dynamic")
#pragma GCC optimize("predictive-commoning")
#pragma GCC optimize("tree-partial-pre")
#pragma GCC optimize("ipa-cp-clone")

#pragma GCC target("arch=sapphirerapids,tune=sapphirerapids")
// #pragma GCC optimize("no-stack-protector")
// #pragma GCC optimize("no-exceptions")
#endif

#define NDEBUG
#endif

// #define DEBUG 1

#include <bits/stdc++.h>
#include <x86intrin.h>

// Links:
// https://www.chessprogramming.org/BMI1
// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html

using namespace std;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using u128 = __uint128_t;


#if false

/// executed even with NDEBUG
#define ASSERT(x)                                                                          \
	do {                                                                                    \
		if (!(x)) {                                                                          \
			cerr << "ASSERT(" << #x << ") FAILED IN " << __func__ << ":" << __LINE__ << endl; \
			abort();                                                                          \
		}                                                                                    \
	} while (0)

#define ASSERT_EQ(a, b)                                                                                     \
	do {                                                                                                     \
		if (!((a) == (b))) {                                                                                  \
			cerr << "ASSERT_EQ(" << #a << ", " << #b << ") FAILED IN " << __func__ << ":" << __LINE__ << endl; \
			cerr << "  " << #a << " = " << (a) << endl;                                                        \
			cerr << "  " << #b << " = " << (b) << endl;                                                        \
			abort();                                                                                           \
		}                                                                                                     \
	} while (0)
#else

#define ASSERT(x) ((void) 0)
#define ASSERT_EQ(a, b) ((void) 0)

#endif

// ATTRIBUTES
#define INIT [[gnu::constructor(101)]]
#define INLINE [[gnu::always_inline]] inline
#define NOINLINE [[gnu::noinline]]
#define HOT [[gnu::hot]]
#define COLD [[gnu::cold]]

#ifdef IDEA
#define PROFILE NOINLINE
#define PROFILE_INLINE NOINLINE
#else
#define IDEA 0
#define PROFILE
#define PROFILE_INLINE INLINE
#endif

#define noop ((void) 0)
#define INF 1'000'000'007
#define all(x) begin(x), end(x)
#define rall(x) rbegin(x), rend(x)

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define unreachable()          \
	do {                        \
		ASSERT(false);           \
		__builtin_unreachable(); \
	} while (0)

#define loop(i, n) for (remove_cvref_t<decltype(n)> i{}; i < (n); ++i)

chrono::time_point<chrono::steady_clock> _T0, _T1;
#define TIC _T0 = chrono::steady_clock::now()
#define TAC _T1 = chrono::steady_clock::now()
#define TIME_MS chrono::duration_cast<chrono::milliseconds>(_T1 - _T0).count()
#define TIME_US chrono::duration_cast<chrono::microseconds>(_T1 - _T0).count()
#define TIME_NS chrono::duration_cast<chrono::nanoseconds>(_T1 - _T0).count()

#define derr \
	if constexpr (1) cerr

template<typename T>
void DoNotOptimize(T const &val) {
	asm volatile("" : : "m"(val) : "memory");
}

template<class... Args>
void read(Args &...args) {
	((cin >> args), ...);
}

template<class T>
T read_one() {
	T temp;
	cin >> temp;
	return temp;
}

#define read_def(T, ...) \
	T __VA_ARGS__;        \
	read(__VA_ARGS__)

template<class T> vector<T> read_vec(size_t n) {
	vector<T> out(n, {});
	for (size_t i = 0; i < n; i++) cin >> out[i];
	return out;
}

template<class T> vector<T> read_vec() {
	string line;
	getline(cin, line);
	istringstream iss(line);

	T v;
	vector<T> out;
	while (iss >> v) out.push_back(v);
	return out;
}

mt19937 mt_rng(0);

void enable_fast_io() {
	ios_base::sync_with_stdio(false);
	cout.tie(nullptr);
	cin.tie(nullptr);
}

struct Hasher {
	INLINE u64 fastmix64(u64 x) const {
		x ^= x >> 32;
		x *= 0xd6e8feb86659fd93ULL;
		x ^= x >> 32;
		return x;
	}

	static u64 splitmix64_finalizer(u64 x) {
		x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
		x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
		return x ^ (x >> 31);
	}

	INLINE u64 splitmix64_sequence(u64 x) const { return splitmix64_finalizer(x + 0x9e3779b97f4a7c15ULL); }

	INLINE u64 murmur3_fmix64(u64 x) const {
		x = (x ^ (x >> 33)) * 0xff51afd7ed558ccdULL;
		x = (x ^ (x >> 33)) * 0xc4ceb9fe1a85ec53ULL;
		return x ^ (x >> 33);
	}

	INLINE u64 combine2(u64 h1, u64 h2) const { return (h1 + 0x9e3779b97f4a7c15ULL) * (h2 + 0xbf58476d1ce4e5b9ULL); }

	template<typename... Hs>
	INLINE u64 combine(u64 h0, Hs... hs) const {
		u64 h = h0;
		((h = combine2(h, hs)), ...);
		return h;
	}

	INLINE u64 operator()(u64 x) const { return fastmix64(x); }

	template<typename Out>
	INLINE Out reduce(u64 h) const {
		static_assert(std::is_unsigned_v<Out>, "Out must be unsigned");
		static_assert(sizeof(Out) <= sizeof(u64), "Out too large");

		if constexpr (sizeof(Out) <= 4) h ^= h >> 32;
		if constexpr (sizeof(Out) <= 2) h ^= h >> 16;
		if constexpr (sizeof(Out) <= 1) h ^= h >> 8;
		return static_cast<Out>(h);
	}

} hasher;

struct Fastrand {
	u64 state_u64;
	u64 pool_u64 = 0;
	bool has_pool_u32 = false;

	explicit Fastrand(u64 seed) : state_u64(seed) {}

	INLINE u64 next_u64() {
		state_u64 += 0xa0761d6478bd642f;
		u128 m = (u128) state_u64 * 0xe7037ed1a0b428db;
		return (u64) m ^ (u64) (m >> 64);
	}

	INLINE u32 next_u32() {
		if (has_pool_u32) {
			has_pool_u32 = false;
			return pool_u64 >> 32;
		}
		pool_u64 = next_u64();
		has_pool_u32 = true;
		return pool_u64;
	}

	INLINE u32 next_u32(u32 max) {
		u32 x = next_u32();
		u64 m = (u64) x * max;
		u32 l = m;

		u32 threshold = (0U - max) % max;
		if (l < threshold) [[unlikely]] {
			while (l < threshold) {
				u64 fresh = next_u64();
				x = fresh;
				m = (u64) x * max;
				l = m;
			}
		}
		return m >> 32;
	}

	INLINE int next_int(int min, int max) { return min + next_u32(max - min); }

	u64 bool_pool = 0;

	INLINE bool next_bool() {
		if (bool_pool <= 1) {
			bool_pool = next_u64();
			bool_pool |= (1ULL << 63); // add sentinel bit
		}
		bool res = bool_pool & 1;
		bool_pool >>= 1;
		return res;
	}

	INLINE double next_double() {
		// double has 53 bits of mantissa precision
		return (next_u64() >> 11) * 0x1.0p-53;
	}
} rng(212);

template<typename KT, typename VT>
struct PiecewiseLerp {
	const vector<KT> keys;
	const vector<VT> values;

	PiecewiseLerp(initializer_list<KT> keys, initializer_list<VT> values) : keys(keys), values(values) {
		assert(keys.size() == values.size());
		assert(is_sorted(all(keys)));
	}

	INLINE VT operator()(KT key) const {
		if (key <= keys.front()) return values.front();
		if (key >= keys.back()) return values.back();

		for (size_t i = 0; i < keys.size() - 1; i++)
			if (key <= keys[i + 1])
				return values[i] + (values[i + 1] - values[i]) * double(key - keys[i]) / (keys[i + 1] - keys[i]);

		unreachable();
	}
};

template<size_t Capacity, class ValType>
struct FlatHashMap {
	static_assert(Capacity > 0, "Capacity must be > 0");
	static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two");

	using KeyType = u64;
	static constexpr KeyType EMPTY = 0;

	size_t size = 0;
	array<KeyType, Capacity> keys{}; // zero-initialized => all EMPTY
	array<ValType, Capacity> values;

	void clear() {
		size = 0;
		keys.fill(0);
	}

	pair<size_t, bool> find_slot(KeyType key) {
		ASSERT(key != EMPTY);
		size_t idx = hasher.splitmix64_finalizer(key) & (Capacity - 1);

		loop(step, Capacity) {
			const auto k = keys[idx];
			if (k == key) return {idx, false}; // already present

			if (k == EMPTY) {
				++size;
				keys[idx] = key;    // claim slot
				return {idx, true}; // newly inserted
			}
			idx = (idx + 1) & (Capacity - 1); // linear probe
		}
		unreachable();
	}
};

template<size_t Capacity>
struct FlatHashSet {
	static_assert(Capacity > 0, "Capacity must be > 0");
	static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two");

	using KeyType = u64;
	static constexpr KeyType EMPTY = 0;

	size_t size = 0;
	array<KeyType, Capacity> keys{}; // zero-initialized => all EMPTY

	void clear() {
		size = 0;
		keys.fill(0);
	}

	PROFILE_INLINE bool maybe_insert(KeyType key) {
		ASSERT(key != EMPTY);
		size_t idx = hasher.splitmix64_finalizer(key) & (Capacity - 1);

		loop(step, Capacity) {
			const auto k = keys[idx];
			if (k == key) return false; // already present

			if (k == EMPTY) {
				++size;
				keys[idx] = key; // claim slot
				return true;     // newly inserted
			}
			idx = (idx + 1) & (Capacity - 1); // linear probe
		}
		unreachable();
	}
};

template<size_t Capacity>
struct BinaryHashSet {
	static_assert(Capacity > 0, "Capacity must be > 0");
	static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two");
	using KeyType = u64;

	bitset<Capacity> bits;
	size_t size = 0;

	void clear() {
		size = 0;
		bits.reset();
	}

	bool maybe_insert(KeyType key) {
		size_t idx = hasher.splitmix64_finalizer(key) & (Capacity - 1);
		if (bits._Unchecked_test(idx)) return false;
		bits._Unchecked_set(idx);
		return true;
	}
};

template<class T>
INLINE bool odometer_increment(T &state, const T &limit, int n) {
	for (auto i = n; i-- > 0;) {
		if (++state[i] < limit[i]) return true;
		state[i] = 0; // overflow single digit
	}
	return false; // full overflow
}

INIT static void _START_TIMER() {
	TIC, TAC;
}

/////////////////////
///  HEADER ENDS  ///
/////////////////////

constexpr int MAX_S = 192;
constexpr int MAX_C = 7;
constexpr int MAX_N = 16;
constexpr int MAX_AREA = 256;

constexpr int BOARD_SNK = 1;
constexpr int COLOR_RED = 2;
constexpr int BOARD_LIM = 9;

namespace seed {
int N; // grid size between [8,16]
int AREA;

int S; // sequence length [N^2/4,3*N^2/4]
int C; // number of colors [3,7]

u8 sequence[MAX_S];
u8 foodmap[MAX_AREA];
u8 edgemap[MAX_AREA];
u8 valid_dirs_mask[MAX_AREA];

constexpr array<char, 4> DIRS4_CHAR{'R', 'L', 'D', 'U'};
constexpr array<int, 4> DIRS4X{1, -1, 0, 0};
array<int, 4> DIRS4F{1, -1, 0, 0};

COLD void load_from_stdin() {
	read(N, S, C);
	cerr << "[DATA] N = " << N << endl;
	cerr << "[DATA] S = " << S << endl;
	cerr << "[DATA] C = " << C << endl;

	ASSERT(C >= 3 and C <= 7);
	ASSERT(N >= 8 and N <= 16);
	ASSERT(S >= N * N / 4 and S <= 3 * N * N / 4);

	AREA = N * N;
	DIRS4F = {1, -1, N, -N};

	loop(i, S) {
		read_def(int, v);
		ASSERT(v >= 1 and v <= C);
		if (i <= 4) ASSERT(v == 1);
		sequence[i] = v - 1 + COLOR_RED;
	}

	loop(y, N) {
		loop(x, N) {
			int pos = y * N + x;
			read_def(int, v);
			ASSERT(v >= 0 and v <= C);
			if (x == 0 and y <= 4) ASSERT(v == 0);
			if (v > 0) v = v - 1 + COLOR_RED;
			foodmap[pos] = v;
		}
	}

	{ // initialize edgemap
		fill_n(edgemap, AREA, 0);
		// border_penalty coefficients
		constexpr int EDGE_COEF = 4;
		constexpr int CORN_COEF = 12;
		loop(i, N) {
			edgemap[i] = EDGE_COEF;
			edgemap[i * N] = EDGE_COEF;
			edgemap[i * N + N - 1] = EDGE_COEF;
			edgemap[(N - 1) * N + i] = EDGE_COEF;
		}
		int corners[4] = {0, N - 1, (N - 1) * N, (N - 1) * N + N - 1};
		loop(i, 4) edgemap[corners[i]] = CORN_COEF;
	}

	{
		loop(pos, AREA) {
			const int y = pos / N;
			const int x = pos % N;
			u8 mask = 0;
			if (x + 1 < N) mask |= 1u << 0;
			if (x > 0) mask |= 1u << 1;
			if (y + 1 < N) mask |= 1u << 2;
			if (y > 0) mask |= 1u << 3;
			valid_dirs_mask[pos] = mask;
		}
	}
}

} // namespace seed

COLD int get_distance(int a, int b) {
	return abs(a / seed::N - b / seed::N) + abs(a % seed::N - b % seed::N);
}

template<int size, int bits, typename word_t>
struct PackedArray {
	static_assert(size > 0);
	static_assert(bits > 0);
	static_assert(std::is_integral_v<word_t>);
	static_assert(std::is_unsigned_v<word_t>);
	static constexpr int WORD_BITS = sizeof(word_t) * 8;
	static_assert(bits <= WORD_BITS);

	static_assert(WORD_BITS % bits == 0);
	static constexpr int ITEMS_PER_WORD = WORD_BITS / bits;
	static constexpr int WORD_CNT = (size + ITEMS_PER_WORD - 1) / ITEMS_PER_WORD;
	static constexpr word_t MASK = bits == WORD_BITS ? ~word_t(0) : (word_t(1) << bits) - 1;

	word_t words[WORD_CNT]{};

	INLINE constexpr void set(size_t index, word_t value) noexcept {
		if constexpr (bits == WORD_BITS) {
			words[index] = value;
		} else {
			const size_t word = index / ITEMS_PER_WORD;
			const size_t offset = index % ITEMS_PER_WORD * bits;
			words[word] &= ~(MASK << offset);
			words[word] |= (value & MASK) << offset;
		}
	}

	INLINE constexpr word_t get(size_t index) const noexcept {
		if constexpr (bits == WORD_BITS) {
			return words[index];
		} else {
			const size_t word = index / ITEMS_PER_WORD;
			const size_t offset = index % ITEMS_PER_WORD * bits;
			return words[word] >> offset & MASK;
		}
	}
};

#define MAX_BITE 5

struct Scorer {
	u8 good_size;
	// u8 heal_size;
	u8 border_penalty;
	u8 stall_turns;

	INLINE i16 compute() const {
		i16 v = 128 * good_size;
		// v += 128 * heal_size;
		v -= border_penalty;
		// if (stall_turns > 0) v -= 15 * (stall_turns - 0);
		v -= 16 * stall_turns;
		return v;
	}
};

template<int S, int N>
struct GameState {
	static constexpr int AREA = N * N;
	static constexpr int HEAL_CAP = 6;

	PackedArray<AREA, 4, u8> board;
	PackedArray<S, 4, u8> color;
	u8 shape[S]; // tail..head
	u8 size;
	u8 head_idx;
	Scorer scorer;

	using _S = GameState<MAX_S, MAX_N>;

	PROFILE_INLINE Scorer preview_empty() const {
		Scorer next = scorer;
		++next.stall_turns;
		return next;
	}

	PROFILE_INLINE Scorer preview_food(const u8 dest, const u8 dest_color) const {
		Scorer next = scorer;
		next.stall_turns = 0;
		if (size == scorer.good_size and dest_color == seed::sequence[size]) {
			++next.good_size;
		}
		next.border_penalty -= seed::edgemap[dest];
		return next;
	}

	PROFILE_INLINE Scorer preview_self(const u8 dest, u8 &self_size) const {
		Scorer next = scorer;
		next.stall_turns = 0;
		self_size = size;

		int idx = pre_tail_idx();
		int lim = head_idx - next.good_size + (MAX_BITE + 2);
		for (; idx <= lim; ++idx) {
			u8 pretail = shape[idx];
			if (pretail == dest) break;
			next.border_penalty += seed::edgemap[pretail];
			--self_size;
		}
		next.good_size = min<u8>(next.good_size, self_size);
		return next;
	}

	static GameState init() {
		GameState state{};
		state.size = 5;
		state.head_idx = 4;
		state.scorer.good_size = 5;

		loop(pos, seed::AREA) {
			int food = seed::foodmap[pos];
			state.board.set(pos, food);
			if (food > 0) state.scorer.border_penalty += seed::edgemap[pos];
		}

		loop(y, 5) {
			int p = y * seed::N;
			state.shape[y] = p;
			state.color.set(y, COLOR_RED);
			if (p != 0) state.board.set(p, BOARD_SNK);
		}
		return state;
	}

	COLD void verify() const {
		int apples = 0;
		const int tail_idx = get_tail_idx();
		loop(y, seed::N) {
			loop(x, seed::N) {
				bool overlap = false;
				int pos = y * seed::N + x;

				auto val = board.get(pos);
				ASSERT(val < BOARD_LIM);
				if (val >= COLOR_RED && val != BOARD_SNK) ++apples;

				for (int i = tail_idx + 1; i <= head_idx; ++i) {
					if (pos == shape[i]) overlap = true;
				}

				if (overlap) ASSERT(val == BOARD_SNK);
				else ASSERT(val != BOARD_SNK);
			}
		}
		loop(i, size) {
			int c = color.get(i);
			ASSERT(c > 0 && c != BOARD_SNK);
		}
		ASSERT(apples + size == seed::S);
	}

	INLINE int get_tail_idx() const { return head_idx - size + 1; }

	INLINE int pre_tail_idx() const { return head_idx - size + 2; }

	PROFILE_INLINE void append_head(u8 dest) {
		if (head_idx == S - 1) {
			memmove(shape, shape + S - size + 1, size - 1);
			head_idx = size - 1;
		} else ++head_idx;
		shape[head_idx] = dest;
	}

	PROFILE_INLINE void move_empty(u8 dest) {
		append_head(dest);
		const int tail = shape[get_tail_idx()];
		assert(board.get(tail) == BOARD_SNK);
		board.set(dest, BOARD_SNK);
		board.set(tail, 0);
	}

	PROFILE_INLINE void move_food(u8 dest, u8 dest_color) {
		append_head(dest);
		assert(size < S);
		board.set(dest, BOARD_SNK);
		color.set(size++, dest_color);
	}

	PROFILE_INLINE void move_self(u8 dest, u8 self_size) {
		append_head(dest);
		int tail_idx = get_tail_idx();
		u8 new_size = size;
		while (new_size > self_size) {
			const int tail = shape[tail_idx++];
			board.set(tail, color.get(--new_size));
		}
		size = new_size;
		assert(board.get(dest) == BOARD_SNK);
	}

	COLD string repr() const;
};

constexpr u32 MAX_WIDTH = 16'384;
constexpr u32 MAX_DEPTH = 1500;
constexpr u32 MAX_BUDGET = MAX_WIDTH * 2000 * MAX_N;

u32 CUR_DEPTH_LIMIT = MAX_DEPTH;
u32 CUR_BUDGET = 0;

#define TIMED 1
constexpr u32 TIME_BUDGET_MS = 1980;

bool budget_exceeded() {
#if TIMED
	TAC;
	return TIME_MS >= TIME_BUDGET_MS;
#else
	return CUR_BUDGET >= MAX_BUDGET;
#endif
}

struct Trace {
	u16 didx : 2;
	u16 pslo : 14;
};

Trace traces_table[MAX_DEPTH * MAX_WIDTH]{};
Trace *child_traces = &traces_table[0];
Trace *finished_trace_ptr = nullptr;
int finished_slot = -1;

bool game_finished_bef = false;

i16 *__restrict beam_new_score = new i16[MAX_WIDTH]{};
vector<char> top_moves1;
vector<char> top_moves2;

void chase_path(int depth, int slot, vector<char> &moves) {
	moves.clear();

	cerr << slot << " ";
	while (--depth >= 0) {
		auto [didx, pslo] = traces_table[depth * MAX_WIDTH + slot];
		moves.push_back(seed::DIRS4_CHAR[didx]);
		slot = pslo;
		cerr << slot << " ";
	}
	cerr << endl;
	reverse(all(moves));
}

u32 fallback_slot = UINT32_MAX;

void update_top_moves_after_search2(int depth) {
	bool game_finished_now = finished_slot != -1;
	if constexpr (IDEA) {
		TAC;
		println(cerr,
		        ">> clock={:.02f}ms | depth={} | fin_bef={:d} | fin_now={:d}",
		        TIME_US / 1000.0,
		        depth,
		        game_finished_bef,
		        game_finished_now);
	}
	if (game_finished_bef and !game_finished_now) return;
	if (not game_finished_now and not top_moves2.empty()) return;
	int depth_lim = top_moves2.empty() ? INT_MAX : top_moves2.size();
	if (game_finished_now and game_finished_bef and depth >= depth_lim) return;

	int top_slot = finished_slot;
	if (top_slot == -1) {
		top_slot = fallback_slot;
		depth -= 2;
	}

	chase_path(depth, top_slot, top_moves2);
	for (auto c : top_moves2) cerr << c << ' ';
	cerr << endl;

	if (game_finished_now) {
		game_finished_bef = true;
		CUR_DEPTH_LIMIT = depth - 1;
	}
}

static inline u64 *active_words_pool = nullptr;
static inline u64 *__restrict active_words_old;
static inline u64 *__restrict active_words_new;

template<int S, int N>
struct GameSearch {
	using State = GameState<S, N>;
	static constexpr u32 BUCKETS = State::AREA * 4;
	static constexpr u32 SLOTS = MAX_WIDTH / BUCKETS;
	static constexpr u32 WIDTH = SLOTS * BUCKETS;
	static constexpr u32 ACTIVE_WORDS = (WIDTH + 63) / 64;

	static inline State *state_pool = nullptr;
	static inline State *__restrict beam_old_state;
	static inline State *__restrict beam_new_state;

	GameSearch() = delete;

	HOT static void commit_child(const State &parent,
	                             const u16 parent_slot,
	                             const u32 slot,
	                             const i16 child_score,
	                             const Scorer &child_scorer,
	                             u32 dir) { //
		State &child = beam_new_state[slot];
		child = parent;
		child.scorer = child_scorer;

		beam_new_score[slot] = child_score;
		active_words_new[slot >> 6] |= 1ULL << (slot & 63);
		child_traces[slot] = {(u16) dir, parent_slot};
		const u8 child_good_size = child_scorer.good_size;
		if (child_good_size == seed::S) {
			finished_trace_ptr = &child_traces[slot];
			finished_slot = slot;
		}
	}

	HOT PROFILE_INLINE static void try_expand_dir(const State &parent,
	                                              const u16 parent_slot,
	                                              const u32 dest,
	                                              const u32 dir) { //
		const u8 dest_color = parent.board.get(dest);
		const u32 slot_base = (dest * 4 + (dir & 1) * 2) * SLOTS + rng.next_u32(SLOTS);

		switch (dest_color) {
			case 0: {
				const Scorer child_scorer = parent.preview_empty();
				const u32 child_clean = child_scorer.good_size == parent.size;
				const u32 slot = slot_base + child_clean * SLOTS;
				const i16 child_score = child_scorer.compute();
				if (likely(child_score <= beam_new_score[slot])) return;
				commit_child(parent, parent_slot, slot, child_score, child_scorer, dir);
				beam_new_state[slot].move_empty(dest);
				return;
			}

			case BOARD_SNK: {
				u8 self_size;
				const Scorer child_scorer = parent.preview_self(dest, self_size);
				if (parent.scorer.good_size - self_size > MAX_BITE) return; // do not allow biting too much

				const u32 child_clean = child_scorer.good_size == self_size;
				const u32 slot = slot_base + child_clean * SLOTS;
				const i16 child_score = child_scorer.compute();
				if (likely(child_score <= beam_new_score[slot])) return;
				commit_child(parent, parent_slot, slot, child_score, child_scorer, dir);
				beam_new_state[slot].move_self(dest, self_size);
				return;
			}

			default: {
				const Scorer child_scorer = parent.preview_food(dest, dest_color);
				const u32 child_clean = child_scorer.good_size == parent.size + 1;
				const u32 slot = slot_base + child_clean * SLOTS;
				const i16 child_score = child_scorer.compute();
				if (likely(child_score <= beam_new_score[slot])) return;
				commit_child(parent, parent_slot, slot, child_score, child_scorer, dir);
				beam_new_state[slot].move_food(dest, dest_color);
			}
		}
	}

	PROFILE static void expand(const State &parent, u16 parent_slot, u8 blocked_mask) {
		const u32 head_pos = parent.shape[parent.head_idx];
		const u8 valid_mask = seed::valid_dirs_mask[head_pos] & ~blocked_mask;

		if (valid_mask & (1u << 0)) try_expand_dir(parent, parent_slot, head_pos + 1, 0);
		if (valid_mask & (1u << 1)) try_expand_dir(parent, parent_slot, head_pos - 1, 1);
		if (valid_mask & (1u << 2)) try_expand_dir(parent, parent_slot, head_pos + N, 2);
		if (valid_mask & (1u << 3)) try_expand_dir(parent, parent_slot, head_pos - N, 3);
	}

	static void clear_before_search() {
		if (state_pool == nullptr) {
			state_pool = new State[WIDTH * 2];
			active_words_pool = new u64[ACTIVE_WORDS * 2];
		}

		beam_old_state = state_pool + 0;
		beam_new_state = state_pool + WIDTH;

		memset(active_words_pool, 0, ACTIVE_WORDS * 2 * sizeof(*active_words_pool));
		active_words_old = active_words_pool + 0;
		active_words_new = active_words_pool + ACTIVE_WORDS;

		memset(beam_new_score, 0, MAX_WIDTH * sizeof(*beam_new_score));
		child_traces = &traces_table[0];
		finished_trace_ptr = nullptr;
		finished_slot = -1;
	}

	static void clear_before_expand() {
		swap(beam_old_state, beam_new_state);
		memset(beam_new_score, 0, WIDTH * sizeof(*beam_new_score));

		swap(active_words_old, active_words_new);
		memset(active_words_new, 0, ACTIVE_WORDS * sizeof(*active_words_new));
	}

	static void run_search() {
		clear_before_search();
		expand(State::init(), 0, 1u << 3);
		fallback_slot = UINT32_MAX;

		u32 depth = 1;
		u32 last_expanded_slot = 0;
		for (; depth < CUR_DEPTH_LIMIT; ++depth) {
			if (last_expanded_slot == UINT32_MAX) break;
			if (finished_trace_ptr != nullptr) break;
			if (budget_exceeded()) break;
			clear_before_expand();

			child_traces = &traces_table[depth * MAX_WIDTH];
			const Trace *const parent_traces = child_traces - MAX_WIDTH;
			const State *const old_state = beam_old_state;
			const u32 remaining_turns = CUR_DEPTH_LIMIT - depth;

			last_expanded_slot = UINT32_MAX;

			loop(word_idx, ACTIVE_WORDS) {
				u64 bits = active_words_old[word_idx];
				while (bits != 0) {
					const u32 bit_idx = countr_zero(bits);
					const u32 slot = (word_idx << 6) + bit_idx;
					const State &parent = old_state[slot];
					const u32 remaining_food = seed::S - parent.scorer.good_size;
					if (remaining_food <= remaining_turns) {
						expand(parent, slot, 1u << (parent_traces[slot].didx ^ 1));
					}
					bits &= bits - 1;
					last_expanded_slot = slot;
				}
			}

			if (last_expanded_slot != UINT32_MAX) {
				fallback_slot = last_expanded_slot;
			}

			if constexpr (IDEA) {
				if ((depth & 63) == 63) println(cerr, "DEPTH={} EXPANDED={}", depth, WIDTH);
			}
			if constexpr (!TIMED) CUR_BUDGET += WIDTH * N;
		}

		update_top_moves_after_search2(depth);
	}
};

template<int S = 32, int N = 8>
void dispatch() {
	if constexpr (S > MAX_S || N > MAX_N) unreachable();
	else if (seed::S > S) dispatch<S + 32, N>();
	else if (seed::N > N) dispatch<S, N + 1>();
	else {
		GameSearch<S, N>::run_search();
	}
}

int test();

int main() {
	enable_fast_io();

#if TEST
	test();
	exit(0);
#endif

	seed::load_from_stdin();
	if constexpr (IDEA) {
		cerr << GameState<MAX_S, MAX_N>::init().repr() << endl;
	}

	// TIC;
	int searches_cnt = 0;
	do {
		++searches_cnt;
		dispatch();
	} while (!budget_exceeded());

	TAC;

#ifdef LOCAL
	cerr << "[DATA] time = " << TIME_US / 1000.0 << endl;
	if (not game_finished_bef) cerr << "[DATA] fail = 1" << endl;
	else cerr << "[DATA] fail = 0" << endl;
	cerr << "[DATA] searches = " << searches_cnt << endl;

	if (seed::N <= 10) cerr << "[DATA] typN = S" << endl;
	else if (seed::N <= 13) cerr << "[DATA] typN = M" << endl;
	else cerr << "[DATA] typN = L" << endl;

	if (seed::C <= 4) cerr << "[DATA] typC = S" << endl;
	else cerr << "[DATA] typC = L" << endl;

	double ratio = (double) seed::S / seed::AREA;
	if (ratio <= 0.5) cerr << "[DATA] typS = S" << endl;
	else cerr << "[DATA] typS = L" << endl;

#endif

	for (auto m : top_moves2) cout << m << ' ';
	cout << endl;
}

/////////////////////
///  TRASH BELOW  ///
/////////////////////

#ifdef EOF

COLD string repr_helper_body_symbol(int prev, int cur, int next) {
	enum { U = 1, D = 2, L = 4, R = 8 };

	auto dir = [&](int a, int b) {
		int d = b - a;
		if (d == -seed::N) return U;
		if (d == seed::N) return D;
		if (d == -1) return L;
		if (d == 1) return R;
		unreachable();
	};

	switch (dir(cur, prev) | dir(cur, next)) {
		case U | D: return "?";
		case L | R: return "?";
		case U | R: return "?";
		case U | L: return "?";
		case D | R: return "?";
		case D | L: return "?";
		default: unreachable();
	}
};

COLD string repr_helper_connector(const string &s1, const string &s2) {
	if (s1 == "?" && s2 == "?") return "?";
	if (s1 == "?" && s2 == "?") return "?";
	if (s1 == "?" && s2 == "?") return "?";
	if (s1 == "?" && s2 == "?") return "?";
	if (s1 == "?" && s2 == "?") return "?";
	if (s1 == "?" && s2 == "?") return "?";
	if (s1 == "?" && s2 == "?") return "?";
	if (s1 == "?" && s2 == "?") return "?";
	if (s1 == "?" && s2 == "?") return "?";
	return " ";
}

template<int S, int N>
COLD string GameState<S, N>::repr() const {
	verify();

	stringstream ss;
	println(ss, ">> SIZE: {} | GOOD: {}", size, scorer.good_size);
	ss << ">> ";
	loop(i, size) ss << char('0' + color.get(i));
	ss << "\n>> ";
	loop(i, seed::S) ss << char('0' + seed::sequence[i]);
	ss << '\n';

	string m[seed::AREA];

	const int tail_idx = get_tail_idx();
	for (int i = tail_idx + 1; i < head_idx; ++i) {
		m[shape[i]] = repr_helper_body_symbol(shape[i - 1], shape[i], shape[i + 1]);
	}
	loop(y, seed::N) {
		string prev;
		loop(x, seed::N) {
			int pos = y * seed::N + x;
			int val = board.get(pos);
			string s = ".";
			if (pos == shape[head_idx]) s = "?";
			else if (val == BOARD_SNK) s = m[pos];
			else if (val >= COLOR_RED) s = '0' + val;

			string connector = repr_helper_connector(prev, s);
			ss << connector << s;
			prev = s;
			if (pos % seed::N == seed::N - 1) ss << '\n';
		}
	}
	return ss.str();
}

#endif
