#ifndef BINWRITER_H
#define BINWRITER_H
#include <algorithm>
#include <fstream>
#include <concepts>

template<typename std::streamsize buffer_size = 4096>
class BinWriter {
private:
    std::ofstream file;
    char *buffer;
    std::streamsize index;

    void tryUpdateBuffer()
    {
        if (index == buffer_size << 3)
        {
            file.write(buffer, buffer_size);
            index = 0;
            memset(buffer, 0, buffer_size);
        }
    }

public:
    BinWriter() = delete;

    explicit BinWriter(const std::string &path) :
            index(0)
    {
        static_assert((buffer_size > 0) & !(buffer_size & (buffer_size - 1)),
                      "Template parameter must be a power of two.");

        file.open(path, std::ios::binary | std::ios::out);
        if (!file.is_open())
        {
            throw std::invalid_argument("file path?");
        }
        buffer = new char[buffer_size];
        memset(buffer, 0, buffer_size);
    }

    virtual ~BinWriter()
    {
        flush();
        delete[] buffer;
        file.close();
    }

    bool canWrite()
    {
        return file.is_open() && file.good();
    }

    void writeBit(bool bit)
    {
        tryUpdateBuffer();
        if (bit)
        {
            buffer[index >> 3] = static_cast<unsigned char>(buffer[index >> 3] | (1 << (index % 8)));
        }
        index++;
    }

    void writeByte(char newByte)
    {
        std::streamsize used = (index % 8);
        std::streamsize left = 8 - used;

        tryUpdateBuffer();

        if (used != 0)
        {
            unsigned char first = newByte << used;
            buffer[index >> 3] = buffer[index >> 3] & ((1u << used) - 1u);
            buffer[index >> 3] = buffer[index >> 3] | first;
            index += left;
            tryUpdateBuffer();

            unsigned char nextByte = newByte >> left;
            buffer[index >> 3] = static_cast<char>(nextByte);
            index += used;
        } else
        {
            buffer[index >> 3] = newByte;
            index += 8;
        }
    }

    template<typename Type>
    void writeType(Type t)
    {
        constexpr size_t size = sizeof(Type);
        static_assert(size >= 1, "Type has to be 1 or more bytes.");
        char byte;
        for (size_t i = 0; i < size; ++i)
        {
            byte = (t & (0xff << (i * 8))) >> (i * 8);
            writeByte(byte);
        }
    }

    void flush()
    {
        std::streamsize size = index >> 3;
        if (index % 8 != 0)
        {
            size++;
        }
        file.write(buffer, size);
        index = 0;
        file.flush();
        memset(buffer, 0, buffer_size);
    }
};
#endif