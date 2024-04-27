#ifndef GUARD_BIG_EDIAN_HPP
#define GUARD_BIG_EDIAN_HPP

#include <cstdio>
#include <cstdlib>
#include "Types.hpp"

class BigEdian
{
private:
    std::FILE *_fileBuffer;
    size_t _size;

public:
    BigEdian(std::FILE *file);
    ~BigEdian();
    u8 readU8();
    u8 *readBytes(const size_t &length);
    u16 readU16();
    u32 readU24();
    u32 readU32();
    void writeU8(const u8 &toWrite);
    void writeBytes(const u8 *toWrite, const size_t &length);
    void writeU16(const u16 &toWrite);
    void writeU24(const u32 &toWrite);
    void writeU32(const u32 &toWrite);
    void flush();
    void seek(const size_t offset);
    size_t size();
    bool isEnd();
};

enum
{
    BITS_u8 = 8,
    BITS_u16 = 16,
    BITS_u24 = 24,
    BITS_u32 = 32
};

#define BITS_IN(x) (BITS_##x)

#endif // GUARD_BIG_EDIAN_HPP