#ifdef _MSC_VER
inline unsigned int clz(unsigned int x)
{
    unsigned long r;
    _BitScanReverse(&r, x);
    return 31 - r;
}
inline unsigned long long clzll(unsigned long long x)
{
    unsigned long r;
    _BitScanReverse64(&r, x);
    return 63 - r;
}
#else
#define clz __builtin_clz
#define clzll __builtin_clzll
#endif // _MSC_VER
// C
#ifndef _GLIBCXX_NO_ASSERT
#include <cassert>
#endif
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#if __cplusplus >= 201103L
#include <ccomplex>
#include <cfenv>
#include <cinttypes>
#include <cstdalign>
#include <cstdbool>
#include <cstdint>
#include <ctgmath>
#include <cuchar>
#include <cwchar>
#include <cwctype>
#endif

// C++
#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>

#include <array>
#include <atomic>
#include <chrono>
#include <codecvt>
#include <condition_variable>
#include <forward_list>
#include <future>
#include <initializer_list>
#include <mutex>
#include <random>
#include <ratio>
#include <regex>
#include <scoped_allocator>
#include <system_error>
#include <thread>
#include <tuple>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

#if __cplusplus >= 201402L
#include <shared_mutex>
#endif

#if __cplusplus >= 201703L
#include <charconv>
#endif

using namespace std;
using namespace std::chrono;
using ll = long long;
using ull = unsigned long long;
using pll = pair<long long, long long>;
using tlll = tuple<long long, long long, long long>;
using tllll = tuple<long long, long long, long long, long long>;
std::mt19937 mt(time(0)); //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
std::uniform_int_distribution<ll> toobig(0, 1e9);
string argnum;

class xor_shift_128
{
public:
    typedef uint32_t result_type;
    xor_shift_128(result_type seed = mt())
    {
        set_seed(seed);
    }
    void set_seed(result_type seed)
    {
        a = seed = 1812433253 * (seed ^ (seed >> 30));
        b = seed = 1812433253 * (seed ^ (seed >> 30)) + 1;
        c = seed = 1812433253 * (seed ^ (seed >> 30)) + 2;
        d = seed = 1812433253 * (seed ^ (seed >> 30)) + 3;
    }
    result_type operator()()
    {
        result_type t = (a ^ (a << 11));
        a = b;
        b = c;
        c = d;
        return d = (d ^ (d >> 19)) ^ (t ^ (t >> 8));
    }
    static constexpr result_type max() { return numeric_limits<result_type>::max(); }
    static constexpr result_type min() { return numeric_limits<result_type>::min(); }

private:
    result_type a, b, c, d;
};
xor_shift_128 xorrand;
ifstream in;
ofstream out;

namespace radix_heap
{
    namespace internal
    {
        template <bool Is64bit>
        class find_bucket_impl;

        template <>
        class find_bucket_impl<false>
        {
        public:
            static inline constexpr size_t find_bucket(uint32_t x, uint32_t last)
            {
                return x == last ? 0 : 32 - clz(x ^ last);
            }
        };

        template <>
        class find_bucket_impl<true>
        {
        public:
            static inline constexpr size_t find_bucket(uint64_t x, uint64_t last)
            {
                return x == last ? 0 : 64 - clzll(x ^ last);
            }
        };

        template <typename T>
        inline constexpr size_t find_bucket(T x, T last)
        {
            return find_bucket_impl<sizeof(T) == 8>::find_bucket(x, last);
        }

        template <typename KeyType, bool IsSigned>
        class encoder_impl_integer;

        template <typename KeyType>
        class encoder_impl_integer<KeyType, false>
        {
        public:
            typedef KeyType key_type;
            typedef KeyType unsigned_key_type;

            inline static constexpr unsigned_key_type encode(key_type x)
            {
                return x;
            }

            inline static constexpr key_type decode(unsigned_key_type x)
            {
                return x;
            }
        };

        template <typename KeyType>
        class encoder_impl_integer<KeyType, true>
        {
        public:
            typedef KeyType key_type;
            typedef typename std::make_unsigned<KeyType>::type unsigned_key_type;

            inline static constexpr unsigned_key_type encode(key_type x)
            {
                return static_cast<unsigned_key_type>(x) ^
                       (unsigned_key_type(1) << unsigned_key_type(std::numeric_limits<unsigned_key_type>::digits - 1));
            }

            inline static constexpr key_type decode(unsigned_key_type x)
            {
                return static_cast<key_type>(x ^ (unsigned_key_type(1) << (std::numeric_limits<unsigned_key_type>::digits - 1)));
            }
        };

        template <typename KeyType, typename UnsignedKeyType>
        class encoder_impl_decimal
        {
        public:
            typedef KeyType key_type;
            typedef UnsignedKeyType unsigned_key_type;

            inline static constexpr unsigned_key_type encode(key_type x)
            {
                return raw_cast<key_type, unsigned_key_type>(x) ^
                       ((-(raw_cast<key_type, unsigned_key_type>(x) >> (std::numeric_limits<unsigned_key_type>::digits - 1))) |
                        (unsigned_key_type(1) << (std::numeric_limits<unsigned_key_type>::digits - 1)));
            }

            inline static constexpr key_type decode(unsigned_key_type x)
            {
                return raw_cast<unsigned_key_type, key_type>(x ^ (((x >> (std::numeric_limits<unsigned_key_type>::digits - 1)) - 1) |
                                                                  (unsigned_key_type(1) << (std::numeric_limits<unsigned_key_type>::digits - 1))));
            }

        private:
            template <typename T, typename U>
            union raw_cast
            {
            public:
                constexpr raw_cast(T t) : t_(t) {}
                operator U() const { return u_; }

            private:
                T t_;
                U u_;
            };
        };

        template <typename KeyType>
        class encoder : public encoder_impl_integer<KeyType, std::is_signed<KeyType>::value>
        {
        };
        template <>
        class encoder<float> : public encoder_impl_decimal<float, uint32_t>
        {
        };
        template <>
        class encoder<double> : public encoder_impl_decimal<double, uint64_t>
        {
        };
    } // namespace internal

    template <typename KeyType, typename EncoderType = internal::encoder<KeyType>>
    class radix_heap
    {
    public:
        typedef KeyType key_type;
        typedef EncoderType encoder_type;
        typedef typename encoder_type::unsigned_key_type unsigned_key_type;

        radix_heap() : size_(0), last_(), buckets_()
        {
            buckets_min_.fill(std::numeric_limits<unsigned_key_type>::max());
        }

        void push(key_type key)
        {
            const unsigned_key_type x = encoder_type::encode(key);
            assert(last_ <= x);
            ++size_;
            const size_t k = internal::find_bucket(x, last_);
            buckets_[k].emplace_back(x);
            buckets_min_[k] = std::min(buckets_min_[k], x);
        }

        key_type top()
        {
            pull();
            return encoder_type::decode(last_);
        }

        void pop()
        {
            pull();
            buckets_[0].pop_back();
            --size_;
        }

        size_t size() const
        {
            return size_;
        }

        bool empty() const
        {
            return size_ == 0;
        }

        void clear()
        {
            size_ = 0;
            last_ = key_type();
            for (auto &b : buckets_)
                b.clear();
            buckets_min_.fill(std::numeric_limits<unsigned_key_type>::max());
        }

        void swap(radix_heap<KeyType, EncoderType> &a)
        {
            std::swap(size_, a.size_);
            std::swap(last_, a.last_);
            buckets_.swap(a.buckets_);
            buckets_min_.swap(a.buckets_min_);
        }

    private:
        size_t size_;
        unsigned_key_type last_;
        std::array<std::vector<unsigned_key_type>,
                   std::numeric_limits<unsigned_key_type>::digits + 1>
            buckets_;
        std::array<unsigned_key_type,
                   std::numeric_limits<unsigned_key_type>::digits + 1>
            buckets_min_;

        void pull()
        {
            assert(size_ > 0);
            if (!buckets_[0].empty())
                return;

            size_t i;
            for (i = 1; buckets_[i].empty(); ++i)
                ;
            last_ = buckets_min_[i];

            for (unsigned_key_type x : buckets_[i])
            {
                const size_t k = internal::find_bucket(x, last_);
                buckets_[k].emplace_back(x);
                buckets_min_[k] = std::min(buckets_min_[k], x);
            }
            buckets_[i].clear();
            buckets_min_[i] = std::numeric_limits<unsigned_key_type>::max();
        }
    };

    template <typename KeyType, typename ValueType, typename EncoderType = internal::encoder<KeyType>>
    class pair_radix_heap
    {
    public:
        typedef KeyType key_type;
        typedef ValueType value_type;
        typedef EncoderType encoder_type;
        typedef typename encoder_type::unsigned_key_type unsigned_key_type;

        pair_radix_heap() : size_(0), last_(), buckets_()
        {
            buckets_min_.fill(std::numeric_limits<unsigned_key_type>::max());
        }

        void push(key_type key, const value_type &value)
        {
            const unsigned_key_type x = encoder_type::encode(key);
            assert(last_ <= x);
            ++size_;
            const size_t k = internal::find_bucket(x, last_);
            buckets_[k].emplace_back(x, value);
            buckets_min_[k] = std::min(buckets_min_[k], x);
        }

        void push(key_type key, value_type &&value)
        {
            const unsigned_key_type x = encoder_type::encode(key);
            assert(last_ <= x);
            ++size_;
            const size_t k = internal::find_bucket(x, last_);
            buckets_[k].emplace_back(x, std::move(value));
            buckets_min_[k] = std::min(buckets_min_[k], x);
        }

        template <class... Args>
        void emplace(key_type key, Args &&...args)
        {
            const unsigned_key_type x = encoder_type::encode(key);
            assert(last_ <= x);
            ++size_;
            const size_t k = internal::find_bucket(x, last_);
            buckets_[k].emplace_back(std::piecewise_construct,
                                     std::forward_as_tuple(x), std::forward_as_tuple(args...));
            buckets_min_[k] = std::min(buckets_min_[k], x);
        }

        key_type top_key()
        {
            pull();
            return encoder_type::decode(last_);
        }

        value_type &top_value()
        {
            pull();
            return buckets_[0].back().second;
        }

        void pop()
        {
            pull();
            buckets_[0].pop_back();
            --size_;
        }

        size_t size() const
        {
            return size_;
        }

        bool empty() const
        {
            return size_ == 0;
        }

        void clear()
        {
            size_ = 0;
            last_ = key_type();
            for (auto &b : buckets_)
                b.clear();
            buckets_min_.fill(std::numeric_limits<unsigned_key_type>::max());
        }

        void swap(pair_radix_heap<KeyType, ValueType, EncoderType> &a)
        {
            std::swap(size_, a.size_);
            std::swap(last_, a.last_);
            buckets_.swap(a.buckets_);
            buckets_min_.swap(a.buckets_min_);
        }

    private:
        size_t size_;
        unsigned_key_type last_;
        std::array<std::vector<std::pair<unsigned_key_type, value_type>>,
                   std::numeric_limits<unsigned_key_type>::digits + 1>
            buckets_;
        std::array<unsigned_key_type,
                   std::numeric_limits<unsigned_key_type>::digits + 1>
            buckets_min_;

        void pull()
        {
            assert(size_ > 0);
            if (!buckets_[0].empty())
                return;

            size_t i;
            for (i = 1; buckets_[i].empty(); ++i)
                ;
            last_ = buckets_min_[i];

            for (size_t j = 0; j < buckets_[i].size(); ++j)
            {
                const unsigned_key_type x = buckets_[i][j].first;
                const size_t k = internal::find_bucket(x, last_);
                buckets_[k].emplace_back(std::move(buckets_[i][j]));
                buckets_min_[k] = std::min(buckets_min_[k], x);
            }
            buckets_[i].clear();
            buckets_min_[i] = std::numeric_limits<unsigned_key_type>::max();
        }
    };
} // namespace radix_heap

constexpr int deka = (1 << 16) - 1;
int fullopemax = 10000;
int breaksiz = 50;

class solver
{
public:
    ll score = 0;
    ll siz = 0;
    vector<vector<int>> g;
    vector<vector<char>> adj;
    vector<ll> dirty;

    deque<int> fullope;
    vector<deque<int>> cellope;

    int nowfro = 0;

    map<ll, ll> ltotal, lsuc;

    deque<ll> froera;
    ll nowfroera;

    radix_heap::pair_radix_heap<int, int> rh;
    vector<ll> sankaku;

    ll dirtysum = 0, frobacsum = 0, sankakufrobacsum = 0;

    vector<double> logv;

    vector<int> per;

    vector<deque<int>::iterator> altfind;

    vector<int> gsiz;
    void init()
    {
        cin >> siz;
        g = vector<vector<int>>(siz * siz);
        adj = vector<vector<char>>(siz * siz, vector<char>(siz * siz, 0));

        for (int i = 0; i < siz - 1; i++)
        {
            for (int j = 0; j < siz; j++)
            {
                char moji;
                cin >> moji;

                if (moji == '0')
                {
                    g[yxtoi(i, j)].push_back(yxtoi(i + 1, j));
                    g[yxtoi(i + 1, j)].push_back(yxtoi(i, j));

                    adj[yxtoi(i, j)][yxtoi(i + 1, j)] = 1;
                    adj[yxtoi(i + 1, j)][yxtoi(i, j)] = 1;
                }
            }
        }

        for (int i = 0; i < siz; i++)
        {
            for (int j = 0; j < siz - 1; j++)
            {
                char moji;
                cin >> moji;

                if (moji == '0')
                {
                    g[yxtoi(i, j)].push_back(yxtoi(i, j + 1));
                    g[yxtoi(i, j + 1)].push_back(yxtoi(i, j));

                    adj[yxtoi(i, j)][yxtoi(i, j + 1)] = 1;
                    adj[yxtoi(i, j + 1)][yxtoi(i, j)] = 1;
                }
            }
        }

        dirty = vector<ll>(siz * siz);

        for (int i = 0; i < siz; i++)
        {
            for (int j = 0; j < siz; j++)
            {
                cin >> dirty[yxtoi(i, j)];
            }
        }

        dirtysum = accumulate(dirty.begin(), dirty.end(), 0LL);

        vector<char> dou(siz * siz, 0);
        stack<int> st;
        st.push(0);

        fullope.clear();
        cellope = vector<deque<int>>(siz * siz);

        nowfro = 0;
        while (st.size())
        {
            int t = st.top();

            cellope[t].emplace_back(fullope.size() + nowfro);
            fullope.emplace_back(t);

            dou.at(t) = true;
            vector<int> per(g[t].size());
            for (int i = 0; i < per.size(); i++)
            {
                per.at(i) = i;
            }
            for (int i = 0; i < per.size(); i++)
            {
                // swap(per.at(i), per.at(i + xorrand() % (g.at(t).size() - i)));
                int x = g[t].at(per[i]);
                if (!dou[x])
                {
                    st.push(x);
                    break;
                }
            }

            if (st.top() == t)
            {
                st.pop();
            }
        }

        cellope[0].pop_back();
        fullope.pop_back();

        sankaku = vector<ll>(fullopemax + 21, 0);
        for (int i = 0; i < fullopemax + 20; i++)
        {
            sankaku[i + 1] = sankaku[i] + i + 1;
        }

        gsiz = vector<int>(siz * siz);
        for (int i = 0; i < siz * siz; i++)
        {
            gsiz[i] = g[i].size();
            rh.push(cellope[i].back(), i);
            for (int j = 0; j < cellope[i].size() - 1; j++)
            {
                int diff = cellope[i][j + 1] - cellope[i][j] - 1;
                score += sankaku[diff] * dirty[i];
            }
            int diff = cellope.at(i).front() + fullope.size() - cellope.at(i).back() - 1;
            score += sankaku[diff] * dirty.at(i);

            /*for (auto x : cellope.at(i))
            {
                cout << x << ' ';
            }
            cout << endl;*/
        }

        logv = vector<double>(deka);
        for (int i = 0; i < deka; i++)
        {
            logv[i] = log((i + 1.0) / deka);
        }
        calc_nanika();

        per = {1, -1, (int)siz, -(int)siz};

        gshuffle();

        altfind = vector<deque<int>::iterator>(siz * siz);
    }
    int yxtoi(int y, int x)
    {
        return y * siz + x;
    }
    void modify(double temp)
    {
        ll scorediff = 0;

        int bac = fullope.back();

        ll prob = 0;

        int d = 0;
        ll fbs = frobacsum, sfbs = sankakufrobacsum, ds = dirtysum;
        ll addfbs = 0, addsfbs = 0, addds = 0;
        ll t = nowfro;
        ll fullopesiz = 0;
        int tgt = 0;
        for (int k = 0; k < gsiz[bac]; ++k)
        {
            // swap(g[bac][k], g[bac][k + xorrand() % (g[bac].size() - k)]);
            tgt = g[bac][k];
            if (cellope[tgt][0] == nowfro)
            {
                if (cellope[tgt].size() == 1)
                    continue;
                tgt = cellope[tgt][1];
            }
            else
            {
                tgt = cellope[tgt][0];
            }
            if (tgt - nowfro > breaksiz || rh.top_key() < tgt)
                continue;

            scorediff = 0;
            prob = logv[xorrand() & deka] * temp + score * (tgt - nowfro);

            fbs = frobacsum, sfbs = sankakufrobacsum, ds = dirtysum;
            addfbs = 0, addsfbs = 0, addds = 0;
            t = nowfro;
            fullopesiz = fullope.size();

            scorediff = 0;

            for (auto &x : fullope)
            {
                if (t == cellope[x].front())
                {
                    altfind[x] = cellope[x].begin();
                    d = cellope[x].front() + fullopesiz - cellope[x].back() - 1;
                    scorediff -= dirty[x] * sankaku[d];
                    sfbs -= dirty[x] * sankaku[d];
                    fbs -= dirty[x] * d;
                    ds -= dirty[x];
                    addds += dirty[x];
                }
                if (t != cellope[x].back())
                {
                    ++altfind[x];
                    if (*altfind[x] >= tgt)
                    {
                        d = *altfind[x] + fullopesiz - cellope[x].back() - tgt + nowfro - 1;
                        scorediff += sankaku[d] * dirty[x];
                        addsfbs += sankaku[d] * dirty[x];
                        addfbs += d * dirty[x];
                    }
                    scorediff -= sankaku[*altfind[x] - cellope[x][altfind[x] - cellope[x].begin() - 1] - 1] * dirty[x];
                }
                t++;
                if (t >= tgt)
                    break;
            }

            scorediff -= sfbs;
            sfbs -= fbs * (tgt - nowfro) - ds * sankaku[tgt - nowfro - 1];
            fbs -= ds * (tgt - nowfro);
            scorediff += sfbs;

            if (prob < -scorediff * fullopesiz)
            {
                // cout << prob << ' ' << temp << endl;
                // cout << scorediff << ' ' << score * (ll)(fullope.size() - tgt + nowfro) - (score + scorediff) * (ll)fullope.size() << endl;

                score += scorediff;

                while (cellope[fullope.front()].front() != tgt)
                {
                    cellope[fullope.front()].pop_front();
                    fullope.pop_front();
                }
                nowfro = tgt;

                frobacsum = fbs + addfbs;
                sankakufrobacsum = sfbs + addsfbs;
                dirtysum = ds + addds;

                // sita = true;
                // break;
                // k=-1;
            }
        }

        if (fullope.size() <= fullopemax)
        {
            ll adddiff = 0;
            int addcell = 0;
            for (int k = 0; k < gsiz[bac]; ++k)
            {
                scorediff = dirty[bac];
                // swap(g[bac][k], g[bac][k + xorrand() % (g[bac].size() - k)]);
                addcell = g[bac][k];

                sfbs = sankakufrobacsum, fbs = frobacsum, ds = dirtysum;
                fullopesiz = fullope.size();

                prob = logv[xorrand() & deka] * temp - score * 2;

                adddiff = (cellope[addcell].front() - cellope[addcell].back() + fullope.size() - 1);
                scorediff -= sankaku[adddiff] * dirty[addcell];

                adddiff = (cellope[addcell].front() - nowfro + 1);
                scorediff += sankaku[adddiff] * dirty[addcell];
                adddiff = (nowfro + fullope.size() - cellope[addcell].back() - 1);
                scorediff += sankaku[adddiff] * dirty[addcell];

                d = (cellope[bac].front() - cellope[bac].back() + fullope.size() - 1);
                ds -= dirty[bac];
                fbs -= dirty[bac] * d;
                sfbs -= dirty[bac] * sankaku[d];

                d = (cellope[addcell].front() - cellope[addcell].back() + fullope.size() - 1);
                ds -= dirty[addcell];
                fbs -= dirty[addcell] * d;
                sfbs -= dirty[addcell] * sankaku[d];

                scorediff += (fbs + ds) * 2 + ds;

                // cerr << scorediff << endl;
                if (prob < -scorediff * fullopesiz)
                {
                    score += scorediff;

                    cellope[bac].emplace_front(nowfro - 1);
                    fullope.emplace_front(bac);

                    cellope[addcell].emplace_front(nowfro - 2);
                    fullope.emplace_front(addcell);

                    nowfro -= 2;

                    sfbs += fbs * 2 + ds * 3;
                    fbs += ds * 2;

                    d = cellope[bac].front() - cellope[bac].back() + fullope.size() - 1;
                    ds += dirty[bac];
                    fbs += dirty[bac] * d;
                    sfbs += dirty[bac] * sankaku[d];

                    d = cellope[addcell].front() - cellope[addcell].back() + fullope.size() - 1;
                    ds += dirty[addcell];
                    fbs += dirty[addcell] * d;
                    sfbs += dirty[addcell] * sankaku[d];

                    dirtysum = ds;
                    frobacsum = fbs;
                    sankakufrobacsum = sfbs;

                    break;
                }
            }

            int adda = 0, addb = 0;
            for (int k = 0; k < 4; ++k)
            {
                scorediff = 0;
                // swap(per[k], per[k + xorrand() % (4 - k)]);
                adda = fullope.back() + per[k], addb = fullope.front() + per[k];
                if (adda < 0 || addb < 0 || adda >= siz * siz || addb >= siz * siz)
                    continue;
                bool ok = true;
                ok &= adj[fullope.back()][adda];
                ok &= adj[fullope.front()][addb];
                ok &= adj[addb][adda];

                if (!ok)
                    continue;

                prob = logv[xorrand() & deka] * temp - score * 2;

                scorediff = 0;
                sfbs = sankakufrobacsum, fbs = frobacsum, ds = dirtysum;

                adddiff = (cellope[adda].front() - cellope[adda].back() + fullope.size() - 1);
                scorediff -= sankaku[adddiff] * dirty[adda];

                adddiff = (cellope[addb].front() - cellope[addb].back() + fullope.size() - 1);
                scorediff -= sankaku[adddiff] * dirty[addb];

                adddiff = (cellope[adda].front() - nowfro + 1);
                scorediff += sankaku[adddiff] * dirty[adda];
                adddiff = (nowfro + fullope.size() - cellope[adda].back() - 1);
                scorediff += sankaku[adddiff] * dirty[adda];

                adddiff = (cellope[addb].front() - nowfro);
                scorediff += sankaku[adddiff] * dirty[addb];
                adddiff = (nowfro + fullope.size() - cellope[addb].back());
                scorediff += sankaku[adddiff] * dirty[addb];

                d = (cellope[adda].front() - cellope[adda].back() + fullope.size() - 1);

                ds -= dirty[adda];
                fbs -= dirty[adda] * d;
                sfbs -= dirty[adda] * sankaku[d];

                d = (cellope[addb].front() - cellope[addb].back() + fullope.size() - 1);

                ds -= dirty[addb];
                fbs -= dirty[addb] * d;
                sfbs -= dirty[addb] * sankaku[d];

                scorediff += (fbs + ds) * 2 + ds;

                // cerr << scorediff << endl;
                if (prob < -scorediff * fullopesiz)
                {
                    score += scorediff;

                    cellope[addb].emplace_front(nowfro - 1);
                    fullope.emplace_front(addb);

                    cellope[adda].emplace_front(nowfro - 2);
                    fullope.emplace_front(adda);

                    nowfro -= 2;

                    sfbs += fbs * 2 + ds * 3;
                    fbs += ds * 2;

                    d = cellope[adda].front() - cellope[adda].back() + fullope.size() - 1;
                    ds += dirty[adda];
                    fbs += dirty[adda] * d;
                    sfbs += dirty[adda] * sankaku[d];

                    d = cellope[addb].front() - cellope[addb].back() + fullope.size() - 1;
                    ds += dirty[addb];
                    fbs += dirty[addb] * d;
                    sfbs += dirty[addb] * sankaku[d];

                    dirtysum = ds;
                    frobacsum = fbs;
                    sankakufrobacsum = sfbs;

                    break;
                }
            }
        }

        rh.push(nowfro + fullope.size(), fullope.front());

        d = (cellope[fullope.front()].front() - cellope[fullope.front()].back() + fullope.size() - 1);
        frobacsum -= d * dirty[fullope.front()];
        sankakufrobacsum -= sankaku[d] * dirty[fullope.front()];

        cellope[fullope.front()].emplace_back(nowfro + fullope.size());
        cellope[fullope.front()].pop_front();
        nowfro++;

        d = (cellope[fullope.front()].front() - cellope[fullope.front()].back() + fullope.size() - 1);
        frobacsum += d * dirty[fullope.front()];
        sankakufrobacsum += sankaku[d] * dirty[fullope.front()];

        fullope.emplace_back(fullope.front());
        fullope.pop_front();

        while (cellope[rh.top_value()].back() != rh.top_key())
        {
            rh.pop();
        }

        assert(nowfro == cellope[fullope.front()].front());
    }
    void calc_nanika()
    {
        frobacsum = 0;
        sankakufrobacsum = 0;
        ll d = 0;
        for (int i = 0; i < siz * siz; i++)
        {
            d = (cellope[i].front() - cellope[i].back() + fullope.size() - 1);
            frobacsum += d * dirty[i];
            sankakufrobacsum += sankaku[d] * dirty[i];
        }
    }
    void gshuffle()
    {
        for (int i = 0; i < siz * siz; i++)
        {
            for (int j = 0; j < g[i].size(); j++)
            {
                swap(g[i][j], g[i][j + xorrand() % (g[i].size() - j)]);
            }
        }
        for (int i = 0; i < 4; i++)
        {
            swap(per[i], per[xorrand() % (4 - i)]);
        }
    }
    void answer()
    {
        ll zen = 0, suc = 0;
        for (auto &x : ltotal)
        {
            // cout << x.first << ' ' << x.second << ' ' << lsuc[x.first] << endl;
            zen += x.second;
            suc += lsuc[x.first];
        }
        // cerr << zen << ' ' << suc << endl;
        while (fullope.front() != 0)
        {
            fullope.emplace_back(fullope.front());
            fullope.pop_front();
        }
        for (int i = 0; i < fullope.size(); i++)
        {
            ll diff = fullope[i] - fullope[(i + 1) % fullope.size()];
            if (diff == siz)
            {
                cout << 'U';
            }
            if (diff == -siz)
            {
                cout << 'D';
            }
            if (diff == 1)
            {
                cout << 'L';
            }
            if (diff == -1)
            {
                cout << 'R';
            }
        }
        cout << endl;
    }
};
vector<double> starts = {5.43, 5.99, 7.01, 6.53, 6.76, 7.09, 7.11};
vector<double> owas = {3.33, 3.22, 4.76, 4.33, 3.54, 5.14, 5.29};
vector<double> pows = {0.16, 0.16, 0.79, 0.28, 0.2, 0.55, 0.69};
int main(int argc, char **argv)
{
    ios::sync_with_stdio(false);
    cin.tie(0);
    long long a = 0, b = 0, c = 0, d = 0, e = 0, f = 0, m = 0, n = 0, p = 0, q = 0, mod = 998244353;
    long double pi = 3.1415926535, ad = 0, bd = 0, cd = 0, dd = 0;
    string s1, s2;
    char moji, moja;

    double starttemp = 7.25, endtemp = 4.75, temppow = 1.0;
    fullopemax = 10000;
    breaksiz = 50;
    if (argc > 2)
    {
        in.open(argv[1]);
        cin.rdbuf(in.rdbuf());
        out.open(argv[2]);
        cout.rdbuf(out.rdbuf());

        if (argc > 5)
        {
            starttemp = stod(argv[3]);
            endtemp = stod(argv[4]);
            temppow = stod(argv[5]);
        }
    }

    ll timelimit = 1980;

    solver sol;
    sol.init();

    b = (sol.siz - 20) / 3;
    starttemp = starts[b];
    endtemp = owas[b];
    temppow = pows[b];

    a = accumulate(sol.dirty.begin(), sol.dirty.end(), 0LL);
    ll bestscore = 1e12;
    deque<int> bestope;

    double nowtemp = starttemp;

    auto starttime = chrono::system_clock::now();

    for (int turn = 0; 1; turn++)
    {
        if ((turn & 31) == 31)
        {
            ll timediff = duration_cast<milliseconds>(chrono::system_clock::now() - starttime).count();
            if (timediff > timelimit)
            {
                n = turn;
                break;
            }
            nowtemp = endtemp - ((endtemp - starttemp) * pow(timelimit - timediff, temppow)) / pow(timelimit, temppow);
            nowtemp = pow(10, nowtemp) * a;
            if ((turn & 2047) == 2047)
                sol.gshuffle();
        }
        sol.modify(nowtemp);
        if (sol.score * bestope.size() < bestscore * sol.fullope.size())
        {
            bestscore = sol.score;
            bestope = sol.fullope;
        }
    }

    sol.score = bestscore;
    sol.fullope = bestope;
    sol.answer();
    cerr << n << endl;
    cerr << "Score = " << sol.score / sol.fullope.size() << endl;
    return 0;
}