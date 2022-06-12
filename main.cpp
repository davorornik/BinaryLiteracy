#include "binreader.h"
#include "binwriter.h"
#include <iostream>

int main() {
    // a simple example of copying a file
    auto *reader = new BinReaderMSB("test.txt");
    auto *writer = new BinWriterMSB("out.txt");
    while (reader->canRead()) {
        writer->writeByte(reader->readByte());
    }


    delete reader;
    delete writer;
    return 0;
}
