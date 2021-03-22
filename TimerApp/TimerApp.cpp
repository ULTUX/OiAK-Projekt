#include <iostream>
#include <windows.h>
using namespace std;
int main()
{
    int sum = 0;
    __int32 subtime = 0;
    __asm {
        cpuid
        rdtsc
        mov subtime, eax
        cpuid
        rdtsc
        sub eax, subtime
        mov subtime, eax
        cpuid
        rdtsc
        mov subtime, eax
        cpuid
        rdtsc
        sub eax, subtime
        mov subtime, eax
        cpuid
        rdtsc
        mov subtime, eax
        cpuid
        rdtsc
        sub eax, subtime
        mov subtime, eax
    }
    for (int i = 0; i < 1000; i++) {
        int volatile a = 201;
        __int32 volatile time = 0;
        __asm {
            CPUID
            rdtsc
            mov time, eax
        }
        a++;
        for (int i = 0; i < 1000; i++) {}
        __asm {
            CPUID
            rdtsc
            sub eax, time // Find the difference
            mov time, eax
        }
        sum += time-subtime;
    }
    sum /= 20000-20;
    cout << sum <<endl<< (double)sum/3.56;
    return 0;
}