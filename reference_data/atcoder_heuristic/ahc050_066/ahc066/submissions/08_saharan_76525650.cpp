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
		constexpr int len(const array<T, N>& a) {
			return N;
		}

		template <class T, int N>
		constexpr int len(const T (&a)[N]) {
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
				min(value0, value1), max(value0, value1));
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

			const word* words() const {
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
constexpr short INF_DIST = numeric_limits<short>::max();
constexpr int MAX_N = 20;
constexpr int MAX_M = 40;
constexpr int MAX_CELLS = MAX_N * MAX_N;
constexpr int MAX_STATES = MAX_CELLS * 4;
#define repn(i) rep0(i, n)

// portable problem data
struct Problem {
	int n;
	int m;
	int numT;
	vector<string> verticalWall;
	vector<string> horizontalWall;
	vector<pii> ballPos;
	vector<pii> basketPos;
	vector<char> canMoveTable;

	void load(istream& in) {
		in >> n >> m >> numT;
		verticalWall.resize(n);
		rep0(i, n) {
			in >> verticalWall[i];
		}
		horizontalWall.resize(n - 1);
		rep0(i, n - 1) {
			in >> horizontalWall[i];
		}
		buildCanMoveTable();
		ballPos.resize(m);
		basketPos.resize(m);
		rep0(k, m) {
			in >> ballPos[k].first >> ballPos[k].second >> basketPos[k].first >> basketPos[k].second;
		}
	}

	bool canMoveRaw(int i, int j, int dir) const {
		if (dir == 0) {
			return i > 0 && horizontalWall[i - 1][j] == '0';
		}
		if (dir == 1) {
			return j + 1 < n && verticalWall[i][j] == '0';
		}
		if (dir == 2) {
			return i + 1 < n && horizontalWall[i][j] == '0';
		}
		return j > 0 && verticalWall[i][j - 1] == '0';
	}

	void buildCanMoveTable() {
		canMoveTable.assign(n * n * 4, 0);
		rep0(i, n) {
			rep0(j, n) {
				int cellId = i * n + j;
				rep0(dir, 4) {
					canMoveTable[(cellId << 2) | dir] = canMoveRaw(i, j, dir);
				}
			}
		}
	}

	bool canMove(int cellId, int dir) const {
		return canMoveTable[(cellId << 2) | dir];
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
			ostringstream out;
			auto flush = [&]() {
				mov.close_file();
				ofstream out2(output_file_name(seed));
				out2 << out.str();
				out2.close();
			};
			on_abort = [&]() {
				flush();
			};
			solveMain(out);
			flush();
		} else {
			solveMain(cout);
		}
	}

private:
	static constexpr int DI[4] = {-1, 0, 1, 0};
	static constexpr int DJ[4] = {0, 1, 0, -1};
	static constexpr int TAIL_MACRO_OPTIMIZE_LENGTH = 64;
	static constexpr int TAIL_MACRO_MIN_PATTERN_LENGTH = 2;
	static constexpr int TAIL_MACRO_MAX_PATTERN_LENGTH = 10;
	static constexpr int ENDGAME_BFS_ACTIVE_BALL_LIMIT = 8; // exclusive

	struct MacroRegistration {
		string y;
		string encodedX;
		string operations;
		int outputLength = 0;
		int expandedLength = 0;
		int initialState = 0;
		bool compressed = false;
		bool prefix = false;
	};

	struct Candidate {
		string x;
		string operations;
		MacroRegistration registration;
		string ppSwitchPattern;
		ll score = (ll) 4e18;
		int abstractLength = 0;
		int expandedLength = 0;
		int ppSwitchExpandedLength = 0;
		int xUseCount = 0;
		int outputLength = 0;
		bool valid = false;
		string invalidReason;
	};

	struct SmallOutputTail {
		int length = 0;
		char data[TAIL_MACRO_OPTIMIZE_LENGTH];

		void clear() {
			length = 0;
		}

		void append(char c) {
			if (length < TAIL_MACRO_OPTIMIZE_LENGTH) {
				data[length++] = c;
				return;
			}
			memmove(data, data + 1, TAIL_MACRO_OPTIMIZE_LENGTH - 1);
			data[TAIL_MACRO_OPTIMIZE_LENGTH - 1] = c;
		}

		void append(string_view s) {
			int sLength = (int) s.size();
			if (sLength >= TAIL_MACRO_OPTIMIZE_LENGTH) {
				memcpy(data, s.data() + sLength - TAIL_MACRO_OPTIMIZE_LENGTH, TAIL_MACRO_OPTIMIZE_LENGTH);
				length = TAIL_MACRO_OPTIMIZE_LENGTH;
				return;
			}
			for (char c : s) {
				append(c);
			}
		}
	};

	struct AnnealResult {
		Candidate best;
		vector<Candidate> exactCandidates;
	};

	int numCells = 0;
	int numStates = 0;
	int initialStateId = 0;
	vector<int> ballInitialCell;
	vector<int> basketCell;
	vector<int> initialBallAtCell;
	vector<ull> ballHash;
	vector<ull> stateHash;
	ull xRegistrationHash = 0;
	ull ppSwitchHash = 0;
	vector<short> basicNextF;
	vector<short> basicNextL;
	vector<short> basicNextR;
	vector<short> reverseNonPPreds;
	alignas(32) short fastDistance[MAX_STATES];
	alignas(32) short fastPUseCount[MAX_STATES];
	alignas(32) short fastQueue[MAX_STATES];
	alignas(32) short stateBall[MAX_STATES];
	alignas(32) short fastXNextStates[MAX_STATES];
	alignas(32) unsigned short fastSeen[MAX_STATES];
	unsigned short fastVisitToken = 1;
	vector<string> ppSwitchCandidatePatterns;
	unordered_map<string, string> ppSwitchPatternCache;
	string lastBeamFailureReason;
	vector<Candidate> lastBeamTerminalCandidates;
	static constexpr int X_NEXT_STATE_MIN_CACHED_PREFIX_LENGTH = 8;
	static constexpr int FAST_EVAL_IGNORE_EXPANDED_LIMIT_N_MULTIPLIER = 5;
	struct XNextStateCacheNode {
		int child[3] = {-1, -1, -1};
		alignas(32) short nextStates[MAX_STATES];
	};
	vector<XNextStateCacheNode> xNextStateCache;

	void init() {
		assert_le(p.n, MAX_N);
		assert_le(p.m, MAX_M);
		numCells = p.n * p.n;
		numStates = numCells * 4;
		initialStateId = stateId({0, 0}, 1);
		ballInitialCell.resize(p.m);
		basketCell.resize(p.m);
		initialBallAtCell.assign(numCells, -1);
		rep0(ball, p.m) {
			ballInitialCell[ball] = id(p.ballPos[ball]);
			basketCell[ball] = id(p.basketPos[ball]);
			initialBallAtCell[ballInitialCell[ball]] = ball;
		}
		memset(fastSeen, 0, sizeof(fastSeen));
		fastVisitToken = 1;
		buildPpSwitchCandidatePatterns();
		ppSwitchPatternCache.clear();
		ppSwitchPatternCache.reserve(4096);

		ballHash.resize(p.m);
		for (ull& h : ballHash) {
			h = rng.next_ull();
		}

		stateHash.resize(numStates);
		for (ull& h : stateHash) {
			h = rng.next_ull();
		}
		xRegistrationHash = rng.next_ull();
		ppSwitchHash = rng.next_ull();

		basicNextF.resize(numStates);
		basicNextL.resize(numStates);
		basicNextR.resize(numStates);
		rep0(state, numStates) {
			int cellId = state >> 2;
			int i = cellId / p.n;
			int j = cellId % p.n;
			int dir = state & 3;
			int nextCellId = cellId;
			if (p.canMove(cellId, dir)) {
				nextCellId = (i + DI[dir]) * p.n + (j + DJ[dir]);
			}
			basicNextF[state] = (short) ((nextCellId << 2) | dir);
			basicNextL[state] = (short) ((cellId << 2) | ((dir + 3) & 3));
			basicNextR[state] = (short) ((cellId << 2) | ((dir + 1) & 3));
			stateBall[state] = (short) initialBallAtCell[cellId];
		}

		reverseNonPPreds.resize(numStates * 3);
		rep0(targetState, numStates) {
			int cellId = targetState >> 2;
			int i = cellId / p.n;
			int j = cellId - i * p.n;
			int dir = targetState & 3;
			int nonPBase = targetState * 3;
			reverseNonPPreds[nonPBase] = (short) ((cellId << 2) | ((dir + 3) & 3));
			reverseNonPPreds[nonPBase + 1] = (short) ((cellId << 2) | ((dir + 1) & 3));
			int pi = i - DI[dir];
			int pj = j - DJ[dir];
			int previousCellId = pi * p.n + pj;
			if (0 <= pi && pi < p.n && 0 <= pj && pj < p.n && p.canMove(previousCellId, dir)) {
				reverseNonPPreds[nonPBase + 2] = (short) ((previousCellId << 2) | dir);
			} else {
				reverseNonPPreds[nonPBase + 2] = (short) targetState;
			}
		}

		xNextStateCache.clear();
		xNextStateCache.reserve(32768);
		xNextStateCache.emplace_back();
		rep0(state, numStates) {
			xNextStateCache[0].nextStates[state] = (short) state;
		}
	}

	int id(pii cell) const {
		return cell.first * p.n + cell.second;
	}

	int stateId(pii cell, int dir) const {
		return (id(cell) << 2) | dir;
	}

	bool shouldIgnoreFastExpandedLimit() const {
		return p.numT > 3 * p.m * FAST_EVAL_IGNORE_EXPANDED_LIMIT_N_MULTIPLIER * p.n;
	}

	string normalizePpSwitchPattern(const string& pattern) const {
		string normalized;
		normalized.reserve(len(pattern));
		for (char operation : pattern) {
			normalized.push_back(operation);
			while (true) {
				int n = len(normalized);
				if (n >= 2) {
					char a = normalized[n - 2];
					char b = normalized[n - 1];
					if ((a == 'L' && b == 'R') || (a == 'R' && b == 'L')) {
						normalized.resize(n - 2);
						continue;
					}
					if (a == 'R' && b == 'R') {
						normalized[n - 2] = 'L';
						normalized[n - 1] = 'L';
						continue;
					}
				}
				if (n >= 3) {
					char a = normalized[n - 3];
					char b = normalized[n - 2];
					char c = normalized[n - 1];
					if (a == 'R' && b == 'R' && c == 'R') {
						normalized.resize(n - 3);
						normalized.push_back('L');
						continue;
					}
					if (a == 'L' && b == 'L' && c == 'L') {
						normalized.resize(n - 3);
						normalized.push_back('R');
						continue;
					}
				}
				break;
			}
		}
		return normalized;
	}

	bool isNormalizedPpSwitchPattern(const string& pattern) const {
		return pattern.find("LR") == string::npos && pattern.find("RL") == string::npos &&
			pattern.find("LLL") == string::npos && pattern.find("RRR") == string::npos &&
			pattern.find("RR") == string::npos;
	}

	void buildPpSwitchCandidatePatterns() {
		ppSwitchCandidatePatterns.clear();
		unordered_set<string> seen;
		string current;
		auto dfs = [&](auto&& self, int targetLength) -> void {
			if (len(current) == targetLength) {
				string normalized = normalizePpSwitchPattern(current);
				int pCount = 0;
				for (char operation : normalized) {
					if (operation == 'P') {
						pCount++;
					}
				}
				if (pCount < 2 || !isNormalizedPpSwitchPattern(normalized)) {
					return;
				}
				if (seen.insert(normalized).second) {
					ppSwitchCandidatePatterns.push_back(std::move(normalized));
				}
				return;
			}
			for (char operation : {'P', 'L', 'R'}) {
				current.push_back(operation);
				self(self, targetLength);
				current.pop_back();
			}
		};
		rep(length, 2, 5) {
			dfs(dfs, length);
		}
		sort(ppSwitchCandidatePatterns.begin(), ppSwitchCandidatePatterns.end());
		assert(find(ppSwitchCandidatePatterns.begin(), ppSwitchCandidatePatterns.end(), "PP") !=
			ppSwitchCandidatePatterns.end());
	}

	int xOperationIndex(char operation) const {
		if (operation == 'F') {
			return 0;
		}
		if (operation == 'L') {
			return 1;
		}
		assert_eq(operation, 'R');
		return 2;
	}

	void applyXOperationRaw(short* nextStates, char operation) const {
		const short* table = operation == 'F' ? basicNextF.data()
			: operation == 'L'                ? basicNextL.data()
											  : basicNextR.data();
		rep0(state, numStates) {
			nextStates[state] = table[(unsigned short) nextStates[state]];
		}
	}

	void applyXOperation(vector<short>& nextStates, char operation) const {
		applyXOperationRaw(nextStates.data(), operation);
	}

	void getXNextStatesRaw(const string& x, short* output) {
		assert(!x.empty());
		assert(!xNextStateCache.empty());
		int xLength = len(x);
		int createLimit = max(xLength / 2, min(X_NEXT_STATE_MIN_CACHED_PREFIX_LENGTH, xLength));
		int node = 0;
		int consumedLength = 0;
		for (; consumedLength < xLength; consumedLength++) {
			char operation = x[consumedLength];
			int operationIndex = xOperationIndex(operation);
			int child = xNextStateCache[node].child[operationIndex];
			if (child == -1) {
				if (consumedLength >= createLimit) {
					break;
				}
				xNextStateCache.emplace_back();
				child = len(xNextStateCache) - 1;
				XNextStateCacheNode& nextNode = xNextStateCache[child];
				memcpy(nextNode.nextStates, xNextStateCache[node].nextStates, sizeof(short) * numStates);
				applyXOperationRaw(nextNode.nextStates, operation);
				xNextStateCache[node].child[operationIndex] = child;
			}
			node = child;
		}
		memcpy(output, xNextStateCache[node].nextStates, sizeof(short) * numStates);
		for (int index = consumedLength; index < xLength; index++) {
			applyXOperationRaw(output, x[index]);
		}
	}

	void getXNextStates(const string& x, vector<short>& output) {
		output.resize(numStates);
		getXNextStatesRaw(x, output.data());
	}

	ull hashPTransitions(const vector<short>& xNextStates) const {
		ull hash = 1469598103934665603ULL;
		for (int nextState : xNextStates) {
			hash ^= (ull) nextState + 0x9e3779b97f4a7c15ULL;
			hash *= 1099511628211ULL;
		}
		return hash;
	}

	ll xReachDistanceSum_donotuse(const string& x) {
		vector<short> xNextStates;
		getXNextStates(x, xNextStates);
		int numCells = p.n * p.n;
		int numStates = numCells * 4;
		vector<short> distance(numStates, INF_DIST);
		vector<short> que(numStates);
		int head = 0;
		int tail = 0;
		rep0(dir, 4) {
			int startState = dir;
			distance[startState] = 0;
			que[tail++] = (short) startState;
		}

		auto push = [&](int nextState, int nextDistance) {
			if (distance[nextState] != INF_DIST) {
				return;
			}
			distance[nextState] = (short) nextDistance;
			que[tail++] = (short) nextState;
		};

		while (head < tail) {
			int currentState = que[head++];
			int nextDistance = distance[currentState] + 1;
			push(xNextStates[currentState], nextDistance);
			push(basicNextF[currentState], nextDistance);
			push(basicNextR[currentState], nextDistance);
			push(basicNextL[currentState], nextDistance);
		}

		ll sum = 0;
		rep0(cellId, numCells) {
			int baseState = cellId << 2;
			short bestDistance = INF_DIST;
			rep0(dir, 4) {
				bestDistance = min(bestDistance, distance[baseState | dir]);
			}
			assert_ne(bestDistance, INF_DIST);
			sum += bestDistance;
		}
		return sum;
	}

	string replaceAllNonOverlapping(const string& x, const string& y) const {
		assert(!y.empty());
		string encoded;
		encoded.reserve(len(x));
		for (int i = 0; i < len(x);) {
			if (i + len(y) <= len(x) && x.compare(i, len(y), y) == 0) {
				encoded.push_back('P');
				i += len(y);
			} else {
				encoded.push_back(x[i]);
				i++;
			}
		}
		return encoded;
	}

	vector<MacroRegistration> buildMacroRegistrations(
		const string& x, const vector<short>& xNextStates) const {
		assert(!x.empty());
		assert(!xNextStates.empty());

		int startState = stateId({0, 0}, 1);
		MacroRegistration baseline;
		baseline.encodedX = x;
		baseline.outputLength = len(x) + 1;
		baseline.expandedLength = 0;
		baseline.initialState = startState;
		baseline.compressed = false;
		baseline.prefix = false;

		auto isBetterRegistration = [](const MacroRegistration& candidate, const MacroRegistration& base) {
			if (candidate.outputLength != base.outputLength) {
				return candidate.outputLength < base.outputLength;
			}
			if (candidate.expandedLength != base.expandedLength) {
				return candidate.expandedLength < base.expandedLength;
			}
			return candidate.operations < base.operations;
		};

		optional<MacroRegistration> bestCompressed;
		int maxFRun = 0;
		int currentFRun = 0;
		for (char operation : x) {
			if (operation == 'F') {
				currentFRun++;
				maxFRun = max(maxFRun, currentFRun);
			} else {
				currentFRun = 0;
			}
		}
		rep(length, 2, maxFRun + 1) {
			string y(length, 'F');
			string encodedX = replaceAllNonOverlapping(x, y);

			MacroRegistration candidate;
			candidate.y = y;
			candidate.encodedX = encodedX;
			candidate.operations = "M" + y + "MM" + encodedX + "M";
			candidate.outputLength = len(candidate.operations);
			candidate.expandedLength = len(y) + len(x);
			candidate.initialState = startState;
			candidate.compressed = true;
			candidate.prefix = false;

			if (!bestCompressed || isBetterRegistration(candidate, *bestCompressed)) {
				bestCompressed = std::move(candidate);
			}
		}

		vector<MacroRegistration> registrations;
		registrations.push_back(std::move(baseline));
		if (bestCompressed && bestCompressed->outputLength < registrations[0].outputLength) {
			registrations.push_back(std::move(*bestCompressed));
		}
		return registrations;
	}

	int expandedLength(const string& abstractOperations, const string& x) const {
		int expanded = 0;
		for (char operation : abstractOperations) {
			expanded += operation == 'P' ? len(x) : 1;
		}
		return expanded;
	}

	int xUseCount(const string& abstractOperations) const {
		int count = 0;
		for (char operation : abstractOperations) {
			if (operation == 'P') {
				count++;
			}
		}
		return count;
	}

	struct TailMacroPlan {
		int bestLength = INF;
		int patternStart = -1;
		int patternLength = 0;
		int replacementCount = 0;
		int replacementStarts[TAIL_MACRO_OPTIMIZE_LENGTH];

		bool valid() const {
			return patternStart != -1;
		}
	};

	int tailMacroOperationCode(char operation) const {
		if (operation == 'F') {
			return 0;
		}
		if (operation == 'L') {
			return 1;
		}
		if (operation == 'R') {
			return 2;
		}
		if (operation == 'M') {
			return 3;
		}
		if (operation == 'P') {
			return 4;
		}
		if (operation == 'S') {
			return 5;
		}
		assert(false);
		return 0;
	}

	void appendTailMacroTransformedSuffix(
		string& output, string_view targetOperations, const TailMacroPlan& plan) const {
		int replacementIndex = plan.replacementCount - 1;
		int targetLength = (int) targetOperations.size();
		rep0(index, targetLength) {
			if (replacementIndex >= 0 && index == plan.replacementStarts[replacementIndex]) {
				if (replacementIndex == plan.replacementCount - 1) {
					output.push_back('M');
					output.append(targetOperations.data() + plan.patternStart, plan.patternLength);
					output.push_back('M');
				} else {
					output.push_back('P');
				}
				index += plan.patternLength - 1;
				replacementIndex--;
			} else {
				output.push_back(targetOperations[index]);
			}
		}
	}

	TailMacroPlan buildTailMacroPlan(
		int originalLength, string_view targetOperations, bool keepLexicographicallySmallest) const {
		int targetLength = (int) targetOperations.size();
		TailMacroPlan bestPlan;
		bestPlan.bestLength = originalLength;
		if (targetLength < TAIL_MACRO_MIN_PATTERN_LENGTH) {
			return bestPlan;
		}

		ull patternKeys[TAIL_MACRO_MAX_PATTERN_LENGTH + 1][TAIL_MACRO_OPTIMIZE_LENGTH];
		ull rawKeys[TAIL_MACRO_MAX_PATTERN_LENGTH + 1][TAIL_MACRO_OPTIMIZE_LENGTH];
		ull usedKeys[TAIL_MACRO_MAX_PATTERN_LENGTH + 1][TAIL_MACRO_OPTIMIZE_LENGTH];
		int usedCounts[TAIL_MACRO_MAX_PATTERN_LENGTH + 1] = {};
		ull operationCodes[TAIL_MACRO_OPTIMIZE_LENGTH];
		rep0(index, targetLength) {
			operationCodes[index] = (ull) tailMacroOperationCode(targetOperations[index]);
		}

		int maxPatternLength = min(TAIL_MACRO_MAX_PATTERN_LENGTH, targetLength);
		rep0(start, targetLength) {
			rawKeys[1][start] = operationCodes[start];
			patternKeys[1][start] = ((ull) 1 << 60) | rawKeys[1][start];
		}
		rep(length, 2, maxPatternLength + 1) {
			rep0(start, targetLength - length + 1) {
				rawKeys[length][start] =
					(rawKeys[length - 1][start] << 3) | operationCodes[start + length - 1];
				patternKeys[length][start] = ((ull) length << 60) | rawKeys[length][start];
			}
		}

		int currentReplacementStarts[TAIL_MACRO_OPTIMIZE_LENGTH];
		static string currentSuffix;
		static string bestSuffix;
		currentSuffix.clear();
		bestSuffix.clear();

		rep0(start, targetLength) {
			rep(length, TAIL_MACRO_MIN_PATTERN_LENGTH, maxPatternLength + 1) {
				if (start + length > targetLength) {
					break;
				}
				ull key = patternKeys[length][start];
				bool alreadyUsed = false;
				rep0(i, usedCounts[length]) {
					if (usedKeys[length][i] == key) {
						alreadyUsed = true;
						break;
					}
				}
				if (alreadyUsed) {
					continue;
				}
				usedKeys[length][usedCounts[length]++] = key;

				int replacementCount = 0;
				int cursor = targetLength;
				while (cursor >= length) {
					int occurrenceStart = cursor - length;
					if (patternKeys[length][occurrenceStart] == key) {
						currentReplacementStarts[replacementCount++] = occurrenceStart;
						cursor = occurrenceStart;
					} else {
						if (targetOperations[cursor - 1] == 'P') {
							break;
						}
						cursor--;
					}
				}
				if (replacementCount == 0) {
					continue;
				}
				int newLength = originalLength + 2 - (length - 1) * (replacementCount - 1);
				if (newLength >= originalLength || newLength > bestPlan.bestLength) {
					continue;
				}

				TailMacroPlan currentPlan;
				currentPlan.bestLength = newLength;
				currentPlan.patternStart = start;
				currentPlan.patternLength = length;
				currentPlan.replacementCount = replacementCount;
				rep0(index, replacementCount) {
					currentPlan.replacementStarts[index] = currentReplacementStarts[index];
				}

				bool take = newLength < bestPlan.bestLength || !bestPlan.valid();
				if (!take && keepLexicographicallySmallest && newLength == bestPlan.bestLength) {
					currentSuffix.clear();
					bestSuffix.clear();
					appendTailMacroTransformedSuffix(currentSuffix, targetOperations, currentPlan);
					appendTailMacroTransformedSuffix(bestSuffix, targetOperations, bestPlan);
					take = currentSuffix < bestSuffix;
				}
				if (take) {
					bestPlan = currentPlan;
				}
			}
		}
		return bestPlan;
	}

	int tailMacroOptimizedLength(int originalLength, string_view targetOperations) const {
		return buildTailMacroPlan(originalLength, targetOperations, false).bestLength;
	}

	int tailMacroOptimizedLength(int originalLength, const SmallOutputTail& tail) const {
		if (tail.length < TAIL_MACRO_MIN_PATTERN_LENGTH) {
			return originalLength;
		}
		int start = 0;
		rep0(index, tail.length) {
			if (tail.data[index] == 'M') {
				start = index + 1;
			}
		}
		return tailMacroOptimizedLength(originalLength, string_view(tail.data + start, tail.length - start));
	}

	bool optimizeTailMacro(Candidate& candidate) const {
		if (!candidate.valid || candidate.operations.empty()) {
			return false;
		}
		int lastM = -1;
		rep0(index, len(candidate.operations)) {
			if (candidate.operations[index] == 'M') {
				lastM = index;
			}
		}
		if (lastM == -1 || lastM + 1 >= len(candidate.operations)) {
			return false;
		}

		const string& operations = candidate.operations;
		int originalLength = len(operations);
		int targetStart = max(lastM + 1, originalLength - TAIL_MACRO_OPTIMIZE_LENGTH);
		int targetLength = originalLength - targetStart;
		string_view targetOperations(operations.data() + targetStart, targetLength);
		TailMacroPlan plan = buildTailMacroPlan(originalLength, targetOperations, true);
		if (!plan.valid()) {
			return false;
		}

		string bestOperations;
		bestOperations.reserve(plan.bestLength);
		bestOperations.append(operations, 0, targetStart);
		appendTailMacroTransformedSuffix(bestOperations, targetOperations, plan);
		assert_eq(plan.bestLength, len(bestOperations));
		candidate.operations = std::move(bestOperations);
		candidate.outputLength = plan.bestLength;
		candidate.abstractLength = plan.bestLength;
		candidate.score = plan.bestLength;
		assert_eq(len(candidate.operations), candidate.outputLength);
		assert_le(candidate.outputLength, p.numT);
		assert_le(candidate.expandedLength, p.numT);
		return true;
	}

	string buildOutputOperations(const string& abstractOperations, const string& x) const {
		int firstXUse = (int) abstractOperations.find('P');
		assert(firstXUse != (int) string::npos);

		string operations;
		operations.reserve(len(abstractOperations) + len(x) + 1);
		rep0(i, firstXUse) {
			operations.push_back(abstractOperations[i]);
		}
		operations.push_back('M');
		for (char operation : x) {
			operations.push_back(operation);
		}
		operations.push_back('M');
		rep(i, firstXUse + 1, len(abstractOperations)) {
			operations.push_back(abstractOperations[i]);
		}
		return operations;
	}

	int outputLengthWithRegistration(
		const string& abstractOperations, const MacroRegistration& registration) const {
		int outputLength = 0;
		for (char operation : abstractOperations) {
			if (operation == 'A') {
				outputLength += len(registration.y) + 2;
			} else if (operation == 'B') {
				outputLength += len(registration.encodedX) + 2;
			} else {
				outputLength += operation == 'G' ? registration.outputLength : 1;
			}
		}
		return outputLength;
	}

	int expandedLengthWithRegistration(
		const string& abstractOperations, const string& x, const MacroRegistration& registration) const {
		int expanded = 0;
		for (char operation : abstractOperations) {
			if (operation == 'P') {
				expanded += len(x);
			} else if (operation == 'Q') {
				expanded += len(registration.y);
			} else if (operation == 'A') {
				expanded += len(registration.y);
			} else if (operation == 'B') {
				expanded += len(x);
			} else if (operation == 'G') {
				expanded += registration.expandedLength;
			} else {
				expanded++;
			}
		}
		return expanded;
	}

	string buildOutputOperationsWithRegistration(
		const string& abstractOperations, const MacroRegistration& registration) const {
		string operations;
		operations.reserve(outputLengthWithRegistration(abstractOperations, registration));
		int oldRegistrationCount = 0;
		int yRegistrationCount = 0;
		int xRegistrationCount = 0;
		for (char operation : abstractOperations) {
			if (operation == 'A') {
				operations.push_back('M');
				operations += registration.y;
				operations.push_back('M');
				yRegistrationCount++;
			} else if (operation == 'B') {
				operations.push_back('M');
				operations += registration.encodedX;
				operations.push_back('M');
				xRegistrationCount++;
			} else if (operation == 'Q') {
				operations.push_back('P');
			} else if (operation == 'G') {
				operations += registration.operations;
				oldRegistrationCount++;
			} else {
				operations.push_back(operation);
			}
		}
		assert_eq(oldRegistrationCount, 0);
		assert_eq(yRegistrationCount, 1);
		assert_eq(xRegistrationCount, 1);
		return operations;
	}

	struct PpSwitchOperationConversion {
		string operations;
		int outputLength = 0;
		int expandedLength = 0;
		int zUseCount = 0;
		bool valid = true;
		string invalidReason;
	};

	struct PpSwitchMacro {
		string pattern = "PP";
		vector<short> nextStates;
		int expandedLength = 0;
		int firstZExtraOutputCost = 3;
	};

	PpSwitchOperationConversion buildOutputOperationsWithPpSwitch(const string& abstractOperations,
		const string& x, const MacroRegistration& registration, const PpSwitchMacro& ppSwitchMacro) const {
		PpSwitchOperationConversion result;
		result.operations.reserve(len(abstractOperations) + len(x) + len(ppSwitchMacro.pattern) + 16);
		bool xRegistered = false;
		int oldRegistrationCount = 0;
		int yRegistrationCount = 0;
		int xRegistrationCount = 0;

		auto invalidate = [&](const string& reason) {
			if (result.valid) {
				result.valid = false;
				result.invalidReason = reason;
			}
		};

		for (char operation : abstractOperations) {
			if (operation == 'A') {
				result.operations.push_back('M');
				result.operations += registration.y;
				result.operations.push_back('M');
				result.outputLength += len(registration.y) + 2;
				result.expandedLength += len(registration.y);
				yRegistrationCount++;
			} else if (operation == 'B') {
				result.operations.push_back('M');
				result.operations += registration.encodedX;
				result.operations.push_back('M');
				result.outputLength += len(registration.encodedX) + 2;
				result.expandedLength += len(x);
				xRegistered = true;
				xRegistrationCount++;
			} else if (operation == 'Q') {
				result.operations.push_back('P');
				result.outputLength++;
				result.expandedLength += len(registration.y);
			} else if (operation == 'G') {
				result.operations += registration.operations;
				result.outputLength += registration.outputLength;
				result.expandedLength += registration.expandedLength;
				xRegistered = true;
				oldRegistrationCount++;
			} else if (operation == 'P') {
				if (registration.compressed) {
					if (!xRegistered) {
						invalidate("P before X registration");
					}
					result.operations.push_back('P');
					result.outputLength++;
				} else if (!xRegistered) {
					result.operations.push_back('M');
					result.operations += x;
					result.operations.push_back('M');
					result.outputLength += len(x) + 2;
					xRegistered = true;
				} else {
					result.operations.push_back('P');
					result.outputLength++;
				}
				result.expandedLength += len(x);
			} else if (operation == 'Z') {
				if (!xRegistered) {
					invalidate("Z before X registration");
				}
				result.zUseCount++;
				if (result.zUseCount == 1) {
					result.operations.push_back('M');
					result.operations += ppSwitchMacro.pattern;
					result.operations.push_back('M');
					result.outputLength += len(ppSwitchMacro.pattern) + 2;
				} else {
					result.operations.push_back('P');
					result.outputLength++;
				}
				result.expandedLength += ppSwitchMacro.expandedLength;
			} else {
				result.operations.push_back(operation);
				result.outputLength++;
				result.expandedLength++;
			}
		}

		if (registration.compressed) {
			if (oldRegistrationCount != 0 || yRegistrationCount != 1 || xRegistrationCount != 1) {
				invalidate("compressed registration marker mismatch");
			}
		}
		assert_eq(result.outputLength, len(result.operations));
		assert_eq(result.operations.find('Z'), string::npos);
		return result;
	}

	struct SimulationResult {
		bool valid = true;
		string reason;
		int executedBasicLength = 0;
		int outputLength = 0;
		int state = 0;
		int heldBall = -1;
		int failedBall = -1;
		int actualCell = -1;
		int expectedCell = -1;
	};

	SimulationResult simulateFinalOperations(const string& operations) const {
		SimulationResult result;
		result.outputLength = len(operations);
		result.state = initialStateId;
		vector<short> cellBall(numCells, -1);
		vector<short> ballCell(p.m);
		rep0(ball, p.m) {
			int cell = ballInitialCell[ball];
			cellBall[cell] = (short) ball;
			ballCell[ball] = (short) cell;
		}

		bool recording = false;
		vector<char> macro;
		vector<char> recordingBuffer;
		macro.reserve(p.numT);
		recordingBuffer.reserve(p.numT);

		auto markInvalid = [&](const string& reason) {
			if (result.valid) {
				result.valid = false;
				result.reason = reason;
			}
			return false;
		};

		auto executeBasic = [&](char operation) {
			if (result.executedBasicLength >= p.numT) {
				return markInvalid("expanded execution exceeded T");
			}
			result.executedBasicLength++;
			if (operation == 'F') {
				result.state = basicNextF[result.state];
			} else if (operation == 'L') {
				result.state = basicNextL[result.state];
			} else if (operation == 'R') {
				result.state = basicNextR[result.state];
			} else if (operation == 'S') {
				int cell = result.state >> 2;
				if (result.heldBall == -1) {
					int pickedBall = cellBall[cell];
					if (pickedBall != -1) {
						result.heldBall = pickedBall;
						cellBall[cell] = -1;
						ballCell[pickedBall] = (short) numCells;
					}
				} else {
					int placedBall = result.heldBall;
					int pickedBall = cellBall[cell];
					cellBall[cell] = (short) placedBall;
					ballCell[placedBall] = (short) cell;
					result.heldBall = pickedBall;
					if (pickedBall != -1) {
						ballCell[pickedBall] = (short) numCells;
					}
				}
			} else {
				return markInvalid("invalid basic operation");
			}
			return true;
		};

		for (char operation : operations) {
			if (!result.valid) {
				break;
			}
			if (operation == 'M') {
				if (recording) {
					macro = recordingBuffer;
					recordingBuffer.clear();
					recording = false;
				} else {
					recordingBuffer.clear();
					recording = true;
				}
			} else if (operation == 'P') {
				vector<char> macroSnapshot = macro;
				for (char basicOperation : macroSnapshot) {
					if (!executeBasic(basicOperation)) {
						break;
					}
				}
				if (result.valid && recording) {
					recordingBuffer.insert(recordingBuffer.end(), macroSnapshot.begin(), macroSnapshot.end());
				}
			} else if (operation == 'F' || operation == 'L' || operation == 'R' || operation == 'S') {
				if (!executeBasic(operation)) {
					break;
				}
				if (recording) {
					recordingBuffer.push_back(operation);
				}
			} else {
				markInvalid("invalid output operation");
			}
		}

		if (!result.valid) {
			return result;
		}
		if (recording) {
			markInvalid("recording left open");
			return result;
		}
		if (result.heldBall != -1) {
			result.failedBall = result.heldBall;
			result.actualCell = numCells;
			result.expectedCell = basketCell[result.heldBall];
			markInvalid("finished while holding a ball");
			return result;
		}
		rep0(ball, p.m) {
			if (ballCell[ball] != basketCell[ball]) {
				result.failedBall = ball;
				result.actualCell = ballCell[ball];
				result.expectedCell = basketCell[ball];
				markInvalid("ball is not stored in its basket");
				return result;
			}
		}
		return result;
	}

	void assertFinalOperationsValid(const Candidate& candidate) const {
		SimulationResult simulation = simulateFinalOperations(candidate.operations);
		if (!simulation.valid) {
			trace("final simulation failed: reason=", simulation.reason, ", output=", simulation.outputLength,
				", expandedExecuted=", simulation.executedBasicLength, "/", p.numT,
				", state=", simulation.state, ", heldBall=", simulation.heldBall,
				", failedBall=", simulation.failedBall, ", actualCell=", simulation.actualCell,
				", expectedCell=", simulation.expectedCell, ", candidateScore=", candidate.score,
				", candidateExpanded=", candidate.expandedLength);
			trace(candidate.operations);
		}
		assert(simulation.valid);
	}

	struct EndgameSnapshot {
		bool valid = false;
		string invalidReason;
		int prefixLength = 0;
		int prefixExpandedLength = 0;
		int state = 0;
		int heldBall = -1;
		vector<short> ballCell;
		vector<short> cellBall;
		vector<char> macro;
	};

	EndgameSnapshot buildEndgameSnapshot(const string& operations) const {
		EndgameSnapshot snapshot;
		int currentState = initialStateId;
		int currentHeldBall = -1;
		vector<short> currentBallCell(p.m);
		vector<short> currentCellBall(numCells, (short) -1);
		rep0(ball, p.m) {
			int cell = ballInitialCell[ball];
			currentBallCell[ball] = (short) cell;
			currentCellBall[cell] = (short) ball;
		}

		int executedBasicLength = 0;
		bool recording = false;
		vector<char> macro;
		vector<char> recordingBuffer;
		macro.reserve(p.numT);
		recordingBuffer.reserve(p.numT);

		auto invalidate = [&](const string& reason) {
			snapshot.valid = false;
			snapshot.invalidReason = reason;
			return false;
		};

		auto executeBasic = [&](char operation) {
			if (executedBasicLength >= p.numT) {
				return invalidate("expanded execution exceeded T");
			}
			executedBasicLength++;
			if (operation == 'F') {
				currentState = basicNextF[currentState];
			} else if (operation == 'L') {
				currentState = basicNextL[currentState];
			} else if (operation == 'R') {
				currentState = basicNextR[currentState];
			} else if (operation == 'S') {
				int cell = currentState >> 2;
				if (currentHeldBall == -1) {
					int pickedBall = currentCellBall[cell];
					if (pickedBall != -1) {
						currentHeldBall = pickedBall;
						currentCellBall[cell] = -1;
						currentBallCell[pickedBall] = (short) numCells;
					}
				} else {
					int placedBall = currentHeldBall;
					int pickedBall = currentCellBall[cell];
					currentCellBall[cell] = (short) placedBall;
					currentBallCell[placedBall] = (short) cell;
					currentHeldBall = pickedBall;
					if (pickedBall != -1) {
						currentBallCell[pickedBall] = (short) numCells;
					}
				}
			} else {
				return invalidate("invalid basic operation");
			}
			return true;
		};

		auto saveSnapshot = [&](int prefixLength) {
			snapshot.valid = true;
			snapshot.invalidReason.clear();
			snapshot.prefixLength = prefixLength;
			snapshot.prefixExpandedLength = executedBasicLength;
			snapshot.state = currentState;
			snapshot.heldBall = currentHeldBall;
			snapshot.ballCell = currentBallCell;
			snapshot.cellBall = currentCellBall;
			snapshot.macro = macro;
		};

		rep0(index, len(operations)) {
			char operation = operations[index];
			if (operation == 'M') {
				if (recording) {
					macro = recordingBuffer;
					recordingBuffer.clear();
					recording = false;
					saveSnapshot(index + 1);
				} else {
					recordingBuffer.clear();
					recording = true;
				}
			} else if (operation == 'P') {
				vector<char> macroSnapshot = macro;
				for (char basicOperation : macroSnapshot) {
					if (!executeBasic(basicOperation)) {
						return snapshot;
					}
				}
				if (recording) {
					recordingBuffer.insert(recordingBuffer.end(), macroSnapshot.begin(), macroSnapshot.end());
				}
			} else if (operation == 'F' || operation == 'L' || operation == 'R' || operation == 'S') {
				if (!executeBasic(operation)) {
					return snapshot;
				}
				if (recording) {
					recordingBuffer.push_back(operation);
				}
			} else {
				invalidate("invalid output operation");
				return snapshot;
			}
		}
		if (recording) {
			invalidate("recording left open");
		}
		return snapshot;
	}

	bool tryEndgameBfs(Candidate& candidate, int timeLimit) const {
		if (!candidate.valid || candidate.operations.empty()) {
			return false;
		}
		EndgameSnapshot snapshot = buildEndgameSnapshot(candidate.operations);
		if (!snapshot.valid) {
			trace("endgame bfs skipped: no valid final registration snapshot, reason=", snapshot.invalidReason);
			return false;
		}
		if (snapshot.macro.empty()) {
			trace("endgame bfs skipped: empty macro");
			return false;
		}

		vector<int> activeBalls;
		activeBalls.reserve(p.m);
		rep0(ball, p.m) {
			if (snapshot.heldBall == ball || snapshot.ballCell[ball] != basketCell[ball]) {
				activeBalls.push_back(ball);
			}
		}
		int activeCount = len(activeBalls);
		if (activeCount >= ENDGAME_BFS_ACTIVE_BALL_LIMIT) {
			trace("endgame bfs skipped: activeBalls=", activeCount);
			return false;
		}

		vector<int> activeIndex(p.m, -1);
		vector<int> activeInitialCell(activeCount);
		vector<int> activeBasketCell(activeCount);
		rep0(index, activeCount) {
			int ball = activeBalls[index];
			activeIndex[ball] = index;
			activeInitialCell[index] = snapshot.ballCell[ball];
			activeBasketCell[index] = basketCell[ball];
		}
		int initialHeldIndex = snapshot.heldBall == -1 ? -1 : activeIndex[snapshot.heldBall];
		int allCompletedMask = (1 << activeCount) - 1;
		if (activeCount == 0) {
			if (snapshot.prefixLength < candidate.outputLength) {
				string newOperations = candidate.operations.substr(0, snapshot.prefixLength);
				trace("endgame bfs improved: activeBalls=0, old=", candidate.outputLength,
					", new=", len(newOperations));
				candidate.operations = std::move(newOperations);
				candidate.outputLength = len(candidate.operations);
				candidate.abstractLength = candidate.outputLength;
				candidate.score = candidate.outputLength;
				candidate.expandedLength = snapshot.prefixExpandedLength;
				candidate.xUseCount = (int) count(candidate.operations.begin(), candidate.operations.end(), 'P');
				return true;
			}
			trace("endgame bfs skipped: activeBalls=0 no suffix");
			return false;
		}

		vector<char> fixedOccupied(numCells, 0);
		rep0(ball, p.m) {
			if (activeIndex[ball] == -1 && snapshot.ballCell[ball] != numCells) {
				fixedOccupied[snapshot.ballCell[ball]] = 1;
			}
		}

		struct MacroTransition {
			int endState = -1;
			vector<short> sCells;
		};
		vector<MacroTransition> macroTransitions(numStates);
		rep0(startState, numStates) {
			int state = startState;
			MacroTransition& transition = macroTransitions[startState];
			transition.sCells.reserve(4);
			for (char operation : snapshot.macro) {
				if (operation == 'F') {
					state = basicNextF[state];
				} else if (operation == 'L') {
					state = basicNextL[state];
				} else if (operation == 'R') {
					state = basicNextR[state];
				} else {
					assert_eq(operation, 'S');
					transition.sCells.push_back((short) (state >> 2));
				}
			}
			transition.endState = state;
		}

		int heldSlots = activeCount + 1;
		int maskCount = 1 << activeCount;
		int keyCount = numStates * heldSlots * maskCount;
		auto encodeKey = [&](int state, int heldIndex, int completedMask) {
			int heldSlot = heldIndex + 1;
			return (completedMask * heldSlots + heldSlot) * numStates + state;
		};
		auto decodeKey = [&](int key, int& state, int& heldIndex, int& completedMask) {
			state = key % numStates;
			key /= numStates;
			int heldSlot = key % heldSlots;
			completedMask = key / heldSlots;
			heldIndex = heldSlot - 1;
		};

		auto activeAtCell = [&](int cell, int heldIndex, int completedMask) {
			rep0(index, activeCount) {
				if ((completedMask & (1 << index)) || index == heldIndex) {
					continue;
				}
				if (activeInitialCell[index] == cell) {
					return index;
				}
			}
			return -1;
		};
		auto canPlaceHeldAt = [&](int heldIndex, int cell, int completedMask) {
			return heldIndex != -1 && cell == activeBasketCell[heldIndex] &&
				!fixedOccupied[cell] && activeAtCell(cell, heldIndex, completedMask) == -1;
		};
		auto applyStrictS = [&](int state, int heldIndex, int completedMask,
								int& nextHeldIndex, int& nextCompletedMask) {
			int cell = state >> 2;
			nextHeldIndex = heldIndex;
			nextCompletedMask = completedMask;
			if (heldIndex == -1) {
				int pickedIndex = activeAtCell(cell, heldIndex, completedMask);
				if (pickedIndex == -1) {
					return false;
				}
				nextHeldIndex = pickedIndex;
				return true;
			}
			if (!canPlaceHeldAt(heldIndex, cell, completedMask)) {
				return false;
			}
			nextCompletedMask |= 1 << heldIndex;
			nextHeldIndex = -1;
			return true;
		};

		struct MacroApplyResult {
			bool computed = false;
			bool valid = false;
			int nextKey = -1;
		};
		vector<MacroApplyResult> macroCache(keyCount);
		auto applyMacro = [&](int key) -> MacroApplyResult {
			MacroApplyResult& cached = macroCache[key];
			if (cached.computed) {
				return cached;
			}
			cached.computed = true;
			int state, heldIndex, completedMask;
			decodeKey(key, state, heldIndex, completedMask);
			const MacroTransition& transition = macroTransitions[state];
			for (int cell : transition.sCells) {
				int eventState = cell << 2;
				int nextHeldIndex, nextCompletedMask;
				if (!applyStrictS(eventState, heldIndex, completedMask, nextHeldIndex, nextCompletedMask)) {
					return cached;
				}
				heldIndex = nextHeldIndex;
				completedMask = nextCompletedMask;
			}
			cached.valid = true;
			cached.nextKey = encodeKey(transition.endState, heldIndex, completedMask);
			return cached;
		};

		struct Label {
			int key = 0;
			int output = 0;
			int expanded = 0;
			int prev = -1;
			char operation = 0;
			bool valid = true;
		};
		vector<Label> labels;
		labels.reserve(4096);
		vector<vector<int>> labelsByKey(keyCount);
		struct QueueEntry {
			int output;
			int expanded;
			int label;
			bool operator>(const QueueEntry& other) const {
				if (output != other.output) {
					return output > other.output;
				}
				return expanded > other.expanded;
			}
		};
		priority_queue<QueueEntry, vector<QueueEntry>, greater<QueueEntry>> que;

		auto addLabel = [&](int key, int output, int expanded, int prev, char operation) {
			if (snapshot.prefixLength + output > p.numT) {
				return -1;
			}
			if (snapshot.prefixExpandedLength + expanded > p.numT) {
				return -1;
			}
			for (int labelId : labelsByKey[key]) {
				const Label& label = labels[labelId];
				if (label.valid && label.output <= output && label.expanded <= expanded) {
					return -1;
				}
			}
			for (int labelId : labelsByKey[key]) {
				Label& label = labels[labelId];
				if (label.valid && output <= label.output && expanded <= label.expanded) {
					label.valid = false;
				}
			}
			int labelId = len(labels);
			labels.push_back({key, output, expanded, prev, operation, true});
			labelsByKey[key].push_back(labelId);
			que.push({output, expanded, labelId});
			return labelId;
		};

		int startKey = encodeKey(snapshot.state, initialHeldIndex, 0);
		addLabel(startKey, 0, 0, -1, 0);
		int goalLabel = -1;
		while (!que.empty()) {
			if (timer() >= timeLimit) {
				trace("endgame bfs aborted: activeBalls=", activeCount, ", labels=", len(labels),
					", time=", timer(), " ms");
				return false;
			}
			QueueEntry entry = que.top();
			que.pop();
			if (!labels[entry.label].valid ||
				labels[entry.label].output != entry.output ||
				labels[entry.label].expanded != entry.expanded) {
				continue;
			}
			const Label& label = labels[entry.label];
			int state, heldIndex, completedMask;
			decodeKey(label.key, state, heldIndex, completedMask);
			if (completedMask == allCompletedMask && heldIndex == -1) {
				goalLabel = entry.label;
				break;
			}
			auto addTransition = [&](int nextKey, char operation, int expandedDelta) {
				addLabel(nextKey, label.output + 1, label.expanded + expandedDelta, entry.label, operation);
			};
			addTransition(encodeKey(basicNextL[state], heldIndex, completedMask), 'L', 1);
			addTransition(encodeKey(basicNextR[state], heldIndex, completedMask), 'R', 1);
			int nextF = basicNextF[state];
			if (nextF != state) {
				addTransition(encodeKey(nextF, heldIndex, completedMask), 'F', 1);
			}
			int nextHeldIndex, nextCompletedMask;
			if (applyStrictS(state, heldIndex, completedMask, nextHeldIndex, nextCompletedMask)) {
				addTransition(encodeKey(state, nextHeldIndex, nextCompletedMask), 'S', 1);
			}
			MacroApplyResult macroResult = applyMacro(label.key);
			if (macroResult.valid) {
				addTransition(macroResult.nextKey, 'P', len(snapshot.macro));
			}
		}

		if (goalLabel == -1) {
			trace("endgame bfs failed: activeBalls=", activeCount, ", labels=", len(labels));
			return false;
		}
		string suffix;
		for (int labelId = goalLabel; labels[labelId].prev != -1; labelId = labels[labelId].prev) {
			suffix.push_back(labels[labelId].operation);
		}
		reverse(suffix.begin(), suffix.end());
		int newOutputLength = snapshot.prefixLength + len(suffix);
		int newExpandedLength = snapshot.prefixExpandedLength + labels[goalLabel].expanded;
		if (newOutputLength >= candidate.outputLength) {
			trace("endgame bfs success no improve: activeBalls=", activeCount,
				", old=", candidate.outputLength, ", new=", newOutputLength,
				", labels=", len(labels));
			return false;
		}

		string newOperations = candidate.operations.substr(0, snapshot.prefixLength) + suffix;
		trace("endgame bfs improved: activeBalls=", activeCount,
			", old=", candidate.outputLength, ", new=", newOutputLength,
			", oldExpanded=", candidate.expandedLength, ", newExpanded=", newExpandedLength,
			", labels=", len(labels));
		candidate.operations = std::move(newOperations);
		candidate.outputLength = newOutputLength;
		candidate.abstractLength = newOutputLength;
		candidate.score = newOutputLength;
		candidate.expandedLength = newExpandedLength;
		candidate.xUseCount = (int) count(candidate.operations.begin(), candidate.operations.end(), 'P');
		candidate.valid = true;
		return true;
	}

	struct RouteTable {
		short* packed = nullptr;
		char* dirMasks = nullptr;
		short* pUseCounts = nullptr;
		short* dirDistances = nullptr;

		static short pack(int distance, int endDir) {
			return (short) ((distance << 2) | endDir);
		}

		static int firstDir(int mask) {
			assert_ne(mask, 0);
			return std::countr_zero((unsigned) mask);
		}

		int shortestDirMask(int state) const {
			return ((unsigned char) dirMasks[state]) & 0x0f;
		}

		int nearDirMask(int state) const {
			return (((unsigned char) dirMasks[state]) >> 4) & 0x0f;
		}

		int distance(int state) const {
			short value = packed[state];
			return value == INF_DIST ? INF_DIST : value >> 2;
		}

		int endDir(int state) const {
			return firstDir(shortestDirMask(state));
		}

		int distanceForDir(int state, int dir) const {
			if (dirDistances != nullptr) {
				short value = dirDistances[(state << 2) | dir];
				return value == INF_DIST ? INF : value;
			}
			int value = distance(state);
			if (value == INF_DIST) {
				return INF;
			}
			int bit = 1 << dir;
			if (shortestDirMask(state) & bit) {
				return value;
			}
			if (nearDirMask(state) & bit) {
				return value + 1;
			}
			return INF;
		}

		int pUseCountForDir(int state, int dir) const {
			assert(pUseCounts != nullptr);
			int value = pUseCounts[(state << 2) | dir];
			return value == INF_DIST ? INF : value;
		}

		int expandedDistanceForDir(int state, int dir, int xLength) const {
			int distance = distanceForDir(state, dir);
			if (distance == INF) {
				return INF;
			}
			int pUseCount = pUseCountForDir(state, dir);
			if (pUseCount == INF) {
				return INF;
			}
			return distance + pUseCount * (xLength - 1);
		}
	};

	struct FirstRegistrationRoute {
		vector<int> distance;
		vector<int> expanded;
		vector<int> previous;
		vector<char> previousOperation;
	};

	struct FirstRegistrationTargetRoute {
		vector<int> distance;
		vector<int> expanded;
		vector<int> next;
		vector<char> operation;
		vector<short> endState;
	};

	struct PpSwitchRoute {
		short* distance = nullptr;
		short* endState = nullptr;
		int* expanded = nullptr;
	};

	struct BeamOperationsResult {
		string operations;
		vector<string> lastOperations;
		int terminalOptimizedScore = INF;
	};

	struct ReverseGraph {
		const short* nonPPreds = nullptr;
		vector<short> pStart;
		vector<short> pSources;
	};

	struct BeamState {
		int currentCell = 0;
		int currentDir = 1;
		int storedCount = 0;
		ull storedBits = 0;
		ull storedHash = 0;
		bool xRegistered = false;
		bool ppSwitched = false;
		int executedCost = 0;
		int expandedCost = 0;
		int remainingLowerBound = 0;
		int remainingLowerBoundXX = 0;
		int approachLowerBound = 0;
		int approachLowerBoundXX = 0;
		int outputLengthSoFar = 0;
		int terminalOptimizedScore = INF;
		SmallOutputTail outputTail;
	};

	struct BeamMove {
		int pickupBall = -1;
		int storedBall = -1;
		int pickupStartState = -1;
		int basketStartState = -1;
		int firstPickupLayerState = -1;
		int pickupEndDir = -1;
		int basketEndDir = -1;
		int nextState = -1;
		int ppPickupStartLayerState = -1;
		int ppBasketStartLayerState = -1;
		ull nextStoredBits = 0;
		ull nextStoredHash = 0;
		bool nextXRegistered = false;
		bool nextPpSwitched = false;
		int executedCostDelta = 0;
		int expandedCostDelta = 0;
		int remainingLowerBoundDelta = 0;
		int remainingLowerBoundXXDelta = 0;
		int approachLowerBoundDelta = 0;
		int approachLowerBoundXXDelta = 0;
		bool useFirstRegistrationRoute = false;
		bool usePpSwitchPickupRoute = false;
		bool usePpSwitchBasketRoute = false;
		int candidateScore = INF;
		ull candidateHash = 0;
	};

	struct BeamCandidateData {
		int score = INF;
	};

	struct BeamConfig {
		int beamWidth = 96;
		int sameParentCount = 8;
		bool useDynamicBeamWidth = false;
		int dynamicTimeLimit = 0;
		int beamWidthManagerWindow = 1;
		bool traceFilterBeforeCount = false;
		bool useFixedRegistration = false;
		MacroRegistration fixedRegistration;
		bool enablePpSwitch = true;
		bool collectLastCandidates = false;
		int lastCandidateLimit = 0;
	};

	enum FinalMoveId {
		FINAL_MOVE_L = 0,
		FINAL_MOVE_R = 1,
		FINAL_MOVE_F = 2,
		FINAL_MOVE_P = 3,
		FINAL_MOVE_S = 4,
		FINAL_MOVE_REGISTER = 5,
		FINAL_MOVE_COUNT = 6,
	};

	struct FinalBeamUndo {
		int state = 0;
		bool registered = false;
		int heldBall = -1;
		int correctCount = 0;
		ll evalSum = 0;
		int outputLength = 0;
		int expandedLength = 0;
		int xUseCount = 0;
		ull placementHash = 0;
		int changedBallCount = 0;
		int changedBalls[2] = {-1, -1};
		short oldBallCells[2] = {-1, -1};
		int changedCell = -1;
		short oldCellBall = -1;
	};

	struct FinalBeamState {
		int state = 0;
		bool registered = false;
		int heldBall = -1;
		int correctCount = 0;
		ll evalSum = 0;
		int outputLength = 0;
		int expandedLength = 0;
		int xUseCount = 0;
		ull placementHash = 0;
		vector<short> ballCell;
		vector<short> cellBall;
		vector<FinalBeamUndo> undoStack;
	};

	struct FinalBeamCandidateData {
		ll score = INF;
		int outputLength = 0;
		int state = 0;
	};

	void buildReverseGraph(const vector<short>& xNextStates, ReverseGraph& graph) const {
		assert(!xNextStates.empty());
		graph.nonPPreds = reverseNonPPreds.data();

		graph.pStart.assign(numStates + 1, 0);
		rep0(sourceState, numStates) {
			int targetState = (unsigned short) xNextStates[sourceState];
			graph.pStart[targetState + 1]++;
		}
		rep0(state, numStates) {
			graph.pStart[state + 1] += graph.pStart[state];
		}
		graph.pSources.resize(numStates);
		rep0(sourceState, numStates) {
			int targetState = (unsigned short) xNextStates[sourceState];
			graph.pSources[graph.pStart[targetState]++] = (short) sourceState;
		}
		for (int state = numStates; state > 0; state--) {
			graph.pStart[state] = graph.pStart[state - 1];
		}
		graph.pStart[0] = 0;
	}

	ReverseGraph buildReverseGraph(const vector<short>& xNextStates) const {
		ReverseGraph graph;
		buildReverseGraph(xNextStates, graph);
		return graph;
	}

	void buildRouteTable(
		pii target, const ReverseGraph& graph, const RouteTable& route, vector<short>& que) const {
		int numStates = p.n * p.n * 4;
		short* packed = route.packed;
		char* dirMasks = route.dirMasks;
		short* pUseCounts = route.pUseCounts;
		short* dirDistances = route.dirDistances;
		const short* nonPPreds = graph.nonPPreds;
		const short* pStart = graph.pStart.data();
		const short* pSources = graph.pSources.data();
		fill(packed, packed + numStates, INF_DIST);
		fill(dirMasks, dirMasks + numStates, 0);
		if (len(que) < numStates * 16) {
			que.resize(numStates * 16);
		}

		int head = 0;
		int tail = 0;

		int targetCell = id(target);
		rep0(dir, 4) {
			int targetState = (targetCell << 2) | dir;
			packed[targetState] = RouteTable::pack(0, dir);
			dirMasks[targetState] = (char) (1 << dir);
			que[tail++] = (short) targetState;
		}

		while (head < tail) {
			int currentState = que[head++];
			short currentPacked = packed[currentState];
			int currentDistance = currentPacked >> 2;
			unsigned char currentMasks = (unsigned char) dirMasks[currentState];
			int currentShortestMask = currentMasks & 0x0f;
			int currentPlusOneMask = currentMasks >> 4;
			int shortestDistance = currentDistance + 1;
			int plusOneDistance = currentDistance + 2;

			int nonPBase = currentState * 3;
			{
				int sourceState = nonPPreds[nonPBase];
				if (currentShortestMask != 0) {
					short sourcePacked = packed[sourceState];
					if (sourcePacked == INF_DIST) {
						packed[sourceState] =
							RouteTable::pack(shortestDistance, RouteTable::firstDir(currentShortestMask));
						dirMasks[sourceState] = (char) currentShortestMask;
						que[tail++] = (short) sourceState;
					} else {
						int sourceDistance = sourcePacked >> 2;
						unsigned char sourceMasks = (unsigned char) dirMasks[sourceState];
						int sourceShortestMask = sourceMasks & 0x0f;
						int sourcePlusOneMask = sourceMasks >> 4;
						if (sourceDistance == shortestDistance) {
							int nextShortestMask = sourceShortestMask | currentShortestMask;
							if (nextShortestMask != sourceShortestMask) {
								dirMasks[sourceState] = (char) ((sourcePlusOneMask << 4) | nextShortestMask);
								que[tail++] = (short) sourceState;
							}
						} else if (sourceDistance > shortestDistance) {
							int nextPlusOneMask =
								sourceDistance == shortestDistance + 1 ? sourceShortestMask : 0;
							packed[sourceState] =
								RouteTable::pack(shortestDistance, RouteTable::firstDir(currentShortestMask));
							dirMasks[sourceState] = (char) ((nextPlusOneMask << 4) | currentShortestMask);
							que[tail++] = (short) sourceState;
						}
					}
				}
				if (currentPlusOneMask != 0) {
					short sourcePacked = packed[sourceState];
					if (sourcePacked != INF_DIST) {
						int sourceDistance = sourcePacked >> 2;
						if (plusOneDistance == sourceDistance + 1) {
							unsigned char sourceMasks = (unsigned char) dirMasks[sourceState];
							int sourcePlusOneMask = sourceMasks >> 4;
							int nextPlusOneMask = sourcePlusOneMask | currentPlusOneMask;
							if (nextPlusOneMask != sourcePlusOneMask) {
								int sourceShortestMask = sourceMasks & 0x0f;
								dirMasks[sourceState] = (char) ((nextPlusOneMask << 4) | sourceShortestMask);
								que[tail++] = (short) sourceState;
							}
						}
					}
				}
			}
			{
				int sourceState = nonPPreds[nonPBase + 1];
				if (currentShortestMask != 0) {
					short sourcePacked = packed[sourceState];
					if (sourcePacked == INF_DIST) {
						packed[sourceState] =
							RouteTable::pack(shortestDistance, RouteTable::firstDir(currentShortestMask));
						dirMasks[sourceState] = (char) currentShortestMask;
						que[tail++] = (short) sourceState;
					} else {
						int sourceDistance = sourcePacked >> 2;
						unsigned char sourceMasks = (unsigned char) dirMasks[sourceState];
						int sourceShortestMask = sourceMasks & 0x0f;
						int sourcePlusOneMask = sourceMasks >> 4;
						if (sourceDistance == shortestDistance) {
							int nextShortestMask = sourceShortestMask | currentShortestMask;
							if (nextShortestMask != sourceShortestMask) {
								dirMasks[sourceState] = (char) ((sourcePlusOneMask << 4) | nextShortestMask);
								que[tail++] = (short) sourceState;
							}
						} else if (sourceDistance > shortestDistance) {
							int nextPlusOneMask =
								sourceDistance == shortestDistance + 1 ? sourceShortestMask : 0;
							packed[sourceState] =
								RouteTable::pack(shortestDistance, RouteTable::firstDir(currentShortestMask));
							dirMasks[sourceState] = (char) ((nextPlusOneMask << 4) | currentShortestMask);
							que[tail++] = (short) sourceState;
						}
					}
				}
				if (currentPlusOneMask != 0) {
					short sourcePacked = packed[sourceState];
					if (sourcePacked != INF_DIST) {
						int sourceDistance = sourcePacked >> 2;
						if (plusOneDistance == sourceDistance + 1) {
							unsigned char sourceMasks = (unsigned char) dirMasks[sourceState];
							int sourcePlusOneMask = sourceMasks >> 4;
							int nextPlusOneMask = sourcePlusOneMask | currentPlusOneMask;
							if (nextPlusOneMask != sourcePlusOneMask) {
								int sourceShortestMask = sourceMasks & 0x0f;
								dirMasks[sourceState] = (char) ((nextPlusOneMask << 4) | sourceShortestMask);
								que[tail++] = (short) sourceState;
							}
						}
					}
				}
			}
			{
				int sourceState = nonPPreds[nonPBase + 2];
				if (currentShortestMask != 0) {
					short sourcePacked = packed[sourceState];
					if (sourcePacked == INF_DIST) {
						packed[sourceState] =
							RouteTable::pack(shortestDistance, RouteTable::firstDir(currentShortestMask));
						dirMasks[sourceState] = (char) currentShortestMask;
						que[tail++] = (short) sourceState;
					} else {
						int sourceDistance = sourcePacked >> 2;
						unsigned char sourceMasks = (unsigned char) dirMasks[sourceState];
						int sourceShortestMask = sourceMasks & 0x0f;
						int sourcePlusOneMask = sourceMasks >> 4;
						if (sourceDistance == shortestDistance) {
							int nextShortestMask = sourceShortestMask | currentShortestMask;
							if (nextShortestMask != sourceShortestMask) {
								dirMasks[sourceState] = (char) ((sourcePlusOneMask << 4) | nextShortestMask);
								que[tail++] = (short) sourceState;
							}
						} else if (sourceDistance > shortestDistance) {
							int nextPlusOneMask =
								sourceDistance == shortestDistance + 1 ? sourceShortestMask : 0;
							packed[sourceState] =
								RouteTable::pack(shortestDistance, RouteTable::firstDir(currentShortestMask));
							dirMasks[sourceState] = (char) ((nextPlusOneMask << 4) | currentShortestMask);
							que[tail++] = (short) sourceState;
						}
					}
				}
				if (currentPlusOneMask != 0) {
					short sourcePacked = packed[sourceState];
					if (sourcePacked != INF_DIST) {
						int sourceDistance = sourcePacked >> 2;
						if (plusOneDistance == sourceDistance + 1) {
							unsigned char sourceMasks = (unsigned char) dirMasks[sourceState];
							int sourcePlusOneMask = sourceMasks >> 4;
							int nextPlusOneMask = sourcePlusOneMask | currentPlusOneMask;
							if (nextPlusOneMask != sourcePlusOneMask) {
								int sourceShortestMask = sourceMasks & 0x0f;
								dirMasks[sourceState] = (char) ((nextPlusOneMask << 4) | sourceShortestMask);
								que[tail++] = (short) sourceState;
							}
						}
					}
				}
			}

			const short* it = pSources + pStart[currentState];
			const short* end = pSources + pStart[currentState + 1];
			while (it != end) {
				int sourceState = *it++;
				if (currentShortestMask != 0) {
					short sourcePacked = packed[sourceState];
					if (sourcePacked == INF_DIST) {
						packed[sourceState] =
							RouteTable::pack(shortestDistance, RouteTable::firstDir(currentShortestMask));
						dirMasks[sourceState] = (char) currentShortestMask;
						que[tail++] = (short) sourceState;
					} else {
						int sourceDistance = sourcePacked >> 2;
						unsigned char sourceMasks = (unsigned char) dirMasks[sourceState];
						int sourceShortestMask = sourceMasks & 0x0f;
						int sourcePlusOneMask = sourceMasks >> 4;
						if (sourceDistance == shortestDistance) {
							int nextShortestMask = sourceShortestMask | currentShortestMask;
							if (nextShortestMask != sourceShortestMask) {
								dirMasks[sourceState] = (char) ((sourcePlusOneMask << 4) | nextShortestMask);
								que[tail++] = (short) sourceState;
							}
						} else if (sourceDistance > shortestDistance) {
							int nextPlusOneMask =
								sourceDistance == shortestDistance + 1 ? sourceShortestMask : 0;
							packed[sourceState] =
								RouteTable::pack(shortestDistance, RouteTable::firstDir(currentShortestMask));
							dirMasks[sourceState] = (char) ((nextPlusOneMask << 4) | currentShortestMask);
							que[tail++] = (short) sourceState;
						}
					}
				}
				if (currentPlusOneMask != 0) {
					short sourcePacked = packed[sourceState];
					if (sourcePacked != INF_DIST) {
						int sourceDistance = sourcePacked >> 2;
						if (plusOneDistance == sourceDistance + 1) {
							unsigned char sourceMasks = (unsigned char) dirMasks[sourceState];
							int sourcePlusOneMask = sourceMasks >> 4;
							int nextPlusOneMask = sourcePlusOneMask | currentPlusOneMask;
							if (nextPlusOneMask != sourcePlusOneMask) {
								int sourceShortestMask = sourceMasks & 0x0f;
								dirMasks[sourceState] = (char) ((nextPlusOneMask << 4) | sourceShortestMask);
								que[tail++] = (short) sourceState;
							}
						}
					}
				}
			}
		}

		if (dirDistances != nullptr) {
			for (int state = 0; state < numStates; state++) {
				short packedValue = packed[state];
				int base = state << 2;
				if (packedValue == INF_DIST) {
					dirDistances[base] = INF_DIST;
					dirDistances[base + 1] = INF_DIST;
					dirDistances[base + 2] = INF_DIST;
					dirDistances[base + 3] = INF_DIST;
					continue;
				}
				int distanceValue = packedValue >> 2;
				unsigned char masks = (unsigned char) dirMasks[state];
				int shortestMask = masks & 0x0f;
				int nearMask = masks >> 4;
				for (int dir = 0; dir < 4; dir++) {
					int bit = 1 << dir;
					dirDistances[base + dir] = (short) ((shortestMask & bit) ? distanceValue
						: (nearMask & bit) ? distanceValue + 1
						                 : INF_DIST);
				}
			}
		}
		if (pUseCounts == nullptr) {
			return;
		}
		fill(pUseCounts, pUseCounts + numStates * 4, INF_DIST);
		head = 0;
		tail = 0;

		auto pushPUse = [&](int layerState) {
			if (tail >= len(que)) {
				que.resize(len(que) * 2);
			}
			que[tail++] = (short) layerState;
		};
		auto relaxPUse = [&](int sourceState, int endDir, int sourceDistance, int candidatePUse) {
			int sourceLayerState = (sourceState << 2) | endDir;
			if (dirDistances != nullptr && dirDistances[sourceLayerState] != sourceDistance) {
				return;
			}
			if (dirDistances == nullptr && route.distanceForDir(sourceState, endDir) != sourceDistance) {
				return;
			}
			if (candidatePUse < pUseCounts[sourceLayerState]) {
				pUseCounts[sourceLayerState] = (short) candidatePUse;
				pushPUse(sourceLayerState);
			}
		};

		rep0(dir, 4) {
			int targetState = (targetCell << 2) | dir;
			int targetLayerState = (targetState << 2) | dir;
			pUseCounts[targetLayerState] = 0;
			pushPUse(targetLayerState);
		}

		while (head < tail) {
			int currentLayerState = (unsigned short) que[head++];
			int currentState = currentLayerState >> 2;
			int endDir = currentLayerState & 3;
			int currentDistance = dirDistances != nullptr ? dirDistances[currentLayerState]
				: route.distanceForDir(currentState, endDir);
			int sourceDistance = currentDistance + 1;
			int currentPUse = pUseCounts[currentLayerState];
			int nonPBase = currentState * 3;

			relaxPUse(nonPPreds[nonPBase], endDir, sourceDistance, currentPUse);
			relaxPUse(nonPPreds[nonPBase + 1], endDir, sourceDistance, currentPUse);
			relaxPUse(nonPPreds[nonPBase + 2], endDir, sourceDistance, currentPUse);

			const short* it = pSources + pStart[currentState];
			const short* end = pSources + pStart[currentState + 1];
			while (it != end) {
				int sourceState = (unsigned short) *it++;
				relaxPUse(sourceState, endDir, sourceDistance, currentPUse + 1);
			}
		}
	}

	template <class Emit>
	void forEachRouteOperation(const RouteTable& route, int startState, const vector<short>& xNextStates,
		int targetEndDir, Emit&& emit) const {
		assert_ne(route.distanceForDir(startState, targetEndDir), INF);
		assert_ne(route.pUseCountForDir(startState, targetEndDir), INF);
		int currentState = startState;
		while (route.distanceForDir(currentState, targetEndDir) > 0) {
			int currentDistance = route.distanceForDir(currentState, targetEndDir);
			int currentPUseCount = route.pUseCountForDir(currentState, targetEndDir);
			auto tryMove = [&](int nextState, char operation) {
				if (route.distanceForDir(nextState, targetEndDir) != currentDistance - 1) {
					return false;
				}
				int nextPUseCount = route.pUseCountForDir(nextState, targetEndDir);
				if (nextPUseCount + (operation == 'P') != currentPUseCount) {
					return false;
				}
				emit(operation);
				currentState = nextState;
				return true;
			};

			if (!xNextStates.empty() && tryMove(xNextStates[currentState], 'P')) {
				continue;
			}

			if (tryMove(basicNextF[currentState], 'F')) {
				continue;
			}
			if (tryMove(basicNextR[currentState], 'R')) {
				continue;
			}
			if (tryMove(basicNextL[currentState], 'L')) {
				continue;
			}
			assert(false);
		}
	}

	string restoreRouteOperations(
		const RouteTable& route, int startState, const vector<short>& xNextStates, int targetEndDir) const {
		string operations;
		operations.reserve(route.distanceForDir(startState, targetEndDir));
		forEachRouteOperation(route, startState, xNextStates, targetEndDir, [&](char operation) {
			operations.push_back(operation);
		});
		return operations;
	}

	string restoreRouteOperations(
		const RouteTable& route, int startState, const vector<short>& xNextStates) const {
		return restoreRouteOperations(route, startState, xNextStates, route.endDir(startState));
	}

	FirstRegistrationRoute buildFirstRegistrationRoute(
		const vector<short>& xNextStates, const MacroRegistration& registration) {
		assert(registration.compressed);
		int numStates = p.n * p.n * 4;
		constexpr int NUM_REGISTRATION_LAYERS = 3;
		constexpr int UNREGISTERED_LAYER = 0;
		constexpr int Y_REGISTERED_LAYER = 1;
		constexpr int X_REGISTERED_LAYER = 2;
		int numLayerStates = numStates * NUM_REGISTRATION_LAYERS;
		vector<short> yNextStates;
		getXNextStates(registration.y, yNextStates);

		FirstRegistrationRoute route;
		route.distance.assign(numLayerStates, INF);
		route.expanded.assign(numLayerStates, INF);
		route.previous.assign(numLayerStates, -1);
		route.previousOperation.assign(numLayerStates, '\0');

		int yRegistrationCost = len(registration.y) + 2;
		int xRegistrationCost = len(registration.encodedX) + 2;
		int yExpandedCost = len(registration.y);
		int xExpandedCost = registration.expandedLength - yExpandedCost;
		int maxBucketDistance = numLayerStates + yRegistrationCost + xRegistrationCost + 10;
		vector<int> bucketHead(maxBucketDistance + 1, -1);
		vector<int> entryNext;
		vector<int> entryState;
		vector<int> entryDistance;
		entryNext.reserve(numLayerStates * 8);
		entryState.reserve(numLayerStates * 8);
		entryDistance.reserve(numLayerStates * 8);
		int queuedCount = 0;

		auto pushBucket = [&](int state, int distance) {
			if (distance >= len(bucketHead)) {
				bucketHead.resize(distance + 1, -1);
			}
			int entry = len(entryState);
			entryState.push_back(state);
			entryDistance.push_back(distance);
			entryNext.push_back(bucketHead[distance]);
			bucketHead[distance] = entry;
			queuedCount++;
		};

		auto relax = [&](int currentLayerState, int nextLayerState, int nextDistance, int expandedDelta,
						 char operation) {
			int nextExpanded = route.expanded[currentLayerState] + expandedDelta;
			if (route.distance[nextLayerState] < nextDistance) {
				return;
			}
			if (route.distance[nextLayerState] == nextDistance &&
				route.expanded[nextLayerState] <= nextExpanded) {
				return;
			}
			route.distance[nextLayerState] = nextDistance;
			route.expanded[nextLayerState] = nextExpanded;
			route.previous[nextLayerState] = currentLayerState;
			route.previousOperation[nextLayerState] = operation;
			pushBucket(nextLayerState, nextDistance);
		};

		int startState = stateId({0, 0}, 1);
		int startLayerState = startState * NUM_REGISTRATION_LAYERS + UNREGISTERED_LAYER;
		route.distance[startLayerState] = 0;
		route.expanded[startLayerState] = 0;
		pushBucket(startLayerState, 0);

		int distance = 0;
		while (queuedCount > 0) {
			while (distance < len(bucketHead) && bucketHead[distance] == -1) {
				distance++;
			}
			if (distance == len(bucketHead)) {
				break;
			}
			int entry = bucketHead[distance];
			bucketHead[distance] = entryNext[entry];
			queuedCount--;
			int currentLayerState = entryState[entry];
			if (route.distance[currentLayerState] != distance) {
				continue;
			}

			int currentState = currentLayerState / NUM_REGISTRATION_LAYERS;
			int currentLayer = currentLayerState % NUM_REGISTRATION_LAYERS;

			if (currentLayer == Y_REGISTERED_LAYER) {
				relax(currentLayerState,
					yNextStates[currentState] * NUM_REGISTRATION_LAYERS + Y_REGISTERED_LAYER, distance + 1,
					yExpandedCost, 'Q');
			} else if (currentLayer == X_REGISTERED_LAYER) {
				relax(currentLayerState,
					xNextStates[currentState] * NUM_REGISTRATION_LAYERS + X_REGISTERED_LAYER, distance + 1,
					xExpandedCost, 'P');
			}

			relax(currentLayerState, basicNextF[currentState] * NUM_REGISTRATION_LAYERS + currentLayer,
				distance + 1, 1, 'F');
			relax(currentLayerState, basicNextR[currentState] * NUM_REGISTRATION_LAYERS + currentLayer,
				distance + 1, 1, 'R');
			relax(currentLayerState, basicNextL[currentState] * NUM_REGISTRATION_LAYERS + currentLayer,
				distance + 1, 1, 'L');

			if (currentLayer == UNREGISTERED_LAYER) {
				relax(currentLayerState,
					yNextStates[currentState] * NUM_REGISTRATION_LAYERS + Y_REGISTERED_LAYER,
					distance + yRegistrationCost, yExpandedCost, 'A');
			} else if (currentLayer == Y_REGISTERED_LAYER) {
				relax(currentLayerState,
					xNextStates[currentState] * NUM_REGISTRATION_LAYERS + X_REGISTERED_LAYER,
					distance + xRegistrationCost, xExpandedCost, 'B');
			}
		}
		return route;
	}

	FirstRegistrationTargetRoute buildFirstRegistrationTargetRoute(pii target,
		const ReverseGraph& xReverseGraph, const ReverseGraph& yReverseGraph,
		const MacroRegistration& registration) const {
		assert(registration.compressed);
		int numStates = p.n * p.n * 4;
		constexpr int NUM_REGISTRATION_LAYERS = 3;
		constexpr int UNREGISTERED_LAYER = 0;
		constexpr int Y_REGISTERED_LAYER = 1;
		constexpr int X_REGISTERED_LAYER = 2;
		int numLayerStates = numStates * NUM_REGISTRATION_LAYERS;

		FirstRegistrationTargetRoute route;
		route.distance.assign(numLayerStates, INF);
		route.expanded.assign(numLayerStates, INF);
		route.next.assign(numLayerStates, -1);
		route.operation.assign(numLayerStates, '\0');
		route.endState.assign(numLayerStates, -1);

		int yRegistrationCost = len(registration.y) + 2;
		int xRegistrationCost = len(registration.encodedX) + 2;
		int yExpandedCost = len(registration.y);
		int xExpandedCost = registration.expandedLength - yExpandedCost;
		int maxBucketDistance = numLayerStates + yRegistrationCost + xRegistrationCost + 10;
		vector<int> bucketHead(maxBucketDistance + 1, -1);
		vector<int> entryNext;
		vector<int> entryState;
		vector<int> entryDistance;
		entryNext.reserve(numLayerStates * 8);
		entryState.reserve(numLayerStates * 8);
		entryDistance.reserve(numLayerStates * 8);
		int queuedCount = 0;

		auto pushBucket = [&](int state, int distance) {
			if (distance >= len(bucketHead)) {
				bucketHead.resize(distance + 1, -1);
			}
			int entry = len(entryState);
			entryState.push_back(state);
			entryDistance.push_back(distance);
			entryNext.push_back(bucketHead[distance]);
			bucketHead[distance] = entry;
			queuedCount++;
		};

		auto relax = [&](int sourceLayerState, int nextLayerState, int nextDistance, int expandedDelta,
						 char operation) {
			int nextExpanded = route.expanded[nextLayerState] + expandedDelta;
			if (route.distance[sourceLayerState] < nextDistance) {
				return;
			}
			if (route.distance[sourceLayerState] == nextDistance &&
				route.expanded[sourceLayerState] <= nextExpanded) {
				return;
			}
			route.distance[sourceLayerState] = nextDistance;
			route.expanded[sourceLayerState] = nextExpanded;
			route.next[sourceLayerState] = nextLayerState;
			route.operation[sourceLayerState] = operation;
			route.endState[sourceLayerState] = route.endState[nextLayerState];
			pushBucket(sourceLayerState, nextDistance);
		};

		int targetCell = id(target);
		rep0(dir, 4) {
			int targetState = (targetCell << 2) | dir;
			int targetLayerState = targetState * NUM_REGISTRATION_LAYERS + X_REGISTERED_LAYER;
			route.distance[targetLayerState] = 0;
			route.expanded[targetLayerState] = 0;
			route.endState[targetLayerState] = (short) targetState;
			pushBucket(targetLayerState, 0);
		}

		int distance = 0;
		while (queuedCount > 0) {
			while (distance < len(bucketHead) && bucketHead[distance] == -1) {
				distance++;
			}
			if (distance == len(bucketHead)) {
				break;
			}
			int entry = bucketHead[distance];
			bucketHead[distance] = entryNext[entry];
			queuedCount--;
			int currentLayerState = entryState[entry];
			if (route.distance[currentLayerState] != distance) {
				continue;
			}

			int currentState = currentLayerState / NUM_REGISTRATION_LAYERS;
			int currentLayer = currentLayerState % NUM_REGISTRATION_LAYERS;
			int nonPBase = currentState * 3;
			relax(xReverseGraph.nonPPreds[nonPBase] * NUM_REGISTRATION_LAYERS + currentLayer,
				currentLayerState, distance + 1, 1, 'R');
			relax(xReverseGraph.nonPPreds[nonPBase + 1] * NUM_REGISTRATION_LAYERS + currentLayer,
				currentLayerState, distance + 1, 1, 'L');
			relax(xReverseGraph.nonPPreds[nonPBase + 2] * NUM_REGISTRATION_LAYERS + currentLayer,
				currentLayerState, distance + 1, 1, 'F');

			if (currentLayer == X_REGISTERED_LAYER) {
				rep(i, xReverseGraph.pStart[currentState], xReverseGraph.pStart[currentState + 1]) {
					int sourceState = xReverseGraph.pSources[i];
					relax(sourceState * NUM_REGISTRATION_LAYERS + X_REGISTERED_LAYER, currentLayerState,
						distance + 1, xExpandedCost, 'P');
					relax(sourceState * NUM_REGISTRATION_LAYERS + Y_REGISTERED_LAYER, currentLayerState,
						distance + xRegistrationCost, xExpandedCost, 'B');
				}
			} else if (currentLayer == Y_REGISTERED_LAYER) {
				rep(i, yReverseGraph.pStart[currentState], yReverseGraph.pStart[currentState + 1]) {
					int sourceState = yReverseGraph.pSources[i];
					relax(sourceState * NUM_REGISTRATION_LAYERS + Y_REGISTERED_LAYER, currentLayerState,
						distance + 1, yExpandedCost, 'Q');
					relax(sourceState * NUM_REGISTRATION_LAYERS + UNREGISTERED_LAYER, currentLayerState,
						distance + yRegistrationCost, yExpandedCost, 'A');
				}
			}
		}

		return route;
	}

	void buildPpSwitchRoute(pii target, const ReverseGraph& xReverseGraph,
		const ReverseGraph& switchReverseGraph, const PpSwitchRoute& route, vector<short>& que, int xLength,
		int switchExpandedLength) const {
		constexpr int NUM_PP_LAYERS = 2;
		constexpr int BEFORE_PP_LAYER = 0;
		constexpr int AFTER_PP_LAYER = 1;
		int numLayerStates = numStates * NUM_PP_LAYERS;

		short* __restrict distancePtr = route.distance;
		short* __restrict endStatePtr = route.endState;
		int* __restrict expandedPtr = route.expanded;
		const short* __restrict xNonPPreds = xReverseGraph.nonPPreds;
		const short* __restrict xPStart = xReverseGraph.pStart.data();
		const short* __restrict xPSources = xReverseGraph.pSources.data();
		const short* __restrict switchPStart = switchReverseGraph.pStart.data();
		const short* __restrict switchPSources = switchReverseGraph.pSources.data();

		fill(distancePtr, distancePtr + numLayerStates, INF_DIST);
		fill(expandedPtr, expandedPtr + numLayerStates, INF);
		if (len(que) < numLayerStates * 8) {
			que.resize(numLayerStates * 8);
		}
		int head = 0;
		int tail = 0;

#define PP_RELAX(sourceLayerState__, nextLayerState__, nextDistance__, expandedDelta__) \
	do {                                                                                \
		int sourceLayerState_ = (sourceLayerState__);                                   \
		int nextLayerState_ = (nextLayerState__);                                       \
		int nextExpanded_ = expandedPtr[nextLayerState_] + (expandedDelta__);           \
		if (distancePtr[sourceLayerState_] == INF_DIST ||                               \
			distancePtr[sourceLayerState_] > (nextDistance__) ||                        \
			(distancePtr[sourceLayerState_] == (nextDistance__) &&                      \
				expandedPtr[sourceLayerState_] > nextExpanded_)) {                      \
			distancePtr[sourceLayerState_] = (short) (nextDistance__);                  \
			expandedPtr[sourceLayerState_] = nextExpanded_;                             \
			endStatePtr[sourceLayerState_] = endStatePtr[nextLayerState_];              \
			if (tail >= len(que)) {                                                     \
				que.resize(len(que) * 2);                                               \
			}                                                                           \
			que[tail++] = (short) sourceLayerState_;                                    \
		}                                                                               \
	} while (false)

		int targetCell = id(target);
		rep0(dir, 4) {
			int targetState = (targetCell << 2) | dir;
			int targetLayerState = (targetState << 1) | AFTER_PP_LAYER;
			distancePtr[targetLayerState] = 0;
			expandedPtr[targetLayerState] = 0;
			endStatePtr[targetLayerState] = (short) targetState;
			que[tail++] = (short) targetLayerState;
		}

		while (head < tail) {
			int currentLayerState = (unsigned short) que[head++];
			int distance = distancePtr[currentLayerState];
			int currentState = currentLayerState >> 1;
			int currentLayer = currentLayerState & 1;
			int nextDistance = distance + 1;

			int nonPBase = currentState * 3;
			PP_RELAX((xNonPPreds[nonPBase] << 1) | currentLayer, currentLayerState, nextDistance, 1);
			PP_RELAX((xNonPPreds[nonPBase + 1] << 1) | currentLayer, currentLayerState, nextDistance, 1);
			PP_RELAX((xNonPPreds[nonPBase + 2] << 1) | currentLayer, currentLayerState, nextDistance, 1);

			if (currentLayer == AFTER_PP_LAYER) {
				const short* it = switchPSources + switchPStart[currentState];
				const short* end = switchPSources + switchPStart[currentState + 1];
				while (it != end) {
					int sourceState = (unsigned short) *it++;
					PP_RELAX((sourceState << 1) | AFTER_PP_LAYER, currentLayerState, nextDistance,
						switchExpandedLength);
					PP_RELAX((sourceState << 1) | BEFORE_PP_LAYER, currentLayerState, nextDistance,
						switchExpandedLength);
				}
			} else {
				const short* it = xPSources + xPStart[currentState];
				const short* end = xPSources + xPStart[currentState + 1];
				while (it != end) {
					int sourceState = (unsigned short) *it++;
					PP_RELAX((sourceState << 1) | BEFORE_PP_LAYER, currentLayerState, nextDistance, xLength);
				}
			}
		}
#undef PP_RELAX
	}

	template <class Emit>
	void forEachFirstRegistrationPrefixOperation(
		const FirstRegistrationRoute& route, int targetLayerState, Emit&& emit) const {
		assert_ne(route.distance[targetLayerState], INF);
		static vector<char> operations;
		operations.clear();
		while (route.previous[targetLayerState] != -1) {
			operations.push_back(route.previousOperation[targetLayerState]);
			targetLayerState = route.previous[targetLayerState];
		}
		for (int index = len(operations) - 1; index >= 0; index--) {
			emit(operations[index]);
		}
	}

	string restoreFirstRegistrationPrefixRoute(
		const FirstRegistrationRoute& route, int targetLayerState) const {
		string operations;
		operations.reserve(route.distance[targetLayerState]);
		forEachFirstRegistrationPrefixOperation(route, targetLayerState, [&](char operation) {
			operations.push_back(operation);
		});
		return operations;
	}

	template <class Emit>
	void forEachFirstRegistrationTargetOperation(
		const FirstRegistrationTargetRoute& route, int sourceLayerState, Emit&& emit) const {
		assert_ne(route.distance[sourceLayerState], INF);
		while (route.next[sourceLayerState] != -1) {
			emit(route.operation[sourceLayerState]);
			sourceLayerState = route.next[sourceLayerState];
		}
	}

	string restoreFirstRegistrationTargetRoute(
		const FirstRegistrationTargetRoute& route, int sourceLayerState) const {
		string operations;
		operations.reserve(route.distance[sourceLayerState]);
		forEachFirstRegistrationTargetOperation(route, sourceLayerState, [&](char operation) {
			operations.push_back(operation);
		});
		return operations;
	}

	template <class Emit>
	void forEachPpSwitchRouteOperation(const PpSwitchRoute& route, int sourceLayerState,
		const short* xNextStates, const short* switchNextStates, int xLength, int switchExpandedLength,
		Emit&& emit) const {
		assert_ne(route.distance[sourceLayerState], INF_DIST);
		const short* __restrict nextF = basicNextF.data();
		const short* __restrict nextR = basicNextR.data();
		const short* __restrict nextL = basicNextL.data();

		auto tryMove = [&](int nextLayerState, char operation, int expandedDelta) {
			if (route.distance[nextLayerState] != route.distance[sourceLayerState] - 1) {
				return false;
			}
			if (route.endState[nextLayerState] != route.endState[sourceLayerState]) {
				return false;
			}
			if (route.expanded[nextLayerState] + expandedDelta != route.expanded[sourceLayerState]) {
				return false;
			}
			emit(operation);
			sourceLayerState = nextLayerState;
			return true;
		};

		while (route.distance[sourceLayerState] > 0) {
			int currentState = sourceLayerState >> 1;
			int currentLayer = sourceLayerState & 1;
			if (currentLayer == 1) {
				if (tryMove(((unsigned short) switchNextStates[currentState] << 1) | 1, 'Z',
						switchExpandedLength)) {
					continue;
				}
			} else {
				if (tryMove(((unsigned short) switchNextStates[currentState] << 1) | 1, 'Z',
						switchExpandedLength)) {
					continue;
				}
				if (tryMove(((unsigned short) xNextStates[currentState] << 1), 'P', xLength)) {
					continue;
				}
			}
			if (tryMove(((unsigned short) nextF[currentState] << 1) | currentLayer, 'F', 1)) {
				continue;
			}
			if (tryMove(((unsigned short) nextR[currentState] << 1) | currentLayer, 'R', 1)) {
				continue;
			}
			if (tryMove(((unsigned short) nextL[currentState] << 1) | currentLayer, 'L', 1)) {
				continue;
			}
			assert(false);
		}
	}

	string restorePpSwitchRoute(const PpSwitchRoute& route, int sourceLayerState, const short* xNextStates,
		const short* switchNextStates, int xLength, int switchExpandedLength) const {
		string operations;
		operations.reserve(route.distance[sourceLayerState]);
		forEachPpSwitchRouteOperation(route, sourceLayerState, xNextStates, switchNextStates, xLength,
			switchExpandedLength, [&](char operation) {
				operations.push_back(operation);
			});
		return operations;
	}

	optional<BeamOperationsResult> buildBeamOperations(const vector<short>& xNextStates,
		const MacroRegistration& registration, const BeamConfig& beamConfig,
		const PpSwitchMacro* ppSwitchMacro) {
		lastBeamFailureReason.clear();
		static ReverseGraph reverseGraph;
		buildReverseGraph(xNextStates, reverseGraph);
		int numCells = p.n * p.n;
		int numStates = numCells * 4;
		int xLength = registration.compressed ? registration.expandedLength - len(registration.y)
											  : len(registration.encodedX);
		constexpr int NUM_PP_LAYERS = 2;
		constexpr int BEFORE_PP_LAYER = 0;
		constexpr int AFTER_PP_LAYER = 1;
		static ReverseGraph switchReverseGraph;
		int ppSwitchExtraOutputCost = 0;
		int ppSwitchExpandedLength = 0;
		if (beamConfig.enablePpSwitch) {
			assert(ppSwitchMacro != nullptr);
			ppSwitchExtraOutputCost = ppSwitchMacro->firstZExtraOutputCost;
			ppSwitchExpandedLength = ppSwitchMacro->expandedLength;
			buildReverseGraph(ppSwitchMacro->nextStates, switchReverseGraph);
		}
		optional<FirstRegistrationRoute> firstRegistrationRoute;
		vector<FirstRegistrationTargetRoute> firstBasketRoutes;
		if (registration.compressed) {
			firstRegistrationRoute = buildFirstRegistrationRoute(xNextStates, registration);
			vector<short> yNextStates;
			getXNextStates(registration.y, yNextStates);
			static ReverseGraph yReverseGraph;
			buildReverseGraph(yNextStates, yReverseGraph);
			firstBasketRoutes.resize(p.m);
			rep0(ball, p.m) {
				firstBasketRoutes[ball] = buildFirstRegistrationTargetRoute(
					p.basketPos[ball], reverseGraph, yReverseGraph, registration);
			}
		}
		static vector<short> routePacked;
		static vector<char> routeDirMasks;
		static vector<short> routePUseCounts;
		static vector<short> routeDirDistances;
		static vector<short> routeQueue;
		static vector<RouteTable> ballRoutes;
		static vector<RouteTable> basketRoutes;
		static vector<short> ballStates;
		static vector<short> basketStates;
		routePacked.resize(2 * p.m * numStates);
		routeDirMasks.resize(2 * p.m * numStates);
		routePUseCounts.resize(2 * p.m * numStates * 4);
		routeDirDistances.resize(2 * p.m * numStates * 4);
		routeQueue.resize(numStates * 8);
		ballRoutes.resize(p.m);
		basketRoutes.resize(p.m);
		ballStates.resize(p.m * 4);
		basketStates.resize(p.m * 4);
		auto ballDirIndex = [](int ball, int dir) {
			return (ball << 2) | dir;
		};
		rep0(ball, p.m) {
			int ballCellId = id(p.ballPos[ball]);
			rep0(dir, 4) {
				int index = ballDirIndex(ball, dir);
				ballStates[index] = (short) ((ballCellId << 2) | dir);
			}
		}
		rep0(ball, p.m) {
			int basketCellId = id(p.basketPos[ball]);
			rep0(dir, 4) {
				int index = ballDirIndex(ball, dir);
				basketStates[index] = (short) ((basketCellId << 2) | dir);
			}
		}
		auto routeTable = [&](int routeIndex) {
			int offset = routeIndex * numStates;
			int pUseOffset = routeIndex * numStates * 4;
			return RouteTable{routePacked.data() + offset, routeDirMasks.data() + offset,
				routePUseCounts.data() + pUseOffset, routeDirDistances.data() + pUseOffset};
		};
		rep0(index, p.m) {
			ballRoutes[index] = routeTable(index * 2);
			basketRoutes[index] = routeTable(index * 2 + 1);
			buildRouteTable(p.ballPos[index], reverseGraph, ballRoutes[index], routeQueue);
			buildRouteTable(p.basketPos[index], reverseGraph, basketRoutes[index], routeQueue);
		}
		static vector<PpSwitchRoute> ppSwitchBallRoutes;
		static vector<PpSwitchRoute> ppSwitchBasketRoutes;
		static vector<short> ppSwitchDistanceStorage;
		static vector<short> ppSwitchEndStateStorage;
		static vector<int> ppSwitchExpandedStorage;
		static vector<short> ppSwitchQueue;
		if (beamConfig.enablePpSwitch) {
			int numPpLayerStates = numStates * NUM_PP_LAYERS;
			int numPpRoutes = p.m * 2;
			ppSwitchBallRoutes.resize(p.m);
			ppSwitchBasketRoutes.resize(p.m);
			ppSwitchDistanceStorage.resize(numPpRoutes * numPpLayerStates);
			ppSwitchEndStateStorage.resize(numPpRoutes * numPpLayerStates);
			ppSwitchExpandedStorage.resize(numPpRoutes * numPpLayerStates);
			ppSwitchQueue.resize(numPpLayerStates);
			auto ppRoute = [&](int routeIndex) {
				int offset = routeIndex * numPpLayerStates;
				return PpSwitchRoute{ppSwitchDistanceStorage.data() + offset,
					ppSwitchEndStateStorage.data() + offset, ppSwitchExpandedStorage.data() + offset};
			};
			rep0(index, p.m) {
				ppSwitchBallRoutes[index] = ppRoute(index * 2);
				ppSwitchBasketRoutes[index] = ppRoute(index * 2 + 1);
				buildPpSwitchRoute(p.ballPos[index], reverseGraph, switchReverseGraph,
					ppSwitchBallRoutes[index], ppSwitchQueue, xLength, ppSwitchExpandedLength);
				buildPpSwitchRoute(p.basketPos[index], reverseGraph, switchReverseGraph,
					ppSwitchBasketRoutes[index], ppSwitchQueue, xLength, ppSwitchExpandedLength);
			}
		}
		static vector<int> minStoreCosts;
		static vector<int> minStoreCostsXX;
		minStoreCosts.resize(p.m);
		minStoreCostsXX.assign(p.m, 0);
		rep0(ball, p.m) {
			const RouteTable& basketRoute = basketRoutes[ball];
			int minBasketDistance = INF;
			rep0(dir, 4) {
				int pickupState = ballStates[ballDirIndex(ball, dir)];
				minBasketDistance = min(minBasketDistance, basketRoute.distance(pickupState));
			}
			assert_ne(minBasketDistance, INF_DIST);
			minStoreCosts[ball] = minBasketDistance + 2;
		}
		if (beamConfig.enablePpSwitch) {
			rep0(ball, p.m) {
				const PpSwitchRoute& basketRoute = ppSwitchBasketRoutes[ball];
				int minBasketDistance = INF;
				rep0(dir, 4) {
					int pickupState = ballStates[ballDirIndex(ball, dir)];
					int layerState = pickupState * NUM_PP_LAYERS + AFTER_PP_LAYER;
					minBasketDistance = min(minBasketDistance, (int) basketRoute.distance[layerState]);
				}
				assert_ne(minBasketDistance, INF_DIST);
				minStoreCostsXX[ball] = minBasketDistance + 2;
			}
		}
		static vector<int> cellToBallMin;
		static vector<int> basketToBallMin;
		static vector<int> cellToBallMinXX;
		static vector<int> basketToBallMinXX;
		static vector<short> basketOrderByBall;
		static vector<short> basketOrderByBallXX;
		static vector<short> basketOrderWork;
		cellToBallMin.assign(numCells * p.m, INF);
		basketToBallMin.assign(p.m * p.m, INF);
		rep0(cell, numCells) {
			int baseState = cell << 2;
			rep0(ball, p.m) {
				const RouteTable& ballRoute = ballRoutes[ball];
				int bestDistance = INF;
				rep0(dir, 4) {
					bestDistance = min(bestDistance, ballRoute.distance(baseState | dir));
				}
				cellToBallMin[cell * p.m + ball] = bestDistance;
			}
		}
		rep0(basketBall, p.m) {
			int basketCell = id(p.basketPos[basketBall]);
			rep0(ball, p.m) {
				basketToBallMin[basketBall * p.m + ball] = cellToBallMin[basketCell * p.m + ball];
			}
		}
		auto buildBasketOrderByBall = [&](const vector<int>& currentBasketToBallMin,
										  vector<short>& currentBasketOrderByBall) {
			currentBasketOrderByBall.resize(p.m * p.m);
			basketOrderWork.resize(p.m);
			rep0(targetBall, p.m) {
				rep0(basketBall, p.m) {
					basketOrderWork[basketBall] = (short) basketBall;
				}
				sort(basketOrderWork.begin(), basketOrderWork.end(), [&](short a, short b) {
					int distanceA = currentBasketToBallMin[(int) a * p.m + targetBall];
					int distanceB = currentBasketToBallMin[(int) b * p.m + targetBall];
					if (distanceA != distanceB) {
						return distanceA < distanceB;
					}
					return a < b;
				});
				int orderBase = targetBall * p.m;
				rep0(index, p.m) {
					currentBasketOrderByBall[orderBase + index] = basketOrderWork[index];
				}
			}
		};
		buildBasketOrderByBall(basketToBallMin, basketOrderByBall);
		if (beamConfig.enablePpSwitch) {
			cellToBallMinXX.assign(numCells * p.m, INF);
			basketToBallMinXX.assign(p.m * p.m, INF);
			rep0(cell, numCells) {
				int baseState = cell << 2;
				rep0(ball, p.m) {
					const PpSwitchRoute& ballRoute = ppSwitchBallRoutes[ball];
					int bestDistance = INF;
					rep0(dir, 4) {
						int layerState = (baseState | dir) * NUM_PP_LAYERS + AFTER_PP_LAYER;
						bestDistance = min(bestDistance, (int) ballRoute.distance[layerState]);
					}
					cellToBallMinXX[cell * p.m + ball] = bestDistance;
				}
			}
			rep0(basketBall, p.m) {
				int basketCell = id(p.basketPos[basketBall]);
				rep0(ball, p.m) {
					basketToBallMinXX[basketBall * p.m + ball] = cellToBallMinXX[basketCell * p.m + ball];
				}
			}
			buildBasketOrderByBall(basketToBallMinXX, basketOrderByBallXX);
		}
		if (p.m == 0) {
			return BeamOperationsResult{};
		}

		assert_le(p.m, 64);

		static vector<BeamMove> beamMoves;
		beamMoves.clear();
		beamMoves.reserve(
			p.m * beamConfig.beamWidth * beamConfig.sameParentCount + beamConfig.sameParentCount);
		using Beam = shr::beam_search::beam_search<BeamState, int, int, -1, BeamCandidateData, less<int>, 18>;
		static Beam beam;
		beam.max_turn = p.m;

		auto appendConcreteOperation = [&](SmallOutputTail& outputTail, int& outputLength, bool& xRegistered,
										   bool& ppSwitched, char operation) {
			auto appendChar = [&](char c) {
				outputTail.append(c);
				outputLength++;
			};
			auto appendString = [&](string_view s) {
				outputTail.append(s);
				outputLength += (int) s.size();
			};

			if (operation == 'A') {
				appendChar('M');
				appendString(registration.y);
				appendChar('M');
			} else if (operation == 'B') {
				appendChar('M');
				appendString(registration.encodedX);
				appendChar('M');
				xRegistered = true;
			} else if (operation == 'Q') {
				appendChar('P');
			} else if (operation == 'G') {
				appendString(registration.operations);
				xRegistered = true;
			} else if (operation == 'P') {
				if (registration.compressed) {
					assert(xRegistered);
					appendChar('P');
				} else if (!xRegistered) {
					appendChar('M');
					appendString(registration.encodedX);
					appendChar('M');
					xRegistered = true;
				} else {
					appendChar('P');
				}
			} else if (operation == 'Z') {
				assert(xRegistered);
				assert(beamConfig.enablePpSwitch);
				assert(ppSwitchMacro != nullptr);
				if (!ppSwitched) {
					appendChar('M');
					appendString(ppSwitchMacro->pattern);
					appendChar('M');
					ppSwitched = true;
				} else {
					appendChar('P');
				}
			} else {
				appendChar(operation);
			}
		};

		auto appendMoveOutput = [&](SmallOutputTail& outputTail, int& outputLength, bool& xRegistered,
									bool& ppSwitched, const BeamMove& move) {
			auto emit = [&](char operation) {
				appendConcreteOperation(outputTail, outputLength, xRegistered, ppSwitched, operation);
			};
			if (move.useFirstRegistrationRoute) {
				assert(firstRegistrationRoute);
				forEachFirstRegistrationPrefixOperation(
					*firstRegistrationRoute, move.firstPickupLayerState, emit);
			} else if (move.usePpSwitchPickupRoute) {
				forEachPpSwitchRouteOperation(ppSwitchBallRoutes[move.pickupBall],
					move.ppPickupStartLayerState, xNextStates.data(), ppSwitchMacro->nextStates.data(),
					xLength, ppSwitchExpandedLength, emit);
			} else {
				forEachRouteOperation(
					ballRoutes[move.pickupBall], move.pickupStartState, xNextStates, move.pickupEndDir, emit);
			}
			emit('S');
			if (move.useFirstRegistrationRoute) {
				forEachFirstRegistrationTargetOperation(
					firstBasketRoutes[move.storedBall], move.firstPickupLayerState, emit);
			} else if (move.usePpSwitchBasketRoute) {
				forEachPpSwitchRouteOperation(ppSwitchBasketRoutes[move.storedBall],
					move.ppBasketStartLayerState, xNextStates.data(), ppSwitchMacro->nextStates.data(),
					xLength, ppSwitchExpandedLength, emit);
			} else {
				forEachRouteOperation(basketRoutes[move.storedBall], move.basketStartState, xNextStates,
					move.basketEndDir, emit);
			}
			emit('S');
		};

		auto terminalOptimizedScoreAfterMove = [&](const BeamState& st, const BeamMove& move) {
			SmallOutputTail outputTail = st.outputTail;
			int outputLength = st.outputLengthSoFar;
			bool xRegistered = st.xRegistered;
			bool ppSwitched = st.ppSwitched;
			appendMoveOutput(outputTail, outputLength, xRegistered, ppSwitched, move);
			assert_eq(xRegistered, move.nextXRegistered);
			assert_eq(ppSwitched, move.nextPpSwitched);
			return tailMacroOptimizedLength(outputLength, outputTail);
		};

		struct TerminalRecord {
			int score = INF;
			vector<int> moveIds;
		};
		static vector<TerminalRecord> terminalRecords;
		terminalRecords.clear();
		static vector<int> currentMovePath;
		currentMovePath.clear();
		static vector<BeamState> undoStack;
		undoStack.clear();

		auto getScore = [&](BeamState& st) {
			int score;
			if (st.storedCount == p.m && st.terminalOptimizedScore != INF) {
				score = st.terminalOptimizedScore;
			} else if (beamConfig.enablePpSwitch) {
				int remainingEstimate = st.ppSwitched
					? st.remainingLowerBoundXX + st.approachLowerBoundXX
					: min(st.remainingLowerBound + st.approachLowerBound,
						  ppSwitchExtraOutputCost + st.remainingLowerBoundXX + st.approachLowerBoundXX);
				score = st.executedCost + remainingEstimate;
			} else {
				score = st.executedCost + st.remainingLowerBound + st.approachLowerBound;
			}
			if (beamConfig.collectLastCandidates && st.storedCount == p.m) {
				terminalRecords.push_back({score, currentMovePath});
			}
			return score;
		};

		auto applyMove = [&](BeamState& st, int moveId) {
			if (beamConfig.collectLastCandidates) {
				undoStack.push_back(st);
				currentMovePath.push_back(moveId);
			}
			const BeamMove& move = beamMoves[moveId];
			bool xRegistered = st.xRegistered;
			bool ppSwitched = st.ppSwitched;
			appendMoveOutput(st.outputTail, st.outputLengthSoFar, xRegistered, ppSwitched, move);
			assert_eq(xRegistered, move.nextXRegistered);
			assert_eq(ppSwitched, move.nextPpSwitched);
			st.currentCell = move.nextState >> 2;
			st.currentDir = move.nextState & 3;
			st.storedCount++;
			st.storedBits = move.nextStoredBits;
			st.storedHash = move.nextStoredHash;
			st.xRegistered = move.nextXRegistered;
			st.ppSwitched = move.nextPpSwitched;
			st.executedCost += move.executedCostDelta;
			st.expandedCost += move.expandedCostDelta;
			st.remainingLowerBound += move.remainingLowerBoundDelta;
			st.remainingLowerBoundXX += move.remainingLowerBoundXXDelta;
			st.approachLowerBound += move.approachLowerBoundDelta;
			st.approachLowerBoundXX += move.approachLowerBoundXXDelta;
			if (st.storedCount == p.m) {
				st.terminalOptimizedScore = tailMacroOptimizedLength(st.outputLengthSoFar, st.outputTail);
			}
		};

		auto undoMove = [&](BeamState& st) {
			assert(beamConfig.collectLastCandidates);
			assert(!undoStack.empty());
			assert(!currentMovePath.empty());
			st = undoStack.back();
			undoStack.pop_back();
			currentMovePath.pop_back();
		};

		struct ReachableBall {
			int distance;
			int ball;
			int state;
			int firstPickupLayerState = -1;
			int firstPrefixDistance = INF;
			int firstBasketDistance = INF;
			int firstBasketEndState = -1;
		};
		static vector<ReachableBall> reachableBalls;
		reachableBalls.clear();
		reachableBalls.reserve(p.m);
		static vector<BeamMove> parentMoves;
		parentMoves.clear();
		parentMoves.reserve(p.m * 16);
		static vector<int> debugPreFilterCandidates;
		static vector<int> debugPostFilterCandidates;
		static vector<int> debugExpandedRejectedMoves;
		static vector<int> debugEnumeratedParentStates;
		static vector<int> debugParentMoves;
		static vector<int> debugReachableBalls;
		debugPreFilterCandidates.assign(p.m, -1);
		debugPostFilterCandidates.assign(p.m, -1);
		debugExpandedRejectedMoves.assign(p.m, 0);
		debugEnumeratedParentStates.assign(p.m, 0);
		debugParentMoves.assign(p.m, 0);
		debugReachableBalls.assign(p.m, 0);
		static vector<int> basketBestByBall;
		static vector<int> basketBestByBallXX;
		basketBestByBall.resize(p.m);
		if (beamConfig.enablePpSwitch) {
			basketBestByBallXX.resize(p.m);
		}

		auto computeApproachLowerBounds =
			[&](const BeamState& st, const vector<int>& currentCellToBallMin,
				const vector<int>& currentBasketToBallMin, const vector<short>& currentBasketOrderByBall,
				vector<int>& currentBasketBestByBall, int& basketOnlyApproachSum) {
				basketOnlyApproachSum = 0;
				int maxParentCurrentSaving = 0;
				int currentCellBallBase = st.currentCell * p.m;
				rep0(ball, p.m) {
					ull bit = 1ull << ball;
					if (st.storedBits & bit) {
						continue;
					}
					int orderBase = ball * p.m;
					int basketBest = INF;
					rep0(orderIndex, p.m) {
						int basketBall = currentBasketOrderByBall[orderBase + orderIndex];
						if (st.storedBits & (1ull << basketBall)) {
							continue;
						}
						basketBest = currentBasketToBallMin[basketBall * p.m + ball];
						break;
					}
					assert_ne(basketBest, INF);
					currentBasketBestByBall[ball] = basketBest;
					basketOnlyApproachSum += basketBest;
					maxParentCurrentSaving = max(maxParentCurrentSaving,
						basketBest - currentCellToBallMin[currentCellBallBase + ball]);
				}

				return basketOnlyApproachSum - max(0, maxParentCurrentSaving);
			};

		auto buildDirCandidates = [](int shortestMask, int nearMask, int* dirs) {
			int count = 0;
			rep0(dir, 4) {
				if (shortestMask & (1 << dir)) {
					dirs[count] = dir;
					count++;
				}
			}
			if (std::has_single_bit((unsigned) shortestMask)) {
				int oppositeDir = (RouteTable::firstDir(shortestMask) + 2) & 3;
				int oppositeBit = 1 << oppositeDir;
				if ((nearMask & oppositeBit) && !(shortestMask & oppositeBit)) {
					dirs[count] = oppositeDir;
					count++;
				}
			}
			return count;
		};

		auto beamMoveScore = [&](const BeamState& st, const BeamMove& move) {
			if (beamConfig.enablePpSwitch) {
				int executedCost = st.executedCost + move.executedCostDelta;
				int remainingX = st.remainingLowerBound + move.remainingLowerBoundDelta;
				int remainingXX = st.remainingLowerBoundXX + move.remainingLowerBoundXXDelta;
				int approachX = st.approachLowerBound + move.approachLowerBoundDelta;
				int approachXX = st.approachLowerBoundXX + move.approachLowerBoundXXDelta;
				int remainingEstimate = move.nextPpSwitched
					? remainingXX + approachXX
					: min(remainingX + approachX, ppSwitchExtraOutputCost + remainingXX + approachXX);
				return executedCost + remainingEstimate;
			}
			return st.executedCost + move.executedCostDelta + st.remainingLowerBound +
				move.remainingLowerBoundDelta + st.approachLowerBound + move.approachLowerBoundDelta;
		};

		auto beamMoveHash = [&](const BeamMove& move) {
			ull hash = move.nextStoredHash ^ stateHash[move.nextState];
			if (move.nextXRegistered) {
				hash ^= xRegistrationHash;
			}
			if (move.nextPpSwitched) {
				hash ^= ppSwitchHash;
			}
			return hash;
		};

		auto addBeamMove = [&](const BeamState& st, const BeamMove& move) {
			int moveId = len(beamMoves);
			ull hash = move.candidateHash;
			BeamCandidateData data{move.candidateScore};
			beamMoves.push_back(move);
			beam.add_candidate(moveId, data, hash);
		};

		auto isBetterParentMove = [](const BeamMove& a, const BeamMove& b) {
			if (a.candidateScore != b.candidateScore) {
				return a.candidateScore < b.candidateScore;
			}
			return a.candidateHash < b.candidateHash;
		};

		auto addParentMove = [&](const BeamState& st, BeamMove& move) {
			if (st.expandedCost + move.expandedCostDelta > p.numT) {
				debugExpandedRejectedMoves[st.storedCount]++;
				return;
			}
			move.candidateScore = st.storedCount + 1 == p.m ? terminalOptimizedScoreAfterMove(st, move)
															: beamMoveScore(st, move);
			move.candidateHash = beamMoveHash(move);
			int limit = beamConfig.sameParentCount;
			if (len(parentMoves) < limit) {
				parentMoves.push_back(move);
			} else if (limit > 0 && isBetterParentMove(move, parentMoves.back())) {
				parentMoves.back() = move;
			} else {
				return;
			}
			for (int i = len(parentMoves) - 1;
				i > 0 && isBetterParentMove(parentMoves[i], parentMoves[i - 1]); i--) {
				swap(parentMoves[i], parentMoves[i - 1]);
			}
		};

		auto enumerateCandidates = [&](BeamState& st) {
			if (st.storedCount == p.m) {
				return;
			}

			int currentState = (st.currentCell << 2) | st.currentDir;
			parentMoves.clear();
			debugEnumeratedParentStates[st.storedCount]++;
			int basketOnlyApproachSum = 0;
			computeApproachLowerBounds(st, cellToBallMin, basketToBallMin, basketOrderByBall,
				basketBestByBall, basketOnlyApproachSum);
			int parentApproachSum = st.approachLowerBound;
			int basketOnlyApproachSumXX = 0;
			int parentApproachSumXX = st.approachLowerBoundXX;
			if (beamConfig.enablePpSwitch) {
				computeApproachLowerBounds(st, cellToBallMinXX, basketToBallMinXX, basketOrderByBallXX,
					basketBestByBallXX, basketOnlyApproachSumXX);
			}

			reachableBalls.clear();
			bool useFirstRegistrationRoute = registration.compressed && st.storedCount == 0;
			if (useFirstRegistrationRoute) {
				assert(firstRegistrationRoute);
				const FirstRegistrationRoute& firstRoute = *firstRegistrationRoute;
				rep0(ball, p.m) {
					ull bit = 1ull << ball;
					if (st.storedBits & bit) {
						continue;
					}
					const FirstRegistrationTargetRoute& firstBasketRoute = firstBasketRoutes[ball];
					int bestTotalDistance = INF;
					int bestTotalExpanded = INF;
					int bestPrefixDistance = INF;
					int bestBasketDistance = INF;
					int bestState = -1;
					int bestLayerState = -1;
					int bestBasketEndState = -1;
					rep0(dir, 4) {
						int index = ballDirIndex(ball, dir);
						int state = ballStates[index];
						rep0(layer, 3) {
							int layerState = state * 3 + layer;
							int prefixDistance = firstRoute.distance[layerState];
							int basketDistance = firstBasketRoute.distance[layerState];
							if (prefixDistance == INF || basketDistance == INF) {
								continue;
							}
							int totalDistance = prefixDistance + 1 + basketDistance + 1;
							int totalExpanded = firstRoute.expanded[layerState] + 1 +
								firstBasketRoute.expanded[layerState] + 1;
							if (totalDistance < bestTotalDistance ||
								(totalDistance == bestTotalDistance && totalExpanded < bestTotalExpanded)) {
								bestTotalDistance = totalDistance;
								bestTotalExpanded = totalExpanded;
								bestPrefixDistance = prefixDistance;
								bestBasketDistance = basketDistance;
								bestLayerState = layerState;
								bestBasketEndState = firstBasketRoute.endState[layerState];
								bestState = state;
							}
						}
					}
					if (bestState == -1) {
						continue;
					}
					reachableBalls.push_back({bestTotalDistance, ball, bestState, bestLayerState,
						bestPrefixDistance, bestBasketDistance, bestBasketEndState});
				}
			} else if (!beamConfig.enablePpSwitch || !st.ppSwitched) {
				rep0(ball, p.m) {
					ull bit = 1ull << ball;
					if (st.storedBits & bit) {
						continue;
					}
					const RouteTable& ballRoute = ballRoutes[ball];
					int distance = ballRoute.distance(currentState);
					if (distance == INF_DIST) {
						continue;
					}
					reachableBalls.push_back({distance, ball, -1});
				}
			}

			rep0(i, len(reachableBalls)) {
				const ReachableBall& reachable = reachableBalls[i];
				int pickupDirs[5];
				int pickupCount = 0;
				if (useFirstRegistrationRoute) {
					pickupDirs[0] = reachable.state & 3;
					pickupCount = 1;
				} else {
					const RouteTable& ballRoute = ballRoutes[reachable.ball];
					pickupCount = buildDirCandidates(ballRoute.shortestDirMask(currentState),
						ballRoute.nearDirMask(currentState), pickupDirs);
				}
				rep0(pickupIndex, pickupCount) {
					int pickupDir = pickupDirs[pickupIndex];
					int pickupBall = reachable.ball;
					int pickupState = ballStates[ballDirIndex(pickupBall, pickupDir)];
					int toBallDistance = useFirstRegistrationRoute
						? reachable.firstPrefixDistance
						: ballRoutes[pickupBall].distanceForDir(currentState, pickupDir);
					if (toBallDistance == INF) {
						continue;
					}
					int toBallExpanded = useFirstRegistrationRoute
						? firstRegistrationRoute->expanded[reachable.firstPickupLayerState]
						: ballRoutes[pickupBall].expandedDistanceForDir(currentState, pickupDir, xLength);
					if (toBallExpanded == INF) {
						continue;
					}
					int toBallPUseCount = useFirstRegistrationRoute
						? 0
						: ballRoutes[pickupBall].pUseCountForDir(currentState, pickupDir);

					if (useFirstRegistrationRoute) {
						int basketDistance = reachable.firstBasketDistance;
						int basketEndState = reachable.firstBasketEndState;
						if (basketDistance == INF_DIST || basketEndState == -1) {
							continue;
						}
						int basketExpanded =
							firstBasketRoutes[pickupBall].expanded[reachable.firstPickupLayerState];
						if (basketExpanded == INF) {
							continue;
						}

						BeamMove move;
						move.pickupBall = pickupBall;
						move.storedBall = pickupBall;
						move.pickupStartState = currentState;
						move.basketStartState = pickupState;
						move.firstPickupLayerState = reachable.firstPickupLayerState;
						move.pickupEndDir = pickupDir;
						move.basketEndDir = basketEndState & 3;
						move.nextState = basketEndState;
						move.nextStoredBits = st.storedBits | (1ull << pickupBall);
						move.nextStoredHash = st.storedHash ^ ballHash[pickupBall];
						move.nextXRegistered = true;
						move.nextPpSwitched = st.ppSwitched;
						int actualCostDelta = toBallDistance + 1 + basketDistance + 1;
						move.executedCostDelta = actualCostDelta;
						move.expandedCostDelta = toBallExpanded + 1 + basketExpanded + 1;
						move.remainingLowerBoundDelta = -minStoreCosts[pickupBall];
						if (beamConfig.enablePpSwitch) {
							move.remainingLowerBoundXXDelta = -minStoreCostsXX[pickupBall];
							move.approachLowerBoundXXDelta = basketOnlyApproachSumXX -
								basketBestByBallXX[pickupBall] - parentApproachSumXX;
						}
						move.approachLowerBoundDelta =
							basketOnlyApproachSum - basketBestByBall[pickupBall] - parentApproachSum;
						move.useFirstRegistrationRoute = true;

						addParentMove(st, move);
						continue;
					}

					const RouteTable& basketRoute = basketRoutes[pickupBall];
					int basketDirs[5];
					int basketCount = buildDirCandidates(basketRoute.shortestDirMask(pickupState),
						basketRoute.nearDirMask(pickupState), basketDirs);
					rep0(basketIndex, basketCount) {
						int basketEndDir = basketDirs[basketIndex];
						int basketDistance = basketRoute.distanceForDir(pickupState, basketEndDir);
						if (basketDistance == INF) {
							continue;
						}
						int basketExpanded =
							basketRoute.expandedDistanceForDir(pickupState, basketEndDir, xLength);
						if (basketExpanded == INF) {
							continue;
						}
						int basketPUseCount = basketRoute.pUseCountForDir(pickupState, basketEndDir);

						BeamMove move;
						move.pickupBall = pickupBall;
						move.storedBall = pickupBall;
						move.pickupStartState = currentState;
						move.basketStartState = pickupState;
						move.pickupEndDir = pickupDir;
						move.basketEndDir = basketEndDir;
						move.nextState = basketStates[ballDirIndex(pickupBall, basketEndDir)];
						move.nextStoredBits = st.storedBits | (1ull << pickupBall);
						move.nextStoredHash = st.storedHash ^ ballHash[pickupBall];
						move.nextXRegistered = st.xRegistered || toBallPUseCount > 0 || basketPUseCount > 0;
						move.nextPpSwitched = st.ppSwitched;
						int actualCostDelta = toBallDistance + 1 + basketDistance + 1;
						move.executedCostDelta = actualCostDelta;
						move.expandedCostDelta = toBallExpanded + 1 + basketExpanded + 1;
						move.remainingLowerBoundDelta = -minStoreCosts[pickupBall];
						if (beamConfig.enablePpSwitch) {
							move.remainingLowerBoundXXDelta = -minStoreCostsXX[pickupBall];
							move.approachLowerBoundXXDelta = basketOnlyApproachSumXX -
								basketBestByBallXX[pickupBall] - parentApproachSumXX;
						}
						move.approachLowerBoundDelta =
							basketOnlyApproachSum - basketBestByBall[pickupBall] - parentApproachSum;
						move.useFirstRegistrationRoute = useFirstRegistrationRoute;

						addParentMove(st, move);
					}

					if (beamConfig.enablePpSwitch && !st.ppSwitched && st.storedCount > 0 &&
						(st.xRegistered || toBallPUseCount > 0)) {
						const PpSwitchRoute& ppBasketRoute = ppSwitchBasketRoutes[pickupBall];
						int basketStartLayerState = pickupState * NUM_PP_LAYERS + BEFORE_PP_LAYER;
						int basketDistance = ppBasketRoute.distance[basketStartLayerState];
						int basketEndState = ppBasketRoute.endState[basketStartLayerState];
						if (basketDistance != INF_DIST && basketEndState != -1) {
							int basketExpanded = ppBasketRoute.expanded[basketStartLayerState];
							BeamMove move;
							move.pickupBall = pickupBall;
							move.storedBall = pickupBall;
							move.pickupStartState = currentState;
							move.basketStartState = pickupState;
							move.ppBasketStartLayerState = basketStartLayerState;
							move.pickupEndDir = pickupDir;
							move.basketEndDir = basketEndState & 3;
							move.nextState = basketEndState;
							move.nextStoredBits = st.storedBits | (1ull << pickupBall);
							move.nextStoredHash = st.storedHash ^ ballHash[pickupBall];
							move.nextXRegistered = true;
							move.nextPpSwitched = true;
							int actualCostDelta =
								toBallDistance + 1 + basketDistance + ppSwitchExtraOutputCost + 1;
							move.executedCostDelta = actualCostDelta;
							move.expandedCostDelta = toBallExpanded + 1 + basketExpanded + 1;
							move.remainingLowerBoundDelta = -minStoreCosts[pickupBall];
							move.remainingLowerBoundXXDelta = -minStoreCostsXX[pickupBall];
							move.approachLowerBoundDelta =
								basketOnlyApproachSum - basketBestByBall[pickupBall] - parentApproachSum;
							move.approachLowerBoundXXDelta = basketOnlyApproachSumXX -
								basketBestByBallXX[pickupBall] - parentApproachSumXX;
							move.usePpSwitchBasketRoute = true;

							addParentMove(st, move);
						}
					}
				}
			}

			if (beamConfig.enablePpSwitch && !useFirstRegistrationRoute && st.storedCount > 0 &&
				(st.ppSwitched || st.xRegistered)) {
				int startLayer = st.ppSwitched ? AFTER_PP_LAYER : BEFORE_PP_LAYER;
				rep0(ball, p.m) {
					ull bit = 1ull << ball;
					if (st.storedBits & bit) {
						continue;
					}
					const PpSwitchRoute& ppBallRoute = ppSwitchBallRoutes[ball];
					int pickupStartLayerState = currentState * NUM_PP_LAYERS + startLayer;
					int toBallDistance = ppBallRoute.distance[pickupStartLayerState];
					int pickupState = ppBallRoute.endState[pickupStartLayerState];
					if (toBallDistance == INF_DIST || pickupState == -1) {
						continue;
					}
					int toBallExpanded = ppBallRoute.expanded[pickupStartLayerState];
					const PpSwitchRoute& ppBasketRoute = ppSwitchBasketRoutes[ball];
					int basketStartLayerState = pickupState * NUM_PP_LAYERS + AFTER_PP_LAYER;
					int basketDistance = ppBasketRoute.distance[basketStartLayerState];
					int basketEndState = ppBasketRoute.endState[basketStartLayerState];
					if (basketDistance == INF_DIST || basketEndState == -1) {
						continue;
					}
					int basketExpanded = ppBasketRoute.expanded[basketStartLayerState];

					BeamMove move;
					move.pickupBall = ball;
					move.storedBall = ball;
					move.pickupStartState = currentState;
					move.basketStartState = pickupState;
					move.ppPickupStartLayerState = pickupStartLayerState;
					move.ppBasketStartLayerState = basketStartLayerState;
					move.pickupEndDir = pickupState & 3;
					move.basketEndDir = basketEndState & 3;
					move.nextState = basketEndState;
					move.nextStoredBits = st.storedBits | (1ull << ball);
					move.nextStoredHash = st.storedHash ^ ballHash[ball];
					move.nextXRegistered = true;
					move.nextPpSwitched = true;
					int switchExtraCost = st.ppSwitched ? 0 : ppSwitchExtraOutputCost;
					int actualCostDelta = toBallDistance + switchExtraCost + 1 + basketDistance + 1;
					move.executedCostDelta = actualCostDelta;
					move.expandedCostDelta = toBallExpanded + 1 + basketExpanded + 1;
					move.remainingLowerBoundDelta = -minStoreCosts[ball];
					move.remainingLowerBoundXXDelta = -minStoreCostsXX[ball];
					move.approachLowerBoundDelta =
						basketOnlyApproachSum - basketBestByBall[ball] - parentApproachSum;
					move.approachLowerBoundXXDelta =
						basketOnlyApproachSumXX - basketBestByBallXX[ball] - parentApproachSumXX;
					move.usePpSwitchPickupRoute = true;
					move.usePpSwitchBasketRoute = true;

					addParentMove(st, move);
				}
			}

			debugReachableBalls[st.storedCount] += len(reachableBalls);
			int parentLimit = len(parentMoves);
			debugParentMoves[st.storedCount] += parentLimit;
			if (parentLimit <= 0) {
				return;
			}
			rep0(i, parentLimit) {
				addBeamMove(st, parentMoves[i]);
			}
		};

		auto compareCandidate = [](const auto& a, const auto& b) {
			if (a.data.score != b.data.score) {
				return a.data.score < b.data.score;
			}
			auto ha = a.hash;
			auto hb = b.hash;
			return ha < hb;
		};
		shr::beam_search::beam_width_manager beamWidthManager(beamConfig.beamWidth);
		beamWidthManager.window_size = max(1, beamConfig.beamWidthManagerWindow);
		auto filterCandidates = [&](int turn) {
			auto& candidates = beam.candidates_to_filter();
			int preFilterCandidateCount = len(candidates);
			if (0 <= turn && turn < p.m) {
				debugPreFilterCandidates[turn] = preFilterCandidateCount;
			}
			beam.remove_duplicates([](auto& a, auto& b) {
				return a.data.score < b.data.score;
			});
			int currentBeamWidth = beamConfig.beamWidth;
			if (beamConfig.useDynamicBeamWidth) {
				if (timer() >= beamConfig.dynamicTimeLimit) {
					currentBeamWidth = 1;
				} else {
					double progress = min(1.0, max(0.0, (turn + 1) / (double) p.m));
					currentBeamWidth = beamWidthManager.next(progress, timer(), beamConfig.dynamicTimeLimit);
				}
				currentBeamWidth = max(1, currentBeamWidth);
			}
			if (beamConfig.traceFilterBeforeCount) {
				trace("final order beam turn=", turn, ", cands=", preFilterCandidateCount,
					", width=", currentBeamWidth, ", time=", timer());
			}
			if (beamConfig.enablePpSwitch) {
				using CandidateType = remove_reference_t<decltype(candidates)>::value_type;
				static vector<CandidateType> ppCandidates;
				static vector<CandidateType> nonPpCandidates;
				ppCandidates.clear();
				nonPpCandidates.clear();
				ppCandidates.reserve(len(candidates));
				nonPpCandidates.reserve(len(candidates));
				for (const auto& candidate : candidates) {
					if (beamMoves[candidate.move_id].nextPpSwitched) {
						ppCandidates.push_back(candidate);
					} else {
						nonPpCandidates.push_back(candidate);
					}
				}

				int groupLimit = max(1, currentBeamWidth * 4 / 5);
				auto shrinkGroup = [&](auto& group) {
					if (len(group) <= groupLimit) {
						return;
					}
					nth_element(group.begin(), group.begin() + groupLimit, group.end(), compareCandidate);
					group.resize(groupLimit);
				};
				shrinkGroup(ppCandidates);
				shrinkGroup(nonPpCandidates);

				candidates.clear();
				candidates.reserve(len(ppCandidates) + len(nonPpCandidates));
				candidates.insert(candidates.end(), ppCandidates.begin(), ppCandidates.end());
				candidates.insert(candidates.end(), nonPpCandidates.begin(), nonPpCandidates.end());
			}
			if (len(candidates) > currentBeamWidth) {
				nth_element(candidates.begin(), candidates.begin() + currentBeamWidth, candidates.end(),
					compareCandidate);
				candidates.resize(currentBeamWidth);
			}
			if (beamConfig.useDynamicBeamWidth) {
				beamWidthManager.report(len(candidates));
			}
			if (0 <= turn && turn < p.m) {
				debugPostFilterCandidates[turn] = len(candidates);
			}
			// trace(show(turn, len(candidates)));
		};

		BeamState initialState;
		int initialStateValue = registration.initialState;
		initialState.currentCell = initialStateValue >> 2;
		initialState.currentDir = initialStateValue & 3;
		initialState.xRegistered = registration.prefix;
		if (registration.prefix) {
			initialState.expandedCost = registration.expandedLength;
			initialState.outputTail.append(registration.operations);
			initialState.outputLengthSoFar = registration.outputLength;
		}
		rep0(ball, p.m) {
			initialState.remainingLowerBound += minStoreCosts[ball];
			if (beamConfig.enablePpSwitch) {
				initialState.remainingLowerBoundXX += minStoreCostsXX[ball];
			}
		}
		int initialBasketOnlyApproachSum = 0;
		initialState.approachLowerBound = computeApproachLowerBounds(initialState, cellToBallMin,
			basketToBallMin, basketOrderByBall, basketBestByBall, initialBasketOnlyApproachSum);
		if (beamConfig.enablePpSwitch) {
			int initialBasketOnlyApproachSumXX = 0;
			initialState.approachLowerBoundXX = computeApproachLowerBounds(initialState, cellToBallMinXX,
				basketToBallMinXX, basketOrderByBallXX, basketBestByBallXX, initialBasketOnlyApproachSumXX);
		}

		vector<int> moveIds;
		if (beamConfig.collectLastCandidates) {
			moveIds = beam.run_tree(
				initialState, getScore, applyMove, undoMove, enumerateCandidates, filterCandidates);
		} else {
			moveIds = beam.run(initialState, getScore, applyMove, enumerateCandidates, filterCandidates);
		}
		if (len(moveIds) != p.m) {
			int failedTurn = -1;
			rep0(turn, p.m) {
				if (debugPreFilterCandidates[turn] == 0 || debugPostFilterCandidates[turn] == 0) {
					failedTurn = turn;
					break;
				}
			}
			if (failedTurn == -1) {
				rep0(turn, p.m) {
					if (debugPreFilterCandidates[turn] == -1) {
						failedTurn = turn;
						break;
					}
				}
			}
			if (failedTurn == -1) {
				failedTurn = min(len(moveIds), p.m - 1);
			}
			auto statAt = [&](const vector<int>& values) {
				return 0 <= failedTurn && failedTurn < len(values) ? values[failedTurn] : -1;
			};
			lastBeamFailureReason = "moves=" + tos(len(moveIds)) + "/" + tos(p.m) +
				", failedTurn=" + tos(failedTurn) + ", pre=" + tos(statAt(debugPreFilterCandidates)) +
				", post=" + tos(statAt(debugPostFilterCandidates)) +
				", parents=" + tos(statAt(debugEnumeratedParentStates)) +
				", reachable=" + tos(statAt(debugReachableBalls)) +
				", parentMoves=" + tos(statAt(debugParentMoves)) +
				", expandedReject=" + tos(statAt(debugExpandedRejectedMoves));
			return nullopt;
		}

		auto restoreMoveOperations = [&](const vector<int>& currentMoveIds) {
			string operations;
			for (int moveId : currentMoveIds) {
				const BeamMove& move = beamMoves[moveId];
				if (move.useFirstRegistrationRoute) {
					assert(firstRegistrationRoute);
					operations +=
						restoreFirstRegistrationPrefixRoute(*firstRegistrationRoute, move.firstPickupLayerState);
				} else if (move.usePpSwitchPickupRoute) {
					operations += restorePpSwitchRoute(ppSwitchBallRoutes[move.pickupBall],
						move.ppPickupStartLayerState, xNextStates.data(), ppSwitchMacro->nextStates.data(),
						xLength, ppSwitchExpandedLength);
				} else {
					operations += restoreRouteOperations(
						ballRoutes[move.pickupBall], move.pickupStartState, xNextStates, move.pickupEndDir);
				}
				operations.push_back('S');
				if (move.useFirstRegistrationRoute) {
					operations += restoreFirstRegistrationTargetRoute(
						firstBasketRoutes[move.storedBall], move.firstPickupLayerState);
				} else if (move.usePpSwitchBasketRoute) {
					operations += restorePpSwitchRoute(ppSwitchBasketRoutes[move.storedBall],
						move.ppBasketStartLayerState, xNextStates.data(), ppSwitchMacro->nextStates.data(),
						xLength, ppSwitchExpandedLength);
				} else {
					operations += restoreRouteOperations(
						basketRoutes[move.storedBall], move.basketStartState, xNextStates, move.basketEndDir);
				}
				operations.push_back('S');
			}
			return operations;
		};

		BeamOperationsResult result;
		result.operations = restoreMoveOperations(moveIds);
		if (beamConfig.collectLastCandidates && beamConfig.lastCandidateLimit > 0) {
			sort(terminalRecords.begin(), terminalRecords.end(),
				[](const TerminalRecord& a, const TerminalRecord& b) {
					if (a.score != b.score) {
						return a.score < b.score;
					}
					return a.moveIds < b.moveIds;
				});
			unordered_set<string> usedOperations;
			for (const TerminalRecord& record : terminalRecords) {
				if (len(record.moveIds) != p.m) {
					continue;
				}
				string operations = restoreMoveOperations(record.moveIds);
				if (!usedOperations.insert(operations).second) {
					continue;
				}
				result.lastOperations.push_back(std::move(operations));
				if (len(result.lastOperations) >= beamConfig.lastCandidateLimit) {
					break;
				}
			}
		}
		result.terminalOptimizedScore = beam.best_score;
		return result;
	}

	Candidate evaluateX(const string& x) {
		BeamConfig beamConfig;
		return evaluateX(x, beamConfig);
	}

	string encodeFRunMacro(const string& x, int length) const {
		string encoded;
		encoded.reserve(len(x));
		for (int i = 0; i < len(x);) {
			if (x[i] != 'F') {
				encoded.push_back(x[i++]);
				continue;
			}
			int j = i + 1;
			while (j < len(x) && x[j] == 'F') {
				j++;
			}
			int run = j - i;
			int pCount = run / length;
			int rest = run - pCount * length;
			encoded.append(pCount, 'P');
			encoded.append(rest, 'F');
			i = j;
		}
		return encoded;
	}

	MacroRegistration buildBestFastMacroRegistration(const string& x) const {
		int xLength = len(x);
		int startState = initialStateId;
		MacroRegistration best;
		best.encodedX = x;
		best.outputLength = xLength + 1;
		best.expandedLength = 0;
		best.initialState = startState;
		best.compressed = false;
		best.prefix = false;

		static constexpr int MAX_FAST_F_RUNS = 512;
		int runs[MAX_FAST_F_RUNS];
		int runCount = 0;
		int fTotal = 0;
		int maxFRun = 0;
		bool runOverflow = false;
		for (int i = 0; i < xLength;) {
			if (x[i] != 'F') {
				i++;
				continue;
			}
			int j = i + 1;
			while (j < xLength && x[j] == 'F') {
				j++;
			}
			int run = j - i;
			if (runCount < MAX_FAST_F_RUNS) {
				runs[runCount++] = run;
			} else {
				runOverflow = true;
			}
			fTotal += run;
			maxFRun = max(maxFRun, run);
			i = j;
		}

		int bestCompressedOutput = INF;
		int bestCompressedLength = INF;
		int nonFCount = xLength - fTotal;
		for (int length = 2; length <= maxFRun; length++) {
			int encodedLength = nonFCount;
			if (!runOverflow) {
				for (int ri = 0; ri < runCount; ri++) {
					int run = runs[ri];
					encodedLength += run / length + run % length;
				}
			} else {
				for (int i = 0; i < xLength;) {
					if (x[i] != 'F') {
						i++;
						continue;
					}
					int j = i + 1;
					while (j < xLength && x[j] == 'F') {
						j++;
					}
					int run = j - i;
					encodedLength += run / length + run % length;
					i = j;
				}
			}
			int outputLength = length + encodedLength + 4;
			if (outputLength < bestCompressedOutput ||
				(outputLength == bestCompressedOutput && length < bestCompressedLength)) {
				bestCompressedOutput = outputLength;
				bestCompressedLength = length;
			}
		}
		if (bestCompressedOutput < best.outputLength) {
			string y(bestCompressedLength, 'F');
			string encodedX = encodeFRunMacro(x, bestCompressedLength);
			MacroRegistration compressed;
			compressed.y = std::move(y);
			compressed.encodedX = std::move(encodedX);
			compressed.operations.reserve(bestCompressedOutput);
			compressed.operations.push_back('M');
			compressed.operations += compressed.y;
			compressed.operations += "MM";
			compressed.operations += compressed.encodedX;
			compressed.operations.push_back('M');
			compressed.outputLength = bestCompressedOutput;
			compressed.expandedLength = bestCompressedLength + xLength;
			compressed.initialState = startState;
			compressed.compressed = true;
			compressed.prefix = false;
			best = std::move(compressed);
		}
		return best;
	}

	Candidate evaluateXFastWithNextStates(const string& x, const short* macroNextStates,
		int macroExpandedLength, const MacroRegistration& registration, int bestKnownScore = INF) {
		Candidate candidate;
		candidate.x = x;
		if (x.empty()) {
			candidate.invalidReason = "empty X";
			return candidate;
		}
		assert_le(p.m, 64);

		candidate.registration = registration;

		bool ignoreFastExpandedLimit = shouldIgnoreFastExpandedLimit();

		struct FastBfsResult {
			int state = -1;
			int ball = -1;
			int distance = 0;
			int pUses = 0;
		};

		auto nextFastVisitToken = [&]() {
			if (++fastVisitToken == 0) {
				memset(fastSeen, 0, sizeof(fastSeen));
				fastVisitToken = 1;
			}
			return fastVisitToken;
		};

		auto runBfsCountingP = [&](int startState, int targetCell, ull storedBits) -> FastBfsResult {
			int token = nextFastVisitToken();
			int head = 0;
			int tail = 0;
			fastSeen[startState] = token;
			fastPUseCount[startState] = 0;
			fastQueue[tail++] = (short) startState;
			const short* __restrict xNext = macroNextStates;
			const short* __restrict nextF = basicNextF.data();
			const short* __restrict nextR = basicNextR.data();
			const short* __restrict nextL = basicNextL.data();
			const short* __restrict ballAtState = stateBall;

#define FAST_PUSH_COUNT_P_PICKUP(ns, pc, dist)                           \
	do {                                                                 \
		int nextState__ = (unsigned short) (ns);                         \
		if (fastSeen[nextState__] != token) {                            \
			int ball__ = ballAtState[nextState__];                       \
			if (ball__ != -1 && !(storedBits & (1ull << ball__))) {      \
				return FastBfsResult{nextState__, ball__, (dist), (pc)}; \
			}                                                            \
			fastSeen[nextState__] = token;                               \
			fastPUseCount[nextState__] = (short) (pc);                   \
			fastQueue[tail++] = (short) nextState__;                     \
		}                                                                \
	} while (false)

#define FAST_PUSH_COUNT_P_TARGET(ns, pc, dist)                       \
	do {                                                             \
		int nextState__ = (unsigned short) (ns);                     \
		if (fastSeen[nextState__] != token) {                        \
			if ((nextState__ & ~3) == targetBaseState) {             \
				return FastBfsResult{nextState__, -1, (dist), (pc)}; \
			}                                                        \
			fastSeen[nextState__] = token;                           \
			fastPUseCount[nextState__] = (short) (pc);               \
			fastQueue[tail++] = (short) nextState__;                 \
		}                                                            \
	} while (false)

			if (targetCell == -1) {
				int currentDistance = 0;
				while (head < tail) {
					int levelEnd = tail;
					int nextDistance = currentDistance + 1;
					while (head < levelEnd) {
						int currentState = (unsigned short) fastQueue[head++];
						int ball = ballAtState[currentState];
						int currentPUseCount = fastPUseCount[currentState];
						if (ball != -1 && !(storedBits & (1ull << ball))) {
							return FastBfsResult{currentState, ball, currentDistance, currentPUseCount};
						}
						FAST_PUSH_COUNT_P_PICKUP(xNext[currentState], currentPUseCount + 1, nextDistance);
						FAST_PUSH_COUNT_P_PICKUP(nextF[currentState], currentPUseCount, nextDistance);
						FAST_PUSH_COUNT_P_PICKUP(nextR[currentState], currentPUseCount, nextDistance);
						FAST_PUSH_COUNT_P_PICKUP(nextL[currentState], currentPUseCount, nextDistance);
					}
					currentDistance = nextDistance;
				}
			} else {
				int targetBaseState = targetCell << 2;
				int currentDistance = 0;
				while (head < tail) {
					int levelEnd = tail;
					int nextDistance = currentDistance + 1;
					while (head < levelEnd) {
						int currentState = (unsigned short) fastQueue[head++];
						int currentPUseCount = fastPUseCount[currentState];
						if ((currentState & ~3) == targetBaseState) {
							return FastBfsResult{currentState, -1, currentDistance, currentPUseCount};
						}
						FAST_PUSH_COUNT_P_TARGET(xNext[currentState], currentPUseCount + 1, nextDistance);
						FAST_PUSH_COUNT_P_TARGET(nextF[currentState], currentPUseCount, nextDistance);
						FAST_PUSH_COUNT_P_TARGET(nextR[currentState], currentPUseCount, nextDistance);
						FAST_PUSH_COUNT_P_TARGET(nextL[currentState], currentPUseCount, nextDistance);
					}
					currentDistance = nextDistance;
				}
			}
#undef FAST_PUSH_COUNT_P_TARGET
#undef FAST_PUSH_COUNT_P_PICKUP
			return FastBfsResult{};
		};

		auto runBfs = [&](int startState, int targetCell, ull storedBits) -> FastBfsResult {
			int token = nextFastVisitToken();
			int head = 0;
			int tail = 0;
			fastSeen[startState] = token;
			fastQueue[tail++] = (short) startState;
			const short* __restrict xNext = macroNextStates;
			const short* __restrict nextF = basicNextF.data();
			const short* __restrict nextR = basicNextR.data();
			const short* __restrict nextL = basicNextL.data();
			const short* __restrict ballAtState = stateBall;

#define FAST_PUSH_PICKUP(ns, dist)                                    \
	do {                                                              \
		int nextState__ = (unsigned short) (ns);                      \
		if (fastSeen[nextState__] != token) {                         \
			int ball__ = ballAtState[nextState__];                    \
			if (ball__ != -1 && !(storedBits & (1ull << ball__))) {   \
				return FastBfsResult{nextState__, ball__, (dist), 0}; \
			}                                                         \
			fastSeen[nextState__] = token;                            \
			fastQueue[tail++] = (short) nextState__;                  \
		}                                                             \
	} while (false)

#define FAST_PUSH_TARGET(ns, dist)                                \
	do {                                                          \
		int nextState__ = (unsigned short) (ns);                  \
		if (fastSeen[nextState__] != token) {                     \
			if ((nextState__ & ~3) == targetBaseState) {          \
				return FastBfsResult{nextState__, -1, (dist), 0}; \
			}                                                     \
			fastSeen[nextState__] = token;                        \
			fastQueue[tail++] = (short) nextState__;              \
		}                                                         \
	} while (false)

			if (targetCell == -1) {
				int currentDistance = 0;
				while (head < tail) {
					int levelEnd = tail;
					int nextDistance = currentDistance + 1;
					while (head < levelEnd) {
						int currentState = (unsigned short) fastQueue[head++];
						int ball = ballAtState[currentState];
						if (ball != -1 && !(storedBits & (1ull << ball))) {
							return FastBfsResult{currentState, ball, currentDistance, 0};
						}
						FAST_PUSH_PICKUP(xNext[currentState], nextDistance);
						FAST_PUSH_PICKUP(nextF[currentState], nextDistance);
						FAST_PUSH_PICKUP(nextR[currentState], nextDistance);
						FAST_PUSH_PICKUP(nextL[currentState], nextDistance);
					}
					currentDistance = nextDistance;
				}
			} else {
				int targetBaseState = targetCell << 2;
				int currentDistance = 0;
				while (head < tail) {
					int levelEnd = tail;
					int nextDistance = currentDistance + 1;
					while (head < levelEnd) {
						int currentState = (unsigned short) fastQueue[head++];
						if ((currentState & ~3) == targetBaseState) {
							return FastBfsResult{currentState, -1, currentDistance, 0};
						}
						FAST_PUSH_TARGET(xNext[currentState], nextDistance);
						FAST_PUSH_TARGET(nextF[currentState], nextDistance);
						FAST_PUSH_TARGET(nextR[currentState], nextDistance);
						FAST_PUSH_TARGET(nextL[currentState], nextDistance);
					}
					currentDistance = nextDistance;
				}
			}
#undef FAST_PUSH_TARGET
#undef FAST_PUSH_PICKUP
			return FastBfsResult{};
		};

		int abstractLength = 0;
		int fastXUseCount = 0;
		int currentState = initialStateId;
		ull storedBits = 0;
		int fastExpandedLimit = min(p.numT * 12 / 10, p.numT + 500);
		int scoreCutoff = bestKnownScore + 50;

		auto updateFastCandidateLengths = [&]() {
			candidate.abstractLength = abstractLength;
			candidate.expandedLength = ignoreFastExpandedLimit
				? abstractLength
				: abstractLength + fastXUseCount * (macroExpandedLength - 1);
			candidate.xUseCount = fastXUseCount;
			int fastOutputLength = candidate.registration.outputLength + abstractLength;
			candidate.outputLength = fastOutputLength;
			candidate.score = fastOutputLength + candidate.registration.outputLength;
		};
		auto isExpandedOverLimit = [&](int nextAbstractLength, int nextXUseCount) {
			return nextAbstractLength + nextXUseCount * (macroExpandedLength - 1) > fastExpandedLimit;
		};
		rep0(_, p.m) {
			FastBfsResult pickupResult = ignoreFastExpandedLimit
				? runBfs(currentState, -1, storedBits)
				: runBfsCountingP(currentState, -1, storedBits);
			int pickupState = pickupResult.state;
			int ball = pickupResult.ball;
			if (pickupState == -1) {
				if (candidate.invalidReason.empty()) {
					candidate.invalidReason = "fast ball bfs failed";
				}
				return candidate;
			}
			abstractLength += pickupResult.distance + 1;
			if (!ignoreFastExpandedLimit) {
				fastXUseCount += pickupResult.pUses;
				if (isExpandedOverLimit(abstractLength, fastXUseCount)) {
					updateFastCandidateLengths();
					candidate.invalidReason = "expanded over T";
					return candidate;
				}
			}
			currentState = pickupState;

			int targetBasketCell = basketCell[ball];
			FastBfsResult basketResult = ignoreFastExpandedLimit
				? runBfs(currentState, targetBasketCell, storedBits)
				: runBfsCountingP(currentState, targetBasketCell, storedBits);
			int basketState = basketResult.state;
			if (basketState == -1) {
				if (candidate.invalidReason.empty()) {
					candidate.invalidReason = "fast basket bfs failed";
				}
				return candidate;
			}
			abstractLength += basketResult.distance + 1;
			if (!ignoreFastExpandedLimit) {
				fastXUseCount += basketResult.pUses;
				if (isExpandedOverLimit(abstractLength, fastXUseCount)) {
					updateFastCandidateLengths();
					candidate.invalidReason = "expanded over T";
					return candidate;
				}
			}
			currentState = basketState;
			storedBits |= 1ull << ball;
			updateFastCandidateLengths();
			if (candidate.score >= scoreCutoff) {
				candidate.invalidReason = "fast score cutoff";
				return candidate;
			}
		}

		updateFastCandidateLengths();
		if (!ignoreFastExpandedLimit && candidate.expandedLength > fastExpandedLimit) {
			candidate.invalidReason = "expanded over T";
		} else if (candidate.outputLength > p.numT) {
			candidate.invalidReason = "output over T";
		} else {
			candidate.valid = true;
		}
		return candidate;
	}

	Candidate evaluateXFast(const string& x, int bestKnownScore = INF) {
		if (x.empty()) {
			Candidate candidate;
			candidate.x = x;
			candidate.invalidReason = "empty X";
			return candidate;
		}
		MacroRegistration registration = buildBestFastMacroRegistration(x);
		if (bestKnownScore < INF / 2) {
			int minimumAbstractLength = 2 * p.m;
			int minimumScore = 2 * registration.outputLength + minimumAbstractLength;
			if (minimumScore >= bestKnownScore + 50) {
				Candidate candidate;
				candidate.x = x;
				candidate.registration = registration;
				candidate.abstractLength = minimumAbstractLength;
				candidate.expandedLength = minimumAbstractLength;
				candidate.outputLength = registration.outputLength + minimumAbstractLength;
				candidate.score = minimumScore;
				candidate.invalidReason = "fast score cutoff";
				return candidate;
			}
		}
		getXNextStatesRaw(x, fastXNextStates);
		return evaluateXFastWithNextStates(x, fastXNextStates, len(x), registration, bestKnownScore);
	}

	int ppSwitchPatternExpandedLength(const string& pattern, int xLength) const {
		int expandedLength = 0;
		for (char operation : pattern) {
			expandedLength += operation == 'P' ? xLength : 1;
		}
		return expandedLength;
	}

	void buildPpSwitchNextStatesRaw(
		const string& pattern, const vector<short>& xNextStates, short* output) const {
		rep0(state, numStates) {
			output[state] = (short) state;
		}
		for (char operation : pattern) {
			if (operation == 'P') {
				rep0(state, numStates) {
					output[state] = xNextStates[(unsigned short) output[state]];
				}
			} else {
				const short* table = operation == 'L' ? basicNextL.data() : basicNextR.data();
				rep0(state, numStates) {
					output[state] = table[(unsigned short) output[state]];
				}
			}
		}
	}

	PpSwitchMacro buildPpSwitchMacro(
		const string& pattern, const vector<short>& xNextStates, int xLength) const {
		PpSwitchMacro macro;
		macro.pattern = pattern;
		macro.nextStates.resize(numStates);
		buildPpSwitchNextStatesRaw(pattern, xNextStates, macro.nextStates.data());
		macro.expandedLength = ppSwitchPatternExpandedLength(pattern, xLength);
		macro.firstZExtraOutputCost = len(pattern) + 1;
		return macro;
	}

	PpSwitchMacro selectPpSwitchMacro(const string& x, const vector<short>& xNextStates, int xLength) {
		auto cachedPattern = ppSwitchPatternCache.find(x);
		if (cachedPattern != ppSwitchPatternCache.end()) {
			return buildPpSwitchMacro(cachedPattern->second, xNextStates, xLength);
		}

		MacroRegistration noRegistration;
		noRegistration.outputLength = 0;
		noRegistration.expandedLength = 0;
		noRegistration.initialState = initialStateId;
		noRegistration.compressed = false;
		noRegistration.prefix = false;

		string bestPattern = "PP";
		ll bestScore = (ll) 4e18;
		int bestExpandedLength = INF;
		for (const string& pattern : ppSwitchCandidatePatterns) {
			int expandedLength = ppSwitchPatternExpandedLength(pattern, xLength);
			buildPpSwitchNextStatesRaw(pattern, xNextStates, fastXNextStates);
			Candidate candidate =
				evaluateXFastWithNextStates(x, fastXNextStates, expandedLength, noRegistration, INF);
			if (!candidate.valid) {
				continue;
			}
			if (candidate.score < bestScore ||
				(candidate.score == bestScore && candidate.expandedLength < bestExpandedLength) ||
				(candidate.score == bestScore && candidate.expandedLength == bestExpandedLength &&
					pattern < bestPattern)) {
				bestScore = candidate.score;
				bestExpandedLength = candidate.expandedLength;
				bestPattern = pattern;
			}
		}
		ppSwitchPatternCache.emplace(x, bestPattern);
		return buildPpSwitchMacro(bestPattern, xNextStates, xLength);
	}

	Candidate evaluateX(const string& x, const BeamConfig& beamConfig) {
		lastBeamTerminalCandidates.clear();
		Candidate candidate;
		candidate.x = x;
		if (x.empty()) {
			candidate.invalidReason = "empty X";
			return candidate;
		}

		vector<short> xNextStates;
		getXNextStates(x, xNextStates);
		vector<MacroRegistration> registrations;
		if (beamConfig.useFixedRegistration) {
			registrations.push_back(beamConfig.fixedRegistration);
		} else {
			registrations = buildMacroRegistrations(x, xNextStates);
		}
		optional<PpSwitchMacro> ppSwitchMacro;
		if (beamConfig.enablePpSwitch) {
			ppSwitchMacro = selectPpSwitchMacro(x, xNextStates, len(x));
		}

		auto evaluateRegistration = [&](const MacroRegistration& registration) {
			Candidate trial;
			trial.x = x;
			trial.registration = registration;
			if (beamConfig.enablePpSwitch) {
				assert(ppSwitchMacro);
				trial.ppSwitchPattern = ppSwitchMacro->pattern;
				trial.ppSwitchExpandedLength = ppSwitchMacro->expandedLength;
			}

			optional<BeamOperationsResult> maybeBeamResult = buildBeamOperations(
				xNextStates, registration, beamConfig, ppSwitchMacro ? &*ppSwitchMacro : nullptr);
			if (!maybeBeamResult) {
				trial.invalidReason = "beam failed";
				if (!lastBeamFailureReason.empty()) {
					trial.invalidReason += ": ";
					trial.invalidReason += lastBeamFailureReason;
				}
				return trial;
			}

			auto buildTrialFromAbstractOperations = [&](const string& abstractOperations, bool checkScore) {
				Candidate converted;
				converted.x = x;
				converted.registration = registration;
				if (beamConfig.enablePpSwitch) {
					assert(ppSwitchMacro);
					converted.ppSwitchPattern = ppSwitchMacro->pattern;
					converted.ppSwitchExpandedLength = ppSwitchMacro->expandedLength;
					PpSwitchOperationConversion conversion =
						buildOutputOperationsWithPpSwitch(abstractOperations, x, registration, *ppSwitchMacro);
					converted.abstractLength = conversion.outputLength;
					converted.expandedLength = conversion.expandedLength;
					converted.xUseCount = xUseCount(abstractOperations);
					converted.score = conversion.outputLength;
					converted.outputLength = conversion.outputLength;
					if (!conversion.valid) {
						converted.invalidReason = conversion.invalidReason;
					} else if (converted.expandedLength > p.numT) {
						converted.invalidReason = "expanded over T";
					} else if (converted.outputLength > p.numT) {
						converted.invalidReason = "output over T";
					} else {
						converted.operations = std::move(conversion.operations);
						converted.valid = true;
						optimizeTailMacro(converted);
						if (checkScore) {
							assert_eq(converted.score, maybeBeamResult->terminalOptimizedScore);
						}
					}
					return converted;
				}

				converted.abstractLength = registration.compressed
					? outputLengthWithRegistration(abstractOperations, registration)
					: len(abstractOperations);
				converted.expandedLength = registration.compressed
					? expandedLengthWithRegistration(abstractOperations, x, registration)
					: registration.expandedLength + expandedLength(abstractOperations, x);
				converted.xUseCount = xUseCount(abstractOperations);
				converted.score = registration.compressed ? converted.abstractLength
														   : registration.outputLength + len(abstractOperations);
				converted.outputLength = (int) converted.score;
				if (converted.expandedLength > p.numT) {
					converted.invalidReason = "expanded over T";
				} else if (converted.outputLength > p.numT) {
					converted.invalidReason = "output over T";
				} else {
					if (registration.prefix) {
						converted.operations = registration.operations + abstractOperations;
					} else if (registration.compressed) {
						converted.operations =
							buildOutputOperationsWithRegistration(abstractOperations, registration);
					} else {
						converted.operations = buildOutputOperations(abstractOperations, x);
					}
					converted.valid = true;
					optimizeTailMacro(converted);
					if (checkScore) {
						assert_eq(converted.score, maybeBeamResult->terminalOptimizedScore);
					}
				}
				return converted;
			};

			trial = buildTrialFromAbstractOperations(maybeBeamResult->operations, true);
			for (const string& lastAbstractOperations : maybeBeamResult->lastOperations) {
				Candidate lastCandidate = buildTrialFromAbstractOperations(lastAbstractOperations, false);
				if (lastCandidate.valid) {
					lastBeamTerminalCandidates.push_back(std::move(lastCandidate));
				}
			}
			return trial;
		};

		bool hasBestCandidate = false;
		for (const MacroRegistration& registration : registrations) {
			Candidate trial = evaluateRegistration(registration);
			bool take = !hasBestCandidate;
			if (hasBestCandidate) {
				if (trial.valid != candidate.valid) {
					take = trial.valid;
				} else if (trial.score != candidate.score) {
					take = trial.score < candidate.score;
				} else if (trial.expandedLength != candidate.expandedLength) {
					take = trial.expandedLength < candidate.expandedLength;
				} else {
					take = trial.registration.operations < candidate.registration.operations;
				}
			}
			if (take) {
				candidate = std::move(trial);
				hasBestCandidate = true;
			}
		}
		return candidate;
	}

	Candidate runFinalSingleStepBeam(const Candidate& base) {
		constexpr int FINAL_BEAM_WIDTH = 500000;

		Candidate candidate;
		candidate.x = base.x;
		candidate.registration = base.registration;
		candidate.invalidReason = "final beam failed";
		if (!base.valid || base.x.empty()) {
			return candidate;
		}

		const string& x = base.x;
		const MacroRegistration& registration = base.registration;
		int numCells = p.n * p.n;
		int numStates = numCells * 4;
		int heldCell = numCells;

		vector<short> xNextStates;
		getXNextStates(x, xNextStates);
		ReverseGraph reverseGraph = buildReverseGraph(xNextStates);
		vector<short> routePacked(p.m * numStates);
		vector<char> routeDirMasks(p.m * numStates);
		vector<short> routeQueue(numStates * 8);
		vector<RouteTable> basketRoutes(p.m);
		vector<int> basketCells(p.m);
		auto basketRoute = [&](int ball) {
			return RouteTable{routePacked.data() + ball * numStates, routeDirMasks.data() + ball * numStates};
		};
		rep0(ball, p.m) {
			basketRoutes[ball] = basketRoute(ball);
			basketCells[ball] = id(p.basketPos[ball]);
			buildRouteTable(p.basketPos[ball], reverseGraph, basketRoutes[ball], routeQueue);
		}

		vector<int> placedCost(p.m * numCells, INF);
		vector<int> heldCost(p.m * numStates, INF);
		auto placedCostAt = [&](int ball, int cell) -> int& {
			return placedCost[ball * numCells + cell];
		};
		auto heldCostAt = [&](int ball, int state) -> int& {
			return heldCost[ball * numStates + state];
		};
		rep0(ball, p.m) {
			const RouteTable& route = basketRoutes[ball];
			rep0(cell, numCells) {
				if (cell == basketCells[ball]) {
					placedCostAt(ball, cell) = 0;
					continue;
				}
				int maxDistance = 0;
				bool reachable = true;
				int baseState = cell << 2;
				rep0(dir, 4) {
					int distance = route.distance(baseState | dir);
					if (distance == INF_DIST) {
						reachable = false;
						break;
					}
					maxDistance = max(maxDistance, distance);
				}
				placedCostAt(ball, cell) = reachable ? maxDistance + 1 : INF;
			}
			rep0(state, numStates) {
				int distance = route.distance(state);
				heldCostAt(ball, state) = distance == INF_DIST ? INF : distance;
			}
		}

		string registrationOperations =
			registration.compressed ? registration.operations : string("M") + x + "M";
		int registrationOutputLength = len(registrationOperations);
		int registrationExpandedLength = registration.compressed ? registration.expandedLength : len(x);
		vector<short> registrationNextStates(numStates);
		if (registration.compressed) {
			vector<short> yNextStates;
			getXNextStates(registration.y, yNextStates);
			rep0(state, numStates) {
				registrationNextStates[state] = xNextStates[yNextStates[state]];
			}
		} else {
			registrationNextStates = xNextStates;
		}

		vector<ull> ballCellHash(p.m * (numCells + 1));
		for (ull& h : ballCellHash) {
			h = rng.next_ull();
		}
		ull registeredHash = rng.next_ull();
		auto ballCellHashAt = [&](int ball, int cell) {
			return ballCellHash[ball * (numCells + 1) + cell];
		};

		FinalBeamState initialState;
		initialState.state = stateId({0, 0}, 1);
		initialState.ballCell.assign(p.m, 0);
		initialState.cellBall.assign(numCells, (short) -1);
		rep0(ball, p.m) {
			int cell = id(p.ballPos[ball]);
			assert_eq(initialState.cellBall[cell], (short) -1);
			initialState.ballCell[ball] = (short) cell;
			initialState.cellBall[cell] = (short) ball;
			initialState.placementHash ^= ballCellHashAt(ball, cell);
			if (cell == basketCells[ball]) {
				initialState.correctCount++;
			}
			initialState.evalSum += placedCostAt(ball, cell);
		}
		initialState.undoStack.reserve(p.numT + 8);

		array<int, FINAL_MOVE_COUNT> outputDeltas = {
			1,
			1,
			1,
			1,
			1,
			registrationOutputLength,
		};
		array<int, FINAL_MOVE_COUNT> expandedDeltas = {
			1,
			1,
			1,
			len(x),
			1,
			registrationExpandedLength,
		};

		auto isGoalState = [&](const FinalBeamState& st) {
			return st.correctCount == p.m && st.heldBall == -1;
		};

		auto pushUndo = [&](FinalBeamState& st) -> FinalBeamUndo& {
			st.undoStack.push_back(FinalBeamUndo());
			FinalBeamUndo& undo = st.undoStack.back();
			undo.state = st.state;
			undo.registered = st.registered;
			undo.heldBall = st.heldBall;
			undo.correctCount = st.correctCount;
			undo.evalSum = st.evalSum;
			undo.outputLength = st.outputLength;
			undo.expandedLength = st.expandedLength;
			undo.xUseCount = st.xUseCount;
			undo.placementHash = st.placementHash;
			return undo;
		};

		auto recordBallChange = [&](FinalBeamState& st, FinalBeamUndo& undo, int ball) {
			assert_lt(undo.changedBallCount, 2);
			undo.changedBalls[undo.changedBallCount] = ball;
			undo.oldBallCells[undo.changedBallCount] = st.ballCell[ball];
			undo.changedBallCount++;
		};

		auto setBallCell = [&](FinalBeamState& st, FinalBeamUndo& undo, int ball, int newCell) {
			int oldCell = st.ballCell[ball];
			recordBallChange(st, undo, ball);
			if (oldCell != heldCell && oldCell == basketCells[ball]) {
				st.correctCount--;
			}
			if (newCell != heldCell && newCell == basketCells[ball]) {
				st.correctCount++;
			}
			st.placementHash ^= ballCellHashAt(ball, oldCell);
			st.placementHash ^= ballCellHashAt(ball, newCell);
			st.ballCell[ball] = (short) newCell;
		};

		auto setCellBall = [&](FinalBeamState& st, FinalBeamUndo& undo, int cell, int ball) {
			assert_eq(undo.changedCell, -1);
			undo.changedCell = cell;
			undo.oldCellBall = st.cellBall[cell];
			st.cellBall[cell] = (short) ball;
		};

		auto transitionState = [&](FinalBeamState& st, int nextState) {
			int heldBall = st.heldBall;
			if (heldBall != -1) {
				st.evalSum -= heldCostAt(heldBall, st.state);
				st.evalSum += heldCostAt(heldBall, nextState);
			}
			st.state = nextState;
		};

		auto applyMove = [&](FinalBeamState& st, int moveId) {
			if (st.undoStack.capacity() == 0) {
				st.undoStack.reserve(p.numT + 8);
			}
			FinalBeamUndo& undo = pushUndo(st);
			st.outputLength += outputDeltas[moveId];
			st.expandedLength += expandedDeltas[moveId];

			if (moveId == FINAL_MOVE_L) {
				transitionState(st, basicNextL[st.state]);
			} else if (moveId == FINAL_MOVE_R) {
				transitionState(st, basicNextR[st.state]);
			} else if (moveId == FINAL_MOVE_F) {
				transitionState(st, basicNextF[st.state]);
			} else if (moveId == FINAL_MOVE_P) {
				assert(st.registered);
				transitionState(st, xNextStates[st.state]);
				st.xUseCount++;
			} else if (moveId == FINAL_MOVE_REGISTER) {
				assert(!st.registered);
				transitionState(st, registrationNextStates[st.state]);
				st.registered = true;
			} else {
				assert_eq(moveId, FINAL_MOVE_S);
				int cell = st.state >> 2;
				int cellBall = st.cellBall[cell];
				if (st.heldBall == -1) {
					assert_ne(cellBall, -1);
					st.evalSum -= placedCostAt(cellBall, cell);
					st.evalSum += heldCostAt(cellBall, st.state);
					setBallCell(st, undo, cellBall, heldCell);
					setCellBall(st, undo, cell, -1);
					st.heldBall = cellBall;
				} else if (cellBall == -1) {
					int heldBall = st.heldBall;
					st.evalSum -= heldCostAt(heldBall, st.state);
					st.evalSum += placedCostAt(heldBall, cell);
					setBallCell(st, undo, heldBall, cell);
					setCellBall(st, undo, cell, heldBall);
					st.heldBall = -1;
				} else {
					int heldBall = st.heldBall;
					st.evalSum -= heldCostAt(heldBall, st.state);
					st.evalSum -= placedCostAt(cellBall, cell);
					st.evalSum += placedCostAt(heldBall, cell);
					st.evalSum += heldCostAt(cellBall, st.state);
					setBallCell(st, undo, heldBall, cell);
					setBallCell(st, undo, cellBall, heldCell);
					setCellBall(st, undo, cell, heldBall);
					st.heldBall = cellBall;
				}
			}
		};

		auto undoMove = [&](FinalBeamState& st) {
			assert(!st.undoStack.empty());
			FinalBeamUndo undo = st.undoStack.back();
			st.undoStack.pop_back();
			rep0(i, undo.changedBallCount) {
				st.ballCell[undo.changedBalls[i]] = undo.oldBallCells[i];
			}
			if (undo.changedCell != -1) {
				st.cellBall[undo.changedCell] = undo.oldCellBall;
			}
			st.state = undo.state;
			st.registered = undo.registered;
			st.heldBall = undo.heldBall;
			st.correctCount = undo.correctCount;
			st.evalSum = undo.evalSum;
			st.outputLength = undo.outputLength;
			st.expandedLength = undo.expandedLength;
			st.xUseCount = undo.xUseCount;
			st.placementHash = undo.placementHash;
		};

		using FinalBeam =
			shr::beam_search::beam_search<FinalBeamState, ll, int, -1, FinalBeamCandidateData, less<ll>, 24>;
		static FinalBeam beam;
		beam.max_turn = p.numT;
		bool foundGoal = false;

		auto getScore = [&](FinalBeamState& st) -> ll {
			return isGoalState(st) ? -1 : st.evalSum;
		};

		auto addCandidateAfterMove = [&](FinalBeamState& st, int moveId) {
			if (st.outputLength + outputDeltas[moveId] > p.numT) {
				return;
			}
			if (st.expandedLength + expandedDeltas[moveId] > p.numT) {
				return;
			}
			applyMove(st, moveId);
			ll score = isGoalState(st) ? -1 : st.evalSum;
			FinalBeamCandidateData data{score, st.outputLength, st.state};
			ull hash = st.placementHash ^ stateHash[st.state] ^ (st.registered ? registeredHash : 0ull);
			beam.add_candidate(moveId, data, hash);
			undoMove(st);
		};

		auto enumerateCandidates = [&](FinalBeamState& st) {
			if (isGoalState(st)) {
				foundGoal = true;
				return;
			}
			addCandidateAfterMove(st, FINAL_MOVE_L);
			addCandidateAfterMove(st, FINAL_MOVE_R);
			if (basicNextF[st.state] != st.state) {
				addCandidateAfterMove(st, FINAL_MOVE_F);
			}
			if (st.registered) {
				addCandidateAfterMove(st, FINAL_MOVE_P);
			}
			int cell = st.state >> 2;
			if (st.heldBall != -1 || st.cellBall[cell] != -1) {
				addCandidateAfterMove(st, FINAL_MOVE_S);
			}
			if (!st.registered) {
				addCandidateAfterMove(st, FINAL_MOVE_REGISTER);
			}
		};

		auto compareFinalCandidate = [](const auto& a, const auto& b) {
			if (a.data.score != b.data.score) {
				return a.data.score < b.data.score;
			}
			if (a.data.outputLength != b.data.outputLength) {
				return a.data.outputLength < b.data.outputLength;
			}
			if (a.hash != b.hash) {
				ull ha = a.hash;
				return ha < b.hash;
			}
			return a.move_id < b.move_id;
		};

		vector<int> stateCounts(numStates, 0);
		auto filterCandidates = [&](int turn) {
			auto& candidates = beam.candidates_to_filter();
			beam.remove_duplicates([](auto& a, auto& b) {
				if (a.data.score != b.data.score) {
					return a.data.score < b.data.score;
				}
				return a.data.outputLength < b.data.outputLength;
			});
			if (foundGoal) {
				candidates.clear();
				return;
			}

			if (turn < 10 || turn % 10 == 0)
				trace(show(turn, len(candidates)));

			sort(candidates.begin(), candidates.end(), compareFinalCandidate);
			int perStateLimit = max(1, FINAL_BEAM_WIDTH / numStates);
			int write = 0;
			for (auto& cand : candidates) {
				int state = cand.data.state;
				if (stateCounts[state] >= perStateLimit) {
					continue;
				}
				stateCounts[state]++;
				candidates[write++] = cand;
				if (write == FINAL_BEAM_WIDTH) {
					break;
				}
			}
			candidates.resize(write);
			fill(stateCounts.begin(), stateCounts.end(), 0);
		};

		vector<int> moveIds =
			beam.run_tree(initialState, getScore, applyMove, undoMove, enumerateCandidates, filterCandidates);

		auto appendMoveOperations = [&](string& operations, int moveId) {
			if (moveId == FINAL_MOVE_L) {
				operations.push_back('L');
			} else if (moveId == FINAL_MOVE_R) {
				operations.push_back('R');
			} else if (moveId == FINAL_MOVE_F) {
				operations.push_back('F');
			} else if (moveId == FINAL_MOVE_P) {
				operations.push_back('P');
			} else if (moveId == FINAL_MOVE_S) {
				operations.push_back('S');
			} else {
				assert_eq(moveId, FINAL_MOVE_REGISTER);
				operations += registrationOperations;
			}
		};

		FinalBeamState finalState = initialState;
		finalState.undoStack.reserve(len(moveIds) + 8);
		string operations;
		operations.reserve(min(p.numT, base.outputLength));
		for (int moveId : moveIds) {
			appendMoveOperations(operations, moveId);
			applyMove(finalState, moveId);
		}

		candidate.operations = std::move(operations);
		candidate.abstractLength = finalState.outputLength;
		candidate.outputLength = finalState.outputLength;
		candidate.expandedLength = finalState.expandedLength;
		candidate.xUseCount = finalState.xUseCount;
		candidate.score = finalState.outputLength;
		if (!registration.compressed) {
			candidate.registration.operations = registrationOperations;
			candidate.registration.outputLength = registrationOutputLength;
			candidate.registration.expandedLength = registrationExpandedLength;
		}

		if (len(candidate.operations) != candidate.outputLength) {
			candidate.invalidReason = "output length mismatch";
		} else if (!isGoalState(finalState)) {
			candidate.invalidReason = "goal not reached";
		} else if (candidate.outputLength > p.numT) {
			candidate.invalidReason = "output over T";
		} else if (candidate.expandedLength > p.numT) {
			candidate.invalidReason = "expanded over T";
		} else {
			candidate.valid = true;
		}
		return candidate;
	}

	void traceCandidate(const string& label, const Candidate& candidate) const {
		if (candidate.valid) {
			if (candidate.ppSwitchPattern.empty()) {
				trace(label, " X=", candidate.x, ": valid, score=", candidate.score,
					", exp=", candidate.expandedLength, "/", p.numT, ", Y=", candidate.registration.y);
			} else {
				trace(label, " X=", candidate.x, ": valid, score=", candidate.score,
					", exp=", candidate.expandedLength, "/", p.numT, ", Y=", candidate.registration.y,
					", ppMacro=", candidate.ppSwitchPattern,
					", ppExpanded=", candidate.ppSwitchExpandedLength);
			}
		} else {
			if (candidate.ppSwitchPattern.empty()) {
				trace(label, " X=", candidate.x, ": invalid(", candidate.invalidReason,
					"), score=", candidate.score, ", exp=", candidate.expandedLength, "/", p.numT,
					", Y=", candidate.registration.y);
			} else {
				trace(label, " X=", candidate.x, ": invalid(", candidate.invalidReason,
					"), score=", candidate.score, ", exp=", candidate.expandedLength, "/", p.numT,
					", Y=", candidate.registration.y, ", ppMacro=", candidate.ppSwitchPattern,
					", ppExpanded=", candidate.ppSwitchExpandedLength);
			}
		}
	}

	bool isBetter(const Candidate& candidate, const Candidate& base) const {
		return candidate.valid && (!base.valid || candidate.score < base.score);
	}

	string normalizeX(const string& x) const {
		string normalized;
		normalized.reserve(len(x));
		for (char operation : x) {
			normalized.push_back(operation);
			while (true) {
				int n = len(normalized);
				if (n >= 2) {
					char a = normalized[n - 2];
					char b = normalized[n - 1];
					if ((a == 'L' && b == 'R') || (a == 'R' && b == 'L')) {
						normalized.resize(n - 2);
						continue;
					}
					if (a == 'R' && b == 'R') {
						normalized[n - 2] = 'L';
						normalized[n - 1] = 'L';
						continue;
					}
				}
				if (n >= 3) {
					char a = normalized[n - 3];
					char b = normalized[n - 2];
					char c = normalized[n - 1];
					if (a == 'R' && b == 'R' && c == 'R') {
						normalized.resize(n - 3);
						normalized.push_back('L');
						continue;
					}
					if (a == 'L' && b == 'L' && c == 'L') {
						normalized.resize(n - 3);
						normalized.push_back('R');
						continue;
					}
				}
				break;
			}
		}
		return normalized;
	}

	int finalBeamBudget() const {
		return ilinearmap(80, 200, 16.0, 30.0, p.m);
	}

	int exactCandidateScoringBudget() const {
		return ilinearmap(1000, 600, 10, 20, p.n);
	}

	AnnealResult annealX(int fastAnnealTl, int exactReevaluationTl) {
		bool useFastEvaluation = true; // p.m >= 20;
		int rankingEndTl = useFastEvaluation ? fastAnnealTl : exactReevaluationTl;
		struct OperationConfig {
			char operation;
			double weight;
		};
		static constexpr OperationConfig X_OPERATIONS[3] = {
			{'F', 8.0},
			{'L', 1.0},
			{'R', 1.0},
		};
		enum class NeighborType {
			Insert,
			Switch,
			Swap,
			Turn,
			Delete,
		};
		static constexpr int NUM_NEIGHBOR_TYPES = 5;
		struct NeighborConfig {
			NeighborType type;
			double weight;
		};
		static constexpr NeighborConfig NEIGHBOR_CONFIGS[5] = {
			{NeighborType::Insert, 80.0},
			{NeighborType::Switch, 10.0},
			{NeighborType::Swap, 10.0},
			{NeighborType::Turn, 10.0},
			{NeighborType::Delete, 80.0},
		};
		static constexpr double INSERT_LENGTH_WEIGHTS[] = {
			1.0,
			0.5,
			0.25,
			0.125,
			0.0625,
			0.03125,
		};
		static constexpr int RANKED_CANDIDATE_LIMIT = 800;
		static constexpr int RANKED_CANDIDATE_CLASS_LIMIT = RANKED_CANDIDATE_LIMIT * 3 / 4;

		unordered_map<string, Candidate> cache;
		cache.max_load_factor(0.7f);
		cache.reserve(65536);
		vector<const Candidate*> rankedCandidates;
		rankedCandidates.reserve(RANKED_CANDIDATE_LIMIT);
		ll iterationCount = 0;
		ll cacheHitCount = 0;
		ll evaluateCount = 0;
		ll normalizeChangedCount = 0;
		ll normalizedEmptyCount = 0;
		ll sameCandidateCount = 0;
		// ll pruned = 0;
		array<ll, NUM_NEIGHBOR_TYPES> neighborTrialCounts = {};
		array<ll, NUM_NEIGHBOR_TYPES> neighborNewValidCounts = {};

		auto neighborIndex = [](NeighborType type) {
			return (int) type;
		};

		auto neighborName = [](NeighborType type) {
			if (type == NeighborType::Insert) {
				return "insert";
			}
			if (type == NeighborType::Switch) {
				return "switch";
			}
			if (type == NeighborType::Swap) {
				return "swap";
			}
			if (type == NeighborType::Turn) {
				return "turn";
			}
			return "delete";
		};

		auto compareFastCandidate = [](const Candidate* a, const Candidate* b) {
			if (a->score != b->score) {
				return a->score < b->score;
			}
			if (a->expandedLength != b->expandedLength) {
				return a->expandedLength < b->expandedLength;
			}
			return a->x < b->x;
		};

		auto rankedCandidateClass = [](const Candidate* candidate) {
			for (char operation : candidate->x) {
				if (operation == 'L') {
					return 0;
				}
				if (operation == 'R') {
					return 1;
				}
			}
			return 2;
		};

		vector<const Candidate*> rankedScratch;
		rankedScratch.reserve(RANKED_CANDIDATE_LIMIT);
		auto filterSortedRankedCandidates = [&]() {
			rankedScratch.clear();
			int classCounts[2] = {};
			for (const Candidate* candidate : rankedCandidates) {
				int cls = rankedCandidateClass(candidate);
				if (cls < 2 && classCounts[cls] >= RANKED_CANDIDATE_CLASS_LIMIT) {
					continue;
				}
				rankedScratch.push_back(candidate);
				if (cls < 2) {
					classCounts[cls]++;
				}
				if (len(rankedScratch) == RANKED_CANDIDATE_LIMIT) {
					break;
				}
			}
			rankedCandidates.swap(rankedScratch);
		};
		auto filterRankedCandidates = [&]() {
			sort(rankedCandidates.begin(), rankedCandidates.end(), compareFastCandidate);
			filterSortedRankedCandidates();
		};

		Candidate best;
		// ll bestDistanceSum = INF;

		bool cached = false;

		auto getCandidate = [&](const string& normalizedX) -> const Candidate& {
			auto it = cache.find(normalizedX);
			if (it != cache.end()) {
				cacheHitCount++;
				cached = true;
				return it->second;
			}
			cached = false;
			evaluateCount++;
			int bestKnownScore = best.valid ? best.score : INF;
			Candidate candidate =
				useFastEvaluation ? evaluateXFast(normalizedX, bestKnownScore) : evaluateX(normalizedX);
			auto [inserted, _] = cache.emplace(normalizedX, std::move(candidate));
			return inserted->second;
		};

		auto randomOperation = [&]() {
			double r = rng.next_float() * 10.0;
			return r < 8.0 ? 'F' : r < 9.0 ? 'L' : 'R';
		};

		auto randomInsertLength = [&]() {
			double r = rng.next_float() * 1.96875;
			if (r < 1.0)
				return 1;
			r -= 1.0;
			if (r < 0.5)
				return 2;
			r -= 0.5;
			if (r < 0.25)
				return 3;
			r -= 0.25;
			if (r < 0.125)
				return 4;
			r -= 0.125;
			return r < 0.0625 ? 5 : 6;
		};

		auto hasTurnTarget = [&](const string& x) {
			for (int i = 0; i + 1 < len(x); i++) {
				if (x[i] == 'F' && x[i + 1] == 'F') {
					return true;
				}
			}
			return false;
		};

		auto isProtectedDoubleLeft = [&](const string& x, int index) {
			return x[index] == 'L' &&
				((index > 0 && x[index - 1] == 'L') || (index + 1 < len(x) && x[index + 1] == 'L'));
		};

		auto hasSwitchTarget = [&](const string& x) {
			rep0(i, len(x)) {
				if ((x[i] == 'L' || x[i] == 'R') && !isProtectedDoubleLeft(x, i)) {
					return true;
				}
			}
			return false;
		};

		auto pickNeighborType = [&](const string& x) {
			bool canDelete = len(x) >= 2;
			bool canSwap = len(x) >= 2;
			bool canSwitch = hasSwitchTarget(x);
			bool canTurn = hasTurnTarget(x);
			double totalWeight = 80.0;
			NeighborType defaultType = NeighborType::Insert;
			if (canSwitch) {
				totalWeight += 10.0;
				defaultType = NeighborType::Switch;
			}
			if (canSwap) {
				totalWeight += 10.0;
				defaultType = NeighborType::Swap;
			}
			if (canTurn) {
				totalWeight += 10.0;
				defaultType = NeighborType::Turn;
			}
			if (canDelete) {
				totalWeight += 80.0;
				defaultType = NeighborType::Delete;
			}
			double r = rng.next_float() * totalWeight;
			if (r < 80.0)
				return NeighborType::Insert;
			r -= 80.0;
			if (canSwitch) {
				if (r < 10.0)
					return NeighborType::Switch;
				r -= 10.0;
			}
			if (canSwap) {
				if (r < 10.0)
					return NeighborType::Swap;
				r -= 10.0;
			}
			if (canTurn) {
				if (r < 10.0)
					return NeighborType::Turn;
				r -= 10.0;
			}
			if (canDelete) {
				if (r < 80.0)
					return NeighborType::Delete;
			}
			return defaultType;
		};

		struct GeneratedNeighbor {
			string x;
			NeighborType type;
			bool valid;
		};

		auto generateNeighbor = [&](const string& x) {
			string next = x;
			NeighborType type = pickNeighborType(x);
			if (type == NeighborType::Delete) {
				int pos = rng.next_int(len(next));
				next.erase(next.begin() + pos);
			} else if (type == NeighborType::Switch) {
				int selectedPos = -1;
				int candidateCount = 0;
				rep0(i, len(next)) {
					if ((next[i] != 'L' && next[i] != 'R') || isProtectedDoubleLeft(next, i)) {
						continue;
					}
					candidateCount++;
					if (rng.next_int(candidateCount) == 0) {
						selectedPos = i;
					}
				}
				if (selectedPos == -1) {
					return GeneratedNeighbor{next, type, false};
				}
				next[selectedPos] = 'F';
			} else if (type == NeighborType::Swap) {
				int selectedPos = -1;
				int candidateCount = 0;
				rep0(i, len(next) - 1) {
					if (next[i] == next[i + 1]) {
						continue;
					}
					candidateCount++;
					if (rng.next_int(candidateCount) == 0) {
						selectedPos = i;
					}
				}
				if (selectedPos == -1) {
					return GeneratedNeighbor{next, type, false};
				}
				swap(next[selectedPos], next[selectedPos + 1]);
			} else if (type == NeighborType::Turn) {
				int selectedPos = -1;
				int candidateCount = 0;
				for (int i = 0; i + 1 < len(next); i++) {
					if (next[i] != 'F' || next[i + 1] != 'F') {
						continue;
					}
					candidateCount++;
					if (rng.next_int(candidateCount) == 0) {
						selectedPos = i;
					}
				}
				if (selectedPos == -1) {
					return GeneratedNeighbor{next, type, false};
				}
				next.replace(selectedPos, 2, rng.next_int(2) == 0 ? "FLF" : "FRF");
			} else {
				int pos = rng.next_prob(0.1) ? len(next) : rng.next_int(len(next) + 1);
				int insertLength = randomInsertLength();
				char inserted[6];
				rep0(i, insertLength) {
					inserted[i] = randomOperation();
				}
				next.insert(pos, inserted, insertLength);
			}
			return GeneratedNeighbor{next, type, true};
		};

		auto buildInitialXs = [&]() {
			vector<string> initialXs;
			unordered_set<string> seenInitialXs;
			seenInitialXs.reserve(p.m * p.m * 4 + 16);
			auto addInitialX = [&](const string& x) {
				if (seenInitialXs.insert(x).second) {
					initialXs.push_back(x);
				}
			};
			addInitialX("FF");

			vector<int> pos;
			pos.reserve(p.m * 2);
			rep0(ball, p.m) {
				pos.push_back(ballInitialCell[ball]);
				pos.push_back(basketCell[ball]);
			}
			sort(pos.begin(), pos.end());
			pos.erase(unique(pos.begin(), pos.end()), pos.end());
			int numPos = len(pos);
			if (numPos == 0) {
				return initialXs;
			}

			vector<short> allDistance(numPos * 4 * numStates);
			vector<short> allPreviousState(numPos * 4 * numStates);
			vector<char> allPreviousOperation(numPos * 4 * numStates);
			vector<short> que(numStates);
			auto tableOffset = [&](int sourceIndex, int startDir) {
				return (sourceIndex * 4 + startDir) * numStates;
			};
			const short* __restrict nextF = basicNextF.data();
			const short* __restrict nextR = basicNextR.data();
			const short* __restrict nextL = basicNextL.data();

			for (int sourceIndex = 0; sourceIndex < numPos; sourceIndex++) {
				int sourceCell = pos[sourceIndex];
				rep0(startDir, 4) {
					int offset = tableOffset(sourceIndex, startDir);
					short* __restrict distance = allDistance.data() + offset;
					short* __restrict previousState = allPreviousState.data() + offset;
					char* __restrict previousOperation = allPreviousOperation.data() + offset;
					fill(distance, distance + numStates, INF_DIST);
					int startState = (sourceCell << 2) | startDir;
					int head = 0;
					int tail = 0;
					distance[startState] = 0;
					previousState[startState] = -1;
					previousOperation[startState] = '\0';
					que[tail++] = (short) startState;
#define INIT_PUSH(nextState__, operation__)                                      \
					do {                                                                 \
						int nextState_ = (unsigned short) (nextState__);                   \
						if (distance[nextState_] == INF_DIST) {                            \
							distance[nextState_] = (short) (distance[currentState] + 1);     \
							previousState[nextState_] = (short) currentState;                \
							previousOperation[nextState_] = (operation__);                   \
							que[tail++] = (short) nextState_;                                \
						}                                                                  \
					} while (false)
					while (head < tail) {
						int currentState = (unsigned short) que[head++];
						INIT_PUSH(nextF[currentState], 'F');
						INIT_PUSH(nextR[currentState], 'R');
						INIT_PUSH(nextL[currentState], 'L');
					}
#undef INIT_PUSH
				}
			}

			auto restorePath = [&](int sourceIndex, int startDir, int targetState) {
				int offset = tableOffset(sourceIndex, startDir);
				const short* __restrict previousState = allPreviousState.data() + offset;
				const char* __restrict previousOperation = allPreviousOperation.data() + offset;
				int startState = (pos[sourceIndex] << 2) | startDir;
				string operations;
				int currentState = targetState;
				while (currentState != startState) {
					char operation = previousOperation[currentState];
					operations.push_back(operation);
					currentState = previousState[currentState];
				}
				reverse(operations.begin(), operations.end());
				return operations;
			};

			auto buildShortestFlrPath = [&](int sourceIndex, int targetCell) {
				string bestPath;
				int bestDistance = INF;
				rep0(startDir, 4) {
					int offset = tableOffset(sourceIndex, startDir);
					const short* __restrict distance = allDistance.data() + offset;
					rep0(endDir, 4) {
						int targetState = (targetCell << 2) | endDir;
						if (distance[targetState] == INF_DIST || distance[targetState] >= bestDistance) {
							continue;
						}
						bestDistance = distance[targetState];
						bestPath = restorePath(sourceIndex, startDir, targetState);
					}
				}
				return bestPath;
			};

			auto addPosPairPath = [&](int sourceIndex, int targetIndex) {
				string rawPath = buildShortestFlrPath(sourceIndex, pos[targetIndex]);
				if (rawPath.empty()) {
					return;
				}
				string normalizedPath = normalizeX(rawPath);
				if (8 <= len(normalizedPath) && len(normalizedPath) <= 32) {
					addInitialX(normalizedPath);
				}
			};

			rep0(i, numPos) {
				rep(j, i + 1, numPos) {
					addPosPairPath(i, j);
					addPosPairPath(j, i);
				}
			}
			return initialXs;
		};
		vector<string> initialXs = buildInitialXs();
		for (const string& initialX : initialXs) {
			const Candidate& candidate = getCandidate(initialX);
			if (!candidate.valid) {
				continue;
			}
			rankedCandidates.push_back(&candidate);
			if (isBetter(candidate, best)) {
				best = candidate;
			}
		}
		filterRankedCandidates();
		trace("initial candidates: generated=", len(initialXs), ", valid=", len(rankedCandidates),
			", best=", best.score);
		trace("ranking search: mode=", useFastEvaluation ? "fast" : "exact", ", start=", timer(),
			", end=", rankingEndTl, ", cacheSize=", cache.size(), ", rankedSize=", len(rankedCandidates));

		int rankingTimerInterval = 1;
		while (true) {
			if (--rankingTimerInterval <= 0) {
				if (timer() > rankingEndTl || rankedCandidates.empty()) {
					break;
				}
				rankingTimerInterval = 256;
			}

			iterationCount++;
			double r = rng.next_float();
			double r2 = r * r;
			double r4 = r2 * r2;
			double r8 = r4 * r4;
			int parentIndex = (int) (r8 * len(rankedCandidates));
			if (parentIndex >= len(rankedCandidates)) {
				parentIndex = len(rankedCandidates) - 1;
			}
			if (rng.next_prob(0.1)) {
				parentIndex /= 8;
			}
			const Candidate* parent = rankedCandidates[parentIndex];
			GeneratedNeighbor generated = generateNeighbor(parent->x);
			neighborTrialCounts[neighborIndex(generated.type)]++;
			if (!generated.valid) {
				continue;
			}
			string normalizedX = normalizeX(generated.x);
			if (normalizedX != generated.x) {
				normalizeChangedCount++;
			}
			if (normalizedX.empty()) {
				normalizedEmptyCount++;
			}
			if (normalizedX == parent->x) {
				sameCandidateCount++;
				continue;
			}

			const Candidate& candidate = getCandidate(normalizedX);
			if (cached || !candidate.valid) {
				continue;
			}
			if (len(rankedCandidates) >= RANKED_CANDIDATE_LIMIT &&
				!compareFastCandidate(&candidate, rankedCandidates.back())) {
				continue;
			}

			auto insertPos = lower_bound(
				rankedCandidates.begin(), rankedCandidates.end(), &candidate, compareFastCandidate);
			rankedCandidates.insert(insertPos, &candidate);
			filterSortedRankedCandidates();
			neighborNewValidCounts[neighborIndex(generated.type)]++;
			if (isBetter(candidate, best)) {
				best = candidate;
				traceCandidate("fast best", best);
			}
		}
		trace("ranking stats: iterations=", iterationCount, ", evaluated=", evaluateCount,
			", cacheHits=", cacheHitCount, ", cacheSize=", cache.size(),
			", rankedSize=", len(rankedCandidates));
		trace("  normalize: changed=", normalizeChangedCount, ", empty=", normalizedEmptyCount,
			", sameRejected=", sameCandidateCount);
		for (const NeighborConfig& config : NEIGHBOR_CONFIGS) {
			ll trials = neighborTrialCounts[neighborIndex(config.type)];
			ll newValids = neighborNewValidCounts[neighborIndex(config.type)];
			double newValidRate = trials == 0 ? 0.0 : newValids / (double) trials;
			trace("  neighbor ", neighborName(config.type), ": trials=", trials,
				", newValidRate=", newValidRate, " (", newValids, "/", trials, ")");
		}

		assert(best.valid);
		if (!useFastEvaluation) {
			traceCandidate("ranking exact selected", best);
			AnnealResult result;
			result.best = best;
			result.exactCandidates.push_back(best);
			return result;
		}

		Candidate exactBest;
		vector<Candidate> exactCandidates;
		int exactEvaluationCount = 0;
		auto isUsableExactCandidate = [](const Candidate& candidate) {
			return candidate.valid && !candidate.operations.empty();
		};
		auto keepExactCandidate = [&](Candidate&& exactCandidate, int index, bool printCand) {
			if (printCand) {
				traceCandidate("cand " + tos(index), exactCandidate);
			}
			if (!isUsableExactCandidate(exactCandidate)) {
				return false;
			}
			if (isBetter(exactCandidate, exactBest)) {
				exactBest = exactCandidate;
				if (!printCand) {
					traceCandidate("cand " + tos(index), exactCandidate);
				}
				trace("new exact best!");
			}
			exactCandidates.push_back(std::move(exactCandidate));
			return true;
		};
		int nextExactIndex = 0;
		for (; nextExactIndex < len(rankedCandidates); nextExactIndex++) {
			if (timer() >= exactReevaluationTl) {
				break;
			}
			Candidate exactCandidate = evaluateX(rankedCandidates[nextExactIndex]->x);
			exactEvaluationCount++;
			keepExactCandidate(std::move(exactCandidate), nextExactIndex, nextExactIndex < 100);
		}
		trace(
			"anneal exact candidates=", exactEvaluationCount, "/", len(rankedCandidates), ", time=", timer());
		if (!isUsableExactCandidate(exactBest)) {
			trace("anneal exact failed: no usable exact candidate, nextIndex=", nextExactIndex,
				", rankedSize=", len(rankedCandidates), ", fastBestX=", best.x,
				", lastBeamFailure=", lastBeamFailureReason);
		}
		sort(exactCandidates.begin(), exactCandidates.end(), [](const Candidate& a, const Candidate& b) {
			if (a.score != b.score) {
				return a.score < b.score;
			}
			if (a.expandedLength != b.expandedLength) {
				return a.expandedLength < b.expandedLength;
			}
			return a.x < b.x;
		});
		assert(isUsableExactCandidate(exactBest));
		AnnealResult result;
		result.best = std::move(exactBest);
		result.exactCandidates = std::move(exactCandidates);
		return result;
	}

	void solveMain(ostream& out) {
		constexpr int TOTAL_TL = 1900;
		constexpr int ENDGAME_BFS_TL = 1970;
		int finalOrderBeamBudget = finalBeamBudget();
		int exactCandidateBudget = exactCandidateScoringBudget();
		int exactReevaluationTl = TOTAL_TL - finalOrderBeamBudget;
		int fastAnnealTl = exactReevaluationTl - exactCandidateBudget;
		trace("time budget: m=", p.m, ", fastAnnealTl=", fastAnnealTl,
			", exactReevaluationTl=", exactReevaluationTl, ", finalBeamTl=", TOTAL_TL,
			", finalBeamBudget=", finalOrderBeamBudget, ", exactCandidateBudget=", exactCandidateBudget);
		trace("fast eval bfs=", shouldIgnoreFastExpandedLimit() ? "normal" : "countP");

		AnnealResult annealResult = annealX(fastAnnealTl, exactReevaluationTl);
		Candidate best = std::move(annealResult.best);
		vector<Candidate> endgameBfsCandidates;
		for (const Candidate& exactCandidate : annealResult.exactCandidates) {
			endgameBfsCandidates.push_back(exactCandidate);
		}
		unordered_set<string> finalBeamTriedKeys;
		auto finalBeamKey = [](const Candidate& candidate) {
			return candidate.x + "\n" + candidate.registration.operations;
		};
		auto runFinalOrderBeam = [&](const Candidate& base, const string& label) {
			if (timer() >= TOTAL_TL) {
				return false;
			}
			string key = finalBeamKey(base);
			if (!finalBeamTriedKeys.insert(key).second) {
				return false;
			}
			BeamConfig finalOrderBeamConfig;
			finalOrderBeamConfig.beamWidth = 4096;
			finalOrderBeamConfig.sameParentCount = 8;
			finalOrderBeamConfig.useDynamicBeamWidth = true;
			finalOrderBeamConfig.dynamicTimeLimit = TOTAL_TL;
			finalOrderBeamConfig.beamWidthManagerWindow = 2;
			finalOrderBeamConfig.traceFilterBeforeCount = false;
			finalOrderBeamConfig.useFixedRegistration = true;
			finalOrderBeamConfig.fixedRegistration = base.registration;
			finalOrderBeamConfig.enablePpSwitch = true;
			finalOrderBeamConfig.collectLastCandidates = true;
			finalOrderBeamConfig.lastCandidateLimit = 32;
			int finalOrderBeamStartTime = timer();
			Candidate finalOrderBeamCandidate = evaluateX(base.x, finalOrderBeamConfig);
			for (const Candidate& candidate : lastBeamTerminalCandidates) {
				endgameBfsCandidates.push_back(candidate);
			}
			endgameBfsCandidates.push_back(finalOrderBeamCandidate);
			int finalOrderBeamElapsed = timer() - finalOrderBeamStartTime;
			traceCandidate(label, finalOrderBeamCandidate);
			trace(label, " time=", finalOrderBeamElapsed, " ms");
			if (isBetter(finalOrderBeamCandidate, best)) {
				trace(
					label, " accepted: oldScore=", best.score, ", newScore=", finalOrderBeamCandidate.score);
				best = std::move(finalOrderBeamCandidate);
			}
			return true;
		};
		if (timer() >= TOTAL_TL) {
			trace("final order beam skipped: time=", timer(), " ms");
		} else {
			runFinalOrderBeam(best, "final order beam");
			int extraFinalBeamCount = 0;
			for (const Candidate& exactCandidate : annealResult.exactCandidates) {
				if (timer() >= TOTAL_TL) {
					break;
				}
				if (runFinalOrderBeam(exactCandidate, "final order beam extra")) {
					extraFinalBeamCount++;
				}
			}
			trace("final order beam extra count=", extraFinalBeamCount, ", time=", timer(), " ms");
		}

		// {
		// 	BeamConfig ppSwitchBeamConfig;
		// 	ppSwitchBeamConfig.beamWidth = 4096;
		// 	ppSwitchBeamConfig.sameParentCount = 8;
		// 	ppSwitchBeamConfig.traceFilterBeforeCount = true;
		// 	ppSwitchBeamConfig.useFixedRegistration = true;
		// 	ppSwitchBeamConfig.fixedRegistration = best.registration;
		// 	ppSwitchBeamConfig.enablePpSwitch = true;
		// 	int ppSwitchBeamStartTime = timer();
		// 	Candidate ppSwitchBeamCandidate = evaluateX(best.x, ppSwitchBeamConfig);
		// 	int ppSwitchBeamElapsed = timer() - ppSwitchBeamStartTime;
		// 	traceCandidate("final pp-switch beam", ppSwitchBeamCandidate);
		// 	trace("final pp-switch beam time=", ppSwitchBeamElapsed, " ms");
		// 	if (isBetter(ppSwitchBeamCandidate, best)) {
		// 		trace("final pp-switch beam accepted: oldScore=", best.score,
		// 			", newScore=", ppSwitchBeamCandidate.score);
		// 		best = std::move(ppSwitchBeamCandidate);
		// 	}
		// }

		// Candidate finalSingleStepCandidate = runFinalSingleStepBeam(best);
		// traceCandidate("final single-step beam", finalSingleStepCandidate);
		// if (isBetter(finalSingleStepCandidate, best)) {
		// 	trace("final single-step beam accepted: oldScore=", best.score,
		// 		", newScore=", finalSingleStepCandidate.score);
		// 	best = std::move(finalSingleStepCandidate);
		// }
		// best = std::move(finalSingleStepCandidate);

		// {
		// 	BeamConfig finalBeamConfig;
		// 	finalBeamConfig.beamWidth = 2048;
		// 	finalBeamConfig.sameParentCount = 8;
		// 	Candidate finalBeamCandidate = evaluateX(best.x, finalBeamConfig);
		// 	traceCandidate("final beam", finalBeamCandidate);
		// 	if (isBetter(finalBeamCandidate, best)) {
		// 		trace("final beam accepted: oldScore=", best.score,
		// 			", newScore=", finalBeamCandidate.score);
		// 		best = std::move(finalBeamCandidate);
		// 	}
		// }

		endgameBfsCandidates.push_back(best);
		sort(endgameBfsCandidates.begin(), endgameBfsCandidates.end(),
			[](const Candidate& a, const Candidate& b) {
				if (a.score != b.score) {
					return a.score < b.score;
				}
				if (a.expandedLength != b.expandedLength) {
					return a.expandedLength < b.expandedLength;
				}
				return a.operations < b.operations;
			});
		vector<Candidate> uniqueEndgameBfsCandidates;
		uniqueEndgameBfsCandidates.reserve(len(endgameBfsCandidates));
		unordered_set<string> usedEndgameOperations;
		for (Candidate& candidate : endgameBfsCandidates) {
			if (!candidate.valid || candidate.operations.empty()) {
				continue;
			}
			if (!usedEndgameOperations.insert(candidate.operations).second) {
				continue;
			}
			uniqueEndgameBfsCandidates.push_back(std::move(candidate));
		}
		int endgameBfsTryCount = 0;
		rep0(index, len(uniqueEndgameBfsCandidates)) {
			if (timer() >= ENDGAME_BFS_TL) {
				break;
			}
			endgameBfsTryCount++;
			Candidate trial = uniqueEndgameBfsCandidates[index];
			int oldScore = trial.score;
			tryEndgameBfs(trial, ENDGAME_BFS_TL);
			if (isBetter(trial, best)) {
				trace("endgame bfs accepted: rank=", index, ", oldBest=", best.score,
					", candidateOld=", oldScore, ", new=", trial.score);
				best = std::move(trial);
			}
		}
		trace("endgame bfs candidates=", len(uniqueEndgameBfsCandidates), ", tried=", endgameBfsTryCount,
			", time=", timer(), " ms");
		assertFinalOperationsValid(best);

		trace("final:");
		trace("X=", best.x, " score=", best.score, " eexp=", best.expandedLength, "/", p.numT,
			" Y=", best.registration.y);
		trace(best.operations);

		for (char operation : best.operations) {
			out << operation << '\n';
		}
		result.score = best.score;
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
	oss << "seed n m t" << endl;
	rep0(seed, 1000) {
		Solver sol;
		sol.load(seed);
		int numTrees = 0;
		int n = sol.p.n;
		int m = sol.p.m;
		int t = sol.p.numT / (n * m);
		oss << seed << " " << n << " " << m << " " << t << endl;
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
