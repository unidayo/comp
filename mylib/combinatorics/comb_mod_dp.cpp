// 最終更新: 2026-07-12 15:27:39 JST
// Pascalの三角形を使って 0 <= n <= max_n の二項係数を前計算する。
//
// 法なし:
//   CombinationDP<long long> comb(60);
//   cout << comb(60, 30) << '\n';
//
// 法あり:
//   CombinationDP<long long> comb(2000, 1'000'000'007LL);
//   cout << comb(2000, 1000) << '\n';
//
// 任意精度整数やmodintを T に指定する場合は法なし形式を使う。
//   CombinationDP<boost::multiprecision::cpp_int> exact(1000);
//   CombinationDP<mint> mod_comb(2000);
//
// 構築時間・メモリは O(max_n^2)。法なしの組み込み整数型では、
// 真の二項係数が T の範囲を超えるとオーバーフローする。
template <class T = long long>
class CombinationDP {
  public:
    explicit CombinationDP(int max_n) : _max_n(max_n) {
        assert(max_n >= 0);
        build();
    }

    CombinationDP(int max_n, const T &modulus) : _max_n(max_n), _modulus(modulus) {
        assert(max_n >= 0);
        assert(modulus > T{});
        build();
    }

    int max_n() const { return _max_n; }
    bool has_modulus() const { return _modulus.has_value(); }

    // k < 0 または n < k なら0を返す。n は前計算範囲内でなければならない。
    const T &operator()(int n, int k) const {
        assert(0 <= n && n <= _max_n);
        if (k < 0 || n < k) return zero();
        return _table[n][k];
    }

    const vector<vector<T>> &table() const { return _table; }

  private:
    int _max_n;
    optional<T> _modulus;
    vector<vector<T>> _table;

    static const T &zero() {
        static const T value{};
        return value;
    }

    T add(const T &a, const T &b) const {
        if (!_modulus) return a + b;

        if constexpr (is_integral_v<T> && is_signed_v<T>) {
            return (T)(((__int128_t)a + b) % *_modulus);
        } else if constexpr (is_integral_v<T> && is_unsigned_v<T>) {
            return (T)(((__uint128_t)a + b) % *_modulus);
        } else {
            return (a + b) % *_modulus;
        }
    }

    void build() {
        _table.resize((size_t)_max_n + 1);
        _table[0] = {T{1}};
        for (int n = 1; n <= _max_n; ++n) {
            _table[n].resize((size_t)n + 1);
            _table[n][0] = T{1};
            _table[n][n] = T{1};
            for (int k = 1; k < n; ++k) {
                _table[n][k] = add(_table[n - 1][k - 1], _table[n - 1][k]);
            }
        }
    }
};
