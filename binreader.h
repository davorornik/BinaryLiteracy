#ifndef BINREADER_H
#define BINREADER_H

#include <type_traits>
#include <filesystem>
#include <fstream>
#include <cstring>

// @brief the Binary Reader class
// @tparam LSB means if it should pick the byte bits from the right
// @tparam buffer_size is the internal buffer for the data
template<const bool LSB = false, const std::streamsize buffer_size = 4096>
class BinReader {
private:
    std::ifstream *file;
    std::uintmax_t to_read;
    std::streamsize last_read;
    std::streamsize index;
    char *buffer;
    bool lastBit;
    bool eof;

    void tryUpdateBuffer() {
        if (index == last_read << 3) {
            index = 0;
            std::memset(buffer, 0, buffer_size);
            last_read = file->read(buffer, buffer_size).gcount();
            if (last_read == 0) {
                eof = true;
            }
            to_read -= last_read;
        }
    }


public:
    BinReader() = delete;

    // Assuming that ofstream is oppened as std::ios:binnary and std::ios::in
    explicit BinReader(std::ifstream &ifstream) :
            last_read(0),
            index(0),
            lastBit(false),
            eof(false) {
        static_assert((buffer_size > 0) & !(buffer_size & (buffer_size - 1)),
                      "Template parameter must be a power of two.");
        file = &ifstream;
        to_read = file->tellg();
        file->seekg(0, std::ios::beg);
        if (!file->is_open())
            throw std::invalid_argument("file stream?");
        buffer = new char[buffer_size];
        std::memset(buffer, 0, buffer_size);
    }

    explicit BinReader(const std::string &path) :
            last_read(0),
            index(0),
            lastBit(false),
            eof(false) {
        static_assert((buffer_size > 0) & !(buffer_size & (buffer_size - 1)),
                      "Template parameter must be a power of two.");
        to_read = std::filesystem::file_size(path);
        file = new std::ifstream();
        file->open(path, std::ios::binary | std::ios::in);
        if (!file->is_open())
            throw std::invalid_argument("file path? " + path);
        buffer = new char[buffer_size];
        std::memset(buffer, 0, buffer_size);
    }

    virtual ~BinReader() {
        delete[] buffer;
        file->close();
        delete file;
    }

    bool canRead() const {
        if (to_read > 0) {
            return true;
        } else {
            return !eof && index < (last_read << 3);
        }
    }

    bool readBit() {
        tryUpdateBuffer();
        if (LSB) {
            lastBit = (buffer[index >> 3] >> (index % 8)) & 1;
        } else {
            lastBit = (buffer[index >> 3] >> (7 - index % 8)) & 1;
        }
        index++;
        return lastBit;
    }

    char readByte() {
        tryUpdateBuffer();
        std::streamsize used = index % 8;
        std::streamsize left = 8 - used;
        unsigned int newByte;
        newByte = static_cast<unsigned char>(buffer[index >> 3]);
        if (used != 0) {
            unsigned int nextByte;
            if (LSB) {
                newByte = newByte >> used;
                index += left;
                tryUpdateBuffer();
                nextByte = static_cast<unsigned char>(buffer[index >> 3]);
                nextByte = nextByte & ((1u << used) - 1u);
                nextByte = nextByte << left;
                newByte = newByte | nextByte;
                index += used;
                lastBit = (newByte >> left);
            } else {
                newByte = newByte & ((1u << left) - 1u);
                newByte = newByte << used;
                index += left;
                tryUpdateBuffer();
                nextByte = static_cast<unsigned char>(buffer[index >> 3]);
                nextByte = (nextByte >> left) & ((1u << used) - 1u);
                newByte = newByte | nextByte;
                index += used;
                lastBit = (newByte >> left);
            }
        } else {
            index += 8;
            lastBit = (newByte >> 7);
        }

        return static_cast<char>(newByte);
    }

    // Ensure T is a POD type to prevent reading complex structures that might have dynamic memory or non-trivial constructors.
    template<typename T>
    typename std::enable_if<std::is_pod<T>::value, bool>::type readStruct(T &result) {
        if (!file->read(reinterpret_cast<char *>(&result), sizeof(T))) {
            return false;
        }
        return true;
    }

    bool getLastBit() const {
        return lastBit;
    }
};

using BinReaderLSB = BinReader<true>;
using BinReaderMSB = BinReader<false>;

#endif

