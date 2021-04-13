#include <immintrin.h>
#include <cstdio>
#include <cinttypes>
#include <iostream>

// Windows
#ifdef _WIN32

#include <intrin.h>
// Linux/GCC
#else
//#include <x86intrin.h>
uint64_t __rdtsc()
{
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc"
                         : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}
#endif

#include <cpuid.h>

using namespace std;

int *Array;

uint64_t start()
{
    uint64_t cycles = __rdtsc();
    return cycles;
}

uint64_t stop(uint64_t startCycles)
{
    uint64_t stopTime = __rdtsc();
    stopTime = stopTime - startCycles;
    return stopTime;
}

void initializeWithoutCache(int size)
{
    Array = new int[size];
    while (((uintptr_t)Array) % 16 != 0)
    {
        delete[] Array;
        Array = new int[size];
    }
    //Load data to mem
    for (int i = 0; i < size / 4; i++)
    {
        __m128i vals = _mm_setr_epi32(4 * i + 0, 4 * i + 1, 4 * i + 2, 4 * i + 3);
        _mm_stream_si128((__m128i *)Array + i, vals);
    }
}

void initializeWithCache(int size)
{
    Array = new int[size];
    while (((uintptr_t)Array) % 16 != 0)
    {
        delete[] Array;
        Array = new int[size];
    }

    for (int i = 0; i < size; i++)
    {
        Array[i] = i;
        if (i % 4 == 0 && i > 0)
        {
            __m128i testVal = _mm_loadu_si128(((__m128i *)Array) + (i - 4) / 4);
        }
    }
}

void deleteArray()
{
    delete Array;
}

volatile uint64_t testWithoutCache(int size)
{
    volatile uint64_t cycles;
    volatile int i = 0;
    __m128i data;

    cycles = start();
    while (i < size / 4)
    {
        __m128i data = _mm_stream_load_si128((__m128i *)(Array) + i);
        i++;
    }
    cycles = stop(cycles);
    return cycles;
}
volatile uint64_t testWithCache(int size)
{
    volatile uint64_t cycles;
    volatile int i = 0;
    __m128i data;
    //First run to make sure data is in cache
    while (i < size / 4)
    {
        data = _mm_load_si128((__m128i *)(Array) + i);
        i++;
    }
    i = 0;
    cycles = start();
    while (i < size / 4)
    {
        data = _mm_load_si128((__m128i *)(Array) + i);
        i++;
    }
    cycles = stop(cycles);
    return cycles;
}

uint64_t getBaseLatency()
{
    volatile uint64_t base = 0;
    //Cache warming for timer func
    uint64_t t1 = __rdtsc();
    uint64_t t2 = __rdtsc();
    t2 -= t1;
    t1 = __rdtsc();
    t2 = __rdtsc();
    t2 -= t1;
    t1 = __rdtsc();
    t2 = __rdtsc();
    base = t2 - t1;
    return base;
}

uint64_t testCache(int size)
{
    uint64_t base = getBaseLatency();
    initializeWithCache(size);
    uint64_t result = testWithCache(size);
    deleteArray();
    return result - base;
}

uint64_t testNoCache(int size)
{
    uint64_t base = getBaseLatency();
    initializeWithoutCache(size);
    uint64_t result = testWithoutCache(size);
    deleteArray();
    return result - base;
}

int *testMultiple(int startingSize, int increment, double multiplier, int testCount, bool cache)
{
    int *result = new int[testCount];
    int currSize = startingSize;
    for (int i = 0; i < testCount; i++)
    {
        uint64_t currResult = cache ? testCache(currSize) : testNoCache(currSize);
        result[i] = currResult;
        currSize *= multiplier;
        currSize += increment;
    }
    return result;
}