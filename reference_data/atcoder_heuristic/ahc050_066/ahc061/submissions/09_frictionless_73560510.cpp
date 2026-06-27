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

// ThreadUtils.hpp

class ThreadImpl;

class Thread
{
public:
    Thread();
    explicit Thread(std::function<void()> f);
    Thread(Thread&& other);
    ~Thread();
    Thread& operator=(Thread&& other);

    void swap(Thread& other);
    bool joinable() const;
    void join();
    void detach();

    static uint32_t hardwareConcurrency();

private:
    std::unique_ptr<ThreadImpl> m_pImpl;

private:
    Thread(const Thread& other) = delete;
    Thread& operator=(const Thread& other) = delete;
};

void swap(Thread& x, Thread& y);

class MutexImpl;

class Mutex
{
public:
    Mutex();
    ~Mutex();

    void lock();
    bool tryLock();
    void unlock();

private:
    friend class ConditionVariable;

    std::unique_ptr<MutexImpl> m_pImpl;

private:
    Mutex(const Mutex& other) = delete;
    Mutex(Mutex&& other) = delete;
    Mutex& operator=(const Mutex& other) = delete;
    Mutex& operator=(Mutex&& other) = delete;
};

struct DeferLockType {};
struct TryToLockType {};
struct AdoptLockType {};

constexpr DeferLockType DeferLock{};
constexpr TryToLockType TryToLock{};
constexpr AdoptLockType AdoptLock{};

template<class _MutexType>
class LockGuard
{
public:
    typedef _MutexType MutexType;

    explicit LockGuard(MutexType& mutex) : m_mutex(mutex) { m_mutex.lock(); }
    LockGuard(MutexType& mutex, AdoptLockType) : m_mutex(mutex) {}
    ~LockGuard() { m_mutex.unlock(); }

private:
    MutexType& m_mutex;

private:
    LockGuard(const LockGuard& other) = delete;
    LockGuard(LockGuard&& other) = delete;
    LockGuard& operator=(const LockGuard& other) = delete;
    LockGuard& operator=(LockGuard&& other) = delete;
};

template<class _MutexType>
class UniqueLock
{
public:
    typedef _MutexType MutexType;

    UniqueLock() :
        m_pMutex(nullptr), m_owns(false) {}

    explicit UniqueLock(MutexType& mutex) :
        m_pMutex(&mutex), m_owns(true)
    {
        mutex.lock();
    }

    UniqueLock(MutexType& mutex, DeferLockType) :
        m_pMutex(&mutex), m_owns(false) {}

    UniqueLock(MutexType& mutex, TryToLockType) :
        m_pMutex(&mutex), m_owns(mutex.tryLock()) {}

    UniqueLock(MutexType& mutex, AdoptLockType) :
        m_pMutex(&mutex), m_owns(true) {}

    UniqueLock(UniqueLock&& other) :
        m_pMutex(other.m_pMutex), m_owns(other.m_owns)
    {
        other.m_pMutex = nullptr;
        other.m_owns = false;
    }

    ~UniqueLock()
    {
        if (m_owns)
        {
            m_pMutex->unlock();
        }
    }

    UniqueLock& operator=(UniqueLock&& other)
    {
        if (this != &other)
        {
            if (m_owns)
            {
                m_pMutex->unlock();
            }
            m_pMutex = other.m_pMutex;
            m_owns = other.m_owns;
            other.m_pMutex = nullptr;
            other.m_owns = false;
        }
        return *this;
    }

    void lock()
    {
        m_pMutex->lock();
        m_owns = true;
    }

    bool tryLock()
    {
        m_owns = m_pMutex->tryLock();
        return m_owns;
    }

    void unlock()
    {
        m_pMutex->unlock();
        m_owns = false;
    }

    void swap(UniqueLock& other)
    {
        using std::swap;
        swap(m_pMutex, other.m_pMutex);
        swap(m_owns, other.m_owns);
    }

    MutexType* release()
    {
        MutexType* pMutex = m_pMutex;
        m_pMutex = nullptr;
        m_owns = false;
        return pMutex;
    }

    bool ownsLock() const
    {
        return m_owns;
    }

    explicit operator bool() const
    {
        return m_owns;
    }

    MutexType* mutex() const
    {
        return m_pMutex;
    }

private:
    MutexType* m_pMutex;
    bool m_owns;

private:
    UniqueLock(const UniqueLock& other) = delete;
    UniqueLock& operator=(const UniqueLock& other) = delete;
};

enum class CVStatus : uint32_t
{
    NoTimeout,
    Timeout
};

class ConditionVariableImpl;

class ConditionVariable
{
public:
    ConditionVariable();
    ~ConditionVariable();

    void notifyOne();
    void notifyAll();

    void wait(UniqueLock<Mutex>& lock);

    template<class Predicate>
    void wait(UniqueLock<Mutex>& lock, Predicate pred)
    {
        while (!pred())
        {
            wait(lock);
        }
    }

    CVStatus waitUntilMS(UniqueLock<Mutex>& lock, uint64_t timeMS);

    template<class Predicate>
    bool waitUntilMS(UniqueLock<Mutex>& lock, uint64_t timeMS, Predicate pred)
    {
        while (!pred())
        {
            if (waitUntilMS(lock, timeMS) == CVStatus::Timeout)
            {
                return pred();
            }
        }
        return true;
    }

    CVStatus waitForMS(UniqueLock<Mutex>& lock, uint32_t durationMS);

    template<class Predicate>
    bool waitForMS(UniqueLock<Mutex>& lock, uint32_t durationMS, Predicate pred)
    {
        return waitUntilMS(lock, getTimeMS() + durationMS, std::move(pred));
    }

private:
    static uint64_t getTimeMS();

    std::unique_ptr<ConditionVariableImpl> m_pImpl;

private:
    ConditionVariable(const ConditionVariable& other) = delete;
    ConditionVariable(ConditionVariable&& other) = delete;
    ConditionVariable& operator=(const ConditionVariable& other) = delete;
    ConditionVariable& operator=(ConditionVariable&& other) = delete;
};

enum class FutureStatus : uint32_t
{
    Ready,
    Timeout
};

template<class T> class Future;
template<class T> class Promise;

template<class T>
class SharedStateImpl
{
private:
    friend class Future<T>;
    friend class Promise<T>;

    SharedStateImpl() :
        m_pValue(),
        m_mutex(),
        m_cond(),
        m_refCount(1),
        m_futureRetrieved(false),
        m_valueSet(false)
    {}

    void release()
    {
        bool destroy = false;
        {
            LockGuard<Mutex> lock(m_mutex);
            m_refCount--;
            if (m_refCount == 0)
            {
                destroy = true;
            }
        }
        if (destroy)
        {
            delete this;
        }
    }

    std::unique_ptr<T> m_pValue;
    Mutex m_mutex;
    ConditionVariable m_cond;
    uint32_t m_refCount;
    bool m_futureRetrieved;
    bool m_valueSet;
};

template<class T>
class Promise
{
public:
    Promise() : m_pImpl(new SharedStateImpl<T>) {}

    Promise(Promise&& other) : m_pImpl(other.m_pImpl)
    {
        other.m_pImpl = nullptr;
    }

    ~Promise()
    {
        if (m_pImpl)
        {
            m_pImpl->release();
        }
    }

    Promise& operator=(Promise&& other)
    {
        if (this != &other)
        {
            if (m_pImpl)
            {
                m_pImpl->release();
            }
            m_pImpl = other.m_pImpl;
            other.m_pImpl = nullptr;
        }
        return *this;
    }

    void swap(Promise& other)
    {
        using std::swap;
        swap(m_pImpl, other.m_pImpl);
    }

    Future<T> getFuture()
    {
        if (!m_pImpl)
        {
            fprintf(stderr, "Promise::getFuture: No state\n");
            exit(1);
        }
        LockGuard<Mutex> lock(m_pImpl->m_mutex);
        if (m_pImpl->m_futureRetrieved)
        {
            fprintf(stderr, "Promise::getFuture: Future already retrieved\n");
            exit(1);
        }
        m_pImpl->m_futureRetrieved = true;
        m_pImpl->m_refCount++;
        return Future<T>(m_pImpl);
    }

    void setValue(T value)
    {
        if (!m_pImpl)
        {
            fprintf(stderr, "Promise::setValue: No state\n");
            exit(1);
        }
        LockGuard<Mutex> lock(m_pImpl->m_mutex);
        if (m_pImpl->m_valueSet)
        {
            fprintf(stderr, "Promise::setValue: Promise already satisfied\n");
            exit(1);
        }
        m_pImpl->m_valueSet = true;
        m_pImpl->m_pValue.reset(new T(std::move(value)));
        m_pImpl->m_cond.notifyOne();
    }

private:
    SharedStateImpl<T>* m_pImpl;

private:
    Promise(const Promise& other) = delete;
    Promise& operator=(const Promise& other) = delete;
};

template<class T>
void swap(Promise<T>& x, Promise<T>& y)
{
    x.swap(y);
}

template<class T>
class Future
{
public:
    Future() : m_pImpl(nullptr) {}

    Future(Future&& other) : m_pImpl(other.m_pImpl)
    {
        other.m_pImpl = nullptr;
    }

    ~Future()
    {
        if (m_pImpl)
        {
            m_pImpl->release();
        }
    }

    Future& operator=(Future&& other)
    {
        if (this != &other)
        {
            if (m_pImpl)
            {
                m_pImpl->release();
            }
            m_pImpl = other.m_pImpl;
            other.m_pImpl = nullptr;
        }

        return *this;
    }

    T get()
    {
        std::unique_ptr<T> pValue;
        {
            UniqueLock<Mutex> lock(m_pImpl->m_mutex);
            auto pred = [&]() { return m_pImpl->m_valueSet; };
            m_pImpl->m_cond.wait(lock, pred);
            pValue = std::move(m_pImpl->m_pValue);
        }
        m_pImpl->release();
        m_pImpl = nullptr;
        return std::move(*pValue);
    }

    bool valid() const
    {
        return m_pImpl != nullptr;
    }

    void wait() const
    {
        UniqueLock<Mutex> lock(m_pImpl->m_mutex);
        auto pred = [&]() { return m_pImpl->m_valueSet; };
        m_pImpl->m_cond.wait(lock, pred);
    }

    FutureStatus waitForMS(uint32_t durationMS) const
    {
        UniqueLock<Mutex> lock(m_pImpl->m_mutex);
        auto pred = [&]() { return m_pImpl->m_valueSet; };
        return m_pImpl->m_cond.waitForMS(lock, durationMS, pred) ?
            FutureStatus::Ready :
            FutureStatus::Timeout;
    }

    FutureStatus waitUntilMS(uint64_t timeMS) const
    {
        UniqueLock<Mutex> lock(m_pImpl->m_mutex);
        auto pred = [&]() { return m_pImpl->m_valueSet; };
        return m_pImpl->m_cond.waitUntilMS(lock, timeMS, pred) ?
            FutureStatus::Ready :
            FutureStatus::Timeout;
    }

private:
    friend class Promise<T>;

    Future(SharedStateImpl<T>* pImpl) : m_pImpl(pImpl) {}

    SharedStateImpl<T>* m_pImpl;

private:
    Future(const Future& other) = delete;
    Future& operator=(const Future& other) = delete;
};

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

// ThreadUtils.cpp

#include <exception>
#include <stdexcept>

#if PLATFORM_WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#endif

class ThreadImpl
{
public:
    bool m_joinable;
#if PLATFORM_WINDOWS
    HANDLE m_hThread;
#else
    pthread_t m_threadId;
#endif
};

Thread::Thread() :
    m_pImpl(new ThreadImpl)
{
    m_pImpl->m_joinable = false;
}

namespace
{
#if PLATFORM_WINDOWS
    DWORD WINAPI threadFunc(LPVOID arg)
    {
        std::unique_ptr<std::function<void()> >
            pf((std::function<void()>*)arg);
        (*pf)();
        return 0;
    }
#else
    void* threadFunc(void* arg)
    {
        std::unique_ptr<std::function<void()> >
            pf((std::function<void()>*)arg);
        (*pf)();
        return nullptr;
    }
#endif
}

Thread::Thread(std::function<void()> f) :
    m_pImpl(new ThreadImpl)
{
    std::function<void()>* pf = new std::function<void()>(std::move(f));
#if PLATFORM_WINDOWS
    m_pImpl->m_hThread = CreateThread(nullptr,
                                      0,
                                      threadFunc,
                                      pf,
                                      0,
                                      nullptr);
    if (m_pImpl->m_hThread != NULL)
    {
        m_pImpl->m_joinable = true;
    }
    else
    {
        delete pf;
        m_pImpl->m_joinable = false;
    }
#else
    if (pthread_create(&m_pImpl->m_threadId,
                       nullptr,
                       threadFunc,
                       pf) == 0)
    {
        m_pImpl->m_joinable = true;
    }
    else
    {
        delete pf;
        m_pImpl->m_joinable = false;
    }
#endif
}

Thread::Thread(Thread&& other) :
    m_pImpl(new ThreadImpl)
{
    m_pImpl->m_joinable = other.m_pImpl->m_joinable;
#if PLATFORM_WINDOWS
    m_pImpl->m_hThread = other.m_pImpl->m_hThread;
#else
    m_pImpl->m_threadId = other.m_pImpl->m_threadId;
#endif
    other.m_pImpl->m_joinable = false;
}

Thread::~Thread()
{
    if (m_pImpl->m_joinable)
    {
        std::terminate();
    }
}

Thread& Thread::operator=(Thread&& other)
{
    if (m_pImpl->m_joinable)
    {
        std::terminate();
    }
    m_pImpl->m_joinable = other.m_pImpl->m_joinable;
#if PLATFORM_WINDOWS
    m_pImpl->m_hThread = other.m_pImpl->m_hThread;
#else
    m_pImpl->m_threadId = other.m_pImpl->m_threadId;
#endif
    other.m_pImpl->m_joinable = false;

    return *this;
}

void Thread::swap(Thread& other)
{
    using std::swap;
    swap(m_pImpl, other.m_pImpl);
}

bool Thread::joinable() const
{
    return m_pImpl->m_joinable;
}

void Thread::join()
{
    if (!m_pImpl->m_joinable)
    {
        throw std::invalid_argument("this");
    }
#if PLATFORM_WINDOWS
    WaitForSingleObject(m_pImpl->m_hThread, INFINITE);
    CloseHandle(m_pImpl->m_hThread);
#else
    pthread_join(m_pImpl->m_threadId, nullptr);
#endif
    m_pImpl->m_joinable = false;
}

void Thread::detach()
{
    if (!m_pImpl->m_joinable)
    {
        throw std::invalid_argument("this");
    }
#if PLATFORM_WINDOWS
    CloseHandle(m_pImpl->m_hThread);
#else
    pthread_detach(m_pImpl->m_threadId);
#endif
    m_pImpl->m_joinable = false;
}

uint32_t Thread::hardwareConcurrency()
{
#if PLATFORM_WINDOWS
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    DWORD result = systemInfo.dwNumberOfProcessors;
    if (result <= 0 || result > 256)
    {
        return 0;
    }
    else
    {
        return (uint32_t)result;
    }
#else
    long result = sysconf(_SC_NPROCESSORS_ONLN);
    if (result <= 0 || result > 256)
    {
        return 0;
    }
    else
    {
        return (uint32_t)result;
    }
#endif
}

void swap(Thread& x, Thread& y)
{
    x.swap(y);
}

class MutexImpl
{
public:
#if PLATFORM_WINDOWS
    CRITICAL_SECTION m_cs;
#else
    pthread_mutex_t m_mutex;
#endif
};

Mutex::Mutex() :
    m_pImpl(new MutexImpl)
{
#if PLATFORM_WINDOWS
    InitializeCriticalSection(&m_pImpl->m_cs);
#else
    if (pthread_mutex_init(&m_pImpl->m_mutex, nullptr) != 0)
    {
        fprintf(stderr, "pthread_mutex_init failed\n");
        exit(1);
    }
#endif
}

Mutex::~Mutex()
{
#if PLATFORM_WINDOWS
    DeleteCriticalSection(&m_pImpl->m_cs);
#else
    pthread_mutex_destroy(&m_pImpl->m_mutex);
#endif
}

void Mutex::lock()
{
#if PLATFORM_WINDOWS
    EnterCriticalSection(&m_pImpl->m_cs);
#else
    if (pthread_mutex_lock(&m_pImpl->m_mutex) != 0)
    {
        fprintf(stderr, "pthread_mutex_lock failed\n");
        exit(1);
    }
#endif
}

bool Mutex::tryLock()
{
#if PLATFORM_WINDOWS
    return TryEnterCriticalSection(&m_pImpl->m_cs) ? true : false;
#else
    int result = pthread_mutex_trylock(&m_pImpl->m_mutex);
    if (result == 0)
    {
        return true;
    }
    else
    {
        if (result != EBUSY)
        {
            fprintf(stderr, "pthread_mutex_trylock failed\n");
            exit(1);
        }
        return false;
    }
#endif
}

void Mutex::unlock()
{
#if PLATFORM_WINDOWS
    LeaveCriticalSection(&m_pImpl->m_cs);
#else
    if (pthread_mutex_unlock(&m_pImpl->m_mutex) != 0)
    {
        fprintf(stderr, "pthread_mutex_lock failed\n");
        exit(1);
    }
#endif
}

class ConditionVariableImpl
{
public:
#if PLATFORM_WINDOWS
    CONDITION_VARIABLE m_cv;
#else
    pthread_cond_t m_cond;
    clockid_t m_clockId;
#endif
};

ConditionVariable::ConditionVariable() :
    m_pImpl(new ConditionVariableImpl)
{
#if PLATFORM_WINDOWS
    InitializeConditionVariable(&m_pImpl->m_cv);
#else
    pthread_condattr_t attr;
    if (pthread_condattr_init(&attr) != 0)
    {
        fprintf(stderr, "pthread_condattr_init failed\n");
        exit(1);
    }
    if (pthread_condattr_setclock(&attr, CLOCK_MONOTONIC_RAW) != 0)
    {
        if (pthread_condattr_setclock(&attr, CLOCK_MONOTONIC) != 0)
        {
            // Use the default clock
        }
    }
    if (pthread_condattr_getclock(&attr, &m_pImpl->m_clockId) != 0)
    {
        fprintf(stderr, "pthread_condattr_getclock failed\n");
        exit(1);
    }
    if (pthread_cond_init(&m_pImpl->m_cond, &attr) != 0)
    {
        fprintf(stderr, "pthread_cond_init failed\n");
        exit(1);
    }
    if (pthread_condattr_destroy(&attr) != 0)
    {
        fprintf(stderr, "pthread_condattr_destroy failed\n");
        exit(1);
    }
#endif
}

ConditionVariable::~ConditionVariable()
{
#if PLATFORM_WINDOWS
#else
    pthread_cond_destroy(&m_pImpl->m_cond);
#endif
}

void ConditionVariable::notifyOne()
{
#if PLATFORM_WINDOWS
    WakeConditionVariable(&m_pImpl->m_cv);
#else
    if (pthread_cond_signal(&m_pImpl->m_cond) != 0)
    {
        fprintf(stderr, "pthread_cond_signal failed\n");
        exit(1);
    }
#endif
}

void ConditionVariable::notifyAll()
{
#if PLATFORM_WINDOWS
    WakeAllConditionVariable(&m_pImpl->m_cv);
#else
    if (pthread_cond_broadcast(&m_pImpl->m_cond) != 0)
    {
        fprintf(stderr, "pthread_cond_broadcast failed\n");
        exit(1);
    }
#endif
}

void ConditionVariable::wait(UniqueLock<Mutex>& lock)
{
#if PLATFORM_WINDOWS
    if (SleepConditionVariableCS(&m_pImpl->m_cv,
                                 &lock.mutex()->m_pImpl->m_cs,
                                 INFINITE) == 0)
    {
        fprintf(stderr, "SleepConditionVariableCS failed\n");
        exit(1);
    }
#else
    if (pthread_cond_wait(&m_pImpl->m_cond,
                          &lock.mutex()->m_pImpl->m_mutex) != 0)
    {
        fprintf(stderr, "pthread_cond_wait failed\n");
        exit(1);
    }
#endif
}

CVStatus ConditionVariable::waitUntilMS(UniqueLock<Mutex>& lock,
                                        uint64_t timeMS)
{
    uint64_t durationMS = timeMS - getTimeMS();
    if (durationMS >= UINT64_C(0x8000000000000000))
    {
        durationMS = 0;
    }
    else if (durationMS > UINT64_C(0x0000000070000000))
    {
        durationMS = UINT64_C(0x0000000070000000);
    }

    return waitForMS(lock, (uint32_t)durationMS);
}

CVStatus ConditionVariable::waitForMS(UniqueLock<Mutex>& lock,
                                      uint32_t durationMS)
{
    if (durationMS > 0x70000000)
    {
        durationMS = 0x70000000;
    }

#if PLATFORM_WINDOWS
    if (SleepConditionVariableCS(&m_pImpl->m_cv,
                                 &lock.mutex()->m_pImpl->m_cs,
                                 (DWORD)durationMS) == 0)
    {
        if (GetLastError() == ERROR_TIMEOUT)
        {
            return CVStatus::Timeout;
        }
        else
        {
            fprintf(stderr, "SleepConditionVariableCS failed\n");
            exit(1);
        }
    }
    else
    {
        return CVStatus::NoTimeout;
    }
#else
    struct timespec ts;
    memset(&ts, 0, sizeof(ts));
    if (clock_gettime(m_pImpl->m_clockId, &ts) != 0)
    {
        fprintf(stderr, "clock_gettime failed\n");
        exit(1);
    }

    ts.tv_sec += (time_t)(durationMS / 1000);
    ts.tv_nsec += (long)((durationMS % 1000) * 1000000);
    if (ts.tv_nsec >= 1000000000)
    {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec++;
    }

    int result =
        pthread_cond_timedwait(&m_pImpl->m_cond,
                               &lock.mutex()->m_pImpl->m_mutex,
                               &ts);
    if (result != 0)
    {
        if (result == ETIMEDOUT)
        {
            return CVStatus::Timeout;
        }
        else
        {
            fprintf(stderr, "pthread_cond_timedwait failed\n");
            exit(1);
        }
    }
    else
    {
        return CVStatus::NoTimeout;
    }
#endif
}

uint64_t ConditionVariable::getTimeMS()
{
    return ::getTimeMS();
}

// solve.cpp

#include <cmath>

using std::string;
using std::vector;
using std::unique_ptr;

#define PROFILE 0

typedef Xoshiro256StarStar Gen;

constexpr const uint64_t N = 10;
constexpr const uint64_t minM = 2;
constexpr const uint64_t maxM = 8;
constexpr const uint64_t T = 100;
constexpr const uint64_t minU = 1;
constexpr const uint64_t maxU = 5;

inline uint64_t makeCoord(uint64_t x, uint64_t y) { return y * 10 + x; }
inline uint64_t getX(uint64_t coord) { return coord % 10; }
inline uint64_t getY(uint64_t coord) { return coord / 10; }

constexpr const uint64_t dirToDelta[4] = { 1, N, (uint64_t)-1, (uint64_t)-N };

constexpr const uint8_t coordToDirs[N * N] =
{
    0b0011,0b0111,0b0111,0b0111,0b0111,0b0111,0b0111,0b0111,0b0111,0b0110,
    0b1011,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1110,
    0b1011,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1110,
    0b1011,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1110,
    0b1011,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1110,
    0b1011,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1110,
    0b1011,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1110,
    0b1011,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1110,
    0b1011,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1110,
    0b1001,0b1101,0b1101,0b1101,0b1101,0b1101,0b1101,0b1101,0b1101,0b1100
};

class Problem
{
public:
    uint64_t M;
    uint64_t U;
    uint32_t values[N * N];
    uint8_t moves[maxM];
    uint8_t pieces[maxM];
    uint8_t owners[N * N];
    uint8_t levels[N * N];
    uint64_t turn;
};

class AiPlayer
{
public:
    double wa;
    double wb;
    double wc;
    double wd;
    double eps;
};

class AiRand
{
public:
    double r1;
    double r2;
};

class Ai
{
public:
    AiPlayer players[maxM];
    AiRand rands[T][maxM];
};

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

void readProblem(FILE* f, Problem& problem, Ai* pAi = nullptr)
{
    string paramLine = readLine(f);
    vector<string> paramFields = split(paramLine, ' ');
    uint64_t problemN;
    uint64_t problemT;
    if (paramFields.size() != 4 ||
        !parseU64(paramFields[0], &problemN) ||
        !parseU64(paramFields[1], &problem.M) ||
        !parseU64(paramFields[2], &problemT) ||
        !parseU64(paramFields[3], &problem.U))
    {
        fprintf(stderr, "Error parsing params\n");
        exit(1);
    }

    if (problemN != N)
    {
        fprintf(stderr, "Unexpected N = %" PRIu64 "\n", problemN);
        exit(1);
    }

    if (problem.M < minM || problem.M > maxM)
    {
        fprintf(stderr, "Unexpected M = %" PRIu64 "\n", problem.M);
        exit(1);
    }

    if (problemT != T)
    {
        fprintf(stderr, "Unexpected T = %" PRIu64 "\n", problemT);
        exit(1);
    }

    if (problem.U < minU || problem.U > maxU)
    {
        fprintf(stderr, "Unexpected U = %" PRIu64 "\n", problem.U);
        exit(1);
    }

    uint64_t M = problem.M;

    for (uint64_t y = 0; y < N; y++)
    {
        string valueLine = readLine(f);
        vector<string> valueFields = split(valueLine, ' ');
        if (valueFields.size() != N)
        {
            fprintf(stderr, "Unexpected number of values in value line\n");
            exit(1);
        }

        for (uint64_t x = 0; x < N; x++)
        {
            uint64_t coord = makeCoord(x, y);

            uint32_t value;
            if (!parseU32(valueFields[x], &value))
            {
                fprintf(stderr, "Error parsing value\n");
                exit(1);
            }

            problem.values[coord] = value;
            problem.owners[coord] = 255;
            problem.levels[coord] = 0;
        }
    }

    for (uint64_t m = 0; m < maxM; m++)
    {
        problem.moves[m] = 0;
        problem.pieces[m] = 0;
    }

    for (uint64_t m = 0; m < M; m++)
    {
        string pieceLine = readLine(f);
        vector<string> pieceFields = split(pieceLine, ' ');
        uint64_t x;
        uint64_t y;
        if (pieceFields.size() != 2 ||
            !parseU64(pieceFields[0], &y) || y >= N ||
            !parseU64(pieceFields[1], &x) || x >= N)
        {
            fprintf(stderr, "Error parsing piece\n");
            exit(1);
        }

        uint64_t coord = makeCoord(x, y);

        if (problem.levels[coord] != 0)
        {
            fprintf(stderr, "Duplicate piece coordinate\n");
            exit(1);
        }

        problem.moves[m] = coord;
        problem.pieces[m] = coord;
        problem.owners[coord] = m;
        problem.levels[coord] = 1;
    }

    problem.turn = 0;

    if (pAi)
    {
        auto& ai = *pAi;

        for (uint64_t m = 0; m < maxM; m++)
        {
            auto& player = ai.players[m];
            player.wa = 0.0;
            player.wb = 0.0;
            player.wc = 0.0;
            player.wd = 0.0;
            player.eps = 0.0;
        }

        for (uint64_t m = 1; m < M; m++)
        {
            auto& player = ai.players[m];
            string playerLine = readLine(f);
            vector<string> playerFields = split(playerLine, ' ');
            if (playerFields.size() != 5 ||
                !parseDouble(playerFields[0], &player.wa) ||
                !parseDouble(playerFields[1], &player.wb) ||
                !parseDouble(playerFields[2], &player.wc) ||
                !parseDouble(playerFields[3], &player.wd) ||
                !parseDouble(playerFields[4], &player.eps))
            {
                fprintf(stderr, "Error parsing AI player\n");
                exit(1);
            }
        }

        for (uint64_t t = 0; t < T; t++)
        {
            for (uint64_t m = 0; m < maxM; m++)
            {
                ai.rands[t][m].r1 = 0.0;
                ai.rands[t][m].r2 = 0.0;
            }

            for (uint64_t m = 1; m < M; m++)
            {
                string randLine = readLine(f);
                vector<string> randFields = split(randLine, ' ');
                if (randFields.size() != 2 ||
                    !parseDouble(randFields[0], &ai.rands[t][m].r1) ||
                    !parseDouble(randFields[1], &ai.rands[t][m].r2))
                {
                    fprintf(stderr, "Error parsing random line\n");
                    exit(1);
                }
            }
        }
    }
}

void readStep(FILE* f, Problem& problem)
{
    uint64_t M = problem.M;
    uint64_t U = problem.U;

    for (uint64_t m = 0; m < M; m++)
    {
        string moveLine = readLine(f);
        vector<string> moveFields = split(moveLine, ' ');
        uint64_t x;
        uint64_t y;
        if (moveFields.size() != 2 ||
            !parseU64(moveFields[0], &y) || y >= N ||
            !parseU64(moveFields[1], &x) || x >= N)
        {
            fprintf(stderr, "Error parsing move\n");
            exit(1);
        }

        uint64_t coord = makeCoord(x, y);
        problem.moves[m] = coord;
    }

    for (uint64_t m = 0; m < M; m++)
    {
        string pieceLine = readLine(f);
        vector<string> pieceFields = split(pieceLine, ' ');
        uint64_t x;
        uint64_t y;
        if (pieceFields.size() != 2 ||
            !parseU64(pieceFields[0], &y) || y >= N ||
            !parseU64(pieceFields[1], &x) || x >= N)
        {
            fprintf(stderr, "Error parsing piece\n");
            exit(1);
        }

        uint64_t coord = makeCoord(x, y);
        for (uint64_t prevM = 0; prevM < m; prevM++)
        {
            if (coord == problem.pieces[prevM])
            {
                fprintf(stderr, "Duplicate piece coordinate\n");
                exit(1);
            }
        }

        problem.pieces[m] = coord;
    }

    for (uint64_t y = 0; y < N; y++)
    {
        string ownerLine = readLine(f);
        vector<string> ownerFields = split(ownerLine, ' ');
        if (ownerFields.size() != N)
        {
            fprintf(stderr, "Unexpected owner line size\n");
            exit(1);
        }

        for (uint64_t x = 0; x < N; x++)
        {
            uint64_t coord = makeCoord(x, y);

            int64_t owner;
            if (!parseI64(ownerFields[x], &owner))
            {
                fprintf(stderr, "Error parsing owner\n");
                exit(1);
            }

            if (owner == -1)
            {
                problem.owners[coord] = 255;
            }
            else if ((uint64_t)owner >= M)
            {
                fprintf(stderr, "Unexpected owner\n");
                exit(1);
            }
            else
            {
                problem.owners[coord] = (uint64_t)owner;
            }
        }
    }

    for (uint64_t y = 0; y < N; y++)
    {
        string levelLine = readLine(f);
        vector<string> levelFields = split(levelLine, ' ');
        if (levelFields.size() != N)
        {
            fprintf(stderr, "Unexpected level line size\n");
            exit(1);
        }

        for (uint64_t x = 0; x < N; x++)
        {
            uint64_t coord = makeCoord(x, y);

            uint64_t level;
            if (!parseU64(levelFields[x], &level))
            {
                fprintf(stderr, "Error parsing level\n");
                exit(1);
            }

            if (level > U)
            {
                fprintf(stderr, "Unexpected level\n");
                exit(1);
            }

            problem.levels[coord] = level;
        }
    }

    problem.turn++;
}

void writeMove(FILE* f, uint64_t coord)
{
    printf("%" PRIu64 " %" PRIu64 "\n", getY(coord), getX(coord));
}

class Solver
{
public:
    Solver() {}
    virtual ~Solver() {}

    virtual void init(const Problem& problem) {}
    virtual uint64_t step(const Problem& problem) = 0;
};

uint64_t calcScore(const Problem& problemIn,
                   const Ai& ai,
                   Solver* pSolver,
                   uint64_t* pDurationMS = nullptr)
{
    Problem problem(problemIn);

    uint64_t M = problem.M;
    uint64_t U = problem.U;

    uint8_t visited[N * N];
    uint8_t q[N * N];
    uint8_t bests[N * N];

    uint64_t durationMS = 0;

    for (uint64_t turn = 0; turn < T; turn++)
    {
        problem.turn = turn;

        uint64_t startTimeMS = getTimeMS();
        if (turn == 0) pSolver->init(problem);
        problem.moves[0] = pSolver->step(problem);
        uint64_t endTimeMS = getTimeMS();
        durationMS += (endTimeMS - startTimeMS);

#if 0
        if (problem.M != 2)
        writeMove(stdout, problem.moves[0]);
#endif

        for (uint64_t m = 0; m < M; m++)
        {
            for (uint64_t coord = 0; coord < N * N; coord++)
            {
                visited[coord] = 0;
            }

            uint64_t pieceCoord = problem.pieces[m];
            visited[pieceCoord] = 1;
            q[0] = pieceCoord;
            uint64_t qSize = 1;

            for (uint64_t qIdx = 0; qIdx < qSize; qIdx++)
            {
                uint64_t coord = q[qIdx];
                uint64_t owner = problem.owners[coord];
                if (owner != m) continue;

                uint64_t dirs = coordToDirs[coord];
                for (uint64_t dir = 0; dir < 4; dir++)
                {
                    if (!(dirs & ((uint64_t)1 << dir))) continue;
                    uint64_t ncoord = coord + dirToDelta[dir];
                    if (visited[ncoord]) continue;
                    uint64_t nowner = problem.owners[ncoord];
                    if (nowner != m && nowner != 255 && problem.pieces[nowner] == ncoord) continue;
                    visited[ncoord] = 1;
                    q[qSize++] = ncoord;
                }
            }

            if (m == 0)
            {
                if (!visited[problem.moves[0]])
                {
                    if (pDurationMS) *pDurationMS = durationMS;
                    return 0;
                }
            }
            else
            {
                auto& player = ai.players[m];
                auto& rand = ai.rands[turn][m];

                if (rand.r1 < player.eps)
                {
                    uint64_t moveIdx = (uint64_t)(rand.r2 * (double)qSize);
                    if (moveIdx >= qSize) moveIdx = qSize - 1;
                    problem.moves[m] = q[moveIdx];
                }
                else
                {
                    double bestScore = 0.0;
                    uint64_t bestCount = 0;

                    for (uint64_t qIdx = 0; qIdx < qSize; qIdx++)
                    {
                        uint64_t coord = q[qIdx];
                        uint64_t owner = problem.owners[coord];
                        uint64_t level = problem.levels[coord];
                        double value = (double)problem.values[coord];
                        double score;
                        if (owner == 255)
                        {
                            score = value * player.wa;
                        }
                        else if (owner == m)
                        {
                            if (level < U)
                            {
                                score = value * player.wb;
                            }
                            else
                            {
                                score = 0.0;
                            }
                        }
                        else
                        {
                            if (level == 1)
                            {
                                score = value * player.wc;
                            }
                            else
                            {
                                score = value * player.wd;
                            }
                        }

                        if (score > bestScore)
                        {
                            bestScore = score;
                            bests[0] = coord;
                            bestCount = 1;
                        }
                        else if (score == bestScore)
                        {
                            bests[bestCount++] = coord;
                        }
                    }

                    uint64_t bestIdx = (uint64_t)(rand.r2 * (double)bestCount);
                    if (bestIdx >= bestCount) bestIdx = bestCount - 1;
                    problem.moves[m] = bests[bestIdx];
                }
            }
        }

        uint8_t removed[maxM];

        for (uint64_t m = 0; m < M; m++)
        {
            removed[m] = 0;
            uint64_t move = problem.moves[m];
            if (problem.owners[move] == m) continue;
            for (uint64_t m2 = 0; m2 < M; m2++)
            {
                if (m2 == m) continue;
                if (problem.moves[m2] == move)
                {
                    removed[m] = 1;
                    break;
                }
            }
        }

        for (uint64_t m = 0; m < M; m++)
        {
            if (removed[m]) continue;
            uint64_t move = problem.moves[m];
            uint64_t owner = problem.owners[move];
            if (owner == m)
            {
                if (problem.levels[move] < U)
                {
                    problem.levels[move]++;
                }
            }
            else if (owner == 255)
            {
                problem.owners[move] = m;
                problem.levels[move] = 1;
            }
            else
            {
                if (problem.levels[move] > 1)
                {
                    problem.levels[move]--;
                    continue;
                }
                problem.owners[move] = m;
            }
            problem.pieces[m] = problem.moves[m];
        }
    }

    uint64_t scores[maxM];
    for (uint64_t m = 0; m < M; m++)
    {
        scores[m] = 0;
    }

    for (uint64_t coord = 0; coord < N * N; coord++)
    {
        uint64_t owner = problem.owners[coord];
        if (owner == 255) continue;
        uint64_t value = problem.values[coord];
        uint64_t level = problem.levels[coord];
        scores[owner] += value * level;
    }

    uint64_t myScore = scores[0];
    uint64_t bestScore = 0;
    for (uint64_t m = 1; m < M; m++)
    {
        bestScore = std::max(bestScore, scores[m]);
    }

    uint64_t score = (uint64_t)(std::log2(1.0 + (double)myScore / (double)bestScore) * 100000.0 + 0.5);

    if (pDurationMS) *pDurationMS = durationMS;
    return score;
}

class TrivialSolver : public Solver
{
public:
    virtual uint64_t step(const Problem& problem) override
    {
        return problem.pieces[0];
    }
};

class RandomSolver : public Solver
{
public:
    virtual void init(const Problem& problem) override
    {
#if 1
        {
            uint64_t timeMS = getTimeMS();
            uint32_t pid = getProcessId();
            uint32_t seed0 = 0x40928ec6;
            uint32_t seed1 = (uint32_t)timeMS;
            uint32_t seed2 = (uint32_t)pid ^ (uint32_t)(timeMS >> 32);
            std::seed_seq seq{seed0, seed1, seed2};
            gen.seed(seq);
        }
#endif
#if 0
        {
            uint32_t seed0 = 0x40928ec6;
            std::seed_seq seq{seed0};
            gen.seed(seq);
        }
#endif
    }

    virtual uint64_t step(const Problem& problem) override
    {
        uint8_t visited[N * N];
        uint8_t q[N * N];

        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            visited[coord] = 0;
        }

        uint64_t m = 0;

        uint64_t pieceCoord = problem.pieces[m];
        visited[pieceCoord] = 1;
        q[0] = pieceCoord;
        uint64_t qSize = 1;

        for (uint64_t qIdx = 0; qIdx < qSize; qIdx++)
        {
            uint64_t coord = q[qIdx];
            uint64_t owner = problem.owners[coord];
            if (owner != m) continue;

            uint64_t dirs = coordToDirs[coord];
            for (uint64_t dir = 0; dir < 4; dir++)
            {
                if (!(dirs & ((uint64_t)1 << dir))) continue;
                uint64_t ncoord = coord + dirToDelta[dir];
                if (visited[ncoord]) continue;
                uint64_t nowner = problem.owners[ncoord];
                if (nowner != m && nowner != 255 && problem.pieces[nowner] == ncoord) continue;
                visited[ncoord] = 1;
                q[qSize++] = ncoord;
            }
        }

        uint64_t moveIdx = fastRandom(qSize, gen);
        return q[moveIdx];
    }

private:
    Gen gen;
};

class GreedySolver : public Solver
{
public:
    class Player
    {
    public:
        uint8_t grid[N * N];
        uint8_t moves[N * N];
        uint64_t moveCount;
        uint32_t bestValues[4];
        uint8_t bestCounts[4];
        double probGrid[N * N];
        uint64_t score;
        double epsSum;
        double eps;
        double weightSums[4];
        uint8_t weightCounts[4];
        double weights[4];
    };

    virtual void init(const Problem& problem) override
    {
#if 0
        {
            uint64_t timeMS = getTimeMS();
            uint32_t pid = getProcessId();
            uint32_t seed0 = 0x40928ec6;
            uint32_t seed1 = (uint32_t)timeMS;
            uint32_t seed2 = (uint32_t)pid ^ (uint32_t)(timeMS >> 32);
            std::seed_seq seq{seed0, seed1, seed2};
            gen.seed(seq);
        }
#endif
#if 1
        {
            uint32_t seed0 = 0x40928ec6;
            std::seed_seq seq{seed0};
            gen.seed(seq);
        }
#endif
    }

    virtual uint64_t step(const Problem& problem) override
    {
        uint64_t M = problem.M;
        uint64_t U = problem.U;

        if (problem.turn == 0)
        {
            for (uint64_t m = 1; m < M; m++)
            {
                auto& player = players[m];
                player.epsSum = 0.3;
                player.eps = 0.3;
                for (uint64_t kind = 0; kind < 4; kind++)
                {
                    player.weightSums[kind] = 0.65;
                }
                for (uint64_t kind = 0; kind < 4; kind++)
                {
                    player.weightCounts[kind] = 1;
                }
                for (uint64_t kind = 0; kind < 4; kind++)
                {
                    player.weights[kind] = 0.65;
                }
            }
        }
        else
        {
            for (uint64_t m = 1; m < M; m++)
            {
                auto& player = players[m];
                uint64_t coord = problem.moves[m];
                uint64_t kind = player.grid[coord];
                uint32_t value = problem.values[coord];
                bool isBest = (kind < 4 && value == player.bestValues[kind]);
                if (!isBest)
                {
                    uint64_t moveCount = player.moveCount;
                    uint64_t nonBestCount = moveCount;
                    nonBestCount -= player.bestCounts[0];
                    nonBestCount -= player.bestCounts[1];
                    nonBestCount -= player.bestCounts[2];
                    nonBestCount -= player.bestCounts[3];
                    player.epsSum += (double)moveCount / (double)nonBestCount;
                }
                double eps = player.epsSum / (double)(problem.turn + 1);
                if (eps < 0.1) eps = 0.1;
                else if (eps > 0.5) eps = 0.5;
                player.eps = eps;
                if (isBest)
                {
                    for (uint64_t kind2 = 0; kind2 < 4; kind2++)
                    {
                        if (player.bestCounts[kind2] == 0) continue;
                        player.weightCounts[kind2]++;
                        double curWeight = 1000.0 / (double)player.bestValues[kind2];
                        if (kind2 != kind) curWeight *= 0.3;
                        player.weightSums[kind2] += curWeight;
                        player.weights[kind2] = player.weightSums[kind2] / (double)player.weightCounts[kind2];
                    }
                }
            }
        }

        for (uint64_t m = 0; m < M; m++)
        {
            auto& player = players[m];

            for (uint64_t coord = 0; coord < N * N; coord++)
            {
                player.grid[coord] = 255;
            }

            for (uint64_t kind = 0; kind < 4; kind++)
            {
                player.bestValues[kind] = 0;
            }
            for (uint64_t kind = 0; kind < 4; kind++)
            {
                player.bestCounts[kind] = 0;
            }

            player.score = 0;

            uint64_t startCoord = problem.pieces[m];
            player.grid[startCoord] = 0;
            player.moves[0] = startCoord;
            uint64_t moveCount = 1;

            for (uint64_t moveIdx = 0; moveIdx < moveCount; moveIdx++)
            {
                uint64_t coord = player.moves[moveIdx];
                uint64_t owner = problem.owners[coord];

                if (m != 0)
                {
                    uint8_t kind;
                    if (owner == 255)
                    {
                        kind = 0;
                    }
                    else if (owner == m)
                    {
                        if (problem.levels[coord] < U)
                        {
                            kind = 1;
                        }
                        else
                        {
                            kind = 4;
                        }
                    }
                    else
                    {
                        if (problem.levels[coord] == 1)
                        {
                            kind = 2;
                        }
                        else
                        {
                            kind = 3;
                        }
                    }
                    player.grid[coord] = kind;

                    if (kind < 4)
                    {
                        uint32_t value = problem.values[coord];
                        if (value > player.bestValues[kind])
                        {
                            player.bestValues[kind] = value;
                            player.bestCounts[kind] = 1;
                        }
                        else if (value == player.bestValues[kind])
                        {
                            player.bestCounts[kind]++;
                        }
                    }
                }

                if (owner != m) continue;

                player.score += problem.values[coord] * problem.levels[coord];

                uint8_t dirs = coordToDirs[coord];
                for (uint64_t dir = 0; dir < 4; dir++)
                {
                    uint8_t dirBit = (uint8_t)1 << dir;
                    if ((dirs & dirBit) == 0) continue;
                    uint64_t ncoord = coord + dirToDelta[dir];
                    if (player.grid[ncoord] != 255) continue;
                    uint64_t nowner = problem.owners[ncoord];
                    if (nowner != m && nowner != 255 && problem.pieces[nowner] == ncoord) continue;
                    player.grid[ncoord] = 0;
                    player.moves[moveCount++] = ncoord;
                }
            }

            player.moveCount = moveCount;
        }

        for (uint64_t m = 1; m < M; m++)
        {
            auto& player = players[m];

            for (uint64_t coord = 0; coord < N * N; coord++)
            {
                player.probGrid[coord] = 0.0;
            }

            uint64_t kindCount =
                (player.bestCounts[0] != 0) +
                (player.bestCounts[1] != 0) +
                (player.bestCounts[2] != 0) +
                (player.bestCounts[3] != 0);

            uint64_t moveCount = player.moveCount;
            double randProb = 1.0 / (double)moveCount;
            if (kindCount != 0)
            {
                randProb *= player.eps;
            }

            double weightedValues[4];
            double totalWeightedValue = 0.0;
            for (uint64_t kind = 0; kind < 4; kind++)
            {
                double weight = player.weights[kind];
                double value = (double)player.bestValues[kind];
                double weightedValue = weight * value * value;
                weightedValues[kind] = weightedValue;
                totalWeightedValue += weightedValue;
            }

            double kindProbs[4];
            for (uint64_t kind = 0; kind < 4; kind++)
            {
                double weightedValue = weightedValues[kind];
                kindProbs[kind] = (1.0 - player.eps) * weightedValue / (totalWeightedValue * (double)player.bestCounts[kind]);
            }

            for (uint64_t moveIdx = 0; moveIdx < moveCount; moveIdx++)
            {
                uint64_t coord = player.moves[moveIdx];
                uint8_t kind = player.grid[coord];
                double prob = randProb;
                if (kind < 4 && problem.values[coord] == player.bestValues[kind])
                {
                    prob += kindProbs[kind];
                }

                player.probGrid[coord] = prob;
            }
        }

        double totalDefendScore = 0.0;
        double defendScores[N * N];
        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            double defendScore = 0.0;
            if (problem.owners[coord] == 0)
            {
                double prob = 0.0;
                for (uint64_t m = 1; m < M; m++)
                {
                    auto& player = players[m];
                    double curProb = player.probGrid[coord];
                    for (uint64_t m2 = 1; m2 < M; m2++)
                    {
                        if (m2 == m) continue;
                        auto& player2 = players[m2];
                        curProb *= 1.0 - player2.probGrid[coord];
                    }
                    prob += curProb;
                }
                defendScore = prob * (double)problem.values[coord];
                defendScore *= 0.6;
            }
            defendScores[coord] = defendScore;
            totalDefendScore += defendScore;
        }

        uint64_t attackM = 1;
        uint64_t attackBestScore = 0;
        for (uint64_t m = 1; m < M; m++)
        {
            auto& player = players[m];
            uint64_t score = player.score;
            if (score > attackBestScore)
            {
                attackBestScore = score;
                attackM = m;
            }
        }

        auto& player0 = players[0];
        uint64_t moveCount = player0.moveCount;
        double bestScore = -1.0e100;
        uint64_t bestCount = 0;
        uint64_t bestMove = 0;
        for (uint64_t moveIdx = 0; moveIdx < moveCount; moveIdx++)
        {
            uint64_t coord = player0.moves[moveIdx];
            uint64_t owner = problem.owners[coord];
            uint64_t level = problem.levels[coord];
            uint32_t value = problem.values[coord];
            double score = (double)player0.score;
            score -= totalDefendScore - defendScores[coord];
            if (owner == 0)
            {
                if (level < U)
                {
                    score += (double)value;
                    //score += 1000.0;
                }
            }
            else if (level <= 1)
            {
                double prob = 1.0;
                for (uint64_t m = 1; m < M; m++)
                {
                    auto& player = players[m];
                    prob *= 1.0 - player.probGrid[coord];
                }
                if (owner == 255)
                {
                    //score += (double)value * prob;
                    score += 1000.0 * prob;
                }
                else
                {
                    //score += (double)value * prob;
                    score += 1000.0 * prob;
                }
            }

            double attackScore = (double)attackBestScore;
            if (owner == attackM)
            {
                double prob = 0.4;
                for (uint64_t m = 1; m < M; m++)
                {
                    auto& player = players[m];
                    prob *= 1.0 - player.probGrid[coord];
                }
                //attackScore -= prob * (double)value;
                attackScore -= prob * 1000.0;
            }

            score /= attackScore;

            if (score > bestScore)
            {
                bestScore = score;
                bestCount = 1;
                bestMove = coord;
            }
            else if (score == bestScore)
            {
                bestCount++;
                if (fastRandom(bestCount, gen) == 0)
                {
                    bestMove = coord;
                }
            }
        }

        return bestMove;
    }

private:
    Gen gen;
    Player players[maxM];
};

class TwoPlayerSolver : public Solver
{
public:
    static constexpr const uint64_t M = 2;

    class State
    {
    public:
        uint8_t owners[N * N];
        uint8_t levels[N * N];
        uint8_t pieces[M];
        uint32_t scores[M];
    };

    class Moves
    {
    public:
        uint64_t moveCount0;
        uint64_t moveCount1;
        uint8_t moves0[N * N];
        uint8_t moves1[N * N];
        double probs[N * N];
    };

    virtual void init(const Problem& problem) override
    {
        startTimeMS = getTimeMS();

#if 1
        {
            uint64_t timeMS = startTimeMS;
            uint32_t pid = getProcessId();
            uint32_t seed0 = 0x40928ec6;
            uint32_t seed1 = (uint32_t)timeMS;
            uint32_t seed2 = (uint32_t)pid ^ (uint32_t)(timeMS >> 32);
            std::seed_seq seq{seed0, seed1, seed2};
            gen.seed(seq);
        }
#endif
#if 0
        {
            uint32_t seed0 = 0x40928ec6;
            std::seed_seq seq{seed0};
            gen.seed(seq);
        }
#endif

        log2Table.reset(new double[500001]);
        log2Table[0] = 0.0;
        for (uint32_t i = 1; i <= 500000; i++)
        {
            log2Table[i] = __builtin_log2(i) * 100000.0;
        }

        uint64_t endTimeMS = getTimeMS();
        elapsedMS = endTimeMS - startTimeMS;
    }

    void getMoves(const Problem& problem, Moves& moves)
    {
        uint64_t U = problem.U;

        uint8_t grid[N * N];
        uint8_t q[N * N];

        {
            for (uint64_t coord = 0; coord < N * N; coord++)
            {
                grid[coord] = 255;
            }

            uint64_t startCoord = state.pieces[0];
            grid[startCoord] = 0;
            q[0] = startCoord;
            uint64_t qSize = 1;

            uint64_t moveCount0 = 0;

            if (state.levels[startCoord] != U)
            {
                moves.moves0[moveCount0++] = startCoord;
            }

            for (uint64_t qIdx = 0; qIdx < qSize; qIdx++)
            {
                uint64_t coord = q[qIdx];
                uint8_t dirs = coordToDirs[coord];
                for (uint64_t dir = 0; dir < 4; dir++)
                {
                    uint8_t dirBit = (uint8_t)1 << dir;
                    if ((dirs & dirBit) == 0) continue;
                    uint64_t ncoord = coord + dirToDelta[dir];
                    if (ncoord == state.pieces[1]) continue;
                    if (grid[ncoord] != 255) continue;
                    grid[ncoord] = 0;
                    uint64_t nowner = state.owners[ncoord];
                    if (nowner == 0) q[qSize++] = ncoord;
                    if (nowner != 0 || state.levels[ncoord] != U)
                    {
                        moves.moves0[moveCount0++] = ncoord;
                    }
                }
            }

            //if (moveCount0 == 0) moves.moves0[moveCount0++] = startCoord;

            moves.moveCount0 = moveCount0;
        }

        {
            uint32_t bestValues[4] = { 0, 0, 0, 0 };
            uint8_t bestCounts[4] = { 0, 0, 0, 0 };

            for (uint64_t coord = 0; coord < N * N; coord++)
            {
                grid[coord] = 255;
            }

            uint64_t startCoord = state.pieces[1];
            grid[startCoord] = 0;
            q[0] = startCoord;
            uint64_t qSize = 1;

            uint64_t moveCount1 = 0;

            for (uint64_t qIdx = 0; qIdx < qSize; qIdx++)
            {
                uint64_t coord = q[qIdx];
                uint64_t owner = state.owners[coord];

                uint8_t kind;
                if (owner == 255)
                {
                    kind = 0;
                }
                else if (owner == 1)
                {
                    if (state.levels[coord] < U)
                    {
                        kind = 1;
                    }
                    else
                    {
                        kind = 4;
                    }
                }
                else
                {
                    if (state.levels[coord] == 1)
                    {
                        kind = 2;
                    }
                    else
                    {
                        kind = 3;
                    }
                }
                grid[coord] = kind;

                if (kind < 4)
                {
                    uint32_t value = problem.values[coord];
                    if (value > bestValues[kind])
                    {
                        bestValues[kind] = value;
                        bestCounts[kind] = 1;
                    }
                    else if (value == bestValues[kind])
                    {
                        bestCounts[kind]++;
                    }
                }

                if (owner != 1) continue;

                uint8_t dirs = coordToDirs[coord];
                for (uint64_t dir = 0; dir < 4; dir++)
                {
                    uint8_t dirBit = (uint8_t)1 << dir;
                    if ((dirs & dirBit) == 0) continue;
                    uint64_t ncoord = coord + dirToDelta[dir];
                    if (ncoord == state.pieces[0]) continue;
                    if (grid[ncoord] != 255) continue;
                    grid[ncoord] = 0;
                    q[qSize++] = ncoord;
                }
            }

            double doubleBestValues[4];
            double recipBestValues[4];
            for (uint64_t kind = 0; kind < 4; kind++)
            {
                if (bestCounts[kind] == 0) continue;
                doubleBestValues[kind] = (double)bestValues[kind];
                recipBestValues[kind] = 1.0 / doubleBestValues[kind];
            }

            double kindProbs[4];
            double totalKindProb = 0.0;
            for (uint64_t kind = 0; kind < 4; kind++)
            {
                if (bestCounts[kind] == 0)
                {
                    kindProbs[kind] = 0.0;
                    continue;
                }

                double prob = 0.0;
                for (double w = 0.325; w < 1.0; w += 0.05)
                {
                    double curProb = 1.0;
                    for (uint64_t kind2 = 0; kind2 < 4; kind2++)
                    {
                        if (kind2 == kind) continue;
                        if (bestCounts[kind2] == 0) continue;
                        double w2 = w * doubleBestValues[kind] * recipBestValues[kind2];
                        if (w2 < 0.3) w2 = 0.3;
                        else if (w2 > 1.0) w2 = 1.0;
                        curProb *= (w2 - 0.3) * (1.0 / 0.7);
                    }
                    prob += curProb;
                }
                prob *= (1.0 / 14.0);
                kindProbs[kind] = prob;
                totalKindProb += prob;
            }
            for (uint64_t kind = 0; kind < 4; kind++)
            {
                if (bestCounts[kind] == 0) continue;
                kindProbs[kind] /= totalKindProb * (double)bestCounts[kind];
            }

            for (uint64_t qIdx = 0; qIdx < qSize; qIdx++)
            {
                uint64_t coord = q[qIdx];
                uint8_t kind = grid[coord];
                if (kind < 4 && problem.values[coord] == bestValues[kind])
                {
                    moves.moves1[moveCount1] = coord;
                    moves.probs[moveCount1] = kindProbs[kind];
                    moveCount1++;
                }
            }

#if 0
            if (moveCount1 == 0)
            {
                moves.moves1[moveCount1] = startCoord;
                moves.probs[moveCount1] = 1.0;
                moveCount1++;
            }
#endif

            moves.moveCount1 = moveCount1;
        }
    }

    double search(const Problem& problem, uint64_t depth)
    {
        depth--;

        uint64_t U = problem.U;

        Moves moves;
        getMoves(problem, moves);

        uint64_t moveCount0 = moves.moveCount0;
        uint64_t moveCount1 = moves.moveCount1;

        if (depth == 0)
        {
            double bestScore = -1.0e100;
            for (uint64_t moveIdx0 = 0; moveIdx0 < moveCount0; moveIdx0++)
            {
                uint64_t coord0 = moves.moves0[moveIdx0];

                double score = 0.0;

                for (uint64_t moveIdx1 = 0; moveIdx1 < moveCount1; moveIdx1++)
                {
                    uint64_t coord1 = moves.moves1[moveIdx1];

                    uint8_t oldOwner0 = state.owners[coord0];
                    uint8_t oldOwner1 = state.owners[coord1];
                    uint8_t oldLevel0 = state.levels[coord0];
                    uint8_t oldLevel1 = state.levels[coord1];

                    uint32_t score0 = state.scores[0];
                    uint32_t score1 = state.scores[1];

                    uint32_t value0 = problem.values[coord0];
                    uint32_t value1 = problem.values[coord1];

                    if (oldOwner0 == 0)
                    {
                        if (oldLevel0 < U)
                        {
                            score0 += value0;
                        }
                    }
                    else if (coord0 != coord1)
                    {
                        if (oldOwner0 == 255)
                        {
                            score0 += value0;
                        }
                        else
                        {
                            score1 -= value0;
                            if (oldLevel0 == 1)
                            {
                                score0 += value0;
                            }
                        }
                    }

                    if (oldOwner1 == 1)
                    {
                        if (oldLevel1 < U)
                        {
                            score1 += value1;
                        }
                    }
                    else if (coord0 != coord1)
                    {
                        if (oldOwner1 == 255)
                        {
                            score1 += value1;
                        }
                        else
                        {
                            score0 -= value1;
                            if (oldLevel1 == 1)
                            {
                                score1 += value1;
                            }
                        }
                    }

                    double curScore = log2Table[score0 + score1] - log2Table[score1];

                    score += curScore * moves.probs[moveIdx1];

                    evalCount++;
                    if ((evalCount & 0xffff) == 0)
                    {
                        uint64_t durationMS = getTimeMS() - startTimeMS;
                        if (durationMS >= limitDurationMS)
                        {
                            timeout = true;
                        }
                    }
                }

                if (score > bestScore)
                {
                    bestScore = score;
                }
            }

            return bestScore;
        }
        else
        {
            uint8_t oldPiece0 = state.pieces[0];
            uint8_t oldPiece1 = state.pieces[1];
            uint32_t oldScore0 = state.scores[0];
            uint32_t oldScore1 = state.scores[1];

            double bestScore = -1.0e100;
            for (uint64_t moveIdx0 = 0; moveIdx0 < moveCount0; moveIdx0++)
            {
                if (timeout) break;

                uint64_t coord0 = moves.moves0[moveIdx0];

                double score = 0.0;

                for (uint64_t moveIdx1 = 0; moveIdx1 < moveCount1; moveIdx1++)
                {
                    uint64_t coord1 = moves.moves1[moveIdx1];

                    uint8_t oldOwner0 = state.owners[coord0];
                    uint8_t oldOwner1 = state.owners[coord1];
                    uint8_t oldLevel0 = state.levels[coord0];
                    uint8_t oldLevel1 = state.levels[coord1];

                    state.pieces[0] = coord0;
                    state.pieces[1] = coord1;
                    state.scores[0] = oldScore0;
                    state.scores[1] = oldScore1;

                    uint32_t value0 = problem.values[coord0];
                    uint32_t value1 = problem.values[coord1];

                    if (oldOwner0 == 0)
                    {
                        if (oldLevel0 < U)
                        {
                            state.levels[coord0] = oldLevel0 + 1;
                            state.scores[0] += value0;
                        }
                    }
                    else if (coord0 != coord1)
                    {
                        if (oldOwner0 == 255)
                        {
                            state.owners[coord0] = 0;
                            state.levels[coord0] = 1;
                            state.scores[0] += value0;
                        }
                        else
                        {
                            state.scores[1] -= value0;
                            if (oldLevel0 == 1)
                            {
                                state.owners[coord0] = 0;
                                state.scores[0] += value0;
                            }
                            else
                            {
                                state.levels[coord0] = oldLevel0 - 1;
                            }
                        }
                    }

                    if (oldOwner1 == 1)
                    {
                        if (oldLevel1 < U)
                        {
                            state.levels[coord1] = oldLevel1 + 1;
                            state.scores[1] += value1;
                        }
                    }
                    else if (coord0 != coord1)
                    {
                        if (oldOwner1 == 255)
                        {
                            state.owners[coord1] = 1;
                            state.levels[coord1] = 1;
                            state.scores[1] += value1;
                        }
                        else
                        {
                            state.scores[0] -= value1;
                            if (oldLevel1 == 1)
                            {
                                state.owners[coord1] = 1;
                                state.scores[1] += value1;
                            }
                            else
                            {
                                state.levels[coord1] = oldLevel1 - 1;
                            }
                        }
                    }

                    if (coord0 == coord1)
                    {
                        if (oldOwner0 != 0) state.pieces[0] = oldPiece0;
                        if (oldOwner0 != 1) state.pieces[1] = oldPiece1;
                    }

                    double curScore = search(problem, depth);

                    score += curScore * moves.probs[moveIdx1];

                    state.levels[coord1] = oldLevel1;
                    state.levels[coord0] = oldLevel0;
                    state.owners[coord1] = oldOwner1;
                    state.owners[coord0] = oldOwner0;
                }

                if (score > bestScore)
                {
                    bestScore = score;
                }
            }

            state.scores[1] = oldScore1;
            state.scores[0] = oldScore0;
            state.pieces[1] = oldPiece1;
            state.pieces[0] = oldPiece0;

            return bestScore;
        }
    }

    virtual uint64_t step(const Problem& problem) override
    {
        startTimeMS = getTimeMS();

        uint64_t U = problem.U;

        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            state.owners[coord] = problem.owners[coord];
        }
        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            state.levels[coord] = problem.levels[coord];
        }
        for (uint64_t m = 0; m < M; m++)
        {
            state.pieces[m] = problem.pieces[m];
        }
        for (uint64_t m = 0; m < M; m++)
        {
            state.scores[m] = 0;
        }
        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            uint8_t owner = state.owners[coord];
            if (owner > 1) continue;
            uint32_t value = problem.values[coord];
            uint8_t level = state.levels[coord];
            state.scores[owner] += value * level;
        }

        Moves moves;
        getMoves(problem, moves);

        uint64_t moveCount0 = moves.moveCount0;
        uint64_t moveCount1 = moves.moveCount1;

#if 0
        printf("================================\n");
        for (uint64_t moveIdx1 = 0; moveIdx1 < moveCount1; moveIdx1++)
        {
            printf("move[%" PRIu64 "], %2" PRIu8 ", %8.6f\n", moveIdx1, moves.moves1[moveIdx1], moves.probs[moveIdx1]);
        }
#endif

        fastShuffle(&moves.moves0[0], &moves.moves0[0] + moveCount0, gen);

        uint32_t moveScores[N * N];

        uint64_t maxDepth = 5;
        maxDepth = std::min(maxDepth, T - problem.turn);

        uint64_t stopDurationMS = 1800;
#ifdef LOCAL
        stopDurationMS /= 2;
#endif

        limitDurationMS = (elapsedMS < stopDurationMS) ? (stopDurationMS - elapsedMS) / (T - problem.turn) : (uint64_t)0;
        evalCount = 0;
        timeout = false;

        uint64_t depth = 1;
        //uint64_t moveDepth = 0;
        uint64_t bestMove = 255;

        for (; depth <= maxDepth; depth++)
        {
            uint8_t oldPiece0 = state.pieces[0];
            uint8_t oldPiece1 = state.pieces[1];
            uint32_t oldScore0 = state.scores[0];
            uint32_t oldScore1 = state.scores[1];

            uint32_t bestMoveScore = 0;
            for (uint64_t moveIdx0 = 0; moveIdx0 < moveCount0; moveIdx0++)
            {
                if (timeout) break;

                uint64_t coord0 = moves.moves0[moveIdx0];

                double score = 0.0;

                for (uint64_t moveIdx1 = 0; moveIdx1 < moveCount1; moveIdx1++)
                {
                    uint64_t coord1 = moves.moves1[moveIdx1];

                    uint8_t oldOwner0 = state.owners[coord0];
                    uint8_t oldOwner1 = state.owners[coord1];
                    uint8_t oldLevel0 = state.levels[coord0];
                    uint8_t oldLevel1 = state.levels[coord1];

                    state.pieces[0] = coord0;
                    state.pieces[1] = coord1;
                    state.scores[0] = oldScore0;
                    state.scores[1] = oldScore1;

                    uint32_t value0 = problem.values[coord0];
                    uint32_t value1 = problem.values[coord1];

                    if (oldOwner0 == 0)
                    {
                        if (oldLevel0 < U)
                        {
                            state.levels[coord0] = oldLevel0 + 1;
                            state.scores[0] += value0;
                        }
                    }
                    else if (coord0 != coord1)
                    {
                        if (oldOwner0 == 255)
                        {
                            state.owners[coord0] = 0;
                            state.levels[coord0] = 1;
                            state.scores[0] += value0;
                        }
                        else
                        {
                            state.scores[1] -= value0;
                            if (oldLevel0 == 1)
                            {
                                state.owners[coord0] = 0;
                                state.scores[0] += value0;
                            }
                            else
                            {
                                state.levels[coord0] = oldLevel0 - 1;
                            }
                        }
                    }

                    if (oldOwner1 == 1)
                    {
                        if (oldLevel1 < U)
                        {
                            state.levels[coord1] = oldLevel1 + 1;
                            state.scores[1] += value1;
                        }
                    }
                    else if (coord0 != coord1)
                    {
                        if (oldOwner1 == 255)
                        {
                            state.owners[coord1] = 1;
                            state.levels[coord1] = 1;
                            state.scores[1] += value1;
                        }
                        else
                        {
                            state.scores[0] -= value1;
                            if (oldLevel1 == 1)
                            {
                                state.owners[coord1] = 1;
                                state.scores[1] += value1;
                            }
                            else
                            {
                                state.levels[coord1] = oldLevel1 - 1;
                            }
                        }
                    }

                    if (coord0 == coord1)
                    {
                        if (oldOwner0 != 0) state.pieces[0] = oldPiece0;
                        if (oldOwner0 != 1) state.pieces[1] = oldPiece1;
                    }

                    double curScore;
                    if (depth == 1)
                    {
                        curScore = log2Table[state.scores[0] + state.scores[1]] - log2Table[state.scores[1]];
                    }
                    else
                    {
                        curScore = search(problem, depth - 1);
                    }

                    score += curScore * moves.probs[moveIdx1];

                    state.levels[coord1] = oldLevel1;
                    state.levels[coord0] = oldLevel0;
                    state.owners[coord1] = oldOwner1;
                    state.owners[coord0] = oldOwner0;
                }

                uint32_t moveScore = (uint32_t)(score + 0.5);
                moveScores[moveIdx0] = moveScore;

                if (moveIdx0 == 0 || moveScore > bestMoveScore)
                {
                    bestMoveScore = moveScore;
                    if (bestMove != coord0)
                    {
                        bestMove = coord0;
                        //moveDepth = depth;
                    }
                }
            }

            if (timeout)
            {
                depth--;
                break;
            }

            if (depth == maxDepth)
            {
                break;
            }

            state.scores[1] = oldScore1;
            state.scores[0] = oldScore0;
            state.pieces[1] = oldPiece1;
            state.pieces[0] = oldPiece0;

            uint8_t order[N * N];
            for (uint64_t moveIdx0 = 0; moveIdx0 < moveCount0; moveIdx0++)
            {
                order[moveIdx0] = moveIdx0;
            }

            auto moveComp = [&](uint8_t idxA, uint8_t idxB) -> bool
            {
                uint32_t scoreA = moveScores[idxA];
                uint32_t scoreB = moveScores[idxB];
                if (scoreA != scoreB) return scoreA > scoreB;
                return idxA < idxB;
            };

            std::sort(&order[0], &order[0] + moveCount0, moveComp);

            //uint32_t bestMoveScore = moveScores[order[0]];

            for (uint64_t moveIdx0 = 0; moveIdx0 < moveCount0; moveIdx0++)
            {
                order[moveIdx0] = moves.moves0[order[moveIdx0]];
            }

            for (uint64_t moveIdx0 = 0; moveIdx0 < moveCount0; moveIdx0++)
            {
                moves.moves0[moveIdx0] = order[moveIdx0];
            }
        }

        //printf("depth = %" PRIu64 ", moveDepth = %" PRIu64 ", move = %2" PRIu64 "\n", depth, moveDepth, bestMove);

        uint64_t endTimeMS = getTimeMS();
        elapsedMS += endTimeMS - startTimeMS;

        return bestMove;
    }

private:
    Gen gen;
    unique_ptr<double[]> log2Table;
    uint64_t elapsedMS;
    uint64_t startTimeMS;
    uint64_t limitDurationMS;
    uint64_t evalCount;
    bool timeout;
    State state;
};

class MultiPlayerSolver : public Solver
{
public:
    class State
    {
    public:
        uint8_t owners[N * N];
        uint8_t levels[N * N];
        uint8_t pieces[maxM];
        uint32_t scores[maxM];
    };

    static constexpr const uint64_t maxEnemyMoveCount = 4;

    class Moves
    {
    public:
        uint64_t myMoveCount;
        uint64_t enemyMoveCount;
        uint8_t myMoves[N * N];
        uint8_t enemyMoves[maxEnemyMoveCount][maxM - 1];
        double probs[maxEnemyMoveCount];
    };

    virtual void init(const Problem& problem) override
    {
        startTimeMS = getTimeMS();

#if 1
        {
            uint64_t timeMS = startTimeMS;
            uint32_t pid = getProcessId();
            uint32_t seed0 = 0x40928ec6;
            uint32_t seed1 = (uint32_t)timeMS;
            uint32_t seed2 = (uint32_t)pid ^ (uint32_t)(timeMS >> 32);
            std::seed_seq seq{seed0, seed1, seed2};
            gen.seed(seq);
        }
#endif
#if 0
        {
            uint32_t seed0 = 0x40928ec6;
            std::seed_seq seq{seed0};
            gen.seed(seq);
        }
#endif

        log2Table.reset(new double[500001]);
        log2Table[0] = 0.0;
        for (uint32_t i = 1; i <= 500000; i++)
        {
            log2Table[i] = __builtin_log2(i) * 100000.0;
        }

        uint64_t endTimeMS = getTimeMS();
        elapsedMS = endTimeMS - startTimeMS;
    }

    void getMoves(const Problem& problem,
                  Moves& moves,
                  bool updateFakeValues = false)
    {
        uint64_t M = problem.M;
        uint64_t U = problem.U;

        uint8_t grid[N * N];
        uint8_t q[N * N];

        {
            for (uint64_t coord = 0; coord < N * N; coord++)
            {
                grid[coord] = 255;
            }

            uint64_t startCoord = state.pieces[0];
            grid[startCoord] = 0;
            q[0] = startCoord;
            uint64_t qSize = 1;

            uint64_t myMoveCount = 0;

            if (state.levels[startCoord] != U)
            {
                moves.myMoves[myMoveCount++] = startCoord;
            }

            for (uint64_t qIdx = 0; qIdx < qSize; qIdx++)
            {
                uint64_t coord = q[qIdx];
                uint8_t dirs = coordToDirs[coord];
                for (uint64_t dir = 0; dir < 4; dir++)
                {
                    uint8_t dirBit = (uint8_t)1 << dir;
                    if ((dirs & dirBit) == 0) continue;
                    uint64_t ncoord = coord + dirToDelta[dir];
                    if (grid[ncoord] != 255) continue;
                    grid[ncoord] = 0;
                    uint64_t nowner = state.owners[ncoord];
                    if (nowner != 255 && ncoord == state.pieces[nowner]) continue;
                    if (nowner == 0) q[qSize++] = ncoord;
                    if (nowner != 0 || state.levels[ncoord] != U)
                    {
                        moves.myMoves[myMoveCount++] = ncoord;
                    }
                }
            }

            if (myMoveCount == 0) moves.myMoves[myMoveCount++] = startCoord;

            moves.myMoveCount = myMoveCount;
        }

        uint8_t allMoves[maxM - 1][N * N];
        double allProbs[maxM - 1][N * N];
        uint64_t allMoveCounts[maxM - 1];

        for (uint64_t m = 1; m < M; m++)
        {
            uint32_t bestValues[4] = { 0, 0, 0, 0 };
            uint8_t bestCounts[4] = { 0, 0, 0, 0 };

            for (uint64_t coord = 0; coord < N * N; coord++)
            {
                grid[coord] = 255;
            }

            uint64_t startCoord = state.pieces[m];
            grid[startCoord] = 0;
            q[0] = startCoord;
            uint64_t qSize = 1;

            for (uint64_t qIdx = 0; qIdx < qSize; qIdx++)
            {
                uint64_t coord = q[qIdx];
                uint64_t owner = state.owners[coord];

                uint8_t kind;
                if (owner == 255)
                {
                    kind = 0;
                }
                else if (owner == m)
                {
                    if (state.levels[coord] < U)
                    {
                        kind = 1;
                    }
                    else
                    {
                        kind = 4;
                    }
                }
                else
                {
                    if (state.levels[coord] == 1)
                    {
                        kind = 2;
                    }
                    else
                    {
                        kind = 3;
                    }
                }
                grid[coord] = kind;

                if (kind < 4)
                {
                    uint32_t value = problem.values[coord];
                    if (value > bestValues[kind])
                    {
                        bestValues[kind] = value;
                        bestCounts[kind] = 1;
                    }
                    else if (value == bestValues[kind])
                    {
                        bestCounts[kind]++;
                    }
                }

                if (owner != m) continue;

                uint8_t dirs = coordToDirs[coord];
                for (uint64_t dir = 0; dir < 4; dir++)
                {
                    uint8_t dirBit = (uint8_t)1 << dir;
                    if ((dirs & dirBit) == 0) continue;
                    uint64_t ncoord = coord + dirToDelta[dir];
                    if (grid[ncoord] != 255) continue;
                    grid[ncoord] = 0;
                    uint64_t nowner = state.owners[ncoord];
                    if (nowner != 255 && ncoord == state.pieces[nowner]) continue;
                    q[qSize++] = ncoord;
                }
            }

            double doubleBestValues[4];
            double recipBestValues[4];
            for (uint64_t kind = 0; kind < 4; kind++)
            {
                if (bestCounts[kind] == 0) continue;
                doubleBestValues[kind] = (double)bestValues[kind];
                recipBestValues[kind] = 1.0 / doubleBestValues[kind];
            }

            double kindProbs[4];
            double totalKindProb = 0.0;
            for (uint64_t kind = 0; kind < 4; kind++)
            {
                if (bestCounts[kind] == 0)
                {
                    kindProbs[kind] = 0.0;
                    continue;
                }

                double prob = 0.0;
                for (double w = 0.325; w < 1.0; w += 0.05)
                {
                    double curProb = 1.0;
                    for (uint64_t kind2 = 0; kind2 < 4; kind2++)
                    {
                        if (kind2 == kind) continue;
                        if (bestCounts[kind2] == 0) continue;
                        double w2 = w * doubleBestValues[kind] * recipBestValues[kind2];
                        if (w2 < 0.3) w2 = 0.3;
                        else if (w2 > 1.0) w2 = 1.0;
                        curProb *= (w2 - 0.3) * (1.0 / 0.7);
                    }
                    prob += curProb;
                }
                prob *= (1.0 / 14.0);
                kindProbs[kind] = prob;
                totalKindProb += prob;
            }
            for (uint64_t kind = 0; kind < 4; kind++)
            {
                if (bestCounts[kind] == 0) continue;
                kindProbs[kind] /= totalKindProb * (double)bestCounts[kind];
            }

            uint64_t allMoveCount = 0;

            for (uint64_t qIdx = 0; qIdx < qSize; qIdx++)
            {
                uint64_t coord = q[qIdx];
                uint8_t kind = grid[coord];
                if (kind < 4 && problem.values[coord] == bestValues[kind])
                {
                    allMoves[m - 1][allMoveCount] = coord;
                    allProbs[m - 1][allMoveCount] = kindProbs[kind];
                    allMoveCount++;
                }
            }

            if (allMoveCount == 0)
            {
                allMoves[m - 1][allMoveCount] = startCoord;
                allProbs[m - 1][allMoveCount] = 1.0;
                allMoveCount++;
            }

            allMoveCounts[m - 1] = allMoveCount;

            if (updateFakeValues)
            {
                for (uint64_t qIdx = 0; qIdx < qSize; qIdx++)
                {
                    uint64_t coord = q[qIdx];
                    fakeValues[coord] = 1;
                }
            }
        }

        double splitProb = 1.0 / (double)maxEnemyMoveCount;
        for (uint64_t enemyMoveIdx = 0; enemyMoveIdx < maxEnemyMoveCount; enemyMoveIdx++)
        {
            for (uint64_t m = 1; m < M; m++)
            {
                uint64_t allMoveCount = allMoveCounts[m - 1];
                double bestProb = -1.0e100;
                uint64_t bestIdx = 0;
                for (uint64_t idx = 0; idx < allMoveCount; idx++)
                {
                    double prob = allProbs[m - 1][idx];
                    if (prob > bestProb)
                    {
                        bestProb = prob;
                        bestIdx = idx;
                    }
                }
                moves.enemyMoves[enemyMoveIdx][m - 1] = allMoves[m - 1][bestIdx];
                allProbs[m - 1][bestIdx] -= splitProb;
            }
            moves.probs[enemyMoveIdx] = splitProb;
        }
        moves.enemyMoveCount = maxEnemyMoveCount;
    }

    double search(const Problem& problem, uint64_t depth)
    {
        depth--;

        uint64_t M = problem.M;
        uint64_t U = problem.U;

        Moves moves;
        getMoves(problem, moves);

        uint64_t myMoveCount = moves.myMoveCount;
        uint64_t enemyMoveCount = moves.enemyMoveCount;

        if (depth == 0)
        {
            double bestScore = -1.0e100;
            for (uint64_t myMoveIdx = 0; myMoveIdx < myMoveCount; myMoveIdx++)
            {
                uint64_t coords[maxM];
                coords[0] = moves.myMoves[myMoveIdx];

                double score = 0.0;

                for (uint64_t enemyMoveIdx = 0; enemyMoveIdx < enemyMoveCount; enemyMoveIdx++)
                {
                    for (uint64_t m = 1; m < M; m++)
                    {
                        coords[m] = moves.enemyMoves[enemyMoveIdx][m - 1];
                    }

                    uint32_t scores[maxM];
                    for (uint64_t m = 0; m < M; m++)
                    {
                        scores[m] = state.scores[m];
                    }

                    uint8_t removed[maxM];

                    for (uint64_t m = 0; m < M; m++)
                    {
                        removed[m] = 0;
                        uint64_t coord = coords[m];
                        if (state.owners[coord] == m) continue;
                        for (uint64_t m2 = 0; m2 < M; m2++)
                        {
                            if (m2 == m) continue;
                            if (coords[m2] == coord)
                            {
                                removed[m] = 1;
                                break;
                            }
                        }
                    }

                    for (uint64_t m = 0; m < M; m++)
                    {
                        if (removed[m]) continue;
                        uint64_t coord = coords[m];
                        uint64_t owner = state.owners[coord];
                        if (owner == m)
                        {
                            if (state.levels[coord] < U)
                            {
                                scores[m] += fakeValues[coord];
                            }
                        }
                        else if (owner == 255)
                        {
                            scores[m] += fakeValues[coord];
                        }
                        else
                        {
                            scores[owner] -= fakeValues[coord];
                            if (state.levels[coord] == 1)
                            {
                                scores[m] += fakeValues[coord];
                            }
                        }
                    }

                    uint32_t bestEnemyScore = 0;
                    for (uint64_t m = 1; m < M; m++)
                    {
                        bestEnemyScore = std::max(bestEnemyScore, scores[m]);
                    }
                    double curScore = log2Table[scores[0] + bestEnemyScore] - log2Table[bestEnemyScore];

                    score += curScore * moves.probs[enemyMoveIdx];

                    evalCount++;
                    if ((evalCount & 0x3fff) == 0)
                    {
                        uint64_t durationMS = getTimeMS() - startTimeMS;
                        if (durationMS >= limitDurationMS)
                        {
                            timeout = true;
                        }
                    }
                }

                if (score > bestScore)
                {
                    bestScore = score;
                }
            }

            return bestScore;
        }
        else
        {
            double bestScore = -1.0e100;
            for (uint64_t myMoveIdx = 0; myMoveIdx < myMoveCount; myMoveIdx++)
            {
                if (timeout) break;

                uint64_t coords[maxM];
                coords[0] = moves.myMoves[myMoveIdx];

                double score = 0.0;

                for (uint64_t enemyMoveIdx = 0; enemyMoveIdx < enemyMoveCount; enemyMoveIdx++)
                {
                    for (uint64_t m = 1; m < M; m++)
                    {
                        coords[m] = moves.enemyMoves[enemyMoveIdx][m - 1];
                    }

                    uint8_t oldPieces[maxM];
                    uint32_t oldScores[maxM];
                    uint8_t oldOwners[maxM];
                    uint8_t oldLevels[maxM];
                    for (uint64_t m = 0; m < M; m++)
                    {
                        oldPieces[m] = state.pieces[m];
                        oldScores[m] = state.scores[m];
                        oldOwners[m] = state.owners[coords[m]];
                        oldLevels[m] = state.levels[coords[m]];
                    }

                    uint8_t removed[maxM];

                    for (uint64_t m = 0; m < M; m++)
                    {
                        removed[m] = 0;
                        uint64_t coord = coords[m];
                        if (state.owners[coord] == m) continue;
                        for (uint64_t m2 = 0; m2 < M; m2++)
                        {
                            if (m2 == m) continue;
                            if (coords[m2] == coord)
                            {
                                removed[m] = 1;
                                break;
                            }
                        }
                    }

                    for (uint64_t m = 0; m < M; m++)
                    {
                        if (removed[m]) continue;
                        uint64_t coord = coords[m];
                        uint64_t owner = state.owners[coord];
                        if (owner == m)
                        {
                            if (state.levels[coord] < U)
                            {
                                state.levels[coord]++;
                                state.scores[m] += fakeValues[coord];
                            }
                        }
                        else if (owner == 255)
                        {
                            state.owners[coord] = m;
                            state.levels[coord] = 1;
                            state.scores[m] += fakeValues[coord];
                        }
                        else
                        {
                            state.scores[owner] -= fakeValues[coord];
                            if (state.levels[coord] > 1)
                            {
                                state.levels[coord]--;
                                continue;
                            }
                            state.owners[coord] = m;
                            state.scores[m] += fakeValues[coord];
                        }
                        state.pieces[m] = coord;
                    }

                    double curScore = search(problem, depth);

                    score += curScore * moves.probs[enemyMoveIdx];

                    for (uint64_t m = 0; m < M; m++)
                    {
                        state.pieces[m] = oldPieces[m];
                        state.scores[m] = oldScores[m];
                        state.owners[coords[m]] = oldOwners[m];
                        state.levels[coords[m]] = oldLevels[m];
                    }
                }

                if (score > bestScore)
                {
                    bestScore = score;
                }
            }

            return bestScore;
        }
    }

    virtual uint64_t step(const Problem& problem) override
    {
        startTimeMS = getTimeMS();

        uint64_t M = problem.M;
        uint64_t U = problem.U;

        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            state.owners[coord] = problem.owners[coord];
        }
        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            state.levels[coord] = problem.levels[coord];
        }
        for (uint64_t m = 0; m < M; m++)
        {
            state.pieces[m] = problem.pieces[m];
        }
        for (uint64_t m = 0; m < M; m++)
        {
            state.scores[m] = 0;
        }

        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            fakeValues[coord] = 0;
        }

        Moves moves;
        getMoves(problem, moves, true);

        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            uint64_t reducedValue = 600;
            if (fakeValues[coord] != 0 && problem.values[coord] > reducedValue)
            {
                fakeValues[coord] = reducedValue;
            }
            else
            {
                fakeValues[coord] = problem.values[coord];
            }
        }

        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            uint8_t owner = state.owners[coord];
            if (owner == 255) continue;
            uint32_t value = problem.values[coord];
            uint8_t level = state.levels[coord];
            state.scores[owner] += value * level;
        }

        uint64_t myMoveCount = moves.myMoveCount;
        uint64_t enemyMoveCount = moves.enemyMoveCount;

        fastShuffle(&moves.myMoves[0], &moves.myMoves[0] + myMoveCount, gen);

        uint32_t moveScores[N * N];

        uint64_t maxDepth = 5;
        maxDepth = std::min(maxDepth, T - problem.turn);

        uint64_t stopDurationMS = 1800;
#ifdef LOCAL
        stopDurationMS /= 2;
#endif

        limitDurationMS = (elapsedMS < stopDurationMS) ? (stopDurationMS - elapsedMS) / (T - problem.turn) : (uint64_t)0;
        evalCount = 0;
        timeout = false;

        uint64_t depth = 1;
        //uint64_t moveDepth = 0;
        uint64_t bestMove = 255;

        for (; depth <= maxDepth; depth++)
        {
            uint32_t bestMoveScore = 0;
            for (uint64_t myMoveIdx = 0; myMoveIdx < myMoveCount; myMoveIdx++)
            {
                if (timeout) break;

                uint64_t coords[maxM];
                coords[0] = moves.myMoves[myMoveIdx];

                double score = 0.0;

                for (uint64_t enemyMoveIdx = 0; enemyMoveIdx < enemyMoveCount; enemyMoveIdx++)
                {
                    for (uint64_t m = 1; m < M; m++)
                    {
                        coords[m] = moves.enemyMoves[enemyMoveIdx][m - 1];
                    }

                    uint8_t oldPieces[maxM];
                    uint32_t oldScores[maxM];
                    uint8_t oldOwners[maxM];
                    uint8_t oldLevels[maxM];
                    for (uint64_t m = 0; m < M; m++)
                    {
                        oldPieces[m] = state.pieces[m];
                        oldScores[m] = state.scores[m];
                        oldOwners[m] = state.owners[coords[m]];
                        oldLevels[m] = state.levels[coords[m]];
                    }

                    uint8_t removed[maxM];

                    for (uint64_t m = 0; m < M; m++)
                    {
                        removed[m] = 0;
                        uint64_t coord = coords[m];
                        if (state.owners[coord] == m) continue;
                        for (uint64_t m2 = 0; m2 < M; m2++)
                        {
                            if (m2 == m) continue;
                            if (coords[m2] == coord)
                            {
                                removed[m] = 1;
                                break;
                            }
                        }
                    }

                    for (uint64_t m = 0; m < M; m++)
                    {
                        if (removed[m]) continue;
                        uint64_t coord = coords[m];
                        uint64_t owner = state.owners[coord];
                        if (owner == m)
                        {
                            if (state.levels[coord] < U)
                            {
                                state.levels[coord]++;
                                state.scores[m] += fakeValues[coord];
                            }
                        }
                        else if (owner == 255)
                        {
                            state.owners[coord] = m;
                            state.levels[coord] = 1;
                            state.scores[m] += fakeValues[coord];
                        }
                        else
                        {
                            state.scores[owner] -= fakeValues[coord];
                            if (state.levels[coord] > 1)
                            {
                                state.levels[coord]--;
                                continue;
                            }
                            state.owners[coord] = m;
                            state.scores[m] += fakeValues[coord];
                        }
                        state.pieces[m] = coord;
                    }

                    double curScore;
                    if (depth == 1)
                    {
                        uint32_t bestEnemyScore = 0;
                        for (uint64_t m = 1; m < M; m++)
                        {
                            bestEnemyScore = std::max(bestEnemyScore, state.scores[m]);
                        }
                        curScore = log2Table[state.scores[0] + bestEnemyScore] - log2Table[bestEnemyScore];
                    }
                    else
                    {
                        curScore = search(problem, depth - 1);
                    }

                    score += curScore * moves.probs[enemyMoveIdx];

                    for (uint64_t m = 0; m < M; m++)
                    {
                        state.pieces[m] = oldPieces[m];
                        state.scores[m] = oldScores[m];
                        state.owners[coords[m]] = oldOwners[m];
                        state.levels[coords[m]] = oldLevels[m];
                    }
                }

                uint32_t moveScore = (uint32_t)(score + 0.5);
                moveScores[myMoveIdx] = moveScore;

                if (myMoveIdx == 0 || moveScore > bestMoveScore)
                {
                    bestMoveScore = moveScore;
                    if (bestMove != coords[0])
                    {
                        bestMove = coords[0];
                        //moveDepth = depth;
                    }
                }
            }

            if (timeout)
            {
                depth--;
                break;
            }

            if (depth == maxDepth)
            {
                break;
            }

            uint8_t order[N * N];
            for (uint64_t myMoveIdx = 0; myMoveIdx < myMoveCount; myMoveIdx++)
            {
                order[myMoveIdx] = myMoveIdx;
            }

            auto moveComp = [&](uint8_t idxA, uint8_t idxB) -> bool
            {
                uint32_t scoreA = moveScores[idxA];
                uint32_t scoreB = moveScores[idxB];
                if (scoreA != scoreB) return scoreA > scoreB;
                return idxA < idxB;
            };

            std::sort(&order[0], &order[0] + myMoveCount, moveComp);

            for (uint64_t myMoveIdx = 0; myMoveIdx < myMoveCount; myMoveIdx++)
            {
                order[myMoveIdx] = moves.myMoves[order[myMoveIdx]];
            }

            for (uint64_t myMoveIdx = 0; myMoveIdx < myMoveCount; myMoveIdx++)
            {
                moves.myMoves[myMoveIdx] = order[myMoveIdx];
            }
        }

        //printf("depth = %" PRIu64 ", moveDepth = %" PRIu64 ", move = %2" PRIu64 "\n", depth, moveDepth, bestMove);

        uint64_t endTimeMS = getTimeMS();
        elapsedMS += endTimeMS - startTimeMS;

        return bestMove;
    }

private:
    Gen gen;
    unique_ptr<double[]> log2Table;
    uint64_t elapsedMS;
    uint64_t startTimeMS;
    uint64_t limitDurationMS;
    uint64_t evalCount;
    bool timeout;
    State state;
    uint32_t fakeValues[N * N];
};

class HybridSolver : public Solver
{
public:
    virtual void init(const Problem& problem) override
    {
        uint64_t M = problem.M;

        if (M == 2)
        {
            pSolver.reset(new TwoPlayerSolver);
        }
        else
        {
            pSolver.reset(new MultiPlayerSolver);
        }

        pSolver->init(problem);
    }

    virtual uint64_t step(const Problem& problem) override
    {
        return pSolver->step(problem);
    }

private:
    unique_ptr<Solver> pSolver;
};

class MctsSolver : public Solver
{
public:
    class State
    {
    public:
        uint8_t owners[N * N];
        uint8_t levels[N * N];
        uint8_t pieces[maxM];
        uint32_t scores[maxM];
    };

    class Moves
    {
    public:
        uint8_t grid[N * N];
        uint8_t moves[N * N];
        uint64_t moveCount;
        uint32_t bestValues[4];
        uint8_t bestCounts[4];
    };

    class Player
    {
    public:
        double epsSum;
        double eps;
        uint8_t weightCounts[7][7][7][7];
        uint32_t weights[4];
    };

    virtual void init(const Problem& problem) override
    {
        startTimeMS = getTimeMS();

#if 1
        {
            uint64_t timeMS = startTimeMS;
            uint32_t pid = getProcessId();
            uint32_t seed0 = 0x40928ec6;
            uint32_t seed1 = (uint32_t)timeMS;
            uint32_t seed2 = (uint32_t)pid ^ (uint32_t)(timeMS >> 32);
            std::seed_seq seq{seed0, seed1, seed2};
            gen.seed(seq);
        }
#endif
#if 0
        {
            uint32_t seed0 = 0x40928ec6;
            std::seed_seq seq{seed0};
            gen.seed(seq);
        }
#endif

        log2Table.reset(new double[500001]);
        log2Table[0] = 0.0;
        for (uint32_t i = 1; i <= 500000; i++)
        {
            log2Table[i] = __builtin_log2(i) * 100000.0;
        }

        uint64_t endTimeMS = getTimeMS();
        elapsedMS = endTimeMS - startTimeMS;
    }

#if PROFILE
    __attribute__((noinline))
#endif
    void getMyMoves(const Problem& problem,
                    const State& state,
                    Moves& moves)
    {
        uint64_t U = problem.U;

        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            moves.grid[coord] = 255;
        }

        uint64_t startCoord = state.pieces[0];
        moves.grid[startCoord] = 0;
        moves.moves[0] = startCoord;
        uint64_t allMoveCount = 1;
        uint64_t moveCount = 0;

        for (uint64_t moveIdx = 0; moveIdx < allMoveCount; moveIdx++)
        {
            uint64_t coord = moves.moves[moveIdx];
            uint64_t owner = state.owners[coord];

            if (owner != 0 || state.levels[coord] != U)
            {
                moves.moves[moveCount++] = coord;
            }

            if (owner != 0) continue;

            uint8_t dirs = coordToDirs[coord];
            for (uint64_t dir = 0; dir < 4; dir++)
            {
                uint8_t dirBit = (uint8_t)1 << dir;
                if ((dirs & dirBit) == 0) continue;
                uint64_t ncoord = coord + dirToDelta[dir];
                if (moves.grid[ncoord] != 255) continue;
                moves.grid[ncoord] = 0;
                uint64_t nowner = state.owners[ncoord];
                if (nowner != 255 && ncoord == state.pieces[nowner]) continue;
                moves.moves[allMoveCount++] = ncoord;
            }
        }

        if (moveCount == 0) moves.moves[moveCount++] = startCoord;

        moves.moveCount = moveCount;
    }

#if PROFILE
    __attribute__((noinline))
#endif
    void getEnemyMoves(const Problem& problem,
                       const State& state,
                       uint64_t m,
                       Moves& moves)
    {
        uint64_t U = problem.U;

        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            moves.grid[coord] = 255;
        }

        for (uint64_t kind = 0; kind < 4; kind++)
        {
            moves.bestValues[kind] = 0;
        }

        for (uint64_t kind = 0; kind < 4; kind++)
        {
            moves.bestCounts[kind] = 0;
        }

        uint64_t startCoord = state.pieces[m];
        moves.grid[startCoord] = 0;
        moves.moves[0] = startCoord;
        uint64_t moveCount = 1;

        for (uint64_t moveIdx = 0; moveIdx < moveCount; moveIdx++)
        {
            uint64_t coord = moves.moves[moveIdx];
            uint64_t owner = state.owners[coord];

            uint8_t kind;
            if (owner == 255)
            {
                kind = 0;
            }
            else if (owner == m)
            {
                if (state.levels[coord] < U)
                {
                    kind = 1;
                }
                else
                {
                    kind = 4;
                }
            }
            else
            {
                if (state.levels[coord] == 1)
                {
                    kind = 2;
                }
                else
                {
                    kind = 3;
                }
            }
            moves.grid[coord] = kind;

            if (kind < 4)
            {
                uint32_t value = problem.values[coord];
                if (value > moves.bestValues[kind])
                {
                    moves.bestValues[kind] = value;
                    moves.bestCounts[kind] = 1;
                }
                else if (value == moves.bestValues[kind])
                {
                    moves.bestCounts[kind]++;
                }
            }

            if (owner != m) continue;

            uint8_t dirs = coordToDirs[coord];
            for (uint64_t dir = 0; dir < 4; dir++)
            {
                uint8_t dirBit = (uint8_t)1 << dir;
                if ((dirs & dirBit) == 0) continue;
                uint64_t ncoord = coord + dirToDelta[dir];
                if (moves.grid[ncoord] != 255) continue;
                moves.grid[ncoord] = 0;
                uint64_t nowner = state.owners[ncoord];
                if (nowner != 255 && ncoord == state.pieces[nowner]) continue;
                moves.moves[moveCount++] = ncoord;
            }
        }

        moves.moveCount = moveCount;
    }

#if PROFILE
    __attribute__((noinline))
#endif
    uint8_t getEnemyMove(const Problem& problem,
                         const State& state,
                         uint64_t m,
                         double r1,
                         double r2)
    {
        uint64_t U = problem.U;

        uint8_t grid[N * N];
        uint8_t moves[N * N];
        uint8_t bests[N * N];

        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            grid[coord] = 255;
        }

        uint64_t startCoord = state.pieces[m];
        grid[startCoord] = 0;
        moves[0] = startCoord;
        uint64_t moveCount = 1;
        uint64_t bestCount = 0;
        double bestScore = -1.0e100;

        for (uint64_t moveIdx = 0; moveIdx < moveCount; moveIdx++)
        {
            uint64_t coord = moves[moveIdx];
            uint64_t owner = state.owners[coord];

            uint8_t kind;
            if (owner == 255)
            {
                kind = 0;
            }
            else if (owner == m)
            {
                if (state.levels[coord] < U)
                {
                    kind = 1;
                }
                else
                {
                    kind = 4;
                }
            }
            else
            {
                if (state.levels[coord] == 1)
                {
                    kind = 2;
                }
                else
                {
                    kind = 3;
                }
            }
            //moves.grid[coord] = kind;

            if (kind < 4)
            {
                uint32_t value = problem.values[coord];
                double score = (uint64_t)players[m].weights[kind] * (uint64_t)value;
                if (score > bestScore)
                {
                    bestScore = score;
                    bests[0] = coord;
                    bestCount = 1;
                }
                else if (score == bestScore)
                {
                    bests[bestCount++] = coord;
                }
            }

            if (owner != m) continue;

            uint8_t dirs = coordToDirs[coord];
            for (uint64_t dir = 0; dir < 4; dir++)
            {
                uint8_t dirBit = (uint8_t)1 << dir;
                if ((dirs & dirBit) == 0) continue;
                uint64_t ncoord = coord + dirToDelta[dir];
                if (grid[ncoord] != 255) continue;
                grid[ncoord] = 0;
                uint64_t nowner = state.owners[ncoord];
                if (nowner != 255 && ncoord == state.pieces[nowner]) continue;
                moves[moveCount++] = ncoord;
            }
        }

        if (r1 < players[m].eps || bestCount == 0)
        {
            uint64_t moveIdx = (uint64_t)(r2 * (double)moveCount);
            if (moveIdx >= moveCount) moveIdx = moveCount - 1;
            return moves[moveIdx];
        }
        else
        {
            uint64_t bestIdx = (uint64_t)(r2 * (double)bestCount);
            if (bestIdx >= bestCount) bestIdx = bestCount - 1;
            return bests[bestIdx];
        }
    }

    virtual uint64_t step(const Problem& problem) override
    {
        startTimeMS = getTimeMS();

        uint64_t M = problem.M;
        uint64_t U = problem.U;

        State initState;

        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            initState.owners[coord] = problem.owners[coord];
        }
        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            initState.levels[coord] = problem.levels[coord];
        }
        for (uint64_t m = 0; m < M; m++)
        {
            initState.pieces[m] = problem.pieces[m];
        }
        for (uint64_t m = 0; m < M; m++)
        {
            initState.scores[m] = 0;
        }
        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            uint8_t owner = initState.owners[coord];
            if (owner == 255) continue;
            uint32_t value = problem.values[coord];
            uint8_t level = initState.levels[coord];
            initState.scores[owner] += value * level;
        }

        for (uint64_t m = 1; m < M; m++)
        {
            auto& player = players[m];
            auto& moves = topMoves[m];

            if (problem.turn == 0)
            {
                player.epsSum = 0.3 * 4.0;
                player.eps = 0.3;

                for (uint64_t ia = 0; ia < 7; ia++)
                {
                    for (uint64_t ib = 0; ib < 7; ib++)
                    {
                        for (uint64_t ic = 0; ic < 7; ic++)
                        {
                            for (uint64_t id = 0; id < 7; id++)
                            {
                                player.weightCounts[ia][ib][ic][id] = 0;
                            }
                        }
                    }
                }

                for (uint64_t kind = 0; kind < 4; kind++)
                {
                    player.weights[kind] = 1;
                }
            }
            else
            {
                uint64_t coord = problem.moves[m];
                uint64_t kind = moves.grid[coord];
                uint32_t value = problem.values[coord];
                bool isBest = (kind < 4 && value == moves.bestValues[kind]);

                if (!isBest)
                {
                    uint64_t moveCount = moves.moveCount;
                    uint64_t nonBestCount = moveCount;
                    nonBestCount -= moves.bestCounts[0];
                    nonBestCount -= moves.bestCounts[1];
                    nonBestCount -= moves.bestCounts[2];
                    nonBestCount -= moves.bestCounts[3];
                    player.epsSum += (double)moveCount / (double)nonBestCount;
                }

                double eps = player.epsSum / (double)(problem.turn + 4);
                if (eps < 0.1) eps = 0.1;
                else if (eps > 0.5) eps = 0.5;
                player.eps = eps;

                if (isBest)
                {
                    uint8_t maxCount = 0;

                    uint8_t ws[4];
                    ws[0] = 35;
                    for (uint64_t ia = 0; ia < 7; ia++, ws[0] += 10)
                    {
                        ws[1] = 35;
                        for (uint64_t ib = 0; ib < 7; ib++, ws[1] += 10)
                        {
                            ws[2] = 35;
                            for (uint64_t ic = 0; ic < 7; ic++, ws[2] += 10)
                            {
                                ws[3] = 35;
                                for (uint64_t id = 0; id < 7; id++, ws[3] += 10)
                                {
                                    bool ok = true;

                                    for (uint64_t kind2 = 0; kind2 < 4; kind2++)
                                    {
                                        if (kind2 == kind) continue;
                                        if (moves.bestCounts[kind2] == 0) continue;
                                        if ((uint64_t)ws[kind] * moves.bestValues[kind] < (uint64_t)ws[kind2] * moves.bestValues[kind2])
                                        {
                                            ok = false;
                                            break;
                                        }
                                    }

                                    if (ok)
                                    {
                                        player.weightCounts[ia][ib][ic][id]++;
                                    }

                                    maxCount = std::max(maxCount, player.weightCounts[ia][ib][ic][id]);
                                }
                            }
                        }
                    }

                    uint32_t sums[4] = {};
                    ws[0] = 35;
                    for (uint64_t ia = 0; ia < 7; ia++, ws[0] += 10)
                    {
                        ws[1] = 35;
                        for (uint64_t ib = 0; ib < 7; ib++, ws[1] += 10)
                        {
                            ws[2] = 35;
                            for (uint64_t ic = 0; ic < 7; ic++, ws[2] += 10)
                            {
                                ws[3] = 35;
                                for (uint64_t id = 0; id < 7; id++, ws[3] += 10)
                                {
                                    if (player.weightCounts[ia][ib][ic][id] == maxCount)
                                    {
                                        sums[0] += ws[0];
                                        sums[1] += ws[1];
                                        sums[2] += ws[2];
                                        sums[3] += ws[3];
                                    }
                                }
                            }
                        }
                    }

                    for (uint64_t kind = 0; kind < 4; kind++)
                    {
                        player.weights[kind] = sums[kind];
                    }
                }
            }
        }

#if 0
        printf("==============================\n");
        for (uint64_t m = 1; m < M; m++)
        {
            auto& player = players[m];
            printf("m %" PRIu64 ", turn %2" PRIu64 ":", m, problem.turn);
            for (uint64_t kind = 0; kind < 4; kind++)
            {
                printf(" %8.6f", player.weights[kind]);
            }
            printf("  %8.6f", player.eps);
            printf("\n");
        }
#endif

        getMyMoves(problem, initState, topMoves[0]);
        for (uint64_t m = 1; m < M; m++)
        {
            getEnemyMoves(problem, initState, m, topMoves[m]);
        }

        uint64_t firstMoveCount = topMoves[0].moveCount;

        uint64_t moveScoreSums[N * N];
        for (uint64_t firstMoveIdx = 0; firstMoveIdx < firstMoveCount; firstMoveIdx++)
        {
            moveScoreSums[firstMoveIdx] = 0;
        }

        uint64_t stopDurationMS = 1800;
#ifdef LOCAL
        stopDurationMS /= 2;
#endif

        uint64_t limitDurationMS = (elapsedMS < stopDurationMS) ? (stopDurationMS - elapsedMS) / (T - problem.turn) : (uint64_t)0;

        uint64_t iter = 0;
        for (; iter < 4096; iter++)
        {
            if (iter != 0)
            {
                uint64_t durationMS = getTimeMS() - startTimeMS;
                if (durationMS >= limitDurationMS)
                {
                    break;
                }
            }

            static constexpr const uint64_t maxTurnLimit = 14;
            uint64_t turnLimit = std::min(maxTurnLimit, T - problem.turn);

            double r1s[maxTurnLimit][maxM];
            double r2s[maxTurnLimit][maxM];

            for (uint64_t turn = 0; turn < turnLimit; turn++)
            {
                for (uint64_t m = 1; m < M; m++)
                {
                    r1s[turn][m] = fastRandomDouble(gen);
                    r2s[turn][m] = fastRandomDouble(gen);
                }
            }

            uint8_t topCoords[maxM];
            for (uint64_t m = 1; m < M; m++)
            {
                topCoords[m] = getEnemyMove(problem, initState, m, r1s[0][m], r2s[0][m]);
            }

            for (uint64_t firstMoveIdx = 0; firstMoveIdx < firstMoveCount; firstMoveIdx++)
            {
                State state(initState);

                for (uint64_t turn = 0; turn < 1; turn++)
                {
                    topCoords[0] = topMoves[0].moves[firstMoveIdx];

                    uint8_t removed[maxM];

                    for (uint64_t m = 0; m < M; m++)
                    {
                        removed[m] = 0;
                        uint64_t coord = topCoords[m];
                        if (state.owners[coord] == m) continue;
                        for (uint64_t m2 = 0; m2 < M; m2++)
                        {
                            if (m2 == m) continue;
                            uint64_t coord2 = topCoords[m2];
                            if (coord2 == coord)
                            {
                                removed[m] = 1;
                                break;
                            }
                        }
                    }

                    for (uint64_t m = 0; m < M; m++)
                    {
                        if (removed[m]) continue;
                        uint64_t coord = topCoords[m];
                        uint64_t owner = state.owners[coord];
                        if (owner == m)
                        {
                            if (state.levels[coord] < U)
                            {
                                state.levels[coord]++;
                                state.scores[m] += problem.values[coord];
                            }
                        }
                        else if (owner == 255)
                        {
                            state.owners[coord] = m;
                            state.levels[coord] = 1;
                            state.scores[m] += problem.values[coord];
                        }
                        else
                        {
                            state.scores[owner] -= problem.values[coord];
                            if (state.levels[coord] > 1)
                            {
                                state.levels[coord]--;
                                continue;
                            }
                            state.owners[coord] = m;
                            state.scores[m] += problem.values[coord];
                        }
                        state.pieces[m] = coord;
                    }
                }

                for (uint64_t turn = 1; turn < turnLimit; turn++)
                {
                    uint8_t coords[maxM];
                    for (uint64_t m = 1; m < M; m++)
                    {
                        coords[m] = getEnemyMove(problem, state, m, r1s[turn][m], r2s[turn][m]);
                    }

                    getMyMoves(problem, state, myMoves);
                    uint64_t myMoveCount = myMoves.moveCount;

                    uint32_t scores[maxM];
                    for (uint64_t m = 0; m < M; m++)
                    {
                        scores[m] = state.scores[m];
                    }

                    uint32_t scoreDeltas[N * N][maxM] = {};
                    for (uint64_t myMoveIdx = 0; myMoveIdx < myMoveCount; myMoveIdx++)
                    {
                        uint64_t coord = myMoves.moves[myMoveIdx];
                        uint64_t owner = state.owners[coord];
                        if (owner != 0)
                        {
                            bool myRemoved = false;
                            for (uint64_t m2 = 1; m2 < M; m2++)
                            {
                                uint64_t coord2 = coords[m2];
                                if (coord2 == coord)
                                {
                                    myRemoved = true;
                                    break;
                                }
                            }
                            if (myRemoved) continue;
                        }

                        if (owner == 0)
                        {
                            if (state.levels[coord] < U)
                            {
                                scoreDeltas[coord][0] += problem.values[coord];
                            }
                        }
                        else if (owner == 255)
                        {
                            scoreDeltas[coord][0] += problem.values[coord];
                        }
                        else
                        {
                            scoreDeltas[coord][owner] -= problem.values[coord];
                            if (state.levels[coord] == 1)
                            {
                                scoreDeltas[coord][0] += problem.values[coord];
                            }
                        }
                    }

                    for (uint64_t m = 1; m < M; m++)
                    {
                        uint64_t coord = coords[m];
                        uint64_t owner = state.owners[coord];
                        if (owner != m)
                        {
                            bool removed = false;
                            for (uint64_t m2 = 1; m2 < M; m2++)
                            {
                                if (m2 == m) continue;
                                uint64_t coord2 = coords[m2];
                                if (coord2 == coord)
                                {
                                    removed = true;
                                    break;
                                }
                            }
                            if (removed) continue;
                        }

                        if (owner == m)
                        {
                            if (state.levels[coord] < U)
                            {
                                scores[m] += problem.values[coord];
                            }
                        }
                        else if (owner == 255)
                        {
                            scores[m] += problem.values[coord];
                            scoreDeltas[coord][m] -= problem.values[coord];
                        }
                        else
                        {
                            scores[owner] -= problem.values[coord];
                            scoreDeltas[coord][owner] += problem.values[coord];
                            if (state.levels[coord] == 1)
                            {
                                scores[m] += problem.values[coord];
                                scoreDeltas[coord][m] -= problem.values[coord];
                            }
                        }

                    }

                    uint64_t bestMoveIdx = 0;
                    uint32_t bestMoveScore = 0;

                    for (uint64_t myMoveIdx = 0; myMoveIdx < myMoveCount; myMoveIdx++)
                    {
                        uint64_t myCoord = myMoves.moves[myMoveIdx];
                        uint32_t myScore = scores[0] + scoreDeltas[myCoord][0];

                        uint32_t bestEnemyScore = 0;
                        for (uint64_t m = 1; m < M; m++)
                        {
                            uint32_t enemyScore = scores[m] + scoreDeltas[myCoord][m];
                            bestEnemyScore = std::max(bestEnemyScore, enemyScore);
                        }

                        double score = log2Table[myScore + bestEnemyScore] - log2Table[bestEnemyScore];
                        uint32_t moveScore = (uint32_t)(score + 0.5);
                        if (moveScore > bestMoveScore)
                        {
                            bestMoveScore = moveScore;
                            bestMoveIdx = myMoveIdx;
                        }
                    }

                    coords[0] = myMoves.moves[bestMoveIdx];

                    for (uint64_t m = 0; m < M; m++)
                    {
                        uint64_t coord = coords[m];
                        uint64_t owner = state.owners[coord];
                        if (owner != m)
                        {
                            bool removed = false;
                            for (uint64_t m2 = 0; m2 < M; m2++)
                            {
                                if (m2 == m) continue;
                                uint64_t coord2 = coords[m2];
                                if (coord2 == coord)
                                {
                                    removed = true;
                                    break;
                                }
                            }
                            if (removed) continue;
                        }

                        if (owner == m)
                        {
                            if (state.levels[coord] < U)
                            {
                                state.levels[coord]++;
                                state.scores[m] += problem.values[coord];
                            }
                        }
                        else if (owner == 255)
                        {
                            state.owners[coord] = m;
                            state.levels[coord] = 1;
                            state.scores[m] += problem.values[coord];
                        }
                        else
                        {
                            state.scores[owner] -= problem.values[coord];
                            if (state.levels[coord] > 1)
                            {
                                state.levels[coord]--;
                                continue;
                            }
                            state.owners[coord] = m;
                            state.scores[m] += problem.values[coord];
                        }
                        state.pieces[m] = coord;
                    }
                }

                uint32_t bestEnemyScore = 0;
                for (uint64_t m = 1; m < M; m++)
                {
                    bestEnemyScore = std::max(bestEnemyScore, state.scores[m]);
                }
                double score = log2Table[state.scores[0] + bestEnemyScore] - log2Table[bestEnemyScore];
                uint32_t moveScore = (uint32_t)(score + 0.5);
                moveScoreSums[firstMoveIdx] += moveScore;

                //printf("turn %2" PRIu64 "  iter %3" PRIu64 "  move %02" PRIu8 "  score %6" PRIu32 "\n", problem.turn, iter, topMoves[0].moves[firstMoveIdx], moveScore);
            }
        }

        //printf("turn %2" PRIu64 ": %4" PRIu64 " iters\n", problem.turn, iter);

        uint64_t bestFirstMoveIdx = 0;
        uint64_t bestMoveScoreSum = 0;
        for (uint64_t firstMoveIdx = 0; firstMoveIdx < firstMoveCount; firstMoveIdx++)
        {
            uint64_t moveScoreSum = moveScoreSums[firstMoveIdx];
            if (moveScoreSum > bestMoveScoreSum)
            {
                bestMoveScoreSum = moveScoreSum;
                bestFirstMoveIdx = firstMoveIdx;
            }
            //printf("turn %2" PRIu64 "            move %02" PRIu8 "  score %6" PRIu64 "\n", problem.turn, topMoves[0].moves[firstMoveIdx], moveScoreSum / iterCount);
        }

        //printf("turn %2" PRIu64 ", %6" PRIu64 "\n", problem.turn, bestMoveScoreSum / iterCount);

        uint64_t endTimeMS = getTimeMS();
        elapsedMS += endTimeMS - startTimeMS;

        return topMoves[0].moves[bestFirstMoveIdx];
    }

private:
    Gen gen;
    unique_ptr<double[]> log2Table;
    uint64_t elapsedMS;
    uint64_t startTimeMS;
    Moves topMoves[maxM];
    Player players[maxM];
    Moves myMoves;
};

class Mcts2Solver : public Solver
{
public:
    class State
    {
    public:
        uint8_t owners[N * N];
        uint8_t levels[N * N];
        uint8_t pieces[maxM];
        uint32_t scores[maxM];
    };

    class Moves
    {
    public:
        uint8_t grid[N * N];
        uint8_t moves[N * N];
        uint64_t moveCount;
        uint32_t bestValues[4];
        uint8_t bestCounts[4];
    };

    class Player
    {
    public:
        double epsSum;
        double eps;
        uint8_t weightCounts[7][7][7][7];
        uint32_t weights[4];
    };

    class Rand
    {
    public:
        double r1;
        uint32_t r2mul;
        uint32_t r2add;
    };

    virtual void init(const Problem& problem) override
    {
        startTimeMS = getTimeMS();

#if 1
        {
            uint64_t timeMS = startTimeMS;
            uint32_t pid = getProcessId();
            uint32_t seed0 = 0x40928ec6;
            uint32_t seed1 = (uint32_t)timeMS;
            uint32_t seed2 = (uint32_t)pid ^ (uint32_t)(timeMS >> 32);
            std::seed_seq seq{seed0, seed1, seed2};
            gen.seed(seq);
        }
#endif
#if 0
        {
            uint32_t seed0 = 0x40928ec6;
            std::seed_seq seq{seed0};
            gen.seed(seq);
        }
#endif

        log2Table.reset(new double[500001]);
        log2Table[0] = 0.0;
        for (uint32_t i = 1; i <= 500000; i++)
        {
            log2Table[i] = __builtin_log2(i) * 100000.0;
        }

        uint64_t endTimeMS = getTimeMS();
        elapsedMS = endTimeMS - startTimeMS;
    }

#if PROFILE
    __attribute__((noinline))
#endif
    void getMyMoves(const Problem& problem,
                    const State& state,
                    Moves& moves)
    {
        uint64_t U = problem.U;

        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            moves.grid[coord] = 255;
        }

        uint8_t q[N * N];

        uint64_t startCoord = state.pieces[0];
        moves.grid[startCoord] = 0;
        q[0] = startCoord;
        uint64_t qSize = 1;
        uint64_t moveCount = 0;

        if (state.levels[startCoord] != U)
        {
            moves.moves[moveCount++] = startCoord;
        }

        for (uint64_t qIdx = 0; qIdx < qSize; qIdx++)
        {
            uint64_t coord = q[qIdx];

            uint8_t dirs = coordToDirs[coord];
            for (uint64_t dir = 0; dir < 4; dir++)
            {
                uint8_t dirBit = (uint8_t)1 << dir;
                if ((dirs & dirBit) == 0) continue;
                uint64_t ncoord = coord + dirToDelta[dir];
                if (moves.grid[ncoord] != 255) continue;
                moves.grid[ncoord] = 0;
                uint64_t nowner = state.owners[ncoord];
                if (nowner != 255 && ncoord == state.pieces[nowner]) continue;
                if (nowner != 0 || state.levels[ncoord] != U)
                {
                    moves.moves[moveCount++] = ncoord;
                }
                if (nowner == 0)
                {
                    q[qSize++] = ncoord;
                }
            }
        }

        if (moveCount == 0) moves.moves[moveCount++] = startCoord;

        moves.moveCount = moveCount;
    }

#if PROFILE
    __attribute__((noinline))
#endif
    void getEnemyMoves(const Problem& problem,
                       const State& state,
                       uint64_t m,
                       Moves& moves)
    {
        uint64_t U = problem.U;

        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            moves.grid[coord] = 255;
        }

        for (uint64_t kind = 0; kind < 4; kind++)
        {
            moves.bestValues[kind] = 0;
        }

        for (uint64_t kind = 0; kind < 4; kind++)
        {
            moves.bestCounts[kind] = 0;
        }

        uint64_t startCoord = state.pieces[m];
        moves.grid[startCoord] = 0;
        moves.moves[0] = startCoord;
        uint64_t moveCount = 1;

        for (uint64_t moveIdx = 0; moveIdx < moveCount; moveIdx++)
        {
            uint64_t coord = moves.moves[moveIdx];
            uint64_t owner = state.owners[coord];

            uint8_t kind;
            if (owner == 255)
            {
                kind = 0;
            }
            else if (owner == m)
            {
                if (state.levels[coord] < U)
                {
                    kind = 1;
                }
                else
                {
                    kind = 4;
                }
            }
            else
            {
                if (state.levels[coord] == 1)
                {
                    kind = 2;
                }
                else
                {
                    kind = 3;
                }
            }
            moves.grid[coord] = kind;

            if (kind < 4)
            {
                uint32_t value = problem.values[coord];
                if (value > moves.bestValues[kind])
                {
                    moves.bestValues[kind] = value;
                    moves.bestCounts[kind] = 1;
                }
                else if (value == moves.bestValues[kind])
                {
                    moves.bestCounts[kind]++;
                }
            }

            if (owner != m) continue;

            uint8_t dirs = coordToDirs[coord];
            for (uint64_t dir = 0; dir < 4; dir++)
            {
                uint8_t dirBit = (uint8_t)1 << dir;
                if ((dirs & dirBit) == 0) continue;
                uint64_t ncoord = coord + dirToDelta[dir];
                if (moves.grid[ncoord] != 255) continue;
                moves.grid[ncoord] = 0;
                uint64_t nowner = state.owners[ncoord];
                if (nowner != 255 && ncoord == state.pieces[nowner]) continue;
                moves.moves[moveCount++] = ncoord;
            }
        }

        moves.moveCount = moveCount;
    }

#if PROFILE
    __attribute__((noinline))
#endif
    uint8_t getEnemyMove(const Problem& problem,
                         const State& state,
                         uint64_t m,
                         const Rand& rand)
    {
        uint64_t U = problem.U;

        if (rand.r1 < players[m].eps)
        {
            uint8_t grid[N * N];
            uint8_t q[N * N];

            for (uint64_t coord = 0; coord < N * N; coord++)
            {
                grid[coord] = 255;
            }

            uint64_t startCoord = state.pieces[m];
            grid[startCoord] = 0;
            q[0] = startCoord;
            uint64_t qSize = 1;

            uint32_t r2mul = rand.r2mul;
            uint32_t r2add = rand.r2add;

            uint64_t bestCoord = startCoord;
            uint32_t bestR = (uint32_t)startCoord * r2mul + r2add;

            for (uint64_t qIdx = 0; qIdx < qSize; qIdx++)
            {
                uint64_t coord = q[qIdx];

                uint8_t dirs = coordToDirs[coord];
                for (uint64_t dir = 0; dir < 4; dir++)
                {
                    uint8_t dirBit = (uint8_t)1 << dir;
                    if ((dirs & dirBit) == 0) continue;
                    uint64_t ncoord = coord + dirToDelta[dir];
                    if (grid[ncoord] != 255) continue;
                    grid[ncoord] = 0;
                    uint64_t nowner = state.owners[ncoord];
                    if (nowner != 255 && ncoord == state.pieces[nowner]) continue;
                    uint32_t r = (uint32_t)ncoord * r2mul + r2add;
                    if (r > bestR)
                    {
                        bestR = r;
                        bestCoord = ncoord;
                    }
                    if (nowner == m) q[qSize++] = ncoord;
                }
            }

            return bestCoord;
        }
        else
        {
            uint8_t grid[N * N];
            uint8_t q[N * N];

            for (uint64_t coord = 0; coord < N * N; coord++)
            {
                grid[coord] = 255;
            }

            uint64_t startCoord = state.pieces[m];
            grid[startCoord] = 0;
            q[0] = startCoord;
            uint64_t qSize = 1;

            uint32_t r2mul = rand.r2mul;
            uint32_t r2add = rand.r2add;

            uint64_t bestCoord = startCoord;
            uint32_t bestR = 0;

            uint64_t bestScore = 0;

            if (state.levels[startCoord] < U)
            {
                uint8_t kind = 1;
                uint32_t value = problem.values[startCoord];
                bestScore = (uint64_t)players[m].weights[kind] * (uint64_t)value;
                bestCoord = startCoord;
                bestR = (uint32_t)startCoord * r2mul + r2add;
            }

            for (uint64_t qIdx = 0; qIdx < qSize; qIdx++)
            {
                uint64_t coord = q[qIdx];

                uint8_t dirs = coordToDirs[coord];
                for (uint64_t dir = 0; dir < 4; dir++)
                {
                    uint8_t dirBit = (uint8_t)1 << dir;
                    if ((dirs & dirBit) == 0) continue;
                    uint64_t ncoord = coord + dirToDelta[dir];
                    if (grid[ncoord] != 255) continue;
                    grid[ncoord] = 0;
                    uint64_t nowner = state.owners[ncoord];
                    if (nowner != 255 && ncoord == state.pieces[nowner]) continue;
                    if (nowner == m) q[qSize++] = ncoord;

                    uint8_t kind;
                    if (nowner == 255)
                    {
                        kind = 0;
                    }
                    else if (nowner == m)
                    {
                        if (state.levels[ncoord] < U)
                        {
                            kind = 1;
                        }
                        else
                        {
                            kind = 4;
                        }
                    }
                    else
                    {
                        if (state.levels[ncoord] == 1)
                        {
                            kind = 2;
                        }
                        else
                        {
                            kind = 3;
                        }
                    }

                    if (kind < 4)
                    {
                        uint32_t value = problem.values[ncoord];
                        uint64_t score = (uint64_t)players[m].weights[kind] * (uint64_t)value;
                        if (score > bestScore)
                        {
                            bestScore = score;
                            bestCoord = ncoord;
                            bestR = (uint32_t)ncoord * r2mul + r2add;
                        }
                        else if (score == bestScore)
                        {
                            uint32_t r = (uint32_t)ncoord * r2mul + r2add;
                            if (r > bestR)
                            {
                                bestR = r;
                                bestCoord = ncoord;
                            }
                        }
                    }
                }
            }

            return bestCoord;
        }
    }

    virtual uint64_t step(const Problem& problem) override
    {
        startTimeMS = getTimeMS();

        uint64_t M = problem.M;
        uint64_t U = problem.U;

        State initState;

        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            initState.owners[coord] = problem.owners[coord];
        }
        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            initState.levels[coord] = problem.levels[coord];
        }
        for (uint64_t m = 0; m < M; m++)
        {
            initState.pieces[m] = problem.pieces[m];
        }
        for (uint64_t m = 0; m < M; m++)
        {
            initState.scores[m] = 0;
        }
        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            uint8_t owner = initState.owners[coord];
            if (owner == 255) continue;
            uint32_t value = problem.values[coord];
            uint8_t level = initState.levels[coord];
            initState.scores[owner] += value * level;
        }

        for (uint64_t m = 1; m < M; m++)
        {
            auto& player = players[m];
            auto& moves = topMoves[m];

            if (problem.turn == 0)
            {
                player.epsSum = 0.3 * 4.0;
                player.eps = 0.3;

                for (uint64_t ia = 0; ia < 7; ia++)
                {
                    for (uint64_t ib = 0; ib < 7; ib++)
                    {
                        for (uint64_t ic = 0; ic < 7; ic++)
                        {
                            for (uint64_t id = 0; id < 7; id++)
                            {
                                player.weightCounts[ia][ib][ic][id] = 0;
                            }
                        }
                    }
                }

                for (uint64_t kind = 0; kind < 4; kind++)
                {
                    player.weights[kind] = 1;
                }
            }
            else
            {
                uint64_t coord = problem.moves[m];
                uint64_t kind = moves.grid[coord];
                uint32_t value = problem.values[coord];
                bool isBest = (kind < 4 && value == moves.bestValues[kind]);

                if (!isBest)
                {
                    uint64_t moveCount = moves.moveCount;
                    uint64_t nonBestCount = moveCount;
                    nonBestCount -= moves.bestCounts[0];
                    nonBestCount -= moves.bestCounts[1];
                    nonBestCount -= moves.bestCounts[2];
                    nonBestCount -= moves.bestCounts[3];
                    player.epsSum += (double)moveCount / (double)nonBestCount;
                }

                double eps = player.epsSum / (double)(problem.turn + 4);
                if (eps < 0.1) eps = 0.1;
                else if (eps > 0.5) eps = 0.5;
                player.eps = eps;

                if (isBest)
                {
                    uint8_t maxCount = 0;

                    uint8_t ws[4];
                    ws[0] = 35;
                    for (uint64_t ia = 0; ia < 7; ia++, ws[0] += 10)
                    {
                        ws[1] = 35;
                        for (uint64_t ib = 0; ib < 7; ib++, ws[1] += 10)
                        {
                            ws[2] = 35;
                            for (uint64_t ic = 0; ic < 7; ic++, ws[2] += 10)
                            {
                                ws[3] = 35;
                                for (uint64_t id = 0; id < 7; id++, ws[3] += 10)
                                {
                                    bool ok = true;

                                    for (uint64_t kind2 = 0; kind2 < 4; kind2++)
                                    {
                                        if (kind2 == kind) continue;
                                        if (moves.bestCounts[kind2] == 0) continue;
                                        if ((uint64_t)ws[kind] * moves.bestValues[kind] < (uint64_t)ws[kind2] * moves.bestValues[kind2])
                                        {
                                            ok = false;
                                            break;
                                        }
                                    }

                                    if (ok)
                                    {
                                        player.weightCounts[ia][ib][ic][id]++;
                                    }

                                    maxCount = std::max(maxCount, player.weightCounts[ia][ib][ic][id]);
                                }
                            }
                        }
                    }

                    uint32_t sums[4] = {};
                    ws[0] = 35;
                    for (uint64_t ia = 0; ia < 7; ia++, ws[0] += 10)
                    {
                        ws[1] = 35;
                        for (uint64_t ib = 0; ib < 7; ib++, ws[1] += 10)
                        {
                            ws[2] = 35;
                            for (uint64_t ic = 0; ic < 7; ic++, ws[2] += 10)
                            {
                                ws[3] = 35;
                                for (uint64_t id = 0; id < 7; id++, ws[3] += 10)
                                {
                                    if (player.weightCounts[ia][ib][ic][id] == maxCount)
                                    {
                                        sums[0] += ws[0];
                                        sums[1] += ws[1];
                                        sums[2] += ws[2];
                                        sums[3] += ws[3];
                                    }
                                }
                            }
                        }
                    }

                    for (uint64_t kind = 0; kind < 4; kind++)
                    {
                        player.weights[kind] = sums[kind];
                    }
                }
            }
        }

#if 0
        printf("==============================\n");
        for (uint64_t m = 1; m < M; m++)
        {
            auto& player = players[m];
            printf("m %" PRIu64 ", turn %2" PRIu64 ":", m, problem.turn);
            for (uint64_t kind = 0; kind < 4; kind++)
            {
                printf(" %8.6f", player.weights[kind]);
            }
            printf("  %8.6f", player.eps);
            printf("\n");
        }
#endif

        getMyMoves(problem, initState, topMoves[0]);
        for (uint64_t m = 1; m < M; m++)
        {
            getEnemyMoves(problem, initState, m, topMoves[m]);
        }

        uint64_t firstMoveCount = topMoves[0].moveCount;

        uint64_t moveScoreSums[N * N];
        for (uint64_t firstMoveIdx = 0; firstMoveIdx < firstMoveCount; firstMoveIdx++)
        {
            moveScoreSums[firstMoveIdx] = 0;
        }

        uint64_t stopDurationMS = 1800;
#ifdef LOCAL
        stopDurationMS /= 2;
#endif

        uint64_t limitDurationMS = (elapsedMS < stopDurationMS) ? (stopDurationMS - elapsedMS) / (T - problem.turn) : (uint64_t)0;
        uint64_t elimLimitDurationMS = (limitDurationMS + 1) / 2;
        bool elimDone = false;

        uint8_t elim[N * N];
        for (uint64_t firstMoveIdx = 0; firstMoveIdx < firstMoveCount; firstMoveIdx++)
        {
            elim[firstMoveIdx] = 0;
        }

        uint64_t iter = 0;
        for (; iter < 4096; iter++)
        {
            if (iter != 0)
            {
                uint64_t durationMS = getTimeMS() - startTimeMS;
                if (durationMS >= limitDurationMS)
                {
                    break;
                }

                if (!elimDone && durationMS >= elimLimitDurationMS)
                {
                    uint8_t order[N * N];
                    for (uint64_t firstMoveIdx = 0; firstMoveIdx < firstMoveCount; firstMoveIdx++)
                    {
                        order[firstMoveIdx] = firstMoveIdx;
                    }

                    uint64_t keepCount = (firstMoveCount + 7) / 8;
                    keepCount = std::max(keepCount, (uint64_t)2);
                    keepCount = std::min(keepCount, firstMoveCount);

                    auto elimComp = [&](uint8_t idxA, uint8_t idxB) -> bool
                    {
                        uint64_t moveScoreSumA = moveScoreSums[idxA];
                        uint64_t moveScoreSumB = moveScoreSums[idxB];
                        if (moveScoreSumA != moveScoreSumB)
                        {
                            return moveScoreSumA > moveScoreSumB;
                        }
                        return idxA < idxB;
                    };

                    std::nth_element(&order[0], &order[0] + (keepCount - 1), &order[0] + firstMoveCount, elimComp);

                    for (uint64_t orderIdx = keepCount; orderIdx < firstMoveCount; orderIdx++)
                    {
                        elim[order[orderIdx]] = 1;
                    }

                    elimDone = true;
                }
            }

            static constexpr const uint64_t maxTurnLimit = 14;
            uint64_t turnLimit = (M == 2) ? 10 : 14;
            turnLimit = std::min(turnLimit, T - problem.turn);

            Rand rands[maxTurnLimit][maxM];

            for (uint64_t turn = 0; turn < turnLimit; turn++)
            {
                for (uint64_t m = 0; m < M; m++)
                {
                    auto& rand = rands[turn][m];
                    rand.r1 = fastRandomDouble(gen);
                    uint64_t r2 = gen();
                    rand.r2mul = (uint32_t)r2;
                    rand.r2add = (uint32_t)(r2 >> 32);
                }
            }

            uint8_t topCoords[maxM];
            for (uint64_t m = 1; m < M; m++)
            {
                topCoords[m] = getEnemyMove(problem, initState, m, rands[0][m]);
            }

            for (uint64_t firstMoveIdx = 0; firstMoveIdx < firstMoveCount; firstMoveIdx++)
            {
                if (elim[firstMoveIdx]) continue;

                State state(initState);

                for (uint64_t turn = 0; turn < 1; turn++)
                {
                    topCoords[0] = topMoves[0].moves[firstMoveIdx];

                    uint8_t removed[maxM];

                    for (uint64_t m = 0; m < M; m++)
                    {
                        removed[m] = 0;
                        uint64_t coord = topCoords[m];
                        if (state.owners[coord] == m) continue;
                        for (uint64_t m2 = 0; m2 < M; m2++)
                        {
                            if (m2 == m) continue;
                            uint64_t coord2 = topCoords[m2];
                            if (coord2 == coord)
                            {
                                removed[m] = 1;
                                break;
                            }
                        }
                    }

                    for (uint64_t m = 0; m < M; m++)
                    {
                        if (removed[m]) continue;
                        uint64_t coord = topCoords[m];
                        uint64_t owner = state.owners[coord];
                        if (owner == m)
                        {
                            if (state.levels[coord] < U)
                            {
                                state.levels[coord]++;
                                state.scores[m] += problem.values[coord];
                            }
                        }
                        else if (owner == 255)
                        {
                            state.owners[coord] = m;
                            state.levels[coord] = 1;
                            state.scores[m] += problem.values[coord];
                        }
                        else
                        {
                            state.scores[owner] -= problem.values[coord];
                            if (state.levels[coord] > 1)
                            {
                                state.levels[coord]--;
                                continue;
                            }
                            state.owners[coord] = m;
                            state.scores[m] += problem.values[coord];
                        }
                        state.pieces[m] = coord;
                    }
                }

                for (uint64_t turn = 1; turn < turnLimit; turn++)
                {
                    uint8_t coords[maxM];
                    for (uint64_t m = 1; m < M; m++)
                    {
                        coords[m] = getEnemyMove(problem, state, m, rands[turn][m]);
                    }

                    getMyMoves(problem, state, myMoves);
                    uint64_t myMoveCount = myMoves.moveCount;

                    double randomEps = (M == 2) ? 0.15 : 0.30;

                    if (rands[turn][0].r1 < randomEps)
                    {
                        auto& rand = rands[turn][0];
                        uint32_t bestCoord = 0;
                        uint32_t bestRand = 0;
                        for (uint64_t moveIdx = 0; moveIdx < myMoveCount; moveIdx++)
                        {
                            uint32_t coord = myMoves.moves[moveIdx];
                            uint32_t r = coord * rand.r2mul + rand.r2add;
                            if (r > bestRand)
                            {
                                bestRand = r;
                                bestCoord = coord;
                            }
                        }
                        coords[0] = bestCoord;
                    }
                    else
                    {
                        uint32_t scores[maxM];
                        for (uint64_t m = 0; m < M; m++)
                        {
                            scores[m] = state.scores[m];
                        }

                        uint32_t scoreDeltas[N * N][maxM] = {};
                        for (uint64_t myMoveIdx = 0; myMoveIdx < myMoveCount; myMoveIdx++)
                        {
                            uint64_t coord = myMoves.moves[myMoveIdx];
                            uint64_t owner = state.owners[coord];
                            if (owner != 0)
                            {
                                bool myRemoved = false;
                                for (uint64_t m2 = 1; m2 < M; m2++)
                                {
                                    uint64_t coord2 = coords[m2];
                                    if (coord2 == coord)
                                    {
                                        myRemoved = true;
                                        break;
                                    }
                                }
                                if (myRemoved) continue;
                            }

                            if (owner == 0)
                            {
                                if (state.levels[coord] < U)
                                {
                                    scoreDeltas[coord][0] += problem.values[coord];
                                }
                            }
                            else if (owner == 255)
                            {
                                scoreDeltas[coord][0] += problem.values[coord];
                            }
                            else
                            {
                                scoreDeltas[coord][owner] -= problem.values[coord];
                                if (state.levels[coord] == 1)
                                {
                                    scoreDeltas[coord][0] += problem.values[coord];
                                }
                            }
                        }

                        for (uint64_t m = 1; m < M; m++)
                        {
                            uint64_t coord = coords[m];
                            uint64_t owner = state.owners[coord];
                            if (owner != m)
                            {
                                bool removed = false;
                                for (uint64_t m2 = 1; m2 < M; m2++)
                                {
                                    if (m2 == m) continue;
                                    uint64_t coord2 = coords[m2];
                                    if (coord2 == coord)
                                    {
                                        removed = true;
                                        break;
                                    }
                                }
                                if (removed) continue;
                            }

                            if (owner == m)
                            {
                                if (state.levels[coord] < U)
                                {
                                    scores[m] += problem.values[coord];
                                }
                            }
                            else if (owner == 255)
                            {
                                scores[m] += problem.values[coord];
                                scoreDeltas[coord][m] -= problem.values[coord];
                            }
                            else
                            {
                                scores[owner] -= problem.values[coord];
                                scoreDeltas[coord][owner] += problem.values[coord];
                                if (state.levels[coord] == 1)
                                {
                                    scores[m] += problem.values[coord];
                                    scoreDeltas[coord][m] -= problem.values[coord];
                                }
                            }

                        }

                        uint64_t bestMoveIdx = 0;
                        uint32_t bestMoveScore = 0;

                        for (uint64_t myMoveIdx = 0; myMoveIdx < myMoveCount; myMoveIdx++)
                        {
                            uint64_t myCoord = myMoves.moves[myMoveIdx];
                            uint32_t myScore = scores[0] + scoreDeltas[myCoord][0];

                            uint32_t bestEnemyScore = 0;
                            for (uint64_t m = 1; m < M; m++)
                            {
                                uint32_t enemyScore = scores[m] + scoreDeltas[myCoord][m];
                                bestEnemyScore = std::max(bestEnemyScore, enemyScore);
                            }

                            double score = log2Table[myScore + bestEnemyScore] - log2Table[bestEnemyScore];
                            uint32_t moveScore = (uint32_t)(score + 0.5);
                            if (moveScore > bestMoveScore)
                            {
                                bestMoveScore = moveScore;
                                bestMoveIdx = myMoveIdx;
                            }
                        }

                        coords[0] = myMoves.moves[bestMoveIdx];
                    }

                    for (uint64_t m = 0; m < M; m++)
                    {
                        uint64_t coord = coords[m];
                        uint64_t owner = state.owners[coord];
                        if (owner != m)
                        {
                            bool removed = false;
                            for (uint64_t m2 = 0; m2 < M; m2++)
                            {
                                if (m2 == m) continue;
                                uint64_t coord2 = coords[m2];
                                if (coord2 == coord)
                                {
                                    removed = true;
                                    break;
                                }
                            }
                            if (removed) continue;
                        }

                        if (owner == m)
                        {
                            if (state.levels[coord] < U)
                            {
                                state.levels[coord]++;
                                state.scores[m] += problem.values[coord];
                            }
                        }
                        else if (owner == 255)
                        {
                            state.owners[coord] = m;
                            state.levels[coord] = 1;
                            state.scores[m] += problem.values[coord];
                        }
                        else
                        {
                            state.scores[owner] -= problem.values[coord];
                            if (state.levels[coord] > 1)
                            {
                                state.levels[coord]--;
                                continue;
                            }
                            state.owners[coord] = m;
                            state.scores[m] += problem.values[coord];
                        }
                        state.pieces[m] = coord;
                    }
                }

                uint32_t bestEnemyScore = 0;
                for (uint64_t m = 1; m < M; m++)
                {
                    bestEnemyScore = std::max(bestEnemyScore, state.scores[m]);
                }
                double score = log2Table[state.scores[0] + bestEnemyScore] - log2Table[bestEnemyScore];
                uint32_t moveScore = (uint32_t)(score + 0.5);
                moveScoreSums[firstMoveIdx] += moveScore;

                //printf("turn %2" PRIu64 "  iter %3" PRIu64 "  move %02" PRIu8 "  score %6" PRIu32 "\n", problem.turn, iter, topMoves[0].moves[firstMoveIdx], moveScore);
            }
        }

        //printf("turn %2" PRIu64 ": %4" PRIu64 " iters\n", problem.turn, iter);

        uint64_t bestFirstMoveIdx = 0;
        uint64_t bestMoveScoreSum = 0;
        for (uint64_t firstMoveIdx = 0; firstMoveIdx < firstMoveCount; firstMoveIdx++)
        {
            uint64_t moveScoreSum = moveScoreSums[firstMoveIdx];
            if (moveScoreSum > bestMoveScoreSum)
            {
                bestMoveScoreSum = moveScoreSum;
                bestFirstMoveIdx = firstMoveIdx;
            }
            //printf("turn %2" PRIu64 "            move %02" PRIu8 "  score %6" PRIu64 "\n", problem.turn, topMoves[0].moves[firstMoveIdx], moveScoreSum / iterCount);
        }

        //printf("turn %2" PRIu64 ", %6" PRIu64 "\n", problem.turn, bestMoveScoreSum / iterCount);

        uint64_t endTimeMS = getTimeMS();
        elapsedMS += endTimeMS - startTimeMS;

        return topMoves[0].moves[bestFirstMoveIdx];
    }

private:
    Gen gen;
    unique_ptr<double[]> log2Table;
    uint64_t elapsedMS;
    uint64_t startTimeMS;
    Moves topMoves[maxM];
    Player players[maxM];
    Moves myMoves;
};

class Mcts3Solver : public Solver
{
public:
    class State
    {
    public:
        uint8_t owners[N * N];
        uint8_t levels[N * N];
        uint8_t pieces[maxM];
        uint32_t scores[maxM];
    };

    class Moves
    {
    public:
        uint8_t grid[N * N];
        uint8_t moves[N * N];
        uint64_t moveCount;
        uint32_t bestValues[4];
        uint8_t bestCounts[4];
    };

    class Player
    {
    public:
        double epsSum;
        double eps;
        uint8_t weightCounts[7][7][7][7];
        uint32_t weights[4];
    };

    class Rand
    {
    public:
        double r1;
        uint32_t r2mul;
        uint32_t r2add;
    };

    virtual void init(const Problem& problem) override
    {
        startTimeMS = getTimeMS();

#if 1
        {
            uint64_t timeMS = startTimeMS;
            uint32_t pid = getProcessId();
            uint32_t seed0 = 0x40928ec6;
            uint32_t seed1 = (uint32_t)timeMS;
            uint32_t seed2 = (uint32_t)pid ^ (uint32_t)(timeMS >> 32);
            std::seed_seq seq{seed0, seed1, seed2};
            gen.seed(seq);
        }
#endif
#if 0
        {
            uint32_t seed0 = 0x40928ec6;
            std::seed_seq seq{seed0};
            gen.seed(seq);
        }
#endif

        log2Table.reset(new double[500001]);
        log2Table[0] = 0.0;
        for (uint32_t i = 1; i <= 500000; i++)
        {
            log2Table[i] = __builtin_log2(i) * 100000.0;
        }

        uint64_t endTimeMS = getTimeMS();
        elapsedMS = endTimeMS - startTimeMS;
    }

#if PROFILE
    __attribute__((noinline))
#endif
    void getMyMoves(const Problem& problem,
                    const State& state,
                    Moves& moves)
    {
        uint64_t U = problem.U;

        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            moves.grid[coord] = 255;
        }

        uint8_t q[N * N];

        uint64_t startCoord = state.pieces[0];
        moves.grid[startCoord] = 0;
        q[0] = startCoord;
        uint64_t qSize = 1;
        uint64_t moveCount = 0;

        if (state.levels[startCoord] != U)
        {
            moves.moves[moveCount++] = startCoord;
        }

        for (uint64_t qIdx = 0; qIdx < qSize; qIdx++)
        {
            uint64_t coord = q[qIdx];

            uint8_t dirs = coordToDirs[coord];
            for (uint64_t dir = 0; dir < 4; dir++)
            {
                uint8_t dirBit = (uint8_t)1 << dir;
                if ((dirs & dirBit) == 0) continue;
                uint64_t ncoord = coord + dirToDelta[dir];
                if (moves.grid[ncoord] != 255) continue;
                moves.grid[ncoord] = 0;
                uint64_t nowner = state.owners[ncoord];
                if (nowner != 255 && ncoord == state.pieces[nowner]) continue;
                if (nowner != 0 || state.levels[ncoord] != U)
                {
                    moves.moves[moveCount++] = ncoord;
                }
                if (nowner == 0)
                {
                    q[qSize++] = ncoord;
                }
            }
        }

        if (moveCount == 0) moves.moves[moveCount++] = startCoord;

        moves.moveCount = moveCount;
    }

#if PROFILE
    __attribute__((noinline))
#endif
    void getEnemyMoves(const Problem& problem,
                       const State& state,
                       uint64_t m,
                       Moves& moves)
    {
        uint64_t U = problem.U;

        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            moves.grid[coord] = 255;
        }

        for (uint64_t kind = 0; kind < 4; kind++)
        {
            moves.bestValues[kind] = 0;
        }

        for (uint64_t kind = 0; kind < 4; kind++)
        {
            moves.bestCounts[kind] = 0;
        }

        uint64_t startCoord = state.pieces[m];
        moves.grid[startCoord] = 0;
        moves.moves[0] = startCoord;
        uint64_t moveCount = 1;

        for (uint64_t moveIdx = 0; moveIdx < moveCount; moveIdx++)
        {
            uint64_t coord = moves.moves[moveIdx];
            uint64_t owner = state.owners[coord];

            uint8_t kind;
            if (owner == 255)
            {
                kind = 0;
            }
            else if (owner == m)
            {
                if (state.levels[coord] < U)
                {
                    kind = 1;
                }
                else
                {
                    kind = 4;
                }
            }
            else
            {
                if (state.levels[coord] == 1)
                {
                    kind = 2;
                }
                else
                {
                    kind = 3;
                }
            }
            moves.grid[coord] = kind;

            if (kind < 4)
            {
                uint32_t value = problem.values[coord];
                if (value > moves.bestValues[kind])
                {
                    moves.bestValues[kind] = value;
                    moves.bestCounts[kind] = 1;
                }
                else if (value == moves.bestValues[kind])
                {
                    moves.bestCounts[kind]++;
                }
            }

            if (owner != m) continue;

            uint8_t dirs = coordToDirs[coord];
            for (uint64_t dir = 0; dir < 4; dir++)
            {
                uint8_t dirBit = (uint8_t)1 << dir;
                if ((dirs & dirBit) == 0) continue;
                uint64_t ncoord = coord + dirToDelta[dir];
                if (moves.grid[ncoord] != 255) continue;
                moves.grid[ncoord] = 0;
                uint64_t nowner = state.owners[ncoord];
                if (nowner != 255 && ncoord == state.pieces[nowner]) continue;
                moves.moves[moveCount++] = ncoord;
            }
        }

        moves.moveCount = moveCount;
    }

#if PROFILE
    __attribute__((noinline))
#endif
    uint8_t getEnemyMove(const Problem& problem,
                         const State& state,
                         uint64_t m,
                         const Rand& rand)
    {
        uint64_t U = problem.U;

        if (rand.r1 < players[m].eps)
        {
            uint8_t grid[N * N];
            uint8_t q[N * N];

            for (uint64_t coord = 0; coord < N * N; coord++)
            {
                grid[coord] = 255;
            }

            uint64_t startCoord = state.pieces[m];
            grid[startCoord] = 0;
            q[0] = startCoord;
            uint64_t qSize = 1;

            uint32_t r2mul = rand.r2mul;
            uint32_t r2add = rand.r2add;

            uint64_t bestCoord = startCoord;
            uint32_t bestR = (uint32_t)startCoord * r2mul + r2add;

            for (uint64_t qIdx = 0; qIdx < qSize; qIdx++)
            {
                uint64_t coord = q[qIdx];

                uint8_t dirs = coordToDirs[coord];
                for (uint64_t dir = 0; dir < 4; dir++)
                {
                    uint8_t dirBit = (uint8_t)1 << dir;
                    if ((dirs & dirBit) == 0) continue;
                    uint64_t ncoord = coord + dirToDelta[dir];
                    if (grid[ncoord] != 255) continue;
                    grid[ncoord] = 0;
                    uint64_t nowner = state.owners[ncoord];
                    if (nowner != 255 && ncoord == state.pieces[nowner]) continue;
                    uint32_t r = (uint32_t)ncoord * r2mul + r2add;
                    if (r > bestR)
                    {
                        bestR = r;
                        bestCoord = ncoord;
                    }
                    if (nowner == m) q[qSize++] = ncoord;
                }
            }

            return bestCoord;
        }
        else
        {
            uint8_t grid[N * N];
            uint8_t q[N * N];

            for (uint64_t coord = 0; coord < N * N; coord++)
            {
                grid[coord] = 255;
            }

            uint64_t startCoord = state.pieces[m];
            grid[startCoord] = 0;
            q[0] = startCoord;
            uint64_t qSize = 1;

            uint32_t r2mul = rand.r2mul;
            uint32_t r2add = rand.r2add;

            uint64_t bestCoord = startCoord;
            uint32_t bestR = 0;

            uint64_t bestScore = 0;

            if (state.levels[startCoord] < U)
            {
                uint8_t kind = 1;
                uint32_t value = problem.values[startCoord];
                bestScore = (uint64_t)players[m].weights[kind] * (uint64_t)value;
                bestCoord = startCoord;
                bestR = (uint32_t)startCoord * r2mul + r2add;
            }

            for (uint64_t qIdx = 0; qIdx < qSize; qIdx++)
            {
                uint64_t coord = q[qIdx];

                uint8_t dirs = coordToDirs[coord];
                for (uint64_t dir = 0; dir < 4; dir++)
                {
                    uint8_t dirBit = (uint8_t)1 << dir;
                    if ((dirs & dirBit) == 0) continue;
                    uint64_t ncoord = coord + dirToDelta[dir];
                    if (grid[ncoord] != 255) continue;
                    grid[ncoord] = 0;
                    uint64_t nowner = state.owners[ncoord];
                    if (nowner != 255 && ncoord == state.pieces[nowner]) continue;
                    if (nowner == m) q[qSize++] = ncoord;

                    uint8_t kind;
                    if (nowner == 255)
                    {
                        kind = 0;
                    }
                    else if (nowner == m)
                    {
                        if (state.levels[ncoord] < U)
                        {
                            kind = 1;
                        }
                        else
                        {
                            kind = 4;
                        }
                    }
                    else
                    {
                        if (state.levels[ncoord] == 1)
                        {
                            kind = 2;
                        }
                        else
                        {
                            kind = 3;
                        }
                    }

                    if (kind < 4)
                    {
                        uint32_t value = problem.values[ncoord];
                        uint64_t score = (uint64_t)players[m].weights[kind] * (uint64_t)value;
                        if (score > bestScore)
                        {
                            bestScore = score;
                            bestCoord = ncoord;
                            bestR = (uint32_t)ncoord * r2mul + r2add;
                        }
                        else if (score == bestScore)
                        {
                            uint32_t r = (uint32_t)ncoord * r2mul + r2add;
                            if (r > bestR)
                            {
                                bestR = r;
                                bestCoord = ncoord;
                            }
                        }
                    }
                }
            }

            return bestCoord;
        }
    }

    virtual uint64_t step(const Problem& problem) override
    {
        startTimeMS = getTimeMS();

        uint64_t M = problem.M;
        uint64_t U = problem.U;

        State initState;

        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            initState.owners[coord] = problem.owners[coord];
        }
        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            initState.levels[coord] = problem.levels[coord];
        }
        for (uint64_t m = 0; m < M; m++)
        {
            initState.pieces[m] = problem.pieces[m];
        }
        for (uint64_t m = 0; m < M; m++)
        {
            initState.scores[m] = 0;
        }
        for (uint64_t coord = 0; coord < N * N; coord++)
        {
            uint8_t owner = initState.owners[coord];
            if (owner == 255) continue;
            uint32_t value = problem.values[coord];
            uint8_t level = initState.levels[coord];
            initState.scores[owner] += value * level;
        }

        for (uint64_t m = 1; m < M; m++)
        {
            auto& player = players[m];
            auto& moves = topMoves[m];

            if (problem.turn == 0)
            {
                player.epsSum = 0.3 * 4.0;
                player.eps = 0.3;

                for (uint64_t ia = 0; ia < 7; ia++)
                {
                    for (uint64_t ib = 0; ib < 7; ib++)
                    {
                        for (uint64_t ic = 0; ic < 7; ic++)
                        {
                            for (uint64_t id = 0; id < 7; id++)
                            {
                                player.weightCounts[ia][ib][ic][id] = 0;
                            }
                        }
                    }
                }

                for (uint64_t kind = 0; kind < 4; kind++)
                {
                    player.weights[kind] = 1;
                }
            }
            else
            {
                uint64_t coord = problem.moves[m];
                uint64_t kind = moves.grid[coord];
                uint32_t value = problem.values[coord];
                bool isBest = (kind < 4 && value == moves.bestValues[kind]);

                if (!isBest)
                {
                    uint64_t moveCount = moves.moveCount;
                    uint64_t nonBestCount = moveCount;
                    nonBestCount -= moves.bestCounts[0];
                    nonBestCount -= moves.bestCounts[1];
                    nonBestCount -= moves.bestCounts[2];
                    nonBestCount -= moves.bestCounts[3];
                    player.epsSum += (double)moveCount / (double)nonBestCount;
                }

                double eps = player.epsSum / (double)(problem.turn + 4);
                if (eps < 0.1) eps = 0.1;
                else if (eps > 0.5) eps = 0.5;
                player.eps = eps;

                if (isBest)
                {
                    uint8_t maxCount = 0;

                    uint8_t ws[4];
                    ws[0] = 35;
                    for (uint64_t ia = 0; ia < 7; ia++, ws[0] += 10)
                    {
                        ws[1] = 35;
                        for (uint64_t ib = 0; ib < 7; ib++, ws[1] += 10)
                        {
                            ws[2] = 35;
                            for (uint64_t ic = 0; ic < 7; ic++, ws[2] += 10)
                            {
                                ws[3] = 35;
                                for (uint64_t id = 0; id < 7; id++, ws[3] += 10)
                                {
                                    bool ok = true;

                                    for (uint64_t kind2 = 0; kind2 < 4; kind2++)
                                    {
                                        if (kind2 == kind) continue;
                                        if (moves.bestCounts[kind2] == 0) continue;
                                        if ((uint64_t)ws[kind] * moves.bestValues[kind] < (uint64_t)ws[kind2] * moves.bestValues[kind2])
                                        {
                                            ok = false;
                                            break;
                                        }
                                    }

                                    if (ok)
                                    {
                                        player.weightCounts[ia][ib][ic][id]++;
                                    }

                                    maxCount = std::max(maxCount, player.weightCounts[ia][ib][ic][id]);
                                }
                            }
                        }
                    }

                    uint32_t sums[4] = {};
                    ws[0] = 35;
                    for (uint64_t ia = 0; ia < 7; ia++, ws[0] += 10)
                    {
                        ws[1] = 35;
                        for (uint64_t ib = 0; ib < 7; ib++, ws[1] += 10)
                        {
                            ws[2] = 35;
                            for (uint64_t ic = 0; ic < 7; ic++, ws[2] += 10)
                            {
                                ws[3] = 35;
                                for (uint64_t id = 0; id < 7; id++, ws[3] += 10)
                                {
                                    if (player.weightCounts[ia][ib][ic][id] == maxCount)
                                    {
                                        sums[0] += ws[0];
                                        sums[1] += ws[1];
                                        sums[2] += ws[2];
                                        sums[3] += ws[3];
                                    }
                                }
                            }
                        }
                    }

                    for (uint64_t kind = 0; kind < 4; kind++)
                    {
                        player.weights[kind] = sums[kind];
                    }
                }
            }
        }

#if 0
        printf("==============================\n");
        for (uint64_t m = 1; m < M; m++)
        {
            auto& player = players[m];
            printf("m %" PRIu64 ", turn %2" PRIu64 ":", m, problem.turn);
            for (uint64_t kind = 0; kind < 4; kind++)
            {
                printf(" %8.6f", player.weights[kind]);
            }
            printf("  %8.6f", player.eps);
            printf("\n");
        }
#endif

        getMyMoves(problem, initState, topMoves[0]);
        for (uint64_t m = 1; m < M; m++)
        {
            getEnemyMoves(problem, initState, m, topMoves[m]);
        }

        uint64_t firstMoveCount = topMoves[0].moveCount;

        //printf("turn %2" PRIu64 ", moveCount %2" PRIu64 "\n", problem.turn, firstMoveCount);

        uint64_t moveScoreSums[N * N];
        for (uint64_t firstMoveIdx = 0; firstMoveIdx < firstMoveCount; firstMoveIdx++)
        {
            moveScoreSums[firstMoveIdx] = 0;
        }

        uint64_t stopDurationMS = 1950;
#ifdef LOCAL
        stopDurationMS /= 2;
#endif

        if (problem.turn < 20) stopDurationMS /= 2;

        uint64_t limitDurationMS = (elapsedMS < stopDurationMS) ? (stopDurationMS - elapsedMS) / (T - problem.turn) : (uint64_t)0;
        uint64_t elimLimitDurationMS = (limitDurationMS + 1) / 2;
        bool elimDone = false;

        uint8_t elim[N * N];
        for (uint64_t firstMoveIdx = 0; firstMoveIdx < firstMoveCount; firstMoveIdx++)
        {
            elim[firstMoveIdx] = 0;
        }

        uint64_t iter = 0;
        for (; iter < 4096; iter++)
        {
            if (iter != 0)
            {
                uint64_t durationMS = getTimeMS() - startTimeMS;
                if (durationMS >= limitDurationMS)
                {
                    break;
                }

                if (!elimDone && durationMS >= elimLimitDurationMS)
                {
                    uint8_t order[N * N];
                    for (uint64_t firstMoveIdx = 0; firstMoveIdx < firstMoveCount; firstMoveIdx++)
                    {
                        order[firstMoveIdx] = firstMoveIdx;
                    }

                    uint64_t keepCount = (firstMoveCount + 7) / 8;
                    keepCount = std::max(keepCount, (uint64_t)2);
                    keepCount = std::min(keepCount, firstMoveCount);

                    auto elimComp = [&](uint8_t idxA, uint8_t idxB) -> bool
                    {
                        uint64_t moveScoreSumA = moveScoreSums[idxA];
                        uint64_t moveScoreSumB = moveScoreSums[idxB];
                        if (moveScoreSumA != moveScoreSumB)
                        {
                            return moveScoreSumA > moveScoreSumB;
                        }
                        return idxA < idxB;
                    };

                    std::nth_element(&order[0], &order[0] + (keepCount - 1), &order[0] + firstMoveCount, elimComp);

                    for (uint64_t orderIdx = keepCount; orderIdx < firstMoveCount; orderIdx++)
                    {
                        elim[order[orderIdx]] = 1;
                    }

                    elimDone = true;
                }
            }

            static constexpr const uint64_t maxTurnLimit = 14;
            uint64_t turnLimit = (M == 2) ? 10 : 14;
            turnLimit = std::min(turnLimit, T - problem.turn);

            Rand rands[maxTurnLimit][maxM];

            for (uint64_t turn = 0; turn < turnLimit; turn++)
            {
                for (uint64_t m = 0; m < M; m++)
                {
                    auto& rand = rands[turn][m];
                    rand.r1 = fastRandomDouble(gen);
                    uint64_t r2 = gen();
                    rand.r2mul = (uint32_t)r2;
                    rand.r2add = (uint32_t)(r2 >> 32);
                }
            }

            uint8_t topCoords[maxM];
            for (uint64_t m = 1; m < M; m++)
            {
                topCoords[m] = getEnemyMove(problem, initState, m, rands[0][m]);
            }

            for (uint64_t firstMoveIdx = 0; firstMoveIdx < firstMoveCount; firstMoveIdx++)
            {
                if (elim[firstMoveIdx]) continue;

                State state(initState);

                for (uint64_t turn = 0; turn < 1; turn++)
                {
                    topCoords[0] = topMoves[0].moves[firstMoveIdx];

                    uint8_t removed[maxM];

                    for (uint64_t m = 0; m < M; m++)
                    {
                        removed[m] = 0;
                        uint64_t coord = topCoords[m];
                        if (state.owners[coord] == m) continue;
                        for (uint64_t m2 = 0; m2 < M; m2++)
                        {
                            if (m2 == m) continue;
                            uint64_t coord2 = topCoords[m2];
                            if (coord2 == coord)
                            {
                                removed[m] = 1;
                                break;
                            }
                        }
                    }

                    for (uint64_t m = 0; m < M; m++)
                    {
                        if (removed[m]) continue;
                        uint64_t coord = topCoords[m];
                        uint64_t owner = state.owners[coord];
                        if (owner == m)
                        {
                            if (state.levels[coord] < U)
                            {
                                state.levels[coord]++;
                                state.scores[m] += problem.values[coord];
                            }
                        }
                        else if (owner == 255)
                        {
                            state.owners[coord] = m;
                            state.levels[coord] = 1;
                            state.scores[m] += problem.values[coord];
                        }
                        else
                        {
                            state.scores[owner] -= problem.values[coord];
                            if (state.levels[coord] > 1)
                            {
                                state.levels[coord]--;
                                continue;
                            }
                            state.owners[coord] = m;
                            state.scores[m] += problem.values[coord];
                        }
                        state.pieces[m] = coord;
                    }
                }

                for (uint64_t turn = 1; turn < turnLimit; turn++)
                {
                    uint8_t coords[maxM];
                    for (uint64_t m = 1; m < M; m++)
                    {
                        coords[m] = getEnemyMove(problem, state, m, rands[turn][m]);
                    }

                    getMyMoves(problem, state, myMoves);
                    uint64_t myMoveCount = myMoves.moveCount;

                    double randomEps = (M == 2) ? 0.15 : 0.30;

                    if (rands[turn][0].r1 < randomEps)
                    {
                        auto& rand = rands[turn][0];
                        uint32_t bestCoord = 0;
                        uint32_t bestRand = 0;
                        for (uint64_t moveIdx = 0; moveIdx < myMoveCount; moveIdx++)
                        {
                            uint32_t coord = myMoves.moves[moveIdx];
                            uint32_t r = coord * rand.r2mul + rand.r2add;
                            if (r > bestRand)
                            {
                                bestRand = r;
                                bestCoord = coord;
                            }
                        }
                        coords[0] = bestCoord;
                    }
                    else
                    {
                        uint32_t scores[maxM];
                        for (uint64_t m = 0; m < M; m++)
                        {
                            scores[m] = state.scores[m];
                        }

                        int8_t scoreDeltas[N * N][maxM] = {};
                        for (uint64_t myMoveIdx = 0; myMoveIdx < myMoveCount; myMoveIdx++)
                        {
                            uint64_t coord = myMoves.moves[myMoveIdx];
                            uint64_t owner = state.owners[coord];
                            if (owner != 0)
                            {
                                bool myRemoved = false;
                                for (uint64_t m2 = 1; m2 < M; m2++)
                                {
                                    uint64_t coord2 = coords[m2];
                                    if (coord2 == coord)
                                    {
                                        myRemoved = true;
                                        break;
                                    }
                                }
                                if (myRemoved) continue;
                            }

                            if (owner == 0)
                            {
                                if (state.levels[coord] < U)
                                {
                                    scoreDeltas[coord][0]++;
                                }
                            }
                            else if (owner == 255)
                            {
                                scoreDeltas[coord][0]++;
                            }
                            else
                            {
                                scoreDeltas[coord][owner]--;
                                if (state.levels[coord] == 1)
                                {
                                    scoreDeltas[coord][0]++;
                                }
                            }
                        }

                        for (uint64_t m = 1; m < M; m++)
                        {
                            uint64_t coord = coords[m];
                            uint64_t owner = state.owners[coord];
                            if (owner != m)
                            {
                                bool removed = false;
                                for (uint64_t m2 = 1; m2 < M; m2++)
                                {
                                    if (m2 == m) continue;
                                    uint64_t coord2 = coords[m2];
                                    if (coord2 == coord)
                                    {
                                        removed = true;
                                        break;
                                    }
                                }
                                if (removed) continue;
                            }

                            if (owner == m)
                            {
                                if (state.levels[coord] < U)
                                {
                                    scores[m] += problem.values[coord];
                                }
                            }
                            else if (owner == 255)
                            {
                                scores[m] += problem.values[coord];
                                scoreDeltas[coord][m]--;
                            }
                            else
                            {
                                scores[owner] -= problem.values[coord];
                                scoreDeltas[coord][owner]++;
                                if (state.levels[coord] == 1)
                                {
                                    scores[m] += problem.values[coord];
                                    scoreDeltas[coord][m]--;
                                }
                            }

                        }

                        uint64_t bestMoveIdx = 0;
                        uint32_t bestMoveScore = 0;

                        for (uint64_t myMoveIdx = 0; myMoveIdx < myMoveCount; myMoveIdx++)
                        {
                            uint64_t myCoord = myMoves.moves[myMoveIdx];
                            uint32_t myScore = scores[0] + (uint32_t)(int32_t)scoreDeltas[myCoord][0] * problem.values[myCoord];

                            uint32_t bestEnemyScore = 0;
                            for (uint64_t m = 1; m < M; m++)
                            {
                                uint32_t enemyScore = scores[m] + (uint32_t)(int32_t)scoreDeltas[myCoord][m] * problem.values[myCoord];
                                bestEnemyScore = std::max(bestEnemyScore, enemyScore);
                            }

                            double score = log2Table[myScore + bestEnemyScore] - log2Table[bestEnemyScore];
                            uint32_t moveScore = (uint32_t)(score + 0.5);
                            if (moveScore > bestMoveScore)
                            {
                                bestMoveScore = moveScore;
                                bestMoveIdx = myMoveIdx;
                            }
                        }

                        coords[0] = myMoves.moves[bestMoveIdx];
                    }

                    for (uint64_t m = 0; m < M; m++)
                    {
                        uint64_t coord = coords[m];
                        uint64_t owner = state.owners[coord];
                        if (owner != m)
                        {
                            bool removed = false;
                            for (uint64_t m2 = 0; m2 < M; m2++)
                            {
                                if (m2 == m) continue;
                                uint64_t coord2 = coords[m2];
                                if (coord2 == coord)
                                {
                                    removed = true;
                                    break;
                                }
                            }
                            if (removed) continue;
                        }

                        if (owner == m)
                        {
                            if (state.levels[coord] < U)
                            {
                                state.levels[coord]++;
                                state.scores[m] += problem.values[coord];
                            }
                        }
                        else if (owner == 255)
                        {
                            state.owners[coord] = m;
                            state.levels[coord] = 1;
                            state.scores[m] += problem.values[coord];
                        }
                        else
                        {
                            state.scores[owner] -= problem.values[coord];
                            if (state.levels[coord] > 1)
                            {
                                state.levels[coord]--;
                                continue;
                            }
                            state.owners[coord] = m;
                            state.scores[m] += problem.values[coord];
                        }
                        state.pieces[m] = coord;
                    }
                }

                uint32_t bestEnemyScore = 0;
                for (uint64_t m = 1; m < M; m++)
                {
                    bestEnemyScore = std::max(bestEnemyScore, state.scores[m]);
                }
                double score = log2Table[state.scores[0] + bestEnemyScore] - log2Table[bestEnemyScore];
                uint32_t moveScore = (uint32_t)(score + 0.5);
                moveScoreSums[firstMoveIdx] += moveScore;

                //printf("turn %2" PRIu64 "  iter %3" PRIu64 "  move %02" PRIu8 "  score %6" PRIu32 "\n", problem.turn, iter, topMoves[0].moves[firstMoveIdx], moveScore);
            }
        }

        //printf("turn %2" PRIu64 ": %4" PRIu64 " iters\n", problem.turn, iter);

        uint64_t bestFirstMoveIdx = 0;
        uint64_t bestMoveScoreSum = 0;
        for (uint64_t firstMoveIdx = 0; firstMoveIdx < firstMoveCount; firstMoveIdx++)
        {
            uint64_t moveScoreSum = moveScoreSums[firstMoveIdx];
            if (moveScoreSum > bestMoveScoreSum)
            {
                bestMoveScoreSum = moveScoreSum;
                bestFirstMoveIdx = firstMoveIdx;
            }
            //printf("turn %2" PRIu64 "            move %02" PRIu8 "  score %6" PRIu64 "\n", problem.turn, topMoves[0].moves[firstMoveIdx], moveScoreSum / iterCount);
        }

        //printf("turn %2" PRIu64 ", %6" PRIu64 "\n", problem.turn, bestMoveScoreSum / iterCount);

        uint64_t endTimeMS = getTimeMS();
        elapsedMS += endTimeMS - startTimeMS;

        return topMoves[0].moves[bestFirstMoveIdx];
    }

private:
    Gen gen;
    unique_ptr<double[]> log2Table;
    uint64_t elapsedMS;
    uint64_t startTimeMS;
    Moves topMoves[maxM];
    Player players[maxM];
    Moves myMoves;
};

unique_ptr<Solver> createSolver(const string& solverName)
{
    unique_ptr<Solver> pSolver;

    if (solverName == "trivial") pSolver.reset(new TrivialSolver);
    else if (solverName == "random") pSolver.reset(new RandomSolver);
    else if (solverName == "greedy") pSolver.reset(new GreedySolver);
    else if (solverName == "hybrid") pSolver.reset(new HybridSolver);
    else if (solverName == "mcts") pSolver.reset(new MctsSolver);
    else if (solverName == "mcts2") pSolver.reset(new Mcts2Solver);
    else if (solverName == "mcts3") pSolver.reset(new Mcts3Solver);

    return pSolver;
}

constexpr const uint64_t maxSeedCount = 10000;

int listProblems()
{
    printf("seed  M  U\n");
    printf("----  -  -\n");
    for (uint64_t seed = 0; seed < maxSeedCount; seed++)
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

        printf("%1" PRIu64 "  %1" PRIu64 "\n", problem.M, problem.U);
    }
    return 0;
}

class Result
{
public:
    bool ok = true;
    string msg;
    uint64_t M = 0;
    uint64_t U = 0;
    uint64_t score = 0;
    uint64_t durationMS = 0;
};

class Totals
{
public:
    uint64_t totalScore = 0;
    uint64_t paramScores[maxM - minM + 1][maxU - minU + 1] = {};
    uint64_t paramCounts[maxM - minM + 1][maxU - minU + 1] = {};
    uint64_t maxDurationMS = 0;
};

void addTotals(Totals& totals, const Result& result)
{
    if (result.ok)
    {
        totals.totalScore += result.score;
        totals.paramScores[result.M - minM][result.U - minU] += result.score;
        totals.paramCounts[result.M - minM][result.U - minU]++;
    }
    totals.maxDurationMS = std::max(totals.maxDurationMS, result.durationMS);
}

void printResult(uint64_t seed, const Result& result)
{
    printf("%4" PRIu64 "  ", seed);
    if (!result.ok)
    {
        printf("%s\n", result.msg.c_str());
    }
    else
    {
        printf("%9" PRIu64 "  %4" PRIu64 " ms\n", result.score, result.durationMS);
    }
}

void printTotals(const Totals& totals, uint64_t seedCount)
{
    printf("      %9" PRIu64 "  %4" PRIu64 " ms\n", totals.totalScore / seedCount, totals.maxDurationMS);

    uint64_t scoreM[maxM - minM + 1] = {};
    uint64_t countM[maxM - minM + 1] = {};
    uint64_t scoreU[maxU - minU + 1] = {};
    uint64_t countU[maxU - minU + 1] = {};
    uint64_t scoreAll = 0;
    uint64_t countAll = 0;

    printf("\n");
    printf(" ");
    for (uint64_t U = minU; U <= maxU; U++)
    {
        printf("       %" PRIu64 "", U);
    }
    printf("\n");
    printf("\n");
    for (uint64_t M = minM; M <= maxM; M++)
    {
        printf("%" PRIu64 "", M);
        for (uint64_t U = minU; U <= maxU; U++)
        {
            uint64_t score = totals.paramScores[M - minM][U - minU];
            uint64_t count = totals.paramCounts[M - minM][U - minU];
            scoreM[M - minM] += score;
            countM[M - minM] += count;
            scoreU[U - minU] += score;
            countU[U - minU] += count;
            scoreAll += score;
            countAll += count;
            if (count != 0) score /= count;
            printf("  %6" PRIu64 "", score);
        }
        if (countM[M - minM] != 0) scoreM[M - minM] /= countM[M - minM];
        printf("    %6" PRIu64 "", scoreM[M - minM]);
        printf("\n");
    }
    printf("\n");
    printf(" ");
    for (uint64_t U = minU; U <= maxU; U++)
    {
        if (countU[U - minU] != 0) scoreU[U - minU] /= countU[U - minU];
        printf("  %6" PRIu64 "", scoreU[U - minU]);
    }
    if (countAll != 0) scoreAll /= countAll;
    printf("    %6" PRIu64 "", scoreAll);
    printf("\n");
}

Result runSeed(const string& solverName, uint64_t seed)
{
    Result result;
    result.ok = false;

    // Create solver

    unique_ptr<Solver> pSolver = createSolver(solverName);
    if (!pSolver)
    {
        result.msg = "Unknown solver";
        return result;
    }

    // Read input

    Problem problem;
    Ai ai;
    {
        string fileName = strprintf("../../../in/%04" PRIu64 ".txt", seed);
        FILE* f = fopen(fileName.c_str(), "rb");
        if (!f)
        {
            result.msg = "Error opening input file";
            return result;
        }
        readProblem(f, problem, &ai);
        fclose(f);
    }

    // Calculate score

    result.M = problem.M;
    result.U = problem.U;
    result.score = calcScore(problem, ai, pSolver.get(), &result.durationMS);

    result.ok = true;
    return result;
}

int runTournament(const string& solverName, uint64_t seedCount, uint64_t threadCount)
{
    vector<Promise<Result>> promises(seedCount);
    vector<Thread> threads(threadCount);

    Mutex mutex;
    uint64_t seedToStart = 0;

    auto threadFn = [&]()
    {
        while (true)
        {
            uint64_t seed;
            {
                LockGuard<Mutex> lock(mutex);
                if (seedToStart == seedCount) break;
                seed = seedToStart++;
            }

            promises[seed].setValue(runSeed(solverName, seed));
        }
    };

    for (uint64_t threadIdx = 0; threadIdx < threadCount; threadIdx++)
    {
        threads[threadIdx] = Thread(threadFn);
    }

    Totals totals;
    for (uint64_t seed = 0; seed < seedCount; seed++)
    {
        Result result = promises[seed].getFuture().get();
        printResult(seed, result);
        addTotals(totals, result);
    }
    printTotals(totals, seedCount);

    for (uint64_t threadIdx = 0; threadIdx < threadCount; threadIdx++)
    {
        threads[threadIdx].join();
    }

    return 0;
}

constexpr const char* defaultSolverName = "mcts3";

void usage()
{
    printf("Usage: solve [<options>] [<solver>]\n");
    printf("  <solver>\n");
    printf("        Solver to use (default: %s)\n", defaultSolverName);
    printf("Options:\n");
    printf("  -h    Print usage information and exit\n");
    printf("  -l    List problems\n");
    printf("  -t <n>\n");
    printf("        Tournament mode: run solver on <n> seeds (1-%" PRIu64 ")\n", maxSeedCount);
    printf("  -T <n>\n");
    printf("        Use <n> threads in tournament mode (1-256, default: 1)\n");
;
    printf("  -v    Verbose: don't suppress debug output in tournament mode\n");
    printf("\n");
    printf("Solver      Description\n");
    printf("----------  ------------------------------------------------------------\n");
    printf("trivial     Always move to the starting location\n");
    printf("random      Make random moves\n");
    printf("greedy      Look one move ahead with heuristics\n");
    printf("hybrid      Use different expectimax solvers for M = 2 and M > 2\n");
    printf("mcts        Evaluate each move using multiple rollouts\n");
    printf("mcts2       Tweaks\n");
    printf("mcts3       More tweaks\n");
}

int main(int argc, char *argv[])
{
    bool gotThreadCount = false;
    bool gotSolverName = false;

    bool help = false;
    bool list = false;
    bool tourney = false;
    uint64_t seedCount = 1;
    uint64_t threadCount = 1;
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
            if (iArg >= argc)
            {
                usage();
                return 1;
            }
            strArg = argv[iArg++];
            if (!parseU64(strArg, &seedCount) || seedCount < 1 || seedCount > maxSeedCount)
            {
                usage();
                return 1;
            }
            tourney = true;
        }
        else if (strArg == "-T")
        {
            if (iArg >= argc || gotThreadCount)
            {
                usage();
                return 1;
            }
            strArg = argv[iArg++];
            if (!parseU64(strArg, &threadCount) || threadCount < 1 || threadCount > 256)
            {
                usage();
                return 1;
            }
            gotThreadCount = true;
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

        return runTournament(solverName, seedCount, threadCount);
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

    for (uint64_t turn = 0; turn < T; turn++)
    {
        // Initialize

        if (turn == 0)
        {
            pSolver->init(problem);
        }

        // Calculate move

        uint64_t move = pSolver->step(problem);

        // Write move

        writeMove(stdout, move);
        fflush(stdout);

        // Read new state

        readStep(stdin, problem);
    }

    return 0;
}
