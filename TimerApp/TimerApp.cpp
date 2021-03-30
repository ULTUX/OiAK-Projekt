#include <iostream>
#include <immintrin.h>
using namespace std;

int* Array;
//Clock frequency in Ghz
const float BASE_CLOCK = 3.56;
const int SIZE = 800; // Must divide by 4

__int32 start() {
    __int32  cycles;
    __asm {
        LFENCE
        RDTSC
        mov cycles, eax
    }
    return cycles;
}

__int32 stop(int startCycles) {
    __int32  cycles = startCycles;
    __asm {
        LFENCE
        RDTSC
        sub eax, cycles
        mov cycles, eax
    }
}

void initializeWithoutCache() {
    Array = new int[SIZE];
    while (((uintptr_t)Array) % 16 != 0) {
        delete[SIZE] Array;
        Array = new int[SIZE];
    }
    //Load data to mem
    for (int i = 0; i < SIZE/4; i++) {
        __m128i vals = _mm_setr_epi32(4 * i + 0, 4 * i + 1, 4 * i + 2, 4 * i + 3);
        _mm_stream_si128((__m128i*)Array + i, vals);
    }
}

void initializeWithCache() {
    Array = new int[SIZE];
    while (((uintptr_t)Array) % 16 != 0) {
        delete[SIZE] Array;
        Array = new int[SIZE];
    }

    for (int i = 0; i < SIZE; i++) {
        Array[i] = i;
        if (i % 4 == 0 && i > 0) {
            __m128i testVal = _mm_loadu_si128(((__m128i*)Array) + (i - 4) / 4);
        }
    }

}

__int32 testWithoutCache() {
    volatile __int32 cycles;
    volatile int i = 0;
   __m128i data;

    cycles = start();
    for (; i < SIZE/4; i++) {
        __m128i data = _mm_stream_load_si128((__m128i*)(Array) + i);
    }
    cycles = stop(cycles);
    return cycles;
}
volatile __int32 testWithCache() {
    volatile __int32 cycles;
    volatile int i = 0;
    __m128i data;
    //First run to make sure data is in cache
    for (; i < SIZE/4; i++) {
        data = _mm_load_si128((__m128i*)(Array) + i);
    }
    i = 0;
    cycles = start();
    for (; i < SIZE/4; i++) {
        data = _mm_load_si128((__m128i*)(Array) + i);
    }

    cycles = stop(cycles);
    return cycles;
}

int main()
{
    initializeWithCache();
    //initializeWithoutCache();
    unsigned base = 0;
    __int32 cyc = 0;
    volatile int latency = 0;
    //Cache warming for timer func
    __asm {
        LFENCE
        RDTSC
        mov cyc, eax
        LFENCE
        RDTSC
        sub eax, cyc
        mov base, eax
        LFENCE
        RDTSC
        mov cyc, eax
        LFENCE
        RDTSC
        sub eax, cyc
        mov base, eax
        LFENCE
        RDTSC
        mov cyc, eax
        LFENCE
        RDTSC
        sub eax, cyc
        mov base, eax
    }
    latency = testWithCache();
    //latency = testWithoutCache();
    printf("%d\n", latency);
    latency -= base;
    printf("Latency:.............%d clock cycles (%f ns)", latency, latency / BASE_CLOCK);

    return 0;
}
