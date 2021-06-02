
#ifndef OiAK_CSVWRITER_H
#define OiAK_CSVWRITER_H

#include <fstream>

using namespace std;

/**
 * Simple class that writes data to csv file. <br/>
 * This class is copy of class I created in previous project.
 */
class CSVWriter {
private:
    /**
     * Name of file to be written.
     */
    const string fileName;

    /**
     * Amount of data points.
     */
    const int dataSize;

    /**
     * Amount of corresponding data every data point has.
     */
    const int columnWidth;

    /**
     * 2D array of data.
     */
    double** data;

    /**
     * Column names.
     */
    string * headers;

    /**
     * File output stream.
     */
    ofstream stream;

    /**
     * Write data to file.
     */
    void writeData();
public:
    CSVWriter(int dataSize, int columnWidth, string* headers, double** data);

};


#endif //OiAK_CSVWRITER_H