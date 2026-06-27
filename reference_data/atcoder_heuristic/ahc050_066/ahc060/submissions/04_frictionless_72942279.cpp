// Common.hpp

#if defined(_WIN32)
#define PLATFORM_WINDOWS 1
#if defined(_WIN64)
#define ARCH_64 1
#else
#define ARCH_64 0
#endif
#else
#define PLATFORM_WINDOWS 0
#if defined(__x86_64__)
#define ARCH_64 1
#elif defined(__i386__)
#define ARCH_64 0
#else
#error "Can't determine if architecture is 32-bit or 64-bit"
#endif
#endif

#include <stddef.h>
#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#if PLATFORM_WINDOWS
#define PRIuZ "Iu"
#define PRIoZ "Io"
#define PRIxZ "Ix"
#define PRIXZ "IX"
#define PRIdT "Id"
#define PRIiT "Ii"
#define PRIoT "Io"
#define PRIxT "Ix"
#define PRIXT "IX"
#else
#define PRIuZ "zu"
#define PRIoZ "zo"
#define PRIxZ "zx"
#define PRIXZ "zX"
#define PRIdT "td"
#define PRIiT "ti"
#define PRIoT "to"
#define PRIxT "tx"
#define PRIXT "tX"
#endif

#include <string>
#include <vector>
#include <set>
#include <map>
#include <functional>
#include <memory>
#include <utility>
#include <algorithm>

// Xoshiro.hpp

#include <random>

template<typename T, size_t N, typename Derived>
class XoshiroBase
{
public:
    typedef T result_type;

    static constexpr T min() { return 0; }
    static constexpr T max() { return -1; }

    void seed()
    {
        std::seed_seq seq;
        seed(seq);
    }

private:
    void seedHelper(uint32_t value)
    {
        std::seed_seq seq{value};
        seed(seq);
    }

    void seedHelper(uint64_t value)
    {
        std::seed_seq seq{(uint32_t)value, (uint32_t)(value >> 32)};
        seed(seq);
    }

public:
    void seed(T value)
    {
        seedHelper(value);
    }

    template<class Sseq>
    void seed(Sseq& seq)
    {
        constexpr size_t M = sizeof(T) / sizeof(uint32_t);
        uint32_t seeds[N * M];
        seq.generate(seeds, seeds + N * M);
        for (size_t i = 0; i < N; i++)
        {
            T si = 0;
            for (size_t j = 0; j < M; j++)
            {
                si |= (T)seeds[i * M + j] << (32 * j);
            }
            s[i] = si;
        }
    }

    void discard(unsigned long long n)
    {
        Derived* pThis = static_cast<Derived*>(this);
        while (n > 0)
        {
            pThis->operator()();
            n--;
        }
    }

    bool operator==(const XoshiroBase& other) const
    {
        for (size_t i = 0; i < N; i++)
        {
            if (s[i] != other.s[i])
            {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const XoshiroBase& other) const
    {
        return !(*this == other);
    }

protected:
    static T rotl(T x, int k)
    { return (x << k) | (x >> (sizeof(T) * 8 - k)); }

    T s[4];
};

class Xoshiro256PlusPlus : public XoshiroBase<uint64_t, 4, Xoshiro256PlusPlus>
{
public:
    Xoshiro256PlusPlus() { seed(); }
    explicit Xoshiro256PlusPlus(uint64_t value) { seed(value); }
    template<class Sseq>
    explicit Xoshiro256PlusPlus(Sseq& seq) { seed(seq); }

    uint64_t operator()()
    {
        uint64_t result = rotl(s[0] + s[3], 23) + s[0];

        uint64_t t = s[1] << 17;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;

        s[3] = rotl(s[3], 45);

        return result;
    }
};

class Xoshiro256StarStar : public XoshiroBase<uint64_t, 4, Xoshiro256StarStar>
{
public:
    Xoshiro256StarStar() { seed(); }
    explicit Xoshiro256StarStar(uint64_t value) { seed(value); }
    template<class Sseq>
    explicit Xoshiro256StarStar(Sseq& seq) { seed(seq); }

    uint64_t operator()()
    {
        uint64_t result = rotl(s[1] * 5, 7) * 9;

        uint64_t t = s[1] << 17;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;

        s[3] = rotl(s[3], 45);

        return result;
    }
};

class Xoshiro256Plus : public XoshiroBase<uint64_t, 4, Xoshiro256Plus>
{
public:
    Xoshiro256Plus() { seed(); }
    explicit Xoshiro256Plus(uint64_t value) { seed(value); }
    template<class Sseq>
    explicit Xoshiro256Plus(Sseq& seq) { seed(seq); }

    uint64_t operator()()
    {
        uint64_t result = s[0] + s[3];

        uint64_t t = s[1] << 17;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;

        s[3] = rotl(s[3], 45);

        return result;
    }
};

class Xoroshiro128PlusPlus : public XoshiroBase<uint64_t, 2, Xoroshiro128PlusPlus>
{
public:
    Xoroshiro128PlusPlus() { seed(); }
    explicit Xoroshiro128PlusPlus(uint64_t value) { seed(value); }
    template<class Sseq>
    explicit Xoroshiro128PlusPlus(Sseq& seq) { seed(seq); }

    uint64_t operator()()
    {
        uint64_t result = rotl(s[0] + s[1], 17) + s[0];

        s[1] ^= s[0];
        s[0] = rotl(s[0], 49) ^ s[1] ^ (s[1] << 21);
        s[1] = rotl(s[1], 28);

        return result;
    }
};

class Xoroshiro128StarStar : public XoshiroBase<uint64_t, 2, Xoroshiro128StarStar>
{
public:
    Xoroshiro128StarStar() { seed(); }
    explicit Xoroshiro128StarStar(uint64_t value) { seed(value); }
    template<class Sseq>
    explicit Xoroshiro128StarStar(Sseq& seq) { seed(seq); }

    uint64_t operator()()
    {
        uint64_t result = rotl(s[0] * 5, 7) * 9;

        s[1] ^= s[0];
        s[0] = rotl(s[0], 24) ^ s[1] ^ (s[1] << 16);
        s[1] = rotl(s[1], 37);

        return result;
    }
};

class Xoroshiro128Plus : public XoshiroBase<uint64_t, 2, Xoroshiro128Plus>
{
public:
    Xoroshiro128Plus() { seed(); }
    explicit Xoroshiro128Plus(uint64_t value) { seed(value); }
    template<class Sseq>
    explicit Xoroshiro128Plus(Sseq& seq) { seed(seq); }

    uint64_t operator()()
    {
        uint64_t result = s[0] + s[1];

        s[1] ^= s[0];
        s[0] = rotl(s[0], 24) ^ s[1] ^ (s[1] << 16);
        s[1] = rotl(s[1], 37);

        return result;
    }
};

class Xoshiro128PlusPlus : public XoshiroBase<uint32_t, 4, Xoshiro128PlusPlus>
{
public:
    Xoshiro128PlusPlus() { seed(); }
    explicit Xoshiro128PlusPlus(uint32_t value) { seed(value); }
    template<class Sseq>
    explicit Xoshiro128PlusPlus(Sseq& seq) { seed(seq); }

    uint32_t operator()()
    {
        uint32_t result = rotl(s[0] + s[3], 7) + s[0];

        uint32_t t = s[1] << 9;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;

        s[3] = rotl(s[3], 11);

        return result;
    }
};

class Xoshiro128StarStar : public XoshiroBase<uint32_t, 4, Xoshiro128StarStar>
{
public:
    Xoshiro128StarStar() { seed(); }
    explicit Xoshiro128StarStar(uint32_t value) { seed(value); }
    template<class Sseq>
    explicit Xoshiro128StarStar(Sseq& seq) { seed(seq); }

    uint32_t operator()()
    {
        uint32_t result = rotl(s[1] * 5, 7) * 9;

        uint32_t t = s[1] << 9;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;

        s[3] = rotl(s[3], 11);

        return result;
    }
};

class Xoshiro128Plus : public XoshiroBase<uint32_t, 4, Xoshiro128Plus>
{
public:
    Xoshiro128Plus() { seed(); }
    explicit Xoshiro128Plus(uint32_t value) { seed(value); }
    template<class Sseq>
    explicit Xoshiro128Plus(Sseq& seq) { seed(seq); }

    uint32_t operator()()
    {
        uint32_t result = s[0] + s[3];

        uint32_t t = s[1] << 9;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;

        s[3] = rotl(s[3], 11);

        return result;
    }
};

// FastRandom.hpp

#include <type_traits>

// fastRandom
//
// Generate a random number from 0 to range-1 without using division.
// Use as a faster alternative to std::uniform_int_distribution.
// Assumes that Gen returns a full 32 or 64 bits of random data.

template<typename Gen, typename = std::enable_if_t<std::is_same_v<typename Gen::result_type, uint64_t>>>
uint64_t fastRandom(uint64_t range, Gen& gen)
{
#if ARCH_64
    return ((__uint128_t)range * gen()) >> 64;
#else
    return gen() % range;
#endif
}

template<typename Gen, typename = std::enable_if_t<std::is_same_v<typename Gen::result_type, uint32_t>>>
uint32_t fastRandom(uint32_t range, Gen& gen)
{
    return ((uint64_t)range * gen()) >> 32;
}

// fastShuffle
//
// Shuffle the given range into a random order without using division.
// Use as a faster alternative to std::shuffle.
// Assumes that Gen returns a full 32 or 64 bits of random data.

template<class It, class Gen>
void fastShuffle(It first, It last, Gen& gen)
{
    size_t size = last - first;
    while (size > 1)
    {
        size_t pos = fastRandom(size, gen);
        size--;
        using std::swap;
        swap(*(first + pos), *(first + size));
    }
}

// fastRandomReal<T>
// fastRandomDouble
// fastRandomFloat
//
// 1 argument version: Generate a random real number between 0 and 1.
// 2 argument version: Generate a random real number between 0 and range.
// 3 argument version: Generate a random real number between low and high.
// Use as a faster alternative to std::uniform_real_distribution.
// Assumes that Gen returns a full 32 or 64 bits of random data.

template<typename RealType, typename Gen>
RealType fastRandomReal(Gen& gen)
{
    return (RealType)gen() * ((RealType)1 / (RealType)(typename Gen::result_type)-1);
}

template<typename RealType, typename Gen>
RealType fastRandomReal(RealType range, Gen& gen)
{
    return fastRandomReal<RealType>(gen) * range;
}

template<typename RealType, typename Gen>
RealType fastRandomReal(RealType low, RealType high, Gen& gen)
{
    return fastRandomReal<RealType>(gen) * (high - low) + low;
}

template<typename Gen>
double fastRandomDouble(Gen& gen)
{ return fastRandomReal<double>(gen); }

template<typename Gen>
double fastRandomDouble(double range, Gen& gen)
{ return fastRandomReal<double>(range, gen); }

template<typename Gen>
double fastRandomDouble(double low, double high, Gen& gen)
{ return fastRandomReal<double>(low, high, gen); }

template<typename Gen>
float fastRandomFloat(Gen& gen)
{ return fastRandomReal<float>(gen); }

template<typename Gen>
float fastRandomFloat(float range, Gen& gen)
{ return fastRandomReal<float>(range, gen); }

template<typename Gen>
float fastRandomFloat(float low, float high, Gen& gen)
{ return fastRandomReal<float>(low, high, gen); }

// StringUtils.hpp

std::string strprintf(const char* format, ...)
    __attribute__ ((format (printf, 1, 2)));
std::string vstrprintf(const char* format, va_list ap)
    __attribute__ ((format (printf, 1, 0)));

int strcmpNoCase(const char* s1, const char* s2);

// TimeUtils.hpp

void sleepS(uint32_t durationS);
void sleepMS(uint32_t durationMS);

uint64_t getTimeS();
uint64_t getTimeMS();

// ProcessId.hpp

uint32_t getProcessId();

// ParseUtils.hpp

bool parseU32(const std::string& str, uint32_t* pValue = nullptr);
bool parseI32(const std::string& str, int32_t* pValue = nullptr);
bool parseU64(const std::string& str, uint64_t* pValue = nullptr);
bool parseI64(const std::string& str, int64_t* pValue = nullptr);
bool parseFloat(const std::string& str, float* pValue = nullptr);
bool parseDouble(const std::string& str, double* pValue = nullptr);
bool parseDurationS(const std::string& str, uint64_t* pDurationS = nullptr);
bool parseDurationMS(const std::string& str, uint64_t* pDurationMS = nullptr);

// StringUtils.cpp

using std::string;

string strprintf(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    string str = vstrprintf(format, ap);
    va_end(ap);
    return str;
}

string vstrprintf(const char* format, va_list ap)
{
    va_list ap2;
    va_copy(ap2, ap);
    size_t size = vsnprintf(nullptr, 0, format, ap2);
    va_end(ap2);
    std::unique_ptr<char[]> buf(new char[size+1]);
    vsnprintf(buf.get(), size+1, format, ap);
    return buf.get();
}

int strcmpNoCase(const char* s1, const char* s2)
{
    while (true)
    {
        char ch1 = *s1;
        char ch2 = *s2;
        if (ch1 == '\0')
        {
            if (ch2 == '\0')
                return 0;
            else
                return -1;
        }
        if (ch2 == '\0')
            return 1;
        if (ch1 >= 'a' && ch1 <= 'z')
            ch1 = 'A' + (ch1 - 'a');
        if (ch2 >= 'a' && ch2 <= 'z')
            ch2 = 'A' + (ch2 - 'a');
        if (ch1 != ch2)
        {
            if (ch1 < ch2)
                return -1;
            else
                return 1;
        }
        s1++;
        s2++;
    }
}

// TimeUtils.cpp

#if PLATFORM_WINDOWS
#include <windows.h>
#else
#include <time.h>
#endif

void sleepS(uint32_t durationS)
{
    sleepMS(durationS * 1000);
}

void sleepMS(uint32_t durationMS)
{
#if PLATFORM_WINDOWS
    Sleep(durationMS);
#else
    struct timespec req;
    memset(&req, 0, sizeof(req));
    req.tv_sec = (time_t)(durationMS / 1000);
    req.tv_nsec = (long)((durationMS % 1000) * 1000000);
    nanosleep(&req, nullptr);
#endif
}

uint64_t getTimeS()
{
#if PLATFORM_WINDOWS
    return (uint64_t)GetTickCount64() / UINT64_C(1000);
#else
    struct timespec ts;
    memset(&ts, 0, sizeof(ts));
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) != 0)
    {
        fprintf(stderr, "clock_gettime failed\n");
        exit(1);
    }
    uint64_t val = (uint64_t)ts.tv_sec;
    return val;
#endif
}

uint64_t getTimeMS()
{
#if PLATFORM_WINDOWS
    return (uint64_t)GetTickCount64();
#else
    struct timespec ts;
    memset(&ts, 0, sizeof(ts));
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) != 0)
    {
        fprintf(stderr, "clock_gettime failed\n");
        exit(1);
    }
    uint64_t val = (uint64_t)ts.tv_sec;
    val *= 1000;
    val += ((uint64_t)ts.tv_nsec) / 1000000;
    return val;
#endif
}

// ProcessId.cpp

#if PLATFORM_WINDOWS
#include <windows.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

uint32_t getProcessId()
{
#if PLATFORM_WINDOWS
    return (uint32_t)GetCurrentProcessId();
#else
    return (uint32_t)getpid();
#endif
}

// ParseUtils.cpp

#include <limits>
#include <type_traits>

using std::string;

namespace
{
    template<typename T>
    bool parseIntImpl(const string& str, T* pValue)
    {
        T base = 10;
        size_t size = str.size();
        if (size == 0) return false;
        
        bool neg = false;
        size_t pos = 0;
        if (std::is_signed<T>::value && str[0] == '-')
        {
            if (size == 1) return false;
            neg = true;
            pos = 1;
        }

        T value = 0;
        for (; pos < size; pos++)
        {
            if (neg)
            {
                if (value < std::numeric_limits<T>::min() / base)
                {
                    return false;
                }
            }
            else
            {
                if (value > std::numeric_limits<T>::max() / base)
                {
                    return false;
                }
            }
            value *= base;
            char ch = str[pos];
            T digit;
            if (ch >= '0' && ch <= '9')
            {
                digit = ch - '0';
            }
            else if (ch >= 'a' && ch <= 'z')
            {
                digit = ch - 'a' + 10;
            }
            else if (ch >= 'A' && ch <= 'Z')
            {
                digit = ch - 'A' + 10;
            }
            else
            {
                return false;
            }
            if (digit >= base)
            {
                return false;
            }
            if (neg)
            {
                if (-digit < std::numeric_limits<T>::min() - value)
                {
                    return false;
                }
                value -= digit;
            }
            else
            {
                if (digit > std::numeric_limits<T>::max() - value)
                {
                    return false;
                }
                value += digit;
            }
        }
        if (pValue)
        {
            *pValue = value;
        }
        return true;
    }

    template<typename T, typename F>
    bool parseFloatImpl(const string& str, T* pValue, F&& f)
    {
        {
            size_t size = str.size();
            size_t pos = 0;
            if (size != 0 && str[0] == '-')
            {
                pos = 1;
            }
            bool gotDot = false;
            bool gotDigit = false;
            for (; pos < size; pos++)
            {
                char ch = str[pos];
                if (ch == '.')
                {
                    if (gotDot) return false;
                    gotDot = true;
                }
                else if (ch >= '0' && ch <= '9')
                {
                    gotDigit = true;
                }
                else
                {
                    return false;
                }
            }
            if (!gotDigit)
            {
                return false;
            }
        }
        const char* nptr = str.c_str();
        char* endptr = nullptr;
        errno = 0;
        auto value = f(nptr, &endptr);
        if (errno != 0 || *nptr == '\0' || *endptr != '\0')
        {
            return false;
        }
        if (pValue)
        {
            *pValue = (T)value;
        }
        return true;
    }
}

bool parseU32(const string& str, uint32_t* pValue)
{
    return parseIntImpl(str, pValue);
}

bool parseI32(const string& str, int32_t* pValue)
{
    return parseIntImpl(str, pValue);
}

bool parseU64(const string& str, uint64_t* pValue)
{
    return parseIntImpl(str, pValue);
}

bool parseI64(const string& str, int64_t* pValue)
{
    return parseIntImpl(str, pValue);
}

bool parseFloat(const string& str, float* pValue)
{
    return parseFloatImpl(str, pValue, strtof);
}

bool parseDouble(const string& str, double* pValue)
{
    return parseFloatImpl(str, pValue, strtod);
}

namespace
{
    bool parseDurationImpl(const string& str, bool wantMS, uint64_t* pDuration)
    {
        bool gotH = false;
        bool gotM = false;
        bool gotS = false;
        string strH;
        string strM;
        string strS;
        size_t size = str.size();
        size_t pos = 0;
        size_t start = pos;
        bool gotDot = false;
        while (pos < size)
        {
            char ch = str[pos++];
            if (ch >= '0' && ch <= '9')
            {
            }
            else if (ch == '.')
            {
                if (!wantMS || gotDot) return false;
                gotDot = true;
            }
            else if (ch == 'h')
            {
                if (gotDot || gotH || gotM || gotS) return false;
                if (pos - 1 == start) return false;
                gotH = true;
                strH = str.substr(start, pos - 1 - start);
                start = pos;
                gotDot = false;
            }
            else if (ch == 'm')
            {
                if (gotDot || gotM || gotS) return false;
                if (pos - 1 == start) return false;
                gotM = true;
                strM = str.substr(start, pos - 1 - start);
                start = pos;
                gotDot = false;
            }
            else if (ch == 's')
            {
                if (gotS) return false;
                if (pos - 1 == start) return false;
                gotS = true;
                strS = str.substr(start, pos - 1 - start);
                start = pos;
                gotDot = false;
            }
        }
        if (pos != start) return false;
        if (!gotH && !gotM && !gotS) return false;

        uint64_t h = 0;
        if (gotH)
        {
            if (!parseU64(strH, &h)) return false;
        }

        uint64_t m = 0;
        if (gotM)
        {
            if (!parseU64(strM, &m)) return false;
            if (gotH && m >= 60) return false;
        }

        uint64_t s = 0;
        uint64_t ms = 0;
        if (gotS)
        {
            auto dotPos = strS.find('.');
            if (dotPos == string::npos)
            {
                if (!parseU64(strS, &s)) return false;
            }
            else
            {
                string strBefore = strS.substr(0, dotPos);
                string strAfter = strS.substr(dotPos + 1);
                if (!strBefore.empty())
                {
                    if (!parseU64(strBefore, &s)) return false;
                }
                if (strAfter.empty()) return false;
                if (strAfter.size() > 3) return false;
                if (strAfter.size() > 0) ms += 100 * (strAfter[0] - '0');
                if (strAfter.size() > 1) ms += 10 * (strAfter[1] - '0');
                if (strAfter.size() > 2) ms += 1 * (strAfter[2] - '0');
            }
            if (gotM && s >= 60) return false;
            if (gotH && s >= 3600) return false;
        }

        uint64_t limit = std::numeric_limits<uint64_t>::max();
        if (wantMS) limit /= 1000;

        uint64_t duration = 0;

        if (h > (limit - duration) / (60 * 60)) return false;
        duration += h * 60 * 60;

        if (m > (limit - duration) / 60) return false;
        duration += m * 60;

        if (s > (limit - duration)) return false;
        duration += s;

        if (wantMS)
        {
            duration *= 1000;
            limit = std::numeric_limits<uint64_t>::max();

            if (ms > (limit - duration)) return false;
            duration += ms;
        }

        if (pDuration) *pDuration = duration;
        return true;
    }
}

bool parseDurationS(const string& str, uint64_t* pDurationS)
{
    return parseDurationImpl(str, false, pDurationS);
}

bool parseDurationMS(const string& str, uint64_t* pDurationMS)
{
    return parseDurationImpl(str, true, pDurationMS);
}

// solve.cpp

#include <cmath>

using std::string;
using std::vector;
using std::unique_ptr;

typedef Xoshiro256StarStar Gen;

constexpr const uint64_t N = 100;
constexpr const uint64_t K = 10;
constexpr const uint64_t T = 10000;

class Vertex
{
public:
    uint32_t x;
    uint32_t y;
    vector<uint8_t> neighbors;
};

class Edge
{
public:
    uint8_t fromIdx;
    uint8_t toIdx;
};

class Problem
{
public:
    uint64_t M;
    vector<Vertex> vertices;
    vector<Edge> edges;
};

typedef vector<uint8_t> Solution;

string readLine(FILE* f)
{
    string line;
    while (true)
    {
        int iCh = fgetc(f);
        if (iCh == EOF)
        {
            fprintf(stderr, "Unexpected end of input\n");
            exit(1);
        }
        char ch = (char)(unsigned char)iCh;
        if (ch == '\n') break;
        line.push_back(ch);
    }
    return line;
}

vector<string> split(const string& str, char splitCh)
{
    vector<string> strs(1);
    for (char ch : str)
    {
        if (ch == splitCh)
        {
            strs.emplace_back();
        }
        else
        {
            strs.back().push_back(ch);
        }
    }
    return strs;
}

void readProblem(FILE* f, Problem& problem)
{
    string paramLine = readLine(f);
    vector<string> paramFields = split(paramLine, ' ');
    uint64_t problemN;
    uint64_t problemK;
    uint64_t problemT;
    if (paramFields.size() != 4 ||
        !parseU64(paramFields[0], &problemN) ||
        !parseU64(paramFields[1], &problem.M) ||
        !parseU64(paramFields[2], &problemK) ||
        !parseU64(paramFields[3], &problemT))
    {
        fprintf(stderr, "Error parsing params\n");
        exit(1);
    }

    uint64_t M = problem.M;

    if (problemN != N)
    {
        fprintf(stderr, "Unexpected N = %" PRIu64 "\n", problemN);
        exit(1);
    }

    if (problemK != K)
    {
        fprintf(stderr, "Unexpected K = %" PRIu64 "\n", problemK);
        exit(1);
    }

    if (problemT != T)
    {
        fprintf(stderr, "Unexpected T = %" PRIu64 "\n", problemT);
        exit(1);
    }

    problem.vertices.resize(N);
    problem.edges.resize(M);

    for (uint64_t m = 0; m < M; m++)
    {
        string edgeLine = readLine(f);
        vector<string> edgeFields = split(edgeLine, ' ');
        uint64_t fromIdx;
        uint64_t toIdx;
        if (edgeFields.size() != 2 ||
            !parseU64(edgeFields[0], &fromIdx) ||
            !parseU64(edgeFields[1], &toIdx))
        {
            fprintf(stderr, "Error parsing edge\n");
            exit(1);
        }
        if (fromIdx >= N || toIdx >= N)
        {
            fprintf(stderr, "Unexpected vertex index\n");
            exit(1);
        }
        auto& edge = problem.edges[m];
        edge.fromIdx = fromIdx;
        edge.toIdx = toIdx;
        auto& fromVertex = problem.vertices[fromIdx];
        fromVertex.neighbors.push_back(toIdx);
        auto& toVertex = problem.vertices[toIdx];
        toVertex.neighbors.push_back(fromIdx);
    }

    for (uint64_t n = 0; n < N; n++)
    {
        string vertexLine = readLine(f);
        vector<string> vertexFields = split(vertexLine, ' ');
        uint32_t x;
        uint32_t y;
        if (vertexFields.size() != 2 ||
            !parseU32(vertexFields[0], &x) ||
            !parseU32(vertexFields[1], &y))
        {
            fprintf(stderr, "Error parsing vertex\n");
            exit(1);
        }
        if (x > 300 || y > 300)
        {
            fprintf(stderr, "Unexpected coordinate\n");
            exit(1);
        }
        auto& vertex = problem.vertices[n];
        vertex.x = x;
        vertex.y = y;
    }
}

void writeSolution(FILE* f, const Problem& problem, const Solution& solution)
{
    for (uint8_t vertexIdx : solution)
    {
        printf("%" PRIi8 "\n", (int8_t)vertexIdx);
    }
}

bool calcScore(const Problem& problem,
               const Solution& solution,
               uint64_t* pScore)
{
    uint64_t actionCount = solution.size();
    if (actionCount > T) return false;

    uint64_t score = 0;

    uint64_t vertexIdx = 0;
    uint64_t prevVertexIdx = 255;
    uint8_t treeFlavors[N] = {};
    vector<vector<uint8_t>> shopFlavors[K];
    vector<uint8_t> coneFlavor;

    for (uint64_t actionIdx = 0; actionIdx < actionCount; actionIdx++)
    {
        uint8_t action = solution[actionIdx];

        if (action == 255)
        {
            if (vertexIdx < K) return false;
            if (treeFlavors[vertexIdx] != 0) return false;
            treeFlavors[vertexIdx] = 1;
        }
        else
        {
            uint64_t nextVertexIdx = action;
            if (nextVertexIdx == prevVertexIdx) return false;
            auto& vertex = problem.vertices[vertexIdx];
            bool isNeighbor = false;
            for (uint64_t neighborIdx : vertex.neighbors)
            {
                if (nextVertexIdx == neighborIdx)
                {
                    isNeighbor = true;
                    break;
                }
            }
            if (!isNeighbor) return false;
            if (nextVertexIdx < K)
            {
                bool hadFlavor = false;
                for (auto& shopFlavor : shopFlavors[nextVertexIdx])
                {
                    if (coneFlavor == shopFlavor)
                    {
                        hadFlavor = true;
                        break;
                    }
                }
                if (!hadFlavor)
                {
                    shopFlavors[nextVertexIdx].push_back(coneFlavor);
                    score++;
                }
                coneFlavor.clear();
            }
            else
            {
                coneFlavor.push_back(treeFlavors[nextVertexIdx]);
            }
            prevVertexIdx = vertexIdx;
            vertexIdx = nextVertexIdx;
        }
    }

    if (pScore) *pScore = score;
    return true;
}

class Solver
{
public:
    Solver() {}
    virtual ~Solver() {}

    virtual Solution solve(const Problem& problem) = 0;
};

class TrivialSolver : public Solver
{
public:
    virtual Solution solve(const Problem& problem) override
    {
        Solution solution;
        return solution;
    }
};

class RandomSolver : public Solver
{
public:
    virtual Solution solve(const Problem& problem) override
    {
        Gen gen;

#if 0
        {
            uint64_t timeMS = getTimeMS();
            uint32_t pid = getProcessId();
            uint32_t seed0 = 0x0646b73b;
            uint32_t seed1 = (uint32_t)timeMS;
            uint32_t seed2 = (uint32_t)pid ^ (uint32_t)(timeMS >> 32);
            std::seed_seq seq{seed0, seed1, seed2};
            gen.seed(seq);
        }
#endif
#if 1
        {
            uint32_t seed0 = 0x0646b73b;
            std::seed_seq seq{seed0};
            gen.seed(seq);
        }
#endif

        Solution solution;

        uint64_t vertexIdx = 0;
        uint64_t prevVertexIdx = 255;
        uint8_t treeFlavors[N] = {};

        for (uint64_t t = 0; t < T; t++)
        {
            auto& vertex = problem.vertices[vertexIdx];
            uint64_t neighborCount = vertex.neighbors.size();
            uint64_t prevNeighborIdx = (uint64_t)-1;
            for (uint64_t neighborIdx = 0; neighborIdx < neighborCount; neighborIdx++)
            {
                if (vertex.neighbors[neighborIdx] == prevVertexIdx)
                {
                    prevNeighborIdx = neighborIdx;
                    break;
                }
            }
            uint64_t allowedNeighborCount = neighborCount;
            if (prevNeighborIdx != 255) allowedNeighborCount--;
            uint64_t randCount = allowedNeighborCount;
            if (vertexIdx >= K && treeFlavors[vertexIdx] == 0) randCount++;

            uint64_t randIdx = fastRandom(randCount, gen);
            if (randIdx == allowedNeighborCount)
            {
                solution.push_back(255);
                treeFlavors[vertexIdx] = 1;
            }
            else
            {
                uint64_t neighborIdx = randIdx;
                if (prevNeighborIdx != 255 && neighborIdx >= prevNeighborIdx)
                {
                    neighborIdx++;
                }
                uint64_t nextVertexIdx = vertex.neighbors[neighborIdx];
                solution.push_back(nextVertexIdx);
                prevVertexIdx = vertexIdx;
                vertexIdx = nextVertexIdx;
            }
        }

        return solution;
    }
};

class BeamSolver : public Solver
{
public:
    class FlavorEntry
    {
    public:
        uint32_t prevFlavorIdx;
        uint32_t flavor;
    };

    class ActionEntry
    {
    public:
        uint32_t prevActionIdx;
        uint8_t action;
    };

    class State
    {
    public:
        uint8_t vertexIdx;
        uint8_t prevVertexIdx;
        uint64_t treeFlavors[2];
        uint32_t coneFlavor;
        uint32_t flavorIdxs[K];
        uint32_t actionIdx;
        uint64_t score;
        uint64_t redCount;
    };

    class Cand
    {
    public:
        uint64_t oldStateIdx;
        uint8_t action;
        uint64_t score;
        uint64_t rand;

        bool operator<(const Cand& other) const
        {
            if (score != other.score) return score > other.score;
            return rand < other.rand;
        }
    };

    virtual Solution solve(const Problem& problem) override
    {
        Gen gen;

#if 0
        {
            uint64_t timeMS = getTimeMS();
            uint32_t pid = getProcessId();
            uint32_t seed0 = 0x0646b73b;
            uint32_t seed1 = (uint32_t)timeMS;
            uint32_t seed2 = (uint32_t)pid ^ (uint32_t)(timeMS >> 32);
            std::seed_seq seq{seed0, seed1, seed2};
            gen.seed(seq);
        }
#endif
#if 1
        {
            uint32_t seed0 = 0x0646b73b;
            std::seed_seq seq{seed0};
            gen.seed(seq);
        }
#endif

        uint64_t beamWidth = 20 * N;

        vector<FlavorEntry> flavorEntries[K];
        for (uint64_t k = 0; k < K; k++)
        {
            flavorEntries[k].reserve(beamWidth * T + 1);
            flavorEntries[k].resize(1);
        }

        vector<ActionEntry> actionEntries;
        actionEntries.reserve(beamWidth * T + 1);
        actionEntries.resize(1);

        vector<State> oldStates(beamWidth);
        vector<State> newStates(beamWidth);

        uint64_t oldStateCount = 0;
        uint64_t newStateCount = 0;

        {
            auto& state = oldStates[oldStateCount++];
            state.vertexIdx = 0;
            state.prevVertexIdx = 255;
            state.treeFlavors[0] = 0;
            state.treeFlavors[1] = 0;
            state.coneFlavor = 1;
            for (uint64_t k = 0; k < K; k++)
            {
                state.flavorIdxs[k] = 0;
            }
            state.actionIdx = 0;
            state.score = 0;
            state.redCount = 0;
        }

        vector<Cand> cands[N];

        for (uint64_t t = 0; t < T; t++)
        {
            uint64_t maxRedCount = 1 + t / 160;

            for (uint64_t oldStateIdx = 0; oldStateIdx < oldStateCount; oldStateIdx++)
            {
                auto& oldState = oldStates[oldStateIdx];
                uint64_t vertexIdx = oldState.vertexIdx;

                if (vertexIdx >= K && oldState.redCount < maxRedCount)
                {
                    if (((oldState.treeFlavors[vertexIdx >> 6] >> (vertexIdx & 63)) & 1) == 0)
                    {
                        auto& cand = cands[vertexIdx].emplace_back();
                        cand.oldStateIdx = oldStateIdx;
                        cand.action = 255;
                        cand.score = oldState.score;
                        cand.rand = gen();
                    }
                }

                uint64_t prevVertexIdx = oldState.prevVertexIdx;
                auto& vertex = problem.vertices[vertexIdx];
                auto& neighbors = vertex.neighbors;
                uint64_t neighborCount = neighbors.size();

                for (uint64_t neighborIdx = 0; neighborIdx < neighborCount; neighborIdx++)
                {
                    uint64_t nextVertexIdx = neighbors[neighborIdx];
                    if (nextVertexIdx == prevVertexIdx) continue;

                    uint64_t score = oldState.score;
                    if (nextVertexIdx < K)
                    {
                        uint32_t flavor = oldState.coneFlavor * K + nextVertexIdx;
                        bool hadFlavor = false;
                        uint64_t flavorIdx = oldState.flavorIdxs[nextVertexIdx];
                        while (flavorIdx != 0)
                        {
                            auto& flavorEntry = flavorEntries[nextVertexIdx][flavorIdx];
                            if (flavor == flavorEntry.flavor)
                            {
                                hadFlavor = true;
                                break;
                            }
                            flavorIdx = flavorEntry.prevFlavorIdx;
                        }
                        if (!hadFlavor)
                        {
                            score++;
                        }
                    }

                    auto& cand = cands[nextVertexIdx].emplace_back();
                    cand.oldStateIdx = oldStateIdx;
                    cand.action = nextVertexIdx;
                    cand.score = score;
                    cand.rand = gen();
                }
            }

            for (uint64_t n = 0; n < N; n++)
            {
                uint64_t keepCount = std::min(beamWidth / N, (uint64_t)cands[n].size());
                std::nth_element(cands[n].begin(), cands[n].begin() + (keepCount - 1), cands[n].end());

                for (uint64_t candIdx = 0; candIdx < keepCount; candIdx++)
                {
                    auto& cand = cands[n][candIdx];
                    auto& newState = newStates[newStateCount++];
                    auto& oldState = oldStates[cand.oldStateIdx];
                    uint8_t action = cand.action;

                    uint64_t vertexIdx = oldState.vertexIdx;

                    if (action == 255)
                    {
                        newState.vertexIdx = vertexIdx;
                        newState.prevVertexIdx = oldState.prevVertexIdx;
                        newState.treeFlavors[0] = oldState.treeFlavors[0];
                        newState.treeFlavors[1] = oldState.treeFlavors[1];

                        newState.treeFlavors[vertexIdx >> 6] |= (uint64_t)1 << (vertexIdx & 63);

                        newState.coneFlavor = oldState.coneFlavor;
                        for (uint64_t k = 0; k < K; k++)
                        {
                            newState.flavorIdxs[k] = oldState.flavorIdxs[k];
                        }
                        newState.actionIdx = actionEntries.size();
                        {
                            auto& actionEntry = actionEntries.emplace_back();
                            actionEntry.prevActionIdx = oldState.actionIdx;
                            actionEntry.action = action;
                        }
                        newState.score = cand.score;
                        newState.redCount = oldState.redCount + 1;
                    }
                    else
                    {
                        uint64_t nextVertexIdx = action;

                        newState.vertexIdx = nextVertexIdx;
                        newState.prevVertexIdx = vertexIdx;
                        newState.treeFlavors[0] = oldState.treeFlavors[0];
                        newState.treeFlavors[1] = oldState.treeFlavors[1];
                        for (uint64_t k = 0; k < K; k++)
                        {
                            newState.flavorIdxs[k] = oldState.flavorIdxs[k];
                        }

                        if (nextVertexIdx < K)
                        {
                            // Shop
                            newState.coneFlavor = 1;

                            if (cand.score != oldState.score)
                            {
                                newState.flavorIdxs[nextVertexIdx] = flavorEntries[nextVertexIdx].size();
                                auto& flavorEntry = flavorEntries[nextVertexIdx].emplace_back();
                                flavorEntry.prevFlavorIdx = oldState.flavorIdxs[nextVertexIdx];
                            
                                flavorEntry.flavor = oldState.coneFlavor * K + nextVertexIdx;
                            }
                        }
                        else
                        {
                            // Tree
                            uint32_t coneFlavor = oldState.coneFlavor;
                            coneFlavor <<= 1;
                            coneFlavor |= (newState.treeFlavors[nextVertexIdx >> 6] >> (nextVertexIdx & 63)) & 1;

                            newState.coneFlavor = coneFlavor;
                        }

                        newState.actionIdx = actionEntries.size();
                        {
                            auto& actionEntry = actionEntries.emplace_back();
                            actionEntry.prevActionIdx = oldState.actionIdx;
                            actionEntry.action = action;
                        }
                        newState.score = cand.score;
                        newState.redCount = oldState.redCount;
                    }
                }

                cands[n].clear();
            }

            oldStates.swap(newStates);
            oldStateCount = newStateCount;
            newStateCount = 0;
        }

        Solution solution;
        uint64_t actionIdx = oldStates[0].actionIdx;
        while (actionIdx != 0)
        {
            auto& actionEntry = actionEntries[actionIdx];
            solution.push_back(actionEntry.action);
            actionIdx = actionEntry.prevActionIdx;
        }
        std::reverse(solution.begin(), solution.end());
        return solution;
    }
};

class Beam2Solver : public Solver
{
public:
    class FlavorEntry
    {
    public:
        uint32_t prevFlavorIdx;
        uint32_t flavor;
    };

    class ActionEntry
    {
    public:
        uint32_t prevActionIdx;
        uint8_t action;
    };

    class State
    {
    public:
        uint8_t vertexIdx;
        uint8_t prevVertexIdx;
        uint64_t treeFlavors[2];
        uint32_t coneFlavor;
        uint32_t flavorIdxs[K];
        uint32_t actionIdx;
        uint64_t score;
        uint64_t redCount;
    };

    class Cand
    {
    public:
        uint64_t oldStateIdx;
        uint8_t action;
        uint64_t score;
        uint64_t rand;

        bool operator<(const Cand& other) const
        {
            if (score != other.score) return score > other.score;
            return rand < other.rand;
        }
    };

    virtual Solution solve(const Problem& problem) override
    {
        Gen gen;

#if 0
        {
            uint64_t timeMS = getTimeMS();
            uint32_t pid = getProcessId();
            uint32_t seed0 = 0x0646b73b;
            uint32_t seed1 = (uint32_t)timeMS;
            uint32_t seed2 = (uint32_t)pid ^ (uint32_t)(timeMS >> 32);
            std::seed_seq seq{seed0, seed1, seed2};
            gen.seed(seq);
        }
#endif
#if 1
        {
            uint32_t seed0 = 0x0646b73b;
            std::seed_seq seq{seed0};
            gen.seed(seq);
        }
#endif

        uint64_t beamWidth = 16 * N;

        vector<FlavorEntry> flavorEntries[K];
        for (uint64_t k = 0; k < K; k++)
        {
            flavorEntries[k].reserve(beamWidth * T + 1);
            flavorEntries[k].resize(1);
        }

        vector<ActionEntry> actionEntries;
        actionEntries.reserve(beamWidth * T + 1);
        actionEntries.resize(1);

        vector<State> oldStates(beamWidth);
        vector<State> newStates(beamWidth);

        uint64_t oldStateCount = 0;
        uint64_t newStateCount = 0;

        {
            auto& state = oldStates[oldStateCount++];
            state.vertexIdx = 0;
            state.prevVertexIdx = 255;
            state.treeFlavors[0] = 0;
            state.treeFlavors[1] = 0;
            state.coneFlavor = 1;
            for (uint64_t k = 0; k < K; k++)
            {
                state.flavorIdxs[k] = 0;
            }
            state.actionIdx = 0;
            state.score = 0;
            state.redCount = 0;
        }

        vector<Cand> cands[N];

        static constexpr const uint64_t splitT = 6200;
        static constexpr const uint64_t fullT = T;

        static constexpr const uint64_t splitRed = 22;
        static constexpr const uint64_t fullRed = 44;

        for (uint64_t t = 0; t < T; t++)
        {
            uint64_t maxRedCount;
            if (t < splitT)
            {
                maxRedCount = 1 + t * splitRed / splitT;
            }
            else
            {
                maxRedCount = 1 + splitRed + (t - splitT) * (fullRed - splitRed) / (fullT - splitT);
            }

            for (uint64_t oldStateIdx = 0; oldStateIdx < oldStateCount; oldStateIdx++)
            {
                auto& oldState = oldStates[oldStateIdx];
                uint64_t vertexIdx = oldState.vertexIdx;

                if (vertexIdx >= K && oldState.redCount < maxRedCount)
                {
                    if (((oldState.treeFlavors[vertexIdx >> 6] >> (vertexIdx & 63)) & 1) == 0)
                    {
                        auto& cand = cands[vertexIdx].emplace_back();
                        cand.oldStateIdx = oldStateIdx;
                        cand.action = 255;
                        cand.score = oldState.score;
                        cand.rand = gen();
                    }
                }

                uint64_t prevVertexIdx = oldState.prevVertexIdx;
                auto& vertex = problem.vertices[vertexIdx];
                auto& neighbors = vertex.neighbors;
                uint64_t neighborCount = neighbors.size();

                for (uint64_t neighborIdx = 0; neighborIdx < neighborCount; neighborIdx++)
                {
                    uint64_t nextVertexIdx = neighbors[neighborIdx];
                    if (nextVertexIdx == prevVertexIdx) continue;

                    uint64_t score = oldState.score;
                    if (nextVertexIdx < K)
                    {
                        uint32_t flavor = oldState.coneFlavor * K + nextVertexIdx;
                        bool hadFlavor = false;
                        uint64_t flavorIdx = oldState.flavorIdxs[nextVertexIdx];
                        while (flavorIdx != 0)
                        {
                            auto& flavorEntry = flavorEntries[nextVertexIdx][flavorIdx];
                            if (flavor == flavorEntry.flavor)
                            {
                                hadFlavor = true;
                                break;
                            }
                            flavorIdx = flavorEntry.prevFlavorIdx;
                        }
                        if (!hadFlavor)
                        {
                            score++;
                        }
                    }

                    auto& cand = cands[nextVertexIdx].emplace_back();
                    cand.oldStateIdx = oldStateIdx;
                    cand.action = nextVertexIdx;
                    cand.score = score;
                    cand.rand = gen();
                }
            }

            for (uint64_t n = 0; n < N; n++)
            {
                uint64_t keepCount = std::min(beamWidth / N, (uint64_t)cands[n].size());
                std::nth_element(cands[n].begin(), cands[n].begin() + (keepCount - 1), cands[n].end());

                for (uint64_t candIdx = 0; candIdx < keepCount; candIdx++)
                {
                    auto& cand = cands[n][candIdx];
                    auto& newState = newStates[newStateCount++];
                    auto& oldState = oldStates[cand.oldStateIdx];
                    uint8_t action = cand.action;

                    uint64_t vertexIdx = oldState.vertexIdx;

                    if (action == 255)
                    {
                        newState.vertexIdx = vertexIdx;
                        newState.prevVertexIdx = oldState.prevVertexIdx;
                        newState.treeFlavors[0] = oldState.treeFlavors[0];
                        newState.treeFlavors[1] = oldState.treeFlavors[1];

                        newState.treeFlavors[vertexIdx >> 6] |= (uint64_t)1 << (vertexIdx & 63);

                        newState.coneFlavor = oldState.coneFlavor;
                        for (uint64_t k = 0; k < K; k++)
                        {
                            newState.flavorIdxs[k] = oldState.flavorIdxs[k];
                        }
                        newState.actionIdx = actionEntries.size();
                        {
                            auto& actionEntry = actionEntries.emplace_back();
                            actionEntry.prevActionIdx = oldState.actionIdx;
                            actionEntry.action = action;
                        }
                        newState.score = cand.score;
                        newState.redCount = oldState.redCount + 1;
                    }
                    else
                    {
                        uint64_t nextVertexIdx = action;

                        newState.vertexIdx = nextVertexIdx;
                        newState.prevVertexIdx = vertexIdx;
                        newState.treeFlavors[0] = oldState.treeFlavors[0];
                        newState.treeFlavors[1] = oldState.treeFlavors[1];
                        for (uint64_t k = 0; k < K; k++)
                        {
                            newState.flavorIdxs[k] = oldState.flavorIdxs[k];
                        }

                        if (nextVertexIdx < K)
                        {
                            // Shop
                            newState.coneFlavor = 1;

                            if (cand.score != oldState.score)
                            {
                                newState.flavorIdxs[nextVertexIdx] = flavorEntries[nextVertexIdx].size();
                                auto& flavorEntry = flavorEntries[nextVertexIdx].emplace_back();
                                flavorEntry.prevFlavorIdx = oldState.flavorIdxs[nextVertexIdx];
                            
                                flavorEntry.flavor = oldState.coneFlavor * K + nextVertexIdx;
                            }
                        }
                        else
                        {
                            // Tree
                            uint32_t coneFlavor = oldState.coneFlavor;
                            coneFlavor <<= 1;
                            coneFlavor |= (newState.treeFlavors[nextVertexIdx >> 6] >> (nextVertexIdx & 63)) & 1;

                            newState.coneFlavor = coneFlavor;
                        }

                        newState.actionIdx = actionEntries.size();
                        {
                            auto& actionEntry = actionEntries.emplace_back();
                            actionEntry.prevActionIdx = oldState.actionIdx;
                            actionEntry.action = action;
                        }
                        newState.score = cand.score;
                        newState.redCount = oldState.redCount;
                    }
                }

                cands[n].clear();
            }

            oldStates.swap(newStates);
            oldStateCount = newStateCount;
            newStateCount = 0;
        }

        uint64_t bestScore = 0;
        uint64_t bestStateIdx = 0;
        for (uint64_t stateIdx = 0; stateIdx < oldStateCount; stateIdx++)
        {
            auto& state = oldStates[stateIdx];
            uint64_t score = state.score;
            if (score > bestScore)
            {
                bestScore = score;
                bestStateIdx = stateIdx;
            }
        }

        Solution solution;
        uint64_t actionIdx = oldStates[bestStateIdx].actionIdx;
        while (actionIdx != 0)
        {
            auto& actionEntry = actionEntries[actionIdx];
            solution.push_back(actionEntry.action);
            actionIdx = actionEntry.prevActionIdx;
        }
        std::reverse(solution.begin(), solution.end());
        return solution;
    }
};

unique_ptr<Solver> createSolver(const string& solverName)
{
    unique_ptr<Solver> pSolver;

    if (solverName == "trivial") pSolver.reset(new TrivialSolver);
    else if (solverName == "random") pSolver.reset(new RandomSolver);
    else if (solverName == "beam") pSolver.reset(new BeamSolver);
    else if (solverName == "beam2") pSolver.reset(new Beam2Solver);

    return pSolver;
}

int listProblems()
{
    printf("seed     M\n");
    printf("----  ----\n");
    uint64_t seedCount = 1000;
    for (uint64_t seed = 0; seed < seedCount; seed++)
    {
        printf("%4" PRIu64 "  ", seed);
        fflush(stdout);

        Problem problem;
        {
            string fileName = strprintf("../../../in/%04" PRIu64 ".txt", seed);
            FILE* f = fopen(fileName.c_str(), "rb");
            if (!f)
            {
                printf("Error opening input file\n");
                return 1;
            }
            readProblem(f, problem);
            fclose(f);
        }

        printf("%4" PRIu64 "\n", problem.M);
    }
    return 0;
}

int runTournament(const string& solverName)
{
    uint64_t totalScore = 0;
    uint64_t maxDurationMS = 0;
    uint64_t seedCount = 150;
    for (uint64_t seed = 0; seed < seedCount; seed++)
    {
        printf("%4" PRIu64 "  ", seed);
        fflush(stdout);

        // Create solver

        unique_ptr<Solver> pSolver = createSolver(solverName);
        if (!pSolver)
        {
            printf("Unknown solver\n");
            return 1;
        }

        // Read input

        Problem problem;
        {
            string fileName = strprintf("../../../in/%04" PRIu64 ".txt", seed);
            FILE* f = fopen(fileName.c_str(), "rb");
            if (!f)
            {
                printf("Error opening input file\n");
                return 1;
            }
            readProblem(f, problem);
            fclose(f);
        }

        // Solve

        uint64_t startTimeMS = getTimeMS();
        Solution solution = pSolver->solve(problem);
        uint64_t endTimeMS = getTimeMS();
        uint64_t durationMS = endTimeMS - startTimeMS;

        // Calculate score

        bool bad = false;
        uint64_t score;
        if (!calcScore(problem, solution, &score))
        {
            bad = true;
            score = 0;
        }

        printf("%9" PRIu64 "  %4" PRIu64 " ms%s\n", score, durationMS, bad ? "  BAD" : "");

        totalScore += score;
        maxDurationMS = std::max(maxDurationMS, durationMS);
    }
    printf("      %9" PRIu64 "  %4" PRIu64 " ms\n", totalScore * 150 / seedCount, maxDurationMS);
    return 0;
}

constexpr const char* defaultSolverName = "beam2";

void usage()
{
    printf("Usage: solve [<options>] [<solver>]\n");
    printf("  <solver>\n");
    printf("        Solver to use (default: %s)\n", defaultSolverName);
    printf("Options:\n");
    printf("  -h    Print usage information and exit\n");
    printf("  -l    List problems\n");
    printf("  -t    Tournament mode: run solver on a set of seeds\n");
    printf("  -v    Verbose: don't suppress debug output in tournament mode\n");
    printf("\n");
    printf("Solver      Description\n");
    printf("----------  ------------------------------------------------------------\n");
    printf("trivial     Perform no actions\n");
    printf("random      Perform random actions\n");
    printf("beam        Beam search\n");
    printf("beam2       Heuristic tweaks\n");
}

int main(int argc, char *argv[])
{
    bool gotSolverName = false;

    bool help = false;
    bool list = false;
    bool tourney = false;
    bool verbose = false;
    string solverName(defaultSolverName);

    int iArg = 1;
    while (iArg < argc)
    {
        string strArg = argv[iArg++];

        if (strArg == "-h" || strArg == "--help")
        {
            help = true;
        }
        else if (strArg == "-l")
        {
            list = true;
        }
        else if (strArg == "-t")
        {
            tourney = true;
        }
        else if (strArg == "-v")
        {
            verbose = true;
        }
        else if (!gotSolverName)
        {
            solverName = strArg;
            gotSolverName = true;
        }
        else
        {
            usage();
            return 1;
        }
    }

    if (help)
    {
        usage();
        return 0;
    }

    if (list)
    {
        return listProblems();
    }

    if (tourney)
    {
        if (!verbose)
        {
            if (!freopen("/dev/null", "w", stderr))
            {
                fprintf(stderr, "freopen failed\n");
                return 1;
            }
        }

        return runTournament(solverName);
    }

    // Create solver

    unique_ptr<Solver> pSolver = createSolver(solverName);
    if (!pSolver)
    {
        printf("Unknown solver\n");
        return 1;
    }

    // Read input

    Problem problem;
    readProblem(stdin, problem);

    // Solve

    Solution solution = pSolver->solve(problem);

    // Write output

    writeSolution(stdout, problem, solution);
    fflush(stdout);

    return 0;
}
