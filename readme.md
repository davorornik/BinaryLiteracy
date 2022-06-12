# A Binary reader and writer

The binary reader and writer are both a single header implementation. The class template takes an argument to set the
bit order in the byte. The default is to read from the left to the
right. Both use a buffer to minimise the number of system calls. The default value is 4096, but can be adjusted to any
power of 2.
readBit() and readByte() can be used interchangeably. The same applies for writeBit(bool) and writeByte(char).