#include <iostream>
#include "Test.cpp"
#include "CSVWriter.cpp"
using namespace std;

int main()
{
    int count = 100;
    int startSize = 800;
    int increment = 0;
    double multiplier = 1.15;
    int *cacheTest = testMultiple(startSize, increment, multiplier, count, true);
    int *noCacheTest = testMultiple(startSize, increment, multiplier, count, false);

    CSVWriter(3.56, cacheTest, noCacheTest, count, startSize, increment, multiplier);
    return 0;
}