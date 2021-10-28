#ifndef BINREADER_H
#define BINREADER_H
#include <fstream>
#include <filesystem>
#include <concepts>

template<typename std::streamsize buffer_size = 4096>
class BinReader {
private:
    std::ifstream file;
    std::uintmax_t to_read;
    std::streamsize last_read;
    std::streamsize index;
    char *buffer;
    bool lastBit;
    bool eof;
    void tryUpdateBuffer()
    {
        if (index == last_read << 3)
        {
            index = 0;
            memset(buffer, 0, buffer_size);
            last_read = file.read(buffer, buffer_size).gcount();
            if (last_read == 0)
            {
                eof = true;
            }
            to_read -= last_read;
        }
    }

public:
    BinReader() = delete;
    explicit BinReader(const std::string &path) :
            last_read(0),
            index(0),
            lastBit(false),
            eof(false)
    {
        static_assert((buffer_size > 0) & !(buffer_size & (buffer_size - 1)),
                      "Template parameter must be a power of two.");

        to_read = std::filesystem::file_size(path);
        file.open(path, std::ios::binary | std::ios::in);
        if (!file.is_open())
            throw std::invalid_argument("file path?");
        buffer = new char[buffer_size];
        memset(buffer, 0, buffer_size);
    }
    virtual ~BinReader()
    {
        delete[] buffer;
        file.close();
    }

    bool canRead() const
    {
        if (to_read > 0)
        {
            return true;
        } else
        {
            return !eof && index < (last_read << 3);
        }
    }

    bool readBit()
    {
        tryUpdateBuffer();
        lastBit = (buffer[index >> 3] >> (index % 8)) & 1;
        index++;
        return lastBit;
    }

    char readByte()
    {
        tryUpdateBuffer();
        std::streamsize used = index % 8;
        std::streamsize left = 8 - used;
        unsigned int newByte;
        newByte = static_cast<unsigned char>(buffer[index >> 3]);
        if (used != 0)
        {
            unsigned int nextByte;
            newByte = newByte >> used;
            index += left;
            tryUpdateBuffer();
            nextByte = static_cast<unsigned char>(buffer[index >> 3]);
            nextByte = nextByte & ((1u << used) - 1u);
            nextByte = nextByte << left;
            newByte = newByte | nextByte;
            index += used;
        } else
        {
            index += 8;
        }
        lastBit = (newByte >> 7);
        return static_cast<char>(newByte);
    }

    template<typename Type>
    Type readType()
    {
        constexpr size_t size = sizeof(Type);
        static_assert(size >= 1, "Type has to be 1 or more bytes.");
        Type t = 0;
        for (size_t i = 0; i < size; ++i)
        {
            t = t | (static_cast<Type>(readByte()) << (8 * i));
        }
        return t;
    }

    bool getLastBit() const
    {
        return lastBit;
    }
};
#endif

