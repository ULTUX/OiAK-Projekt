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
struct Element
{
    Element *next;
    uint64_t t;
};

/**
 * @brief Begin access time calculation and return this time based on input parameters.
 * This function generates vector of Element structure and saves it into the memory. Then it performs read from memory.
 * @param size Amount of nodes to be created.
 * @param repeats Number of repetitions for better result accuracy.
 * @param isSequential If the data should be written to memory sequentially.
 * @return Access time in nanoseconds.
 */
float volatile calcAccessTime(uint64_t size, volatile int repeats, bool isSequential)
{   
    //Declare variables
    unsigned int a;
    volatile uint64_t startTime, stopTime, error;
    volatile uint64_t took = 0;

    //Create vector of input size containing all nodes.
    vector<Element> mem(size);

    //Create vector of node pointers.
    vector<Element*> elementPtr(size);
    
    //Iterate through every node and node pointer, assign addres of every node to pointer.
    for (int i = 0; i < size; i++){
        elementPtr[i] = &mem[i];
    }

    //Depending on input parameter shuffle the nodes.
    if (!isSequential) random_shuffle(begin(elementPtr), end(elementPtr));

    //Connect all nodes together as single linked list.
    for (uint64_t i = 0; i < size-1; i++) {
        elementPtr[i]->next = elementPtr[i+1];
    }

    //Get first node
    Element* head = elementPtr[0];
    
    //Remove redundant node pointers.
    elementPtr.clear();
    elementPtr.shrink_to_fit();

    //Get current error rate.
    startTime = __rdtscp(&a);
    stopTime = __rdtscp(&a);
    error = stopTime - startTime;
    
    //Start the tests, travers through linked list and repeat this.
    startTime = __rdtscp(&a);
    for (int i = 0; i < repeats; i++){
        Element* headd = head;
        while (headd) headd = headd->next;
    }
    stopTime = __rdtscp(&a);

    //Calculate result
    stopTime -= startTime;
    took = stopTime;
    took = took - error;

    return (took)/((double)repeats*size);
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

    //Current temporary iteration size
    float size = startSizeInKB;

    //Iteration number to be calculated.
    int itNum = 0;

    // Calculate iteration number for tests.
    while (size < endSizeInKB) {
        size *= mult;
        size += step;
        itNum++;
    }

    //Array containing resulting data.
    double NsLatencyResult[itNum];

    //Header names for .csv writer.
    string CSVheaders[] = {"Size [KB]", "Latency per mem access [ns]"};

    //Array containing corresponding size for .csv writer. 
    double sizeArra[itNum];

    //Current iteration size for tests.
    int currSize = startSizeInKB;

    //Start looping every iteration number and begin test for every size.
    for (int i = 0; i < itNum; i++){

        //Amount of Element structures that will take up desired iteration size.
        int nodeAmount = (1024*currSize)/sizeof(Element);

        cout<<"Iteration: "<<i<<" out of: "<<itNum<<endl;

        //Run the test, get test result.
        float cyclesTaken = calcAccessTime(nodeAmount, repeats, isSequential);

        //Comvert clock cycles to value in ns.
        float inNs = cyclesTaken / CPU_FREQ;

        //Push the values into result arrays.
        NsLatencyResult[i] = inNs;
        sizeArra[i] = nodeAmount*sizeof(Element)/(1024);

        //Increate size by iteration params.
        currSize *= mult;
        currSize += step;

    }

    //Write to csv.
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