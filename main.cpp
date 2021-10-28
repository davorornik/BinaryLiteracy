#include "binreader.h"
#include "binwriter.h"

int main()
{
    // Example where we copy a file byte by byte
    auto *reader = new BinReader<>("test.txt");
    auto *writer = new BinWriter<>("out.txt");

    char byte;
    while (reader->canRead())
    {
        byte = reader->readByte();
        writer->writeByte(byte);
    }

    delete reader;
    delete writer;
    return 0;
}
