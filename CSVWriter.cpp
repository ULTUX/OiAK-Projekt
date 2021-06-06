
#include "CSVWriter.h"

CSVWriter::CSVWriter(int dataSize, int columnWidth,
                     string* headers, double** data) :dataSize(dataSize),
                                                    columnWidth(columnWidth), headers(headers), data(data), stream("results.csv") {
    writeData();
}

void CSVWriter::writeData() {
    for (int i = 0; i < columnWidth; i++){
        stream<<headers[i];
        if (i+1 < columnWidth) stream<<",";
    }
    stream<<endl;
    for (int i = 0; i < dataSize; i++){
        for (int j = 0; j < columnWidth; j++){
            stream<<data[j][i];
            if (j+1 < columnWidth) stream<<",";
        }
        stream<<endl;
    }
}