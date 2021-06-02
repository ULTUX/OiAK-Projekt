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

const int SIZE = 8;
const int REPEATS = 10;
const float CPU_FREQ = 3.57;


struct Node
{
    Node *next;
    BYTE padding[SIZE];
};

float volatile traverse_list(uint64_t size, volatile int num_ops)
{
    volatile int num_ops_curr = num_ops*REPEATS;
    unsigned int a;
    volatile uint64_t startTime, stopTime, error;
    volatile uint64_t took = 0;

    vector<Node> mem(size);

    vector<Node*> nodes(size);

    for (int i = 0; i < size; i++){
        nodes[i] = &mem[i];
    }

    random_shuffle(begin(nodes), end(nodes));

    for (uint64_t i = 0; i < size-1; i++) {
        nodes[i]->next = nodes[i+1];
    }

    Node* head = nodes[0];

    nodes.clear();
    nodes.shrink_to_fit();

    startTime = __rdtscp(&a);
    stopTime = __rdtscp(&a);
    error = stopTime - startTime;
    
    startTime = __rdtscp(&a);
    for (int i = 0; i < num_ops; i++){
        Node* headd = head;
        while (headd) headd = headd->next;
    }
    stopTime = __rdtscp(&a);
    stopTime -= startTime;
    took = stopTime;
    took = took - error;

    return (took)/((double)num_ops*REPEATS*size);
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

    double NsLatencyResult[itNum];
    string CSVheaders[] = {"Size [KB]", "Latency per mem access [ns]"};
    double sizeArra[itNum];

    int currSize = startSizeInKB;
    for (int i = 0; i < itNum; i++){
        int nodeAmount = (1024*currSize)/sizeof(Node);
        cout<<"Iteration: "<<i<<" out of: "<<itNum<<endl;

        float cyclesTaken = traverse_list(nodeAmount, REPEATS);
        float inNs = cyclesTaken / CPU_FREQ;
        NsLatencyResult[i] = inNs;
        sizeArra[i] = nodeAmount*sizeof(Node)/(1024);

        currSize *= mult;
        currSize += step;

    }
    double** data = new double*[2];
    data[0] = sizeArra;
    data[1] = NsLatencyResult;

    CSVWriter writer(itNum, 2, CSVheaders, data);

}

int main()
{
    test(300, 1000000, 30, 1.1);
    return 0;
}