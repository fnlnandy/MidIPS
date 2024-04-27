#ifndef GUARD_BIG_EDIAN_HPP
#define GUARD_BIG_EDIAN_HPP

#include <cstdio>
#include <cstdlib>
#include <string>
#include "Types.hpp"

class BigEdian
{
private:
    std::FILE *_fileBuffer;
    std::string _fileName;
    size_t _size;

public:
    BigEdian(const std::string &fileName, const std::string &mode);
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
    size_t tell();
    size_t size();
    bool isEnd();
};

#endif // GUARD_BIG_EDIAN_HPP