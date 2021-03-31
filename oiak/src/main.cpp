#include <stdint.h>
#include <iostream>
#include <smmintrin.h>
//  Windows
#ifdef _WIN32

#include <intrin.h>
uint64_t rdtsc(){
    return __rdtsc();
}

//  Linux/GCC
#else
//#include <x86intrin.h>
uint64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

#endif

const int SIZE = 800;
int *Array;

void initializeWithoutCache() {
    Array = new int[SIZE];
    while (((uintptr_t)Array) % 16 != 0) {
        delete[] Array;
        Array = new int[SIZE];
    }
    //Load data to mem
    for (int i = 0; i < SIZE/4; i++) {
        __m128i vals = _mm_setr_epi32(4 * i + 0, 4 * i + 1, 4 * i + 2, 4 * i + 3);
        _mm_stream_si128((__m128i*)Array + i, vals);
    }
}

uint64_t testWithoutCache() {
    volatile uint64_t cycles;
    volatile int i = 0;
    __m128i data;

    cycles = rdtsc();
    for (; i < SIZE/4; i++) {
        data = _mm_stream_load_si128((__m128i*)(Array) + i);
    }
    cycles = rdtsc() - cycles;
	cycles /= SIZE/4;
    return cycles;
}

int main(){
	initializeWithoutCache();
	std::cout << testWithoutCache() << std::endl;
	return 0;
}