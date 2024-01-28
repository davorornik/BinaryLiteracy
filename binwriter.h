#ifndef BINWRITER_H
#define BINWRITER_H

#include <type_traits>
#include <algorithm>
#include <fstream>
#include <cstring>

// @brief the Binary Writer class
// @tparam LSB means if it should write the byte bits from the right
// @tparam buffer_size is the internal buffer for the data
template<const bool LSB = true, const std::streamsize buffer_size = 4096>
class BinWriter {
private:
    std::ofstream *file;
    char *buffer;
    std::streamsize index;

    bool tryUpdateBuffer() {
        if (index == buffer_size << 3) {
            file->write(buffer, buffer_size);
            index = 0;
            std::memset(buffer, 0, buffer_size);
            return true;
        }
        return false;
    }

public:
    BinWriter() = delete;

    explicit BinWriter(const std::string &path) :
            index(0) {
        static_assert((buffer_size > 0) & !(buffer_size & (buffer_size - 1)),
                      "Template parameter must be a power of two.");

        file = new std::ofstream();
        file->open(path, std::ios::binary | std::ios::out);
        if (!file->is_open()) {
            throw std::invalid_argument("file path? " + path);
        }
        buffer = new char[buffer_size];
        std::memset(buffer, 0, buffer_size);
    }

    virtual ~BinWriter() {
        flush();
        delete[] buffer;
        file->close();
        delete file;
    }

    bool canWrite() {
        return file->is_open() && file->good();
    }

    void writeBit(bool bit) {
        tryUpdateBuffer();
        if (bit) {
            if (LSB) {
                buffer[index >> 3] = static_cast<unsigned char>(buffer[index >> 3] | (1 << (index % 8)));
            } else {
                buffer[index >> 3] = static_cast<unsigned char>(buffer[index >> 3] | (1 << (7 - index % 8)));
            }
        }
        index++;
    }

    void writeByte(char newByte) {
        std::streamsize used = (index % 8);
        std::streamsize left = 8 - used;
        tryUpdateBuffer();
        if (used != 0) {
            unsigned char first, nextByte;
            if (LSB) {
                first = newByte << used;
                buffer[index >> 3] = buffer[index >> 3] & ((1u << used) - 1u);
                buffer[index >> 3] = buffer[index >> 3] | first;
                index += left;
                tryUpdateBuffer();
                nextByte = newByte >> left;
            } else {
                first = (newByte >> used) & ((1u << left) - 1u);
                buffer[index >> 3] = buffer[index >> 3] | first;
                index += left;
                tryUpdateBuffer();
                nextByte = newByte << left;
            }
            buffer[index >> 3] = static_cast<char>(nextByte);
            index += used;
        } else {
            buffer[index >> 3] = newByte;
            index += 8;
        }
    }

    // Ensure T is a POD type to prevent reading complex structures that might have dynamic memory or non-trivial constructors.
    template<typename T>
    typename std::enable_if<std::is_pod<T>::value, bool>::type writeStruct(T &result) {
        if (!file->write(reinterpret_cast<char *>(&result), sizeof(T))) {
            return false;
        }
        return true;
    }


    void flush() {
        std::streamsize size = index >> 3;
        if (index % 8 != 0) {
            size++;
        }
        file->write(buffer, size);
        index = 0;
        file->flush();
        std::memset(buffer, 0, buffer_size);
    }
};

using BinWriterLSB = BinWriter<true>;
using BinWriterMSB = BinWriter<false>;

#endif