#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

void CSVWriter(float referenceCPUFrequency, int *cacheArray, int *noCacheArray, int testCount, int startingSize, int increment, double multiplier)
{
    ofstream stream("testResultTicks.csv");
    if (stream.is_open())
    {
        stream << "Size [kB]"
               << ","
               << "Cache"
               << ","
               << "No Cache" << endl;
        int currSize = startingSize;
        for (int i = 0; i < testCount; i++)
        {
            stream << currSize / 2000 << "," << cacheArray[i] << "," << noCacheArray[i] << endl;
            currSize *= multiplier;
            currSize += increment;
        }
    }
    stream.close();

    ofstream streamNs("testResultNs.csv");
    if (streamNs.is_open())
    {
        streamNs << "Size [kB]"
                 << ","
                 << "Cache [ns]"
                 << ","
                 << "No Cache [ns]" << endl;
        int currSize = startingSize;
        for (int i = 0; i < testCount; i++)
        {
            streamNs.precision(0);
            streamNs << currSize / 2000 << "," << fixed << cacheArray[i] / referenceCPUFrequency << "," << (float)noCacheArray[i] / referenceCPUFrequency << endl;
            currSize *= multiplier;
            currSize += increment;
        }
    }
    streamNs.close();
}