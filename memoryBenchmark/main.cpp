#include <iostream>
#include <x86intrin.h>
#include <vector>
#include <algorithm>
#include <string>
#include "CSVWriter.h"


using namespace std;

typedef unsigned char BYTE;

/**
 * @brief Current cpu frequency in GHz (should be modified depending on the machine).
 */
const float CPU_FREQ = 3.57;    //Current cpu frequency - SHOULD BE CHANGED

//Declaration of structured that will be saved into the memory (every node has two 64-bit variables)
struct Node
{
    Node *next;
    uint64_t t;
};

/**
 * @brief Begin access time calculation and return this time based on input parameters.
 * This function generates vector of Node structure and saves it into the memory. Then it performs read from memory.
 * @param size Amount of nodes to be created.
 * @param num_ops Number of repetitions for better result accuracy.
 * @param isSequential If the data should be written to memory sequentially.
 * @return Access time in nanoseconds.
 */
float volatile calcAccessTime(uint64_t size, volatile int num_ops, bool isSequential)
{
    unsigned int a;
    volatile uint64_t startTime, stopTime, error;
    volatile uint64_t took = 0;

    vector<Node> mem(size);

    vector<Node*> nodes(size);

    for (int i = 0; i < size; i++){
        nodes[i] = &mem[i];
    }

    if (!isSequential) random_shuffle(begin(nodes), end(nodes));

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

    return (took)/((double)num_ops*size);
}

/**
 * @brief Start tests based on input params.
 * This function performs tests based on input params and saves thge results into .csv file.
 * @param startSizeInKB Starting size in KB.
 * @param endSizeInKB Ending size in KB.
 * @param step Step to increase current size every iteration.
 * @param mult Value tobe multiplied by every iteration.
 * @param repeats Amount of repetitions.
 * @param isSequential Whether the memory reads should be squential.
 */
void test(float startSizeInKB, float endSizeInKB, int step, float mult, int repeats, bool isSequential) {

    float size = startSizeInKB;

    int itNum = 0;

    while (size < endSizeInKB) {
        size *= mult;
        size += step;
        itNum++;
    }

    double NsLatencyResult[itNum];
    string CSVheaders[] = {"Size [KB]", "Latency per mem access [ns]"};
    double sizeArra[itNum];

    int currSize = startSizeInKB;
    for (int i = 0; i < itNum; i++){
        int nodeAmount = (1024*currSize)/sizeof(Node);
        cout<<"Iteration: "<<i<<" out of: "<<itNum<<endl;

        float cyclesTaken = calcAccessTime(nodeAmount, repeats, isSequential);
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
    const int REPEATS = 1000;
    test(100, 550000, 100, 1.3, REPEATS, true);
    return 0;
}