#if ONLINE_JUDGE
#define NDEBUG
#pragma GCC target("avx2,fma")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#else
#undef NDEBUG
#endif

#ifdef _MSC_VER
#include <intrin.h>
#define NDEBUG
#else
#include <x86intrin.h>
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
#include <iterator>
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

// note: a huge library begins, the main part starts around line 3150

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
		function<void()> on_abort = nullptr;

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
				if (on_abort) {
					on_abort();
				}
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

		template <class T, size_t N>
		int len(const array<T, N>& a) {
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
		bool mineq(T& current_min, T candidate) {
			if (candidate < current_min) {
				current_min = candidate;
				return true;
			}
			return false;
		}

		template <totally_ordered T>
		bool maxeq(T& current_max, T candidate) {
			if (candidate > current_max) {
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
				return {v, o.option};
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
		concept is_tos_value = is_same_v<T, tos_value<class T::value_type>>;

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

			tos_modifier br(int n = 1) {
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
			auto map(Range& as, F f) {
				using T = decltype(f(*as.begin()));
				vector<T> res;
				for (auto& a : as) {
					res.push_back(f(a));
				}
				return res;
			}

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
				for (const auto& a : as) {
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
			auto zip_with(const RangeA& as, const RangeB& bs, F f) {
				using T = decltype(f(*as.begin(), *bs.begin()));
				vector<T> res;
				auto a = as.begin();
				auto b = bs.begin();
				while (a != as.end() && b != bs.end()) {
					res.push_back(f(*a++, *b++));
				}
				return res;
			}

			template <ranges::range RangeA, ranges::range RangeB, class F>
			auto zip_with(RangeA& as, RangeB& bs, F f) {
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
		return lambda::join(lambda::zip_with(keys, vals,          \
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
#define assert_in(a, lo, hi) assert()
#define assert_size(a, n) assert()
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
#define assert_in(a, lo, hi)                                                                          \
	if (!((a) >= (lo) && (a) <= (hi)))                                                                \
	bombstream("("s + #lo + ") <= (" + #a + ") <= (" + #hi + ")",                                     \
		"i.e. " + tos(lo) + " <= " + tos(a) + " <= " + tos(hi) + " doesn't hold", __FILE__, __LINE__, \
		SHR_ABORT)
#define assert_size(a, n)                           \
	if (!((a) >= 0 && (a) < (n)))                   \
	bombstream("0 <= ("s + #a + ") < (" + #n + ")", \
		"i.e. 0 <= " + tos(a) + " < " + tos(n) + " doesn't hold", __FILE__, __LINE__, SHR_ABORT)
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
				assert_gt(mod, 0);
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
				return (double) next32() / (double) 0x100000000;
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

			bool next_prob(double p) {
				return next_float() < p;
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
		// - random access: O(1)
		// - push: O(1)
		// - insert: n/a
		// - erase by position: O(1), unstable
		// - erase by element: n/a
		// - max size: fixed
		template <class T, int N>
		class fast_vector {
		private:
			T data[N];
			int num = 0;

		public:
			using iterator = T*;
			using const_iterator = const T*;

			fast_vector() {
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
				assert_size(i, num);
				return data[i];
			}

			const T& operator[](int i) const {
				assert_size(i, num);
				return data[i];
			}

			T& back() {
				assert_gt(num, 0);
				return data[num - 1];
			}

			const T& back() const {
				assert_gt(num, 0);
				return data[num - 1];
			}

			int size() const {
				return num;
			}

			bool empty() const {
				return num == 0;
			}
		};

		// fast-clearing array
		template <class T, int N, T DefaultValue = (T) 0>
		class fast_array {
		private:
			struct iterator {
				using difference_type = ptrdiff_t;
				using value_type = T;
				using pointer = T*;
				using reference = T&;
				using iterator_category = forward_iterator_tag;
				fast_array& self;
				int index;

				iterator& operator++() {
					index++;
					return *this;
				}

				iterator operator++(int) {
					iterator tmp = *this;
					index++;
					return tmp;
				}

				T& operator*() {
					return self[index];
				}

				bool operator==(const iterator& other) const {
					return index == other.index;
				}
			};

			struct const_iterator {
				using difference_type = ptrdiff_t;
				using value_type = T;
				using pointer = const T*;
				using reference = const T&;
				using iterator_category = forward_iterator_tag;
				const fast_array& self;
				int index;

				const_iterator& operator++() {
					index++;
					return *this;
				}

				const_iterator operator++(int) {
					const_iterator tmp = *this;
					index++;
					return tmp;
				}

				const T& operator*() const {
					return self[index];
				}

				bool operator==(const const_iterator& other) const {
					return index == other.index;
				}
			};

			mutable T data[N];

			mutable ushort times[N] = {};
			ushort time = 1;

			T& get(int i) const {
				if (times[i] != time) {
					times[i] = time;
					data[i] = DefaultValue;
				}
				return data[i];
			}

		public:
			fast_array() {
			}

			iterator begin() {
				return {*this, 0};
			}

			iterator end() {
				return {*this, N};
			}

			const_iterator begin() const {
				return {*this, 0};
			}

			const_iterator end() const {
				return {*this, N};
			}

			void clear() {
				if (++time == 0) { // in case of overflow
					memset(times, 0, sizeof(times));
					time = 1;
				}
			}

			T& operator[](int i) {
				assert_size(i, N);
				return get(i);
			}

			const T& operator[](int i) const {
				assert_size(i, N);
				return get(i);
			}

			int size() const {
				return N;
			}
		};

		// - random access: O(1), unordered
		// - insert: O(1)
		// - erase: O(1)
		// - contains: O(1)
		// - clear: O(1)
		// - max value: fixed
		template <int N>
		class fast_iset {
		private:
			int data[N];
			mutable int indices[N];
			int num = 0;

			mutable ushort times[N] = {};
			ushort time = 1;

			int get_index(int a) const {
				indices[a] |= -(times[a] != time);
				times[a] = time;
				return indices[a];
			}

		public:
			struct iterator {
				using difference_type = ptrdiff_t;
				using value_type = int;
				using pointer = int*;
				using reference = int&;
				using iterator_category = forward_iterator_tag;
				fast_iset& self;
				int index;

				iterator& operator++() {
					index++;
					return *this;
				}

				iterator operator++(int) {
					iterator tmp = *this;
					index++;
					return tmp;
				}

				int& operator*() {
					return self.data[index];
				}

				strong_ordering operator<=>(const iterator& other) const {
					return index <=> other.index;
				}

				bool operator==(const iterator& other) const {
					return index == other.index;
				}
			};

			struct const_iterator {
				using difference_type = ptrdiff_t;
				using value_type = int;
				using pointer = const int*;
				using reference = const int&;
				using iterator_category = forward_iterator_tag;
				const fast_iset& self;
				int index;

				const_iterator& operator++() {
					index++;
					return *this;
				}

				const_iterator operator++(int) {
					const_iterator tmp = *this;
					index++;
					return tmp;
				}

				const int& operator*() const {
					return self.data[index];
				}

				strong_ordering operator<=>(const const_iterator& other) const {
					return index <=> other.index;
				}

				bool operator==(const const_iterator& other) const {
					return index == other.index;
				}
			};

			fast_iset() {
				memset(indices, -1, sizeof(indices));
			}

			iterator begin() {
				return {*this, 0};
			}

			iterator end() {
				return {*this, num};
			}

			const_iterator begin() const {
				return {*this, 0};
			}

			const_iterator end() const {
				return {*this, num};
			}

			bool insert(int a) {
				assert_size(a, N);
				if (get_index(a) != -1)
					return false;
				data[num] = a;
				indices[a] = num;
				num++;
				return true;
			}

			bool erase(int a) {
				assert_size(a, N);
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
				assert_size(a, N);
				return get_index(a) != -1;
			}

			const int operator[](int i) const {
				assert_size(i, num);
				return data[i];
			}

			int size() const {
				return num;
			}

			bool empty() const {
				return num == 0;
			}
		};

		// - insert: O(1)
		// - erase: n/a
		// - get/set: O(1)
		// - clear: O(1)
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

		// - random access: O(1)
		// - push: O(1)
		// - insert: n/a
		// - erase: n/a
		// - reallocation: never
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
				assert_size(i, num_total);
				return packs[i >> UnitBits][i & UNIT_MASK];
			}

			const T& operator[](int i) const {
				assert_size(i, num_total);
				return packs[i >> UnitBits][i & UNIT_MASK];
			}

			int size() const {
				return num_total;
			}
		};

		// fast and richer bitset, basically an std::bitset with
		// - addition
		// - subtraction
		// - count[r/l]_[zero/one]
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
#define SHR_REP_WORDS(i) for (int i = 0; i < NUM_WORDS; i++)
#define SHR_REP_INNER_WORDS(i) for (int i = 0; i < NUM_WORDS - 1; i++)
#define SHR_REP_WORDS_REV(i) for (int i = NUM_WORDS - 1; i >= 0; i--)
#define SHR_REP_INNER_WORDS_REV(i) for (int i = NUM_WORDS - 2; i >= 0; i--)

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
				SHR_REP_INNER_WORDS(i) {
					res &= data[i] == word(-1);
				}
				res &= data[LAST_WORD] == LAST_WORD_MASK;
				return res;
			}

			bool none() const {
				bool res = true;
				SHR_REP_WORDS(i) {
					res &= data[i] == 0;
				}
				return res;
			}

			bool any() const {
				bool res = false;
				SHR_REP_WORDS(i) {
					res |= data[i] != 0;
				}
				return res;
			}

			int count() const {
				int res = 0;
				SHR_REP_WORDS(i) {
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
				assert_size(pos, Size);
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
				assert_size(pos, Size);
				word bit = word(1) << (pos & WORD_MASK);
				if (val) {
					data[pos >> WORD_SHIFT] |= bit;
				} else {
					data[pos >> WORD_SHIFT] &= ~bit;
				}
				return *this;
			}

			rich_bitset& reset(int pos) {
				assert_size(pos, Size);
				return set(pos, false);
			}

			rich_bitset& flip(int pos) {
				assert_size(pos, Size);
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
				SHR_REP_INNER_WORDS(i) {
					data[i] ^= word(-1);
				}
				data[LAST_WORD] ^= LAST_WORD_MASK;
				return *this;
			}

			word* words() {
				return data;
			}

			rich_bitset& operator&=(const rich_bitset& a) {
				SHR_REP_WORDS(i) {
					data[i] &= a.data[i];
				}
				return *this;
			}

			rich_bitset& operator|=(const rich_bitset& a) {
				SHR_REP_WORDS(i) {
					data[i] |= a.data[i];
				}
				return *this;
			}

			rich_bitset& operator^=(const rich_bitset& a) {
				SHR_REP_WORDS(i) {
					data[i] ^= a.data[i];
				}
				return *this;
			}

			rich_bitset& operator<<=(int amount) {
				assert_size(amount, Size);
				int nw = amount >> WORD_SHIFT;
				if (nw > 0) {
					SHR_REP_WORDS_REV(i) {
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
				assert_size(amount, Size);
				int nw = amount >> WORD_SHIFT;
				if (nw > 0) {
					SHR_REP_WORDS(i) {
						data[i] = i + nw >= NUM_WORDS ? 0 : data[i + nw];
					}
				}
				int nb = amount & WORD_MASK;
				if (nb) {
					SHR_REP_INNER_WORDS(i) {
						data[i] = data[i] >> nb | data[i + 1] << (WORD_SIZE - nb);
					}
					data[LAST_WORD] >>= nb;
				}
				return *this;
			}

			rich_bitset& operator+=(const rich_bitset& a) {
				word carry = 0;
				SHR_REP_WORDS(i) {
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
				SHR_REP_WORDS(i) {
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
				SHR_REP_WORDS(i) {
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
				SHR_REP_WORDS(i) {
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
				SHR_REP_WORDS_REV(i) {
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
#undef SHR_REP_WORDS
#undef SHR_REP_INNER_WORDS
#undef SHR_REP_WORDS_REV
#undef SHR_REP_INNER_WORDS_REV
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

			// chebyshev norm
			int cnorm() const {
				return std::max(std::abs(i), std::abs(j));
			}

			// euclidean norm
			double enorm() const {
				return sqrt(dotll(*this));
			}

			// euclidean norm squared
			ll enorm2() const {
				return i * i + j * j;
			}

			// euclidean norm squared
			int enorm2_int() const {
				ll res = enorm2();
				assert_le(res, (ll) std::numeric_limits<int>::max()) << "enorm2 overflowed: " << *this;
				return (int) res;
			}

			constexpr int dot(int_vec2 a) const {
				return i * a.i + j * a.j;
			}

			constexpr ll dotll(int_vec2 a) const {
				return (ll) i * a.i + (ll) j * a.j;
			}

			constexpr int cross(int_vec2 a) const {
				return i * a.j - j * a.i;
			}

			constexpr ll crossll(int_vec2 a) const {
				return (ll) i * a.j - (ll) j * a.i;
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

#define SHR_DEFINE_IVEC2_OP(TA, ai, aj, TB, bi, bj, op) \
	constexpr friend int_vec2 operator op(TA a, TB b) { \
		return {T(ai op bi), T(aj op bj)};              \
	}
#define SHR_DEFINE_IVEC2_OP_EQ(TA, ai, aj, op) \
	constexpr int_vec2 operator op(TA a) {     \
		i op ai;                               \
		j op aj;                               \
		return *this;                          \
	}
#define SHR_DEFINE_IVEC2_OP_CMP(cond, op)                       \
	constexpr friend bool operator op(int_vec2 a, int_vec2 b) { \
		return cond;                                            \
	}

			SHR_DEFINE_IVEC2_OP(int_vec2, a.i, a.j, int_vec2, b.i, b.j, +)
			SHR_DEFINE_IVEC2_OP(int_vec2, a.i, a.j, int_vec2, b.i, b.j, -)
			SHR_DEFINE_IVEC2_OP(int_vec2, a.i, a.j, int_vec2, b.i, b.j, *)
			SHR_DEFINE_IVEC2_OP(int_vec2, a.i, a.j, int_vec2, b.i, b.j, /)
			SHR_DEFINE_IVEC2_OP(int_vec2, a.i, a.j, int_vec2, b.i, b.j, %)
			SHR_DEFINE_IVEC2_OP(int_vec2, a.i, a.j, T, b, b, +)
			SHR_DEFINE_IVEC2_OP(int_vec2, a.i, a.j, T, b, b, -)
			SHR_DEFINE_IVEC2_OP(int_vec2, a.i, a.j, T, b, b, *)
			SHR_DEFINE_IVEC2_OP(int_vec2, a.i, a.j, T, b, b, /)
			SHR_DEFINE_IVEC2_OP(int_vec2, a.i, a.j, T, b, b, %)
			SHR_DEFINE_IVEC2_OP(T, a, a, int_vec2, b.i, b.j, +)
			SHR_DEFINE_IVEC2_OP(T, a, a, int_vec2, b.i, b.j, -)
			SHR_DEFINE_IVEC2_OP(T, a, a, int_vec2, b.i, b.j, *)
			SHR_DEFINE_IVEC2_OP(T, a, a, int_vec2, b.i, b.j, /)
			SHR_DEFINE_IVEC2_OP(T, a, a, int_vec2, b.i, b.j, %)
			SHR_DEFINE_IVEC2_OP_EQ(int_vec2, a.i, a.j, +=)
			SHR_DEFINE_IVEC2_OP_EQ(int_vec2, a.i, a.j, -=)
			SHR_DEFINE_IVEC2_OP_EQ(int_vec2, a.i, a.j, *=)
			SHR_DEFINE_IVEC2_OP_EQ(int_vec2, a.i, a.j, /=)
			SHR_DEFINE_IVEC2_OP_EQ(int_vec2, a.i, a.j, %=)
			SHR_DEFINE_IVEC2_OP_EQ(T, a, a, +=)
			SHR_DEFINE_IVEC2_OP_EQ(T, a, a, -=)
			SHR_DEFINE_IVEC2_OP_EQ(T, a, a, *=)
			SHR_DEFINE_IVEC2_OP_EQ(T, a, a, /=)
			SHR_DEFINE_IVEC2_OP_EQ(T, a, a, %=)
			SHR_DEFINE_IVEC2_OP_CMP(a.i == b.i && a.j == b.j, ==)
			SHR_DEFINE_IVEC2_OP_CMP(a.i != b.i || a.j != b.j, !=)
			SHR_DEFINE_IVEC2_OP_CMP(a.i < b.i || a.i == b.i && a.j < b.j, <)
			SHR_DEFINE_IVEC2_OP_CMP(a.i > b.i || a.i == b.i && a.j > b.j, >)
			SHR_DEFINE_IVEC2_OP_CMP(a.i <= b.i && a.j <= b.j, <=)
			SHR_DEFINE_IVEC2_OP_CMP(a.i >= b.i && a.j >= b.j, >=)

#undef SHR_DEFINE_IVEC2_OP
#undef SHR_DEFINE_IVEC2_OP_EQ
#undef SHR_DEFINE_IVEC2_OP_CMP

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

			// manhattan norm
			int mnorm() const {
				return std::abs(i) + std::abs(j) + std::abs(k);
			}

			// chebyshev norm
			int cnorm() const {
				return std::max({std::abs(i), std::abs(j), std::abs(k)});
			}

			// euclidean norm
			double enorm() const {
				return sqrt(dotll(*this));
			}

			// euclidean norm squared
			ll enorm2() const {
				return i * i + j * j + k * k;
			}

			// euclidean norm squared
			int enorm2_int() const {
				ll res = enorm2();
				assert_le(res, (ll) std::numeric_limits<int>::max()) << "enorm2 overflowed: " << *this;
				return (int) res;
			}

			constexpr int dot(int_vec3 a) const {
				return i * a.i + j * a.j + k * a.k;
			}

			constexpr ll dotll(int_vec3 a) const {
				return (ll) i * a.i + (ll) j * a.j + (ll) k * a.k;
			}

			constexpr int_vec3 cross(int_vec3 a) const {
				return int_vec3(j * a.k - k * a.j, k * a.i - i * a.k, i * a.j - j * a.i);
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

#define SHR_DEFINE_IVEC3_OP(TA, ai, aj, ak, TB, bi, bj, bk, op) \
	constexpr friend int_vec3 operator op(TA a, TB b) {         \
		return {T(ai op bi), T(aj op bj), T(ak op bk)};         \
	}
#define SHR_DEFINE_IVEC3_OP_EQ(TA, ai, aj, ak, op) \
	constexpr int_vec3 operator op(TA a) {         \
		i op ai;                                   \
		j op aj;                                   \
		k op ak;                                   \
		return *this;                              \
	}
#define SHR_DEFINE_IVEC3_OP_CMP(cond, op)                       \
	constexpr friend bool operator op(int_vec3 a, int_vec3 b) { \
		return cond;                                            \
	}

			SHR_DEFINE_IVEC3_OP(int_vec3, a.i, a.j, a.k, int_vec3, b.i, b.j, b.k, +)
			SHR_DEFINE_IVEC3_OP(int_vec3, a.i, a.j, a.k, int_vec3, b.i, b.j, b.k, -)
			SHR_DEFINE_IVEC3_OP(int_vec3, a.i, a.j, a.k, int_vec3, b.i, b.j, b.k, *)
			SHR_DEFINE_IVEC3_OP(int_vec3, a.i, a.j, a.k, int_vec3, b.i, b.j, b.k, /)
			SHR_DEFINE_IVEC3_OP(int_vec3, a.i, a.j, a.k, int_vec3, b.i, b.j, b.k, %)
			SHR_DEFINE_IVEC3_OP(int_vec3, a.i, a.j, a.k, T, b, b, b, +)
			SHR_DEFINE_IVEC3_OP(int_vec3, a.i, a.j, a.k, T, b, b, b, -)
			SHR_DEFINE_IVEC3_OP(int_vec3, a.i, a.j, a.k, T, b, b, b, *)
			SHR_DEFINE_IVEC3_OP(int_vec3, a.i, a.j, a.k, T, b, b, b, /)
			SHR_DEFINE_IVEC3_OP(int_vec3, a.i, a.j, a.k, T, b, b, b, %)
			SHR_DEFINE_IVEC3_OP(T, a, a, a, int_vec3, b.i, b.j, b.k, +)
			SHR_DEFINE_IVEC3_OP(T, a, a, a, int_vec3, b.i, b.j, b.k, -)
			SHR_DEFINE_IVEC3_OP(T, a, a, a, int_vec3, b.i, b.j, b.k, *)
			SHR_DEFINE_IVEC3_OP(T, a, a, a, int_vec3, b.i, b.j, b.k, /)
			SHR_DEFINE_IVEC3_OP(T, a, a, a, int_vec3, b.i, b.j, b.k, %)
			SHR_DEFINE_IVEC3_OP_EQ(int_vec3, a.i, a.j, a.k, +=)
			SHR_DEFINE_IVEC3_OP_EQ(int_vec3, a.i, a.j, a.k, -=)
			SHR_DEFINE_IVEC3_OP_EQ(int_vec3, a.i, a.j, a.k, *=)
			SHR_DEFINE_IVEC3_OP_EQ(int_vec3, a.i, a.j, a.k, /=)
			SHR_DEFINE_IVEC3_OP_EQ(int_vec3, a.i, a.j, a.k, %=)
			SHR_DEFINE_IVEC3_OP_EQ(T, a, a, a, +=)
			SHR_DEFINE_IVEC3_OP_EQ(T, a, a, a, -=)
			SHR_DEFINE_IVEC3_OP_EQ(T, a, a, a, *=)
			SHR_DEFINE_IVEC3_OP_EQ(T, a, a, a, /=)
			SHR_DEFINE_IVEC3_OP_EQ(T, a, a, a, %=)
			SHR_DEFINE_IVEC3_OP_CMP(a.i == b.i && a.j == b.j && a.k == b.k, ==)
			SHR_DEFINE_IVEC3_OP_CMP(a.i != b.i || a.j != b.j || a.k != b.k, !=)
			SHR_DEFINE_IVEC3_OP_CMP(a.i < b.i || a.i == b.i && (a.j < b.j || a.j == b.j && a.k < b.k), <)
			SHR_DEFINE_IVEC3_OP_CMP(a.i > b.i || a.i == b.i && (a.j > b.j || a.j == b.j && a.k > b.k), >)
			SHR_DEFINE_IVEC3_OP_CMP(a.i <= b.i && a.j <= b.j && a.k <= b.k, <=)
			SHR_DEFINE_IVEC3_OP_CMP(a.i >= b.i && a.j >= b.j && a.k >= b.k, >=)

#undef SHR_DEFINE_IVEC3_OP
#undef SHR_DEFINE_IVEC3_OP_EQ
#undef SHR_DEFINE_IVEC3_OP_CMP

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

			// - get_score: `(state) -> score`
			// - apply_move: `(state, move) -> void`
			// - enumerate_candidates: `(state) -> void`
			// - filter_candidates: `(turn) -> void`
			// - template:
			// `[&](State& st) { return 0; },`
			// `[&](State& st, MoveId move) {},`
			// `[&](State& st) {},`
			// `[&](int turn) {}`
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

			// - get_score: `(state) -> score`
			// - apply_move: `(state, move) -> void`
			// - undo_move: `(state) -> void`
			// - enumerate_candidates: `(state) -> void`
			// - filter_candidates: `(turn) -> void`
			// - template:
			// `[&](State& st) { return 0; },`
			// `[&](State& st, MoveId move) {},`
			// `[&](State& st) {},`
			// `[&](State& st) {},`
			// `[&](int turn) {}`
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
			State best_state;

			simulated_annealing() {
			}

			simulated_annealing(Direction dir) : dir(dir) {
			}

			// - get_score: `(state) -> score`
			// - update_progress: `(iter) -> progress`
			// - try_transition: `(state, tolerance) -> accepted`
			// - best_updated: `(state, score, iter, temp) -> void`
			// - template:
			// `[&](State& st) { return 0; },`
			// `[&](int iter) { return 1; },`
			// `[&](State& st, double tol) { return false; },`
			// `[&](State& st, Score score, int iter, double temp) {}`
			template <get_score<State, Score> GetScore, update_progress UpdateProgress,
				try_transition<State> TryTransition>
			State run(const State& initial_state, rngen& rng, GetScore get_score,
				UpdateProgress update_progress, TryTransition try_transition,
				function<void(State&, Score, int, double)> best_updated = nullptr) {
				State state = initial_state;
				Score score = get_score(state);
				best_score = score;
				best_state = state;

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

	namespace movie {
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
	} // namespace movie
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
using namespace shr::movie;

//
// --- macros ---
//

#define rep(i, from, until) for (int i = (from); i < (until); i++)
#define repr(i, from, until) for (int i = (until) - 1; i >= (from); i--)
#define rep0(i, until) rep(i, 0, until)
#define rep0r(i, until) repr(i, 0, until)

//
// --- tester ---
//

// single-threaded tester, handy but slow
template <typename Solver>
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
		assert(direction == "min" || direction == "max") << direction;
	}

	void run(const vector<int>& seeds) {
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

		bool showRelScore = false;
		if (auto [bests, bestsLoaded] = read_text("scores/"s + (maximize ? "max" : "min") + "_scores.txt");
			bestsLoaded) {
			istringstream iss(bests);
			int seed;
			ll best;
			while (iss >> seed >> best) {
				for (auto& c : cases) {
					if (c.seed == seed) {
						c.best = best;
						showRelScore = true;
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
				print(c, showRelScore);

			trace("sort by seed:");
			sort(cases.begin(), cases.end(), [&](auto a, auto b) {
				return a.seed < b.seed;
			});
			for (auto& c : cases)
				print(c, showRelScore);

			ll scoreSum = 0;
			double relScoreSum = 0;
			for (auto& c : cases) {
				scoreSum += c.score;
				relScoreSum += c.relScore;
			}
			double scale = 1.0 / len(cases);
			trace("sum     : ", scoreSum);
			trace("mean    : ", (ll) round(scoreSum * scale));
			if (showRelScore) {
				trace("rel sum : ", (ll) round(relScoreSum));
				trace("rel mean: ", (ll) round(relScoreSum * scale));
			}
		} else {
			if (showRelScore) {
				trace("abs score: ", cases[0].score);
				trace("rel score: ", cases[0].relScore);
			} else {
				trace("score    : ", cases[0].score);
			}
			trace("time     : ", cases[0].time, " ms");
		}
	}

private:
	static void print(const TestCase& c, bool showRelScore) {
		int seed = c.seed;
		string space = seed < 10 ? "   " : seed < 100 ? "  " : seed < 1000 ? " " : "";
		if (showRelScore) {
			trace("  seed ", space, seed, ": abs = ", c.score, ", rel = ", c.relScore, " (", c.time, " ms)");
		} else {
			trace("  seed ", space, seed, ": ", c.score, " (", c.time, " ms)");
		}
	};
};

// --------------------------------------------------
// main part
// --------------------------------------------------

bool isLocal = false;
bool render = false;
movie mov;

// define N and stuff here
constexpr int INF = (int) 1e9;
constexpr int MAX_N = 20;
constexpr int MAX_N2 = MAX_N * MAX_N;
constexpr int MAX_K = MAX_N2;
#define repn(i) rep0(i, n)
#define rep2() repn(i) repn(j)

// portable problem data
struct Problem {
	int n = 0;
	int numT = 0; // number of targets
	int maxS = 0; // max steps
	ivec2 spos; // start position
	vector<vector<bool>> wallv;
	vector<vector<bool>> wallh;
	vector<ivec2> targets;
	vector<array<bool, 4>> walls;

	void load(istream& in) {
		in >> n >> numT >> maxS;
		wallv.assign(n, vector<bool>(n - 1, false));
		wallh.assign(n - 1, vector<bool>(n, false));
		rep0(i, n) {
			string s;
			in >> s;
			rep0(j, n - 1) {
				wallv[i][j] = s[j] == '1';
			}
		}
		rep0(i, n - 1) {
			string s;
			in >> s;
			rep0(j, n) {
				wallh[i][j] = s[j] == '1';
			}
		}
		rep0(_, numT) {
			int i;
			int j;
			in >> i >> j;
			targets.emplace_back(i, j);
		}

		spos = targets[0];
		targets.erase(targets.begin());
		numT--;

		walls.assign(n * n, array<bool, 4>{false, false, false, false});
		rep2() {
			ivec2 pos(i, j);
			int posi = pos.pack(n);
			rep0(dir, 4) {
				ivec2 npos = pos + ivec2::dir(dir);
				int nposi = npos.pack_if_in_bounds(n);
				if (nposi == -1) {
					walls[posi][dir] = true;
					continue;
				}
				switch (dir) {
				case 0: // up
					walls[posi][dir] = wallh[pos.i - 1][pos.j];
					break;
				case 1: // down
					walls[posi][dir] = wallh[pos.i][pos.j];
					break;
				case 2: // left
					walls[posi][dir] = wallv[pos.i][pos.j - 1];
					break;
				case 3: // right
					walls[posi][dir] = wallv[pos.i][pos.j];
					break;
				}
			}
		}
	}
};

static constexpr int CELL_STOP = 0;
static constexpr int CELL_ICE = 1;
static constexpr int CELL_ROT1 = 2; // UDLR -> LRUD
static constexpr int CELL_ROT2 = 3; // UDLR -> RLDU

struct Cell {
	int kind = CELL_STOP;

	bool stop() const {
		return kind == CELL_STOP;
	}

	bool ice() const {
		return kind == CELL_ICE;
	}

	bool rot() const {
		return kind == CELL_ROT1 || kind == CELL_ROT2;
	}

	int nextDir(int dir) const {
		assert_ne(dir, -1);
		if (stop())
			return -1;
		if (kind == CELL_ICE)
			return dir;
		if (kind == CELL_ROT1) {
			static constexpr int rot_map[4] = {2, 3, 0, 1};
			return rot_map[dir];
		}
		if (kind == CELL_ROT2) {
			static constexpr int rot_map[4] = {3, 2, 1, 0};
			return rot_map[dir];
		}
		fail() << show(dir);
		return -1;
	}

	friend ostream& operator<<(ostream& os, const Cell& cell) {
		switch (cell.kind) {
		case CELL_STOP:
			os << "@";
			break;
		case CELL_ICE:
			os << "_";
			break;
		case CELL_ROT1:
			os << "\\";
			break;
		case CELL_ROT2:
			os << "/";
			break;
		default:
			os << "?";
			break;
		}
		return os;
	}
};

struct ScoreInfo {
	int dist = INF;
	int steps = INF;
	array<int, 5> entryCounts = {};
	array<array<int, 4>, 2> indirectEntryCounts = {};
	double score = INF;
};

struct Solution {
	int numColors = INF;
	int numStates = INF;
	int initialState = -1;
	vector<int> colors;
	vector<array<int, 5>> rules;
	vector<pii> colorStates;
};

struct Compressor {
	struct Cell;
	struct ColorGroup;

	static constexpr int MAX_STATES = 512;
	int numStates;
	int numInitialCells;
	vector<vector<Cell*>> buckets;
	vector<int> possiblePairCounts;
	vector<Cell*> livingCells;

	Cell* cellsBuffer = nullptr;
	ColorGroup* colorGroupsBuffer = nullptr;

	vector<int> mergedInto;
	vector<array<int, 3>> colorStateNextStates;
	vector<int> stateCounts;

	double score = 0;

	struct ColorGroup {
		set<Cell*> cells;
	};

	struct Cell {
		int index = -1;
		vector<int> possibleStates;
		bool stateFixed = false;
		int state = -1;
		int move = -1;
		int bucketIndex = -1;
		ColorGroup* colorGroup = nullptr;
		vector<Cell*> prevPoss;
		Cell* nextPos = nullptr;
		vector<Cell*> prevTimes;
		Cell* nextTime = nullptr;
		vector<pii> nextStateCands; // initial -> final
		int nextState = -1;

		string str() const {
			ostringstream oss;
			oss << "Cell(" << show(index, state, move, bucketIndex, nextState, possibleStates, nextStateCands)
				<< ")";
			return oss.str();
		}
	};

	Compressor(int numStates, int numCells)
		: numStates(numStates), numInitialCells(numCells), possiblePairCounts(numStates, 0),
		  cellsBuffer(new Cell[numCells]), colorGroupsBuffer(new ColorGroup[numCells]),
		  mergedInto(numCells, -1), colorStateNextStates(numCells, {-1, -1, -1}), stateCounts(numStates, 0) {
		int numBuckets = numStates * numStates * 4;
		buckets.resize(numBuckets);
		rep0(i, numCells) {
			auto cell = cellsBuffer + i;
			auto cg = colorGroupsBuffer + i;
			cell->index = i;
			cell->colorGroup = cg;
			cg->cells.insert(cell);
			livingCells.push_back(cell);
		}
	}

	~Compressor() {
		delete[] cellsBuffer;
		delete[] colorGroupsBuffer;
	}

	int computeBucketIndex(const Cell* c) const {
		// [nextState][move][state]
		int res = c->nextState * (numStates * 4) + c->move * numStates + c->state;
		return res;
	}

	int getStateFromBucketIndex(int bucketIndex) const {
		return bucketIndex % numStates;
	}

	int computeNextState(const Cell* c) {
		if (!c->nextTime) {
			return 0; // actually can be any value
		}
		int target = c->nextTime->state;
		for (const auto& [s1, s2] : c->nextStateCands) {
			if (s2 == target) {
				return s1;
			}
		}
		fail() << show(target, c->nextStateCands);
		return -1;
	}

	void onBucketCountChange(int index, int oldNum, int newNum) {
		int state = getStateFromBucketIndex(index);
		score -= sqrt(possiblePairCounts[state]);
		possiblePairCounts[state] -= oldNum * (oldNum - 1) / 2;
		possiblePairCounts[state] += newNum * (newNum - 1) / 2;
		score += sqrt(possiblePairCounts[state]);
	}

	void eraseFromBucket(Cell* c, bool force = false) {
		if (force) {
			assert_ne(c->bucketIndex, -1);
		}
		if (c->bucketIndex == -1)
			return;
		auto& bucket = buckets[c->bucketIndex];
		auto it = find(bucket.begin(), bucket.end(), c);
		assert(it != bucket.end());
		int num = len(bucket);
		bucket.erase(it);
		onBucketCountChange(c->bucketIndex, num, num - 1);
		c->bucketIndex = -1;
	}

	void addToBucket(Cell* c) {
		assert_eq(c->bucketIndex, -1);
		c->bucketIndex = computeBucketIndex(c);
		auto& bucket = buckets[c->bucketIndex];
		int num = len(bucket);
		bucket.push_back(c);
		onBucketCountChange(c->bucketIndex, num, num + 1);
	}

	void updateBucket(Cell* c) {
		eraseFromBucket(c);
		addToBucket(c);
	}

	void changeState(Cell* c, int newState) {
		assert(!c->stateFixed);
		c->state = newState;
		for (Cell* np : c->prevTimes) {
			np->nextState = computeNextState(np);
			updateBucket(np);
		}
		updateBucket(c);
	}

	bool canMergeColorGroups(ColorGroup* cg1, ColorGroup* cg2, Cell* except2 = nullptr) {
		bitset<MAX_STATES> states1;
		bitset<MAX_STATES> states2;
		for (Cell* c : cg1->cells) {
			assert_ne(c->state, -1);
			states1.set(c->state);
		}
		for (Cell* c : cg2->cells) {
			if (c == except2) {
				continue;
			}
			assert_ne(c->state, -1);
			states2.set(c->state);
		}
		return (states1 & states2).none();
	}

	ColorGroup* mergeColorGroups(ColorGroup* cg1, ColorGroup* cg2) {
		assert_ne(cg1, cg2);
		if (len(cg1->cells) < len(cg2->cells)) {
			swap(cg1, cg2);
		}
		for (Cell* c : cg2->cells) {
			c->colorGroup = cg1;
			assert(!cg1->cells.contains(c));
			cg1->cells.insert(c);
		}
		cg2->cells.clear();
		return cg1;
	}

	bool canMergeCells(Cell* c1, Cell* c2) {
		assert_ne(c1, c2);
		assert_eq(c1->state, c2->state);
		auto np1 = c1->nextPos;
		auto np2 = c2->nextPos;
		if (np1 && np2 && np1->colorGroup != np2->colorGroup &&
			!canMergeColorGroups(np1->colorGroup, np2->colorGroup)) {
			return false;
		}
		auto cg1 = c1->colorGroup;
		auto cg2 = c2->colorGroup;
		assert_ne(cg1, cg2);
		if (!canMergeColorGroups(cg1, cg2, c2)) {
			return false;
		}
		if (np1 && (cg1 == np1->colorGroup || cg2 == np1->colorGroup) ||
			np2 && (cg1 == np2->colorGroup || cg2 == np2->colorGroup)) {
			// trace("bruh");
			set<Cell*> all;
			for (auto c : cg1->cells)
				all.insert(c);
			for (auto c : cg2->cells)
				all.insert(c);
			if (np1)
				for (auto c : np1->colorGroup->cells)
					all.insert(c);
			if (np2)
				for (auto c : np2->colorGroup->cells)
					all.insert(c);
			all.erase(c2);
			bool used[MAX_STATES] = {};
			for (auto c : all) {
				assert_ne(c->state, -1);
				if (used[c->state]) {
					// trace("conflict");
					return false;
				}
				used[c->state] = true;
			}
		}
		return true;
	}

	void mergeCells(Cell* c1, Cell* c2) {
		assert_ne(c1, c2);
		assert_eq(c1->state, c2->state);
		assert_eq(c1->nextState, c2->nextState);
		auto np1 = c1->nextPos;
		auto np2 = c2->nextPos;
		if (np1 && np2) {
			if (np1->colorGroup != np2->colorGroup) {
				auto cg = mergeColorGroups(np1->colorGroup, np2->colorGroup);
				np1->colorGroup = cg;
				np2->colorGroup = cg;
			}
			erase(np2->prevPoss, c2);
		} else if (np2) {
			for (auto& c : np2->prevPoss) {
				if (c == c2)
					c = c1;
			}
			c1->nextPos = np2;
		}
		// trace(lambda::map(c1->colorGroup->cells, [](Cell* c) {
		// 	return pii(c->index, c->state);
		// }));
		// trace(lambda::map(c2->colorGroup->cells, [](Cell* c) {
		// 	return pii(c->index, c->state);
		// }));
		c2->colorGroup->cells.erase(c2);
		c1->colorGroup = mergeColorGroups(c1->colorGroup, c2->colorGroup);
		auto nt1 = c1->nextTime;
		auto nt2 = c2->nextTime;
		if (nt1)
			nt1->stateFixed = true;
		if (nt2)
			nt2->stateFixed = true;
		auto& sts1 = c1->possibleStates;
		auto& sts2 = c2->possibleStates;
		erase_if(sts1, [&](int s) {
			return ranges::count(sts2, s) == 0;
		});
		for (auto pp2 : c2->prevPoss) {
			if (pp2 == c2)
				continue;
			pp2->nextPos = c1;
			c1->prevPoss.push_back(pp2);
		}
		for (auto pt2 : c2->prevTimes) {
			if (pt2 == c2)
				continue;
			pt2->nextTime = c1;
			c1->prevTimes.push_back(pt2);
		}
		if (nt1 && nt2) {
			erase(nt2->prevTimes, c2);
		} else if (nt2) {
			for (auto& c : nt2->prevTimes) {
				if (c == c2)
					c = c1;
			}
			c1->nextTime = nt2;
			c1->nextStateCands = c2->nextStateCands;
		}
		// trace(show(c1->str()));
		// trace(show(c2->str()));
		// if (nt1) {
		// 	trace(show(nt1->str()));
		// }
		// if (nt2) {
		// 	trace(show(nt2->str()));
		// }

		if (nt2) {
			assert_eq(ranges::count(nt2->prevTimes, c2), 0);
		}

		c1->stateFixed = c1->stateFixed || c2->stateFixed;
		stateCounts[c2->state]--;
		mergedInto[c2->index] = c1->index;

		auto cands = c2->nextStateCands;
		eraseFromBucket(c2, true);
		int c2i = c2->index;
		*c2 = {};
		c2->index = c2i;
		c2->nextTime = nt2;
		c2->nextStateCands = cands; // keep candidates for sanity check
		erase(livingCells, c2);
	}

	bool canSwapStates(Cell* c1, Cell* c2) {
		assert_ne(c1, c2);
		if (c1->stateFixed || c2->stateFixed) {
			return false;
		}
		int s1 = c1->state;
		int s2 = c2->state;
		assert_ne(s1, s2);
		if (ranges::count(c1->possibleStates, s2) == 0) {
			return false;
		}
		if (ranges::count(c2->possibleStates, s1) == 0) {
			return false;
		}
		if (c1->colorGroup == c2->colorGroup) {
			return true;
		}
		for (auto c : c1->colorGroup->cells) {
			if (c->state == s2) {
				return false;
			}
		}
		for (auto c : c2->colorGroup->cells) {
			if (c->state == s1) {
				return false;
			}
		}
		return true;
	}

	bool canChangeState(Cell* c, int s) {
		assert_ne(c->state, s);
		if (c->stateFixed) {
			return false;
		}
		if (ranges::count(c->possibleStates, s) == 0) {
			return false;
		}
		for (auto cc : c->colorGroup->cells) {
			if (cc->state == s) {
				return false;
			}
		}
		return true;
	}

	void swapStates(Cell* c1, Cell* c2) {
		assert_ne(c1, c2);
		assert(!c1->stateFixed);
		assert(!c2->stateFixed);
		int s1 = c1->state;
		int s2 = c2->state;
		assert_ne(s1, s2);
		changeState(c1, s2);
		changeState(c2, s1);
	}

	void initBuckets() {
		for (auto c : livingCells) {
			assert_ne(c->state, -1);
			assert_ne(c->move, -1);
			stateCounts[c->state]++;
			c->nextState = computeNextState(c);
			updateBucket(c);
		}
	}

	void sanityCheck() {
		vector<bool> isLiving(numInitialCells, false);
		set<ColorGroup*> usedColorGroups;
		for (auto c : livingCells) {
			isLiving[c->index] = true;
			assert_ne(c->state, -1);
			assert_ne(c->bucketIndex, -1);
			auto& bucket = buckets[c->bucketIndex];
			assert_eq(ranges::count(bucket, c), 1);
			assert(c->colorGroup);
			assert(c->colorGroup->cells.contains(c));
			usedColorGroups.insert(c->colorGroup);
			for (auto pp : c->prevPoss) {
				assert_eq(pp->nextPos, c) << show(c->str(), pp->str());
			}
			for (auto pt : c->prevTimes) {
				assert_eq(pt->nextTime, c) << show(c->str(), pt->str());

				int initialSt = pt->nextState;
				int finalSt = c->state;
				bool found = false;
				for (const auto& [s1, s2] : pt->nextStateCands) {
					if (s1 == initialSt && s2 == finalSt) {
						found = true;
						break;
					}
				}
				assert(found) << show(pt->str(), initialSt, finalSt, pt->nextStateCands);
			}
			auto np = c->nextPos;
			auto nt = c->nextTime;
			if (np) {
				assert_eq(ranges::count(np->prevPoss, c), 1);
			}
			if (nt) {
				assert_eq(ranges::count(nt->prevTimes, c), 1);
			}
			assert_eq(ranges::count(c->possibleStates, c->state), 1);
			if (nt) {
				assert_ne(c->nextState, -1);
				bool ok = false;
				for (const auto& [s1, s2] : c->nextStateCands) {
					if (s1 == c->nextState && s2 == nt->state) {
						ok = true;
						break;
					}
				}
				assert(ok) << show(c->nextState, nt->state, c->nextStateCands);
			}
		}
		for (auto cg : usedColorGroups) {
			set<int> usedStates;
			for (auto c : cg->cells) {
				assert(isLiving[c->index]);
				assert(!usedStates.contains(c->state));
				usedStates.insert(c->state);
			}
		}

		rep0(i, numInitialCells) {
			int index = i;
			while (mergedInto[index] != -1) {
				index = mergedInto[index];
			}
			colorStateNextStates[i] = {-1, cellsBuffer[index].state, cellsBuffer[index].nextState};
		}

		rep0(i, numInitialCells) {
			if (!isLiving[i]) {
				auto c = cellsBuffer + i;
				assert_eq(c->state, -1);
				assert_eq(c->move, -1);
				assert_eq(c->nextState, -1);
				assert_eq(c->bucketIndex, -1);
				assert_eq(c->colorGroup, nullptr);
			}
			int index = i;
			while (mergedInto[index] != -1) {
				index = mergedInto[index];
			}
			int initialSt = cellsBuffer[index].nextState;
			if (cellsBuffer[i].nextTime) {
				int finalIndex = cellsBuffer[i].nextTime->index;
				int finalSt = colorStateNextStates[finalIndex][1];
				auto& origCands = cellsBuffer[i].nextStateCands;
				bool found = false;
				for (const auto& [s1, s2] : origCands) {
					if (s1 == initialSt && s2 == finalSt) {
						found = true;
						break;
					}
				}
				assert(found) << show(
					i, index, cellsBuffer[index].str(), initialSt, finalSt, finalIndex, origCands);
			}
		}
	}

	void run(rngen& rng) {
		int st = timer();
		rep0(s, numStates) {
			trace("possible pairs for state ", s, ": ", possiblePairCounts[s]);
		}
		int maxTime = 10;
		rep0(iter, 1000000) {
			if ((iter & 0xff) == 0) {
				if (timer() - st > maxTime)
					break;
			}
			if (rng.next_prob(0.5)) {
				int i;
				int s;
				int trials = 0;
				bool ok = false;
				while (++trials < 100) {
					do {
						i = rng.next_int(len(livingCells));
						s = rng.next_int(numStates);
					} while (livingCells[i]->state == s);
					if (stateCounts[s] < stateCounts[livingCells[i]->state]) {
						ok = true;
						break;
					}
				}
				if (ok && canChangeState(livingCells[i], s)) {
					auto c = livingCells[i];
					// sanityCheck();
					int s1 = s;
					int s2 = c->state;
					changeState(c, s);
					stateCounts[s1]++;
					stateCounts[s2]--;
					// sanityCheck();
				}
			} else {
				int i1;
				int i2;
				do {
					i1 = rng.next_int(len(livingCells));
					i2 = rng.next_int(len(livingCells) - 1);
					if (i2 >= i1)
						i2++;
				} while (livingCells[i1]->state == livingCells[i2]->state);
				auto c1 = livingCells[i1];
				auto c2 = livingCells[i2];
				double pscore = score;
				if (canSwapStates(c1, c2)) {
					// sanityCheck();
					double scoreBefore = score;
					swapStates(c1, c2);
					if (score < scoreBefore && rng.next_prob(0.5)) {
						// revert
						swapStates(c1, c2);
					}
					// trace("swapped states of cells ", c1->index, " and ", c2->index, ": ",
					// 	show(c1->state, c2->state, iter, score));
					// sanityCheck();
				}
			}
			if ((iter & 0xff) == 0) {
				// trace(show(iter, score));
				// trace(possiblePairCounts);
				auto tryToMergeOn = [&](int s) {
					rep0(m, 4) {
						rep0(ns, numStates) {
							int bidx = ns * (numStates * 4) + m * numStates + s;
							auto& bucket = buckets[bidx];
							rep0(i, len(bucket)) {
								auto c1 = bucket[i];
								rep0(j, i) {
									auto c2 = bucket[j];
									if (canMergeCells(c1, c2)) {
										// trace("can merge cells ", c1->index, " and ", c2->index, ": ",
										// 	show(s, m, ns, iter));
										mergeCells(c1, c2);
										// sanityCheck();
										return true;
									}
								}
							}
						}
					}
					return false;
				};
				int maxCount = -1;
				rep0(s, numStates) {
					maxeq(maxCount, stateCounts[s]);
				}
				rep0(s, numStates) {
					if (stateCounts[s] < maxCount)
						continue;
					tryToMergeOn(s);
				}
			}
		}
		trace("---end---");

		// assign colors
		set<ColorGroup*> usedColorGroups;
		for (auto c : livingCells) {
			int size = len(c->colorGroup->cells);
			if (size <= 1) {
				assert_eq(size, 1);
				continue;
			}
			usedColorGroups.insert(c->colorGroup);
		}
		// for (auto cg : usedColorGroups) {
		// 	bitset<MAX_STATES> usedStates;
		// 	for (auto c : cg->cells) {
		// 		assert_ne(c->state, -1);
		// 		usedStates.set(c->state);
		// 	}
		// 	trace(tos(usedStates).substr(MAX_STATES - numStates, numStates));
		// }
		bool used[MAX_STATES][MAX_STATES] = {};
		vector<int> colors(numInitialCells, -1);
		int maxColor = -1;
		for (auto cg : usedColorGroups) {
			rep0(color, MAX_STATES) {
				bool canUse = true;
				for (auto c : cg->cells) {
					if (used[c->state][color]) {
						canUse = false;
						break;
					}
				}
				if (canUse) {
					for (auto c : cg->cells) {
						used[c->state][color] = true;
						colors[c->index] = color;
					}
					maxeq(maxColor, color);
					// trace("assign color ", color, " to group of size ", len(cg->cells));
					// for (auto c : cg->cells) {
					// 	trace("  cell ", c->index, " state ", c->state);
					// }
					break;
				}
			}
		}
		// assign the rest
		for (auto c : livingCells) {
			int i = c->index;
			int s = c->state;
			if (colors[i] != -1)
				continue;
			rep0(color, MAX_STATES) {
				if (!used[s][color]) {
					used[s][color] = true;
					colors[i] = color;
					maxeq(maxColor, color);
					break;
				}
			}
			assert_ne(colors[i], -1);
		}
		trace("used colors up to ", maxColor);
		rep0(i, maxColor + 1) {
			string line;
			rep0(s, numStates) {
				line += used[s][i] ? "@" : "_";
			}
			trace(line);
		}
		// finalize
		rep0(i, numInitialCells) {
			int index = i;
			while (mergedInto[index] != -1) {
				index = mergedInto[index];
			}
			colorStateNextStates[i] = {colors[index], cellsBuffer[index].state, cellsBuffer[index].nextState};
		}
		int en = timer();
		trace("compression time: ", en - st, " ms");
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

	Solver() : rng(123456) {
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
			ofstream out(output_file_name(seed));
			on_abort = [&]() {
				mov.close_file();
				out.close();
			};
			solveMain(out);
			mov.close_file();
			out.close();
		} else {
			solveMain(cout);
		}
	}

private:
	int n;
	int n2;
	int numT;
	int maxS;
	vector<ivec2> unpack;

	void init() {
		n = p.n;
		n2 = n * n;
		numT = p.numT;
		maxS = p.maxS;
		rep2() {
			unpack.push_back(ivec2(i, j));
		}
	}

	vector<vector<array<int, 3>>> computeDistMat(
		int numV, const vector<array<int, 2>> edges[MAX_N2], bool compute_prev) {
		// ------------ Prerequisites ------------
		constexpr int MAX_VS = 512;
		constexpr int MAX_DEG = 16;
		constexpr int MAX_ES = MAX_VS * MAX_DEG;
		static_assert(MAX_VS % 32 == 0, "MAX_VS must be multiple of 32");

		const uint8_t INF_W = 255;

		constexpr int SIMD_LANES = 32; // AVX2: 8bit × 32 lanes
		constexpr int PACK = 64; // distance BFS: process 64 sources at once (64-bit mask)

		assert(numV <= MAX_VS);

		// ------------ CSR (static buffers) ------------
		alignas(32) static int off[MAX_VS + 1];
		alignas(32) static int to[MAX_ES];
		alignas(32) static uint8_t w[MAX_ES];

		for (int i = 0; i <= numV; ++i)
			off[i] = 0;
		int m = 0;
		rep0(u, numV) m += (int) edges[u].size();
		assert(m <= MAX_ES);

		int cur = 0;
		rep0(u, numV) {
			off[u] = cur;
			for (const auto& e : edges[u]) {
				to[cur] = e[0];
				w[cur] = (uint8_t) e[1];
				++cur;
			}
		}
		off[numV] = cur;

		// ------------ Return buffer (distance, weight-sum, predecessor) ------------
		vector<vector<array<int, 3>>> result(numV, vector<array<int, 3>>(numV, {INF, -1, -1}));

		// ------------ (1) Distances: bit-parallel BFS for 64 sources at once ------------
		alignas(32) static int distMat[MAX_VS][MAX_VS];
		alignas(32) static uint64_t visited[MAX_VS];
		alignas(32) static uint64_t frontier[MAX_VS];
		alignas(32) static uint64_t nextFrontier[MAX_VS];

		rep0(s, numV) rep0(v, numV) {
			distMat[s][v] = INF;
		}

		for (int base = 0; base < numV; base += PACK) {
			const int lanes = std::min(PACK, numV - base);
			const uint64_t ACTIVE = (lanes == 64) ? ~0ull : ((1ull << lanes) - 1ull);

			rep0(v, numV) {
				visited[v] = frontier[v] = nextFrontier[v] = 0ull;
			}

			rep0(i, lanes) {
				int s = base + i;
				frontier[s] |= (1ull << i);
				visited[s] |= (1ull << i);
				distMat[s][s] = 0;
			}

			for (int level = 1;; ++level) {
				rep0(v, numV) nextFrontier[v] = 0ull;

				rep0(u, numV) {
					uint64_t mu = frontier[u] & ACTIVE;
					if (!mu)
						continue;
					rep(e, off[u], off[u + 1]) {
						nextFrontier[to[e]] |= mu;
					}
				}

				uint64_t any = 0ull;
				rep0(v, numV) {
					uint64_t newmask = nextFrontier[v] & (~visited[v]) & ACTIVE;
					if (!newmask)
						continue;

					// For each set bit in newmask, set distance
					uint64_t t = newmask;
					while (t) {
						unsigned int i = std::countr_zero(t); // 0..63
						int s = base + (int) i;
						distMat[s][v] = level;
						t &= (t - 1);
					}
					visited[v] |= newmask;
					any |= newmask;
				}
				if (!any)
					break;
				rep0(v, numV) frontier[v] = nextFrontier[v];
			}
		}

		// write distances to result
		rep0(s, numV) rep0(v, numV) {
			result[s][v][0] = distMat[s][v];
		}

		// ------------ (2) Weight sums + (optionally) predecessors: level-synchronous DP for 32 sources
		// (AVX2) ------------
		alignas(32) static __m256i V255 = _mm256_set1_epi8((char) 0xFF);
		alignas(32) static __m256i BIAS = _mm256_set1_epi8((char) 0x80); // for unsigned comparison

		// Work buffers (static)
		alignas(32) static __m256i W[MAX_VS]; // per-vertex lane weights (8bit saturating)
		alignas(32) static int
			Pred[MAX_VS][SIMD_LANES]; // per-vertex per-lane predecessor (used only when compute_prev==true)
		alignas(32) static __m256i maskVec[MAX_VS + 1][MAX_VS]; // [level][v] 32-byte masks
		alignas(32) static int U_level[MAX_VS][MAX_VS]; // vertex lists per level
		alignas(32) static int U_count[MAX_VS];
		alignas(32) static int distBatch[SIMD_LANES][MAX_VS];

		// Epoch array to track whether a mask/level has been built for this batch
		alignas(32) static int tagLV[MAX_VS + 1][MAX_VS]; // [level][v] last batch ID
		static int batchEpoch = 1; // 0 is unused

		// Precomputed vectors where exactly one byte is 0xFF for each lane
		alignas(32) static __m256i BYTE_MASK[32];
		static bool byteMaskInited = false;
		if (!byteMaskInited) {
			rep0(i, 32) {
				alignas(32) uint8_t tmp[32] = {};
				tmp[i] = 0xFF;
				BYTE_MASK[i] = _mm256_load_si256(reinterpret_cast<const __m256i*>(tmp));
			}
			byteMaskInited = true;
		}

		for (int base = 0; base < numV; base += SIMD_LANES, batchEpoch++) {
			const int lanes = std::min(SIMD_LANES, numV - base);
			if (batchEpoch == 0) { // wrap-around protection (unlikely but safe)
				std::fill(&tagLV[0][0], &tagLV[0][0] + (MAX_VS + 1) * MAX_VS, 0);
				batchEpoch = 1;
			}

			int maxLevel = 0;
			rep0(i, lanes) {
				int s = base + i;
				rep0(v, numV) {
					int dv = distMat[s][v];
					distBatch[i][v] = dv;
					if (dv < INF)
						maxLevel = std::max(maxLevel, dv);
				}
			}

			rep0(l, maxLevel + 1) {
				U_count[l] = 0;
			}

			// Build maskVec and U_level using epoch method
			rep0(i, lanes) {
				const __m256i bi = BYTE_MASK[i];
				rep0(v, numV) {
					int l = distBatch[i][v];
					if (l >= INF)
						continue;
					if (tagLV[l][v] != batchEpoch) {
						tagLV[l][v] = batchEpoch;
						maskVec[l][v] = _mm256_setzero_si256();
						U_level[l][U_count[l]++] = v;
					}
					maskVec[l][v] = _mm256_or_si256(maskVec[l][v], bi);
				}
			}

			// Initialize W and Pred (Pred only if compute_prev)
			rep0(v, numV) {
				W[v] = V255;
				if (compute_prev) {
					rep0(i, lanes) Pred[v][i] = -1;
				}
			}
			// Set source lanes to 0 (only touch Pred if compute_prev==true)
			rep0(i, lanes) {
				int s = base + i;
				alignas(32) uint8_t wtmp[32];
				_mm256_store_si256(reinterpret_cast<__m256i*>(wtmp), W[s]);
				wtmp[i] = 0;
				W[s] = _mm256_load_si256(reinterpret_cast<__m256i*>(wtmp));
				if (compute_prev)
					Pred[s][i] = -1;
			}

			// ---------- Level-synchronous DP (level -> level+1) ----------
			if (compute_prev) {
				// Path that also updates predecessors (no branch inside hot loop)
				rep0(level, maxLevel + 1) {
					const int cnt = U_count[level];
					rep0(ui, cnt) {
						int u = U_level[level][ui];

						const __m256i mu = maskVec[level][u];
						if (_mm256_testz_si256(mu, mu))
							continue;

						const __m256i su = W[u];
						rep(e, off[u], off[u + 1]) {
							int v = to[e];

							__m256i mv = (tagLV[level + 1][v] == batchEpoch) ? maskVec[level + 1][v]
																			 : _mm256_setzero_si256();
							__m256i mm = _mm256_and_si256(mu, mv);
							if (_mm256_testz_si256(mm, mm))
								continue;

							const __m256i wvec = _mm256_set1_epi8((char) w[e]);
							__m256i cand = _mm256_adds_epu8(su, wvec);

							// Set lanes outside mask to 0xFF (invalidate)
							__m256i notmm = _mm256_andnot_si256(mm, V255);
							cand = _mm256_or_si256(cand, notmm);

							__m256i Wv = W[v];
							// Compare cand < Wv (unsigned) -> extract lanes to update
							__m256i x = _mm256_xor_si256(cand, BIAS);
							__m256i y = _mm256_xor_si256(Wv, BIAS);
							__m256i lt = _mm256_cmpgt_epi8(y, x); // y > x
							__m256i um = _mm256_and_si256(lt, mm); // lanes to update

							// Value update
							W[v] = _mm256_min_epu8(Wv, cand);

							// Predecessor update
							unsigned int maskBits = (unsigned int) _mm256_movemask_epi8(um);
							while (maskBits) {
								unsigned int lane = std::countr_zero(maskBits);
								Pred[v][lane] = u;
								maskBits &= (maskBits - 1);
							}
						}
					}
				}
			} else {
				// Path that does not update predecessors (minimal ops)
				rep0(level, maxLevel + 1) {
					const int cnt = U_count[level];
					rep0(ui, cnt) {
						int u = U_level[level][ui];

						const __m256i mu = maskVec[level][u];
						if (_mm256_testz_si256(mu, mu))
							continue;

						const __m256i su = W[u];
						rep(e, off[u], off[u + 1]) {
							int v = to[e];

							__m256i mv = (tagLV[level + 1][v] == batchEpoch) ? maskVec[level + 1][v]
																			 : _mm256_setzero_si256();
							__m256i mm = _mm256_and_si256(mu, mv);
							if (_mm256_testz_si256(mm, mm))
								continue;

							const __m256i wvec = _mm256_set1_epi8((char) w[e]);
							__m256i cand = _mm256_adds_epu8(su, wvec);

							// Set lanes outside mask to 0xFF (invalidate)
							__m256i notmm = _mm256_andnot_si256(mm, V255);
							cand = _mm256_or_si256(cand, notmm);

							W[v] = _mm256_min_epu8(W[v], cand);
						}
					}
				}
			}

			// Write-back (weight sum & predecessor if needed)
			rep0(v, numV) {
				alignas(32) uint8_t wtmp[32];
				_mm256_store_si256(reinterpret_cast<__m256i*>(wtmp), W[v]);
				rep0(i, lanes) {
					int s = base + i;
					if (distMat[s][v] == INF) {
						result[s][v][1] = INF;
						if (compute_prev)
							result[s][v][2] = -1;
					} else {
						result[s][v][1] = (int) wtmp[i]; // 0..255
						if (compute_prev)
							result[s][v][2] = Pred[v][i];
						// When compute_prev==false, the third element can remain undefined
					}
				}
			}
		}
		return result;
	}

	// dist, actual steps
	ScoreInfo computeScore(const vector<Cell>& cells, vector<int>& fullPath, bool omitPath, bool verbose) {
		vector<int> indices(n2, -1);
		vector<int> posis;
		struct Edge {
			int to; // vertex index
			vector<int> slideDirs; // directions taken while sliding
			vector<pii> passes; // passed cells (position, entry direction)
		};
		static array<Edge, MAX_N2 * 12> edgeBuffer;
		int edgeBufferSize = 0;
		auto newEdge = [&](int to) {
			assert_lt(edgeBufferSize, MAX_N2 * 12);
			auto& res = edgeBuffer[edgeBufferSize++];
			res.to = to;
			res.slideDirs.clear();
			res.passes.clear();
			return &res;
		};
		static array<vector<Edge*>, MAX_N2> edgess;
		struct PassInfo {
			int v; // from vertex index
			int ei; // edge index
			int stepi; // step index
			int dir; // entry direction
		};
		static array<vector<PassInfo>, MAX_N2> passedBy;
		rep0(i, n2) {
			edgess[i].clear();
			passedBy[i].clear();
		}
		int numV = 0;
		rep0(i, n2) {
			if (cells[i].stop()) {
				indices[i] = numV++;
				posis.push_back(i);
			}
		}

		struct StepResult {
			int posi;
			int dir;

			bool failed() const {
				return posi == -1;
			}
		};

		auto computeStep = [&](int posi, int dir) -> StepResult {
			assert_ne(dir, -1);
			auto& cell = cells[posi];
			if (cell.stop()) {
				return {-1, -1};
			}
			int newDir = cell.nextDir(dir);
			assert_ne(newDir, -1);
			if (p.walls[posi][newDir]) {
				return {-1, -1}; // hit a wall
			}
			ivec2 npos = unpack[posi] + ivec2::dir(newDir);
			int nposi = npos.pack(n);
			return {nposi, newDir};
		};

		static StepResult stepPos[MAX_N2 * 4];
		rep0(i, n2) {
			rep0(dir, 4) {
				stepPos[i * 4 + dir] = computeStep(i, dir);
			}
		}

		// build edges
		rep0(v, numV) {
			int posi = posis[v];
			ivec2 pos = unpack[posi];
			auto& edges = edgess[v];
			rep0(dir, 4) {
				if (p.walls[posi][dir])
					continue;
				ivec2 npos = pos + ivec2::dir(dir);
				int nposi = npos.pack(n);
				if (cells[nposi].stop()) {
					int ei = len(edges);
					auto edge = newEdge(indices[nposi]);
					edge->passes.emplace_back(nposi, dir);
					edges.push_back(edge);
					continue;
				}
				// start sliding
				ivec2 slidePos = npos;
				rep0(firstDir, 4) {
					string dir; // for debug
					int currentDir = firstDir;
					ivec2 currentPos = slidePos;
					int currentPosi = currentPos.pack(n);
					auto edge = newEdge(-1);
					auto& dirs = edge->slideDirs;
					dirs.push_back(currentDir);
					auto& passes = edge->passes;
					passes.emplace_back(currentPosi, currentDir);
					bool ng = false;

					static fast_iset<MAX_N2 * 4> visitedPosiDir;
					visitedPosiDir.clear();

					while (!cells[currentPosi].stop()) {
						auto stepRes = stepPos[currentPosi * 4 + currentDir];
						if (stepRes.failed()) {
							// hit a wall, fail
							ng = true;
							break;
						}
						if (currentDir != stepRes.dir) {
							currentDir = stepRes.dir;
							dirs.push_back(currentDir);
						}
						currentPosi = stepRes.posi;
						currentPos = unpack[currentPosi];
						passes.emplace_back(currentPosi, currentDir);
						if (visitedPosiDir.contains(currentPosi * 4 + currentDir)) {
							// loop detected, fail
							ng = true;
							break;
						}
						visitedPosiDir.insert(currentPosi * 4 + currentDir);
					}
					if (ng) {
						passes.clear();
						continue;
					}
					assert(cells[currentPosi].stop());
					int ei = len(edges);
					static fast_iset<MAX_N2> visited;
					visited.clear();
					rep0(stepi, len(passes)) {
						auto [passPosi, entryDir] = passes[stepi];
						if (passPosi == posi || passPosi == currentPosi)
							continue; // do not count start/end positions
						if (visited.contains(passPosi))
							continue;
						visited.insert(passPosi);
						passedBy[passPosi].emplace_back(v, ei, stepi, entryDir);
					}
					edge->to = indices[currentPosi];
					edges.push_back(edge);
				}
			}
		}

		static vector<array<int, 2>> edgeList[MAX_N2];
		rep0(v, numV) {
			edgeList[v].clear();
			for (const auto e : edgess[v]) {
				edgeList[v].push_back({e->to, len(e->passes)});
			}
		}
		int st = timer();
		auto distMat = computeDistMat(numV, edgeList, !omitPath);
		int en = timer();
		if (verbose)
			trace("distMat time: ", en - st, " ms");

		auto step = [&](int posi, int dir) {
			assert_ne(dir, -1);
			assert(!p.walls[posi][dir]);
			ivec2 pos = unpack[posi];
			ivec2 npos = pos + ivec2::dir(dir);
			return npos.pack(n);
		};

		struct Segment {
			int prevDir = -1; // -1: stopped, 0..3: slide direction
			int dist = 0;
			int steps = 0;
			int lastVertex = -1;
			int lastEdge = -1;
		};
		auto printSeg = [&](const Segment& seg) {
			ostringstream os;
			os << "{pdir: " << seg.prevDir << ", dist: " << seg.dist << ", steps: " << seg.steps
			   << ", v: " << (seg.lastVertex == -1 ? ivec2(-1, -1) : unpack[posis[seg.lastVertex]])
			   << ", e: " << seg.lastEdge << "}";
			return os.str();
		};

		vector<array<Segment, 5>> segments;

		auto computeNextSegments = [&](int fromPosi, int toPosi, const array<Segment, 5>& prevSegments) {
			array<Segment, 5> segments;
			segments.fill({-1, INF, INF, -1, -1});
			auto offer = [&](int prevDir, int dir, int dist, int steps, int vertex, int edge) {
				auto& seg = segments[dir + 1];
				if (dist < seg.dist || dist == seg.dist && steps < seg.steps) {
					seg = {prevDir, dist, steps, vertex, edge};
				}
			};
			rep0(di, 5) {
				int dist = prevSegments[di].dist;
				int steps = prevSegments[di].steps;
				if (dist == INF)
					continue;
				int entryDir = di - 1;
				int posi = fromPosi;
				if (entryDir != -1) {
					bool done = false;
					int currentDir = entryDir;
					while (!cells[posi].stop()) {
						if (posi == toPosi) {
							// reached the target while sliding
							offer(entryDir, currentDir, dist, steps, -1, -1);
							done = true;
							break;
						}
						auto stepRes = stepPos[posi * 4 + currentDir];
						assert(!stepRes.failed());
						posi = stepRes.posi;
						currentDir = stepRes.dir;
						steps++;
					}
					if (done)
						continue;
				}
				int fromv = indices[posi];
				assert_ne(fromv, -1);
				// now at a stop cell
				if (cells[toPosi].stop()) {
					// can stop at the target
					int v = indices[toPosi];
					assert_ne(v, -1);
					auto [ddist, dsteps, _] = distMat[fromv][v];
					// move to v directly
					offer(entryDir, -1, dist + ddist, steps + dsteps, v, -1);
				} else {
					// need to slide
					for (auto [v, ei, stepi, nextEntryDir] : passedBy[toPosi]) {
						// first move to v then use edge ei to slide
						auto [ddist, dsteps, _] = distMat[fromv][v];
						// +1 for the first step
						ddist += 1;
						dsteps += stepi + 1;
						offer(entryDir, nextEntryDir, dist + ddist, steps + dsteps, v, ei);
					}
				}
			}
			return segments;
		};

		array<Segment, 5> initialSegments; // prev: last landed position index
		initialSegments.fill({-1, INF, INF, -1, -1});
		array<int, 4> initialVertices; // the first landed vertex index for each dir
		initialVertices.fill(-1);
		array<vector<int>, 4> initialPaths; // the first path for each dir
		array<vector<int>, 4> initialDirs; // the first slide directions for each dir
		if (cells[p.spos.pack(n)].stop()) {
			initialSegments[0] = {-1, 0, 0, -1, -1}; // standing still
		} else {
			// try all initial slide directions
			rep0(firstDir, 4) {
				ivec2 pos = p.spos;
				int posi = pos.pack(n);
				int currentDir = firstDir;
				int numSteps = 0;
				auto& path = initialPaths[firstDir];
				auto& dirs = initialDirs[firstDir];
				dirs.push_back(currentDir);
				while (!cells[posi].stop()) {
					auto stepRes = stepPos[posi * 4 + currentDir];
					if (stepRes.failed()) {
						// hit a wall
						break;
					}
					posi = stepRes.posi;
					if (currentDir != stepRes.dir) {
						currentDir = stepRes.dir;
						dirs.push_back(currentDir);
					}
					path.push_back(posi);
					if (++numSteps > maxS) {
						// infinite loop protection
						break;
					}
				}
				if (cells[posi].stop()) {
					initialVertices[firstDir] = indices[posi];
					initialSegments[firstDir + 1] = {-1, 0, 0, -1, -1};
				} else {
					path.clear();
					dirs.clear();
				}
			}
		}

		auto drawField = [&](int focus = -1) {
			// draw non-ice cells
			mov.no_stroke();
			mov.fill(0.5);
			rep2() {
				ivec2 pos(i, j);
				int posi = pos.pack(n);
				if (cells[posi].stop()) {
					mov.rect(pos.j, pos.i, 1, 1);
				}
				if (cells[posi].rot()) {
					mov.stroke(1);
					mov.stroke_weight(0.1);
					if (cells[posi].kind == CELL_ROT1) {
						mov.line(pos.j + 0.2, pos.i + 0.2, pos.j + 0.8, pos.i + 0.8);
					} else {
						mov.line(pos.j + 0.2, pos.i + 0.8, pos.j + 0.8, pos.i + 0.2);
					}
					mov.no_stroke();
				}
			}
			if (focus != -1) {
				ivec2 pos = unpack[focus];
				mov.fill(0, 1, 0);
				mov.rect(pos.j, pos.i, 1, 1);
			}
			// draw walls
			mov.stroke(1);
			mov.stroke_weight(0.1);
			rep2() {
				ivec2 pos(i, j);
				int posi = pos.pack(n);
				rep0(dir, 4) {
					if (p.walls[posi][dir]) {
						double x1 = pos.j + 0.5;
						double y1 = pos.i + 0.5;
						double x2 = x1 + ivec2::dir(dir).j;
						double y2 = y1 + ivec2::dir(dir).i;
						double mx = (x1 + x2) / 2;
						double my = (y1 + y2) / 2;
						// rotate 90 deg around midpoint
						double rx1 = mx - (y1 - my);
						double ry1 = my + (x1 - mx);
						double rx2 = mx - (y2 - my);
						double ry2 = my + (x2 - mx);
						mov.line(rx1, ry1, rx2, ry2);
					}
				}
			}
		};

		auto prevSegments = initialSegments;
		vector<array<Segment, 5>> paths;
		int posi = p.spos.pack(n);
		// if (verbose)
		// 	trace(
		// 		"initial segments = ", lambda::map(prevSegments, printSeg) | mods::br(), ", pos = ", p.spos);
		rep0(ti, numT) {
			int targetPosi = p.targets[ti].pack(n);
			prevSegments = computeNextSegments(posi, targetPosi, prevSegments);
			paths.push_back(prevSegments);
			// if (verbose)
			// 	trace("after reaching target ", p.targets[ti],
			// 		", segments = ", lambda::map(prevSegments, printSeg) | mods::br());
			posi = targetPosi;
		}
		ScoreInfo res;
		res.dist = INF;
		res.steps = INF;
		int lastDir = -1;
		rep0(di, 5) {
			if (mineq(res.dist, prevSegments[di].dist)) {
				res.steps = prevSegments[di].steps;
				lastDir = di - 1;
			}
		}
		// trace("total minDist = ", minDist, ", steps = ", steps);

		if (res.dist == INF || omitPath) {
			res.score = res.dist;
			return res;
		}

		static vector<array<int, 2>> departures; // list of (vertex index, edge index)
		static vector<int> entryDirs; // entry directions for each target
		departures.clear();
		entryDirs.clear();
		int curDir = lastDir;
		rep0r(ti, numT) {
			auto& segment = paths[ti][curDir + 1];
			entryDirs.push_back(curDir);
			curDir = segment.prevDir;
		}
		int firstDir = curDir;
		ranges::reverse(entryDirs);
		int v = -1;

		// find the first vertex
		int sposi = p.spos.pack(n);
		if (firstDir == -1) {
			v = indices[sposi];
			assert_ne(v, -1);
		} else {
			departures.push_back({-1, firstDir});
			v = initialVertices[firstDir];
			assert_ne(v, -1);
		}

		rep0(ti, numT) {
			int targetPosi = p.targets[ti].pack(n);
			auto& segment = paths[ti][entryDirs[ti] + 1];
			int nv = segment.lastVertex;
			int e = segment.lastEdge;
			if (nv != -1) {
				// find the path from v to nv
				static vector<array<int, 2>> subpath;
				subpath.clear();
				{
					int cv = nv;
					while (cv != v) {
						auto [ddist, dweight, pv] = distMat[v][cv];
						assert_ne(pv, -1);
						// find which edge from pv to cv
						int ei = -1;
						int minSteps = INF;
						auto& edges = edgess[pv];
						rep0(i, len(edges)) {
							if (edges[i]->to == cv) {
								if (mineq(minSteps, len(edges[i]->passes))) {
									ei = i;
								}
							}
						}
						assert_ne(ei, -1);
						subpath.push_back({pv, ei});
						cv = pv;
					}
					ranges::reverse(subpath);
				}
				for (auto& ve : subpath) {
					departures.push_back(ve);
				}
				v = nv;
				if (e != -1) {
					departures.push_back({nv, e});
					v = edgess[nv][e]->to;
				}
			}
		}
		fullPath.clear();
		fullPath.push_back(p.spos.pack(n));

		auto getLastRotKind = [&](const vector<int>& dirs) {
			if (len(dirs) < 2)
				return -1;
			int d1 = dirs[len(dirs) - 2];
			int d2 = dirs[len(dirs) - 1];
			if (Cell(CELL_ROT1).nextDir(d1) == d2)
				return 0;
			if (Cell(CELL_ROT2).nextDir(d1) == d2)
				return 1;
			fail() << show(dirs);
			return -1;
		};

		for (auto [v, e] : departures) {
			if (v == -1) {
				// initial slide
				int dir = e;
				for (int pi : initialPaths[dir]) {
					fullPath.push_back(pi);
				}
				int entryDir = initialDirs[dir].back();
				res.entryCounts[entryDir + 1]++;
				int rot = getLastRotKind(initialDirs[dir]);
				if (rot != -1) {
					res.indirectEntryCounts[rot][entryDir]++;
				}
			} else {
				auto& edge = edgess[v][e];
				int entryDir = edge->slideDirs.empty() ? -1 : edge->slideDirs.back();
				res.entryCounts[entryDir + 1]++;
				int rot = getLastRotKind(edge->slideDirs);
				if (rot != -1) {
					res.indirectEntryCounts[rot][entryDir]++;
				}
				for (auto [pi, _] : edge->passes) {
					fullPath.push_back(pi);
				}
			}
		}

		int nextTargetIndex = 0;
		if (render && verbose) {
			mov.target("final_path");
			rep0(i, len(fullPath)) {
				drawField(fullPath[i]);
				if (nextTargetIndex < numT && fullPath[i] == p.targets[nextTargetIndex].pack(n)) {
					nextTargetIndex++;
				}
				if (nextTargetIndex < numT) {
					mov.no_stroke();
					mov.fill(1, 0, 0);
					ivec2 pos = p.targets[nextTargetIndex];
					mov.circle(pos.j + 0.5, pos.i + 0.5, 0.3);
					mov.comment("t = " + tos(nextTargetIndex) + "/" + tos(numT) + " at " +
						tos(p.targets[nextTargetIndex]) + " steps = " + tos(i));
				} else {
					mov.comment("t = " + tos(nextTargetIndex) + "/" + tos(numT) + " steps = " + tos(i));
				}
				mov.end_frame();
			}
		}

		if (render && verbose) {
			// render the path
			mov.target("path");
			rep0(ti, numT) {
				int targetPosi = p.targets[ti].pack(n);
				auto& segment = paths[ti][entryDirs[ti] + 1];
				drawField(targetPosi);
				int v = segment.lastVertex;
				int e = segment.lastEdge;
				if (v != -1) {
					mov.no_stroke();
					mov.fill(1, 0, 0);
					ivec2 pos = unpack[posis[v]];
					mov.circle(pos.j + 0.5, pos.i + 0.5, 0.3);
					if (e != -1) {
						auto& edge = edgess[v][e];
						mov.fill(0, 0, 1);
						for (auto [pi, _] : edge->passes) {
							ivec2 ppos = unpack[pi];
							mov.circle(ppos.j + 0.5, ppos.i + 0.5, 0.2);
						}
					}
				}
				mov.end_frame();
			}
		}

		auto mix = [](array<int, 4> arr, const array<double, 4>& w) {
			ranges::sort(arr);
			return (arr[0] * w[0] + arr[1] * w[1] + arr[2] * w[2] + arr[3] * w[3]) /
				(w[0] + w[1] + w[2] + w[3]);
		};

		double maxIndirect0 = mix(res.indirectEntryCounts[0], {1, 3, 10, 1000});
		double maxIndirect1 = mix(res.indirectEntryCounts[1], {1, 3, 10, 1000});
		double maxIndirect = max(max(maxIndirect0, maxIndirect1), min(maxIndirect0, maxIndirect1) * 1.2);
		int deducted = res.entryCounts[0];
		array<int, 4> counts = {
			res.entryCounts[1], res.entryCounts[2], res.entryCounts[3], res.entryCounts[4]};
		double minEntry = mix(counts, {1000, 10, 3, 1});
		double penalty = max(0.0, maxIndirect - minEntry);
		if (verbose) {
			trace(show(res.entryCounts, res.indirectEntryCounts));
			trace(show(deducted, minEntry, maxIndirect, penalty));
		}
		// weight larger values a bit more
		res.score = deducted + mix(counts, {1.0, 1.1, 1.2, 1.3}) * 4 + penalty * 4;

		return res;
	}

	Solution constructRules(const vector<Cell>& cells, const vector<int>& fullPath, bool verbose) {
		Solution res;
		int firstSlideDir = -1;
		{
			auto& cell = cells[fullPath[0]];
			if (cell.stop()) {
				firstSlideDir = 0; // arbitrary
			} else {
				int actualDir = (unpack[fullPath[1]] - unpack[fullPath[0]]).dir_index();
				rep0(dir, 4) {
					if (cell.nextDir(dir) == actualDir) {
						firstSlideDir = dir;
						break;
					}
				}
				assert_ne(firstSlideDir, -1);
			}
		}

		struct UniqueState {
			int posi;
			int move;
			int entryDir;
			bool free;
			int nextIndexTime; // next unique state index in time order
			int nextIndexPos; // next unique state index at the same position
			vector<int> rotsBeforeEntry;
		};

		constexpr int MAX_UNIQUE_STATES = 4096;
		static array<UniqueState, MAX_UNIQUE_STATES> uniqueStates;

		int numUniqueStates = 0;
		array<int, 5> entryCounts = {};
		array<int, 4> indirectEntryCounts = {};
		vector<int> firstUniqueStateIndex(n2, -1);
		vector<int> rotHistory;
		array<array<vector<int>, 4>, 2> usedBy; // [rot][entry]

		{
			vector<int> lastUniqueStateIndicesAt(n2, -1);
			rep0(i, len(fullPath)) {
				auto& cell = cells[fullPath[i]];
				if (cell.rot()) {
					rotHistory.push_back(cell.kind == CELL_ROT1 ? 0 : 1);
				}
				if (cell.stop()) {
					auto& ust = uniqueStates[numUniqueStates];
					int posi = fullPath[i];
					int nposi = i == len(fullPath) - 1 ? posi : fullPath[i + 1];
					ivec2 pos = unpack[posi];
					ivec2 npos = unpack[nposi];
					int moveDir = posi == nposi ? -1 : (npos - pos).dir_index();
					int entry = -1;
					if (i > 0 && !cells[fullPath[i - 1]].stop()) {
						ivec2 ppos = unpack[fullPath[i - 1]];
						entry = (pos - ppos).dir_index();
					}
					entryCounts[entry + 1]++;

					ust.move = moveDir;
					ust.entryDir = entry;
					ust.rotsBeforeEntry = rotHistory;
					ust.posi = posi;
					ust.free = rotHistory.empty();
					ust.nextIndexPos = -1;
					ust.nextIndexTime = -1;

					if (firstUniqueStateIndex[posi] == -1)
						firstUniqueStateIndex[posi] = numUniqueStates;
					if (!ust.free) {
						assert_ne(entry, -1);
						indirectEntryCounts[entry]++;
						usedBy[rotHistory.back()][entry].push_back(numUniqueStates);
					}
					if (numUniqueStates > 0) {
						auto& prevUst = uniqueStates[numUniqueStates - 1];
						prevUst.nextIndexTime = numUniqueStates;
					}
					if (lastUniqueStateIndicesAt[posi] != -1) {
						auto& prevUst = uniqueStates[lastUniqueStateIndicesAt[posi]];
						prevUst.nextIndexPos = numUniqueStates;
					}
					lastUniqueStateIndicesAt[posi] = numUniqueStates;

					numUniqueStates++;
					rotHistory.clear();
				}
			}
		}
		if (verbose) {
			trace(show(numUniqueStates));
			trace(show(entryCounts, indirectEntryCounts));
			trace(lambda::map(usedBy, [&](const array<vector<int>, 4>& arr) {
				return lambda::map(arr, [&](const vector<int>& v) {
					return len(v);
				});
			}));
		}

		auto tryWithColors = [&](int numColors, bool complete) {
			struct Assignment {
				int color = -1;
				int state = -1;
				int firstState = -1;
			};
			static vector<Assignment> assignments;
			assignments.assign(numUniqueStates, {});

			constexpr int MAX_STATES = 512;
			array<int, MAX_STATES> stateToDir;

			array<array<set<int>, 4>, 2> requiredOutStates;

			int color = 0;
			int state = 0;
			int totalIncr = 0;
			rep0(dir, 4) {
				auto incr = [&]() {
					color++;
					if (color >= numColors) {
						state++;
						color = 0;
					}
					totalIncr++;
				};

				if (color > 0) {
					state++;
					color = 0;
				}

				int rotOrder[] = {0, 1};
				if (len(usedBy[1][dir]) > len(usedBy[0][dir]))
					swap(rotOrder[0], rotOrder[1]);
				for (int rot : rotOrder) {
					for (int i : usedBy[rot][dir]) {
						assert_eq(assignments[i].color, -1);
						requiredOutStates[rot][dir].insert(state);
						assignments[i].color = color;
						assignments[i].state = state;
						stateToDir[state] = dir;
						incr();
					}
					// pad if possible
					if (color > 0) {
						rep0(i, numUniqueStates) {
							auto& ust = uniqueStates[i];
							if (assignments[i].color != -1 || !ust.free)
								continue;
							if (ust.entryDir != -1 && ust.entryDir != dir)
								continue;
							assignments[i].color = color;
							assignments[i].state = state;
							stateToDir[state] = dir;
							incr();
							if (color == 0)
								break;
						}
					}
				}
				// assign the left
				rep0(i, numUniqueStates) {
					auto& ust = uniqueStates[i];
					if (assignments[i].color != -1)
						continue;
					if (ust.entryDir != dir)
						continue;
					assert(ust.free);
					assignments[i].color = color;
					assignments[i].state = state;
					stateToDir[state] = dir;
					incr();
				}
				if (color > 0 || dir == 3) {
					// assign the free ones
					rep0(i, numUniqueStates) {
						auto& ust = uniqueStates[i];
						if (assignments[i].color != -1)
							continue;
						if (ust.entryDir != -1)
							continue;
						assignments[i].color = color;
						assignments[i].state = state;
						stateToDir[state] = dir;
						incr();
						if (color == 0 && dir < 3)
							break;
					}
				}
			}
			int numStates = state + (color > 0 ? 1 : 0);

			static array<vector<int>, 4> dirToStates;
			dirToStates[0].clear();
			dirToStates[1].clear();
			dirToStates[2].clear();
			dirToStates[3].clear();

			rep0(s, numStates) {
				int dir = stateToDir[s];
				dirToStates[dir].push_back(s);
			}

			if (verbose) {
				trace(show(requiredOutStates, dirToStates));
			}

			int minStates = INF;
			rep0(dir, 4) {
				mineq(minStates, len(dirToStates[dir]));
			}
			int maxRequired = 0;
			rep0(rot, 2) {
				rep0(dir, 4) {
					maxeq(maxRequired, len(requiredOutStates[rot][dir]));
				}
			}
			if (minStates < maxRequired) {
				if (verbose) {
					trace("!!! possibly infeasible !!!");
					trace(show(requiredOutStates, dirToStates));
				}
				return INF;
			}
			int score = numColors + numStates + 3;
			if (!complete)
				return score;

			rep0(rot, 2) {
				rep0(dir, 4) {
					auto& set = requiredOutStates[rot][dir];
					if (len(set) < maxRequired) {
						// pad states to align the sizes
						for (int s : dirToStates[dir]) {
							set.insert(s);
							if (len(set) >= maxRequired)
								break;
						}
					}
				}
			}
			rep0(rot, 2) {
				rep0(dir, 4) {
					assert_eq(len(requiredOutStates[rot][dir]), maxRequired);
				}
			}

			if (verbose) {
				trace(show(numColors, numStates, score));
			}
			assert_eq(totalIncr, numUniqueStates);

			static array<vector<int>, 2> rotMaps;
			rotMaps[0].assign(numStates, -1);
			rotMaps[1].assign(numStates, -1);
			rep0(inDir, 4) {
				static array<vector<int>, 2> inSets;
				rep0(rot, 2) {
					auto& set = requiredOutStates[rot][inDir];
					inSets[rot].assign(set.begin(), set.end());
				}
				rep0(rot, 2) {
					auto& map = rotMaps[rot];
					Cell cell = {rot == 0 ? CELL_ROT1 : CELL_ROT2};
					int outDir = cell.nextDir(inDir);
					for (auto& inSet : inSets) {
						assert_eq(len(inSet), maxRequired);
						// make an injection
						bool used[MAX_STATES] = {};
						int numAssigned = 0;
						for (int in : inSet) {
							if (map[in] != -1) {
								used[map[in]] = true;
								numAssigned++;
							}
						}
						for (int in : inSet) {
							if (map[in] == -1) {
								// assign
								for (int out : requiredOutStates[rot][outDir]) {
									if (!used[out]) {
										map[in] = out;
										used[out] = true;
										numAssigned++;
										break;
									}
								}
							}
						}
						assert_eq(numAssigned, len(inSet));
					}
				}
			}
			if (verbose) {
				trace(show(rotMaps | mods::br()));
			}

			static vector<int> acceptableStates[MAX_UNIQUE_STATES];
			static vector<int> acceptableFirstStates[MAX_UNIQUE_STATES];
			static vector<int> prevTimeIndices[MAX_UNIQUE_STATES];
			static int mergedToIndices[MAX_UNIQUE_STATES];
			fast_iset<MAX_UNIQUE_STATES> unmergedIndices;

			rep0(i, numUniqueStates) {
				auto& ust = uniqueStates[i];
				auto& rots = ust.rotsBeforeEntry;

				auto& accStates = acceptableStates[i];
				auto& accFirstStates = acceptableFirstStates[i];
				accStates.clear();
				accFirstStates.clear();

				mergedToIndices[i] = -1;
				unmergedIndices.insert(i);

				int ntime = ust.nextIndexTime;
				if (ntime != -1) {
					prevTimeIndices[ntime].push_back(i);
				}

				int reqState = assignments[i].state;
				int dir = stateToDir[reqState];

				if (rots.empty()) {

					// all states in the dir acceptable
					for (int s : dirToStates[dir]) {
						accStates.push_back(s);
						accFirstStates.push_back(s);
					}

					assignments[i].firstState = reqState;
					continue;
				}

				int lastRot = rots.back();
				// find all acceptable states

				int firstState = -1;
				rep0(s, numStates) {
					int state = s;
					for (int rot : rots) {
						state = rotMaps[rot][state];
						if (state == -1)
							break;
					}
					if (state == reqState) {
						firstState = s;
					}
					if (state != -1 && stateToDir[state] == dir) {
						accStates.push_back(state);
						accFirstStates.push_back(s);
					}
				}
				assert_ne(firstState, -1);
				assignments[i].firstState = firstState;
			}

			// compress rules
			auto& origCells = cells;
			Compressor comp(numStates, numUniqueStates);
			auto cells = comp.cellsBuffer;
			rep0(i, numUniqueStates) {
				auto& ust = uniqueStates[i];
				auto cell = cells + i;
				cell->index = i;
				cell->state = assignments[i].state;

				cell->possibleStates = acceptableStates[i];
				sort(cell->possibleStates.begin(), cell->possibleStates.end());
				cell->possibleStates.erase(unique(cell->possibleStates.begin(), cell->possibleStates.end()),
					cell->possibleStates.end());

				cell->move = ust.move;
				if (ust.move == -1) {
					cell->move = 0; // actually arbitrary
				}
				int ntime = ust.nextIndexTime;
				if (ntime != -1) {
					assert_size(ntime, numUniqueStates);
					cell->nextTime = cells + ntime;
					cell->nextTime->prevTimes.push_back(cell);
					auto nt = cell->nextTime;
					for (auto [s1, s2] : lambda::zip(acceptableFirstStates[ntime], acceptableStates[ntime])) {
						cell->nextStateCands.emplace_back(s1, s2);
					}
				}
				int npos = ust.nextIndexPos;
				if (npos != -1) {
					assert_size(npos, numUniqueStates);
					cell->nextPos = cells + npos;
					cell->nextPos->prevPoss.push_back(cell);
				}
			}
			comp.initBuckets();
			comp.sanityCheck();
			comp.run(rng);
			map<pii, int> nextColors;
			int maxColor = -1;
			rep0(i, numUniqueStates) {
				auto [color, state, nextState] = comp.colorStateNextStates[i];
				maxeq(maxColor, color);
				auto& ust = uniqueStates[i];
				assert_ne(color, -1);
				assert_ne(state, -1);
				assert_ne(nextState, -1);
				int ntime = ust.nextIndexTime;
				assert_gt(ranges::count(acceptableStates[i], state), 0);
				if (ntime != -1) {
					auto& nst = uniqueStates[ntime];
					int nstate = comp.colorStateNextStates[ntime][1];
					assert_gt(ranges::count(acceptableFirstStates[ntime], nextState), 0)
						<< show(acceptableFirstStates[ntime], nextState);
					bool ok = false;
					for (auto [s1, s2] : lambda::zip(acceptableFirstStates[ntime], acceptableStates[ntime])) {
						if (s1 == nextState && s2 == nstate) {
							ok = true;
							break;
						}
					}
					assert(ok) << show(
						acceptableFirstStates[ntime], acceptableStates[ntime], nextState, nstate);
				}
				int npos = ust.nextIndexPos;
				if (npos != -1) {
					auto& npst = uniqueStates[npos];
					int ncolor = comp.colorStateNextStates[npos][0];
					pii key = pii(color, state);
					if (nextColors.count(key)) {
						assert_eq(nextColors[key], ncolor) << show(key);
					} else {
						nextColors[key] = ncolor;
					}
				}
			}

			res.numColors = (maxColor + 1) + 3;
			res.numStates = numStates;
			trace("score: ", res.numColors + res.numStates);

			res.initialState = -1;
			{
				int finalSt = comp.colorStateNextStates[0][1];
				for (auto [s1, s2] : lambda::zip(acceptableFirstStates[0], acceptableStates[0])) {
					if (s2 == finalSt) {
						res.initialState = s1;
						break;
					}
				}
			}
			assert_ne(res.initialState, -1);
			res.rules.clear();
			map<pii, array<int, 3>> rules; // (color, state) -> (ncolor, nstate, dir)
			auto addRule = [&](int color, int state, int ncolor, int nstate, int dir) {
				auto key = pii(color, state);
				if (rules.count(key)) {
					auto existing = rules[key];
					array<int, 3> expected = {ncolor, nstate, dir};
					if (expected[0] == -1)
						expected[0] = existing[0];
					if (existing[0] == -1)
						existing[0] = expected[0];
					assert_eq(existing, expected) << show(key);
					rules[key] = expected;
				} else {
					rules[key] = {ncolor, nstate, dir};
				}
			};
			rep0(s, numStates) {
				addRule(0, s, 0, s, stateToDir[s]);
				int s2;
				s2 = rotMaps[0][s];
				if (s2 != -1) {
					addRule(1, s, 1, s2, stateToDir[s2]);
				}
				s2 = rotMaps[1][s];
				if (s2 != -1) {
					addRule(2, s, 2, s2, stateToDir[s2]);
				}
			}
			res.colors.assign(n2, 0);
			res.colorStates.clear();
			rep0(posi, n2) {
				if (origCells[posi].rot()) {
					res.colors[posi] = origCells[posi].kind == CELL_ROT1 ? 1 : 2;
				}
			}
			rep0(i, numUniqueStates) {
				auto& ust = uniqueStates[i];
				int moveDir = ust.move;
				auto [color, state, nstate] = comp.colorStateNextStates[i];
				res.colorStates.push_back({color + 3, state});
				int posi = ust.posi;
				if (res.colors[posi] == 0) {
					res.colors[posi] = color + 3;
				}
				int ntime = ust.nextIndexTime;
				int npos = ust.nextIndexPos;
				int ncolor = npos == -1 ? -1 : comp.colorStateNextStates[npos][0];
				assert_ne(nstate, -1);
				if (moveDir != -1)
					addRule(color + 3, state, ncolor == -1 ? -1 : ncolor + 3, nstate, moveDir);
			}
			for (auto& [key, val] : rules) {
				res.rules.push_back({key.first, key.second, val[0] == -1 ? 0 : val[0], val[1], val[2]});
			}
			return res.numColors + res.numStates;
		};
		int base = (int) ceil(sqrt(numUniqueStates));
		int from = clamp((int) (base * 0.75), 1, base - 4);
		int to = max((int) (base * 1.33), base + 4);
		int best = INF;
		int bestNumColors = -1;
		rep(colors, from, to + 1) {
			if (mineq(best, tryWithColors(colors, false))) {
				bestNumColors = colors;
			}
		}
		tryWithColors((int) (bestNumColors * 1.5), true);
		return res;
	}

	void solveMain(ostream& out) {
		vector<Cell> cells(n2);
		vector<bool> ng(n2, false);
		vector<int> fullPath;
		fast_array<int, MAX_N2> heatmap;
		auto initialInfo = computeScore(cells, fullPath, true, false);
		rep2() {
			ivec2 pos(i, j);
			if ((i + j) % 2 == 0) {
				cells[pos.pack(n)] = {CELL_ICE};
			}
		}
		ScoreInfo bestInfo;
		rep0(iter, INF) {
			computeScore(cells, fullPath, false, false);
			heatmap.clear();
			for (int posi : fullPath) {
				heatmap[posi]++;
			}
			// pick best m position to add ice
			vector<ivec2> candidates;
			rep2() {
				ivec2 pos(i, j);
				int posi = pos.pack(n);
				if (cells[posi].stop() && !ng[posi]) {
					candidates.push_back(pos);
				}
			}
			ranges::sort(candidates, [&](const ivec2& a, const ivec2& b) {
				return heatmap[a.pack(n)] > heatmap[b.pack(n)];
			});
			int bestPos = -1;
			bool done = true;
			for (ivec2 pos : candidates) {
				int posi = pos.pack(n);
				cells[posi] = {CELL_ICE};
				auto score = computeScore(cells, fullPath, true, false);
				if (score.steps < maxS && score.score < bestInfo.score) {
					trace("dist: ", score.dist, " steps: ", score.steps, "/", maxS);
					bestInfo = score;
					bestPos = posi;
					done = false;
				} else {
					cells[posi] = {CELL_STOP};
					if (score.dist == INF)
						ng[posi] = true;
				}
			}
			if (done)
				break;
		}

		Solution bestSolution;
		{
			const double T_FROM = 2;
			const double T_TO = 0.2;
			const double P_ADD_ICE = 0.2;
			const double P_CHANGE_KIND = 0.5;

			vector<Cell> bestCells = cells;
			vector<int> bestFullPath = fullPath;
			auto curInfo = computeScore(cells, fullPath, false, false);
			bestInfo = curInfo;

			double progress = 0;
			auto evaluateInfo = [&](const ScoreInfo& info) -> double {
				double score = info.score;
				if (info.steps <= maxS)
					return score;
				return INF;
			};

			double bestScore = evaluateInfo(bestInfo);
			double score = bestScore;

			auto tryAddIce = [&](double tol) -> bool {
				// pick random non-ice cell
				int addPos = -1;
				for (int attempts = 0; attempts < n2; ++attempts) {
					int cand = rng.next_int(n2);
					if (!cells[cand].ice()) {
						addPos = cand;
						break;
					}
				}
				if (addPos == -1)
					return false; // nothing to add

				auto old = cells[addPos];
				cells[addPos] = {CELL_ICE};
				auto info = computeScore(cells, fullPath, false, false);
				double newScore = evaluateInfo(info);

				if (newScore > score + tol) {
					// revert
					cells[addPos] = old;
					return false;
				}
				curInfo = info;
				return true;
			};

			auto tryMoveCell = [&](double tol) -> bool {
				// pick a random non-ice cell
				int posi = -1;
				for (int attempts = 0; attempts < n2; ++attempts) {
					int cand = rng.next_int(n2);
					if (!cells[cand].ice()) {
						posi = cand;
						break;
					}
				}
				if (posi == -1)
					return false; // no non-ice cell to move/remove

				// pick a random direction that's not blocked
				int dir = -1;
				for (int attempts = 0; attempts < 8; ++attempts) {
					int d = rng.next_int(4);
					if (!p.walls[posi][d]) {
						dir = d;
						break;
					}
				}
				if (dir == -1)
					return false; // trapped, shouldn't happen

				ivec2 pos = unpack[posi];
				ivec2 npos = pos + ivec2::dir(dir);
				int nposi = npos.pack(n);

				auto old = cells[posi];
				swap(cells[posi], cells[nposi]);

				auto info = computeScore(cells, fullPath, false, false);
				double newScore = evaluateInfo(info);

				if (newScore > score + tol) {
					// revert
					swap(cells[posi], cells[nposi]);
					return false;
				}
				curInfo = info;
				return true;
			};

			auto tryChangeKind = [&](double tol) -> bool {
				int posi = -1;
				bool targetIce = rng.next_prob(0.5);
				for (int attempts = 0; attempts < n2; ++attempts) {
					int cand = rng.next_int(n2);
					if (cells[cand].ice() == targetIce) {
						posi = cand;
						break;
					}
				}
				if (posi == -1)
					return false; // no candidate

				Cell old = cells[posi];
				int pool[] = {CELL_STOP, CELL_ROT1, CELL_ROT2};
				vector<int> choices;
				for (int k : pool) {
					if (k != old.kind)
						choices.push_back(k);
				}
				if (choices.empty())
					return false;

				int newKind = choices[rng.next_int((int) choices.size())];
				cells[posi].kind = newKind;

				auto info = computeScore(cells, fullPath, false, false);
				double newScore = evaluateInfo(info);

				if (newScore > score + tol) {
					// revert
					cells[posi] = old;
					return false;
				}
				curInfo = info;
				return true;
			};

			int startTime = timer();
			int endTime = 1950;
			if (startTime >= endTime) {
				endTime = startTime + 1;
				trace("warning: time limit already exceeded before SA!! ", show(startTime, endTime));
			}
			int iters = 0;
			int bestRawScore = INF;
			double lastBestProgress = 0;
			double lastBuiltProgress = 0;
			// main SA loop
			while (true) {
				iters++;
				progress = (timer() - startTime) / (double) (endTime - startTime);

				// update scores
				score = evaluateInfo(curInfo);
				bestScore = evaluateInfo(bestInfo);

				if (progress >= 1.0)
					break;
				// temperature schedule (exponential decay)
				double temp = exp_interp(T_FROM, T_TO, progress);
				double tol = temp * -log(rng.next_float());

				// choose which transition to attempt
				bool accepted = false;
				if (rng.next_prob(P_ADD_ICE)) {
					accepted = tryAddIce(tol);
				} else if (rng.next_prob(P_CHANGE_KIND)) {
					accepted = tryChangeKind(tol);
				} else {
					accepted = tryMoveCell(tol);
				}
				if (progress > lastBestProgress + 0.1 && !accepted) {
					// restart from best
					lastBestProgress = progress;
					cells = bestCells;
					curInfo = bestInfo;
					trace("restarting from best...");
					continue;
				}
				if (!accepted)
					continue;

				auto buildSolution = [&](const vector<Cell>& cells, const vector<int>& fullPath) {
					lastBuiltProgress = progress;
					auto solution = constructRules(cells, fullPath, false);
					int rawScore = solution.numColors + solution.numStates;
					if (mineq(bestRawScore, rawScore)) {
						bestSolution = solution;
						trace("New min raw score: ", bestRawScore, " at iter ", iters,
							" colors=", solution.numColors, " states=", solution.numStates);
					}
				};

				if (progress > 0.6 && progress > lastBuiltProgress + 0.1) {
					buildSolution(cells, fullPath);
				}

				if (score < bestScore) {
					lastBestProgress = progress;
					bestInfo = curInfo;
					bestCells = cells;
					bestFullPath = fullPath;
					trace("score: ", score, " steps: ", curInfo.steps, "/", maxS, ", T: ", temp,
						", iter: ", iters);
					if (curInfo.steps <= maxS && (progress > 0.6 || bestRawScore == INF)) {
						buildSolution(cells, fullPath);
					}
				}
			}
		}

		// print solution
		auto& sol = bestSolution;
		out << sol.numColors << " " << sol.numStates << " " << len(sol.rules) << endl;
		rep2() {
			ivec2 pos(i, j);
			out << sol.colors[pos.pack(n)];
			if (j == n - 1) {
				out << "\n";
			} else {
				out << " ";
			}
		}
		int initialState = sol.initialState;
		auto swapState = [&](int& state) {
			if (state == initialState)
				state = 0;
			else if (state == 0)
				state = initialState;
		};
		for (auto [color, state, ncolor, nstate, dir] : sol.rules) {
			swapState(state);
			swapState(nstate);
			out << color << " " << state << " " << ncolor << " " << nstate << " " << "UDLR"[dir] << "\n";
		}
		out.flush();
		trace(sol.colorStates);

		result.score = sol.numColors + sol.numStates;
	}
};

int main(int argc, char* argv[]) {
#if ONLINE_JUDGE
	isLocal = false;
	render = false;
	timer(true);
	Solver sol;
	sol.load(cin);
	sol.solve();
#elif 0
	// write metadata
	ostringstream oss;
	oss << "seed n t" << endl;
	rep0(seed, 1000) {
		Solver sol;
		sol.load(seed);
		int numTrees = 0;
		int n = sol.p.n;
		int t = sol.p.numT;
		oss << seed << " " << n << " " << t << endl;
	}
	write_text("scores/input.txt", oss.str());
#elif 1
	// for local/remote testers
	debug = true;
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
	EasyTester<Solver> tester("min");
	int num = 20;
	int from = 0;
	int stride = 1;
	int single = -1;

	vector<int> seeds = {};

	debug = true;
	render = true;
	time_scale = 1;

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
