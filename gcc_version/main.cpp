#include <immintrin.h>
#include <cstdio>
#include <cinttypes>
#include <iostream>
#include <intrin.h>

using namespace std;

int *Array;
//Clock frequency in Ghz
const float BASE_CLOCK = 3.56;
const int SIZE = 78643200; // Must divide by 4
//300MB
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

void initializeWithoutCache()
{
    Array = new int[SIZE];
    while (((uintptr_t)Array) % 16 != 0)
    {
        delete[] Array;
        Array = new int[SIZE];
    }
    //Load data to mem
    for (int i = 0; i < SIZE / 4; i++)
    {
        __m128i vals = _mm_setr_epi32(4 * i + 0, 4 * i + 1, 4 * i + 2, 4 * i + 3);
        _mm_stream_si128((__m128i *)Array + i, vals);
    }
}

void initializeWithCache()
{
    Array = new int[SIZE];
    while (((uintptr_t)Array) % 16 != 0)
    {
        delete[] Array;
        Array = new int[SIZE];
    }

    for (int i = 0; i < SIZE; i++)
    {
        Array[i] = i;
        if (i % 4 == 0 && i > 0)
        {
            __m128i testVal = _mm_loadu_si128(((__m128i *)Array) + (i - 4) / 4);
        }
    }
}

volatile uint64_t testWithoutCache()
{
    volatile uint64_t cycles;
    volatile int i = 0;
    __m128i data;

    cycles = start();
    while (i < SIZE / 4)
    {
        __m128i data = _mm_stream_load_si128((__m128i *)(Array) + i);
        i++;
    }
    cycles = stop(cycles);
    return cycles;
}
volatile uint64_t testWithCache()
{
    volatile uint64_t cycles;
    volatile int i = 0;
    __m128i data;
    //First run to make sure data is in cache
    while (i < SIZE / 4)
    {
        data = _mm_load_si128((__m128i *)(Array) + i);
        i++;
    }
    i = 0;
    cycles = start();
    while (i < SIZE / 4)
    {
        data = _mm_load_si128((__m128i *)(Array) + i);
        i++;
    }
    cycles = stop(cycles);
    return cycles;
}

int main()
{
    initializeWithCache();
    // initializeWithoutCache();
    volatile uint64_t base = 0;
    volatile uint64_t latency = 0;
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
    cout << "The base latency is: " << base << endl;
    // Start the test
    latency = testWithCache();
    // latency = testWithoutCache();
    latency -= base;
    printf("Latency:.............%d clock cycles (%f ns)", latency, latency / BASE_CLOCK);

    return 0;
}
