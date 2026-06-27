
#include <sys/types.h>
#include <type_traits>
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
#include <bit>
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
#include <initializer_list>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <numbers>
#include <numeric>
#include <optional>
#include <ostream>
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

// note: a huge library begins, the main part starts around line 2950

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

// to overload macros define foo_N for each N and then use SHR_OVERLOAD(foo, __VA_ARGS__)
#define SHR_VA_COUNT_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, count, ...) count
#define SHR_VA_COUNT(...) SHR_VA_COUNT_IMPL(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define SHR_CAT(macro, count) macro##count
#define SHR_OVERLOAD_IMPL(macro, ...) SHR_CAT(macro##_, __VA_ARGS__)
#define SHR_OVERLOAD(macro, ...) SHR_OVERLOAD_IMPL(macro, SHR_VA_COUNT(__VA_ARGS__))(__VA_ARGS__)

		template <class Abort>
		struct bombstream {
			string cond;
			string note;
			const char* file;
			int line;
			ostringstream os;
			Abort abort;

			bombstream(const string& cond, const string& note, const char* file, int line, Abort abort)
				: cond(cond), note(note), file(file), line(line), abort(abort) {
			}

			~bombstream() {
				cerr << "FAIL: " << cond << " is false" << endl;
				if (!note.empty())
					cerr << "    " << note << endl;
				if (string msg = os.str(); !msg.empty())
					cerr << "    " << msg << endl;
				cerr << "    at " << file << ":" << line << endl;
				abort();
			}

			template <class T>
			bombstream& operator<<(T a) {
				os << a;
				return *this;
			}
		};

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

		constexpr int signum(int a) {
			return (a > 0) - (a < 0);
		}

		constexpr int signum(ll a) {
			return (a > 0) - (a < 0);
		}

		constexpr int signum(double a) {
			return (a > 0) - (a < 0);
		}

		constexpr double linearstep(double edge0, double edge1, double t) {
			return clamp((t - edge0) / (edge1 - edge0), 0.0, 1.0);
		}

		constexpr double linearmap(double value0, double value1, double edge0, double edge1, double t) {
			return value0 + (value1 - value0) * linearstep(edge0, edge1, t);
		}

		constexpr int ilinearmap(int value0, int value1, double edge0, double edge1, double t) {
			int sign = signum(value1 - value0);
			return clamp((int) round(linearmap(value0 - sign * 0.5, value1 + sign * 0.5, edge0, edge1, t)),
				value0, value1);
		}

		constexpr double smoothstep(double edge0, double edge1, double t) {
			t = linearstep(edge0, edge1, t);
			return t * t * (3 - 2 * t);
		}

		double exp_interp(double from, double to, double t) {
			return pow(from, 1 - t) * pow(to, t);
		}
	} // namespace basic
	using namespace basic;

	namespace tos_internal {
		struct tos_option {
			optional<string> separator = nullopt;
			optional<int> break_every = nullopt;
			optional<bool> asint = nullopt;

			template <class T>
			static optional<T> merge(optional<T> a, optional<T> b) {
				assert(!a.has_value() || !b.has_value());
				return a.has_value() ? a : b;
			}

			tos_option operator|(const tos_option& o) const {
				tos_option res;
				res.separator = merge(separator, o.separator);
				res.break_every = merge(break_every, o.break_every);
				res.asint = merge(asint, o.asint);
				return res;
			}
		};

		template <class T>
		struct tos_value {
			using value_type = T;
			const T& value;
			tos_option option;
		};

		struct tos_modifier {
			tos_option option;

			template <class T>
			friend tos_value<T> operator|(const T& v, const tos_modifier& o) {
				return {v, o.option};
			}

			template <class T, int N>
			friend tos_value<T[N]> operator|(const T (&v)[N], const tos_modifier& o) {
				return tos_value{v, o.option};
			}

			template <class T>
			friend tos_value<T> operator|(const tos_value<T>& v, const tos_modifier& o) {
				return {v.value, v.option | o.option};
			}
		};

		template <class T>
		concept printable = requires(T t, ostream& out) { out << t; };

		template <class T>
		concept has_str = requires(T t) {
			{ t.str() } -> convertible_to<string>;
		};

		template <class T>
		concept is_tos_value = is_same_v<T, tos_value<typename T::value_type>>;

		struct printer {
			static string tos(const char* s) {
				return string(s);
			}

			template <class T>
			static string tos(const tos_value<T>& t) {
				auto& v = t.value;
				auto& o = t.option;
				return to_string(v);
			}

			template <printable T>
			static string tos(const tos_value<T>& t) {
				auto& v = t.value;
				auto& o = t.option;
				bool asint = o.asint.value_or(false);
				ostringstream os;
				if constexpr (is_same_v<decay_t<T>, char>) {
					if (asint) {
						os << (int) v;
					} else {
						os << v;
					}
				} else {
					os << v;
				}
				return os.str();
			}

			template <class T>
			requires(has_str<T> && !printable<T>)
			static string tos(const tos_value<T>& t) {
				auto& v = t.value;
				auto& o = t.option;
				return v.str();
			}

			template <class T, class U>
			static string tos(const tos_value<pair<T, U>>& t) {
				auto& v = t.value;
				auto& o = t.option;
				return "(" + tos(v.first) + ", " + tos(v.second) + ")";
			}

			template <class... Ts>
			static string tos(const tos_value<tuple<Ts...>>& t) {
				auto& v = t.value;
				auto& o = t.option;
				vector<string> strs;
				apply(
					[&strs](const auto&... args) {
						((strs.push_back(tos(args)), ...));
					},
					v);
				return "(" + join(strs, o) + ")";
			}

			template <class T>
			requires(!printable<T>)
			static string tos(const tos_value<T>& t) {
				auto& v = t.value;
				auto& o = t.option;
				bool asint = o.asint.value_or(false);
				auto from = v.begin();
				auto until = v.end();
				vector<string> strs;
				while (from != until) {
					if constexpr (is_same_v<decay_t<decltype(*from)>, char>) {
						strs.push_back(asint ? tos((int) *from) : tos(*from));
					} else {
						strs.push_back(tos(*from));
					}
					from++;
				}
				return "{" + join(strs, o) + "}";
			}

			template <class T, int N>
			static string tos(const tos_value<T[N]>& t) {
				auto& v = t.value;
				auto& o = t.option;
				bool asint = o.asint.value_or(false);
				vector<string> strs;
				for (int i = 0; i < N; i++) {
					if constexpr (is_same_v<decay_t<T>, char>) {
						strs.push_back(asint ? tos((int) v[i]) : tos(v[i]));
					} else {
						strs.push_back(tos(v[i]));
					}
				}
				return "{" + join(strs, o) + "}";
			}

			template <class T>
			requires(!is_tos_value<T>)
			static string tos(const T& t) {
				return tos(t | tos_modifier{});
			}

		private:
			static string join(vector<string>& strs, tos_option o) {
				ostringstream os;
				string sep = o.separator.value_or(", ");
				bool do_break = o.break_every.has_value() && len(strs) > o.break_every.value();
				if (do_break) {
					int rows = o.break_every.value();
					// pad with spaces
					if (rows > 1) {
						for (int i = 0; i < rows; i++) {
							int max_len = 0;
							for (int j = i; j < len(strs); j += rows) {
								max_len = max(max_len, len(strs[j]));
							}
							for (int j = i; j < len(strs); j += rows) {
								strs[j] = string(max_len - len(strs[j]), ' ') + strs[j];
							}
						}
					}
					os << endl;
					for (int i = 0; i < len(strs); i++) {
						if (i % rows == 0)
							os << "  ";
						os << strs[i];
						os << sep;
						if (i % rows == rows - 1 || i == len(strs) - 1)
							os << endl;
					}
				} else {
					for (int i = 0; i < len(strs); i++) {
						if (i > 0) {
							os << sep;
						}
						os << strs[i];
					}
				}
				return os.str();
			}
		};

	} // namespace tos_internal
	using namespace tos_internal;

	namespace tos_impl {
		namespace mods {
			tos_modifier sep(const string& s) {
				tos_option o;
				o.separator = s;
				return {o};
			}

			tos_modifier br(int n) {
				tos_option o;
				o.break_every = n;
				return {o};
			}

			tos_modifier asint() {
				tos_option o;
				o.asint = true;
				return {o};
			}
		} // namespace mods

		template <class T>
		string tos(const T& t) {
			return printer::tos(t);
		}

		namespace lambda {
			template <ranges::range Range, class F>
			auto map(const Range& as, F f) {
				using T = decltype(f(*as.begin()));
				vector<T> res;
				for (auto& a : as) {
					res.push_back(f(a));
				}
				return res;
			}

			template <ranges::range Range>
			string join(const Range& as, string sep) {
				ostringstream os;
				bool first = true;
				for (auto& a : as) {
					if (first)
						first = false;
					else
						os << sep;
					os << tos(a);
				}
				return os.str();
			}

			template <ranges::range Range>
			auto zip(const Range& as, const Range& bs) {
				using A = ranges::range_value_t<Range>;
				using B = ranges::range_value_t<Range>;
				vector<pair<A, B>> res;
				auto a = as.begin();
				auto b = bs.begin();
				while (a != as.end() && b != bs.end()) {
					res.emplace_back(*a++, *b++);
				}
				return res;
			}

			template <ranges::range RangeA, ranges::range RangeB, class F>
			auto zipWith(const RangeA& as, const RangeB& bs, F f) {
				using T = decltype(f(*as.begin(), *bs.begin()));
				vector<T> res;
				auto a = as.begin();
				auto b = bs.begin();
				while (a != as.end() && b != bs.end()) {
					res.push_back(f(*a++, *b++));
				}
				return res;
			}
		} // namespace lambda

// handy debug macros
#define SHR_SHOW_BASE(...)                                        \
	([](const vector<string>& keys, const vector<string>& vals) { \
		return lambda::join(lambda::zipWith(keys, vals,           \
								[](auto& a, auto& b) {            \
									return a + " = " + b;         \
								}),                               \
			", ");                                                \
	})(__VA_ARGS__)
#define SHR_SHOW_1(a) SHR_SHOW_BASE({#a}, {tos(a)})
#define SHR_SHOW_2(a, b) SHR_SHOW_BASE({#a, #b}, {tos(a), tos(b)})
#define SHR_SHOW_3(a, b, c) SHR_SHOW_BASE({#a, #b, #c}, {tos(a), tos(b), tos(c)})
#define SHR_SHOW_4(a, b, c, d) SHR_SHOW_BASE({#a, #b, #c, #d}, {tos(a), tos(b), tos(c), tos(d)})
#define SHR_SHOW_5(a, b, c, d, e) \
	SHR_SHOW_BASE({#a, #b, #c, #d, #e}, {tos(a), tos(b), tos(c), tos(d), tos(e)})
#define SHR_SHOW_6(a, b, c, d, e, f) \
	SHR_SHOW_BASE({#a, #b, #c, #d, #e, #f}, {tos(a), tos(b), tos(c), tos(d), tos(e), tos(f)})
#define SHR_SHOW_7(a, b, c, d, e, f, g) \
	SHR_SHOW_BASE({#a, #b, #c, #d, #e, #f, #g}, {tos(a), tos(b), tos(c), tos(d), tos(e), tos(f), tos(g)})
#define SHR_SHOW_8(a, b, c, d, e, f, g, h) \
	SHR_SHOW_BASE(                         \
		{#a, #b, #c, #d, #e, #f, #g, #h}, {tos(a), tos(b), tos(c), tos(d), tos(e), tos(f), tos(g), tos(h)})
#define SHR_SHOW_9(a, b, c, d, e, f, g, h, i)           \
	SHR_SHOW_BASE({#a, #b, #c, #d, #e, #f, #g, #h, #i}, \
		{tos(a), tos(b), tos(c), tos(d), tos(e), tos(f), tos(g), tos(h), tos(i)})
#define show(...) SHR_OVERLOAD(SHR_SHOW, __VA_ARGS__)

// fancy assert macros
#ifdef NDEBUG
#undef assert
#define assert(...) \
	if (false)      \
	cerr
#define assert_op(a, b) assert()
#define assert_eq(a, b) assert()
#define assert_ne(a, b) assert()
#define assert_lt(a, b) assert()
#define assert_le(a, b) assert()
#define assert_gt(a, b) assert()
#define assert_ge(a, b) assert()
#define assert_in(a, lo_incl, hi_excl) assert()
#define fail(...) assert()
#else
#undef assert
#define SHR_ABORT \
	[]() {        \
		abort();  \
	}
#define assert(cond, ...) \
	if (!(cond))          \
	bombstream(#cond, "" __VA_OPT__(+show(__VA_ARGS__)), __FILE__, __LINE__, SHR_ABORT)
#define assert_op(a, op, b)                              \
	if (!((a) op(b)))                                    \
	bombstream("("s + #a + ") " + #op + " (" + #b + ")", \
		"i.e. " + tos(a) + " " + #op + " " + tos(b) + " doesn't hold", __FILE__, __LINE__, SHR_ABORT)
#define assert_in(a, lo_incl, hi_excl)                                                               \
	if (!((a) >= (lo_incl) && (a) < (hi_excl)))                                                      \
	bombstream("("s + #lo_incl + ") <= (" + #a + ") < (" + #hi_excl + ")",                           \
		"i.e. " + tos(lo_incl) + " <= " + tos(a) + " < " + tos(hi_excl) + " doesn't hold", __FILE__, \
		__LINE__, SHR_ABORT)
#define assert_eq(a, b) assert_op(a, ==, b)
#define assert_ne(a, b) assert_op(a, !=, b)
#define assert_lt(a, b) assert_op(a, <, b)
#define assert_le(a, b) assert_op(a, <=, b)
#define assert_gt(a, b) assert_op(a, >, b)
#define assert_ge(a, b) assert_op(a, >=, b)
#define fail(...) assert(false, __VA_ARGS__)
#endif
	} // namespace tos_impl
	using namespace tos_impl;

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

		template <class... Rest>
		void tracen(const Rest&... rest) {
			if (!debug)
				return;
			((cerr << tos(rest)), ...);
		}

		template <class... Rest>
		void trace(const Rest&... rest) {
			if (!debug)
				return;
			((cerr << tos(rest)), ...);
			cerr << endl;
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
				assert_ge(mod, 0);
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
			assert_ge(n, 0);
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

			fast_vector(const fast_vector& other) {
				num = other.num;
				memcpy(data, other.data, sizeof(T) * num);
			}

			fast_vector& operator=(const fast_vector& other) {
				num = other.num;
				memcpy(data, other.data, sizeof(T) * num);
				return *this;
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

			void push_back(T a) {
				assert_lt(num, N);
				data[num++] = a;
			}

			template <class... Args>
			T& emplace_back(Args&&... args) {
				push_back(T(std::forward<Args>(args)...));
				return data[num - 1];
			}

			void erase(iterator where) {
				assert_in(where, begin(), end());
				*where = data[--num];
			}

			void pop_back() {
				assert_gt(num, 0);
				num--;
			}

			void clear() {
				num = 0;
			}

			T& operator[](int i) {
				assert_in(i, 0, num);
				return data[i];
			}

			const T& operator[](int i) const {
				assert_in(i, 0, num);
				return data[i];
			}

			int size() const {
				return num;
			}

			bool empty() const {
				return num == 0;
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
				assert_in(i, 0, num_total);
				return packs[i >> UnitBits][i & UNIT_MASK];
			}

			const T& operator[](int i) const {
				assert_in(i, 0, num_total);
				return packs[i >> UnitBits][i & UNIT_MASK];
			}

			int size() const {
				return num_total;
			}
		};

		// random access: O(1), unordered
		// insert: O(1)
		// erase: O(1)
		// clear: O(1)
		// contains: O(1)
		// max value: fixed
		template <int N>
		class fast_iset {
		private:
			int data[N];
			int indices[N];
			int num = 0;

			ushort times[N] = {};
			ushort time = 1;

			int get_index(int a) {
				indices[a] |= -(times[a] != time);
				times[a] = time;
				return indices[a];
			}

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
				assert_in(a, 0, N);
				if (get_index(a) != -1)
					return false;
				data[num] = a;
				indices[a] = num;
				num++;
				return true;
			}

			bool erase(int a) {
				assert_in(a, 0, N);
				int index = get_index(a);
				if (index == -1)
					return false;
				assert_gt(num, 0);
				indices[data[index] = data[--num]] = index;
				indices[a] = -1;
				return true;
			}

			void clear() {
				if (++time == 0) { // in case of overflow
					memset(times, 0, sizeof(times));
					time = 1;
				}
				num = 0;
			}

			bool contains(int a) const {
				assert_in(a, 0, N);
				return get_index(a) != -1;
			}

			const int& operator[](int i) const {
				assert_in(i, 0, num);
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

			vector<ull> keys;
			vector<T> values;
			vector<ushort> times;
			ushort time = 1;
			int num_elements = 0;
			int last_index = -1;
			ull last_key = -1;
			bool last_found = false;

		public:
			hash_imap() : keys(BUCKET_SIZE), values(BUCKET_SIZE), times(BUCKET_SIZE, 0) {
			}

			void clear() {
				num_elements = 0;
				last_found = false;
				last_index = -1;
				if (++time == 0) {
					memset(times.data(), 0, sizeof(times[0]) * BUCKET_SIZE);
					time = 1;
				}
			}

			bool access(ull key) {
				last_key = key;
				last_index = (int) (key & BUCKET_MASK);
				bool debug = false;
				while (true) {
					if (times[last_index] != time) {
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
				assert_ne(last_index, -1);
				times[last_index] = time;
				keys[last_index] = last_key;
				values[last_index] = value;
				num_elements += !last_found;
				assert_lt(num_elements, 0.85 * BUCKET_SIZE) << "bucket size is too small";
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
				assert_in(pos, 0, Size);
				return (data[pos >> WORD_SHIFT] >> (pos & WORD_MASK)) & 1;
			}

			uint to_uint() const {
				constexpr int BITS = sizeof(uint) * 8;
				for (int i = (BITS + WORD_SIZE - 1) / WORD_SIZE; i < NUM_WORDS; i++) {
					assert_eq(data[i], 0) << "uint overflow";
				}
				if constexpr (WORD_SIZE > BITS) {
					assert_eq(data[0] >> BITS, 0) << "uint overflow";
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
					assert_eq(data[i], 0) << "ull overflow";
				}
				if constexpr (WORD_SIZE > BITS) {
					assert_eq(data[0] >> BITS, 0) << "ull overflow";
				}
				ull res = (ull) data[0];
				for (int i = 1; i < (BITS + WORD_SIZE - 1) / WORD_SIZE && i < NUM_WORDS; i++) {
					res |= (ull) data[i] << i * WORD_SIZE;
				}
				return res;
			}

			rich_bitset& set(int pos, bool val = true) {
				assert_in(pos, 0, Size);
				word bit = word(1) << (pos & WORD_MASK);
				if (val) {
					data[pos >> WORD_SHIFT] |= bit;
				} else {
					data[pos >> WORD_SHIFT] &= ~bit;
				}
				return *this;
			}

			rich_bitset& reset(int pos) {
				assert_in(pos, 0, Size);
				return set(pos, false);
			}

			rich_bitset& flip(int pos) {
				assert_in(pos, 0, Size);
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
				assert_in(amount, 0, Size);
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
				assert_in(amount, 0, Size);
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

			T& back() {
				return data.back();
			}

			const T& back() const {
				return data.back();
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
				assert_eq(i, a.i);
				assert_eq(j, a.j);
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
				assert((i != 0) + (j != 0) == 1, i, j);
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

			// manhattan norm
			int mnorm() const {
				return std::abs(i) + std::abs(j);
			}

			// euclidean norm
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
				assert(in_bounds(si, sj), i, j, si, sj);
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
				assert(int_vec2(i, j).in_bounds(si, sj), i, j, si, sj);
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
				return {T(a.i + b.i), T(a.j + b.j)};
			}

			constexpr friend int_vec2 operator+(T a, int_vec2 b) {
				return {T(a + b.i), T(a + b.j)};
			}

			constexpr friend int_vec2 operator+(int_vec2 a, T b) {
				return {T(a.i + b), T(a.j + b)};
			}

			constexpr friend int_vec2 operator-(int_vec2 a, int_vec2 b) {
				return {T(a.i - b.i), T(a.j - b.j)};
			}

			constexpr friend int_vec2 operator-(T a, int_vec2 b) {
				return {T(a - b.i), T(a - b.j)};
			}

			constexpr friend int_vec2 operator-(int_vec2 a, T b) {
				return {T(a.i - b), T(a.j - b)};
			}

			constexpr friend int_vec2 operator*(int_vec2 a, int_vec2 b) {
				return {T(a.i * b.i), T(a.j * b.j)};
			}

			constexpr friend int_vec2 operator*(T a, int_vec2 b) {
				return {T(a * b.i), T(a * b.j)};
			}

			constexpr friend int_vec2 operator*(int_vec2 a, T b) {
				return {T(a.i * b), T(a.j * b)};
			}

			constexpr friend int_vec2 operator/(int_vec2 a, int_vec2 b) {
				return {T(a.i / b.i), T(a.j / b.j)};
			}

			constexpr friend int_vec2 operator/(T a, int_vec2 b) {
				return {T(a / b.i), T(a / b.j)};
			}

			constexpr friend int_vec2 operator/(int_vec2 a, T b) {
				return {T(a.i / b), T(a.j / b)};
			}

			constexpr friend int_vec2 operator%(int_vec2 a, int_vec2 b) {
				return {T(a.i % b.i), T(a.j % b.j)};
			}

			constexpr friend int_vec2 operator%(T a, int_vec2 b) {
				return {T(a % b.i), T(a % b.j)};
			}

			constexpr friend int_vec2 operator%(int_vec2 a, T b) {
				return {T(a.i % b), T(a.j % b)};
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
				out << "(" << (int) a.i << ", " << (int) a.j << ")";
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
				assert_eq(i, a.i);
				assert_eq(j, a.j);
				assert_eq(k, a.k);
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
				assert((i != 0) + (j != 0) + (k != 0) == 1, i, j, k);
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
				assert(in_bounds(si, sj, sk), i, j, k, si, sj, sk);
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
				assert(int_vec3(i, j, k).in_bounds(si, sj, sk), i, j, k, si, sj, sk);
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
				out << "(" << (int) a.i << ", " << (int) a.j << ", " << (int) a.k << ")";
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
				assert(enumerating) << "not enumerating now";
				candidates.emplace_back((int) candidates.size(), current_parent, move_id, data, hash);
				orig_candidates.emplace_back(current_parent, move_id);
			}

			vector<candidate>& candidates_to_filter() {
				assert(filtering) << "not filtering now";
				return candidates;
			}

			// CAUTION: not stable
			template <predicate<candidate&, candidate&> CandidateDirection>
			void remove_duplicates(CandidateDirection candidate_direction) {
				assert(filtering) << "not filtering now";
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
						assert_gt(num_states, 0) << "no states at the end";

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
				assert(visiting) << "not visiting now";
				toVisit.emplace_back(vertex, total_distance);
			}

			template <get_index<Vertex> GetIndex>
			vector<Vertex> restore_path(Vertex goal, GetIndex get_index) {
				assert_lt(distance[get_index(goal)], Infinity) << "goal is not reached";
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
			assert(res) << "couldn't write file: " << fileName;
		}

		string read_text_assert(string fileName) {
			auto res = read_text(fileName);
			assert(res.second) << "couldn't read file: " << fileName;
			return res.first;
		}
	} // namespace file
} // namespace shr
using namespace shr::basic;
using namespace shr::tos_impl;
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
		assert_ne(file_name, "") << "file name is not set";
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
constexpr int INF = (int) 1e9;
constexpr int N = 100;
constexpr int L = 500000;

#define repn(i) rep0(i, N)
#define repl(i) rep0(i, L)

struct Problem {
	array<int, N> ts;

	void load(istream& in) {
		int n, l;
		in >> n >> l;
		assert_eq(n, N);
		assert_eq(l, L);
		repn(i) {
			in >> ts[i];
		}
	}
};

class Solver {
public:
	struct SolverResult {
		ll score = 0;
	};
	int seed;
	rngen rng;
	SolverResult result;
	Problem p;

	Solver() : rng(12345) {
	}

	void load(istream& in, int seed = -1) {
		p.load(in);
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
	void init() {
	}

	void solveMain(ostream& out) {
		array<array<int, 2>, N> ab;
		auto computeScore = [&]() {
			static int counts[N];
			clear_with(counts, 0);
			int x = 0;
			repl(i) {
				counts[x]++;
				x = ab[x][(counts[x] & 1) ^ 1];
			}
			int res = 0;
			repn(i) {
				res += abs(counts[i] - p.ts[i]);
			}
			return (int) 1e6 - res;
		};
		// initialize
		int counts2[N] = {};

		auto checkErgodic = [&]() {
			// dfs
			bool visited[N] = {};
			static int stack[N];
			int stackSize = 0;
			int visitedCount = 0;
			stack[stackSize++] = 0;
			while (stackSize > 0) {
				int x = stack[--stackSize];
				if (visited[x])
					continue;
				visited[x] = true;
				visitedCount++;
				if (!visited[ab[x][0]])
					stack[stackSize++] = ab[x][0];
				if (!visited[ab[x][1]])
					stack[stackSize++] = ab[x][1];
			}
			int numPositives = 1;
			rep(i, 1, N) {
				numPositives += counts2[i] > 0;
			}
			return visitedCount == numPositives;
		};

		int cost = 0;
		repn(i) {
			ab[i][0] = (i + 1) % N;
			ab[i][1] = (i + 1) % N;
			counts2[ab[i][0]] += p.ts[i];
			counts2[ab[i][1]] += p.ts[i];
		}
		repn(i) {
			cost += abs(counts2[i] - p.ts[i] * 2);
		}
		int bestCost = cost;
		trace(show(cost, computeScore()));
		int iter = 0;
		constexpr double T_FROM = 500;
		constexpr double T_TO = 5;
		double temp = T_FROM;
		double progress = 0;

		double tols[65536];
		rep0(i, 65536) {
			tols[i] = -log(rng.next_float());
		}

		auto best = ab;
		for (;; iter++) {
			if ((iter & 0xfff) == 0) {
				progress = timer() / 1950.0;
				if (progress >= 1)
					break;
				temp = exp_interp(T_FROM, T_TO, progress);
			}

			[&]() { // swap
				int i = rng.next_int(N);
				int j;
				do {
					j = rng.next_int(N);
				} while (i == j);
				int pi = rng.next_int(2);
				int pj = rng.next_int(2);
				int vi = ab[i][pi];
				int vj = ab[j][pj];

				int pdiff = abs(counts2[vi] - p.ts[vi] * 2) + abs(counts2[vj] - p.ts[vj] * 2);

				int dij = p.ts[j] - p.ts[i];
				counts2[vi] += dij;
				counts2[vj] -= dij;

				int ndiff = abs(counts2[vi] - p.ts[vi] * 2) + abs(counts2[vj] - p.ts[vj] * 2);

				int diff = ndiff - pdiff;
				int ncost = cost + diff;

				double tol = temp * tols[iter & 0xffff];
				if (ncost < cost + tol) {
					ab[i][pi] = vj;
					ab[j][pj] = vi;

					if (!checkErgodic()) {
						ab[i][pi] = vi;
						ab[j][pj] = vj;
						counts2[vi] -= dij;
						counts2[vj] += dij;
						return;
					}

					cost = ncost;
					if (update_min(bestCost, cost)) {
						best = ab;
						trace("best1 ", show(bestCost, iter, temp));
					}
				} else {
					counts2[vi] -= dij;
					counts2[vj] += dij;
				}
			}();

			[&]() { // change
				int i = rng.next_int(N);
				int pa = rng.next_int(2);
				int pv = ab[i][pa];
				int nv = rng.next_int(N);

				int pdiff = abs(counts2[pv] - p.ts[pv] * 2) + abs(counts2[nv] - p.ts[nv] * 2);

				counts2[pv] -= p.ts[i];
				counts2[nv] += p.ts[i];

				int ndiff = abs(counts2[pv] - p.ts[pv] * 2) + abs(counts2[nv] - p.ts[nv] * 2);

				int diff = ndiff - pdiff;
				int ncost = cost + diff;

				double tol = temp * tols[iter & 0xffff];
				if (ncost < cost + tol) {
					ab[i][pa] = nv;

					if (!checkErgodic()) {
						ab[i][pa] = pv;
						counts2[pv] += p.ts[i];
						counts2[nv] -= p.ts[i];
						return;
					}

					cost = ncost;
					if (update_min(bestCost, cost)) {
						best = ab;
						trace("best2 ", show(bestCost, iter, temp));
					}
				} else {
					counts2[pv] += p.ts[i];
					counts2[nv] -= p.ts[i];
				}
			}();

			[&]() { // swap2
				int i = rng.next_int(N);
				int j;
				do {
					j = rng.next_int(N);
				} while (i == j);

				int pdiff = abs(counts2[i] - p.ts[i] * 2) + abs(counts2[j] - p.ts[j] * 2);
				swap(counts2[i], counts2[j]);
				int ndiff = abs(counts2[i] - p.ts[i] * 2) + abs(counts2[j] - p.ts[j] * 2);
				int diff = ndiff - pdiff;
				int ncost = cost + diff;
				double tol = temp * tols[iter & 0xffff];
				if (ncost < cost + tol) {
					repn(k) {
						if (ab[k][0] == i)
							ab[k][0] = j;
						else if (ab[k][0] == j)
							ab[k][0] = i;
						if (ab[k][1] == i)
							ab[k][1] = j;
						else if (ab[k][1] == j)
							ab[k][1] = i;
					}

					if (!checkErgodic()) {
						repn(k) {
							if (ab[k][0] == i)
								ab[k][0] = j;
							else if (ab[k][0] == j)
								ab[k][0] = i;
							if (ab[k][1] == i)
								ab[k][1] = j;
							else if (ab[k][1] == j)
								ab[k][1] = i;
						}
						swap(counts2[i], counts2[j]);
						return;
					}

					cost = ncost;
					if (update_min(bestCost, cost)) {
						best = ab;
						trace("best3 ", show(bestCost, iter, temp));
					}
				} else {
					swap(counts2[i], counts2[j]);
				}
			}();
		}
		ab = best;
		int exactScore = computeScore();
		trace(show(bestCost, exactScore));
		result.score = exactScore;
		// print solution
		repn(i) {
			out << ab[i][0] << " " << ab[i][1] << endl;
		}
	}
};

// single-threaded tester, handy but slow
struct EasyTester {
	struct TestCase {
		int seed;
		int time;
		ll score;
		ll best = 0;
		ll relScore = 0;
	};
	bool maximize;

	EasyTester(string direction) : maximize(direction == "max") {
		assert(direction == "min" || direction == "max");
	}

	void run(const vector<int> seeds) {
		vector<TestCase> cases;

		bool doTrace = debug;
		debug = true;
		for (int seed : seeds) {
			timer(true);
			trace("------------ SOLVING SEED ", seed, " ------------");
			debug = doTrace;
			Solver s;
			s.load(seed);
			s.solve();
			debug = true;
			int time = timer();
			if (len(seeds) > 1)
				trace("score: ", s.result.score, " (", time, " ms)\n");
			if (s.result.score != -1) // -1 means skipped
				cases.emplace_back(seed, time, s.result.score);
		}

		if (auto [bests, bestsLoaded] = read_text("scores/"s + (maximize ? "max" : "min") + "_scores.txt");
			bestsLoaded) {
			istringstream iss(bests);
			int seed;
			ll best;
			while (iss >> seed >> best) {
				for (auto& c : cases) {
					if (c.seed == seed) {
						c.best = best;
						break;
					}
				}
			}
		}
		// compute relative scores
		for (auto& c : cases) {
			if (c.score == 0)
				c.relScore = 0;
			else if (c.best == 0)
				c.relScore = (ll) 1e9;
			else if (maximize)
				c.relScore = (ll) round(c.score * 1e9 / c.best);
			else
				c.relScore = (ll) round(c.best * 1e9 / c.score);
		}

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
			double relScoreSum = 0;
			for (auto& c : cases) {
				scoreSum += c.score;
				relScoreSum += c.relScore;
			}
			double scale = 1.0 / len(cases);
			trace("sum     : ", scoreSum);
			trace("mean    : ", (ll) round(scoreSum * scale));
			trace("rel sum : ", (ll) round(relScoreSum));
			trace("rel mean: ", (ll) round(relScoreSum * scale));
		} else {
			trace("abs score: ", cases[0].score);
			trace("rel score: ", cases[0].relScore);
			trace("time     : ", cases[0].time, " ms");
		}
	}

private:
	static void print(const TestCase& c) {
		int seed = c.seed;
		string space = seed < 10 ? "   " : seed < 100 ? "  " : seed < 1000 ? " " : "";
		trace("  seed ", space, seed, ": abs = ", c.score, ", rel = ", c.relScore, " (", c.time, " ms)");
	};
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
	// write metadata
	ostringstream oss;
	oss << "seed m k" << endl;
	rep0(seed, 5000) {
		Solver sol;
		sol.load(seed);
		double mlog = log2(sol.p.m / 50.0);
		double mLogApprox = (int) (round(mlog * 10)) / 10.0;
		int mApprox = (int) (round(pow(2, mLogApprox) * 50));
		int kApprox = (int) (round(sol.p.k / 500.0) * 500);
		oss << seed << " " << mApprox << " " << kApprox << endl;
	}
	write_text("scores/input.txt", oss.str());
#elif 0
	// for local/remote testers
	debug = false;
	render = false;
	int seed;
	cin >> seed;
	cin >> time_scale;

	timer(true);
	Solver sol;
	sol.load(seed);
	sol.solve();

	cout << sol.result.score << " " << timer() << endl;
#elif 1
	EasyTester tester("min");
	int num = 10;
	int from = 0;
	int stride = 1;
	int single = -1;

	vector<int> seeds = {};

	debug = true;
	render = false;

	if (seeds.empty() || single != -1) {
		seeds.clear();
		if (single == -1) {
			rep0(t, num) {
				seeds.push_back(from + t * stride);
			}
		} else {
			seeds.push_back(single);
		}
	}
	tester.run(seeds);
#endif
}
