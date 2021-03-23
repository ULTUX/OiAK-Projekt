// Code for testing a small, stand-alone section of code
// for repeatable results.
// This code will work as is in Microsoft(R) Visual C++(R) 5.0 compiler.
// Simply add in your own code in the marked sections and compile
// it.
#include <stdio.h>

#define CPUID __asm __emit 0fh __asm __emit 0a2h
#define RDTSC __asm __emit 0fh __asm __emit 031h

__int32 TestFunc();

void main() {
	unsigned base = 0;
	__int32 cyc = 0;
	// The following tests run the basic cycle counter to find
	// the overhead associated with each cycle measurement.
	// It is run multiple times simply because the first call
	// to CPUID normally takes longer than subsequent calls.
	// Typically after the second run the results are 
	// consistent. It is run three times just to make sure.
	__asm {
		CPUID
		RDTSC
		mov cyc, eax
		CPUID
		RDTSC
		sub eax, cyc
		mov base, eax
		CPUID
		RDTSC
		mov cyc, eax
		CPUID
		RDTSC
		sub eax, cyc
		mov base, eax
		CPUID
		RDTSC
		mov cyc, eax
		CPUID
		RDTSC
		sub eax, cyc
		mov base, eax
	} // End inline assembly
	// This section calls the function that contains the user's
	// code. It is called multiple times to eliminate instruction
	// cache effects and get repeatable results.
	int sum = 0;
	int j = 0;
	for (int i = 0; i < 200; i++) {
		if (i >= 10) {
			sum += TestFunc();
			j++;
		}
	}
	printf("%d\n", sum);
	sum /= j;
	sum -= base;
	// By the second or third run, both data and instruction
	// cache effects should have been eliminated, and results
	// will be consistent.
	printf("Base : %d\n", base);
	printf("Cycle counts:\n");
	printf("%d\n", sum);
}

__int32 TestFunc() {
	// *** REPLACE THIS SECTION WITH YOUR OWN VARIABLES
	volatile __int32 cycles;
	volatile __int32 
	volatile  x;
	volatile int i;
	// Load the values here, not at creation, to make sure
	// the data is moved into the cache.
	cycles = 0;
	i = 0;
	x = 0;

	// *** END OF USER SECTION
	__asm {
		CPUID
		RDTSC
		mov cycles, eax
	}
	for (i = 0; i < 100; i++) {
		x++;
	}
	__asm {
		CPUID
		RDTSC
		sub eax, cycles
		mov cycles, eax
	}
	return cycles;
}
