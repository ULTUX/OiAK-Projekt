#include <iostream>
#include <x86intrin.h>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <algorithm>
#include <string>
#include "CSVWriter.h"


using namespace std;

typedef unsigned char BYTE;

const int SIZE = 4*1024;
const int REPEATS = 1000;
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
    took = took - error;
    took = (float)took/(float)REPEATS;

    return (took);
}

void test(float startSizeInKB, float endSizeInKB, int step, float mult) {

    float size = startSizeInKB;

    int itNum = 0;

    while (size < endSizeInKB) {
        size *= mult;
        size += step;
        itNum++;
    }
    if (size > endSizeInKB) itNum--;

    double GBperSResult[itNum];
    double NsLatencyResult[itNum];
    string CSVheaders[] = {"Size [KB]", "RAM Speed [GB/s]", "Latency per mem access [ns]"};
    double sizeArra[itNum];
    int currSize = startSizeInKB;
    for (int i = 0; i < itNum; i++){
        volatile int num = (1024*currSize)/(SIZE*1.0);
        cout<<"About to test on "<<num<<" nodes."<<endl;
        vector<Node> list(num);
        for (int i = 0; i < num - 1; i++)
            list[i].next = &list[i + 1];
        list[list.size() - 1].next = &list[0];
        random_shuffle(begin(list), end(list));
        list.shrink_to_fit();
        cout << "Finished preparing nodes, testing..."<<endl;

        uint64_t cyclesTaken = traverse_list(&(list[0]), num);
        
        float inNs = cyclesTaken / CPU_FREQ;
        float transferSpeed = currSize*1000;
        transferSpeed /= cyclesTaken;
        cout << "Took: cycles: " << cyclesTaken <<" | nanosecs: "<< inNs<< ".\nTransfer speed (GB/s): " << transferSpeed << endl;
        GBperSResult[i] = transferSpeed;
        NsLatencyResult[i] = inNs/num;
        sizeArra[i] = currSize;

        currSize *= mult;
        currSize += step;
    }
    double** data = new double*[3];
    data[0] = sizeArra;
    data[1] = GBperSResult;
    data[2] = NsLatencyResult;

    CSVWriter writer(itNum, 3, CSVheaders, data);

}

int main()
{
    test(30, 1000000, 300, 1.1);
    return 0;
}