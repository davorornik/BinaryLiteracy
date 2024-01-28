#define CATCH_CONFIG_MAIN

#include "binreader.h"
#include "binwriter.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <cmath>


TEST_CASE("Alphabet Write and Read Integrity Test", "[integrity][MSB]") {
    std::vector<char> testBytes = {
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
            'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
    };
    auto *writer = new BinWriterMSB("outAlphabet.txt");
    for (const char c: testBytes) {
        writer->writeByte(c);
    }
    writer->flush();
    delete writer;
    auto *reader = new BinReaderMSB("outAlphabet.txt");
    std::vector<char> gottenBytes;
    while (reader->canRead()) {
        gottenBytes.push_back(reader->readByte());
    }
    delete reader;
    bool isSame = true;
    REQUIRE(testBytes.size() == gottenBytes.size());
    for (int i = 0; i < testBytes.size(); ++i) {
        if (testBytes[i] != gottenBytes[i]) {
            isSame = false;
            break;
        }
    }
    REQUIRE(isSame);
    std::remove("outAlphabet.txt");
}


TEST_CASE("Variable Bit Length Storage Test MSB", "[MSB]") {
    const int numbersToStore = 1024;
    auto *writer = new BinWriterMSB("outVarBitsMSB.txt");
    for (int number = 1; number <= numbersToStore; ++number) {
        int bitsNeeded = number == 1 ? 1 : static_cast<int>(std::floor(std::log2(number))) + 1;
        for (int bit = bitsNeeded - 1; bit >= 0; --bit) {
            writer->writeBit((number >> bit) & 1);
        }
    }
    writer->flush();
    delete writer;

    auto *reader = new BinReaderMSB("outVarBits.txt");
    std::vector<int> readNumbers;
    int currentNumber = 1;
    while (reader->canRead() && currentNumber <= numbersToStore) {
        int bitsNeeded = currentNumber == 1 ? 1 : static_cast<int>(std::floor(std::log2(currentNumber))) + 1;
        int number = 0;
        for (int bit = 0; bit < bitsNeeded; ++bit) {
            number = (number << 1) | reader->readBit();
        }
        readNumbers.push_back(number);
        currentNumber++;
    }
    delete reader;

    REQUIRE(numbersToStore == readNumbers.size());
    for (int i = 0; i < numbersToStore; ++i) {
        CHECK(i + 1 == readNumbers[i]);
    }
    std::remove("outVarBitsMSB.txt");
}

TEST_CASE("Variable Bit Length Storage Test LSB", "[LSB]") {
    const int numbersToStore = 1024;
    auto *writer = new BinWriterLSB("outVarBitsLSB.txt");
    for (int number = 1; number <= numbersToStore; ++number) {
        int bitsNeeded = number == 1 ? 1 : static_cast<int>(std::floor(std::log2(number))) + 1;
        for (int bit = bitsNeeded - 1; bit >= 0; --bit) {
            writer->writeBit((number >> bit) & 1);
        }
    }
    writer->flush();
    delete writer;

    auto *reader = new BinReaderLSB("outVarBitsLSB.txt");
    std::vector<int> readNumbers;
    int currentNumber = 1;
    while (reader->canRead() && currentNumber <= numbersToStore) {
        int bitsNeeded = currentNumber == 1 ? 1 : static_cast<int>(std::floor(std::log2(currentNumber))) + 1;
        int number = 0;
        for (int bit = 0; bit < bitsNeeded; ++bit) {
            number = (number << 1) | reader->readBit();
        }
        readNumbers.push_back(number);
        currentNumber++;
    }
    delete reader;

    REQUIRE(numbersToStore == readNumbers.size());
    for (int i = 0; i < numbersToStore; ++i) {
        CHECK(i + 1 == readNumbers[i]);
    }
    std::remove("outVarBitsLSB.txt");
}

TEST_CASE("struct test", "[LSB]") {
    struct TestStruct1 {
        int a;
        double b;
        char c;
    };
    struct TestStruct2 {
        int a: 8;
        int b: 8;
        int c: 32;
    };

    TestStruct1 ts1{10, 4.45, 'X'};
    TestStruct2 ts2{15, 14, 2048};

    auto *writer = new BinWriterMSB("structTest.txt");

    writer->writeStruct(ts1);
    writer->writeStruct(ts2);
    writer->flush();
    delete writer;

    auto *reader = new BinReaderMSB("structTest.txt");
    TestStruct1 ts1_read;
    TestStruct2 ts2_read;
    REQUIRE(reader->readStruct(ts1_read));
    REQUIRE(reader->readStruct(ts2_read));
    delete reader;
    CHECK(ts1.a == ts1_read.a);
    CHECK(ts1.b == ts1_read.b);
    CHECK(ts1.c == ts1_read.c);
    CHECK(ts2.a == ts2_read.a);
    CHECK(ts2.b == ts2_read.b);
    CHECK(ts2.c == ts2_read.c);
    std::remove("structTest.txt");
}
