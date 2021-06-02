#include <iostream>
#include <x86intrin.h>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <algorithm> 
using namespace std;

typedef unsigned char BYTE;

const int SIZE = 4*1024;
const int REPEATS = 1;
const float CPU_FREQ = 3.57;

struct Node
{
    Node *next;
    BYTE padding[SIZE];
};

uint64_t volatile traverse_list(volatile Node *head, volatile int num_ops)
{
    volatile int num_ops_curr = num_ops*REPEATS;
    unsigned int a;
    volatile uint64_t startTime, stopTime, error;
    volatile uint64_t took = 0;

    startTime = __rdtscp(&a);
    stopTime = __rdtscp(&a);
    error = stopTime - startTime;
    
    startTime = __rdtscp(&a);
    while (num_ops_curr--) head = head->next;
    stopTime = __rdtscp(&a);
    stopTime -= startTime;
    took = stopTime;
    cout<<"TOOK: "<<took<<endl;
    took = took - error;
    took = (float)took/(float)REPEATS;
    cout<<"After divide: "<<took<<endl;
    cout<<"ERROR: "<<error<<endl;

    return (took);
}

int main()
{
    volatile float testSizeInMB = 500;
    volatile int num = (1024*1024*testSizeInMB)/(SIZE*1.0);

    cout<<"About to test on "<<num<<" nodes."<<endl;
    vector<Node> list(num);
    for (int i = 0; i < num - 1; i++)
        list[i].next = &list[i + 1];
    list[list.size() - 1].next = &list[0];
    random_shuffle(begin(list), end(list));
    cout << "Finished preparing nodes, testing..."<<endl;

    uint64_t cyclesTaken = traverse_list(&(list[0]), num);
    
    float inNs = cyclesTaken / CPU_FREQ;
    float transferSpeed = testSizeInMB*1000000;
    transferSpeed /= cyclesTaken;
    cout << "Took: cycles: " << cyclesTaken <<" | nanosecs: "<< inNs<< ".\nTransfer speed (GB/s): " << transferSpeed << endl;
    return 0;
}