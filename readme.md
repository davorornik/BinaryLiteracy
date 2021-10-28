# A Binary reader and writer

The binary reader and writer are both a single header implementation. Both use a buffer to minimise the number of system calls. The default value is 4096, but can be adjusted to any power of 2. Little endian is assumed for the bit order. You can call
readBit(), readByte() and readType() interchangeably. The same applies for writeBit(bool), writeByte(char) and writeType(Type).