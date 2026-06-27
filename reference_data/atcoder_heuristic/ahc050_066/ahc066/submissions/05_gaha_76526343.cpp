
// gaha

// #define NDEBUG

#ifdef __cplusplus

// #define DEBUG
// #define NDEBUG

#ifndef IDEA
#define IDEA 0
#endif

#ifndef LOCAL
#define LOCAL 0
#endif

#include <bits/stdc++.h>
#include <cassert>
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

using f32 = float;
using f64 = double;

#if 1
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
#define NOINLINE [[gnu::noinline]]
#define INLINE [[gnu::always_inline]] inline

#define FLATTEN [[gnu::flatten]]
#define COLD [[gnu::cold]]
#define HOT [[gnu::hot]]

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

#define LOOP_SELECT(_1, _2, _3, NAME, ...) NAME
#define loop(...) LOOP_SELECT(__VA_ARGS__, loop3, loop2, loop1)(__VA_ARGS__)
#define loop1(n) for (remove_cvref_t<decltype(n)> _i{}; _i < (n); ++_i)
#define loop2(i, n) for (remove_cvref_t<decltype(n)> i{}; i < (n); ++i)
#define loop3(i, n, m) for (remove_cvref_t<decltype(n)> i{n}; i < (m); ++i)

#define time_t chrono::time_point<chrono::steady_clock>

time_t _T0, _T1;
#define TIC _T0 = chrono::steady_clock::now()
#define TAC _T1 = chrono::steady_clock::now()
#define TIME_MS chrono::duration_cast<chrono::milliseconds>(_T1 - _T0).count()
#define TIME_US chrono::duration_cast<chrono::microseconds>(_T1 - _T0).count()
#define TAC_MS chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - _T0).count()
#define TAC_US chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - _T0).count()
#define TAC_MSF (chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - _T0).count() / 1000.0f)

INLINE time_t time() {
	return chrono::steady_clock::now();
}

INLINE int time_ms(time_t start) {
	auto end = chrono::steady_clock::now();
	return chrono::duration_cast<chrono::milliseconds>(end - start).count();
}

INLINE int time_us(time_t start) {
	auto end = chrono::steady_clock::now();
	return chrono::duration_cast<chrono::microseconds>(end - start).count();
}

#define derr \
	if constexpr (0) cerr

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

void enable_fast_io() {
	ios_base::sync_with_stdio(false);
	cout.tie(nullptr);
	cin.tie(nullptr);
}

template<class T, size_t N, size_t... Ns>
struct md_array_impl {
	using type = std::array<typename md_array_impl<T, Ns...>::type, N>;
};

template<class T, size_t N>
struct md_array_impl<T, N> {
	using type = std::array<T, N>;
};

template<class T, size_t... Ns>
using md_array = md_array_impl<T, Ns...>::type;

template<class T, size_t... Ns>
using mdarr = md_array_impl<T, Ns...>::type;

struct Mixer {
	INLINE constexpr u64 splitmix64(u64 x) const {
		x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
		x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
		return x ^ (x >> 31);
	}

	INLINE constexpr u64 splitmix64_sequence(u64 &x) const {
		x += 0x9e3779b97f4a7c15ULL;
		return splitmix64(x);
	}

	INLINE constexpr u64 murmur3_fmix64(u64 x) const {
		x = (x ^ (x >> 33)) * 0xff51afd7ed558ccdULL;
		x = (x ^ (x >> 33)) * 0xc4ceb9fe1a85ec53ULL;
		return x ^ (x >> 33);
	}

	INLINE u64 _combine2(u64 h1, u64 h2) const { return (h1 + 0x9e3779b97f4a7c15ULL) * (h2 + 0xbf58476d1ce4e5b9ULL); }

	template<typename... Hs>
	INLINE u64 combine(u64 h0, Hs... hs) const {
		u64 h = h0;
		((h = _combine2(h, hs)), ...);
		return h;
	}

	INLINE u64 operator()(u64 x) const { return splitmix64(x); }

	template<typename Out>
	INLINE Out reduce(u64 h) const {
		static_assert(std::is_unsigned_v<Out>, "Out must be unsigned");
		static_assert(sizeof(Out) <= sizeof(u64), "Out too large");

		if constexpr (sizeof(Out) <= 4) h ^= h >> 32;
		if constexpr (sizeof(Out) <= 2) h ^= h >> 16;
		if constexpr (sizeof(Out) <= 1) h ^= h >> 8;
		return static_cast<Out>(h);
	}
} mixer;

struct Fastrand {
	u64 s0, s1;

	explicit Fastrand(u64 seed) {
		s0 = mixer.splitmix64_sequence(seed);
		s1 = mixer.splitmix64_sequence(seed);
		if ((s0 | s1) == 0) s0 = 1;
	}

	INLINE static u64 rotl(u64 x, int k) { return (x << k) | (x >> (64 - k)); }

	INLINE u64 next_u64_xorshift128p() {
		u64 x = s0;
		u64 y = s1;
		s0 = y;
		x ^= x << 23;
		s1 = x ^ y ^ (x >> 17) ^ (y >> 26);
		return s1 + y;
	}

	// xoroshiro128+ 1.0
	INLINE u64 next_u64_xoroshiro128p() {
		const u64 result = s0 + s1;
		s1 ^= s0;
		s0 = rotl(s0, 24) ^ s1 ^ (s1 << 16);
		s1 = rotl(s1, 37);
		return result;
	}

	// xoroshiro128++ 1.0
	INLINE u64 next_u64_xoroshiro128pp() {
		const u64 result = rotl(s0 + s1, 17) + s0;
		s1 ^= s0;
		s0 = rotl(s0, 49) ^ s1 ^ (s1 << 21);
		s1 = rotl(s1, 28);
		return result;
	}

	// xoroshiro128** 1.0
	INLINE u64 next_u64_xoroshiro128ss() {
		const u64 result = rotl(s0 * 5, 7) * 9;
		s1 ^= s0;
		s0 = rotl(s0, 24) ^ s1 ^ (s1 << 16);
		s1 = rotl(s1, 37);
		return result;
	}

	INLINE u64 next_u64() { return next_u64_xoroshiro128ss(); }

	u64 pool_u32 = 0;
	bool has_pool_u32 = false;

	INLINE u32 next_u32() {
		if (has_pool_u32) {
			has_pool_u32 = false;
			return pool_u32 >> 32;
		}
		pool_u32 = next_u64();
		has_pool_u32 = true;
		return pool_u32;
	}

	INLINE u32 clip_u32(const u64 val, const u64 max) const { return (val * max) >> 32; }

	INLINE u32 next_u32(u64 max) { return clip_u32(next_u32(), max); }

	INLINE u32 next_u32(u64 min, u64 max) { return clip_u32(next_u32(), max - min) + min; }

	INLINE u32 clip_u32_exact(u64 val, const u64 max) {
		u64 m = (u64) val * max;
		u32 l = m;

		const u32 threshold = (0U - max) % max;
		if (l < threshold) [[unlikely]] {
			while (l < threshold) {
				m = next_u64() * max;
				l = m;
			}
		}
		return m >> 32;
	}

	u64 pool_bool = 0;
	bool has_pool_bool = false;

	INLINE bool next_bool() {
		if (has_pool_bool) {
			has_pool_bool = false;
			return pool_bool & (1ULL << 31);
		}
		has_pool_bool = true;
		pool_bool = next_u64();
		return pool_bool & 1ULL;
	}

	INLINE double next_double() {
		// has 53 bits of mantissa precision
		return (next_u64() >> 11) * 0x1.0p-53;
	}

	INLINE float next_float() {
		// has 24 bits of mantissa precision
		return (next_u32() >> 8) * 0x1.0p-24f;
	}
} rnd(212);

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
	array<KeyType, Capacity> keys{}; // zero means empty
	array<ValType, Capacity> values;

	void clear() {
		size = 0;
		keys.fill(0);
	}

	pair<size_t, bool> find_slot(KeyType key) {
		// assert(key != EMPTY);
		size_t idx = mixer.splitmix64(key) & (Capacity - 1);

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

	pair<size_t, bool> find_slot_force(KeyType key) {
		// assert(key != EMPTY);
		size_t idx = mixer.splitmix64(key) & (Capacity - 1);

		const auto k = keys[idx];
		if (k == key) return {idx, false}; // already present

		if (k == EMPTY) ++size;
		keys[idx] = key;    // claim slot
		return {idx, true}; // newly inserted
	}
};

template<size_t Capacity>
struct FlatHashSet {
	static_assert(Capacity > 0, "Capacity must be > 0");
	static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two");

	using KeyType = u64;
	static constexpr KeyType EMPTY = 0;

	size_t size = 0;
	array<KeyType, Capacity> keys{}; // zero means empty

	void clear() {
		size = 0;
		keys.fill(0);
	}

	bool maybe_insert(KeyType key) {
		assert(key != EMPTY);
		size_t idx = mixer.splitmix64(key) & (Capacity - 1);

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

void show_hostname() {
	array<char, 256> buf{};
	gethostname(buf.data(), buf.size());
	cerr << "HOST: " << buf.data() << endl;
}

mt19937 rng(212);

#endif

constexpr bool is_power_of_2(const int v) {
	return (v & (v - 1)) == 0;
}

#define endl '\n';

/////////////////////
///  HEADER ENDS  ///
/////////////////////

enum : int {
	R = 0,
	D = 1,
	L = 2,
	U = 3,
};

enum : int {
	MAX_N = 20,
	MIN_N = 10,
	MAX_M = 40,
	MIN_M = 5,
	MAX_AREA = 400,
	MAX_T = 2 * MAX_N * MAX_N * MAX_M,
};

struct Game {
	int N;             // [10, 20]
	int M;             // [N/2, 2N]
	int T;             // [1, 2NNM]
	int T_EXCL_SWITCH; // [1, 2NNM]
	int AREA, AREA_DIR;
	int WALLS_CNT;

	bitset<MAX_AREA * 4> walls;
	array<u16, MAX_M> balls_pos1;
	array<u16, MAX_M> goals_pos1;

	time_t TIME_START;

	int time_left_ms() const {
		constexpr int TOTAL_TIME = 1980;
		return TOTAL_TIME - time_ms(TIME_START);
	}

	void init() {
		read(N, M, T);
		T_EXCL_SWITCH = T - 2 * M;
		TIME_START = time();

		AREA = N * N;
		AREA_DIR = AREA * 4;

		loop(pos, N) {
			walls.set(U + 4 * (pos));
			walls.set(L + 4 * (pos * N));
			walls.set(R + 4 * (N - 1 + pos * N));
			walls.set(D + 4 * ((N - 1) * N + pos));
		}

		loop(r, N) {
			read_def(string, s);
			assert(s.size() == N - 1);
			loop(c, N - 1) if (s[c] == '1') {
				u16 pos = r * N + c;
				walls.set(R + 4 * pos);
				walls.set(L + 4 * (pos + 1));
				++WALLS_CNT;
			}
		}
		loop(r, N - 1) {
			read_def(string, s);
			assert(s.size() == N);
			loop(c, N) if (s[c] == '1') {
				u16 pos = r * N + c;
				walls.set(D + 4 * pos);
				walls.set(U + 4 * (pos + N));
				++WALLS_CNT;
			}
		}
		loop(i, M) {
			read_def(u16, by, bx, gy, gx);
			balls_pos1[i] = by * N + bx;
			goals_pos1[i] = gy * N + gx;
		}
	}
} game;

template<int max_size>
struct String {
	u8 size;
	char data[max_size];

	String() : size{}, data{} {}

	explicit String(string str) {
		size = str.size();
		loop(i, size) data[i] = str[i];
	}

	char operator[](const int i) const { return data[i]; }

	char &operator[](const int i) { return data[i]; }

	bool operator==(const String &other) const {
		if (size != other.size) return false;
		loop(i, size) if (data[i] != other.data[i]) return false;
		return true;
	}

	INLINE String &add(const char ch) {
		// assert(size < max_size);
		data[size++] = ch;
		return *this;
	}

	template<int other_size>
	INLINE String &add(const String<other_size> &other) {
		// assert(size + other.size <= max_size);
		memcpy(data + size, other.data, other.size);
		size += other.size;
		return *this;
	}

	INLINE void insert(int idx, char ch) {
		// assert(size < max_size);
		memmove(data + idx + 1, data + idx, size - idx);
		data[idx] = ch;
		++size;
	}

	INLINE void remove(int idx) {
		// assert(size > 0);
		memmove(data + idx, data + idx + 1, size - idx - 1);
		--size;
	}

	u64 fingerprint() const {
		u64 value = 1;
		loop(i, size) value = value * 257 + u8(data[i]);
		return value;
	}

	string str() const { return string(data, size); }
};

constexpr int MACRO_SIZE = 120;
using Macro = String<MACRO_SIZE>;
using Macro2 = String<MACRO_SIZE + 2>;

int NUM_CHAINS = 0;
constexpr bool TIMED = 1;
constexpr int MAX_CHAINS = TIMED ? 90 : 20;
constexpr int NUM_MACROS = 4;

using Chain = array<Macro, NUM_MACROS>;
array<Chain, MAX_CHAINS> MACROS{};
static_assert(is_power_of_2(NUM_MACROS));

constexpr int MACRO_BITS = bit_width(u32(NUM_MACROS - 1));
static_assert(MACRO_BITS <= 5);

constexpr int DIM = 4 * NUM_MACROS; // all dir+macro combinations

constexpr int MAX_POS3_C = MAX_AREA * 4 * NUM_MACROS;
constexpr int MAX_POS2_C = MAX_AREA * 4;
constexpr int MAX_POS1_C = MAX_AREA;

int MAX_POS1;
int MAX_POS2;
int MAX_POS3;

array<int, 4> forward_delta1;
array<int, 4> forward_delta2;
array<int, 4> forward_delta3;

using Ord = array<u8, MAX_M>;

array<u16, MAX_POS2_C> forward_dest2;
int CHAIN = 0;

struct Pos3 { // aka PosDirMacro
	u16 bits;  // pos1 : 9 | dir : 2 | macro

	Pos3() = default;
	Pos3(u16 bits) : bits(bits) {};
	Pos3(int bits) : bits(bits) {};

	bool operator==(const Pos3 &other) const = default;

	INLINE static Pos3 from1(const u16 pos1) { return pos1 << (MACRO_BITS + 2); }

	INLINE static Pos3 from1(const u16 pos1, const u16 dim) { return (pos1 << (MACRO_BITS + 2)) + dim; }

	INLINE static Pos3 from2(const u16 pos2, const u16 macro) { return (pos2 << MACRO_BITS) + macro; }

	static constexpr u16 _DIR_MASK = (0b11 << MACRO_BITS);

	INLINE Pos3 turn_right() const { return (bits & ~_DIR_MASK) + ((bits + (1 << MACRO_BITS)) & _DIR_MASK); }

	INLINE Pos3 turn_left() const { return (bits & ~_DIR_MASK) + ((bits - (1 << MACRO_BITS)) & _DIR_MASK); }

	INLINE Pos3 operator+(const int other) const { return Pos3{u16(bits + other)}; }

	INLINE u16 macro() const { return bits & (NUM_MACROS - 1); }

	INLINE u16 dir() const { return (bits >> MACRO_BITS) & 0b11; }

	INLINE u16 pos2() const { return bits >> MACRO_BITS; }

	INLINE u16 pos1() const { return bits >> (MACRO_BITS + 2); }

	INLINE operator u16() const { return bits; }

	string repr() const {
		return "Pos3(" + to_string(pos1()) + ", " + to_string(dir()) + ", " + to_string(macro()) + ")";
	}
};

static_assert(sizeof(Pos3) == 2);

INLINE u16 turn_right_pos2(const u16 pos) {
	return (pos & ~0b11) | ((pos + 1) & 0b11);
}

INLINE u16 turn_left_pos2(const u16 pos) {
	return (pos & ~0b11) | ((pos - 1) & 0b11);
}

/// get macro from dim
INLINE u8 dim_macro_get(const u8 dim) {
	return dim & (NUM_MACROS - 1);
}

/// take macro from dim2 and plug into dim1
INLINE u8 dim_macro_replace(const u8 dim1, const u8 dim2) {
	const u8 macro2 = dim_macro_get(dim2);
	const u8 dir1 = dim1 >> MACRO_BITS;
	return dir1 << MACRO_BITS | macro2;
}

/// where we end up after executing a macro
mdarr<u16, MAX_CHAINS, NUM_MACROS, MAX_POS2_C> MACRO_DELTA2;

/// how many turns to transition between macros
mdarr<u8, MAX_CHAINS, NUM_MACROS, NUM_MACROS> MACRO_SWITCH_TURNS;
mdarr<Macro2, MAX_CHAINS, NUM_MACROS, NUM_MACROS> MACRO_SWITCH_ROUTE;

/// main distance tables
alignas(64) mdarr<u16, MAX_CHAINS, MAX_M, DIM> START_LEN;
alignas(64) mdarr<u16, MAX_CHAINS, MAX_M, DIM, DIM> BALLS_LEN;
alignas(64) mdarr<u16, MAX_CHAINS, MAX_M, MAX_M, DIM, DIM> GOALS_LEN;

/// score tables used for DP
alignas(64) mdarr<u16, MAX_M, DIM> START_SCORE;
alignas(64) mdarr<u16, MAX_M, MAX_M, DIM, DIM> GOALS_SCORE;

/// store expanded dist for quick expanded route length checks
alignas(64) mdarr<u16, MAX_CHAINS, MAX_M, DIM> START_EXP;
alignas(64) mdarr<u16, MAX_CHAINS, MAX_M, DIM, DIM> BALLS_EXP;
alignas(64) mdarr<u16, MAX_CHAINS, MAX_M, MAX_M, DIM, DIM> GOALS_EXP;

/// helper tables used for reconstruction
alignas(64) mdarr<u16, MAX_CHAINS, MAX_M, DIM> START_DIM;
alignas(64) mdarr<u16, MAX_CHAINS, MAX_M, MAX_M, DIM, DIM> GOALS_DIM;

using d_t = u8;
alignas(64) static array<d_t, MAX_POS3_C> DISTS_LEN;
alignas(64) static array<u16, MAX_POS3_C> DISTS_EXP;

namespace BFS {

constexpr int N_BUCKETS = 256;

/// Used for precomputation of main distance table
void compute_distances_to_all(const Pos3 root) {
	using Item = pair<Pos3, u16>;
	static mdarr<Item, N_BUCKETS, MAX_POS3_C> bucket_states;

	fill_n(DISTS_LEN.begin(), MAX_POS3, 255);
	mdarr<int, N_BUCKETS> bucket_size{};

	bucket_states[0][0] = {root, 0};
	bucket_size[0] = 1;
	DISTS_LEN[root] = 0;

	const int MAX_MACRO = min(root.macro() + 3, NUM_MACROS);
	const int visited_tgt = (MAX_MACRO - root.macro()) * MAX_POS2;

	for (int depth = 0, vcnt = 0; vcnt < visited_tgt; ++depth) {
		assert(depth < 200);

		const int parent_size = bucket_size[depth];
		auto &parent_states = bucket_states[depth];

		const int next_idx = depth + 1;
		auto &next_states = bucket_states[next_idx];
		int &next_size = bucket_size[next_idx];

		loop(i, parent_size) {
			const auto [parent, parent_exp] = parent_states[i];
			const auto parent_macro = parent.macro();
			const auto parent_pos2 = parent.pos2();
			const auto parent_dist = DISTS_LEN[parent];

			if (parent_dist < depth) continue; // stale - already expanded
			DISTS_EXP[parent] = parent_exp;
			vcnt++;

			const d_t next_dist = depth + 1;
			auto NEXT_INSERT = [&] [[gnu::always_inline]] (const Pos3 child) {
				next_states[next_size] = {child, parent_exp + 1};
				int progress = next_dist < DISTS_LEN[child];
				next_size += progress;
				DISTS_LEN[child] = progress ? next_dist : DISTS_LEN[child];
			};
			NEXT_INSERT(parent.turn_right());
			NEXT_INSERT(parent.turn_left());
			NEXT_INSERT(Pos3::from2(forward_dest2[parent_pos2], parent_macro));

			loop(macro_idx, parent_macro, MAX_MACRO) { // try macros
				const u8 macro_cost = MACRO_SWITCH_TURNS[CHAIN][parent_macro][macro_idx];
				const u16 child_pos2 = MACRO_DELTA2[CHAIN][macro_idx][parent_pos2];
				const Pos3 child = Pos3::from2(child_pos2, macro_idx);
				const d_t child_dist = parent_dist + macro_cost;

				if (child_dist < DISTS_LEN[child]) {
					const int idx = depth + macro_cost;
					bucket_states[idx][bucket_size[idx]++] = {child, parent_exp + MACROS[CHAIN][macro_idx].size};
					DISTS_LEN[child] = child_dist;
				}
			}
		}
		bucket_size[depth] = 0;
	}
}

/// Very hot in Macro Optimization
/// Simpler, optimistic distance over dims with single macro
INLINE int
compute_distance_optimistic(const u16 src_pos1, const u16 dest_pos1, const array<u16, MAX_POS2_C> &new_macro_dest2) {
	static u16 *_q1 = new u16[MAX_POS2_C];
	static u16 *_q2 = new u16[MAX_POS2_C];

	assert(src_pos1 != dest_pos1);
	u16 *__restrict__ old_queue = _q1;
	u16 *__restrict__ new_queue = _q2;
	int old_queue_size = 0;
	int new_queue_size = 0;

	static int visited[MAX_POS2_C]{};
	static int visited_stamp = 0;
	++visited_stamp;

	const u16 src_pos2 = src_pos1 << 2;
	loop(dir, 4) { // allow start from all 4 directions
		u16 pos2 = src_pos2 + dir;
		old_queue[old_queue_size++] = pos2;
		visited[pos2] = visited_stamp;
	}

	int depth;
	for (depth = 0; depth < 200; ++depth) {
		loop(parent_idx, old_queue_size) {
			const u16 parent_pos2 = old_queue[parent_idx];

			auto INSERT = [&] [[gnu::always_inline]] (u16 child) {
				new_queue[new_queue_size] = child;
				new_queue_size += visited[child] != visited_stamp;
				visited[child] = visited_stamp;
			};

			INSERT(turn_right_pos2(parent_pos2));
			INSERT(turn_left_pos2(parent_pos2));

			u16 child = forward_dest2[parent_pos2];
			if (child >> 2 == dest_pos1) return depth + 1;
			INSERT(child);

			child = new_macro_dest2[parent_pos2];
			if (child >> 2 == dest_pos1) return depth + 1;
			INSERT(child);
		}

		old_queue_size = new_queue_size;
		swap(old_queue, new_queue);
		new_queue_size = 0;
	}
	unreachable();
}

int compute_distance_to_balls_optimistic(const u16 src_pos1,
                                         const u16 num_balls,
                                         const array<u16, MAX_POS2_C> &new_macro_dest2) {
	static u16 *_q1 = new u16[MAX_POS2_C];
	static u16 *_q2 = new u16[MAX_POS2_C];
	u16 *__restrict__ old_queue = _q1;
	u16 *__restrict__ new_queue = _q2;
	int old_queue_size = 0;
	int new_queue_size = 0;

	static int visited[MAX_POS2_C]{};
	static int visited_stamp = 0;
	++visited_stamp;

	bitset<MAX_POS1_C> unvisited_balls;
	loop(i, game.M) unvisited_balls[game.balls_pos1[i]] = true;

	int visited_balls_cnt = 0;
	int total_distance = 0;

	const u16 src_pos2 = src_pos1 << 2;
	loop(dir, 4) { // allow start from all 4 directions
		u16 pos2 = src_pos2 | dir;
		old_queue[old_queue_size++] = pos2;
		visited[pos2] = visited_stamp;
	}

	int depth;
	for (depth = 0; depth < 200; ++depth) {
		loop(parent_idx, old_queue_size) {
			const u16 parent_pos2 = old_queue[parent_idx];

			auto INSERT = [&] [[gnu::always_inline]] (u16 child) {
				new_queue[new_queue_size] = child;
				new_queue_size += visited[child] != visited_stamp;
				visited[child] = visited_stamp;
			};

			INSERT(turn_right_pos2(parent_pos2));
			INSERT(turn_left_pos2(parent_pos2));

			u16 child = new_macro_dest2[parent_pos2];
			INSERT(child);
			if (unvisited_balls[child >> 2]) {
				total_distance += depth + 1;
				unvisited_balls[child >> 2] = false;
				if (++visited_balls_cnt >= num_balls) return total_distance;
			}

			child = forward_dest2[parent_pos2];
			INSERT(child);
			if (unvisited_balls[child >> 2]) {
				total_distance += depth + 1;
				unvisited_balls[child >> 2] = false;
				if (++visited_balls_cnt >= num_balls) return total_distance;
			}
		}

		old_queue_size = new_queue_size;
		swap(old_queue, new_queue);
		new_queue_size = 0;
	}
	unreachable();
}

/// Given a sequence of pos3, infer the specific actions take
string _get_actions_from_positions(vector<Pos3> &positions, const int chain) {
	string actions;

	for (int i = 1; i < positions.size(); ++i) {
		Pos3 prev_pos3 = positions[i - 1];
		Pos3 next_pos3 = positions[i];

		const u16 prev_pos2 = prev_pos3.pos2();
		const u16 prev_macro = prev_pos3.macro();
		if (prev_macro != next_pos3.macro()) {
			const u16 next_macro = next_pos3.macro();
			u16 child_pos2 = MACRO_DELTA2[chain][next_macro][prev_pos2];
			Pos3 child_pos3 = Pos3::from2(child_pos2, next_macro);
			assert(child_pos3 == next_pos3);

			auto &macro_switch_string = MACRO_SWITCH_ROUTE[chain][prev_macro][next_macro];
			actions.append(macro_switch_string.data, macro_switch_string.size);
			continue;
		}

		if (prev_pos3.pos1() == next_pos3.pos1()) {
			if (prev_pos3.turn_right() == next_pos3) {
				actions.push_back('R');
				continue;
			}
			if (prev_pos3.turn_left() == next_pos3) {
				actions.push_back('L');
				continue;
			}
			// optimal route can use macro to turn 180
			assert(prev_pos3.turn_right().turn_right() == next_pos3);
		}

		if (not game.walls[prev_pos2]) { // forward
			Pos3 child_pos3 = prev_pos3 + forward_delta3[prev_pos2 & 0b11];
			if (child_pos3 == next_pos3) {
				actions.push_back('F');
				continue;
			}
		}

		// must be the current macro!
		u16 child_pos2 = MACRO_DELTA2[chain][prev_macro][prev_pos2];
		Pos3 child_pos3 = Pos3::from2(child_pos2, prev_macro);
		assert(child_pos3 == next_pos3);
		actions.push_back('P');
	}

	return actions;
}

/// BFS from source pos3 to a destination pos3
void compute_distance_direct(const Pos3 src3, const Pos3 dst3, const int chain) {
	using Item = pair<Pos3, u16>;
	static mdarr<Item, N_BUCKETS, MAX_POS3_C> bucket_states;

	fill_n(DISTS_LEN.begin(), MAX_POS3, 255);

	if (src3 == dst3) {
		DISTS_LEN[dst3] = 0;
		DISTS_EXP[dst3] = 0;
		return;
	}

	mdarr<int, N_BUCKETS> bucket_size{};
	bucket_states[0][0] = {src3, 0};
	bucket_size[0] = 1;
	DISTS_LEN[src3] = 0;

	const int MAX_MACRO = min(src3.macro() + 3, NUM_MACROS);

	for (int depth = 0; depth < 200; ++depth) {
		const int parent_size = bucket_size[depth];
		auto &parent_states = bucket_states[depth];

		const int next_idx = depth + 1;
		auto &next_states = bucket_states[next_idx];
		int &next_size = bucket_size[next_idx];

		loop(i, parent_size) {
			const auto [parent, parent_exp] = parent_states[i];
			const u16 parent_pos2 = parent.pos2();
			const u16 parent_macro = parent.macro();
			const d_t parent_dist = DISTS_LEN[parent];

			if (parent_dist < depth) continue; // stale - already expanded
			DISTS_EXP[parent] = parent_exp;
			if (parent == dst3) return;

			const d_t next_dist = depth + 1;
			auto NEXT_INSERT = [&] [[gnu::always_inline]] (const Pos3 child) {
				next_states[next_size] = {child, parent_exp + 1};
				int progress = next_dist < DISTS_LEN[child];
				next_size += progress;
				DISTS_LEN[child] = progress ? next_dist : DISTS_LEN[child];
			};
			NEXT_INSERT(parent.turn_right());
			NEXT_INSERT(parent.turn_left());
			NEXT_INSERT(Pos3::from2(forward_dest2[parent_pos2], parent_macro));

			loop(macro_idx, parent_macro, MAX_MACRO) { // try macros
				const u8 macro_cost = MACRO_SWITCH_TURNS[chain][parent_macro][macro_idx];
				const u16 child_pos2 = MACRO_DELTA2[chain][macro_idx][parent_pos2];
				const Pos3 child = Pos3::from2(child_pos2, macro_idx);
				const d_t child_dist = parent_dist + macro_cost;

				if (child_dist < DISTS_LEN[child]) {
					const int idx = depth + macro_cost;
					bucket_states[idx][bucket_size[idx]++] = {child, parent_exp + MACROS[chain][macro_idx].size};
					DISTS_LEN[child] = child_dist;
				}
			}
		}
		bucket_size[depth] = 0;
	}
	unreachable();
}

/// Find actions route from a specific pos3 to a specific pos3, used for reconstruction
string construct_actions_direct(const Pos3 src3, const Pos3 dst3, const int chain) {
	static mdarr<Pos3, N_BUCKETS, MAX_POS3_C> bucket_states;
	static array<u8, MAX_POS3_C> dists;
	static array<u16, MAX_POS3_C> predecessor;

	if (src3 == dst3) return "";

	fill_n(dists.begin(), MAX_POS3, 255);
	mdarr<int, N_BUCKETS> bucket_size{};
	bucket_states[0][0] = src3;
	bucket_size[0] = 1;
	dists[src3] = 0;

	const int MAX_MACRO = min(src3.macro() + 3, NUM_MACROS);

	int depth;
	for (depth = 0; depth < 200; ++depth) {
		const int parent_size = bucket_size[depth];
		auto &parent_states = bucket_states[depth];

		const int next_idx = depth + 1;
		auto &next_states = bucket_states[next_idx];
		int &next_size = bucket_size[next_idx];

		loop(i, parent_size) {
			const auto &parent = parent_states[i];
			const u16 parent_pos2 = parent.pos2();
			const u16 parent_macro = parent.macro();
			const u8 parent_dist = dists[parent];
			const u8 next_dist = parent_dist + 1;

			if (parent_dist < depth) continue; // stale - already expanded
			if (parent == dst3) goto FINISH;

			auto NEXT_INSERT = [&] [[gnu::always_inline]] (const Pos3 child) {
				if (next_dist < dists[child]) {
					next_states[next_size++] = child;
					predecessor[child] = parent;
					dists[child] = next_dist;
				}
			};
			NEXT_INSERT(parent.turn_right());
			NEXT_INSERT(parent.turn_left());
			NEXT_INSERT(Pos3::from2(forward_dest2[parent_pos2], parent_macro));

			loop(macro_idx, parent_macro, MAX_MACRO) { // try macros
				const auto macro_cost = MACRO_SWITCH_TURNS[chain][parent_macro][macro_idx];
				const u16 child_pos2 = MACRO_DELTA2[chain][macro_idx][parent_pos2];
				const Pos3 child = Pos3::from2(child_pos2, macro_idx);

				u8 child_dist = parent_dist + macro_cost;
				if (child_dist < dists[child]) {
					const int idx = depth + macro_cost;
					bucket_states[idx][bucket_size[idx]++] = child;
					predecessor[child] = parent;
					dists[child] = child_dist;
				}
			}
		}
		bucket_size[depth] = 0;
	}
	unreachable();

FINISH:
	vector<Pos3> positions = {dst3};
	while (positions.back() != src3) {
		Pos3 cur = positions.back();
		positions.push_back(predecessor[cur]);
	}
	reverse(all(positions));
	string actions = BFS::_get_actions_from_positions(positions, chain);
	assert(actions.size() == depth);
	return actions;
}
} // namespace BFS

namespace DISTANCE_PRECOMP {

NOINLINE void compute_balls_tables(const int chain) {
	loop(ball_idx, game.M) {
		u16 ball_pos1 = game.balls_pos1[ball_idx];
		loop(ball_dim, DIM) {
			u16 goal_pos1 = game.goals_pos1[ball_idx];

			Pos3 ball_pos3 = Pos3::from1(ball_pos1, ball_dim);
			BFS::compute_distances_to_all(ball_pos3);

			loop(goal_dim, DIM) {
				Pos3 goal_pos3 = Pos3::from1(goal_pos1, goal_dim);
				BALLS_LEN[chain][ball_idx][ball_dim][goal_dim] = DISTS_LEN[goal_pos3];
				BALLS_EXP[chain][ball_idx][ball_dim][goal_dim] = DISTS_EXP[goal_pos3];
			}
		}
	}
}

NOINLINE void compute_start_tables(const int chain) {
	BFS::compute_distances_to_all({0});

	loop(idx, game.M) {
		u16 ball_pos1 = game.balls_pos1[idx];

		loop(goal_dim, DIM) {
			u8 top_len = 255;
			u8 top_dim{};
			u16 top_exp{};

			loop(ball_dim, DIM) {
				Pos3 ball_pos3 = Pos3::from1(ball_pos1, ball_dim);
				int len = DISTS_LEN[ball_pos3] + BALLS_LEN[chain][idx][ball_dim][goal_dim];
				int exp = DISTS_EXP[ball_pos3] + BALLS_EXP[chain][idx][ball_dim][goal_dim];

				// assert(len < 255);
				if (len <= top_len and (len < top_len or exp < top_exp)) {
					top_len = len;
					top_exp = exp;
					top_dim = ball_dim;
				}
			}
			START_LEN[chain][idx][goal_dim] = top_len;
			START_EXP[chain][idx][goal_dim] = top_exp;
			START_DIM[chain][idx][goal_dim] = top_dim;
		}
	}
}

NOINLINE void compute_goals_tables_avx(const int chain) {
	loop(start_idx, game.M) {
		u16 start_goal_pos1 = game.goals_pos1[start_idx];
		loop(start_dim, DIM) {
			Pos3 start_goal_pos3 = Pos3::from1(start_goal_pos1, start_dim);
			BFS::compute_distances_to_all(start_goal_pos3);

			loop(final_idx, game.M) {
				u16 ball_pos1 = game.balls_pos1[final_idx];
				const u16 ball_pos3 = Pos3::from1(ball_pos1);

				static_assert(DIM == 16);
				__m256i top_len = _mm256_set1_epi16(UINT16_MAX);
				__m256i top_exp{};
				__m256i top_dim{};
				const __m256i sign_bit = _mm256_set1_epi16(0x8000);

				loop(ball_dim, DIM) {
					__m256i len = _mm256_load_si256((const __m256i *) BALLS_LEN[chain][final_idx][ball_dim].data());
					__m256i exp = _mm256_load_si256((const __m256i *) BALLS_EXP[chain][final_idx][ball_dim].data());
					len = _mm256_add_epi16(len, _mm256_set1_epi16(DISTS_LEN[ball_pos3 + ball_dim]));
					exp = _mm256_add_epi16(exp, _mm256_set1_epi16(DISTS_EXP[ball_pos3 + ball_dim]));

					__m256i len_lt =
						_mm256_cmpgt_epi16(_mm256_xor_si256(top_len, sign_bit), _mm256_xor_si256(len, sign_bit));
					__m256i len_eq = _mm256_cmpeq_epi16(len, top_len);
					__m256i exp_lt =
						_mm256_cmpgt_epi16(_mm256_xor_si256(top_exp, sign_bit), _mm256_xor_si256(exp, sign_bit));
					__m256i mask = _mm256_or_si256(len_lt, _mm256_and_si256(len_eq, exp_lt));

					top_len = _mm256_blendv_epi8(top_len, len, mask);
					top_exp = _mm256_blendv_epi8(top_exp, exp, mask);
					top_dim = _mm256_blendv_epi8(top_dim, _mm256_set1_epi16(ball_dim), mask);
				}

				_mm256_store_si256((__m256i *) GOALS_LEN[chain][start_idx][final_idx][start_dim].data(), top_len);
				_mm256_store_si256((__m256i *) GOALS_EXP[chain][start_idx][final_idx][start_dim].data(), top_exp);
				_mm256_store_si256((__m256i *) GOALS_DIM[chain][start_idx][final_idx][start_dim].data(), top_dim);
			}
		}
	}
}

NOINLINE void compute_goals_tables(const int chain) {
	loop(start_idx, game.M) {
		u16 start_goal_pos1 = game.goals_pos1[start_idx];
		loop(start_dim, DIM) {
			Pos3 start_goal_pos3 = Pos3::from1(start_goal_pos1, start_dim);
			BFS::compute_distances_to_all(start_goal_pos3);

			loop(final_idx, game.M) {
				u16 ball_pos1 = game.balls_pos1[final_idx];
				loop(final_dim, DIM) {
					u16 top_len = 255;
					u16 top_dim{};
					u16 top_exp{};
					const u16 ball_pos3 = Pos3::from1(ball_pos1);

					loop(ball_dim, DIM) {
						int len = DISTS_LEN[ball_pos3 + ball_dim] + BALLS_LEN[chain][final_idx][ball_dim][final_dim];

						if (len <= top_len) {
							int exp = DISTS_EXP[ball_pos3 + ball_dim] + BALLS_EXP[chain][final_idx][ball_dim][final_dim];
							if (len < top_len or exp < top_exp) {
								top_exp = exp;
								top_len = len;
								top_dim = ball_dim;
							}
						}
					}

					GOALS_LEN[chain][start_idx][final_idx][start_dim][final_dim] = top_len;
					GOALS_EXP[chain][start_idx][final_idx][start_dim][final_dim] = top_exp;
					GOALS_DIM[chain][start_idx][final_idx][start_dim][final_dim] = top_dim;
				}
			}
		}
	}
}

struct ComposeState {
	int chain;
	int c1;
	int c2;
} COMPOSE_STATE;

void compose_score_tables(const int chain, const int c1, const int c2) {
	if (COMPOSE_STATE.chain == chain and COMPOSE_STATE.c1 == c1 and COMPOSE_STATE.c2 == c2) return;
	COMPOSE_STATE.chain = chain;
	COMPOSE_STATE.c1 = c1;
	COMPOSE_STATE.c2 = c2;

	loop(i, game.M) loop(k, DIM) {
		START_SCORE[i][k] = c1 * START_LEN[chain][i][k] + c2 * START_EXP[chain][i][k];

		loop(j, game.M) loop(l, DIM) {
			GOALS_SCORE[i][j][k][l] = c1 * GOALS_LEN[chain][i][j][k][l] + c2 * GOALS_EXP[chain][i][j][k][l];
		}
	}
}

} // namespace DISTANCE_PRECOMP

namespace MACRO_OPTIM {

/// where we end up after executing a specific macro from a specific pos2
INLINE u16 compute_macro_delta_val(u16 pos2, const Macro &macro) {
	u16 delta2 = forward_delta2[pos2 & 3];
	loop(i, macro.size) {
		switch (macro[i]) {
			case 'F':
				if (not game.walls[pos2]) pos2 += delta2;
				break;
			case 'R':
				pos2 = turn_right_pos2(pos2);
				delta2 = forward_delta2[pos2 & 3];
				break;
			case 'L':
				pos2 = turn_left_pos2(pos2);
				delta2 = forward_delta2[pos2 & 3];
				break;
		}
	}
	return pos2;
}

INLINE void compute_macro_deltas_row(const Macro &macro, array<u16, MAX_POS2_C> &deltas) {
	loop(pos2, MAX_POS2) {
		deltas[pos2] = compute_macro_delta_val(pos2, macro);
	}
}

/// in what pos2 we end up after executing a macro
void compute_macro_deltas_all(const int chain) {
	loop(macro_idx, NUM_MACROS) {
		compute_macro_deltas_row(MACROS[chain][macro_idx], MACRO_DELTA2[chain][macro_idx]);
	}
}

/// try to create macro2 potentially using macro1 greedily
Macro2 compute_macro_switch_single(const int chain, const u16 macro1_idx, const u16 macro2_idx) {
	Macro2 out;
	if (macro1_idx == macro2_idx) return out.add('P');

	out.add('M');
	auto &macro1 = MACROS[chain][macro1_idx];
	auto &macro2 = MACROS[chain][macro2_idx];
	if (macro1.size <= 1 or macro1.size >= macro2.size) return out.add(macro2).add('M');

	for (int i2 = 0; i2 < macro2.size;) {
		int steps = macro1.size;
		loop(i1, macro1.size) {
			if (i2 + i1 >= macro2.size or macro1[i1] != macro2[i2 + i1]) {
				steps = 1;
				break;
			}
		}
		if (steps > 1) out.add('P');
		else out.add(macro2[i2]);
		i2 += steps;
	}
	return out.add('M');
}

int compute_macro_switch_len_single(const Macro &macro1, const Macro &macro2) {
	if (macro1.size <= 1 or macro1.size >= macro2.size) return macro2.size + 2;

	int len = 2;
	for (int i2 = 0; i2 < macro2.size;) {
		int steps = macro1.size;
		loop(i1, macro1.size) {
			if (i2 + i1 >= macro2.size or macro1[i1] != macro2[i2 + i1]) {
				steps = 1;
				break;
			}
		}
		++len;
		i2 += steps;
	}
	return len;
}

/// matrix of how many turns it takes to transition between macros
void compute_macro_switches(const int chain) {
	loop(i, NUM_MACROS) {
		loop(j, NUM_MACROS) {
			auto switch_str = compute_macro_switch_single(chain, i, j);
			MACRO_SWITCH_TURNS[chain][i][j] = switch_str.size;
			MACRO_SWITCH_ROUTE[chain][i][j] = switch_str;
		}
	}
}

INLINE char get_random_char() {
	switch (rnd.next_u32(7)) {
		case 0: return 'L';
		case 1: return 'R';
		case 2 ... 3: return 'P';
		default: return 'F';
	}
}

INLINE void insert_single_character(Macro &str, bool allow_p) {
	char ch = get_random_char();
	if (not allow_p and ch == 'P') ch = 'F';
	int idx = rnd.next_u32(str.size + 1);
	str.insert(idx, ch);
}

void mutate_single_character(Macro &str, bool allow_p) {
	char ch = get_random_char();
	if (not allow_p and ch == 'P') ch = 'F';

	if (str.size == 0) {
		str.add(ch);
		return;
	}

	switch (rnd.next_u32(str.size < MACRO_SIZE ? 3 : 2)) {
		case 0: { // remove
			int idx = rnd.next_u32(str.size);
			str.remove(idx);
			return;
		}
		case 1: { // modify
			int idx = rnd.next_u32(str.size);
			str[idx] = ch;
			return;
		}
		case 2: { // insert
			int idx = rnd.next_u32(str.size + 1);
			str.insert(idx, ch);
			return;
		}
	}
	unreachable();
}

struct MacroOptimizer {
	const Macro &parent;
	Macro best_recipe, cand_recipe;

	explicit MacroOptimizer(const Macro &parent) : parent(parent) {}

	bool is_stupid(const Macro &s) const {
		loop(i, 1, s.size) {
			char c1 = s[i - 1];
			char c2 = s[i];
			if ((c1 == 'L' and c2 == 'R') or (c1 == 'R' and c2 == 'L')) return true;
		}
		loop(i, 2, s.size) {
			char c1 = s[i - 2];
			char c2 = s[i - 1];
			char c3 = s[i];
			if (c1 != 'F' and c1 == c2 and c2 == c3) return true;
		}
		return false;
	}

	bool expand_ok(Macro &expanded, const int max_size) {
		expanded.size = 0;
		loop(i, cand_recipe.size) {
			char ch = cand_recipe[i];
			int new_cnt = ch == 'P' ? parent.size : 1;
			if (expanded.size + new_cnt > max_size) return false;

			if (ch != 'P') expanded.add(ch);
			else expanded.add(parent);
		}
		return true;
	}

	void make_candidate(Macro &macro, int max_size) {
		bool allow_p = parent.size >= 2;
		do {
			cand_recipe = best_recipe;
			while (cand_recipe.size < 3) insert_single_character(cand_recipe, allow_p);

			switch (rnd.next_u32(5)) {
				case 0 ... 3: mutate_single_character(cand_recipe, allow_p); break;
				case 4: {
					int a = rnd.next_u32(cand_recipe.size);
					rotate(cand_recipe.data, cand_recipe.data + a, cand_recipe.data + cand_recipe.size);
					break;
				}
			}
		} while (cand_recipe.size > 16 or is_stupid(cand_recipe) or not expand_ok(macro, max_size) or
		         cand_recipe == best_recipe);
	}

	void accept_candidate() { best_recipe = cand_recipe; }
};

constexpr mdarr<int, MAX_CHAINS, 4> SIZE_LIMITS = [] {
	mdarr<int, MAX_CHAINS, 4> arr = {{
		{0, 16, 49, MACRO_SIZE}, // 0
		{0, 16, 49, MACRO_SIZE}, // 1
		{0, 15, 49, MACRO_SIZE}, // 2
		{0, 15, 49, MACRO_SIZE}, // 3
		{0, 14, 45, MACRO_SIZE}, // 4
		{0, 14, 42, MACRO_SIZE}, // 5
		{0, 13, 38, MACRO_SIZE}, // 6
		{0, 12, 34, MACRO_SIZE}, // 7
		{0, 11, 30, MACRO_SIZE}, // 8
		{0, 10, 28, MACRO_SIZE}, // 9
		{0, 9, 26, MACRO_SIZE},  // 10
		{0, 8, 24, MACRO_SIZE},  // 11
		{0, 7, 22, MACRO_SIZE},  // 12
		{0, 6, 20, MACRO_SIZE},  // 13
		{0, 4, 30, MACRO_SIZE},  // 14
		{0, 5, 16, MACRO_SIZE},  // 15
		{0, 18, 54, MACRO_SIZE}, // 16
		{0, 2, 44, MACRO_SIZE},  // 17
		{0, 10, 40, MACRO_SIZE}, // 18
		{0, 20, 32, MACRO_SIZE}, // 19
	}};

	// fill missing chains with some default
	Mixer mixer;
	u64 seed = 1;
	loop(i, MAX_CHAINS) {
		if (arr[i][1] == 0) {
			int a = 16 - mixer.splitmix64_sequence(seed) % 4;
			int b = 49 - mixer.splitmix64_sequence(seed) % 5;
			int c = MACRO_SIZE - mixer.splitmix64_sequence(seed) % 10;
			arr[i] = {0, a, b, c};
		}
	}
	return arr;
}();

[[gnu::noipa]] void anneal_macros_sequential() {
	time_t fun_start = time();

	// const float scale = double(4 * game.N + game.M) / (4 * MAX_N + MAX_M);
	// int MACRO_SIZE_LIMIT = lerp(60, MACRO_SIZE, scale);
	// loop(i, MAX_CHAINS) {
	// 	SIZE_LIMITS[i][3] = min(SIZE_LIMITS[i][3], MACRO_SIZE_LIMIT);
	// }

	int total_score = 0;
	const int WORK = 400 * MAX_N * sqrt<int>(MAX_M);
	const int WORK_UNIT = sqrt<int>(game.M) * game.N;
	const int STEPS = (WORK + WORK_UNIT - 1) / WORK_UNIT;

	struct CacheItem {
		float score;
		array<float, MAX_M> scores;
	};

	static FlatHashMap<131'072, CacheItem> cache;

	float START_TEMP = 6.0;
	if (game.M <= 20) START_TEMP = 4.0;
	if (game.M <= 10) START_TEMP = 2.0;
	const float DIFF_TEMP = START_TEMP - 0.1;

	loop(chain_idx, MAX_CHAINS) {
		mdarr<float, NUM_MACROS, MAX_M> other_scores{};
		array<float, MAX_M> chain_best_scores{};
		chain_best_scores.fill(INT_MAX);

		loop(macro_idx, 1, NUM_MACROS) {
			float top_score = INT_MAX;
			float cur_score = INT_MAX;
			auto &top_macro = MACROS[chain_idx][macro_idx];
			const int size_limit = SIZE_LIMITS[chain_idx][macro_idx];

			const int parent_idx = macro_idx - 1;
			Macro &parent = MACROS[chain_idx][parent_idx];
			MacroOptimizer optimizer(parent);

			float progress = 0;
			float temp = START_TEMP;

#define EARLY_STOP 0

#if EARLY_STOP
			double history_sum[MAX_M]{};
			int history_size[MAX_M]{};
#endif

			loop(step, STEPS) {
				if ((step & 31) == 31) {
					progress = float(step) / STEPS;
					temp = START_TEMP - DIFF_TEMP * progress;
				}

				Macro new_macro;
				auto is_duplicate = [&] [[gnu::always_inline]] {
					loop(prev_chain, chain_idx + 1) {
						int macro_limit = prev_chain == chain_idx ? macro_idx : NUM_MACROS;
						loop(prev_macro, 1, macro_limit) if (new_macro == MACROS[prev_chain][prev_macro]) return true;
					}
					return false;
				};


			RETRY:
				optimizer.make_candidate(new_macro, size_limit);
				if (is_duplicate()) goto RETRY;

				u64 fingerprint = new_macro.fingerprint();
				auto [cache_idx, cache_unique] = cache.find_slot(fingerprint);

				float macro_score = 0;
				array<float, MAX_M> &macro_scores = cache.values[cache_idx].scores;

#if EARLY_STOP
				bool early_stopped = false;
#endif

				if (cache_unique) {
					array<u16, MAX_POS2_C> cand_delta2;
					compute_macro_deltas_row(new_macro, cand_delta2);

					loop(ball_idx, game.M) {
						const u16 ball_pos1 = game.balls_pos1[ball_idx];
						const u16 goal_pos1 = game.goals_pos1[ball_idx];
						float dist1 = BFS::compute_distance_optimistic(ball_pos1, goal_pos1, cand_delta2);
						float dist2 = BFS::compute_distance_to_balls_optimistic(goal_pos1, 5, cand_delta2);
						float val = dist1 + dist2 / 6;
						macro_score += val;
						macro_scores[ball_idx] = val;

#if EARLY_STOP
						if (ball_idx >= 5) {
							++history_size[ball_idx];
							history_sum[ball_idx] += macro_score;

							if (history_size[ball_idx] > 40 and macro_score * history_size[ball_idx] > history_sum[ball_idx]) {
								early_stopped = true;
								break;
							}
						}
#endif
					}
#if EARLY_STOP
					if (early_stopped) {
						cache.values[cache_idx].score = macro_score + 10'000;
						continue;
					}
#endif
					cache.values[cache_idx].score = macro_score;

				} else macro_score = cache.values[cache_idx].score;

				float new_chain_score = 0;
				loop(ball_idx, game.M) {
					float v = min(macro_scores[ball_idx], chain_best_scores[ball_idx]);
					new_chain_score += v;
				}
				macro_score = 0.5 * macro_score + 0.75 * new_chain_score;

				const auto osz = max(0, optimizer.cand_recipe.size - 2);
				macro_score += 1.35 * osz + 0.135 * osz * osz;

				float diff = macro_score - cur_score;
				if (diff <= 0 or exp(-float(diff) / temp) > rnd.next_double()) {
					optimizer.accept_candidate();
					cur_score = macro_score;

					if (macro_score < top_score) {
						top_score = macro_score;
						top_macro = new_macro;
						other_scores[macro_idx] = macro_scores;
						// cerr << "step " << step << " new macro score: " << new_score << endl;
					}
				}
			}
			assert(top_score != INT_MAX);
			total_score += top_score;
			loop(ball_idx, game.M) {
				chain_best_scores[ball_idx] = min(chain_best_scores[ball_idx], other_scores[macro_idx][ball_idx]);
			}
		}

		// macro precompute
		compute_macro_switches(chain_idx);
		compute_macro_deltas_all(chain_idx);

		// tables precompute
		CHAIN = chain_idx; // BFS is using CHAIN global
		DISTANCE_PRECOMP::compute_balls_tables(chain_idx);
		DISTANCE_PRECOMP::compute_start_tables(chain_idx);
		DISTANCE_PRECOMP::compute_goals_tables_avx(chain_idx);

		++NUM_CHAINS;

		if constexpr (TIMED) {
			constexpr int MACRO_TIME_MS = 1550;
			if (TAC_MS > MACRO_TIME_MS) break;
		}
	}
	derr << "[TEMP] NUM_CHAINS " << NUM_CHAINS << endl;
	derr << "[DATA] macro_score = " << total_score << endl;
	derr << "[DATA] time1 = " << time_ms(fun_start) << endl;
}


} // namespace MACRO_OPTIM

namespace DP {

/// Main eval for order optimization.
/// Fast DP that returns only the score.
int eval_score(const Ord &order) {
	alignas(32) static u16 DP_VAL[MAX_M][DIM];

	int prev_idx = order[0];
	loop(dim, DIM) DP_VAL[0][dim] = START_SCORE[prev_idx][dim];

	loop(ord, 1, game.M) {
		const int next_idx = order[ord];
		const u16 *const prev_dp = DP_VAL[ord - 1];
		const auto &scores = GOALS_SCORE[prev_idx][next_idx];

#pragma GCC unroll 8
		loop(next_dim, DIM) {
			u16 top_val = UINT16_MAX;
			loop(prev_dim, DIM) {
				u16 val = prev_dp[prev_dim] + scores[prev_dim][next_dim];
				top_val = min(val, top_val);
			}
			DP_VAL[ord][next_dim] = top_val;
		}
		prev_idx = next_idx;
	}
	const u16 *const last_dp = DP_VAL[game.M - 1];
	return *min_element(last_dp, last_dp + DIM);
}

[[gnu::aligned(64)]] HOT FLATTEN INLINE int eval_score_avx(const Ord &order) {
	static_assert(DIM == 16);
	alignas(32) static u16 DP_SCORE[DIM];

	int prev_idx = order[0];
	const u16 *start_row = START_SCORE[prev_idx].data();
	__m256i dp = _mm256_load_si256((const __m256i *) start_row);

	loop(ord, 1, game.M) {
		const int next_idx = order[ord];
		_mm256_store_si256((__m256i *) DP_SCORE, dp);

		__m256i nxt = _mm256_set1_epi16(UINT16_MAX);

#pragma GCC unroll 8
		loop(prev_dim, DIM) {
			const u16 *row = GOALS_SCORE[prev_idx][next_idx][prev_dim].data();
			const __m256i r = _mm256_load_si256((const __m256i *) row);
			const __m256i base = _mm256_set1_epi16(DP_SCORE[prev_dim]);
			nxt = _mm256_min_epu16(nxt, _mm256_add_epi16(base, r));
		}

		dp = nxt;
		prev_idx = next_idx;
	}

	__m128i x = _mm_min_epu16(_mm256_castsi256_si128(dp), _mm256_extracti128_si256(dp, 1));
	return _mm_extract_epi16(_mm_minpos_epu16(x), 0);
}

/// DP to measure how long is the expanded length
int eval_expanded_length(const u8 chain_idx, const Ord &order) {
	static auto DP_VAL = new int[MAX_M][DIM];
	static auto DP_EXP = new int[MAX_M][DIM];

	int prev_idx = order[0];
	loop(dim, DIM) {
		DP_VAL[0][dim] = START_SCORE[prev_idx][dim];
		DP_EXP[0][dim] = START_EXP[chain_idx][prev_idx][dim];
	}

	loop(ord, 1, game.M) {
		const int next_idx = order[ord];
		const int *prev_dp_val = DP_VAL[ord - 1];
		const int *prev_dp_exp = DP_EXP[ord - 1];

		loop(next_dim, DIM) {
			int top_val = INT_MAX;
			int top_exp{};

			loop(prev_dim, DIM) {
				int val = prev_dp_val[prev_dim] + GOALS_SCORE[prev_idx][next_idx][prev_dim][next_dim];
				int exp = prev_dp_exp[prev_dim] + GOALS_EXP[chain_idx][prev_idx][next_idx][prev_dim][next_dim];
				if (val <= top_val and (val < top_val or exp < top_exp)) {
					top_val = val;
					top_exp = exp;
				}
			}
			DP_VAL[ord][next_dim] = top_val;
			DP_EXP[ord][next_dim] = top_exp;
		}
		prev_idx = next_idx;
	}

	const int *const last_val = DP_VAL[game.M - 1];
	const int *const last_exp = DP_EXP[game.M - 1];
	int top_val = INF;
	int top_exp{};
	loop(dim, DIM) {
		int val = last_val[dim];
		int exp = last_exp[dim];
		if (val <= top_val and (val < top_val or exp < top_exp)) {
			top_val = val;
			top_exp = exp;
		}
	}
	assert(top_val < INF);
	return top_exp;
}

/// Verify whether order is under turns limit
NOINLINE bool good_expanded_length(const u8 chain_idx, const Ord &order) {
	return eval_expanded_length(chain_idx, order) <= game.T_EXCL_SWITCH;
}

/// DP
pair<int, int> eval_both_lengths(const u8 chain_idx, const Ord &order) {
	static auto DP_VAL = new int[MAX_M][DIM];
	static auto DP_EXP = new int[MAX_M][DIM];
	static auto DP_LEN = new int[MAX_M][DIM];

	int prev_idx = order[0];
	loop(dim, DIM) {
		DP_VAL[0][dim] = START_SCORE[prev_idx][dim];
		DP_EXP[0][dim] = START_EXP[chain_idx][prev_idx][dim];
		DP_LEN[0][dim] = START_LEN[chain_idx][prev_idx][dim];
	}

	loop(ord, 1, game.M) {
		const int next_idx = order[ord];
		const int *prev_dp_val = DP_VAL[ord - 1];
		const int *prev_dp_exp = DP_EXP[ord - 1];
		const int *prev_dp_len = DP_LEN[ord - 1];

		loop(next_dim, DIM) {
			int top_val = INT_MAX;
			int top_exp{};
			int top_len{};

			loop(prev_dim, DIM) {
				int val = prev_dp_val[prev_dim] + GOALS_SCORE[prev_idx][next_idx][prev_dim][next_dim];
				int exp = prev_dp_exp[prev_dim] + GOALS_EXP[chain_idx][prev_idx][next_idx][prev_dim][next_dim];
				if (val <= top_val and (val < top_val or exp < top_exp)) {
					top_val = val;
					top_exp = exp;
					top_len = prev_dp_len[prev_dim] + GOALS_LEN[chain_idx][prev_idx][next_idx][prev_dim][next_dim];
				}
			}
			DP_VAL[ord][next_dim] = top_val;
			DP_EXP[ord][next_dim] = top_exp;
			DP_LEN[ord][next_dim] = top_len;
		}
		prev_idx = next_idx;
	}

	const int *const last_val = DP_VAL[game.M - 1];
	const int *const last_exp = DP_EXP[game.M - 1];
	const int *const last_len = DP_LEN[game.M - 1];
	int top_val = INF;
	int top_exp{};
	int top_len{};

	loop(dim, DIM) {
		int val = last_val[dim];
		int exp = last_exp[dim];
		if (val <= top_val and (val < top_val or exp < top_exp)) {
			top_val = val;
			top_exp = exp;
			top_len = last_len[dim];
		}
	}
	assert(top_val < INF);
	return make_pair(top_len, top_exp);
}

/// Solve DP to find optimal dims under macro restriction
Ord eval_goal_dims(const u8 chain_idx, const Ord &order) {
	static auto DP_VAL = new int[MAX_M][DIM]{};
	static auto DP_EXP = new int[MAX_M][DIM]{};
	static auto DP_DIM = new int[MAX_M][DIM]{};

	int prev_idx = order[0];
	loop(init_dim, DIM) {
		DP_EXP[0][init_dim] = START_EXP[chain_idx][prev_idx][init_dim];
		DP_VAL[0][init_dim] = START_SCORE[prev_idx][init_dim];
	}

	loop(ord, 1, game.M) {
		const int next_idx = order[ord];
		const int *prev_dp = DP_VAL[ord - 1];
		const int *prev_dp_exp = DP_EXP[ord - 1];

		loop(next_dim, DIM) {
			int top_len = INT_MAX;
			int top_exp{};
			int top_dim{};

			loop(prev_dim, DIM) {
				int len = prev_dp[prev_dim] + GOALS_SCORE[prev_idx][next_idx][prev_dim][next_dim];
				int exp = prev_dp_exp[prev_dim] + GOALS_EXP[chain_idx][prev_idx][next_idx][prev_dim][next_dim];

				if (len <= top_len and (len < top_len or exp < top_exp)) {
					top_len = len;
					top_exp = exp;
					top_dim = prev_dim;
				}
			}
			DP_VAL[ord][next_dim] = top_len;
			DP_EXP[ord][next_dim] = top_exp;
			DP_DIM[ord][next_dim] = top_dim;
		}
		prev_idx = next_idx;
	}

	int top_len = INF;
	int top_dim{};
	int top_exp{};
	loop(goal_dim, DIM) {
		int len = DP_VAL[game.M - 1][goal_dim];
		int exp = DP_EXP[game.M - 1][goal_dim];
		if (len <= top_len and (len < top_len or exp < top_exp)) {
			top_len = len;
			top_exp = exp;
			top_dim = goal_dim;
		}
	}
	assert(top_len < INF);

	Ord dim_sequence;
	for (int ord = game.M - 1; ord >= 0; --ord) {
		dim_sequence[ord] = top_dim;
		top_dim = DP_DIM[ord][top_dim];
	}
	return dim_sequence;
}
} // namespace DP

namespace CONSTRUCT_ACTIONS {

string construct_actions_from_order(const int chain, const Ord &order) {
	Ord goal_dims = DP::eval_goal_dims(chain, order);
	Pos3 prev_goal = 0;
	string actions;

	loop(idx, 0, game.M) {
		const u8 goal_dim = goal_dims[idx];
		u8 ball_dim = idx == 0 ? START_DIM[chain][order[idx]][goal_dim]
		                       : GOALS_DIM[chain][order[idx - 1]][order[idx]][goal_dims[idx - 1]][goal_dim];

		Pos3 ball = Pos3::from1(game.balls_pos1[order[idx]], ball_dim);
		actions.append(BFS::construct_actions_direct(prev_goal, ball, chain));
		actions.push_back('S');

		Pos3 goal = Pos3::from1(game.goals_pos1[order[idx]], goal_dim);
		actions.append(BFS::construct_actions_direct(ball, goal, chain));
		actions.push_back('S');
		prev_goal = goal;
	}
	return actions;
}

/// Expand M (macro) characters in the actions
int get_expanded_length_from_actions(const string &actions) {
	bool macro_record_on = false;
	int new_macro_length = 0;
	int expanded_length = 0;
	int macro_length = 0;

	for (auto action : actions) {
		if (action == 'M') {
			if (macro_record_on) macro_length = new_macro_length;
			else new_macro_length = 0;
			macro_record_on = not macro_record_on;
		} else {
			int steps = action == 'P' ? macro_length : 1;
			if (macro_record_on) new_macro_length += steps;
			expanded_length += steps;
		}
	}
	return expanded_length;
}

} // namespace CONSTRUCT_ACTIONS

namespace SOLVER {

Ord make_random_order() {
	Ord order{};
	iota(order.begin(), order.begin() + game.M, 0);
	shuffle(order.begin(), order.begin() + game.M, rng);
	return order;
}

Ord make_greedy_order() {
	Ord order{};
	array<bool, MAX_M> used{};
	array<int, DIM> cur_dp{};
	array<int, DIM> cand_dp{};
	array<int, DIM> best_dp{};

	int first_idx = -1;
	int first_score = INF;
	loop(idx, game.M) {
		int score = INF;
		loop(dim, DIM) score = min<int>(score, START_SCORE[idx][dim]);
		if (score < first_score) {
			first_score = score;
			first_idx = idx;
		}
	}
	assert(first_idx >= 0);

	order[0] = first_idx;
	used[first_idx] = true;
	loop(dim, DIM) cur_dp[dim] = START_SCORE[first_idx][dim];

	int prev_idx = first_idx;
	loop(ord, 1, game.M) {
		int best_idx = -1;
		int best_score = INF;

		loop(next_idx, game.M) if (not used[next_idx]) {
			const auto &scores = GOALS_SCORE[prev_idx][next_idx];
			int cand_score = INF;

			loop(next_dim, DIM) {
				int top_val = INF;
				loop(prev_dim, DIM) {
					int val = cur_dp[prev_dim] + scores[prev_dim][next_dim];
					top_val = min(top_val, val);
				}
				cand_dp[next_dim] = top_val;
				cand_score = min(cand_score, top_val);
			}

			if (cand_score < best_score) {
				best_score = cand_score;
				best_idx = next_idx;
				best_dp = cand_dp;
			}
		}

		assert(best_idx >= 0);
		order[ord] = best_idx;
		used[best_idx] = true;
		cur_dp = best_dp;
		prev_idx = best_idx;
	}

	return order;
}

INLINE void mutate_order(Ord &new_order) {
	int b, a = rnd.next_u32(game.M);
	do b = rnd.next_u32(game.M);
	while (a == b);
	if (a > b) swap(a, b);

	switch (rnd.next_u32(6)) {
		case 0 ... 1: swap(new_order[a], new_order[b]); break;
		case 2: {
			const u8 moved = new_order[b];
			for (int i = b; i > a; --i) new_order[i] = new_order[i - 1];
			new_order[a] = moved;
			break;
		}
		case 3: {
			const u8 moved = new_order[a];
			for (int i = a; i < b; ++i) new_order[i] = new_order[i + 1];
			new_order[b] = moved;
			break;
		}
		case 4: rotate(new_order.begin(), new_order.begin() + a, new_order.begin() + b + 1); break;
		case 5: rotate(new_order.begin() + a, new_order.begin() + b, new_order.begin() + game.M); break;
	}
}

Ord make_greedy_candidate() {
	Ord best_order{};
	int best_score = INF;
	Ord order = make_greedy_order();
	int score = DP::eval_score_avx(order);
	if (score < best_score) {
		best_score = score;
		best_order = order;
	}
	return best_order;
}

constexpr pair<double, int> SCORE_COEFS[] = {
	{0.75, 32},
	{0.80, 24},
	{0.85, 16},
	{0.90, 12},
	{0.95, 8},
	{0.97, 4},
	{0.98, 2},
	{0.99, 1},
	{INT_MAX, 0},
};

struct Solver {
	static constexpr double START_TEMP = 1.0;
	static constexpr double FINAL_TEMP = 0.3;
	static constexpr double TEMP_DIFF = START_TEMP - FINAL_TEMP;

	static inline double TOTAL_STEPS_INV;

	int steps_done = 0;
	int top_exp = INF;
	int top_len = INF;
	Ord top_order{};
	int chain{};

	int top_c1{};
	int top_c2{};

	template<bool USE_TIME = false>
	void resume(const int steps) {
		// start with the default score composition
		DISTANCE_PRECOMP::compose_score_tables(chain, 1, 0);

		Ord cur_order = steps_done ? top_order : make_greedy_candidate();
		int cur_score = DP::eval_score_avx(cur_order);
		bool ok = DP::good_expanded_length(chain, cur_order);
		int top_score = ok ? cur_score : INF;
		top_order = ok ? cur_order : Ord{};

		const double start_prog = double(steps_done) * TOTAL_STEPS_INV;
		double prog = start_prog;
		double temp = START_TEMP - TEMP_DIFF * start_prog;

		double time_total_fake_inv{};
		if constexpr (USE_TIME) {
			int time_left = game.time_left_ms();
			// double fake_total = time_left / (1.0 - prog);
			// if (fake_total < 100) fake_total = 100;
			// time_total_fake_inv = 1.0 / fake_total;
			time_total_fake_inv = 1.0 / time_left;

			prog = 0.0;
			temp = START_TEMP;
		}

		double steps_inv = 1.0 / steps;
		double prog_local = 0;
		int cur_coef = 0;

		const int LIMIT = USE_TIME ? INT_MAX : steps;
		loop(step, LIMIT) {
			if ((step & 511) == 511) {
				if constexpr (USE_TIME) {
					const int time_left = game.time_left_ms();
					if (time_left <= 0) break;
					prog = 1.0 - time_left * time_total_fake_inv;
					// prog_local = (prog - start_prog) / (1.0 - start_prog);
					prog_local = prog;
				} else {
					prog = (step + steps_done) * TOTAL_STEPS_INV;
					prog_local = step * steps_inv;
				}

				temp = START_TEMP - TEMP_DIFF * prog;
				if (top_score == INF and prog_local >= SCORE_COEFS[cur_coef].first) {
					int c1 = SCORE_COEFS[cur_coef++].second;
					DISTANCE_PRECOMP::compose_score_tables(chain, c1, 1);
					cur_score = DP::eval_score_avx(cur_order);
				}
			}

			Ord new_order = cur_order;
			mutate_order(new_order);

			int new_score = DP::eval_score_avx(new_order);
			int diff = new_score - cur_score;

			if (diff <= 0 or exp(-double(diff) / temp) > rnd.next_double()) {
				cur_order = new_order;
				cur_score = new_score;

				if (cur_score < top_score) {
					if (DP::good_expanded_length(chain, cur_order)) {
						top_score = cur_score;
						top_order = cur_order;
					} else if (top_score < INF) {
						cur_score = top_score;
						cur_order = top_order;
					}
				}
			}
		}

		top_c1 = DISTANCE_PRECOMP::COMPOSE_STATE.c1;
		top_c2 = DISTANCE_PRECOMP::COMPOSE_STATE.c2;
		if (top_score == INF) top_order = cur_order;
		tie(top_len, top_exp) = DP::eval_both_lengths(chain, top_order);
		steps_done += steps;
	}

	bool operator<(const Solver &other) const {
		bool ok1 = top_exp <= game.T_EXCL_SWITCH;
		bool ok2 = other.top_exp <= game.T_EXCL_SWITCH;

		if (ok1 and ok2) return top_len < other.top_len;
		if (ok1 xor ok2) return ok1;
		return top_exp < other.top_exp;
	}
};

vector<int> get_budget_per_round(int budget, int cnt, double pace) {
	assert(0.0 < pace and pace < 1.0);
	vector<int> budgets;
	int total_rounds = 0;
	for (int c = cnt; c > 1; c = min<int>(c - 1, c * pace)) ++total_rounds;

	for (int r = total_rounds; r > 0; --r) {
		assert(cnt > 1);
		int pc = budget / r / cnt;
		int used = cnt * pc;
		assert(used <= budget);

		budget -= used;
		budgets.push_back(pc);
		cnt = min<int>(cnt - 1, cnt * pace);
	}
	return budgets;
}

Solver solve() {
	time_t fun_start = time();

	constexpr int WORK_BASE = 1'000'000;
	const double WORK = WORK_BASE * sqrt<int>(MAX_M);
	const double WORK_UNIT = sqrt<int>(game.M);

	constexpr double pace = 0.5;
	const int TOTAL_STEPS = WORK / WORK_UNIT;
	const int FINAL_STEPS = 0.3 * TOTAL_STEPS;
	const int ELIM_STEPS = TOTAL_STEPS - FINAL_STEPS;

	Solver top_solver;
	array<Solver, MAX_CHAINS> solvers;
	int solvers_remaining = NUM_CHAINS;
	loop(chain, solvers_remaining) solvers[chain].chain = chain;

	const auto budgets = get_budget_per_round(ELIM_STEPS, solvers_remaining, pace);
	const int WINNER_STEPS = accumulate(all(budgets), 0) + FINAL_STEPS;
	Solver::TOTAL_STEPS_INV = 1.0 / WINNER_STEPS;

	for (const int budget : budgets) {
		assert(solvers_remaining > 1);

		loop(chain, solvers_remaining) {
			solvers[chain].resume(budget);

			if constexpr (TIMED) {
				if (game.time_left_ms() < 100) {
					solvers_remaining = chain + 1;
					break;
				}
			}
		}
		sort(solvers.begin(), solvers.begin() + solvers_remaining);
		if (solvers[0] < top_solver) top_solver = solvers[0];

		if constexpr (TIMED) {
			if (game.time_left_ms() < 100) {
				break;
			}
		}
		solvers_remaining = min<int>(solvers_remaining - 1, solvers_remaining * pace);
	}
	if (solvers_remaining != 1) solvers_remaining = 1;
	derr << "[DATA] time2 = " << time_ms(fun_start) << endl;

	fun_start = time();
	solvers[0].resume<TIMED>(FINAL_STEPS);

	if (solvers[0] < top_solver) top_solver = solvers[0];
	assert(top_solver.top_exp <= game.T_EXCL_SWITCH);

	derr << "[DATA] time3 = " << time_ms(fun_start) << endl;
	return top_solver;
}

} // namespace SOLVER

void initialize() {
	game.init();

	MAX_POS1 = game.AREA;
	MAX_POS2 = MAX_POS1 * 4;
	MAX_POS3 = MAX_POS2 * NUM_MACROS;

	forward_delta1 = {1, game.N, -1, -game.N};
	loop(i, 4) {
		forward_delta2[i] = 4 * forward_delta1[i];
		forward_delta3[i] = 4 * NUM_MACROS * forward_delta1[i];
	}
	loop(pos2, MAX_POS2_C) {
		if (game.walls[pos2]) forward_dest2[pos2] = pos2;
		else forward_dest2[pos2] = pos2 + forward_delta2[pos2 & 3];
	}

	derr << "[DATA] N = " << game.N << endl;
	derr << "[DATA] M = " << game.M << endl;
	derr << "[DATA] T = " << game.T << endl;
	derr << "[DATA] W = " << game.WALLS_CNT << endl;
}

int main() {
	initialize();
	MACRO_OPTIM::anneal_macros_sequential();
	// loop(i, NUM_CHAINS) {
	// 	loop(j, 1, NUM_MACROS) {
	// 		derr << MACROS[i][j].str() << endl;
	// 	}
	// 	derr << endl;
	// }

	auto solver = SOLVER::solve();

	DISTANCE_PRECOMP::compose_score_tables(solver.chain, solver.top_c1, solver.top_c2);
	auto actions = CONSTRUCT_ACTIONS::construct_actions_from_order(solver.chain, solver.top_order);
	// assert(actions.size() == solver.top_len + 2 * game.M);

	// int sanity_exp = DP::eval_expanded_length(solver.chain, solver.top_order) + 2 * game.M;
	// assert(sanity_exp == solver.top_exp + 2 * game.M);

	// int length_exp = CONSTRUCT_ACTIONS::get_expanded_length_from_actions(actions);
	// assert(sanity_exp == length_exp);
	// assert(length_exp <= game.T);

	// actions = CONSTRUCT_ACTIONS::postprocess_actions(actions);
	derr << "[DATA] time = " << time_us(game.TIME_START) / 1e3 << endl;
	// derr << "[PROBE] selected_chain " << solver.chain << endl;
	// derr << "[PROBE] actions " << actions << endl;
	cout << actions << endl;
}
