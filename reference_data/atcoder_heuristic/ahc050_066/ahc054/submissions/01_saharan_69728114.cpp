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
		assert(direction == "min" || direction == "max");
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
constexpr double EPS = 1e-8;
constexpr int MIN_N = 20;
constexpr int MAX_N = 40;
constexpr int MAX_CELLS = MAX_N * MAX_N;
#define repn(i) rep0(i, n)
#define rep2() repn(i) repn(j)

// portable problem data
struct Problem {
	int n;
	ivec2 entrance; // entrance
	ivec2 target; // target pos
	vector<vector<bool>> bs; // if blocked

	void load(istream& in) {
		in >> n >> target.i >> target.j;
		assert_in(n, MIN_N, MAX_N);
		entrance.i = 0;
		entrance.j = n / 2;
		bs.resize(n, vector<bool>(n));
		rep0(y, n) rep0(x, n) {
			char c;
			in >> c;
			bs[y][x] = (c == 'T');
		}
	}
};

struct TimeSet {
	int num = 0; // up to 4
	array<double, 4> times;
	double minTime = INF;

	bool contains(double t) const {
		if (num == 0)
			return false;
		if (times[0] == t)
			return true;
		if (num == 1)
			return false;
		if (times[1] == t)
			return true;
		if (num == 2)
			return false;
		if (times[2] == t)
			return true;
		if (num == 3)
			return false;
		if (times[3] == t)
			return true;
		return false;
	}

	void insert(double t) {
		assert_lt(num, 4) << show(times, t);
		times[num++] = t;
		update_min(minTime, t);
	}

	void erase(double t) {
		assert_ge(num, 1);
		if (times[0] == t) {
			times[0] = times[--num];
			if (minTime == t)
				minTime = computeMin();
			return;
		}
		assert_ge(num, 2);
		if (times[1] == t) {
			times[1] = times[--num];
			if (minTime == t)
				minTime = computeMin();
			return;
		}
		assert_ge(num, 3);
		if (times[2] == t) {
			times[2] = times[--num];
			if (minTime == t)
				minTime = computeMin();
			return;
		}
		assert_ge(num, 4);
		if (times[3] == t) {
			times[3] = times[--num];
			if (minTime == t)
				minTime = computeMin();
			return;
		}
		fail();
	}

	void clear() {
		num = 0;
		minTime = INF;
	}

	bool empty() const {
		return num == 0;
	}

private:
	double computeMin() const {
		if (num == 0)
			return INF;
		double res = times[0];
		if (num == 1)
			return res;
		update_min(res, times[1]);
		if (num == 2)
			return res;
		update_min(res, times[2]);
		if (num == 3)
			return res;
		update_min(res, times[3]);
		return res;
	}
};

struct RoomInfo {
	int posi = -1;
	int pathPosi = -1;
	vector<int> outers;
	int area = 0;
	int maxDist = 0;
	double effectiveCount = 0;
	bool isPerfect = false;
};

struct PathIndex {
	int data;

	PathIndex() : data(-1) {
	}

	PathIndex(int kind, int index) : data((kind << 24) | index) {
		assert_in(kind, 0, 0xff);
		assert_in(index, 0, 0xffffff);
	}

	// 0: prologue, 1: path1, 2: path2
	int kind() const {
		return data == -1 ? -1 : data >> 24;
	};

	// index in the path
	int index() const {
		return data == -1 ? -1 : data & 0xffffff;
	}

	bool none() const {
		return data == -1;
	}
};

struct Evaluator {
	struct State {
		ivec2 pos;
		ivec2 target;
		vector<bool> seen;
		vector<bool> passed;
		vector<int> dists;
	};

	double evaluate(const int n, const ivec2 entrance, const ivec2 target, const vector<bool>& hasTree,
		vector<int> priority, bool final) {
		const int n2 = n * n;

		State st;

		rngen rng(final ? 12345 : 45678);
		int iters = final ? 100 : 30;

		vector<int> stepPos;
		vector<ivec2> unpack;
		rep2() {
			ivec2 pos(i, j);
			unpack.emplace_back(i, j);
			auto step = [&](ivec2 d) {
				ivec2 npos = pos + d;
				stepPos.push_back(npos.pack_if_in_bounds(n));
			};
			step(ivec2(-1, 0));
			step(ivec2(1, 0));
			step(ivec2(0, -1));
			step(ivec2(0, 1));
			step(ivec2(-1, -1));
			step(ivec2(-1, 1));
			step(ivec2(1, -1));
			step(ivec2(1, 1));
		}

		int totalSteps = 0;
		rep0(iter, iters) {
			if (iter > 0 || priority.empty()) {
				priority.clear();
				rep2() {
					ivec2 pos(i, j);
					if (pos != entrance) {
						priority.push_back(pos.pack(n));
					}
				}
				shuffle(priority, rng);
			}

			st.passed.assign(n2, false);
			st.seen.assign(n2, false);
			st.pos = entrance;

			auto updateTarget = [&](ivec2 newTarget) {
				st.target = newTarget;
				st.dists.assign(n2, INF);
				int targeti = newTarget.pack(n);
				st.dists[targeti] = 0;
				static queue<int> q;
				q.push(targeti);
				while (!q.empty()) {
					int posi = q.front();
					q.pop();
					rep0(dir, 4) {
						int nposi = stepPos[posi << 3 | dir];
						if (nposi == -1)
							continue;
						if (hasTree[nposi] && st.seen[nposi] || st.dists[nposi] != INF)
							continue;
						st.dists[nposi] = st.dists[posi] + 1;
						q.push(nposi);
					}
				}
			};

			int index = 0;
			updateTarget(unpack[priority[index++]]);
			int steps = 0;

			while (true) {
				int posi = st.pos.pack(n);
				if (iter < 4 && final && render && !st.passed[posi]) {
					mov.target("eval");
					mov.no_stroke();
					rep2() {
						ivec2 pos(i, j);
						int posi = pos.pack(n);
						if (hasTree[posi]) {
							if (st.seen[posi]) {
								mov.fill(0, 0.5, 0);
							} else {
								mov.fill(0, 0.2, 0);
							}
							mov.rect(j + 0.2, i + 0.2, 0.6, 0.6);
							// if (cells[posi].blocked) {
							// 	mov.rect(j + 0.2, i + 0.2, 0.6, 0.6);
							// } else {
							// 	mov.circle(j + 0.5, i + 0.5, 0.3);
							// }
						} else if (st.seen[posi]) {
							mov.fill(0.5, 0.5, 1);
							mov.rect(j, i, 1, 1);
						}
					}
					mov.fill(1);
					mov.circle(st.target.j + 0.5, st.target.i + 0.5, 0.2);
					mov.fill(1, 0, 0);
					mov.circle(st.pos.j + 0.5, st.pos.i + 0.5, 0.3);
					mov.comment(show(steps));
					mov.end_frame();
				}
				if (st.pos == target) {
					break;
				}
				bool changed = false;
				// update vision
				if (!st.passed[posi]) {
					st.passed[posi] = true;
					rep0(dir, 4) {
						ivec2 d = ivec2::dir(dir);
						int posi = st.pos.pack(n);
						do {
							bool newlySeen = !st.seen[posi];
							st.seen[posi] = true;
							if (hasTree[posi]) {
								changed |= newlySeen;
								break;
							}
							posi = stepPos[posi << 3 | dir];
						} while (posi != -1);
					}
				}
				bool flowerSeen = st.seen[target.pack(n)];
				bool targetSeen = st.seen[st.target.pack(n)];
				if (changed) {
					updateTarget(st.target);
				}
				if (flowerSeen && st.target != target) {
					// trace("flower seen");
					updateTarget(target);
				}
				if ((targetSeen || st.dists[st.pos.pack(n)] == INF) && !flowerSeen) {
					// if (targetSeen)
					// 	trace("target seen");
					// if (st.dists[st.pos.pack(n)] == INF)
					// 	trace("no path to target");
					updateTarget(st.pos);
					while (st.seen[priority[index]] || st.dists[priority[index]] == INF) {
						index++;
						if (index == len(priority)) {
							return 0;
						}
					}
					// trace("new target: ", field.unpack[priority[index]]);
					updateTarget(unpack[priority[index]]);
				}
				int minDist = INF;
				int nextDir = -1;
				rep0(dir, 4) {
					int nposi = stepPos[st.pos.pack(n) << 3 | dir];
					if (nposi == -1)
						continue;
					if (update_min(minDist, st.dists[nposi])) {
						nextDir = dir;
					}
				}
				assert_ne(nextDir, -1);
				st.pos += ivec2::dir(nextDir);
				totalSteps++;
				steps++;
				// trace(show(st.pos, steps));
			}
			if (final && iter % 10 == 0)
				trace(totalSteps / (double) (iter + 1), " ", show(iter));
		}
		return totalSteps / (double) iters;
	}

	double evaluateRoom(
		const int n, const ivec2 entrance, const vector<bool>& boundary, const vector<bool>& hasTree) {
		const int n2 = n * n;

		State st;

		rngen rng(12345);
		int iters = 50;

		vector<int> stepPos;
		vector<ivec2> unpack;
		rep2() {
			ivec2 pos(i, j);
			unpack.emplace_back(i, j);
			auto step = [&](ivec2 d) {
				ivec2 npos = pos + d;
				stepPos.push_back(npos.pack_if_in_bounds(n));
			};
			step(ivec2(-1, 0));
			step(ivec2(1, 0));
			step(ivec2(0, -1));
			step(ivec2(0, 1));
			step(ivec2(-1, -1));
			step(ivec2(-1, 1));
			step(ivec2(1, -1));
			step(ivec2(1, 1));
		}

		vector<int> priority;
		rep2() {
			ivec2 pos(i, j);
			if (pos != entrance && !boundary[pos.pack(n)]) {
				priority.push_back(pos.pack(n));
			}
		}

		int totalSteps = 0;
		int totalNumTargetChanged = 0;
		rep0(iter, iters) {
			shuffle(priority, rng);

			st.passed.assign(n2, false);
			st.seen.assign(n2, false);

			rep2() {
				ivec2 pos(i, j);
				if (boundary[pos.pack(n)]) {
					st.seen[pos.pack(n)] = true;
				}
			}

			int numTargetChanged = 1;
			st.pos = entrance;

			auto updateTarget = [&](ivec2 newTarget) {
				st.target = newTarget;
				st.dists.assign(n2, INF);
				int targeti = newTarget.pack(n);
				st.dists[targeti] = 0;
				static queue<int> q;
				q.push(targeti);
				while (!q.empty()) {
					int posi = q.front();
					q.pop();
					rep0(dir, 4) {
						int nposi = stepPos[posi << 3 | dir];
						if (nposi == -1)
							continue;
						if ((boundary[nposi] || hasTree[nposi]) && st.seen[nposi] || st.dists[nposi] != INF)
							continue;
						st.dists[nposi] = st.dists[posi] + 1;
						q.push(nposi);
					}
				}
			};

			int index = 0;
			updateTarget(unpack[priority[index++]]);
			int steps = 0;

			while (true) {
				int posi = st.pos.pack(n);
				bool changed = false;
				// update vision
				if (!st.passed[posi]) {
					st.passed[posi] = true;
					rep0(dir, 4) {
						ivec2 d = ivec2::dir(dir);
						int posi = st.pos.pack(n);
						do {
							bool newlySeen = !st.seen[posi];
							st.seen[posi] = true;
							if (hasTree[posi]) {
								changed |= newlySeen;
								break;
							}
							posi = stepPos[posi << 3 | dir];
						} while (posi != -1);
					}
				}
				bool targetSeen = st.seen[st.target.pack(n)];
				if (changed) {
					updateTarget(st.target);
				}
				if ((targetSeen || st.dists[st.pos.pack(n)] == INF)) {
					st.pos = entrance;
					updateTarget(st.pos);
					while (index < len(priority) &&
						(st.seen[priority[index]] || st.dists[priority[index]] == INF)) {
						index++;
					}
					if (index >= len(priority)) {
						assert_eq(index, len(priority));
						break;
					}
					updateTarget(unpack[priority[index]]);
					numTargetChanged++;
				}
				int minDist = INF;
				int nextDir = -1;
				rep0(dir, 4) {
					int nposi = stepPos[st.pos.pack(n) << 3 | dir];
					if (nposi == -1)
						continue;
					if (update_min(minDist, st.dists[nposi])) {
						nextDir = dir;
					}
				}
				assert_ne(nextDir, -1);
				st.pos += ivec2::dir(nextDir);
				totalSteps++;
				steps++;
			}
			totalNumTargetChanged += numTargetChanged;
		}
		return totalNumTargetChanged / (double) iters;
	}
};

struct Cell {
	bool blocked = false;
	TimeSet observed;
	double steppedAt = -1; // -1 means never stepped

	PathIndex pathIndex; // only meaningful when steppedAt != -1
	PathIndex tmpPathIndex; // used in reconnect

	bool door = false; // whether this cell is a door of some room
	RoomInfo rinfo;
	vector<int> outerOf; // list of rooms where this cell is outer
};

struct Path {
	int index;
	ivec2 start;
	vector<ivec2> poss;
	vector<double> times;
	vector<fast_vector<pair<int, double>, 3>> obss;
};

struct Field {
	int n = 0;
	int n2 = 0;
	ivec2 entrance;
	ivec2 target;
	vector<Cell> cells;
	vector<ivec2> pathBase;
	vector<Path> paths;
	vector<ivec2> unpack;
	fast_iset<MAX_CELLS> doorPoss;
	int numRooms = 0;
	int roomArea = 0;
	double score = 0;
	fast_iset<MAX_CELLS> mustNotRemove;
	fast_iset<MAX_CELLS> shouldBeObserved;
	fast_iset<MAX_CELLS> shouldBeRemoved;
	fast_iset<MAX_CELLS> dangerousCells;
	vector<int> pathDists;
	bool extended = false;
	double density = 0;

	vector<int> stepPos; // U, D, L, R, UL, UR, DL, DR

	Cell& at(ivec2 pos) {
		return cells[pos.pack(n)];
	}

	const Cell& at(ivec2 pos) const {
		return cells[pos.pack(n)];
	}

	vector<int> pathLens() {
		return lambda::map(paths, [](const Path& p) {
			return len(p.poss);
		});
	}

	void init(const Problem& p, rngen& rng) {
		this->p = p;
		n = p.n;
		n2 = n * n;
		entrance = p.entrance;
		target = p.target;
		cells.resize(n2);
		int blocked = 0;
		rep2() {
			ivec2 pos(i, j);
			int posi = pos.pack(n);
			cells[posi].blocked = p.bs[i][j];
			blocked += cells[posi].blocked;
			unpack.emplace_back(i, j);

			auto step = [&](ivec2 d) {
				ivec2 npos = pos + d;
				stepPos.push_back(npos.pack_if_in_bounds(n));
			};
			step(ivec2(-1, 0));
			step(ivec2(1, 0));
			step(ivec2(0, -1));
			step(ivec2(0, 1));
			step(ivec2(-1, -1));
			step(ivec2(-1, 1));
			step(ivec2(1, -1));
			step(ivec2(1, 1));
		}
		density = (double) blocked / n2;
		initPath(p, rng);
	}

	ivec2 rotPos(ivec2 pos, int rot) {
		rot = rot & 3;
		if (rot == 0) {
			return pos;
		} else if (rot == 1) {
			return {pos.j, n - 1 - pos.i};
		} else if (rot == 2) {
			return {n - 1 - pos.i, n - 1 - pos.j};
		} else {
			return {n - 1 - pos.j, pos.i};
		}
	}

	void initRot(int rot) {
		rot = rot & 3;
		entrance = rotPos(p.entrance, rot);
		target = rotPos(p.target, rot);
		auto bs = p.bs;
		rep2() {
			auto rpos = rotPos({i, j}, rot);
			bs[rpos.i][rpos.j] = p.bs[i][j];
		}
		paths.clear();
		pathBase.clear();
		cells.clear();
		cells.resize(n2);
		rep2() {
			ivec2 pos(i, j);
			auto& cell = cells[pos.pack(n)];
			cell.blocked = bs[i][j];
		}
		numRooms = 0;
		roomArea = 0;
		doorPoss.clear();
	}

	void filterTrees() {
		vector<int> q;
		double time = 0;
		PathIndex pathIdx;
		for (int posi : shouldBeRemoved) {
			auto& cell = cells[posi];
			assert(!cell.blocked);
			cell.observed.clear();
			rep0(dir, 4) {
				int nposi = stepPos[posi << 3 | dir];
				if (nposi == -1)
					continue;
				auto& ncell = cells[nposi];
				if (ncell.steppedAt != -1) {
					time = ncell.steppedAt;
					pathIdx = ncell.pathIndex;
				}
			}
			q.push_back(posi);
		}
		vector<RoomInfo> roomsToRemove;
		for (int posi : doorPoss) {
			if (mustNotRemove.contains(posi)) {
				roomsToRemove.push_back(cells[posi].rinfo);
			}
		}
		for (const auto& rinfo : roomsToRemove) {
			removeRoom(rinfo);
		}
		// flood fill path
		while (!q.empty()) {
			int posi = q.back();
			q.pop_back();
			auto& cell = cells[posi];
			if (cell.steppedAt != -1)
				continue;
			cell.steppedAt = time;
			cell.pathIndex = pathIdx;
			rep0(dir, 4) {
				int nposi = stepPos[posi << 3 | dir];
				if (nposi == -1)
					continue;
				auto& ncell = cells[nposi];
				if (ncell.blocked || ncell.steppedAt != -1 || !ncell.observed.empty())
					continue;
				q.push_back(nposi);
			}
		}
	}

	// numLastStraightCells, deadline
	pair<int, double> getFlowerInfo() const {
		auto& path = paths[0]; // path containing the flower
		auto& poss = path.poss;
		auto& times = path.times;
		int l = len(poss);
		int numLastStraightCells = 0;
		while (numLastStraightCells < l) {
			ivec2 pos = poss[l - 1 - numLastStraightCells];
			if (pos.i == target.i || pos.j == target.j) {
				numLastStraightCells++;
			} else {
				break;
			}
		}
		if (numLastStraightCells == l)
			return {numLastStraightCells, 0};
		return {numLastStraightCells, times[l - numLastStraightCells - 1] + EPS};
	}

	double getFlowerGoodness() {
		if (paths.empty())
			return 0;

		mustNotRemove.clear();
		shouldBeObserved.clear();
		shouldBeRemoved.clear();
		dangerousCells.clear();

		auto& path = paths[0]; // path containing the flower
		auto& poss = path.poss;
		auto& times = path.times;

		int l = len(poss);
		auto [numLastStraightCells, deadline] = getFlowerInfo();
		int firstStraightCell = l - numLastStraightCells;
		if (firstStraightCell == 0)
			return 0.1;
		rep(i, firstStraightCell, l) {
			ivec2 pos = poss[i];
			int posi = pos.pack(n);
			rep0(dir, 4) {
				int nposi = stepPos[posi << 3 | dir];
				if (nposi == -1)
					continue;
				if (cells[nposi].observed.num > 0) {
					mustNotRemove.insert(nposi);
					if (i > firstStraightCell && cells[nposi].observed.minTime > deadline) {
						shouldBeObserved.insert(nposi);
					}
				}
			}
		}
		int back = (2 * poss[firstStraightCell] - poss[firstStraightCell + 1]).pack_if_in_bounds(n);
		if (back != -1 && cells[back].observed.num > 0) {
			mustNotRemove.insert(back);
			if (cells[back].observed.minTime > deadline) {
				shouldBeObserved.insert(back);
			}
		}
		for (int posi : shouldBeObserved) {
			if (!tryToObserve(posi, deadline)) {
				dangerousCells.insert(posi);
			}
		}

		int side = (2 * poss[firstStraightCell] - poss[firstStraightCell - 1]).pack_if_in_bounds(n);
		int extraDangerousArea = (numLastStraightCells - 2) + countDangerousArea(side);
		double res = 3.0 / (3 + extraDangerousArea);
		return res;
	}

	int countDangerousArea(int excludePos) const {
		static fast_iset<MAX_CELLS> visited;
		visited.clear();
		static vector<int> q;
		q.clear();
		for (int posi : dangerousCells) {
			visited.insert(posi);
			q.push_back(posi);
		}
		while (!q.empty()) {
			int posi = q.back();
			q.pop_back();
			rep0(dir, 4) {
				int nposi = stepPos[posi << 3 | dir];
				if (nposi == -1 || visited.contains(nposi) || !cells[nposi].observed.empty() ||
					cells[nposi].steppedAt != -1 || nposi == excludePos)
					continue;
				visited.insert(nposi);
				q.push_back(nposi);
			}
		}
		return len(visited);
	}

	bool tryToObserve(int posi, double deadline) {
		int minDist = INF;
		int bestDir = -1;
		rep0(dir, 4) {
			int dist = 0;
			static vector<PathIndex> pathIdxs;
			pathIdxs.clear();
			int nposi = stepPos[posi << 3 | dir];
			bool ok = false;
			while (nposi != -1) {
				auto& cell = cells[nposi];
				if (cell.blocked || !cell.outerOf.empty()) {
					// TODO: remove closures instead?
					// trace("can't cut at ", unpack[posi], " dir=", dir, " blocked=", cell.blocked,
					// 	" outerOf=", cell.outerOf);
					break; // can't cut
				}
				if (cell.steppedAt != -1) {
					if (dist == 0)
						break;
					// connected to the path
					assert_gt(pathIdxs.size(), 0);
					ok = true;
					break;
				}
				bool ng = false;
				dist++;
				rep0(dir, 4) {
					int npos2i = stepPos[nposi << 3 | dir];
					if (npos2i == -1)
						continue;
					auto& cell = cells[npos2i];
					double steppedAt = cell.steppedAt;
					if (steppedAt != -1) {
						if (steppedAt > deadline) {
							ng = true;
							break; // exceeds deadline
						}
						pathIdxs.push_back(cell.pathIndex);
					}
				}
				if (ng)
					break;
				nposi = stepPos[nposi << 3 | dir];
			}
			if (!ok)
				continue;
			if (willShortcutHappen(pathIdxs)) {
				// trace("non-trivial shortcut at ", unpack[posi], " dir=", dir);
				// for (auto pi : pathIdxs)
				// 	trace("  ", pi.kind(), pi.index());
				continue; // non-trivial shortcut
			}
			if (update_min(minDist, dist)) {
				// trace("best dir=", dir, " dist=", dist);
				bestDir = dir;
			}
		}
		if (bestDir == -1)
			return false;
		int nposi = posi;
		rep0(dist, minDist) {
			nposi = stepPos[nposi << 3 | bestDir];
			assert_ne(nposi, -1);
			auto& cell = cells[nposi];
			assert_eq(cell.steppedAt, -1);
			assert(!cell.blocked);
			if (!cell.observed.empty()) {
				shouldBeRemoved.insert(nposi);
			}
		}
		return true;
	}

	template <typename F>
	double computeScore(F posiToPathIndex) {
		static vector<tuple<PathIndex, double>> closures; // path index, effective count
		closures.clear();
		for (int posi : doorPoss) {
			auto& rinfo = cells[posi].rinfo;
			PathIndex pi = posiToPathIndex(rinfo.pathPosi);
			assert(!pi.none()) << show(unpack[posi], unpack[rinfo.pathPosi]);
			closures.emplace_back(pi, rinfo.effectiveCount);
		}
		double countSum = 0;
		for (auto [_, c] : closures) {
			countSum += c;
		}
		double weight = 2 / (countSum * countSum);
		double meanDist = 0;
		int numC = len(closures);
		rep(i, 1, numC) {
			auto [idx1, c1] = closures[i];
			int pathi1 = idx1.kind() - 1;
			rep0(j, i) {
				auto [idx2, c2] = closures[j];
				int pathi2 = idx2.kind() - 1;
				int dist;
				if (pathi1 == pathi2) {
					dist = abs(idx1.index() - idx2.index());
				} else {
					dist = pathDists[pathi1 * len(paths) + pathi2] + idx1.index() + idx2.index();
				}
				assert_lt(dist, n2) << show(idx1.data, idx2.data, pathi1, pathi2, dist);
				meanDist += dist * c1 * c2 * weight;
			}
		}
		double meanClosureUse = 1.0 * countSum;
		double flowerGoodness = getFlowerGoodness();
		int lenSum = 0;
		for (auto& path : paths)
			lenSum += len(path.poss);
		return (lenSum + meanDist * meanClosureUse) * flowerGoodness;
	}

	void addRoom(const RoomInfo& rinfo) {
		int posi = rinfo.posi;
		// trace("adding room at ", unpack[posi], " area=", rinfo.area);
		assert(!cells[posi].door);
		doorPoss.insert(posi);
		cells[posi].door = true;
		cells[posi].rinfo = rinfo;
		numRooms++;
		roomArea += rinfo.area;
		for (int outerPosi : rinfo.outers) {
			cells[outerPosi].outerOf.push_back(posi);
		}
	}

	void removeRoom(const RoomInfo& rinfo) {
		int posi = rinfo.posi;
		// trace("removing room at ", unpack[posi], " area=", rinfo.area);
		assert(cells[posi].door);
		doorPoss.erase(posi);
		cells[posi].door = false;
		cells[posi].rinfo.posi = -1;
		numRooms--;
		roomArea -= rinfo.area;
		for (int outerPosi : rinfo.outers) {
			erase(cells[outerPosi].outerOf, posi);
		}
	}

	bool reconnect(int pathi, int st, int en, double tol, rngen& rng, bool simple = false) {
		// path: -- st -- st+1 ... en-1 -- en --
		//    => -- st -- (random path) -- en --
		assert_size(pathi, len(paths));
		auto& path = paths[pathi];
		auto& obss = path.obss;
		auto& poss = path.poss;
		auto& times = path.times;
		assert_ge(st, 0);
		assert_lt(en, len(poss));
		assert_ge(en - st, 2);

		int pathKind = pathi + 1;

		double stTime = times[st];
		double enTime = times[en];

		// observations between st and en are ignored during the reconnection
		bool blocked[MAX_CELLS] = {};
		bool free[MAX_CELLS] = {};

		auto isBlocked = [&](int posi) {
			auto& cell = cells[posi];
			int kind = cell.pathIndex.kind();
			if (cell.steppedAt != -1) {
				if (kind != pathKind || cell.steppedAt < stTime || cell.steppedAt > enTime) {
					blocked[posi] = true;
					return true;
				}
			}
			auto& obs = cell.observed;
			if (obs.num == 0) {
				free[posi] = true;
				return false;
			}
			rep0(i, obs.num) {
				double t = obs.times[i];
				// negative: observed in prologue
				if (t < 0 || t < stTime || t > enTime + EPS) {
					blocked[posi] = true;
					return true;
				}
			}
			rep0(dir, 4) {
				int nposi = stepPos[posi << 3 | dir];
				if (nposi == -1)
					continue;
				auto& ncell = cells[nposi];
				if (ncell.steppedAt != -1 && ncell.pathIndex.kind() != pathKind) {
					blocked[posi] = true;
					return true;
				}
			}
			free[posi] = true;
			return false;
		};

		static vector<ivec2> subpath;
		subpath.clear();
		static vector<fast_vector<pair<int, double>, 3>> subObss;
		subObss.clear();

		auto revertPathIndices = [&]() {
			for (auto p : subpath) {
				int pi = p.pack(n);
				cells[pi].pathIndex = cells[pi].tmpPathIndex;
			}
		};

		bool freeTail = pathi > 0 && en == len(poss) - 1;
		bool noStraight = en >= len(poss) - 2 && len(poss) > 20;

		// build a subpath from st to en
		{
			int pposi = st > 0 ? poss[st - 1].pack(n) : -1;
			int posi = poss[st].pack(n);

			ivec2 goal = poss[en];
			int goali = goal.pack(n);

			assert(!isBlocked(posi)) << show(st, en, unpack[posi], goal);
			assert(!isBlocked(goali)) << show(st, en, unpack[posi], goal);

			int randomSteps = rng.next_int(2, 6);
			int leftSteps = 10 + (en - st) * 4;
			while (true) {
				int nposi = -1;
				int dirs[4] = {0, 1, 2, 3};
				shuffle(dirs, rng);
				if (randomSteps-- > 0) {
					// random step
					int forbid = subpath.empty() && !freeTail ? poss[st + 1].pack(n) : -1;
					rep0(i, 4) {
						int pos2i = stepPos[posi << 3 | dirs[i]];
						if (pos2i == -1 || pos2i == forbid)
							continue;
						if (blocked[pos2i] || cells[pos2i].blocked || (!free[pos2i] && isBlocked(pos2i)))
							continue;
						nposi = pos2i;
						break;
					}
				} else {
					// greedy step
					int bestDist = INF;
					rep0(i, 4) {
						int pos2i = stepPos[posi << 3 | dirs[i]];
						if (pos2i == -1)
							continue;
						if (blocked[pos2i] || cells[pos2i].blocked || (!free[pos2i] && isBlocked(pos2i)))
							continue;
						int dist = (goal - unpack[pos2i]).mnorm();
						if (update_min(bestDist, dist)) {
							nposi = pos2i;
						}
					}
				}
				if (freeTail) {
					if (nposi == -1) {
						if (subpath.empty())
							return false;
						break; // move as much as possible
					}
				} else {
					if (nposi == goali) {
						// done
						break;
					}
					if (nposi == -1 || leftSteps-- <= 0) {
						// stuck
						// trace("stuck at ", pos, " (", st, ", ", en, ")");
						revertPathIndices();
						return false;
					}
				}
				rep0(dir, 4) {
					int pos2i = stepPos[posi << 3 | dir];
					if (pos2i == -1)
						continue;
					if (pos2i != pposi && pos2i != nposi) {
						// block other neighbors
						blocked[pos2i] = true;
					}
				}
				blocked[posi] = true;
				subpath.push_back(unpack[nposi]);
				cells[nposi].tmpPathIndex = cells[nposi].pathIndex;
				cells[nposi].pathIndex = {pathKind, st + len(subpath)};
				// cells[nposi].tmpPathIndex = PathIndex(pathKind, st + len(subpath));
				pposi = posi;
				posi = nposi;
			}

			if (freeTail && st + len(subpath) < 5) {
				revertPathIndices();
				return false; // too short
			}
		}

		if (noStraight) {
			int subPathIndex = len(subpath) - (en == len(poss) - 1 ? 2 : 1);
			ivec2 pos = subPathIndex == -1 ? poss[st] : subpath[subPathIndex];
			if (pos.i == target.i || pos.j == target.j) {
				revertPathIndices();
				return false; // straight line to the target
			}
		}

		static vector<double> subtimes;
		subtimes.clear();

		// assign times to the subpath
		int sublen = len(subpath);
		rep0(i, sublen) {
			double ratio = (double) (i + 1) / (sublen + !freeTail);
			double t = lerp(stTime, enTime, ratio);
			subtimes.push_back(t);
		}

		constexpr bool RENDER_SUBPATH = false;
		if (RENDER_SUBPATH && render) {
			draw();
			mov.fill(0, 1, 0);
			mov.no_stroke();
			for (ivec2 p : subpath) {
				mov.rect(p.j + 0.3, p.i + 0.3, 0.4, 0.4);
			}
			mov.end_frame();
		}

		int diff = len(subpath) - (en - st - 1);
		static vector<tuple<int, double, bool>> obsChanges;
		obsChanges.clear();
		static fast_iset<MAX_CELLS> changed;
		changed.clear();
		rep(i, st, en + 1) {
			// remove observations made by the old path
			ivec2 pos = poss[i];
			for (auto [pos2i, time] : obss[i]) {
				cells[pos2i].observed.erase(time);
				obsChanges.emplace_back(pos2i, time, true);
				changed.insert(pos2i);
			}
		}
		// update step times
		rep(i, st + 1, en + freeTail) {
			at(poss[i]).steppedAt = -1;
		}
		// assign new step times
		rep0(i, sublen) {
			at(subpath[i]).steppedAt = subtimes[i];
		}

		auto newPath = [&](int i) {
			if (i <= st)
				return poss[i];
			if (i <= st + sublen)
				return subpath[i - (st + 1)];
			assert(!freeTail);
			return poss[i - diff];
		};
		auto newTime = [&](int i) {
			if (i <= st)
				return times[i];
			if (i <= st + sublen)
				return subtimes[i - (st + 1)];
			assert(!freeTail);
			return times[i - diff];
		};

		on_abort = [&]() {
			mov.target("error");
			draw(-1, true);
			mov.comment(show(pathi, st, en, sublen, diff));
			mov.comment(show(subpath));
			mov.end_frame();
		};

		// add observations made by the new path
		int endIndexOfPath = st + sublen + !freeTail;
		{
			ivec2 ppos = st > 0 ? poss[st - 1] : ivec2(-1, -1);
			rep(i, st, endIndexOfPath + 1) {
				auto& obs = subObss.emplace_back();
				ivec2 pos = newPath(i);
				int posi = pos.pack(n);
				double baseTime = newTime(i);
				rep0(dir, 4) {
					int pos2i = stepPos[posi << 3 | dir];
					if (pos2i != -1 && cells[pos2i].steppedAt == -1) {
						double time = baseTime;
						if (i > 0) {
							time += (unpack[pos2i] - ppos).cnorm() * 0.4 * EPS;
						}
						cells[pos2i].observed.insert(time);
						obsChanges.emplace_back(pos2i, time, false);
						obs.emplace_back(pos2i, time);
						changed.insert(pos2i);
					}
				}
				ppos = pos;
			}
		}

		static fast_iset<MAX_CELLS> toCheck;
		toCheck.clear();
		if (!simple) {
			for (int posi : changed) {
				ivec2 pos = unpack[posi];
				toCheck.insert(posi);
				rep0(dir, 8) {
					int pos2i = stepPos[posi << 3 | dir];
					if (pos2i != -1) {
						toCheck.insert(pos2i);
					}
				}
				auto& cell = cells[posi];
				if (cell.observed.empty()) {
					// if a cell lost all observations, check related closures
					for (int outer : cell.outerOf) {
						toCheck.insert(outer);
					}
				}
			}
		}

		static vector<pair<int, RoomInfo>> removedRooms;
		static vector<pair<int, RoomInfo>> addedRooms;
		removedRooms.clear();
		addedRooms.clear();

		auto posiToPathIndex = [&](int posi) -> PathIndex {
			auto& cell = cells[posi];
			if (cell.pathIndex.kind() != pathKind) {
				return cell.pathIndex;
			}
			assert(cell.steppedAt != -1);
			if (cell.steppedAt < stTime) {
				return cell.pathIndex;
			} else if (cell.steppedAt > enTime) {
				int index = cell.pathIndex.index() + diff;
				assert_ge(index, 0) << show(unpack[posi], st, en, sublen, diff, cell.pathIndex.kind(), index);
				return {cell.pathIndex.kind(), index};
			} else {
				return cell.pathIndex;
			}
		};

		if (!simple) {
			// remove related rooms
			for (int posi : toCheck) {
				auto& cell = cells[posi];
				if (cell.door) {
					removedRooms.emplace_back(posi, cell.rinfo);
					removeRoom(cell.rinfo);
				}
			}

			ivec2 semiEnd = en == len(poss) - 1 ? subpath.back() : poss[len(poss) - 2];

			// add related rooms
			for (int posi : toCheck) {
				auto& cell = cells[posi];
				ivec2 pos = unpack[posi];
				int distToEnd = min((pos - semiEnd).mnorm(), (pos - target).mnorm());
				if (distToEnd <= 1) {
					continue;
				}
				auto rinfo = computeRoomInfo(pos, posiToPathIndex);
				if (rinfo.posi != -1) {
					addedRooms.emplace_back(posi, rinfo);
					addRoom(rinfo);
				}
			}
		}

		double newScore;
		if (simple) {
			newScore = diff;
			for (auto& path : paths) {
				newScore += len(path.poss);
			}
		} else {
			newScore = computeScore(posiToPathIndex);
		}

		if (newScore < score - tol) {
			// revert observations
			for (auto [posi, time, erased] : obsChanges | views::reverse) {
				auto& cell = cells[posi];
				if (erased) {
					cell.observed.insert(time);
				} else {
					cell.observed.erase(time);
				}
			}
			// revert step times and path indices
			rep0(i, sublen) {
				auto& cell = at(subpath[i]);
				cell.steppedAt = -1;
				cell.pathIndex = cell.tmpPathIndex;
			}
			rep(i, st + 1, en + freeTail) {
				at(poss[i]).steppedAt = times[i];
			}

			// revert rooms
			for (auto [posi, rinfo] : addedRooms | views::reverse) {
				removeRoom(rinfo);
			}
			for (auto [posi, rinfo] : removedRooms | views::reverse) {
				addRoom(rinfo);
			}
			return false;
		}

		score = newScore;

		// TODO: erase times and obss; use cell[poss[i].pack(n)].steppedAt and stuff
		poss.erase(poss.begin() + st + 1, freeTail ? poss.end() : poss.begin() + en);
		poss.insert(poss.begin() + st + 1, subpath.begin(), subpath.end());
		times.erase(times.begin() + st + 1, freeTail ? times.end() : times.begin() + en);
		times.insert(times.begin() + st + 1, subtimes.begin(), subtimes.end());
		obss.erase(obss.begin() + st, freeTail ? obss.end() : obss.begin() + en + 1);
		obss.insert(obss.begin() + st, subObss.begin(), subObss.end());
		assert_eq(len(obss), len(poss));

		if (!simple) {
			rep(i, st, len(poss)) {
				at(poss[i]).pathIndex = PathIndex(pathKind, i);
			}
		}

		return true;
	}

	void extendPaths() {
		auto path12 =
			getPathBetween(paths[0].poss.back(), paths[1].poss.back(), false, false, [&](ivec2 pos) {
				int posi = pos.pack(n);
				return cells[posi].steppedAt != -1;
			});
		int firstMidIndex = -1;
		int lastMidIndex = -1;
		int entranceIndex = -1;
		rep0(i, len(path12)) {
			if (path12[i] == entrance) {
				entranceIndex = i;
			}
			int posi = path12[i].pack(n);
			int deg = 0;
			rep0(dir, 4) {
				int nposi = stepPos[posi << 3 | dir];
				if (nposi != -1 && cells[nposi].steppedAt != -1) {
					deg++;
				}
			}
			if (deg >= 3) {
				assert_eq(deg, 3);
				if (firstMidIndex == -1) {
					firstMidIndex = i;
				}
				lastMidIndex = i;
			}
		}
		if (firstMidIndex == -1) {
			assert_ne(entranceIndex, -1);
			firstMidIndex = entranceIndex;
			lastMidIndex = entranceIndex;
		}
		vector<ivec2> newPath1;
		vector<ivec2> newPath2;
		newPath1.insert(newPath1.end(), path12.begin(), path12.begin() + firstMidIndex);
		newPath2.insert(newPath2.end(), path12.begin() + lastMidIndex + 1, path12.end());
		ranges::reverse(newPath1);
		bool exclude[MAX_CELLS] = {};
		for (auto p : newPath1) {
			exclude[p.pack(n)] = true;
		}
		for (auto p : newPath2) {
			exclude[p.pack(n)] = true;
		}
		vector<ivec2> newPathBase;
		for (auto p : pathBase) {
			if (!exclude[p.pack(n)]) {
				newPathBase.push_back(p);
			}
		}
		setupPaths(newPathBase, newPath1, newPath2);
		if (render) {
			mov.target("main");
			draw(-1, true, false);
			mov.end_frame();
		}
		setupRooms();
		extended = true;
	}

	void normalizeTimes() {
		for (auto& cell : cells) {
			cell.observed.clear();
			cell.steppedAt = -1;
		}

		int l = len(pathBase);
		rep0(i, l) {
			ivec2 pos = pathBase[i];
			int posi = pos.pack(n);
			auto& cell = cells[posi];
			cell.steppedAt = -EPS;
			cell.pathIndex = {0, 0};
		}
		rep0(i, l) {
			ivec2 pos = pathBase[i];
			int posi = pos.pack(n);
			rep0(dir, 4) {
				int pos2i = stepPos[posi << 3 | dir];
				if (pos2i != -1 && cells[pos2i].steppedAt == -1) {
					cells[pos2i].observed.insert(-EPS);
				}
			}
		}
		for (auto& path : paths) {
			auto& poss = path.poss;
			auto& times = path.times;
			auto& obss = path.obss;
			assert_eq(len(poss), len(times));
			cells[poss[0].pack(n)].observed.clear();

			rep0(i, len(times)) {
				times[i] = (double) i / (len(times) - 1);
			}
			int l = len(poss);
			rep0(i, l) {
				at(poss[i]).steppedAt = times[i];
			}
			rep0(i, l) {
				obss[i].clear();
				rep0(dir, 4) {
					ivec2 pos = poss[i];
					ivec2 pos2 = pos + ivec2::dir(dir);
					int pos2i = pos2.pack_if_in_bounds(n);
					if (pos2i != -1 && cells[pos2i].steppedAt == -1) {
						double time = times[i];
						if (i > 0) {
							ivec2 ppos = poss[i - 1];
							time += (ppos - pos2).cnorm() * 0.4 * EPS;
						}
						cells[pos2i].observed.insert(time);
						obss[i].emplace_back(pos2i, time);
					}
				}
			}
		}
	}

	void optimizeRooms(rngen& rng, int tl) {
		Evaluator ev;

		// {
		// 	vector<bool> hasTree;
		// 	rep0(i, n2) {
		// 		auto& cell = cells[i];
		// 		hasTree.push_back(cell.blocked || (cell.observed.num > 0 && !cell.door));
		// 	}
		// 	trace("pre-score: ", ev.evaluate(n, entrance, target, hasTree, {}, true));
		// }

		static hash_imap<double> scoreCache;
		ull zobrist[MAX_CELLS];
		rep0(i, n2) {
			zobrist[i] = rng.next_ull();
		}

		fast_iset<MAX_CELLS> inRoom;

		struct OptimizableRoomInfo {
			int posi;
			fast_iset<MAX_CELLS> roomCells;
		};
		vector<OptimizableRoomInfo> optimizableRooms;

		for (int posi : doorPoss) {
			static fast_iset<MAX_CELLS> visited;
			visited.clear();
			static vector<int> q;
			q.clear();
			q.push_back(posi);
			visited.insert(posi);
			while (!q.empty()) {
				int posi = q.back();
				q.pop_back();
				if (cells[posi].blocked) {
					continue; // no further expansion
				}
				rep0(dir, 4) {
					int nposi = stepPos[posi << 3 | dir];
					if (nposi == -1 || visited.contains(nposi) || !cells[nposi].observed.empty() ||
						cells[nposi].steppedAt != -1)
						continue;
					visited.insert(nposi);
					q.push_back(nposi);
				}
			}
			for (int posi : visited) {
				inRoom.insert(posi);
			}
			optimizableRooms.push_back({posi, visited});
		}

		if (density > 0.06 || n > 30) {
			auto [_, deadline] = getFlowerInfo();
			fast_iset<MAX_CELLS> visited;
			fast_iset<MAX_CELLS> usedOuters;
			fast_iset<MAX_CELLS> removed;
			rep2() {
				ivec2 pos(i, j);
				int posi = pos.pack(n);
				if (visited.contains(posi))
					continue;
				if (!inRoom.contains(posi) && cells[posi].observed.empty() && cells[posi].steppedAt == -1) {
					// DFS unseen area
					static fast_iset<MAX_CELLS> area;
					static vector<int> q;
					static fast_iset<MAX_CELLS> outer;
					area.clear();
					outer.clear();
					q.push_back(posi);
					visited.insert(posi);
					bool ng = false;
					while (!q.empty()) {
						int posi = q.back();
						q.pop_back();
						area.insert(posi);
						if (cells[posi].blocked) {
							continue; // no further expansion
						}
						rep0(dir, 4) {
							int nposi = stepPos[posi << 3 | dir];
							if (nposi == -1 || inRoom.contains(nposi) || cells[nposi].steppedAt != -1)
								continue;
							if (removed.contains(nposi)) {
								ng = true;
								continue;
							}
							if (visited.contains(nposi))
								continue;
							if (!cells[nposi].observed.empty()) {
								outer.insert(nposi);
								continue;
							}
							visited.insert(nposi);
							q.push_back(nposi);
						}
					}
					trace("unseen area found: ");
					for (int posi : area) {
						trace("  ", unpack[posi]);
					}
					if (ng) {
						trace("already connected");
						continue;
					}
					bool dangerous = false;
					int bestPos = -1;
					double maxTime = -1;
					for (int posi : outer) {
						auto& cell = cells[posi];
						if (cell.blocked)
							continue;
						if (shouldBeRemoved.contains(posi)) {
							dangerous = true;
							break;
						}
						double time = cell.observed.minTime;
						if (time <= 0)
							continue;
						if (time > deadline) // TODO: check path kind?
							continue;
						if (mustNotRemove.contains(posi))
							continue;
						if (!cell.outerOf.empty())
							continue; // may destroy a room
						if (usedOuters.contains(posi)) {
							continue; // may connect to another area
						}
						// how many cell will be seen if we remove this tree?
						int numSeenFromPath = 0;
						vector<PathIndex> pathIdxs;
						rep0(dir, 4) {
							{
								int pos2i = stepPos[posi << 3 | (dir ^ 1)];
								if (pos2i == -1 || cells[pos2i].steppedAt == -1)
									continue;
								pathIdxs.push_back(cells[pos2i].pathIndex);
							}
							// path on the opposite side. step through the area
							int pos2i = posi;
							while (true) {
								pos2i = stepPos[pos2i << 3 | dir];
								if (pos2i == -1 || !area.contains(pos2i))
									break;
								numSeenFromPath++;
							}
						}
						if (numSeenFromPath == len(area)) {
							trace("nope: ", unpack[posi], " sees all ", len(area), " cells");
							continue;
						}
						if (willShortcutHappen(pathIdxs)) {
							trace("nope: ", unpack[posi], " creates a shortcut");
							continue;
						}
						if (update_max(maxTime, time)) {
							bestPos = posi;
						}
					}
					if (dangerous || bestPos == -1)
						continue;
					trace("let's remove ", unpack[bestPos]);
					removed.insert(bestPos);
					visited.insert(bestPos);
					cells[bestPos].observed.clear();
					for (int posi : outer) {
						usedOuters.insert(posi);
					}
					area.insert(bestPos); // becomes a door
					optimizableRooms.push_back({bestPos, area});
				}
			}
		}

		vector<int> sorted;
		rep0(i, len(optimizableRooms)) {
			sorted.push_back(i);
		}
		ranges::sort(sorted, [&](int a, int b) {
			return len(optimizableRooms[a].roomCells) > len(optimizableRooms[b].roomCells);
		});

		for (int rank : sorted) {
			auto& room = optimizableRooms[rank];
			int posi = room.posi;
			auto roomCells = room.roomCells;
			ivec2 mins = unpack[posi];
			ivec2 maxs = unpack[posi];
			for (int posi : roomCells) {
				ivec2 pos = unpack[posi];
				mins = mins.min(pos);
				maxs = maxs.max(pos);
			}
			trace("room aabb: ", mins, " - ", maxs, ", floors: ", len(roomCells));
			if (len(roomCells) == 1) {
				continue; // may sometimes happen with unobserved internal trees
			}
			ivec2 extent = maxs - mins + 1;
			int m = max(extent.i, extent.j);
			vector<bool> boundary(m * m, true);
			vector<bool> blocked(m * m, false);
			vector<bool> hasTree(m * m, false);
			ivec2 doorPos = unpack[posi] - mins;
			for (int posi : roomCells) {
				ivec2 pos = unpack[posi] - mins;
				int mposi = pos.pack(m);
				boundary[mposi] = false;
				if (cells[posi].blocked) {
					hasTree[mposi] = true;
					blocked[mposi] = true;
				}
			}
			trace("room data:");
			rep0(i, m) {
				string line = "  ";
				rep0(j, m) {
					if (boundary[i * m + j]) {
						line += "#";
					} else if (hasTree[i * m + j]) {
						line += "T";
					} else if (ivec2(i, j) == doorPos) {
						line += "D";
					} else {
						line += ".";
					}
				}
				trace(line);
			}
			double score = ev.evaluateRoom(m, doorPos, boundary, hasTree);
			trace("score: ", score);
			if (score == 1)
				continue;
			const int iters = timer() < tl ? 200 : 0;
			auto bestHasTree = hasTree;
			double bestScore = score;
			scoreCache.clear();
			ull hash = 0;

			rep0(iter, iters) {
				if (timer() > tl) {
					trace("alert");
					break;
				}
				double progress = (double) iter / iters;
				double temp = exp_interp(1, 0.01, progress);
				double tol = -temp * log(rng.next_float());
				int posi;
				ivec2 pos;
				do {
					pos = {rng.next_int(m), rng.next_int(m)};
					posi = pos.pack(m);
				} while (boundary[posi] || blocked[posi] || pos == doorPos);
				hasTree[posi] = !hasTree[posi];
				hash ^= zobrist[posi];
				double newScore;
				if (scoreCache.access(hash)) {
					newScore = scoreCache.get();
				} else {
					newScore = ev.evaluateRoom(m, doorPos, boundary, hasTree);
					scoreCache.set(newScore);
				}
				if (newScore < score - tol) {
					hasTree[posi] = !hasTree[posi];
					hash ^= zobrist[posi];
					continue;
				}
				score = newScore;
				if (update_max(bestScore, score)) {
					bestHasTree = hasTree;
					trace(show(iter, bestScore));
				}
			}
			rep0(i, m) {
				rep0(j, m) {
					ivec2 pos = ivec2(i, j) + mins;
					int mposi = i * m + j;
					if (bestHasTree[mposi] != blocked[mposi]) {
						assert(!blocked[mposi]);
						int posi = pos.pack(n);
						cells[posi].observed.insert(0); // add a tree
					}
				}
			}
		}
	}

	void sanityCheck() {
		for (auto& path : paths) {
			auto& poss = path.poss;
			auto pathKind = path.index + 1;
			rep(i, 0, len(poss)) {
				assert_eq(at(poss[i]).pathIndex.kind(), pathKind);
				assert_eq(at(poss[i]).pathIndex.index(), i);
			}
		}
	}

	void draw(int until = -1, bool includeInfo = false, bool noTrees = false) {
		mov.stroke_weight(0.05);

		// draw base path
		int l = len(pathBase);
		mov.no_stroke();
		mov.fill(0, 0, 1);
		rep0(i, l) {
			ivec2 pos = pathBase[i];
			mov.rect(pos.j, pos.i, 1, 1);
		}

		// draw paths
		for (auto& path : paths) {
			auto& poss = path.poss;
			int l = len(poss);
			mov.no_stroke();
			rep0(i, l) {
				ivec2 pos = poss[i];
				double ratio = (double) i / (l - 1);
				mov.fill(ratio, 0, 1 - ratio);
				mov.rect(pos.j, pos.i, 1, 1);
				if (until == i) {
					mov.fill(0);
					mov.circle(pos.j + 0.5, pos.i + 0.5, 0.3);
				}
			}
		}

		// draw trees
		mov.no_stroke();
		double untilTime = until == -1 ? INF : paths[0].times[min(len(paths[0].times) - 1, until)] + EPS;
		rep2() {
			ivec2 pos(i, j);
			bool observed = false;
			if (!noTrees) {
				auto& obs = at(pos).observed;
				rep0(k, obs.num) {
					double t = obs.times[k];
					if (until == -1 || t < untilTime) {
						observed = true;
						break;
					}
				}
			}
			if (!observed && !at(pos).blocked)
				continue;
			if (observed) {
				if (at(pos).door) {
					mov.fill(1, 0.5, 0);
				} else {
					mov.fill(0.8, 0.8, 0);
				}
			} else {
				mov.fill(0, 0.5, 0, at(pos).door ? 0.5 : 1);
			}
			if (at(pos).blocked) {
				mov.circle(j + 0.5, i + 0.5, 0.3);
			} else {
				mov.rect(j + 0.2, i + 0.2, 0.6, 0.6);
			}
		}
		mov.fill(1, 0, 1, 0.5);
		for (int posi : shouldBeObserved) {
			ivec2 pos = unpack[posi];
			mov.rect(pos.j, pos.i, 1, 1);
		}
		mov.fill(0, 0, 1, 0.5);
		for (int posi : shouldBeRemoved) {
			ivec2 pos = unpack[posi];
			mov.rect(pos.j, pos.i, 1, 1);
		}
		mov.fill(1, 0, 1, 0.5);
		for (int posi : dangerousCells) {
			ivec2 pos = unpack[posi];
			mov.rect(pos.j, pos.i, 1, 1);
		}

		// draw player and target
		mov.fill(1);
		mov.circle(target.j + 0.5, target.i + 0.5, 0.2);

		// draw grid
		mov.no_fill();
		mov.stroke(0.5);
		rep0(i, n + 1) {
			mov.line(0, i, n, i);
			mov.line(i, 0, i, n);
		}

		double roomCount = 0;
		for (int posi : doorPoss) {
			roomCount += cells[posi].rinfo.effectiveCount;
		}
		mov.comment(show(pathLens(), numRooms, roomArea, roomCount));
		mov.comment(show(score, getFlowerGoodness()));

		if (includeInfo) {
			mov.fill(0, 0);
			mov.no_stroke();
			rep2() {
				ivec2 pos(i, j);
				auto& cell = at(pos);
				int steppedAt = (int) (cell.steppedAt / EPS);
				auto& obs = cell.observed;
				static vector<int> observed;
				static vector<ivec2> outerOfs;
				observed.clear();
				outerOfs.clear();
				rep0(k, obs.num) {
					observed.push_back((int) (obs.times[k] / EPS));
				}
				for (int outer : cell.outerOf) {
					outerOfs.push_back(unpack[outer]);
				}
				auto pidx = cell.pathIndex;
				string info =
					tos(pos) + ": " + show(steppedAt, observed, outerOfs, pidx.kind(), pidx.index());
				if (cell.door) {
					info += "\nRoom area=" + tos(cell.rinfo.area) + " maxDist=" + tos(cell.rinfo.maxDist) +
						" isPerfect=" + tos(cell.rinfo.isPerfect);
				}
				mov.tooltip(info);
				mov.rect(j, i, 1, 1);
			}
			mov.no_tooltip();
		}
	}

	bool isRoom(int posi) const {
		if (cells[posi].blocked)
			return false;
		auto& obs = cells[posi].observed;
		if (obs.empty())
			return false;
		if (!cells[posi].outerOf.empty()) {
			return false; // part of another closure
		}
		double t = obs.minTime;
		auto blockedEarlier = [&](int dir) {
			int pos2i = stepPos[posi << 3 | dir];
			if (pos2i == -1)
				return true;
			auto& cell2 = cells[pos2i];
			if (cell2.steppedAt != -1)
				return false; // path is not blocked
			return cell2.observed.minTime < t;
		};
		bool bUL = blockedEarlier(4);
		bool bU = blockedEarlier(0);
		bool bUR = blockedEarlier(5);
		bool bL = blockedEarlier(2);
		bool bR = blockedEarlier(3);
		bool bDL = blockedEarlier(6);
		bool bD = blockedEarlier(1);
		bool bDR = blockedEarlier(7);
		// pattern 1 (LR/UD):
		//   ? X ?
		//   .   . ---> the center splits LR
		//   ? X ?
		// pattern 2 (UL/UR/DL/DR):
		//   X . ?
		//   .   A ---> the center splits UL
		//   ? B C
		//  where A|B|C = X

		// check pattern 1
		if (bL && bR && !bU && !bD)
			return true; // LR
		if (bU && bD && !bL && !bR)
			return true; // UD
		// check pattern 2
		if (bUL && (bD || bR || bDR) && !bU && !bL)
			return true; // UL
		if (bUR && (bD || bL || bDL) && !bU && !bR)
			return true; // UR
		if (bDL && (bU || bR || bUR) && !bD && !bL)
			return true; // DL
		if (bDR && (bU || bL || bUL) && !bD && !bR)
			return true; // DR
		return false;
	}

	bool isRoom2(int posi) const {
		auto blocked = [&](int dir) {
			int pos2i = stepPos[posi << 3 | dir];
			if (pos2i == -1)
				return true;
			auto& cell2 = cells[pos2i];
			if (cell2.steppedAt != -1)
				return false; // path is not blocked
			return !cell2.observed.empty();
		};
		bool bUL = blocked(4);
		bool bU = blocked(0);
		bool bUR = blocked(5);
		bool bL = blocked(2);
		bool bR = blocked(3);
		bool bDL = blocked(6);
		bool bD = blocked(1);
		bool bDR = blocked(7);

		// pattern 3 (forbidden):
		//   X . X
		//   .   . ---> the center splits ULR
		//   A B C
		//  where A|B|C = X

		// check pattern 3
		if (bUL && bUR && !bU && !bL && !bR && (bDL || bD || bDR))
			return false; // ULR
		if (bUL && bDL && !bL && !bU && !bD && (bUR || bR || bDR))
			return false; // LUD
		if (bDL && bDR && !bD && !bL && !bR && (bUL || bU || bUR))
			return false; // DLR
		if (bUR && bDR && !bR && !bU && !bD && (bUL || bL || bDL))
			return false; // RUD
		return true;
	}

	template <typename F>
	RoomInfo computeRoomInfo(ivec2 pos, F posiToPathIndex) {
		int posi = pos.pack(n);
		if (!isRoom(posi) || !isRoom2(posi)) {
			return {};
		}
		// possibly a door
		RoomInfo res;
		auto& center = cells[posi];
		double refTime = center.observed.minTime;

		static fast_iset<MAX_CELLS> visited;
		static fast_iset<MAX_CELLS> outers;
		static fast_iset<MAX_CELLS> seenNewTrees;
		visited.clear();
		outers.clear();
		seenNewTrees.clear();

		static queue<ivec2> q;
		while (!q.empty())
			q.pop();
		int dist = 0;
		int area = 0;
		int pathPosi = -1;
		ivec2 mins = pos;
		ivec2 maxs = pos;

		bool isPerfect = true;
		bool isUD = false;
		rep0(dir, 4) {
			ivec2 npos = pos + ivec2::dir(dir);
			int nposi = npos.pack_if_in_bounds(n);
			if (nposi == -1)
				continue;
			auto& cell = cells[nposi];
			if (cell.steppedAt != -1) {
				pathPosi = max(pathPosi, nposi);
				if (dir < 2)
					isUD = true;
			}
			if (!cell.observed.empty()) {
				double t = cell.observed.minTime;
				if (t > refTime) {
					isPerfect = false; // some adjacent tree is seen later
				}
			}
		}
		bool mayGlance = !isPerfect && isUD;
		auto pathIndex = posiToPathIndex(pathPosi);

		// add initial cells
		q.push(pos);
		visited.insert(posi);
		rep0(dir, 4) {
			ivec2 d = ivec2::dir(dir);
			ivec2 npos = pos - d;
			int nposi = npos.pack_if_in_bounds(n);
			if (nposi == -1)
				continue;
			auto& cell = cells[nposi];
			if (cell.steppedAt == -1 && !mayGlance)
				continue;

			// can see inside in this direction

			ivec2 npos2 = pos + d;
			int npos2i = npos2.pack_if_in_bounds(n);
			if (npos2i == -1)
				continue;
			if (!cells[npos2i].observed.empty() || cells[npos2i].steppedAt != -1) {
				// not inside
				continue;
			}
			while (true) {
				assert_eq(cells[npos2i].steppedAt, -1);
				q.push(npos2);
				visited.insert(npos2i);
				if (cells[npos2i].blocked) {
					// hit an unobserved tree; the tree itself can be a part
					// of the area (unlike seen trees), but blocks further expansion
					seenNewTrees.insert(npos2i);
					break;
				}

				// move until hit a tree
				npos2 += d;
				npos2i = npos2.pack_if_in_bounds(n);
				if (npos2i == -1 || !cells[npos2i].observed.empty())
					break;
			}
		}
		assert_ne(pathPosi, -1);

		// on_abort = [&]() {
		// 	mov.target("error");
		// 	draw(-1, true);
		// 	mov.comment(show(pos));
		// 	mov.end_frame();
		// };

		int centerPosi = posi;
		int numUnobservedTrees = 0;

		while (true) {
			int qs = len(q);
			rep0(iter, qs) {
				ivec2 pos = q.front();
				q.pop();
				mins = mins.min(pos);
				maxs = maxs.max(pos);
				int posi = pos.pack(n);
				auto& cell = cells[posi];
				if (dist > 0) {
					area++; // only count non-initial cells
					if (area > 16) {
						// too large
						return {};
					}
				}
				if (posi != centerPosi) {
					assert_eq(cell.observed.num, 0);
				}
				if (cell.blocked) {
					// no further expansion
					continue;
				}
				rep0(dir, 4) {
					ivec2 npos = pos + ivec2::dir(dir);
					int nposi = npos.pack_if_in_bounds(n);
					if (nposi == -1 || visited.contains(nposi))
						continue;
					if (cells[nposi].steppedAt != -1) {
						assert_eq(dist, 0); // can only happen at the first step
						continue; // path
					}
					if (!cells[nposi].observed.empty()) {
						outers.insert(nposi);

						// check path kind consistency
						bool ok = false;
						rep0(dir2, 4) {
							int npos2i = stepPos[nposi << 3 | dir2];
							if (npos2i == -1 || cells[npos2i].steppedAt == -1)
								continue;
							auto pidx = posiToPathIndex(npos2i);
							if (pidx.kind() == pathIndex.kind() || pidx.kind() == 0) {
								ok = true;
							}
						}
						if (!ok) {
							// draw(-1, true);
							// mov.comment(show(pos, npos, pathIndex.kind()));
							// mov.end_frame();
							return {}; // inconsistent path kinds
						}

						continue; // seen tree
					}
					while (true) {
						// move until hit a tree
						visited.insert(nposi);
						q.push(npos);
						if (cells[nposi].blocked) {
							// hit an unobserved tree; the tree itself can be a part
							// of the area (unlike seen trees), but blocks further expansion
							break;
						}
						npos += ivec2::dir(dir);
						nposi = npos.pack_if_in_bounds(n);
						if (nposi == -1 || visited.contains(nposi) || !cells[nposi].observed.empty())
							break;
					}
				}
			}
			if (q.empty())
				break;
			dist++;
		}

		if (dist == 0) {
			// the entire area can be seen from the path. not a closure
			return {};
		}

		res.posi = posi;
		res.pathPosi = pathPosi;
		res.maxDist = dist;
		res.area = area;
		res.outers = vector<int>(outers.begin(), outers.end());
		rep0(i, dist) {
			res.effectiveCount += 1.0 / (i + 1);
		}
		res.effectiveCount += area * 0.1;
		res.isPerfect = isPerfect;

		return res;
	}

private:
	Problem p;

	bool willShortcutHappen(vector<PathIndex>& pathIdxs) const {
		ranges::sort(pathIdxs, [](auto a, auto b) {
			return a.data < b.data;
		});
		bool ng = false;
		rep0(i, len(pathIdxs) - 1) {
			if (pathIdxs[i + 1].data > pathIdxs[i].data + 2) {
				ng = true;
				break;
			}
		}
		return ng;
	}

	vector<ivec2> getPathBetween(ivec2 from, ivec2 to, bool avoidTarget, bool stickToBoundary,
		function<bool(ivec2)> canPass = nullptr) const {
		assert(!at(from).blocked);
		assert(!at(to).blocked);
		int total = n2;
		int fromi = from.pack(n);
		int toi = to.pack(n);
		vector<int> prev(total, -1);

		if (stickToBoundary) {
			// dijkstra
			vector<double> dists(total, INF);
			priority_queue<pdi, vector<pdi>, greater<pdi>> pq;
			pq.push({0, fromi});
			dists[fromi] = 0;
			prev[fromi] = fromi;
			while (!pq.empty()) {
				auto [dist, posi] = pq.top();
				pq.pop();
				if (dist > dists[posi])
					continue;
				if (posi == toi)
					break;
				rep0(dir, 4) {
					int nposi = stepPos[posi << 3 | dir];
					if (nposi == -1)
						continue;
					if (prev[nposi] != -1)
						continue;
					if (canPass) {
						if (!canPass(unpack[nposi]))
							continue;
					} else {
						if (cells[nposi].blocked || !cells[nposi].observed.empty() ||
							cells[nposi].steppedAt != -1)
							continue;
					}
					double ddist = 1;
					ivec2 npos = unpack[nposi];
					if (npos.i == 0 || npos.i == n - 1 || npos.j == 0 || npos.j == n - 1) {
						ddist = 0.1;
					}
					double ndist = dist + ddist;
					if (ndist < dists[nposi]) {
						dists[nposi] = ndist;
						prev[nposi] = posi;
						pq.push({ndist, nposi});
					}
				}
			}
		} else {
			queue<int> q;

			prev[fromi] = fromi;
			q.push(fromi);

			while (!q.empty()) {
				int posi = q.front();
				q.pop();
				if (posi == toi)
					break;
				array<int, 4> dirs;
				if (abs(unpack[posi].i - to.i) > abs(unpack[posi].j - to.j)) {
					dirs = {0, 1, 2, 3};
				} else {
					dirs = {2, 3, 0, 1};
				}
				rep0(diri, 4) {
					int nposi = stepPos[posi << 3 | dirs[diri]];
					if (nposi == -1)
						continue;
					if (prev[nposi] != -1)
						continue;
					if (canPass) {
						if (!canPass(unpack[nposi]))
							continue;
					} else {
						if (cells[nposi].blocked || !cells[nposi].observed.empty() ||
							cells[nposi].steppedAt != -1)
							continue;
					}
					prev[nposi] = posi;
					q.push(nposi);
				}
			}
		}

		if (prev[toi] == -1)
			return {};
		vector<ivec2> path;
		for (int posi = toi;; posi = prev[posi]) {
			ivec2 pos = unpack[posi];
			if (avoidTarget && (pos - target).mnorm() <= 1) {
				return {};
			}
			path.push_back(pos);
			if (posi == fromi)
				break;
		}
		reverse(path.begin(), path.end());
		return path;
	}

	template <typename F>
	vector<ivec2> getPathBetween(
		ivec2 from, F isGoal, bool avoidTarget, function<bool(ivec2)> canPass = nullptr) const {
		int total = n2;
		vector<int> prev(total, -1);
		queue<int> q;

		int fromi = from.pack(n);
		int goali = -1;
		prev[fromi] = fromi;
		q.push(fromi);

		while (!q.empty()) {
			int posi = q.front();
			q.pop();
			if (isGoal(unpack[posi])) {
				goali = posi;
				break;
			}
			rep0(dir, 4) {
				int nposi = stepPos[posi << 3 | dir];
				if (nposi == -1)
					continue;
				if (prev[nposi] != -1)
					continue;
				if (canPass) {
					if (!canPass(unpack[nposi]))
						continue;
				} else {
					if (cells[nposi].blocked || !cells[nposi].observed.empty() ||
						cells[nposi].steppedAt != -1)
						continue;
				}
				prev[nposi] = posi;
				q.push(nposi);
			}
		}

		if (goali == -1 || prev[goali] == -1)
			return {};
		vector<ivec2> path;
		for (int posi = goali;; posi = prev[posi]) {
			ivec2 pos = unpack[posi];
			if (avoidTarget && (pos - target).mnorm() <= 1) {
				return {};
			}
			path.push_back(pos);
			if (posi == fromi)
				break;
		}
		reverse(path.begin(), path.end());
		return path;
	}

	void computePathDists() {
		int numPaths = len(paths);
		pathDists.resize(numPaths * numPaths);
		rep(i, 1, numPaths) {
			rep0(j, i) {
				ivec2 from = paths[i].poss[0];
				ivec2 to = paths[j].poss[0];
				int total = n2;
				vector<int> dists(total, -1);
				queue<int> q;
				int fromi = from.pack(n);
				int toi = to.pack(n);
				dists[fromi] = 0;
				q.push(fromi);
				while (!q.empty()) {
					int posi = q.front();
					q.pop();
					if (posi == toi)
						break;
					rep0(dir, 4) {
						int nposi = stepPos[posi << 3 | dir];
						if (nposi == -1)
							continue;
						if (dists[nposi] != -1)
							continue;
						if (cells[nposi].steppedAt != -EPS && nposi != toi)
							continue;
						dists[nposi] = dists[posi] + 1;
						q.push(nposi);
					}
				}
				assert_ne(dists[toi], -1);
				int dist = dists[toi];
				pathDists[i * numPaths + j] = dist;
				pathDists[j * numPaths + i] = dist;
			}
		}
		trace(show(pathDists));
	}

	vector<ivec2> adjustPath(const vector<ivec2>& basePath, vector<ivec2> path, ivec2 target) {
		int end = len(path);
		auto tmpCells = cells;
		while (end > 0) {
			cells = tmpCells;

			// to avoid going back to the base path
			for (auto pos : basePath) {
				int posi = pos.pack(n);
				cells[posi].steppedAt = -EPS;
				rep0(dir, 4) {
					int pos2i = stepPos[posi << 3 | dir];
					if (pos2i != -1 && cells[pos2i].steppedAt == -1) {
						cells[pos2i].steppedAt = -EPS;
					}
				}
			}
			cells[path[0].pack(n)].steppedAt = -1;

			rep0(i, end) {
				int posi = path[i].pack(n);
				cells[posi].steppedAt = -EPS;
			}
			rep0(i, end - 1) {
				int posi = path[i].pack(n);
				rep0(dir, 4) {
					int pos2i = stepPos[posi << 3 | dir];
					if (pos2i != -1 && cells[pos2i].steppedAt == -1) {
						cells[pos2i].observed.insert(-EPS);
					}
				}
			}
			auto newPath = getPathBetween(path[end - 1], target, false, false);
			if (!newPath.empty()) {
				path.erase(path.begin() + end - 1, path.end());
				path.insert(path.end(), newPath.begin(), newPath.end());
				cells = tmpCells;
				return path;
			}
			end--;
		}
		cells = tmpCells;
		return {};
	}

	void setupPaths(const vector<ivec2>& pathb, const vector<ivec2>& path1, const vector<ivec2>& path2) {
		initRot(0);
		pathBase = pathb;
		for (auto pos : pathBase) {
			auto& cell = cells[pos.pack(n)];
			cell.steppedAt = -EPS;
			cell.pathIndex = {0, 0};
		}
		for (auto pos : pathBase) {
			int posi = pos.pack(n);
			auto& cell = cells[posi];
			rep0(dir, 4) {
				int pos2i = stepPos[posi << 3 | dir];
				if (pos2i != -1 && cells[pos2i].steppedAt == -1) {
					cells[pos2i].observed.insert(-EPS);
				}
			}
		}
		paths.clear();
		rep0(pathi, 2) {
			auto& path = paths.emplace_back();
			path.index = pathi;
			auto& poss = path.poss;
			auto& times = path.times;
			auto& obss = path.obss;
			poss = (pathi == 0) == (path1.back() == target) ? path1 : path2;
			assert(!poss.empty());
			cells[poss[0].pack(n)].observed.clear();
			int l = len(poss);
			times.clear();
			rep0(i, l) {
				double ratio = (double) i / (l - 1);
				times.push_back(ratio);
			}
			obss.resize(l);
			rep0(i, l) {
				auto& cell = cells[poss[i].pack(n)];
				cell.steppedAt = times[i];
				cell.pathIndex = {pathi + 1, i};
			}
			rep0(i, l) {
				ivec2 pos = poss[i];
				rep0(dir, 4) {
					ivec2 pos2 = pos + ivec2::dir(dir);
					int pos2i = pos2.pack_if_in_bounds(n);
					if (pos2i != -1 && cells[pos2i].steppedAt == -1) {
						double time = times[i];
						if (i > 0) {
							ivec2 ppos = poss[i - 1];
							time += (ppos - pos2).cnorm() * 0.4 * EPS;
						}
						cells[pos2i].observed.insert(time);
						obss[i].emplace_back(pos2i, time);
					}
				}
			}
		}
	}

	void setupRooms() {
		auto posiToPathIndex = [&](int posi) {
			return cells[posi].pathIndex;
		};
		rep2() {
			ivec2 pos(i, j);
			auto cinfo = computeRoomInfo(pos, posiToPathIndex);
			if (cinfo.posi != -1) {
				addRoom(cinfo);
			}
		}
		computePathDists();
		score = computeScore(posiToPathIndex);
		sanityCheck();
	}

	bool findPathBase(const Problem& p) {
		struct Move {
			char data;

			constexpr Move(char data = 0) : data(data) {
			}

			constexpr Move(int dj1, int dj2) : data((dj1 & 0xf) << 4 | (dj2 & 0xf)) {
				assert_in(dj1, -8, 7);
				assert_in(dj2, -8, 7);
			}

			int dj1() const {
				int res = (data >> 4) & 0xf;
				if (res >= 8)
					res -= 16;
				return res;
			}

			int dj2() const {
				int res = data & 0xf;
				if (res >= 8)
					res -= 16;
				return res;
			}

			pii djs() const {
				return {dj1(), dj2()};
			}

			// comparators
			bool operator<(const Move& other) const {
				return data < other.data;
			}
			bool operator>(const Move& other) const {
				return data > other.data;
			}
			bool operator==(const Move& other) const {
				return data == other.data;
			}
			bool operator!=(const Move& other) const {
				return data != other.data;
			}
		};
		struct State {
			int i = 0;
			int j1 = 0;
			int j2 = 0;
			int area1 = 0;
			int area2 = 0;
			int distSum = 0;
			int penalty = 0;

			// histories
			easy_stack<int> hj1;
			easy_stack<int> hj2;
			easy_stack<int> harea1;
			easy_stack<int> harea2;
			easy_stack<int> hdistSum;
			easy_stack<int> hpenalty;
			easy_stack<Move> moves;

			void clear() {
				i = 0;
				j1 = 0;
				j2 = 0;
				area1 = 0;
				area2 = 0;
				distSum = 0;
				penalty = 0;
				hj1.clear();
				hj2.clear();
				harea1.clear();
				harea2.clear();
				hdistSum.clear();
				hpenalty.clear();
				moves.clear();
				moves.push_back({0, 0});
			}
		};
		beam_search<State, double, Move, {-8, -8}, double, less<>> bs;
		vector<array<vector<ivec2>, 3>> cands; // base, path1, path2
		State st;
		double maxAreaDiffPenaltyCoeff = linearmap(0.1, 0.5, 0, 1.0 / 6, density);
		auto getScore = [&](State& st) {
			double areaDiffPenaltyCoeff = linearmap(0.05, maxAreaDiffPenaltyCoeff, n * 0.5, n - 4, st.i);
			return st.distSum + abs(st.area1 - st.area2) * areaDiffPenaltyCoeff +
				(st.area1 + st.area2) * 0.2 + st.penalty * linearmap(0.8, 0.2, n * 0.3, n - 4, st.i);
		};
		auto applyMove = [&](State& st, Move move) {
			st.hj1.push_back(st.j1);
			st.hj2.push_back(st.j2);
			st.harea1.push_back(st.area1);
			st.harea2.push_back(st.area2);
			st.hdistSum.push_back(st.distSum);
			st.hpenalty.push_back(st.penalty);
			st.moves.push_back(move);
			st.i++;
			auto [dj1, dj2] = move.djs();
			int nj1 = st.j1 + dj1;
			int nj2 = st.j2 + dj2;
			st.area1 += min(st.j1, nj1);
			st.area2 += n - 1 - max(st.j2, nj2);
			int dist = min(nj2, st.j2) - max(nj1, st.j1);
			st.distSum += dist * dist - 1;
			st.j1 = nj1;
			st.j2 = nj2;
			st.penalty += abs(dj1 * dj1 - 1) + abs(dj2 * dj2 - 1);
		};
		auto undoMove = [&](State& st) {
			st.i--;
			st.j1 = st.hj1.pop_back();
			st.j2 = st.hj2.pop_back();
			st.area2 = st.harea2.pop_back();
			st.area1 = st.harea1.pop_back();
			st.moves.pop_back();
			st.penalty = st.hpenalty.pop_back();
			st.distSum = st.hdistSum.pop_back();
		};
		for (int rot : {0, -1, 1}) {
			initRot(rot);
			bool passedTop[MAX_N] = {};
			rep0(i, 2) {
				rep0(j, n) {
					if (i > 0 && passedTop[j])
						continue;
					ivec2 fork(i, j);
					int forki = fork.pack(n);
					if (cells[forki].blocked)
						continue;
					if ((target - fork).mnorm() < 5)
						continue;
					auto pathToFork = getPathBetween(entrance, fork, true, true);
					if (pathToFork.empty())
						continue;
					int lefti = stepPos[forki << 3 | 2];
					int righti = stepPos[forki << 3 | 3];
					if (lefti == -1 || righti == -1)
						continue;
					if (cells[lefti].blocked || cells[righti].blocked)
						continue;
					passedTop[j] = true;
					ivec2 left = unpack[lefti];
					ivec2 right = unpack[righti];
					st.clear();
					st.i = fork.i;
					st.j1 = left.j;
					st.j2 = right.j;
					auto moves = bs.run_tree(
						st, getScore, applyMove, undoMove,
						[&](State& st) {
							if (st.i == n - 1)
								return;
							int a1;
							int b1;
							int a2;
							int b2;
							{
								int a = st.j1;
								int b = st.j1 - st.moves.back().dj1();
								a1 = min(a, b);
								b1 = max(a, b);
							}
							{
								int a = st.j2;
								int b = st.j2 - st.moves.back().dj2();
								a2 = min(a, b);
								b2 = max(a, b);
							}
							auto isOk = [&](int j) {
								return j >= 0 && j < n && (j < a1 || j > b1) && (j < a2 || j > b2) &&
									!at({st.i, j}).blocked && (target - ivec2(st.i, j)).mnorm() > 1;
							};
							const int moveLimit = 3;
							int minDj1 = 0;
							int maxDj1 = 0;
							while (isOk(st.j1 + minDj1 - 1) && minDj1 > -moveLimit)
								minDj1--;
							while (isOk(st.j1 + maxDj1 + 1) && maxDj1 < moveLimit)
								maxDj1++;
							int minDj2 = 0;
							int maxDj2 = 0;
							while (isOk(st.j2 + minDj2 - 1) && minDj2 > -moveLimit)
								minDj2--;
							while (isOk(st.j2 + maxDj2 + 1) && maxDj2 < moveLimit)
								maxDj2++;
							int j1 = st.j1;
							int j2 = st.j2;
							rep(dj1, minDj1, maxDj1 + 1) {
								rep(dj2, minDj2, maxDj2 + 1) {
									int dist = min(j2, j2 + dj2) - max(j1, j1 + dj1);
									int nj1 = j1 + dj1;
									int nj2 = j2 + dj2;
									if (dist < 2)
										continue;
									if (st.i == target.i - 2) {
										if (nj1 == target.j || nj2 == target.j)
											continue; // avoid touching the target
									}
									if (st.i == target.i - 1) {
										if (nj1 < target.j && nj2 > target.j)
											continue; // avoid sandwiching the target
										if (abs(nj1 - target.j) <= 1 || abs(nj2 - target.j) <= 1)
											continue; // avoid touching the target
									}
									if (at({st.i + 1, nj1}).blocked || at({st.i + 1, nj2}).blocked)
										continue;
									Move move = {dj1, dj2};
									applyMove(st, move);
									bs.add_candidate(
										move, getScore(st), j1 << 24 | j2 << 16 | st.j1 << 8 | st.j2);
									undoMove(st);
								}
							}
						},
						[&](int turn) {
							int width = 100;
							auto& cands = bs.candidates_to_filter();
							auto dir = [&](auto a, auto b) {
								return a.data < b.data;
							};
							int size1 = len(cands);
							bs.remove_duplicates(dir);
							int size2 = len(cands);
							if (len(cands) > width) {
								nth_element(cands.begin(), cands.begin() + width, cands.end(), dir);
								cands.resize(width);
							}
							int size3 = len(cands);
							// trace(show(turn, size1, size2, size3));
						});
					// trace("moves: ", moves);
					if (moves.empty())
						continue;
					ivec2 pos1 = left;
					ivec2 pos2 = right;
					vector<ivec2> path1;
					vector<ivec2> path2;
					path1.push_back(pos1);
					path2.push_back(pos2);
					int targetSide = 0; // -1: left, 1: right
					rep0(i, len(moves)) {
						auto [dj1, dj2] = moves[i].djs();
						int nj1 = pos1.j + dj1;
						while (pos1.j != nj1) {
							pos1.j += (pos1.j < nj1 ? 1 : -1);
							path1.push_back(pos1);
						}
						pos1.i++;
						path1.push_back(pos1);
						int nj2 = pos2.j + dj2;
						while (pos2.j != nj2) {
							pos2.j += (pos2.j < nj2 ? 1 : -1);
							path2.push_back(pos2);
						}
						pos2.i++;
						path2.push_back(pos2);
						if (targetSide == 0 && pos1.i >= target.i) {
							if (nj1 < target.j) {
								targetSide = 1;
							} else if (nj2 > target.j) {
								targetSide = -1;
							} else {
								fail();
							}
						}
					}

					// if (render) {
					// 	mov.target("init");
					// 	paths.clear();
					// 	paths.push_back(Path{0, left, path1,
					// 		lambda::map(path1,
					// 			[](auto a) {
					// 				return 0.0;
					// 			}),
					// 		{}});
					// 	paths.push_back(Path{1, right, path2,
					// 		lambda::map(path2,
					// 			[](auto a) {
					// 				return 0.0;
					// 			}),
					// 		{}});
					// 	draw(-1, true);
					// 	mov.comment(show(path1));
					// 	mov.comment(show(path2));
					// 	mov.end_frame();
					// 	paths.clear();
					// }

					if (len(moves) < n - 2 - fork.i) {
						// didn't reach the bottom
						// mov.comment("didn't reach the bottom");
						continue;
					}

					if (targetSide == 0) {
						// mov.comment("didn't decide the target side");
						continue;
					}

					{
						bool used[MAX_CELLS] = {};
						for (auto pos : pathToFork) {
							used[pos.pack(n)] = true;
						}
						for (auto pos : path1) {
							used[pos.pack(n)] = true;
						}
						for (auto pos : path2) {
							used[pos.pack(n)] = true;
						}
						int nextToTargetCount = 0;
						rep0(dir, 4) {
							int posi = stepPos[target.pack(n) << 3 | dir];
							if (posi == -1)
								continue;
							rep0(dir2, 4) {
								int pos2i = stepPos[posi << 3 | dir2];
								if (pos2i != -1 && used[pos2i]) {
									nextToTargetCount++;
									break;
								}
							}
						}
						if (nextToTargetCount >= 3) {
							// mov.comment("too close to the target");
							continue; // too close to the target
						}
					}

					array<ivec2, 2> targets;

					ivec2 dummyTarget = {n * 2 / 3, targetSide == -1 ? n - 1 : 0};
					while (cells[dummyTarget.pack(n)].blocked) {
						dummyTarget.j += targetSide;
					}

					if (targetSide == -1) {
						targets = {target, dummyTarget};
					} else {
						targets = {dummyTarget, target};
					}

					path1 = adjustPath(pathToFork, path1, targets[0]);
					path2 = adjustPath(pathToFork, path2, targets[1]);

					if (path1.empty() || path2.empty()) {
						// mov.comment("failed to adjust paths");
						continue;
					}

					int touchBottomAt1 = -1;
					int touchBottomAt2 = -1;
					rep0(i, len(path1)) {
						if (path1[i].i >= n - 2) {
							touchBottomAt1 = i;
							break;
						}
					}
					rep0(i, len(path2)) {
						if (path2[i].i >= n - 2) {
							touchBottomAt2 = i;
							break;
						}
					}
					if (touchBottomAt1 == -1 || touchBottomAt2 == -1) {
						// mov.comment("didn't touch the bottom");
						continue;
					}
					touchBottomAt1 = min(touchBottomAt1 + 1, len(path1) - 1);
					touchBottomAt2 = min(touchBottomAt2 + 1, len(path2) - 1);

					set<int> baseCells;
					for (auto pos : pathToFork) {
						baseCells.insert(pos.pack(n));
					}
					rep0(i, touchBottomAt1) {
						baseCells.insert(path1[i].pack(n));
					}
					rep0(i, touchBottomAt2) {
						baseCells.insert(path2[i].pack(n));
					}
					path1.erase(path1.begin(), path1.begin() + touchBottomAt1);
					path2.erase(path2.begin(), path2.begin() + touchBottomAt2);

					if (len(path1) < 4 || len(path2) < 4) {
						// mov.comment("path too short");
						continue;
					}

					bool ng = false;
					for (auto pos : path1) {
						if (ranges::count(pathToFork, pos)) {
							ng = true;
							break;
						}
					}
					for (auto pos : path2) {
						if (ranges::count(pathToFork, pos)) {
							ng = true;
							break;
						}
					}
					if (ng) {
						// mov.comment("path intersects");
						continue;
					}

					// cleanup baseCells; exclude cells that are not needed to reach path1/path2
					{
						auto path = getPathBetween(path1[0], path2[0], false, false, [&](ivec2 pos) {
							if (pos == path1[0] || pos == path2[0])
								return true;
							return baseCells.contains(pos.pack(n));
						});
						bool inPath[MAX_CELLS] = {};
						for (auto pos : path) {
							inPath[pos.pack(n)] = true;
						}
						auto pathToFork = getPathBetween(
							entrance,
							[&](ivec2 pos) {
								return inPath[pos.pack(n)];
							},
							false,
							[&](ivec2 pos) {
								return baseCells.contains(pos.pack(n));
							});
						for (auto pos : pathToFork) {
							inPath[pos.pack(n)] = true;
						}
						rep0(i, n2) {
							if (!inPath[i]) {
								baseCells.erase(i);
							}
						}
					}

					vector<ivec2> pathb;
					for (int posi : baseCells) {
						pathb.push_back(unpack[posi]);
					}

					for (auto& pos : pathb) {
						pos = rotPos(pos, -rot);
					}
					for (auto& pos : path1) {
						pos = rotPos(pos, -rot);
					}
					for (auto& pos : path2) {
						pos = rotPos(pos, -rot);
					}

					cands.push_back({pathb, path1, path2});
				}
			}
		}

		if (cands.empty()) {
			initRot(0);
			return false;
		}

		array<vector<ivec2>, 3> bestCand;
		double bestScore = 0;

		for (auto& [pathb, path1, path2] : cands) {
			vector<bool> used(n2, false);
			for (auto pos : pathb) {
				int posi = pos.pack(n);
				used[posi] = true;
				rep0(dir, 4) {
					int pos2i = stepPos[posi << 3 | dir];
					if (pos2i != -1) {
						used[pos2i] = true;
					}
				}
			}
			int area1 = 0;
			int area2 = 0;
			for (ivec2 start : {path1.back(), path2.back()}) {
				int starti = start.pack(n);
				static vector<int> q;
				q.clear();
				q.push_back(starti);
				used[starti] = true;
				int& area = start == path1.back() ? area1 : area2;
				while (!q.empty()) {
					int posi = q.back();
					q.pop_back();
					area++;
					rep0(dir, 4) {
						int nposi = stepPos[posi << 3 | dir];
						if (nposi == -1 || used[nposi])
							continue;
						used[nposi] = true;
						q.push_back(nposi);
					}
				}
			}
			double areaScore = min(area1, area2) * 2 + max(area1, area2) * 0.5;
			double nearTargetPenalty = 0;
			int distToTarget = INF;
			for (auto pos : pathb) {
				update_min(distToTarget, (pos - target).mnorm());
			}
			nearTargetPenalty = linearmap(0.8, 1.0, 2, 5, distToTarget);

			setupPaths(pathb, path1, path2);

			int numChanged = 0;
			double progress = 0;
			int iters = 2000;
			int iterCount = 0;
			int noHit = 0;
			double best = 0;
			score = 0;
			rngen rng(12345);
			rep0(t, iters) {
				iterCount++;
				if ((t & 0xff) == 0) {
					progress = (double) t / iters;
					if (progress >= 1)
						break;
				}
				double temp = exp_interp(20, 1, progress);
				int pathi = rng.next_int(2);
				auto& path = paths[pathi];
				int l = min(len(path.poss) - 2, rng.next_int(2, 8));
				int st = rng.next_int(len(path.poss) - l);
				int en = st + l;
				double tol = -temp * log(rng.next_float());

				if (reconnect(pathi, st, en, tol, rng, true)) {
					noHit = 0;
					numChanged++;
					if (numChanged % 100 == 0) {
						normalizeTimes();
					}
					update_max(best, score);
				}
			}
			auto lens = pathLens();
			assert_eq(len(lens), 2);
			double pathScore = min(lens[0], lens[1]) * 2 + max(lens[0], lens[1]) * 0.5;
			double totalScore = (areaScore + pathScore * 1.5) * nearTargetPenalty;
			if (update_max(bestScore, totalScore)) {
				bestCand = {pathb, path1, path2};
			}

			if (render) {
				mov.target("init");
				draw();
				mov.comment(show(path1));
				mov.comment(show(path2));
				mov.comment(show(area1, area2, areaScore, pathScore, totalScore));
				mov.end_frame();
				pathBase.clear();
				paths.clear();
			}
		}
		setupPaths(bestCand[0], bestCand[1], bestCand[2]);
		if (render) {
			mov.target("init");
			draw(-1, true);
			mov.end_frame();
		}
		score = 0;
		return true;
	}

	void initPath(const Problem& p, rngen& rng) {
		if (!findPathBase(p)) {
			// use a trivial one
			pathBase.clear();
			paths.clear();
			auto toTarget = getPathBetween(entrance, target, false, false);
			auto& path = paths.emplace_back();
			path.index = 0;
			auto& poss = path.poss;
			auto& times = path.times;
			auto& obss = path.obss;
			poss = toTarget;
			assert(!poss.empty());
			cells[poss[0].pack(n)].observed.clear();
			int l = len(poss);
			times.clear();
			rep0(i, l) {
				double ratio = (double) i / (l - 1);
				times.push_back(ratio);
			}
			obss.resize(l);
			rep0(i, l) {
				auto& cell = cells[poss[i].pack(n)];
				cell.steppedAt = times[i];
				cell.pathIndex = {1, i};
			}
			rep0(i, l) {
				int posi = poss[i].pack(n);
				auto& cell = cells[posi];
				rep0(dir, 4) {
					int pos2i = stepPos[posi << 3 | dir];
					if (pos2i != -1 && cells[pos2i].steppedAt == -1) {
						cells[pos2i].observed.insert(times[i]);
						obss[i].emplace_back(pos2i, times[i]);
					}
				}
			}
		}
		setupRooms();
		if (render) {
			mov.target("init");
			draw(-1, true);
			mov.end_frame();
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

	Field f;
	int n = 0;
	int n2 = 0;
	vector<int> targetPriority;

	Solver() : rng(123456) {
	}

	void load(istream& in, int seed = -1) {
		p.load(in);
		n = p.n;
		n2 = n * n;
		if (isLocal) {
			rep0(i, n2 - 1) {
				ivec2 target;
				in >> target.i >> target.j;
				targetPriority.push_back(target.pack(n));
			}
		}
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
	void init() {
		f.init(p, rng);
	}

	void solveMain(ostream& out) {
		if (render) {
			mov.target("main");
		}
		double best = f.score;
		Field bestF = f;

		const int iters = INF;
		constexpr int RENDER_INTERVAL = 100000;
		constexpr int TL1 = 1800;
		constexpr int TL2 = 1950;

		// const int iters = 500000;
		// constexpr int RENDER_INTERVAL = iters / 100;
		// constexpr int TL1 = INF;
		// constexpr int TL2 = INF;

		int numChanged = 0;

		double progress = 0;
		int iterCount = 0;
		int noHit = 0;
		rep0(t, iters) {
			iterCount++;
			if ((t & 0xff) == 0) {
				if (TL1 == INF) {
					progress = (double) t / iters;
				} else {
					progress = (double) timer() / TL1;
				}
				if (progress >= 1)
					break;
			}
			double temp = exp_interp(500, 10, progress);
			int pathi = rng.next_int(len(f.paths));
			auto& path = f.paths[pathi];
			int l = min(len(path.poss) - 1, (int) round(lerp(2, 16, pow(rng.next_float(), 1.4))));
			int st = rng.next_int(len(path.poss) - l);
			int en = st + l;
			double tol = -temp * log(rng.next_float());

			if (len(f.paths) == 2 && progress > 0.5 && (t & 0xff) == 0 && !f.extended &&
				f.getFlowerGoodness() < linearmap(0.05, 1, 0.5, 0.9, progress)) {
				trace("extending paths at ", show(progress, iterCount));
				f.extendPaths();
			}

			if (f.reconnect(pathi, st, en, tol, rng)) {
				noHit = 0;
				numChanged++;
				if (numChanged % 100 == 0) {
					f.normalizeTimes();
				}
				if (update_max(best, f.score)) {
					if (progress > 0.01) {
						trace("new best: ", show(best, t, progress, temp));
					}
					bestF = f;
				}
			} else {
				noHit++;
				if (noHit >= 2000) {
					noHit = 0;
					f = bestF;
					trace("reset to best");
				}
			}

			if (render) {
				if (t % RENDER_INTERVAL == 0) {
					f.draw(-1, true, false);
					mov.comment(show(t, temp, f.pathLens(), best));
					mov.end_frame();
				}
			}
		}
		f = bestF;
		trace("final: ", show(best, f.pathLens(), iterCount));

		if (render) {
			mov.target("main");
			f.draw(-1, false, false);
			mov.end_frame();
		}

		f.filterTrees();
		f.optimizeRooms(rng, TL2);

		if (render) {
			mov.target("main");
			f.draw(-1, true, false);
			mov.end_frame();
		}

		if (render) {
			mov.target("final");
			int l = len(f.paths[0].poss);
			rep0(t, l) {
				f.draw(t, t == l - 1);
				mov.comment(show(f.paths[0].poss[t], t, l));
				mov.end_frame();
			}
		}

		vector<bool> hasTree;
		rep0(i, n2) {
			auto& cell = f.cells[i];
			hasTree.push_back(cell.blocked || (cell.observed.num > 0 && !cell.door));
		}

		Evaluator ev;

		vector<int> trees;
		rep2() {
			ivec2 pos(i, j);
			auto& cell = f.at(pos);
			if (!cell.blocked && hasTree[pos.pack(n)]) {
				trees.push_back(pos.i);
				trees.push_back(pos.j);
			}
		}
		out << len(trees) / 2 << " " << lambda::join(trees, " ") << endl;
		out << -1 << endl;

		if (isLocal) {
			double est = ev.evaluate(n, p.entrance, p.target, hasTree, targetPriority, true);
			result.score = (int) est;
		}
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
	oss << "seed n d" << endl;
	rep0(seed, 1000) {
		Solver sol;
		sol.load(seed);
		int numTrees = 0;
		int n = sol.n;
		rep2() {
			numTrees += sol.p.bs[i][j];
		}
		double density = (double) numTrees / (n * n);
		double d = (int) round(density * 100);
		oss << seed << " " << sol.n << " " << d << endl;
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
	EasyTester<Solver> tester("max");
	int num = 10;
	int from = 40;
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
