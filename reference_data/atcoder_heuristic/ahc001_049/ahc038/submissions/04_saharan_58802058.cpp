#include <bit>
#include <ostream>
#if ONLINE_JUDGE
#define NDEBUG
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#else
#undef NDEBUG
#endif

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <complex>
#include <concepts>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <optional>
#include <queue>
#include <ranges>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace shr {
	namespace basic {
		using namespace std;
		using uchar = unsigned char;
		using uint = unsigned int;
		using ushort = unsigned short;
		using ull = unsigned long long;
		using ll = long long;
		using pii = pair<int, int>;
		using pdi = pair<double, int>;
#define fail(reason) assert(std::make_pair(reason, false).second)

		template <class T>
		concept printable = requires(T t, ostream& out) { out << t; };

		int len(const string& a) {
			return (int) a.length();
		}

		template <class T>
		int len(const vector<T>& a) {
			return (int) a.size();
		}

		template <class T>
		int len(const set<T>& a) {
			return (int) a.size();
		}

		template <class T>
		int len(const unordered_set<T>& a) {
			return (int) a.size();
		}

		template <class T>
		int len(const deque<T>& a) {
			return (int) a.size();
		}

		template <class T>
		int len(const queue<T>& a) {
			return (int) a.size();
		}

		template <class T>
		int len(const priority_queue<T>& a) {
			return (int) a.size();
		}

		template <class T, int N>
		int len(const T (&a)[N]) {
			return N;
		}

		template <class T, int N>
		void clear_with(T (&a)[N], int val) {
			memset(a, val, sizeof(a));
		}

		template <totally_ordered T>
		bool update_min(T& current_min, T candidate) {
			if (candidate < current_min) {
				current_min = candidate;
				return true;
			}
			return false;
		}

		template <totally_ordered T>
		bool update_min_eq(T& current_min, T candidate) {
			if (candidate <= current_min) {
				current_min = candidate;
				return true;
			}
			return false;
		}

		template <totally_ordered T>
		bool update_max(T& current_max, T candidate) {
			if (candidate > current_max) {
				current_max = candidate;
				return true;
			}
			return false;
		}

		template <totally_ordered T>
		bool update_max_eq(T& current_max, T candidate) {
			if (candidate >= current_max) {
				current_max = candidate;
				return true;
			}
			return false;
		}

		template <class T>
		string tos(T a) {
			return to_string(a);
		}

		template <class T>
		requires(!printable<T>)
		string tos(T a) {
			ostringstream os;
			bool first = true;
			auto from = a.begin();
			auto until = a.end();
			os << "{";
			while (from != until) {
				if (first) {
					first = false;
				} else {
					os << ", ";
				}
				os << tos(*from);
				from++;
			}
			os << "}";
			return os.str();
		}

		template <printable T>
		string tos(T a) {
			ostringstream os;
			os << a;
			return os.str();
		}

		constexpr double linearstep(double edge0, double edge1, double t) {
			return clamp((t - edge0) / (edge1 - edge0), 0.0, 1.0);
		}

		constexpr double smoothstep(double edge0, double edge1, double t) {
			t = linearstep(edge0, edge1, t);
			return t * t * (3 - 2 * t);
		}

		double exp_interp(double from, double to, double t) {
			return pow(from, 1 - t) * pow(to, t);
		}

		template <ranges::range Range, class F>
		auto mapped(Range& as, F f) {
			using T = ranges::range_value_t<Range>;
			using U = invoke_result_t<F, T&>;
			vector<U> res;
			for (auto& a : as) {
				res.push_back(f(a));
			}
			return res;
		}

		template <ranges::range Range, class F>
		auto mapped(const Range& as, F f) {
			using T = ranges::range_value_t<Range>;
			using U = invoke_result_t<F, const T&>;
			vector<U> res;
			for (auto& a : as) {
				res.push_back(f(a));
			}
			return res;
		}

		bool msg(const string& message, bool value) {
			return value;
		}
	} // namespace basic
	using namespace basic;

	namespace timer {
		double time_scale = 1.0;

		// return in ms
		int timer(bool reset = false) {
			static auto st = chrono::system_clock::now();
			if (reset) {
				st = chrono::system_clock::now();
				return 0;
			} else {
				auto en = chrono::system_clock::now();
				int elapsed = (int) chrono::duration_cast<chrono::milliseconds>(en - st).count();
				return (int) round(elapsed / time_scale);
			}
		}
	} // namespace timer

	namespace tracer {
		bool debug = true;

		template <class T>
		concept is_pair = requires(T t) {
			t.first;
			t.second;
		};

		template <class T>
		concept has_str = requires(T t) {
			{ t.str() } -> convertible_to<string>;
		};

		template <printable T>
		void tracen(T&& t) {
			if (!debug)
				return;
			cerr << t;
		}

		template <class T>
		requires(has_str<T> && !printable<T>)
		void tracen(T&& t) {
			if (!debug)
				return;
			cerr << t.str();
		}

		template <class T, class U>
		void tracen(pair<T, U>& t) { // <- ?????????????????????? need this for
			// trace(<iterable of pairs>)
			if (!debug)
				return;
			cerr << "(";
			tracen(t.first);
			cerr << ", ";
			tracen(t.second);
			cerr << ")";
		}

		template <class T, class U>
		void tracen(pair<T, U>&& t) { // <- ?????????????????????? need this for
			// trace(make_pair(1, 2))
			if (!debug)
				return;
			cerr << "(";
			tracen(t.first);
			cerr << ", ";
			tracen(t.second);
			cerr << ")";
		}

		template <class T>
		requires(!printable<T>)
		void tracen(T&& t) {
			if (!debug)
				return;
			bool first = true;
			auto from = t.begin();
			auto until = t.end();
			cerr << "{";
			while (from != until) {
				if (first) {
					first = false;
				} else {
					cerr << ", ";
				}
				tracen(*from);
				from++;
			}
			cerr << "}";
		}

		template <class T, int N>
		requires(!same_as<decay_t<T>, char>)
		void tracen(T (&a)[N]) {
			if (!debug)
				return;
			cerr << "{";
			for (int i = 0; i < N; i++) {
				if (i > 0)
					cerr << ", ";
				tracen(a[i]);
			}
			cerr << "}";
		}

		template <class T1, class T2, class... Rest>
		void tracen(T1&& t1, T2&& t2, Rest&&... rest) {
			if (!debug)
				return;
			tracen(std::forward<T1>(t1));
			tracen(std::forward<T2>(t2), std::forward<Rest>(rest)...);
		}

		void trace() {
			if (!debug)
				return;
			cerr << endl;
		}

		template <class T, class... Rest>
		void trace(T&& t, Rest&&... rest) {
			if (!debug)
				return;
			tracen(std::forward<T>(t), std::forward<Rest>(rest)...);
			cerr << endl;
		}

		template <class T>
		requires(!printable<T>)
		void trace2d(T&& t, int h, int w) {
			if (!debug)
				return;
			bool first = true;
			auto from = t.begin();
			auto until = t.end();
			for (int i = 0; i < h; i++) {
				for (int j = 0; j < w; j++) {
					if (j > 0)
						tracen(" ");
					tracen(*from);
					from++;
					if (j == w - 1)
						trace();
				}
			}
		}

		template <class T, int N>
		requires(!same_as<decay_t<T>, char>)
		void trace2d(T (&a)[N], int h, int w) {
			if (!debug)
				return;
			int idx = 0;
			for (int i = 0; i < h; i++) {
				for (int j = 0; j < w; j++) {
					if (j > 0)
						tracen(" ");
					tracen(a[idx]);
					idx++;
					if (j == w - 1)
						trace();
				}
			}
		}
	} // namespace tracer
	using namespace tracer;

	namespace random {
		class rngen {
		public:
			rngen() {
			}

			// to avoid bugs
			rngen(const rngen&) = delete;

			rngen& operator=(const rngen&&) = delete;

			rngen(int s) {
				seed(s);
			}

			ull get_state() {
				return state;
			}

			void set_state(ull state) {
				this->state = state;
			}

			void seed(int s) {
				state = s + INCR;
				next32();
			}

			int next_int() {
				return next31();
			}

			int next_int(int mod) {
				assert(mod > 0);
				return (int) ((ull) next31() * mod >> 31);
			}

			int next_int(int min, int max) {
				return min + next_int(max - min + 1);
			}

			uint next_uint() {
				return next32();
			}

			ull next_ull() {
				return (ull) next32() << 32 | next32();
			}

			double next_float() {
				return (double) next31() / 0x80000000;
			}

			double next_float(double min, double max) {
				return min + next_float() * (max - min);
			}

			double next_normal() {
				return sqrt(-2 * log(next_float())) * cos(6.283185307179586 * next_float());
			}

			double next_normal(double mean, double sigma) {
				return mean + next_normal() * sigma;
			}

		private:
			static constexpr ull MULT = 0x8b46ad15ae59daadull;
			static constexpr ull INCR = 0xf51827be20401689ull;
			ull state = (ull) chrono::duration_cast<chrono::nanoseconds>(
				chrono::system_clock::now().time_since_epoch())
							.count();

			uint next32() {
				uint r = (uint) (state >> 59);
				state = state * MULT + INCR;
				state ^= state >> 18;
				uint t = (uint) (state >> 27);
				return t >> r | t << (-r & 31);
			}

			int next31() {
				return (int) (next32() & 0x7fffffff);
			}
		};

		void random_permutation(int* a, int n, rngen& rng) {
			assert(n >= 0);
			if (n == 0)
				return;
			a[0] = 0;
			for (int i = 1; i < n; i++) {
				a[i] = i;
				swap(a[i], a[rng.next_int(i + 1)]);
			}
		}

		template <class RandomAccessContainer>
		void shuffle(RandomAccessContainer& c, rngen& rng) {
			int n = len(c);
			for (int i = 1; i < n; i++) {
				swap(c[i], c[rng.next_int(i + 1)]);
			}
		}

		template <class RandomAccessContainer>
		auto& random_pick(RandomAccessContainer& c, rngen& rng) {
			return c[rng.next_int(len(c))];
		}

		template <class RandomAccessContainer>
		const auto& random_pick(const RandomAccessContainer& c, rngen& rng) {
			return c[rng.next_int(len(c))];
		}

		template <class T, int N>
		T& random_pick(T (&c)[N], rngen& rng) {
			return c[rng.next_int(N)];
		}

		template <class T, int N>
		const T& random_pick(const T (&c)[N], rngen& rng) {
			return c[rng.next_int(N)];
		}
	} // namespace random
	using namespace random;

	namespace ds {
		// random access: O(1)
		// push: O(1)
		// insert: n/a
		// erase by position: O(1)
		// erase by element: n/a
		// max size: fixed
		template <class T, int N>
		class fast_vector {
		private:
			T data[N];
			int num = 0;

		public:
			using iterator = T*;
			using const_iterator = const T*;

			iterator begin() {
				return data;
			}

			iterator end() {
				return data + num;
			}

			const_iterator begin() const {
				return data;
			}

			const_iterator end() const {
				return data + num;
			}

			void push_back(T a) {
				assert(num < N);
				data[num++] = a;
			}

			template <class... Args>
			void emplace_back(Args&&... args) {
				push_back(T(forward<Args>(args)...));
			}

			void erase(iterator where) {
				assert(where >= begin() && where < end());
				*where = data[--num];
			}

			void pop_back() {
				assert(num > 0);
				num--;
			}

			T& operator[](int i) {
				assert(i >= 0 && i < num);
				return data[i];
			}

			const T& operator[](int i) const {
				assert(i >= 0 && i < num);
				return data[i];
			}

			int size() const {
				return num;
			}
		};

		// random access: O(1)
		// push: O(1)
		// insert: n/a
		// erase: n/a
		// reallocation: never
		template <class T, int UnitBits = 20>
		class increasing_vector {
		private:
			static constexpr int UNIT_SIZE = 1 << UnitBits;
			static constexpr int UNIT_MASK = UNIT_SIZE - 1;
			T** packs;
			int num_packs = 0;
			int num_total = 0;

		public:
			increasing_vector(const increasing_vector& vec) = delete;
			increasing_vector& operator=(const increasing_vector& vec) = delete;

			increasing_vector() : packs(new T*[65536]) {
			}

			~increasing_vector() {
				for (int i = 0; i < num_packs; i++) {
					delete[] packs[i];
				}
				delete[] packs;
			}

			T* next_pointer() {
				if ((num_total++ & UNIT_MASK) == 0) {
					packs[num_packs++] = new T[UNIT_SIZE];
				}
				return &(*this)[num_total - 1];
			}

			void push_back(T a) {
				*next_pointer() = a;
			}

			template <class... Args>
			void emplace_back(Args&&... args) {
				push_back(T(std::forward<Args>(args)...));
			}

			T& operator[](int i) {
				assert(i >= 0 && i < num_total);
				return packs[i >> UnitBits][i & UNIT_MASK];
			}

			const T& operator[](int i) const {
				assert(i >= 0 && i < num_total);
				return packs[i >> UnitBits][i & UNIT_MASK];
			}

			int size() const {
				return num_total;
			}
		};

		// random access: O(1)
		// insert: O(1)
		// erase: O(1)
		// check: O(1)
		// max value: fixed
		template <int N>
		class fast_iset {
		private:
			int data[N];
			int indices[N];
			int num = 0;

		public:
			using iterator = int*;
			using const_iterator = const int*;

			fast_iset() {
				memset(indices, -1, sizeof(indices));
			}

			iterator begin() {
				return data;
			}

			iterator end() {
				return data + num;
			}

			const_iterator begin() const {
				return data;
			}

			const_iterator end() const {
				return data + num;
			}

			bool insert(int a) {
				assert(a >= 0 && a < N);
				if (indices[a] != -1)
					return false;
				data[num] = a;
				indices[a] = num;
				num++;
				return true;
			}

			bool erase(int a) {
				assert(a >= 0 && a < N);
				int index = indices[a];
				if (index == -1)
					return false;
				assert(num > 0);
				indices[data[index] = data[--num]] = index;
				indices[a] = -1;
				return true;
			}

			void clear() {
				memset(indices, -1, sizeof(indices));
				num = 0;
			}

			bool contains(int a) const {
				return indices[a] != -1;
			}

			const int& operator[](int i) const {
				assert(i >= 0 && i < num);
				return data[i];
			}

			int size() const {
				return num;
			}

			bool empty() const {
				return num == 0;
			}
		};

		// insert: O(1)
		// get/set: O(1)
		// clear: O(1)
		// erase: n/a
		template <class T, int BucketBits = 20>
		class hash_imap {
		private:
			static constexpr int BUCKET_SIZE = 1 << BucketBits;
			static constexpr int BUCKET_MASK = BUCKET_SIZE - 1;

			ull* keys;
			T* values;
			ushort* access_time;
			ushort time = (ushort) -1;
			int num_elements = 0;
			int last_index = -1;
			ull last_key = -1;
			bool last_found = false;

		public:
			hash_imap()
				: keys(new ull[BUCKET_SIZE]), values(new T[BUCKET_SIZE]),
				  access_time(new ushort[BUCKET_SIZE]) {
			}

			~hash_imap() {
				delete[] keys;
				delete[] values;
				delete[] access_time;
			}

			hash_imap(const hash_imap& map)
				: keys(new ull[BUCKET_SIZE]), values(new T[BUCKET_SIZE]),
				  access_time(new ushort[BUCKET_SIZE]) {
				memcpy(keys, map.keys, sizeof(ull[BUCKET_SIZE]));
				memcpy(values, map.values,
					sizeof(T[BUCKET_SIZE])); // can be potentially dangerous?
				memcpy(access_time, map.access_time, sizeof(ushort[BUCKET_SIZE]));
				time = map.time;
				num_elements = map.num_elements;
				last_index = map.last_index;
				last_key = map.last_key;
				last_found = map.last_found;
			}

			hash_imap& operator=(const hash_imap& map) {
				if (this == &map)
					return *this;
				delete[] keys;
				delete[] values;
				delete[] access_time;
				keys = new ull[BUCKET_SIZE];
				values = new T[BUCKET_SIZE];
				access_time = new ushort[BUCKET_SIZE];
				memcpy(keys, map.keys, sizeof(ull[BUCKET_SIZE]));
				memcpy(values, map.values,
					sizeof(T[BUCKET_SIZE])); // can be potentially dangerous?
				memcpy(access_time, map.access_time, sizeof(ushort[BUCKET_SIZE]));
				time = map.time;
				num_elements = map.num_elements;
				last_index = map.last_index;
				last_key = map.last_key;
				last_found = map.last_found;
				return *this;
			}

			void clear() {
				num_elements = 0;
				last_found = false;
				last_index = -1;
				if (++time == 0) {
					memset(access_time, 0, sizeof(ushort[BUCKET_SIZE]));
					time = 1;
				}
			}

			bool access(ull key) {
				last_key = key;
				last_index = (int) (key & BUCKET_MASK);
				bool debug = false;
				while (true) {
					if (access_time[last_index] != time) {
						return last_found = false;
					} else if (keys[last_index] == key) {
						return last_found = true;
					}
					last_index = (last_index + 1) & BUCKET_MASK;
				}
			}

			T get() const {
				assert(last_found);
				return values[last_index];
			}

			void set(T value) {
				assert(last_index != -1);
				access_time[last_index] = time;
				keys[last_index] = last_key;
				values[last_index] = value;
				num_elements += !last_found;
				assert(msg("bucket size is too small", num_elements < 0.85 * BUCKET_SIZE));
			}
		};

		// a bitset, but cooler than std::bitset
		template <int Size>
		class rich_bitset {
		private:
			using word = ull;
			static_assert(has_single_bit(sizeof(word)));
			static constexpr int WORD_SHIFT = std::countr_zero(8 * sizeof(word));
			static constexpr int WORD_SIZE = 1 << WORD_SHIFT;
			static constexpr int WORD_MASK = WORD_SIZE - 1;
			static constexpr int NUM_WORDS = (Size + WORD_SIZE - 1) / WORD_SIZE;
			static constexpr int LAST_WORD = NUM_WORDS - 1;
			static constexpr word LAST_WORD_MASK =
				(Size & WORD_MASK) == 0 ? word(-1) : (word(1) << (Size & WORD_MASK)) - 1;
#define REP_WORDS(i) for (int i = 0; i < NUM_WORDS; i++)
#define REP_INNER_WORDS(i) for (int i = 0; i < NUM_WORDS - 1; i++)
#define REP_WORDS_REV(i) for (int i = NUM_WORDS - 1; i >= 0; i--)
#define REP_INNER_WORDS_REV(i) for (int i = NUM_WORDS - 2; i >= 0; i--)

			// [LAST_WORD] [LAST_WORD - 1] [...] [1] [0]
			// <- higher bits              lower bits ->
			word data[NUM_WORDS];

			struct ref {
				rich_bitset<Size>& bs;
				const int pos;

				ref(rich_bitset<Size>& bs, int pos) : bs(bs), pos(pos) {
				}

				ref& operator=(bool val) {
					bs.set(pos, val);
					return *this;
				}

				operator bool() const {
					return bs.test(pos);
				}
			};

			void trim() {
				if constexpr ((Size & WORD_MASK) != 0) {
					data[LAST_WORD] &= LAST_WORD_MASK;
				}
			}

		public:
			rich_bitset(ull value = 0) {
				constexpr int BITS = sizeof(ull) * 8;
				for (int i = 0; i < (BITS + WORD_SIZE - 1) / WORD_SIZE; i++) {
					data[i] = value >> i * WORD_SIZE;
				}
				constexpr int OFFSET = (BITS + WORD_SIZE - 1) / WORD_SIZE;
				if constexpr (OFFSET < NUM_WORDS) {
					memset(data + OFFSET, 0, sizeof(word) * (NUM_WORDS - OFFSET));
				}
			}

			bool all() const {
				bool res = true;
				REP_INNER_WORDS(i) {
					res &= data[i] == word(-1);
				}
				res &= data[LAST_WORD] == LAST_WORD_MASK;
				return res;
			}

			bool none() const {
				bool res = true;
				REP_WORDS(i) {
					res &= data[i] == 0;
				}
				return res;
			}

			bool any() const {
				bool res = false;
				REP_WORDS(i) {
					res |= data[i] != 0;
				}
				return res;
			}

			int count() const {
				int res = 0;
				REP_WORDS(i) {
					res += popcount(data[i]);
				}
				return res;
			}

			int countr_zero() const {
				if constexpr (LAST_WORD == 0) {
					return std::countr_zero(word(data[LAST_WORD] | ~LAST_WORD_MASK));
				} else {
					int res = std::countr_zero(data[0]);
					int mask = -(res == WORD_SIZE); // continue adding if -1
					for (int i = 1; i < NUM_WORDS - 1; i++) {
						int count = std::countr_zero(data[i]);
						res += count & mask;
						mask &= -(count == WORD_SIZE);
					}
					int count = std::countr_zero(word(data[LAST_WORD] | ~LAST_WORD_MASK));
					res += count & mask;
					return res;
				}
			}

			int countl_zero() const {
				constexpr int LAST_WORD_SIZE = popcount(LAST_WORD_MASK);
				int res = std::countl_zero(word(~(~data[LAST_WORD] << (WORD_SIZE - LAST_WORD_SIZE))));
				int mask = -(res == LAST_WORD_SIZE); // continue adding if -1
				for (int i = NUM_WORDS - 2; i >= 0; i--) {
					int count = std::countl_zero(data[i]);
					res += count & mask;
					mask &= -(count == WORD_SIZE);
				}
				return res;
			}

			int countr_one() const {
				if constexpr (LAST_WORD == 0) {
					return std::countr_one(data[LAST_WORD]);
				} else {
					int res = std::countr_one(data[0]);
					int mask = -(res == WORD_SIZE); // continue adding if -1
					for (int i = 1; i < NUM_WORDS - 1; i++) {
						int count = std::countr_one(data[i]);
						res += count & mask;
						mask &= -(count == WORD_SIZE);
					}
					int count = std::countr_one(data[LAST_WORD]);
					res += count & mask;
					return res;
				}
			}

			int countl_one() const {
				constexpr int LAST_WORD_SIZE = popcount(LAST_WORD_MASK);
				int res = std::countl_one(word(data[LAST_WORD] << (WORD_SIZE - LAST_WORD_SIZE)));
				int mask = -(res == LAST_WORD_SIZE); // continue adding if -1
				for (int i = NUM_WORDS - 2; i >= 0; i--) {
					int count = std::countl_one(data[i]);
					res += count & mask;
					mask &= -(count == WORD_SIZE);
				}
				return res;
			}

			int size() const {
				return Size;
			}

			bool test(int pos) const {
				assert(pos >= 0 && pos < Size);
				return (data[pos >> WORD_SHIFT] >> (pos & WORD_MASK)) & 1;
			}

			uint to_uint() const {
				constexpr int BITS = sizeof(uint) * 8;
				for (int i = (BITS + WORD_SIZE - 1) / WORD_SIZE; i < NUM_WORDS; i++) {
					assert(("uint overflow", data[i] == 0));
				}
				if constexpr (WORD_SIZE > BITS) {
					assert(("uint overflow", (data[0] >> BITS) == 0));
				}
				uint res = (uint) data[0];
				for (int i = 1; i < (BITS + WORD_SIZE - 1) / WORD_SIZE && i < NUM_WORDS; i++) {
					res |= (uint) data[i] << i * WORD_SIZE;
				}
				return res;
			}

			ull to_ull() const {
				constexpr int BITS = sizeof(ull) * 8;
				for (int i = (BITS + WORD_SIZE - 1) / WORD_SIZE; i < NUM_WORDS; i++) {
					assert(("ull overflow", data[i] == 0));
				}
				if constexpr (WORD_SIZE > BITS) {
					assert(("ull overflow", (data[0] >> BITS) == 0));
				}
				ull res = (ull) data[0];
				for (int i = 1; i < (BITS + WORD_SIZE - 1) / WORD_SIZE && i < NUM_WORDS; i++) {
					res |= (ull) data[i] << i * WORD_SIZE;
				}
				return res;
			}

			rich_bitset& set(int pos, bool val = true) {
				assert(pos >= 0 && pos < Size);
				word bit = word(1) << (pos & WORD_MASK);
				if (val) {
					data[pos >> WORD_SHIFT] |= bit;
				} else {
					data[pos >> WORD_SHIFT] &= ~bit;
				}
				return *this;
			}

			rich_bitset& reset(int pos) {
				assert(pos >= 0 && pos < Size);
				return set(pos, false);
			}

			rich_bitset& flip(int pos) {
				assert(pos >= 0 && pos < Size);
				word bit = word(1) << (pos & WORD_MASK);
				data[pos >> WORD_SHIFT] ^= bit;
				return *this;
			}

			rich_bitset& set() {
				clear_with(data, -1);
				trim();
				return *this;
			}

			rich_bitset& reset() {
				clear_with(data, 0);
				return *this;
			}

			rich_bitset& flip() {
				REP_INNER_WORDS(i) {
					data[i] ^= word(-1);
				}
				data[LAST_WORD] ^= LAST_WORD_MASK;
				return *this;
			}

			word* words() {
				return data;
			}

			rich_bitset& operator&=(const rich_bitset& a) {
				REP_WORDS(i) {
					data[i] &= a.data[i];
				}
				return *this;
			}

			rich_bitset& operator|=(const rich_bitset& a) {
				REP_WORDS(i) {
					data[i] |= a.data[i];
				}
				return *this;
			}

			rich_bitset& operator^=(const rich_bitset& a) {
				REP_WORDS(i) {
					data[i] ^= a.data[i];
				}
				return *this;
			}

			rich_bitset& operator<<=(int amount) {
				assert(amount >= 0 && amount < Size);
				int nw = amount >> WORD_SHIFT;
				if (nw > 0) {
					REP_WORDS_REV(i) {
						data[i] = i - nw < 0 ? 0 : data[i - nw];
					}
				}
				int nb = amount & WORD_MASK;
				if (nb) {
					for (int i = NUM_WORDS - 1; i > 0; i--) {
						data[i] = data[i] << nb | data[i - 1] >> (WORD_SIZE - nb);
					}
					data[0] <<= nb;
				}
				trim();
				return *this;
			}

			rich_bitset& operator>>=(int amount) {
				assert(amount >= 0 && amount < Size);
				int nw = amount >> WORD_SHIFT;
				if (nw > 0) {
					REP_WORDS(i) {
						data[i] = i + nw >= NUM_WORDS ? 0 : data[i + nw];
					}
				}
				int nb = amount & WORD_MASK;
				if (nb) {
					REP_INNER_WORDS(i) {
						data[i] = data[i] >> nb | data[i + 1] << (WORD_SIZE - nb);
					}
					data[LAST_WORD] >>= nb;
				}
				return *this;
			}

			rich_bitset& operator+=(const rich_bitset& a) {
				word carry = 0;
				REP_WORDS(i) {
					word l = data[i];
					word r = a.data[i];
					word sum = l + r;
					data[i] = sum + carry;
					carry = (sum < l) | (data[i] < sum);
				}
				trim();
				return *this;
			}

			rich_bitset& operator-=(const rich_bitset& a) {
				word carry = 1;
				REP_WORDS(i) {
					word l = data[i];
					word r = ~a.data[i];
					word sum = l + r;
					data[i] = sum + carry;
					carry = (sum < l) | (data[i] < sum);
				}
				trim();
				return *this;
			}

			rich_bitset& operator++() {
				word carry = 1;
				REP_WORDS(i) {
					word l = data[i];
					data[i] = l + carry;
					carry = (data[i] < l);
				}
				trim();
				return *this;
			}

			rich_bitset operator++(int) {
				rich_bitset res = *this;
				operator++();
				return res;
			}

			rich_bitset& operator--() {
				word carry = 0;
				REP_WORDS(i) {
					word l = data[i];
					data[i] = l - 1 + carry;
					carry = (l | carry) != 0;
				}
				trim();
				return *this;
			}

			rich_bitset operator--(int) {
				rich_bitset res = *this;
				operator--();
				return res;
			}

			rich_bitset operator~() const {
				rich_bitset res = *this;
				res.flip();
				return res;
			}

			friend rich_bitset operator&(const rich_bitset& a, const rich_bitset& b) {
				rich_bitset res = a;
				res &= b;
				return res;
			}

			friend rich_bitset operator|(const rich_bitset& a, const rich_bitset& b) {
				rich_bitset res = a;
				res |= b;
				return res;
			}

			friend rich_bitset operator^(const rich_bitset& a, const rich_bitset& b) {
				rich_bitset res = a;
				res ^= b;
				return res;
			}

			friend rich_bitset operator<<(const rich_bitset& a, int amount) {
				rich_bitset res = a;
				res <<= amount;
				return res;
			}

			friend rich_bitset operator>>(const rich_bitset& a, int amount) {
				rich_bitset res = a;
				res >>= amount;
				return res;
			}

			friend rich_bitset operator+(const rich_bitset& a, const rich_bitset& b) {
				rich_bitset res = a;
				res += b;
				return res;
			}

			friend rich_bitset operator-(const rich_bitset& a, const rich_bitset& b) {
				rich_bitset res = a;
				res -= b;
				return res;
			}

			friend bool operator==(const rich_bitset& a, const rich_bitset& b) {
				return memcmp(a.data, b.data, sizeof(a.data)) == 0;
			}

			friend bool operator!=(const rich_bitset& a, const rich_bitset& b) {
				return memcmp(a.data, b.data, sizeof(a.data)) != 0;
			}

			friend int operator<=>(const rich_bitset& a, const rich_bitset& b) {
				REP_WORDS_REV(i) {
					if (a.data[i] != b.data[i])
						return a.data[i] < b.data[i] ? -1 : 1;
				}
				return 0;
			}

			ref operator[](int pos) {
				return {*this, pos};
			}

			bool operator[](int pos) const {
				return test(pos);
			}

			string str() const {
				ostringstream oss;
				oss << *this;
				return oss.str();
			}

			friend ostream& operator<<(ostream& out, const rich_bitset& bs) {
				for (int i = Size - 1; i >= 0; i--) {
					out << (bs.test(i) ? '1' : '0');
				}
				return out;
			}
#undef REP_WORDS
#undef REP_INNER_WORDS
#undef REP_WORDS_REV
#undef REP_INNER_WORDS_REV
		};

		template <class T>
		class easy_stack {
		public:
			vector<T> data;

			auto begin() {
				return data.begin();
			}

			auto end() {
				return data.end();
			}

			auto begin() const {
				return data.begin();
			}

			auto end() const {
				return data.end();
			}

			void clear() {
				data.clear();
			}

			void push_back(T a) {
				data.push_back(a);
			}

			template <class... Args>
			auto& emplace_back(Args&&... args) {
				return data.emplace_back(std::forward<Args>(args)...);
			}

			T pop_back() {
				T res = data.back();
				data.pop_back();
				return res;
			}

			int size() const {
				return (int) data.size();
			}

			bool empty() const {
				return data.empty();
			}
		};

		template <int N>
		int len(const fast_iset<N>& a) {
			return a.size();
		}

		template <class T, int N>
		int len(const fast_vector<T, N>& a) {
			return a.size();
		}

		template <class T, int BucketBits>
		int len(const hash_imap<T, BucketBits>& a) {
			return a.size();
		}

		template <class T>
		int len(const easy_stack<T>& a) {
			return a.size();
		}

		template <class T>
		requires(same_as<T, char> || same_as<T, short> || same_as<T, int>)
		struct int_vec2 {
			T i;
			T j;

			template <class U>
			constexpr int_vec2(int_vec2<U> a) : i(a.i), j(a.j) {
				assert(i == a.i);
				assert(j == a.j);
			}

			constexpr int_vec2() : i(0), j(0) {
			}

			constexpr int_vec2(T i, T j) : i(i), j(j) {
			}

			constexpr static int_vec2 dir(int index) {
				constexpr T DIRS[4][2] = {
					{-1, 0},
					{1, 0},
					{0, -1},
					{0, 1},
				};
				return {DIRS[index][0], DIRS[index][1]};
			}

			constexpr int dir_index() const {
				assert((i != 0) + (j != 0) == 1);
				return i < 0 ? 0 : i > 0 ? 1 : j < 0 ? 2 : 3;
			}

			constexpr int_vec2 rot(int sij, int num = 1) const {
				num &= 3;
				int_vec2 res = {i, j};
				while (num) {
					res = {sij - 1 - res.j, res.i};
					num--;
				}
				return res;
			}

			constexpr int_vec2 min(int_vec2 a) const {
				return {std::min(i, a.i), std::min(j, a.j)};
			}

			constexpr int_vec2 max(int_vec2 a) const {
				return {std::max(i, a.i), std::max(j, a.j)};
			}

			constexpr int_vec2 clamp(int_vec2 min, int_vec2 max) const {
				return {std::clamp(i, min.i, max.i), std::clamp(j, min.j, max.j)};
			}

			int_vec2 abs() const {
				return {std::abs(i), std::abs(j)};
			}

			int norm() const {
				return std::abs(i) + std::abs(j);
			}

			double enorm() const {
				return sqrt(lldot(*this));
			}

			constexpr int dot(int_vec2 a) const {
				return i * a.i + j * a.j;
			}

			constexpr ll lldot(int_vec2 a) const {
				return (ll) i * a.i + (ll) j * a.j;
			}

			constexpr int pack(int sij) const {
				return pack(sij, sij);
			}

			constexpr int pack(int si, int sj) const {
				assert(in_bounds(si, sj));
				return i * sj + j;
			}

			constexpr int pack_if_in_bounds(int sij) const {
				return pack_if_in_bounds(sij, sij);
			}

			constexpr int pack_if_in_bounds(int si, int sj) const {
				if (!in_bounds(si, sj))
					return -1;
				return i * sj + j;
			}

			constexpr static int_vec2 unpack(int packed, int sij) {
				return unpack(packed, sij, sij);
			}

			constexpr static int_vec2 unpack(int packed, int si, int sj) {
				uint p = packed;
				uint i = packed / sj;
				uint j = packed - i * sj;
				assert(int_vec2(i, j).in_bounds(si, sj));
				return int_vec2(i, j);
			}

			constexpr bool in_bounds(int sij) const {
				return in_bounds(sij, sij);
			}

			constexpr bool in_bounds(int si, int sj) const {
				return i >= 0 && i < si && j >= 0 && j < sj;
			}

			constexpr int_vec2 operator+() const {
				return {i, j};
			}

			constexpr int_vec2 operator-() const {
				return {-i, -j};
			}

			constexpr friend int_vec2 operator+(int_vec2 a, int_vec2 b) {
				return {a.i + b.i, a.j + b.j};
			}

			constexpr friend int_vec2 operator+(T a, int_vec2 b) {
				return {a + b.i, a + b.j};
			}

			constexpr friend int_vec2 operator+(int_vec2 a, T b) {
				return {a.i + b, a.j + b};
			}

			constexpr friend int_vec2 operator-(int_vec2 a, int_vec2 b) {
				return {a.i - b.i, a.j - b.j};
			}

			constexpr friend int_vec2 operator-(T a, int_vec2 b) {
				return {a - b.i, a - b.j};
			}

			constexpr friend int_vec2 operator-(int_vec2 a, T b) {
				return {a.i - b, a.j - b};
			}

			constexpr friend int_vec2 operator*(int_vec2 a, int_vec2 b) {
				return {a.i * b.i, a.j * b.j};
			}

			constexpr friend int_vec2 operator*(T a, int_vec2 b) {
				return {a * b.i, a * b.j};
			}

			constexpr friend int_vec2 operator*(int_vec2 a, T b) {
				return {a.i * b, a.j * b};
			}

			constexpr friend int_vec2 operator/(int_vec2 a, int_vec2 b) {
				return {a.i / b.i, a.j / b.j};
			}

			constexpr friend int_vec2 operator/(T a, int_vec2 b) {
				return {a / b.i, a / b.j};
			}

			constexpr friend int_vec2 operator/(int_vec2 a, T b) {
				return {a.i / b, a.j / b};
			}

			constexpr friend int_vec2 operator%(int_vec2 a, int_vec2 b) {
				return {a.i % b.i, a.j % b.j};
			}

			constexpr friend int_vec2 operator%(T a, int_vec2 b) {
				return {a % b.i, a % b.j};
			}

			constexpr friend int_vec2 operator%(int_vec2 a, T b) {
				return {a.i % b, a.j % b};
			}

			constexpr int_vec2 operator+=(int_vec2 a) {
				i += a.i;
				j += a.j;
				return *this;
			}

			constexpr int_vec2 operator+=(T a) {
				i += a;
				j += a;
				return *this;
			}

			constexpr int_vec2 operator-=(int_vec2 a) {
				i -= a.i;
				j -= a.j;
				return *this;
			}

			constexpr int_vec2 operator-=(T a) {
				i -= a;
				j -= a;
				return *this;
			}

			constexpr int_vec2 operator*=(int_vec2 a) {
				i *= a.i;
				j *= a.j;
				return *this;
			}

			constexpr int_vec2 operator*=(T a) {
				i *= a;
				j *= a;
				return *this;
			}

			constexpr int_vec2 operator/=(int_vec2 a) {
				i /= a.i;
				j /= a.j;
				return *this;
			}

			constexpr int_vec2 operator/=(T a) {
				i /= a;
				j /= a;
				return *this;
			}

			constexpr int_vec2 operator%=(int_vec2 a) {
				i %= a.i;
				j %= a.j;
				return *this;
			}

			constexpr int_vec2 operator%=(T a) {
				i %= a;
				j %= a;
				return *this;
			}

			constexpr friend bool operator==(int_vec2 a, int_vec2 b) {
				return a.i == b.i && a.j == b.j;
			}

			constexpr friend bool operator!=(int_vec2 a, int_vec2 b) {
				return a.i != b.i || a.j != b.j;
			}

			constexpr friend bool operator<(int_vec2 a, int_vec2 b) {
				return a.i < b.i || a.i == b.i && a.j < b.j;
			}

			constexpr friend bool operator<=(int_vec2 a, int_vec2 b) {
				return a.i <= b.i || a.i == b.i && a.j <= b.j;
			}

			constexpr friend bool operator>(int_vec2 a, int_vec2 b) {
				return a.i > b.i || a.i == b.i && a.j > b.j;
			}

			constexpr friend bool operator>=(int_vec2 a, int_vec2 b) {
				return a.i >= b.i || a.i == b.i && a.j >= b.j;
			}

			friend ostream& operator<<(ostream& out, int_vec2 a) {
				out << "(" << a.i << ", " << a.j << ")";
				return out;
			}
		};

		template <class T>
		requires(same_as<T, char> || same_as<T, short> || same_as<T, int>)
		struct int_vec3 {
			T i;
			T j;
			T k;

			template <class U>
			constexpr int_vec3(int_vec3<U> a) : i(a.i), j(a.j), k(a.k) {
				assert(i == a.i);
				assert(j == a.j);
				assert(k == a.k);
			}

			constexpr int_vec3() : i(0), j(0), k(0) {
			}

			constexpr int_vec3(T i, T j, T k) : i(i), j(j), k(k) {
			}

			constexpr static int_vec3 dir(int index) {
				constexpr T DIRS[6][3] = {
					{-1, 0, 0},
					{1, 0, 0},
					{0, -1, 0},
					{0, 1, 0},
					{0, 0, -1},
					{0, 0, 1},
				};
				return {DIRS[index][0], DIRS[index][1], DIRS[index][2]};
			}

			constexpr int dir_index() const {
				assert((i != 0) + (j != 0) + (k != 0) == 1);
				return i < 0 ? 0 : i > 0 ? 1 : j < 0 ? 2 : j > 0 ? 3 : k < 0 ? 4 : 5;
			}

			constexpr int_vec3 min(int_vec3 a) const {
				return {std::min(i, a.i), std::min(j, a.j), std::min(k, a.k)};
			}

			constexpr int_vec3 max(int_vec3 a) const {
				return {std::max(i, a.i), std::max(j, a.j), std::max(k, a.k)};
			}

			constexpr int_vec3 clamp(int_vec3 min, int_vec3 max) const {
				return {
					std::clamp(i, min.i, max.i), std::clamp(j, min.j, max.j), std::clamp(k, min.k, max.k)};
			}

			int_vec3 abs() const {
				return {std::abs(i), std::abs(j), std::abs(k)};
			}

			int norm() const {
				return std::abs(i) + std::abs(j) + std::abs(k);
			}

			constexpr int dot(int_vec3 a) const {
				return i * a.i + j * a.j + k * a.k;
			}

			constexpr int pack(int sijk) const {
				return pack(sijk, sijk, sijk);
			}

			constexpr int pack(int si, int sj, int sk) const {
				assert(in_bounds(si, sj, sk));
				return (i * sj + j) * sk + k;
			}

			constexpr int pack_if_in_bounds(int sijk) const {
				return pack_if_in_bounds(sijk, sijk, sijk);
			}

			constexpr int pack_if_in_bounds(int si, int sj, int sk) const {
				if (!in_bounds(si, sj, sk))
					return -1;
				return (i * sj + j) * sk + k;
			}

			constexpr static int_vec3 unpack(int packed, int sijk) {
				return unpack(packed, sijk, sijk, sijk);
			}

			constexpr static int_vec3 unpack(int packed, int si, int sj, int sk) {
				uint p = packed;
				uint ij = p / sk;
				uint k = p - ij * sk;
				uint i = ij / sj;
				uint j = ij - i * sj;
				assert(int_vec3(i, j, k).in_bounds(si, sj, sk));
				return int_vec3(i, j, k);
			}

			constexpr bool in_bounds(int sijk) const {
				return in_bounds(sijk, sijk, sijk);
			}

			constexpr bool in_bounds(int si, int sj, int sk) const {
				return i >= 0 && i < si && j >= 0 && j < sj && k >= 0 && k < sk;
			}

			constexpr int_vec3 operator+() const {
				return {i, j, k};
			}

			constexpr int_vec3 operator-() const {
				return {-i, -j, -k};
			}

			constexpr friend int_vec3 operator+(int_vec3 a, int_vec3 b) {
				return {a.i + b.i, a.j + b.j, a.k + b.k};
			}

			constexpr friend int_vec3 operator+(T a, int_vec3 b) {
				return {a + b.i, a + b.j, a + b.k};
			}

			constexpr friend int_vec3 operator+(int_vec3 a, T b) {
				return {a.i + b, a.j + b, a.k + b};
			}

			constexpr friend int_vec3 operator-(int_vec3 a, int_vec3 b) {
				return {a.i - b.i, a.j - b.j, a.k - b.k};
			}

			constexpr friend int_vec3 operator-(T a, int_vec3 b) {
				return {a - b.i, a - b.j, a - b.k};
			}

			constexpr friend int_vec3 operator-(int_vec3 a, T b) {
				return {a.i - b, a.j - b, a.k - b};
			}

			constexpr friend int_vec3 operator*(int_vec3 a, int_vec3 b) {
				return {a.i * b.i, a.j * b.j, a.k * b.k};
			}

			constexpr friend int_vec3 operator*(T a, int_vec3 b) {
				return {a * b.i, a * b.j, a * b.k};
			}

			constexpr friend int_vec3 operator*(int_vec3 a, T b) {
				return {a.i * b, a.j * b, a.k * b};
			}

			constexpr friend int_vec3 operator/(int_vec3 a, int_vec3 b) {
				return {a.i / b.i, a.j / b.j, a.k / b.k};
			}

			constexpr friend int_vec3 operator/(T a, int_vec3 b) {
				return {a / b.i, a / b.j, a / b.k};
			}

			constexpr friend int_vec3 operator/(int_vec3 a, T b) {
				return {a.i / b, a.j / b, a.k / b};
			}

			constexpr friend int_vec3 operator%(int_vec3 a, int_vec3 b) {
				return {a.i % b.i, a.j % b.j, a.k % b.k};
			}

			constexpr friend int_vec3 operator%(T a, int_vec3 b) {
				return {a % b.i, a % b.j, a % b.k};
			}

			constexpr friend int_vec3 operator%(int_vec3 a, T b) {
				return {a.i % b, a.j % b, a.k % b};
			}

			constexpr int_vec3 operator+=(int_vec3 a) {
				i += a.i;
				j += a.j;
				k += a.k;
				return *this;
			}

			constexpr int_vec3 operator+=(T a) {
				i += a;
				j += a;
				k += a;
				return *this;
			}

			constexpr int_vec3 operator-=(int_vec3 a) {
				i -= a.i;
				j -= a.j;
				k -= a.k;
				return *this;
			}

			constexpr int_vec3 operator-=(T a) {
				i -= a;
				j -= a;
				k -= a;
				return *this;
			}

			constexpr int_vec3 operator*=(int_vec3 a) {
				i *= a.i;
				j *= a.j;
				k *= a.k;
				return *this;
			}

			constexpr int_vec3 operator*=(T a) {
				i *= a;
				j *= a;
				k *= a;
				return *this;
			}

			constexpr int_vec3 operator/=(int_vec3 a) {
				i /= a.i;
				j /= a.j;
				k /= a.k;
				return *this;
			}

			constexpr int_vec3 operator/=(T a) {
				i /= a;
				j /= a;
				k /= a;
				return *this;
			}

			constexpr int_vec3 operator%=(int_vec3 a) {
				i %= a.i;
				j %= a.j;
				k %= a.k;
				return *this;
			}

			constexpr int_vec3 operator%=(T a) {
				i %= a;
				j %= a;
				k %= a;
				return *this;
			}

			constexpr friend bool operator==(int_vec3 a, int_vec3 b) {
				return a.i == b.i && a.j == b.j && a.k == b.k;
			}

			constexpr friend bool operator!=(int_vec3 a, int_vec3 b) {
				return a.i != b.i || a.j != b.j || a.k != b.k;
			}

			constexpr friend bool operator<(int_vec3 a, int_vec3 b) {
				return a.i < b.i || a.i == b.i && (a.j < b.j || a.j == b.j && a.k < b.k);
			}

			constexpr friend bool operator<=(int_vec3 a, int_vec3 b) {
				return a.i <= b.i || a.i == b.i && (a.j <= b.j || a.j == b.j && a.k <= b.k);
			}

			constexpr friend bool operator>(int_vec3 a, int_vec3 b) {
				return a.i > b.i || a.i == b.i && (a.j > b.j || a.j == b.j && a.k > b.k);
			}

			constexpr friend bool operator>=(int_vec3 a, int_vec3 b) {
				return a.i >= b.i || a.i == b.i && (a.j >= b.j || a.j == b.j && a.k >= b.k);
			}

			friend ostream& operator<<(ostream& out, int_vec3 a) {
				out << "(" << a.i << ", " << a.j << ", " << a.k << ")";
				return out;
			}
		};

		using cvec2 = int_vec2<char>;
		using svec2 = int_vec2<short>;
		using ivec2 = int_vec2<int>;
		using cvec3 = int_vec3<char>;
		using svec3 = int_vec3<short>;
		using ivec3 = int_vec3<int>;
	} // namespace ds
	using namespace ds;

	namespace beam_search {
		// (state) -> score
		template <class T, class State, class Score>
		concept get_score =
			totally_ordered<Score> && invocable<T, State&> && same_as<invoke_result_t<T, State&>, Score>;

		// (state, move) -> void
		template <class T, class State, class MoveId>
		concept apply_move =
			invocable<T, State&, MoveId> && same_as<invoke_result_t<T, State&, MoveId>, void>;

		// (state) -> void
		template <class T, class State>
		concept undo_move = invocable<T, State&> && same_as<invoke_result_t<T, State&>, void>;

		// (state) -> void
		// see also: add_candidate
		template <class T, class State>
		concept enumerate_candidates = invocable<T, State&> && same_as<invoke_result_t<T, State&>, void>;

		// (turn) -> void
		// see also: candidates_to_filter
		template <class T>
		concept filter_candidates = invocable<T, int> && same_as<invoke_result_t<T, int>, void>;

		template <class State, totally_ordered Score, class MoveId, MoveId UnusedMoveId, class CandidateData,
			class Direction = greater<Score>, int HashBucketBits = 20>
		class beam_search {
		private:
			struct candidate {
				int index; // index in orig_candidates
				int parent;
				MoveId move_id;
				CandidateData data;
				ull hash;
			};
			struct orig_candidate {
				int parent;
				MoveId move_id;
				bool chosen;
			};

			Direction dir = {};
			int current_parent = 0;
			hash_imap<int, HashBucketBits> best_indices;
			bool enumerating = false;
			bool filtering = false;
			vector<candidate> candidates;
			vector<orig_candidate> orig_candidates;

			void clear_candidates() {
				candidates.clear();
				orig_candidates.clear();
			}

		public:
			Score best_score = 0;
			int max_turn = -1;

			beam_search() {
			}

			beam_search(Direction dir) : dir(dir) {
			}

			void add_candidate(MoveId move_id, CandidateData data, ull hash) {
				assert(msg("not enumerating now", enumerating));
				candidates.emplace_back((int) candidates.size(), current_parent, move_id, data, hash);
				orig_candidates.emplace_back(current_parent, move_id);
			}

			vector<candidate>& candidates_to_filter() {
				assert(msg("not filtering now", filtering));
				return candidates;
			}

			// CAUTION: not stable
			template <predicate<candidate&, candidate&> CandidateDirection>
			void remove_duplicates(CandidateDirection candidate_direction) {
				assert(msg("not filtering now", filtering));
				best_indices.clear();
				int n = (int) candidates.size();
				for (int i = 0; i < n; i++) {
					candidate& cand = candidates[i];
					if (best_indices.access(cand.hash)) {
						int j = best_indices.get();
						candidate& cand2 = candidates[j];
						if (candidate_direction(cand, cand2)) {
							swap(candidates[i], candidates[j]);
						}
						swap(candidates[i], candidates[--n]);
						i--;
					} else {
						best_indices.set(i);
					}
				}
				candidates.resize(n);
			}

			template <get_score<State, Score> GetScore, apply_move<State, MoveId> ApplyMove,
				enumerate_candidates<State> EnumerateCandidates, filter_candidates FilterCandidates>
			vector<MoveId> run(const State& initial_state, GetScore get_score, ApplyMove apply_move,
				EnumerateCandidates enumerate_candidates, FilterCandidates filter_candidates) {
				struct node {
					State state;
					int history_index;
				};
				struct history {
					MoveId move_id;
					int parent;
				};
				vector<node> src;
				vector<node> dst;
				increasing_vector<history> hs;
				int turn = 0;

				// set initial state
				src.emplace_back(initial_state, -1);

				while (true) {
					int num_states = (int) src.size();

					clear_candidates();
					if (max_turn == -1 || turn < max_turn) {
						// enumerate candidates
						enumerating = true;
						for (int i = 0; i < num_states; i++) {
							current_parent = i;
							enumerate_candidates(src[i].state);
						}
						enumerating = false;

						// filer candiadtes
						filtering = true;
						filter_candidates(turn);
						filtering = false;
					}

					// check if finished
					if (candidates.empty()) {
						assert(msg("no states at the end", num_states > 0));

						// pick the best state
						best_score = get_score(src[0].state);
						int best_index = 0;
						for (int i = 1; i < num_states; i++) {
							Score score = get_score(src[i].state);
							if (dir(score, best_score)) {
								best_score = score;
								best_index = i;
							}
						}

						// restore moves
						vector<MoveId> res;
						int history_top = src[best_index].history_index;
						while (history_top != -1) {
							history& h = hs[history_top];
							res.push_back(h.move_id);
							history_top = h.parent;
						}
						reverse(res.begin(), res.end());
						return res;
					}

					// compute next states
					dst.clear();
					for (const auto& cand : candidates) {
						const auto& src_node = src[cand.parent];
						dst.emplace_back(src_node.state, hs.size());
						apply_move(dst.back().state, cand.move_id);
						hs.emplace_back(cand.move_id, src_node.history_index);
					}
					src.swap(dst);
					turn++;
				}
			}

			template <get_score<State, Score> GetScore, apply_move<State, MoveId> ApplyMove,
				undo_move<State> UndoMove, enumerate_candidates<State> EnumerateCandidates,
				filter_candidates FilterCandidates>
			vector<MoveId> run_tree(const State& initial_state, GetScore get_score, ApplyMove apply_move,
				UndoMove undo_move, EnumerateCandidates enumerate_candidates,
				FilterCandidates filter_candidates) {
				constexpr MoveId UNDO = UnusedMoveId;
				struct tour {
					vector<MoveId> src;
					vector<MoveId> dst;

					void move(const MoveId& move_id) {
						dst.push_back(move_id);
					}

					int position() {
						return (int) dst.size();
					}

					void swap() {
						src.swap(dst);
						dst.clear();
					}
				} tour;
				vector<MoveId> global_path;
				vector<MoveId> path;
				vector<orig_candidate> leaves;
				State st = initial_state;
				int turn = 0;
				int level = 0;
				int next_start_pos = 0;

				auto global_move = [&](const MoveId& move_id) {
					apply_move(st, move_id);
					global_path.push_back(move_id);
					level++;
				};

				auto global_undo = [&]() {
					undo_move(st);
					global_path.pop_back();
					level--;
				};

				while (true) {
					bool has_next_turn = max_turn == -1 || turn < max_turn;

					// compute the next tour
					int pos = next_start_pos;
					int prev_root_level = level;
					int next_root_level = numeric_limits<int>::max();
					orig_candidate best_leaf = {-1, MoveId{}, false};
					enumerating = true;
					clear_candidates();
					if (turn == 0) {
						best_score = get_score(st);
						best_leaf.chosen = true;
						if (has_next_turn) {
							current_parent = tour.position();
							enumerate_candidates(st);
						}
					} else {
						for (const orig_candidate& leaf : leaves) {
							int parent_pos = leaf.parent;

							// visit the parent of the leaf node
							if (pos < parent_pos) {
								// visit the LCA
								path.clear();
								do {
									auto move = tour.src[pos++];
									if (move == UNDO) {
										if (path.empty()) {
											global_undo();
											tour.move(UNDO);
											next_root_level = min(next_root_level, level);
										} else {
											path.pop_back();
										}
									} else {
										path.push_back(move);
									}
								} while (pos < parent_pos);

								// go directly to the parent
								for (auto move : path) {
									global_move(move);
									tour.move(move);
								}
							} // now we are at the parent of the leaf node

							// visit the leaf node
							apply_move(st, leaf.move_id);
							tour.move(leaf.move_id);

							Score score = get_score(st);
							if (!best_leaf.chosen || dir(score, best_score)) {
								best_score = score;
								best_leaf = leaf;
							}
							if (has_next_turn) {
								current_parent = tour.position();
								enumerate_candidates(st);
							}

							// leave the leaf node
							undo_move(st);
							tour.move(UNDO);
						}
					}
					next_root_level = min(next_root_level, level);
					enumerating = false;

					filtering = true;
					filter_candidates(turn);
					filtering = false;

					if (candidates.empty()) {
						assert(best_leaf.chosen);
						// undo to the root level
						while (level > prev_root_level) {
							global_undo();
						}
						// visit the best leaf
						pos = next_start_pos;
						while (pos < best_leaf.parent) {
							auto move = tour.src[pos++];
							if (move == UNDO) {
								global_undo();
							} else {
								global_move(move);
							}
						}
						if (best_leaf.parent != -1) {
							global_move(best_leaf.move_id);
						}
						return global_path;
					}

					// finalize the next tour
					tour.swap();
					turn++;

					// collect the next leaf nodes, in the original order
					leaves.clear();
					for (const candidate& cand : candidates) {
						orig_candidates[cand.index].chosen = true;
					}
					for (const orig_candidate& cand : orig_candidates) {
						if (!cand.chosen)
							continue;
						leaves.push_back(cand);
					}

					// undo to the next root level
					while (level > next_root_level) {
						global_undo();
					}

					// adjust the next starting position
					next_start_pos = next_root_level - prev_root_level;
				}
			}
		};

		class beam_width_manager {
		private:
			double prev_time = 0;
			double moving_average_time = 0;
			vector<double> progress_history;
			vector<double> time_history;
			vector<int> width_history;
			int last_width = 0;
			int count = 0;

		public:
			int window_size = 50;
			int default_width;

			beam_width_manager(int default_width) : default_width(default_width) {
			}

			int next(double progress, double time, double time_limit) {
				progress_history.push_back(progress);
				time_history.push_back(time);
				width_history.push_back(last_width);
				count++;
				if (count <= window_size) {
					return last_width = default_width;
				}
				int i1 = count - 1 - window_size;
				int i2 = count - 1;
				double progress_sum = progress_history[i2] - progress_history[i1];
				double time_sum = time_history[i2] - time_history[i1];
				if (progress_sum == 0 || time_sum == 0) {
					// window size is too small
					window_size *= 2;
					return last_width = default_width;
				}
				int width_sum = 0;
				for (int i = i1 + 1; i <= i2; i++) {
					width_sum += width_history[i];
				}
				double progress_per_turn = progress_sum / window_size;
				double time_per_width = time_sum / width_sum;
				double left_time = time_limit - time;
				double left_progress = 1 - progress;
				if (left_time <= 0 || left_progress <= 0)
					return 1;
				double left_turn = left_progress / progress_per_turn;
				double left_time_per_turn = left_time / left_turn;
				double left_width_per_turn = left_time_per_turn / time_per_width;
				return last_width = (int) round(left_width_per_turn);
			}

			void report(int actual_last_width) {
				last_width = actual_last_width;
			}
		};
	} // namespace beam_search

	namespace simulated_annealing {
		// (state) -> score
		template <class T, class State, class Score>
		concept get_score =
			totally_ordered<Score> && invocable<T, State&> && same_as<invoke_result_t<T, State&>, Score>;

		// (iter) -> progress
		template <class T>
		concept update_progress = invocable<T, int> && same_as<invoke_result_t<T, int>, double>;

		// (state, tolerance) -> accepted
		template <class T, class State>
		concept try_transition =
			invocable<T, State&, double> && same_as<invoke_result_t<T, State&, double>, bool>;

		template <class State, totally_ordered Score, class Direction = greater<Score>>
		class simulated_annealing {
		private:
			Direction dir = {};

		public:
			int clock_interval = 10;
			double t_from = 100;
			double t_to = 0.01;
			double progress = 0;
			int num_iterations = 0;
			int num_acceptances = 0;
			int num_rejections = 0;
			bool use_linear_temp = false;
			Score best_score = 0;

			simulated_annealing() {
			}

			simulated_annealing(Direction dir) : dir(dir) {
			}

			template <get_score<State, Score> GetScore, update_progress UpdateProgress,
				try_transition<State> TryTransition>
			State run(const State& initial_state, rngen& rng, GetScore get_score,
				UpdateProgress update_progress, TryTransition try_transition,
				function<void(State&, Score, int, double)> best_updated = nullptr) {
				State state = initial_state;
				Score score = get_score(state);
				State best_state = state;
				best_score = score;

				num_iterations = 0;
				num_acceptances = 0;
				num_rejections = 0;
				int interval = clock_interval;
				progress = 0;
				double t = t_from;
				while (true) {
					if (--interval <= 0) {
						progress = update_progress(num_iterations);
						if (progress >= 1)
							break;
						t = use_linear_temp ? lerp(t_from, t_to, progress)
											: exp_interp(t_from, t_to, progress);
						interval = clock_interval;
					}
					double tolerance = t * -log(rng.next_float());
					if (try_transition(state, tolerance)) {
						num_acceptances++;
						score = get_score(state);
						if (dir(score, best_score)) {
							best_state = state;
							best_score = score;
							if (best_updated) {
								best_updated(state, score, num_iterations, t);
							}
						}
					} else {
						num_rejections++;
					}
					num_iterations++;
				}
				return best_state;
			}
		};
	} // namespace simulated_annealing

	namespace dijkstra {
		// (vertex) -> index
		template <class T, class Vertex>
		concept get_index = invocable<T, Vertex> && same_as<invoke_result_t<T, Vertex>, int>;

		// (vertex) -> is_goal
		template <class T, class Vertex>
		concept is_goal = invocable<T, Vertex> && same_as<invoke_result_t<T, Vertex>, bool>;

		// (vertex, distance) -> void
		template <class T, class Vertex, class Weight>
		concept visit_adjacent_vertices =
			invocable<T, Vertex, Weight> && same_as<invoke_result_t<T, Vertex, Weight>, void>;

		template <class Vertex, class Weight, Weight Infinity, int MaxVertexIndex>
		requires(integral<Weight> || floating_point<Weight>)
		class dijkstra {
		private:
			using vw = pair<Vertex, Weight>;
			static constexpr int VERTEX_ARRAY_SIZE = MaxVertexIndex + 1;
			vector<vw> toVisit;
			bool visiting = false;

		public:
			array<bool, VERTEX_ARRAY_SIZE> visited;
			array<Weight, VERTEX_ARRAY_SIZE> distance;
			array<optional<Vertex>, VERTEX_ARRAY_SIZE> previous;

			dijkstra() {
			}

			// - get_index: `(vertex) -> index`
			// - is_goal: `(vertex) -> bool`
			// - visit_adjacent_vertices: `(vertex, distance) -> void`
			template <get_index<Vertex> GetIndex, is_goal<Vertex> IsGoal,
				visit_adjacent_vertices<Vertex, Weight> VisitAdjacentVertices>
			void run(const vector<Vertex>& starts, GetIndex get_index, IsGoal is_goal,
				VisitAdjacentVertices visit_adjacent_vertices) {
				auto comp = [](vw& a, vw& b) {
					return a.second > b.second;
				};

				visited.fill(false);
				previous.fill(nullopt);
				distance.fill(Infinity);
				priority_queue<vw, vector<vw>, decltype(comp)> q(comp);
				for (auto& st : starts) {
					distance[get_index(st)] = Weight(0);
					q.emplace(st, Weight(0));
				}

				while (!q.empty()) {
					auto [from, dist] = q.top();
					q.pop();
					int fromi = get_index(from);
					if (visited[fromi])
						continue;
					visited[fromi] = true;
					if (is_goal(from)) {
						return;
					}

					visiting = true;
					toVisit.clear();
					visit_adjacent_vertices(from, dist);
					visiting = false;

					for (vw& pair : toVisit) {
						Vertex to = pair.first;
						int toi = get_index(to);
						Weight new_dist = pair.second;
						if (new_dist < distance[toi]) {
							distance[toi] = new_dist;
							previous[toi] = from;
							q.emplace(to, new_dist);
						}
					}
				}
			}

			void visit(Vertex vertex, Weight total_distance) {
				assert(visiting);
				toVisit.emplace_back(vertex, total_distance);
			}

			template <get_index<Vertex> GetIndex>
			vector<Vertex> restore_path(Vertex goal, GetIndex get_index) {
				assert(msg("goal is not reached", visited[get_index(goal)]));
				vector<Vertex> res;
				Vertex v = goal;
				while (true) {
					res.push_back(v);
					if (!previous[get_index(v)])
						break;
					v = previous[get_index(v)].value();
				}
				reverse(res.begin(), res.end());
				return res;
			}
		};
	}; // namespace dijkstra

	namespace file {
		string pad4(int n) {
			return (n < 0 || n >= 1000 ? "" : n < 10 ? "000" : n < 100 ? "00" : "0") + tos(n);
		}

		string input_file_name(int seed) {
			return "in/" + pad4(seed) + ".txt";
		}

		string output_file_name(int seed) {
			return "out/" + pad4(seed) + ".txt";
		}

		string movie_file_name(int seed) {
			return "mov/" + pad4(seed) + ".smv";
		}

		bool write_text(string fileName, string text, bool append = false) {
			ofstream fout;
			fout.open(fileName, append ? ios::out | ios::app : ios::out);
			if (!fout)
				return false;
			fout << text;
			fout.close();
			return true;
		}

		pair<string, bool> read_text(string fileName) {
			ifstream fin;
			fin.open(fileName, ios::in);
			if (!fin)
				return make_pair("", false);
			string res;
			string line;
			while (getline(fin, line)) {
				res += line;
				res += "\n";
			}
			return make_pair(res, true);
		}

		void write_text_assert(string fileName, string text, bool append = false) {
			auto res = write_text(fileName, text, append);
			assert(res);
		}

		string read_text_assert(string fileName) {
			auto res = read_text(fileName);
			assert(res.second);
			return res.first;
		}
	} // namespace file
} // namespace shr
using namespace shr::basic;
using namespace shr::ds;
using namespace shr::beam_search;
using namespace shr::simulated_annealing;
using namespace shr::dijkstra;
using namespace shr::random;
using namespace shr::timer;
using namespace shr::tracer;
using namespace shr::file;

//
// --- macros ---
//

#define rep(i, from, until) for (int i = (from); i < (until); i++)
#define repr(i, from, until) for (int i = (until) - 1; i >= (from); i--)
#define rep0(i, until) rep(i, 0, until)
#define rep0r(i, until) repr(i, 0, until)

//
// --- movie lib ---
//

class movie {
private:
	bool is_open = false;
	string file_name;
	ofstream out;

#ifdef ONLINE_JUDGE
	void write_instruction(const string& it, const vector<double>& argf, const vector<string>& args) {
	}
#else
	void write_instruction(const string& it, const vector<double>& argf, const vector<string>& args) {
		assert(msg("file name is not set", file_name != ""));
		if (!is_open) {
			is_open = true;
			out = ofstream(file_name, ios_base::out | ios_base::binary);
			out.write("smv", 3);
		}
		write_string(it);
		for (double f : argf) {
			write_float(f);
		}
		for (auto& s : args) {
			write_string(s);
		}
		if (it == "n") {
			out.flush(); // flush at the end of each frame
		}
	}

	void write_float(double a) {
		float f = (float) a;
		out.write((char*) &f, 4);
	}

	void write_string(const string& str) {
		out.write(str.c_str(), str.length() + 1);
	}
#endif

public:
	static constexpr int LEFT = 0;
	static constexpr int CENTER = 1;
	static constexpr int RIGHT = 2;

	movie() {
	}

	void set_file(string file) {
		assert(!is_open);
		file_name = file;
	}

	void close_file() {
		if (!is_open)
			return;
		is_open = false;
		out.close();
	}

	void fill(double rgb, double a = 1.0) {
		fill(rgb, rgb, rgb, a);
	}

	void fill(double r, double g, double b, double a = 1.0) {
		write_instruction("f", {r, g, b, a}, {});
	}

	void stroke(double rgb, double a = 1.0) {
		stroke(rgb, rgb, rgb, a);
	}

	void stroke(double r, double g, double b, double a = 1.0) {
		write_instruction("s", {r, g, b, a}, {});
	}

	void no_fill() {
		write_instruction("nf", {}, {});
	}

	void no_stroke() {
		write_instruction("ns", {}, {});
	}

	void comment(string text) {
		write_instruction("cm", {}, {text});
	}

	void tooltip(string text) {
		write_instruction("tt", {}, {text});
	}

	void no_tooltip() {
		write_instruction("nt", {}, {});
	}

	template <class T>
	void with_tooltip(string text, T f) {
		tooltip(text);
		f();
		no_tooltip();
	}

	void stroke_weight(double weight) {
		write_instruction("sw", {weight}, {});
	}

	void text_size(double size) {
		write_instruction("ts", {size}, {});
	}

	void text_align(int align) {
		write_instruction("ta", {(double) align}, {});
	}

	void rect(double x, double y, double w, double h) {
		write_instruction("r", {x, y, w, h}, {});
	}

	void circle(double x, double y, double r) {
		write_instruction("c", {x, y, r}, {});
	}

	void ellipse(double x, double y, double rx, double ry) {
		write_instruction("e", {x, y, rx, ry}, {});
	}

	void line(double x1, double y1, double x2, double y2) {
		write_instruction("l", {x1, y1, x2, y2}, {});
	}

	void text(string str, double x, double y) {
		write_instruction("t", {x, y}, {str});
	}

	void transform(double e00, double e01, double e10, double e11) {
		write_instruction("tf", {e00, e01, e10, e11}, {});
	}

	void translate(double tx, double ty) {
		write_instruction("tr", {tx, ty}, {});
	}

	void rotate(double ang) {
		write_instruction("ro", {ang}, {});
	}

	void scale(double s) {
		scale(s, s);
	}

	void scale(double sx, double sy) {
		write_instruction("sc", {sx, sy}, {});
	}

	void push() {
		write_instruction("pu", {}, {});
	}

	void pop() {
		write_instruction("po", {}, {});
	}

	void end_frame() {
		write_instruction("n", {}, {});
	}

	void target(string name) {
		write_instruction("tg", {}, {name});
	}
};

movie mov;

// --------------------------------------------------
// main part
// --------------------------------------------------

bool isLocal = false;
bool render = false;

// define N and stuff here
constexpr int INF = 1000000000;
constexpr int MAX_N = 30;
constexpr int MAX_N2 = MAX_N * MAX_N;
constexpr int MAX_M = MAX_N2 / 2;
constexpr int MAX_V = 15;
constexpr int EXTENDED_N = MAX_N + MAX_V * 2;
#define repn(i) rep0(i, n)
#define rep2() repn(i) repn(j)

inline constexpr int rotToDir(int rot) {
	constexpr int R2D[4] = {3, 1, 2, 0};
	return R2D[rot & 3];
}

inline constexpr ivec2 rotToDirVec(int rot) {
	return ivec2::dir(rotToDir(rot));
}

struct Problem {
	int n;
	int m;
	int v;
	bool isSrc[MAX_N][MAX_N];
	bool isDst[MAX_N][MAX_N];

	void load(istream& in) {
		in >> n >> m >> v;
		assert(n <= MAX_N);
		assert(m <= MAX_M);
		assert(v <= MAX_V);
		repn(i) {
			string line;
			in >> line;
			repn(j) {
				isSrc[i][j] = line[j] == '1';
			}
		}
		repn(i) {
			string line;
			in >> line;
			repn(j) {
				isDst[i][j] = line[j] == '1';
			}
		}
	}
};

struct RobotMove {
	short dir = -1;
	int drotBits = 0; // 2 bits per arm

	int getDrot(int armIndex) const {
		return (drotBits >> (armIndex << 1)) & 3;
	}

	void setDrot(int armIndex, int drot) {
		drotBits &= ~(3 << (armIndex << 1));
		drotBits |= (drot & 3) << (armIndex << 1);
	}
};

struct TurnMove {
	RobotMove move;
	int pickBits = 0;

	// equal comparator
	bool operator==(const TurnMove& other) const {
		// use memcmp for performance
		return memcmp(this, &other, sizeof(TurnMove)) == 0;
	}

	// print
	friend ostream& operator<<(ostream& out, const TurnMove& m) {
		out << "{dir: " << (int) m.move.dir << ", drots: ";
		rep0(i, MAX_V) {
			out << m.move.getDrot(i) << " ";
		}
		out << ", picks: ";
		rep0(i, MAX_V) {
			out << m.getPick(i) << " ";
		}
		out << "}";
		return out;
	}

	bool getPick(int armIndex) const {
		return (pickBits >> armIndex) & 1;
	}

	void setPick(int armIndex, bool pick) {
		pickBits = (pickBits & ~(1 << armIndex)) | (pick << armIndex);
	}
};

struct RobotState {
	char i;
	char j;
	array<char, MAX_V - 1> localRots = {};
};

struct Arm {
	int index;
	int length;
	int localRot;
	int globalRot;
	ivec2 pos;
	array<ivec2, 4> rotToDiff;

	Arm(int index, int length, int localRot, int globalRot)
		: index(index), length(length), localRot(localRot), globalRot(globalRot),
		  rotToDiff({{0, length}, {length, 0}, {0, -length}, {-length, 0}}) {
	}
};

struct Robot {
	ivec2 root;
	vector<Arm> globalArms;
	vector<Arm> localArms;

	Robot() {
	}

	void configureArms(const vector<int>& globalArmLengths, const vector<int>& localArmLengths) {
		globalArms.clear();
		localArms.clear();

		int index = 1; // 0 is reserved for the root
		for (int gl : globalArmLengths) {
			globalArms.emplace_back(index++, gl, 0, 0);
		}
		for (int ll : localArmLengths) {
			localArms.emplace_back(index++, ll, 0, 0);
		}

		vector<pii> localArmLengthAndIndexPairs;
		rep0(i, len(localArmLengths)) {
			localArmLengthAndIndexPairs.emplace_back(localArmLengths[i], i);
		}
		ranges::sort(localArmLengthAndIndexPairs);

		init();
	}

	void init() {
		root = {};
		for (auto& a : globalArms) {
			a.localRot = 0;
		}
		for (auto& a : localArms) {
			a.localRot = 0;
		}
		update();
	}

	void update() {
		ivec2 pos = root;
		int globalRot = 0;
		for (auto& a : globalArms) {
			globalRot = (globalRot + a.localRot) & 3;
			a.globalRot = globalRot;
			pos += a.rotToDiff[a.globalRot];
			a.pos = pos;
		}
		for (auto& a : localArms) {
			a.globalRot = globalRot + a.localRot;
			a.pos = pos + a.rotToDiff[a.globalRot & 3];
		}
	}

	void move(const RobotMove& m) {
		if (m.dir != -1) {
			root += ivec2::dir(m.dir);
		}
		for (auto& a : globalArms) {
			a.localRot = (a.localRot + m.getDrot(a.index)) & 3;
		}
		for (auto& a : localArms) {
			a.localRot = (a.localRot + m.getDrot(a.index)) & 3;
		}
		update();
	}

	void undoMove(const RobotMove& m) {
		if (m.dir != -1) {
			root -= ivec2::dir(m.dir);
		}
		for (auto& a : globalArms) {
			a.localRot = (a.localRot - m.getDrot(a.index)) & 3;
		}
		for (auto& a : localArms) {
			a.localRot = (a.localRot - m.getDrot(a.index)) & 3;
		}
		update();
	}

	RobotState getState() const {
		RobotState state;
		state.i = root.i;
		state.j = root.j;
		for (auto& a : globalArms) {
			state.localRots[a.index - 1] = a.localRot;
		}
		for (auto& a : localArms) {
			state.localRots[a.index - 1] = a.localRot;
		}
		return state;
	}

	void setState(const RobotState& state) {
		root = {state.i, state.j};
		for (auto& a : globalArms) {
			a.localRot = state.localRots[a.index - 1];
		}
		for (auto& a : localArms) {
			a.localRot = state.localRots[a.index - 1];
		}
		update();
	}
};

struct PerimeterManager {
	bool on[MAX_N2] = {};
	int n = 0;
	int perimeter = 0;

	void init(int n) {
		this->n = n;
		perimeter = 0;
		clear_with(on, 0);
	}

	void switchOn(ivec2 pos) {
		int ipos = pos.pack(n);
		bool wasOn = on[ipos];
		on[ipos] ^= 1;
		int same = 0;
		rep0(dir, 4) {
			ivec2 pos2 = pos + ivec2::dir(dir);
			int npos2 = pos2.pack_if_in_bounds(n);
			if (wasOn == (npos2 != -1 && on[npos2])) {
				same++;
			}
		}
		perimeter += 2 * same - 4;
	}
};

struct Field {
	int n = 0;
	int m = 0;

	fast_iset<MAX_N2> goals; // not fulfilled goals
	fast_iset<MAX_N2> lefts; // wrongly placed positions

	PerimeterManager goalPerimeter;
	PerimeterManager leftPerimeter;

	ivec2 unpack[MAX_N2];

	array<rich_bitset<MAX_N2>, MAX_N2> masks;
	array<rich_bitset<MAX_N2>, EXTENDED_N * EXTENDED_N> reachMasks;
	rich_bitset<MAX_N2> todoBits;

	int numGoodPairs = 0;

	int sumI = 0;
	int sumJ = 0;
	int sumII = 0;
	int sumIJ = 0;
	int sumJJ = 0;
	int numTodos = 0;

	int sumGoalI = 0;
	int sumGoalJ = 0;
	int sumLeftI = 0;
	int sumLeftJ = 0;

	Field() {
	}

	void setupMasks(int numLocalArms) {
		int maxDist = numLocalArms;
		rep2() {
			ivec2 pos = {i, j};
			int idx = pos.pack(n);
			auto& mask = masks[idx];
			mask.reset();
			int maskCount[MAX_N2] = {};

			repn(gi) {
				repn(gj) {
					if (gi != i && gj != j || gi == i && gj == j) {
						continue;
					}
					ivec2 gpos = {gi, gj};
					int dist = (pos - gpos).norm();
					if (dist > maxDist)
						continue;
					rep(d, 1, maxDist + 1) {
						if (d == dist)
							continue;
						rep0(dir, 4) {
							ivec2 pos2 = gpos + ivec2::dir(dir) * d;
							int idx2 = pos2.pack_if_in_bounds(n);
							if (idx2 != -1) {
								maskCount[idx2]++;
							}
						}
					}
				}
			}

			rep2() {
				ivec2 pos2 = {i, j};
				int idx2 = pos2.pack(n);
				if (maskCount[idx2] > 0) {
					mask.set(idx2);
				}
			}
			// trace("mask for ", pos, ":");
			// repn(i) {
			// 	string s;
			// 	repn(j) {
			// 		s += mask.test(i * n + j) ? "1" : "0";
			// 	}
			// 	trace("  ", s);
			// }

			// trace("mask count for ", pos, ":");
			// repn(i) {
			// 	string s;
			// 	repn(j) {
			// 		int num = maskCount[i * n + j] / 2;
			// 		s += (num < 10 ? " " : "") + tos(num) + " ";
			// 	}
			// 	trace("  ", s);
			// }
		}

		int nExtended = n + MAX_V * 2;
		rep0(i, nExtended) {
			rep0(j, nExtended) {
				ivec2 rawPos = {i, j};
				int idx = rawPos.pack(nExtended);
				ivec2 pos = rawPos - MAX_V;
				auto& reachMask = reachMasks[idx];
				reachMask.reset();
				rep(d, 1, maxDist + 1) {
					rep0(dir, 4) {
						ivec2 pos2 = pos + ivec2::dir(dir) * d;
						int idx2 = pos2.pack_if_in_bounds(n);
						if (idx2 != -1) {
							reachMask.set(idx2);
						}
					}
				}
			}
		}

		initTodoBits();
	}

	void init(int n, int m, const vector<ivec2>& srcs, const vector<ivec2>& dsts) {
		this->n = n;
		this->m = m;
		assert(len(srcs) == m);
		assert(len(dsts) == m);
		clear_with(placedData, 0);
		clear_with(goalData, 0);
		goals.clear();
		lefts.clear();
		sumI = 0;
		sumJ = 0;
		sumII = 0;
		sumIJ = 0;
		sumJJ = 0;
		numTodos = 0;
		sumGoalI = 0;
		sumGoalJ = 0;
		sumLeftI = 0;
		sumLeftJ = 0;
		numWords = (n * n + 63) / 64;
		for (auto p : dsts) {
			int idx = p.pack(n);
			goals.insert(idx);
			goalData[idx] = true;
		}
		for (auto p : srcs) {
			int idx = p.pack(n);
			placedData[idx] = true;
			if (goalData[idx]) {
				goals.erase(idx);
			} else {
				lefts.insert(idx);
			}
		}
		goalPerimeter.init(n);
		leftPerimeter.init(n);
		for (int p : goals) {
			ivec2 pos = ivec2::unpack(p, n);
			sumGoalI += pos.i;
			sumGoalJ += pos.j;
			goalPerimeter.switchOn(pos);
		}
		for (int p : lefts) {
			ivec2 pos = ivec2::unpack(p, n);
			sumLeftI += pos.i;
			sumLeftJ += pos.j;
			leftPerimeter.switchOn(pos);
		}
		rep0(i, n * n) {
			unpack[i] = ivec2::unpack(i, n);
		}

		initTodoBits();
	}

	double todoCost() const {
		return len(goals) + len(lefts);
	}

	int totalPerimeter() const {
		return goalPerimeter.perimeter + leftPerimeter.perimeter;
	}

	double computeMeanDiff() const {
		int numGoals = len(goals);
		int numLefts = len(lefts);
		if (numGoals == 0 || numLefts == 0)
			return 0;
		double invNumGoals = 1.0 / numGoals;
		double invNumLefts = 1.0 / numLefts;
		return abs(sumGoalI * invNumGoals - sumLeftI * invNumLefts) +
			abs(sumGoalJ * invNumGoals - sumLeftJ * invNumLefts);
	}

	double computeVariance() const {
		if (numTodos <= 1)
			return 0;
		double invNum = 1.0 / numTodos;
		double meanI = sumI * invNum;
		double meanJ = sumJ * invNum;
		double meanII = sumII * invNum;
		double meanIJ = sumIJ * invNum;
		double meanJJ = sumJJ * invNum;
		double varI = meanII - meanI * meanI;
		double varJ = meanJJ - meanJ * meanJ;
		double covIJ = meanIJ - meanI * meanJ;
		return varI * varJ - covIJ * covIJ;
	}

	pair<double, double> computeStddevs() const {
		if (numTodos <= 1)
			return {0, 0};
		double invNum = 1.0 / numTodos;
		double meanI = sumI * invNum;
		double meanJ = sumJ * invNum;
		double meanII = sumII * invNum;
		double meanIJ = sumIJ * invNum;
		double meanJJ = sumJJ * invNum;
		double varI = meanII - meanI * meanI;
		double varJ = meanJJ - meanJ * meanJ;
		
		return {sqrt(varI), sqrt(varJ)};

		// double t = varI + varJ;
		// double det = varI * varJ - covIJ * covIJ;
		// double sd = sqrt(t * t - 4 * det);
		// double l1 = (t + sd) / 2;
		// double l2 = (t - sd) / 2;
		// return {max(0.0, l1), max(0.0, l2)};
	}

	bool isPlaced(ivec2 pos) const {
		int idx = pos.pack_if_in_bounds(n);
		return idx != -1 && placedData[idx];
	}

	bool isGoal(ivec2 pos) const {
		int idx = pos.pack_if_in_bounds(n);
		return idx != -1 && goalData[idx];
	}

	void pickOrDrop(ivec2 pos, int idx) {
		if (placedData[idx]) {
			pick(pos, idx);
		} else {
			drop(pos, idx);
		}
	}

	void pick(ivec2 pos, int idx) {
		// int idx = pos.pack(n);
		assert(placedData[idx]);
		placedData[idx] = false;

		if (goalData[idx]) {
			goals.insert(idx);
			sumGoalI += pos.i;
			sumGoalJ += pos.j;
			goalPerimeter.switchOn(pos);
		} else {
			lefts.erase(idx);
			sumLeftI -= pos.i;
			sumLeftJ -= pos.j;
			leftPerimeter.switchOn(pos);
		}

		flipTodo(pos, idx);
	}

	void drop(ivec2 pos, int idx) {
		// int idx = pos.pack(n);
		assert(!placedData[idx]);
		placedData[idx] = true;

		if (goalData[idx]) {
			goals.erase(idx);
			sumGoalI -= pos.i;
			sumGoalJ -= pos.j;
			goalPerimeter.switchOn(pos);
		} else {
			lefts.insert(idx);
			sumLeftI += pos.i;
			sumLeftJ += pos.j;
			leftPerimeter.switchOn(pos);
		}

		flipTodo(pos, idx);
	}

private:
	bool placedData[MAX_N2]; // true if placed
	bool goalData[MAX_N2]; // true if goal
	int numWords = 0;

	void initTodoBits() {
		numGoodPairs = 0;
		todoBits.reset();
		rep2() {
			ivec2 pos = {i, j};
			int idx = pos.pack(n);
			if (placedData[idx] != goalData[idx]) {
				flipTodo(pos, idx);
			}
		}
	}

	void flipTodo(ivec2 pos, int idx) {
		auto maskWords = masks[idx].words();
		auto todoWords = todoBits.words();
		int delta = 0;
		rep0(i, numWords) {
			delta += popcount(maskWords[i] & todoWords[i]);
		}
		if (todoBits.test(idx)) {
			numTodos--;
			sumI -= pos.i;
			sumJ -= pos.j;
			sumII -= pos.i * pos.i;
			sumIJ -= pos.i * pos.j;
			sumJJ -= pos.j * pos.j;
			numGoodPairs -= delta;
		} else {
			numTodos++;
			sumI += pos.i;
			sumJ += pos.j;
			sumII += pos.i * pos.i;
			sumIJ += pos.i * pos.j;
			sumJJ += pos.j * pos.j;
			numGoodPairs += delta;
		}
		todoBits.flip(idx);
	}
};

struct SolverResult {
	ll score = 0;
};

int PARAM_BASE_LENGTH = 8;
int PARAM_NUM_GLOBAL_ARMS = 3;

class Solver {
public:
	int seed;
	rngen rng;
	SolverResult result;
	Problem p;

	int n;
	int m;
	int maxV;
	int v;
	Robot robot;
	Field field;

	double density;
	double perimeterDensity;

	vector<TurnMove> sol;

	Solver() : rng(12345) {
	}

	void load(istream& in, int seed = -1) {
		p.load(in);
		n = p.n;
		m = p.m;
		maxV = p.v;
		v = 0;
		mov.set_file(movie_file_name(seed));
		init();
	}

	void load(int seed) {
		this->seed = seed;
		istringstream in(read_text_assert(input_file_name(seed)));
		isLocal = true;
		load(in, seed);
	}

	void solve() {
		if (isLocal) {
			ostringstream out;
			solveMain(out);
			mov.close_file();
			write_text(output_file_name(seed), out.str());
		} else {
			solveMain(cout);
		}
	}

private:
	ivec2 unpack[MAX_N2];
	ivec2 globalEndPos[1024];
	int globalEndRot[1024];
	int numWords = 0;

	struct State { // state for beam search
		Robot robot;
		array<bool, MAX_V> hold = {};
		Field field;
		ull hash = 0;
		easy_stack<TurnMove> moves;
	};

	void init() {
		rep0(i, n * n) {
			unpack[i] = ivec2::unpack(i, n);
		}
		initField();

		// compute density and perimeter density
		int numTodo = field.lefts.size() + field.goals.size();
		density = (double) (numTodo) / (n * n);
		perimeterDensity = (double) (field.totalPerimeter()) / (numTodo * 4);

		numWords = (n * n + 63) / 64;
	}

	void initField() {
		vector<ivec2> srcs;
		vector<ivec2> dsts;
		rep2() {
			if (p.isSrc[i][j])
				srcs.push_back({i, j});
			if (p.isDst[i][j])
				dsts.push_back({i, j});
		}
		field.init(n, m, srcs, dsts);
	}

	void configureRobot() {
		int baseLength;
		int numGlobalArms;
		// switch (maxV) {
		// case 5:
		// case 6:
		// case 7:
		// 	baseLength = 8;
		// 	numGlobalArms = 2;
		// 	break;
		// default:
		// 	if (n <= 24) {
		// 		baseLength = 8;
		// 		numGlobalArms = 3;
		// 	} else {
		// 		baseLength = 16;
		// 		numGlobalArms = 4;
		// 	}
		// 	break;
		// }

		// int numTodo = field.lefts.size() + field.goals.size();
		// double density = (double) (numTodo) / (n * n);
		// double perimeter = field.totalPerimeter() / (double) (numTodo * 4);
		// trace("density: ", density, " perimeter: ", perimeter);

		// if (perimeter < 0.5 && numGlobalArms >= (maxV >= 10 ? 4 : 3)) {
		// 	numGlobalArms--;
		// }

		if (true) {
			// score prediction
			constexpr array<array<int, 2>, 7> lengths = {
				{{8, 2}, {8, 3}, {8, 4}, {12, 2}, {12, 3}, {12, 4}, {12, 5}}};

			constexpr array<array<double, 15>, 7> coeffs = {{
				{0.02398692696363722, -0.03884752439128093, -0.348047481645801, -0.6690049992268905,
					-0.001265924007831494, 0.0018057403960617096, 0.005781794763903551, 0.013907222567054508,
					-0.0006064647012045709, 0.013462176333601512, -0.0038620092683120818,
					0.0011310648800836713, 0.39676801035628384, 0.25120805500666465, 1.1987824643226446},
				{0.016729601365286034, 0.20174175874052286, -0.3512023987303185, 0.09000026750968203,
					-0.0005075333486898592, 0.00025627470764223725, 0.00264092395993363,
					0.0010667617155813596, -0.009070911766706422, 0.023954294087173995, -0.008715148781985652,
					0.015577218795578683, 0.06423960440598632, -0.0063326318447614474, -0.2851310054311361},
				{0.01499651648057232, 0.3285946829937553, -0.4082471342460953, 0.2894851276169713,
					-0.0002866643296303151, -0.00028548863841786065, 0.00027526420275485923,
					-0.0031697888804288355, -0.012764408862194999, 0.020853905266473353, -0.01226148634770958,
					0.09440961418340367, 0.04428878520989377, -0.03929739813925495, -1.193104411884966},
				{0.02210336118343906, -0.029663520539681174, 0.27393283590181855, -0.020780102598160134,
					-0.0003512258463231367, 0.0004323229771602717, -0.004983317367487083,
					-0.0014070909274502604, -0.0002448078970786805, 0.019024115804307306,
					0.0033235885229338227, -0.21782912272174432, -0.009492302525853603, 0.03930566556191734,
					0.7168642213165988},
				{0.004924163236060407, 0.1944805943528036, -0.042271264132364574, 0.3068736914577727,
					-5.290529248030973e-05, 0.00040829021317724534, -0.004366342114132213,
					-0.0012422042332795183, -0.009212169826895102, 0.029455228930340593,
					-0.008407040473427939, -0.08411251004008095, -0.13101765837730414, -0.09297574821657416,
					-0.30805171334865955},
				{-0.005025846530495623, 0.32630416577232635, -0.3154893901207122, 0.31050655102116387,
					0.00010933142096680726, 0.00011228818662774309, 0.0003166480163216993,
					0.0010272741938498855, -0.01316322870944718, 0.02347100433028251, -0.014028955376702438,
					0.02159198878621432, -0.037338967448377426, -0.0919750505983072, -1.0271967376621332},
				{7.655264777358646e-05, 0.40973187565924524, -0.05964978711436491, 0.575196993313054,
					0.00012343418985993893, -0.00031630430250241537, -0.0033467493603101927,
					-0.0021895054869776045, -0.014654361639320877, 0.006785400826179919, -0.0179776457390667,
					0.036176942565134575, -0.17252149588072782, -0.1653678385806257, -1.9129329129663342},
			}};

			array<double, 15> vars;
			double n = this->n;
			double v = this->maxV;
			double d = this->density;
			double p = this->perimeterDensity;
			vars = {n, v, d, p, n * n, n * v, n * d, n * p, v * v, v * d, v * p, d * d, d * p, p * p, 1};

			double bestScore = -INF;
			int bestModel = -1;
			int maxGlobalArms = maxV - 2;
			int maxArmLength = n - 1;
			rep0(i, 6) {
				if (lengths[i][0] > maxArmLength || lengths[i][1] > maxGlobalArms) {
					trace("model: ", lengths[i], ", skipped");
					continue;
				}
				double score = 0;
				rep0(j, 15) {
					score += coeffs[i][j] * vars[j];
				}
				trace("model: ", lengths[i], ", score: ", score);
				if (update_max(bestScore, score)) {
					bestModel = i;
				}
			}
			trace("best model: ", lengths[bestModel]);
			baseLength = lengths[bestModel][0];
			numGlobalArms = min(maxV - 3, lengths[bestModel][1]); // ensure at least two local arms
		}

		// baseLength = PARAM_BASE_LENGTH;
		// numGlobalArms = PARAM_NUM_GLOBAL_ARMS;

		assert(baseLength <= n - 1);
		assert(numGlobalArms <= maxV - 2);

		vector<int> globalLengths;
		vector<int> localLengths;
		rep0(i, numGlobalArms) {
			globalLengths.push_back((baseLength + ((1 << i) >> 1)) >> i);
		}
		int numLocalArms = maxV - 1 - numGlobalArms;
		rep0(i, numLocalArms) {
			localLengths.push_back(i + 1);
		}

		robot.configureArms(globalLengths, localLengths);
		v = maxV;

		trace("global arm lengths: ", globalLengths);
		trace("local arm lengths: ", localLengths);

		// compute global end positions and rotations
		Robot tmpRobot = robot;
		tmpRobot.root = {0, 0};
		rep0(rotBits, 1 << (numGlobalArms * 2)) {
			auto rots = decodeRots(rotBits, numGlobalArms);
			rep0(i, numGlobalArms) {
				tmpRobot.globalArms[i].localRot = rots[i];
			}
			tmpRobot.update();
			auto& globalEnd = tmpRobot.globalArms[numGlobalArms - 1];
			globalEndPos[rotBits] = globalEnd.pos;
			globalEndRot[rotBits] = globalEnd.globalRot;
		}

		field.setupMasks(numLocalArms);
	}

	void drawField(const Field& field) {
		mov.stroke_weight(0.05);
		rep2() {
			mov.stroke(1);
			ivec2 pos = {i, j};
			bool fulfilled = field.isGoal(pos) && field.isPlaced(pos);
			double alphaCoeff = fulfilled ? 0.2 : 1.0;
			if (field.isGoal(pos)) {
				mov.fill(1, 0, 0, 0.5 * alphaCoeff);
			} else {
				mov.no_fill();
			}
			mov.rect(j, i, 1, 1);
			if (field.isPlaced(pos)) {
				mov.no_stroke();
				mov.fill(0.8, 0.4, 0.3, alphaCoeff);
				mov.circle(j + 0.5, i + 0.5, 0.4);
			}
		}
	}

	void drawArms(const Robot& referenceRobot, const RobotState& st) {
		Robot r = referenceRobot;
		r.setState(st);
		mov.stroke(1);
		mov.stroke_weight(0.05);

		// root
		mov.with_tooltip("root", [&]() {
			mov.fill(1);
			mov.circle(r.root.j + 0.5, r.root.i + 0.5, 0.3);
		});
		ivec2 pos = r.root;

		auto drawNextArm = [&](const Arm& a, bool isGlobal) {
			ivec2 pp = pos;
			ivec2 p = a.pos;
			if (isGlobal)
				pos = p;
			mov.stroke_weight(0.1);
			mov.line(pp.j + 0.5, pp.i + 0.5, p.j + 0.5, p.i + 0.5);
			mov.stroke_weight(0.05);
			mov.with_tooltip("i=" + tos(a.index) + " r=" + tos(a.localRot), [&]() {
				if (isGlobal) {
					mov.fill(1);
					mov.circle(p.j + 0.5, p.i + 0.5, 0.3);
				} else {
					mov.fill(0);
					mov.circle(p.j + 0.5, p.i + 0.5, 0.2);
				}
			});
		};

		for (auto& a : r.globalArms) {
			drawNextArm(a, true);
		}
		for (auto& a : r.localArms) {
			drawNextArm(a, false);
		}
	}

	void draw(const Field& field, const Robot& referenceRobot, const RobotState& st) {
		drawField(field);
		drawArms(robot, st);
	}

	array<int, MAX_V - 1> decodeRots(int rotBits, int num) {
		array<int, MAX_V - 1> res;
		rep0(i, num) {
			res[i] = rotBits >> (i * 2) & 3;
		}
		return res;
	}

	int diffRotSign(int targetRot, int currentRot) {
		int dr = (targetRot - currentRot) & 3;
		constexpr int DR[4] = {0, 1, 1, -1};
		return DR[dr];
	}

	RobotMove nextMoveToReach(Robot robot, ivec2 target, int localArmIndex) {
		assert(robot.localArms[localArmIndex].pos != target);
		ivec2 root = robot.root;
		// compute global end positions
		int numGlobalArms = len(robot.globalArms);
		int minDist = INF;
		int bestRotBits = 0;
		ivec2 bestDiff;
		rep0(bits, 1 << (numGlobalArms * 2)) {
			ivec2 endPos = root + globalEndPos[bits];
			int endRot = globalEndRot[bits];
			rep0(localRot, 4) {
				int rot = (endRot + localRot) & 3;
				ivec2 leafPos = endPos + robot.localArms[localArmIndex].rotToDiff[rot];
				ivec2 diff = target - leafPos;
				if (!(root + diff).in_bounds(n))
					continue; // actually not reachable
				int dist = diff.norm();
				if (update_min(minDist, dist)) {
					bestRotBits = bits;
					bestDiff = diff;
				}
			}
		}
		assert(minDist != INF);

		RobotMove res;

		if (bestDiff != ivec2{}) {
			ivec2 absDiff = bestDiff.abs();
			ivec2 shift;
			if (absDiff.i > absDiff.j) {
				shift.i = bestDiff.i > 0 ? 1 : -1;
			} else {
				shift.j = bestDiff.j > 0 ? 1 : -1;
			}
			res.dir = shift.dir_index();
		}
		auto targetRots = decodeRots(bestRotBits, numGlobalArms);
		bool anyMove = res.dir != -1;
		for (auto& a : robot.globalArms) {
			int drot = diffRotSign(targetRots[a.index - 1], a.localRot);
			res.setDrot(a.index, drot);
			if (drot != 0)
				anyMove = true;
		}
		// assert(anyMove); // TODO: debug
		return res;
	}

	double evaluate(const Field& field, bool dump = false) {
		string st;
		// auto& field = st.field;
		int leftTodo = field.goals.size() + field.lefts.size();
		double score = -field.todoCost();

		double stddevCoeff = 0.08;
		double stddev = sqrt(field.computeVariance());
		score -= stddev * stddevCoeff;

		double meanDiffCoeff = 0.01;
		double meanDiff = field.computeMeanDiff();
		score -= meanDiff * meanDiffCoeff;

		// int maxDistFromRoot = 0;
		// ivec2 root = st.robot.root;
		// for (int p : field.goals) {
		// 	ivec2 pos = unpack[p];
		// 	int dist = (pos - root).norm();
		// 	update_max(maxDistFromRoot, dist);
		// }
		// for (int p : field.lefts) {
		// 	ivec2 pos = unpack[p];
		// 	int dist = (pos - root).norm();
		// 	update_max(maxDistFromRoot, dist);
		// }
		// double maxDistCoeff = 0.5;
		// int globalReach = 0;
		// for (auto& a : st.robot.globalArms) {
		// 	globalReach += a.length;
		// }
		// score -= max(0, maxDistFromRoot - globalReach) * maxDistCoeff;

		if (leftTodo > 2) {
			int maxGoodPairs = leftTodo * (leftTodo - 1);

			int numGoodPairs = field.numGoodPairs;
			double goodness = numGoodPairs / (double) maxGoodPairs;
			double goodnessCoeff = leftTodo;
			score += goodness * goodnessCoeff;
		}

		return score;
	}

	void applyMove(State& st, TurnMove tm) {
		// if (rendering) {
		// 	draw(st.field, st.robot, st.robot.getState());
		// 	mov.comment("before move");
		// 	mov.end_frame();
		// }

		if (st.moves.empty()) {
			ivec2 pos = unpack[tm.move.dir];
			st.robot.root = pos;
			tm.move.dir = -1;
		}
		st.robot.move(tm.move);

		for (auto& a : st.robot.localArms) {
			// trace("local arm: ", a.index, " ", a.pos);
			ivec2 pos = a.pos;
			bool pick = tm.getPick(a.index);
			if (pick) {
				int idx = pos.pack(n);
				if (st.hold[a.index]) {
					// trace("drop: ", pos, " ", a.index);
					st.field.drop(pos, idx);
				} else {
					// trace("pick: ", pos, " ", a.index);
					st.field.pick(pos, idx);
				}
				st.hold[a.index] ^= 1;
			}
		}

		// if (rendering) {
		// 	trace("move: ", tm);
		// 	draw(st.field, st.robot, st.robot.getState());
		// 	mov.comment("after move");
		// 	mov.end_frame();
		// }

		st.moves.push_back(tm);
	};

	void undoMove(State& st) {
		auto tm = st.moves.pop_back();
		// trace("undo: ", tm);

		for (auto& a : st.robot.localArms) {
			ivec2 pos = a.pos;
			bool pick = tm.getPick(a.index);
			if (pick) {
				int idx = pos.pack(n);
				if (st.hold[a.index]) {
					st.field.drop(pos, idx);
				} else {
					st.field.pick(pos, idx);
				}
				st.hold[a.index] ^= 1;
			}
		}

		st.robot.undoMove(tm.move);
	};

	bool bs(int maxTime) {

		initField();
		robot.init();

		State st;
		st.robot = robot;
		st.field = field;

		int numGlobalArms = len(st.robot.globalArms);
		int maxRotBits3 = (int) pow(3, numGlobalArms);

		// zobrist hashing
		ull rootPosHashMap[MAX_N2];
		ull rotHashMap[MAX_V][4];
		ull holdHashMap[MAX_V];
		ull goalHashMap[MAX_N2];
		ull leftHashMap[MAX_N2];
		rep2() {
			int idx = ivec2(i, j).pack(n);
			rootPosHashMap[idx] = rng.next_ull();
			goalHashMap[idx] = rng.next_ull();
			leftHashMap[idx] = rng.next_ull();
		}
		rep0(i, v) {
			holdHashMap[i] = rng.next_ull();
			rep0(r, 4) {
				rotHashMap[i][r] = rng.next_ull();
			}
		}

		if (render)
			mov.target("main");

		auto getScore = [&](const State& st) {
			return m - st.field.goals.size();
		};

		auto computeHash = [&](const State& st) {
			// TODO: optimize
			ull hash = 0;
			hash ^= rootPosHashMap[st.robot.root.pack(n)];
			rep0(i, v) {
				hash ^= holdHashMap[i] * st.hold[i];
			}
			for (auto& a : st.robot.globalArms) {
				hash ^= rotHashMap[a.index][a.localRot];
			}
			for (auto& a : st.robot.localArms) {
				hash ^= rotHashMap[a.index][a.localRot];
			}
			for (int i : st.field.goals) {
				hash ^= goalHashMap[i];
			}
			for (int i : st.field.lefts) {
				hash ^= leftHashMap[i];
			}
			return hash;
		};

		// mov.target("main");

		constexpr TurnMove RESERVED = {{}, -1};
		beam_search<State, int, TurnMove, RESERVED, double, greater<>, 22> bs;
		bs.max_turn = 1500;
		bool found = false;

		beam_width_manager bwm(64);
		bwm.window_size = 2;

		int initialDeviation = field.goals.size() + field.lefts.size();
		double progress = 0;

		int numTurns = 0;

		int snapshotTurn = -1;
		int startTime = timer();

		auto moves = bs.run_tree(
			st, getScore,
			[&](State& st, const TurnMove& tm) {
				applyMove(st, tm);
			},
			[&](State& st) {
				undoMove(st);
			},
			[&](State& st) {
				int deviation = st.field.goals.size() + st.field.lefts.size();
				progress = max(progress, 1.0 - deviation / (double) initialDeviation);

				if (found || st.field.goals.empty()) {
					if (!found) {
						trace("found");
						found = true;
					}
					return;
				}

				if (numTurns + 1 == snapshotTurn && render) {
					ull fieldHash = 0;
					for (int i : st.field.goals) {
						fieldHash ^= goalHashMap[i];
					}
					for (int i : st.field.lefts) {
						fieldHash ^= leftHashMap[i];
					}
					draw(st.field, st.robot, st.robot.getState());
					int leftTodo = st.field.numTodos;
					int maxGoodPairs = leftTodo * (leftTodo - 1);
					int numGoodPairs = st.field.numGoodPairs;
					double goodness = numGoodPairs / (double) maxGoodPairs;
					mov.comment("turn: " + tos(numTurns) + " goodness: " + tos(goodness * 100) + "%");
					mov.end_frame();
				}

				bool firstTurn = st.moves.empty();

				bool canRot4[MAX_V] = {};
				if (!firstTurn) {
					auto& lastMove = st.moves.data.back();
					for (auto& a : robot.localArms) {
						if (!lastMove.getPick(a.index)) {
							canRot4[a.index] = true;
						}
					}
				}

				static vector<tuple<TurnMove, double>> moves;
				moves.clear();
				auto& robot = st.robot;
				auto& field = st.field;
				auto& hold = st.hold;

				int combinationThreshold =
					(int) round(lerp(1, 64, pow(linearstep(100, 10, field.numTodos), 3.0)));

				bool isEmptyGoal[MAX_N2] = {};
				bool isWronglyPlaced[MAX_N2] = {};

				for (int p : field.goals) {
					isEmptyGoal[p] = true;
				}
				for (int p : field.lefts) {
					isWronglyPlaced[p] = true;
				}

				ivec2 baseRoot = robot.root;

				bool armFull = true;
				bool armEmpty = true;
				for (auto& a : robot.localArms) {
					if (hold[a.index]) {
						armEmpty = false;
					} else {
						armFull = false;
					}
				}

				bool caughtAny = false;

				RobotState baseRst = robot.getState();
				rep0(rotBits3, maxRotBits3) {
					TurnMove tm;
					int denom = 1;
					int rotBits = 0;
					int endRot = 0;
					rep0(i, numGlobalArms) {
						int dr = (rotBits3 / denom) % 3 - 1;
						int localRot = (baseRst.localRots[i] + dr) & 3;
						tm.move.setDrot(i + 1, dr);
						endRot = (endRot + localRot) & 3;
						rotBits |= localRot << (i * 2);
						denom *= 3;
					}
					int dirNum = firstTurn ? n * n : 5;
					rep0(dirIndex, dirNum) {
						int dir;
						ivec2 newRoot;
						if (firstTurn) {
							dir = dirIndex;
							newRoot = unpack[dir];
						} else {
							dir = dirIndex - 1;
							newRoot = baseRoot;
							if (dir != -1) {
								newRoot += ivec2::dir(dir);
								if (!newRoot.in_bounds(n))
									continue;
							}
						}
						tm.move.dir = dir;
						ivec2 endPos = newRoot + globalEndPos[rotBits];

						ivec2 endPosExtended = endPos + MAX_V;
						int reachIndex = endPosExtended.pack_if_in_bounds(n + MAX_V * 2);
						if (reachIndex == -1)
							continue;

						auto todoWords = field.todoBits.words();
						auto reachMaskWords = field.reachMasks[reachIndex].words();
						bool mayReach = false;
						rep0(k, numWords) {
							if (todoWords[k] & reachMaskWords[k]) {
								mayReach = true;
								break;
							}
						}

						if (!mayReach)
							continue;

						int caughtCounts[MAX_V];
						int caughtDirs[MAX_V][4];
						int combination = 1;
						static vector<int> caughtArmIndices;
						caughtArmIndices.clear();
						tm.pickBits = 0;
						int pickedIndex[MAX_V];
						clear_with(pickedIndex, -1);

						int numLocalArms = len(robot.localArms);
						rep0(localArmIndex, numLocalArms) {
							auto& a = robot.localArms[localArmIndex];
							int i = a.index;
							tm.move.setDrot(i, 0);
							caughtCounts[i] = 0;
							rep(dr, -1, 3) {
								if (!canRot4[i] && dr == 2)
									continue;
								int r = (endRot + a.localRot + dr) & 3;
								ivec2 pos = endPos + a.rotToDiff[r];
								int idx = pos.pack_if_in_bounds(n);
								if (idx == -1)
									continue;
								if (hold[i] && isEmptyGoal[idx] || !hold[i] && isWronglyPlaced[idx]) {
									caughtDirs[i][caughtCounts[i]++] = dr;
								}
							}
							if (caughtCounts[i] > 0) {
								tm.setPick(i, true);
								caughtArmIndices.push_back(localArmIndex);
								combination *= caughtCounts[i];
							}
						}

						if (caughtArmIndices.empty())
							continue;

						caughtAny = true;
						if (combination < combinationThreshold) {
							// brute force

							rep0(s, combination) {
								int ss = s;
								for (int localArmIndex : caughtArmIndices) {
									auto& a = robot.localArms[localArmIndex];
									int i = a.index;
									int dr = caughtDirs[i][ss % caughtCounts[i]];
									ss /= caughtCounts[i];
									int r = (endRot + a.localRot + dr) & 3;
									ivec2 pos = endPos + a.rotToDiff[r];
									int idx = pos.pack(n);

									int oldIdx = pickedIndex[i];
									if (oldIdx != idx) {
										if (oldIdx != -1) {
											field.pickOrDrop(unpack[oldIdx], oldIdx);
										}
										field.pickOrDrop(pos, idx);
										pickedIndex[i] = idx;
									}

									tm.move.setDrot(i, dr);
								}
								// applyMove(st, tm);
								double score = evaluate(field);
								// undoMove(st);
								moves.emplace_back(tm, score);
							}
						} else {
							// too large, greedily one by one
						    // trace("large combination: ", combination);

							for (int localArmIndex : caughtArmIndices) {
								auto& a = robot.localArms[localArmIndex];
								int i = a.index;
								int bestDr = 0;
								double bestScore = -INF;
								int bestIdx = -1;
								rep0(ri, caughtCounts[i]) {
									int dr = caughtDirs[i][ri];
									int r = (endRot + a.localRot + dr) & 3;
									ivec2 pos = endPos + a.rotToDiff[r];
									int idx = pos.pack(n);
									tm.move.setDrot(i, dr);

									field.pickOrDrop(pos, idx);
									double score = evaluate(field);
									if (update_max(bestScore, score)) {
										bestDr = dr;
										bestIdx = idx;
									}
									field.pickOrDrop(pos, idx);
								}
								assert(bestScore != -INF);

								pickedIndex[i] = bestIdx;
								field.pickOrDrop(unpack[bestIdx], bestIdx);

								tm.move.setDrot(i, bestDr);
							}
							double finalScore = evaluate(field);
							moves.emplace_back(tm, finalScore);
						}
						// revert field
						for (int localArmIndex : caughtArmIndices) {
							auto& a = robot.localArms[localArmIndex];
							int i = a.index;
							if (pickedIndex[i] != -1) {
								field.pickOrDrop(unpack[pickedIndex[i]], pickedIndex[i]);
							}
						}
					}
				}

				if (!caughtAny) {
					// trace("no moves. try direct search");
					double maxPriority = -INF;
					int target = -1;
					int localArmIndex = -1;
					double invNum = 1.0 / (double) field.numTodos;
					double meanI = field.sumI * invNum;
					double meanJ = field.sumJ * invNum;

					meanI = robot.root.i;
					meanJ = robot.root.j;

					array<double, MAX_N2> priorityMap;
					rep2() {
						ivec2 pos = {i, j};
						int idx = pos.pack(n);
						int dist = abs(i - meanI) + abs(j - meanJ);
						priorityMap[idx] = -dist * 0.1; // try to find the closest point
					}

					if (armEmpty) {
						// pick up one with the highest priority
						for (int left : field.lefts) {
							double priority = priorityMap[left];
							if (update_max(maxPriority, priority)) {
								target = left;
							}
						}
						localArmIndex = 0;
						// trace("pick up: ", unpack[target], ", leaf: ", leaf);
					} else {
						// drop one with the highest priority
						double maxPriority = -INF;
						target = -1;
						for (int goal : field.goals) {
							double priority = priorityMap[goal];
							if (update_max(maxPriority, priority)) {
								target = goal;
							}
						}
						rep0(i, len(robot.localArms)) {
							if (hold[robot.localArms[i].index]) {
								localArmIndex = i;
								break;
							}
						}
						// trace("drop to: ", unpack[target], ", leaf: ", leaf);
					}
					assert(target != -1);
					assert(localArmIndex != -1);
					// trace("holds: ", hold);
					auto rm = nextMoveToReach(robot, unpack[target], localArmIndex);
					assert(moves.empty());

					applyMove(st, {rm, {}});
					double finalScore = evaluate(field);
					undoMove(st);

					moves.emplace_back(rm, finalScore);
				}

				int maxMoves = max(16, 512 >> min(numTurns, 16));
				if (len(moves) > maxMoves) {
					nth_element(moves.begin(), moves.begin() + maxMoves, moves.end(),
						[&](const auto& a, const auto& b) {
							return get<1>(a) > get<1>(b);
						});
					moves.resize(maxMoves);
				}

				for (auto& [tm, score] : moves) {
					applyMove(st, tm);
					// double score2 = evaluate(field);
					ull hash = computeHash(st);
					undoMove(st);
					// assert(abs(score - score2) < 1e-6);
					bs.add_candidate(tm, score, hash);
				}
			},
			[&](int turn) {
				numTurns = turn;

				int width = bwm.next(pow(progress, 1.5), timer(), maxTime);
				auto& cands = bs.candidates_to_filter();

				if (found) {
					cands.clear();
					return;
				}

				auto dir = [&](const auto& a, const auto& b) {
					return a.data > b.data;
				};

				int plen = len(cands);
				bs.remove_duplicates(dir);
				int plen2 = len(cands);
				if (len(cands) > width) {
					nth_element(cands.begin(), cands.begin() + width, cands.end(), dir);
					cands.resize(width);
				}

				trace("turn: ", turn + 1, " cands: ", plen, " -> ", plen2, " -> ", len(cands));

				bwm.report(len(cands));
			});

		trace("result: ", len(moves));
		sol = moves;

		// if (!fast) {
		// 	rendering = render;
		// 	if (rendering) {
		// 		State tmpSt = st;
		// 		for (auto& tm : sol) {
		// 			applyMove(tmpSt, tm);
		// 		}
		// 	}
		// }

		// fix 180 degree rotations
		trace("fixing 180 degree rotations...");
		rep(i, 1, len(sol)) {
			auto& pmove = sol[i - 1].move;
			auto& move = sol[i].move;
			for (auto& a : robot.localArms) {
				int j = a.index;
				if (move.getDrot(j) == 2) {
					trace("  turn: ", i, ", vertex: ", j);
					move.setDrot(j, 1);
					pmove.setDrot(j, 1);
				}
			}
		}

		return found;
	}

	void solveMain(ostream& out) { // write answer to out
		trace("N: ", n, " M: ", m, " V: ", maxV);
		configureRobot();
		int numTrials = 1;
		int bestScore = INF;
		bool solved = bs(2800);

		// write robot
		int numGlobalArms = len(robot.globalArms);
		out << 1 + numGlobalArms + len(robot.localArms) << endl;
		for (auto& a : robot.globalArms) {
			out << a.index - 1 << " " << a.length << endl;
		}
		for (auto& a : robot.localArms) {
			out << numGlobalArms << " " << a.length << endl;
		}
		ivec2 initialPos = unpack[sol[0].move.dir];
		out << initialPos.i << " " << initialPos.j << endl;

		// write moves
		int numMoves = len(sol);
		rep0(i, numMoves) {
			auto& tm = sol[i];
			if (i == 0) {
				out << "."; // initial pos is set by this data, do not write it actually
			} else {
				out << ".UDLR"[tm.move.dir + 1];
			}
			rep(i, 1, v) {
				out << ".R?L"[tm.move.getDrot(i)];
			}
			rep0(i, v) {
				out << ".P"[tm.getPick(i)];
			}
			out << endl;
		}

		result.score = solved ? len(sol) : 0;
	}
};

int main(int argc, char* argv[]) {
#if 0 || ONLINE_JUDGE
	isLocal = false;
	render = false;
	timer(true);
	Solver sol;
	sol.load(cin);
	sol.solve();
#elif 0
	// for masters
	makeMovie(argc, argv);
#elif 0
	// write coeffs
	ostringstream oss;
	rep0(seed, 5000) {
		Solver sol;
		sol.load(seed);
		double n = sol.n;
		double v = sol.maxV;
		double d = sol.density;
		double p = sol.perimeterDensity;
		oss << n << " " << v << " " << d << " " << p << endl;
	}
	write_text("nvdp.txt", oss.str());
#elif 0
	// write metadata
	ostringstream oss;
	oss << "seed n m v dens peri" << endl;
	rep0(seed, 5000) {
		Solver sol;
		sol.load(seed);
		int mApprox = (int) pow(round(sqrt(sol.m)), 2);
		oss << seed << " " << sol.n << " " << mApprox << " " << sol.maxV << " "
			<< (int) (round(sol.density * 20)) << " " << (int) (round(sol.perimeterDensity * 20)) << endl;
	}
	write_text("scores/input.txt", oss.str());
#elif 1
	// for local/remote testers
	debug = false;
	render = false;
	int seed;
	cin >> seed;
	cin >> time_scale;

	// cin >> PARAM_BASE_LENGTH;
	// cin >> PARAM_NUM_GLOBAL_ARMS;

	timer(true);
	Solver sol;
	sol.load(seed);
	sol.solve();
	cout << sol.result.score << " " << timer() << endl;
#elif 1
	// single-threaded test, handy but slow
	int num = 10;
	int from = 0;
	int stride = 1;
	int single = -1;

	vector<int> seedList = {57, 316, 379};

	debug = true;
	render = false;

	struct TestCase {
		int seed;
		int time;
		ll score;
	};
	vector<TestCase> cases;

	if (seedList.empty() || single != -1) {
		seedList.clear();
		if (single == -1) {
			rep0(t, num) {
				seedList.push_back(from + t * stride);
			}
		} else {
			seedList.push_back(single);
		}
	}

	bool doTrace = debug;
	debug = true;
	for (int seed : seedList) {
		timer(true);
		trace("------------ SOLVING SEED ", seed, " ------------");
		debug = doTrace;
		Solver s;
		s.load(seed);
		s.solve();
		debug = true;
		int time = timer();
		trace("score: ", s.result.score, " (time ", time, " ms)\n");
		if (s.result.score != -1)
			cases.emplace_back(seed, time, s.result.score);
	}

	auto print = [&](const TestCase& c) {
		int seed = c.seed;
		string space = seed < 10 ? "   " : seed < 100 ? "  " : seed < 1000 ? " " : "";
		trace("  seed ", space, seed, ": ", c.score, " (time ", c.time, " ms)");
	};

	if (len(cases) > 1) {
		trace("------------ summary ------------");

		trace("sort by score:");
		sort(cases.begin(), cases.end(), [&](auto a, auto b) {
			return a.score > b.score;
		});
		for (auto& c : cases)
			print(c);

		trace("sort by seed:");
		sort(cases.begin(), cases.end(), [&](auto a, auto b) {
			return a.seed < b.seed;
		});
		for (auto& c : cases)
			print(c);

		ll scoreSum = 0;
		double logScoreSum = 0;
		for (auto& c : cases) {
			scoreSum += c.score;
			logScoreSum += log(c.score);
		}
		double invDenom = 1.0 / len(cases);
		trace("total score: ", scoreSum, ", mean: ", (ll) (scoreSum * invDenom * 100 + 0.5) / 100.0,
			", mean(log2): ", (ll) (logScoreSum * invDenom * 1000 + 0.5) / 1000.0);
	}
#endif
}
