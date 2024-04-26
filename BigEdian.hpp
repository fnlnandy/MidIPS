#ifndef GUARD_BIG_EDIAN_HPP
#define GUARD_BIG_EDIAN_HPP

#include <cstdio>
#include "Types.hpp"

class BigEdian
{
private:
    std::FILE *_fileBuffer;

public:
    BigEdian(std::FILE *file);
    ~BigEdian();
    u8 readU8();
    u16 readU16();
    u32 readU24();
    u32 readU32();
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