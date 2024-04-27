#include "Global.hpp"
#include "BigEdian.hpp"

BigEdian::BigEdian(const std::string &fileName, const std::ios_base::openmode &mode)
{
    _fileBuffer.open(fileName, mode);

    if (!_fileBuffer.is_open())
        FATAL_ERROR("Unable to open '" << fileName << "' for reading.");

    _fileName = fileName;
    _fileBuffer.seekg(0, std::ios::end);

    if (_fileBuffer.fail())
        FATAL_ERROR("Errors occurred while reading '" << fileName << "'.");

    _size = _fileBuffer.tellg();
    _fileBuffer.clear();
    _fileBuffer.seekg(0, std::ios::beg);
}

BigEdian::~BigEdian()
{
    _fileBuffer.close();
}

u8 BigEdian::readU8()
{
    if (isEnd())
        FATAL_ERROR("Reached end of file: '" << _fileName << "'.");

    return _fileBuffer.get();
}

u8 *BigEdian::readBytes(const size_t &length)
{
    u8 *readArray = new u8[length];

    for (size_t i = 0; i < length; i++)
        readArray[i] = readU8();

    return readArray;
}

u16 BigEdian::readU16()
{
    u16 retVal = readU8();
    retVal <<= BITS_IN(u8);
    retVal |= readU8();

    return retVal;
}

u32 BigEdian::readU24()
{
    u32 retVal = readU16();
    retVal <<= BITS_IN(u8);
    retVal |= readU8();

    return retVal;
}

u32 BigEdian::readU32()
{
    u32 retVal = readU16();
    retVal <<= BITS_IN(u16);
    retVal |= readU16();

    return retVal;
}

void BigEdian::writeU8(const u8 &toWrite)
{
    DEBUG(__PRETTY_FUNCTION__ << " : " << (unsigned int)toWrite);
    _fileBuffer.put(toWrite);
    _fileBuffer.flush();
}

void BigEdian::writeBytes(const u8 *toWrite, const size_t &length)
{
    for (size_t i = 0; i < length; i++)
    {
        writeU8(toWrite[i]);
    }
}

void BigEdian::writeU16(const u16 &toWrite)
{
    DEBUG(__PRETTY_FUNCTION__ << " : " << toWrite);

    u8 hi = (toWrite >> BITS_IN(u8));
    u8 lo = (toWrite);

    writeU8(hi);
    writeU8(lo);
}

void BigEdian::writeU24(const u32 &toWrite)
{
    DEBUG(__PRETTY_FUNCTION__ << " : " << toWrite);

    u16 hi = (toWrite >> BITS_IN(u8));
    u8 lo = (toWrite);

    writeU16(hi);
    writeU8(lo);
}

void BigEdian::writeU32(const u32 &toWrite)
{
    DEBUG(__PRETTY_FUNCTION__ << " : " << toWrite);

    u16 hi = (toWrite >> BITS_IN(u16));
    u16 lo = (toWrite);

    writeU16(hi);
    writeU16(lo);
}

void BigEdian::flush()
{
    _fileBuffer.flush();
}

void BigEdian::seek(const size_t offset)
{
    _fileBuffer.seekg(offset, std::ios::beg);
    _fileBuffer.seekp(offset, std::ios::beg);
}

size_t BigEdian::tell()
{
    return _fileBuffer.tellg();
}

size_t BigEdian::size()
{
    return _size;
}

bool BigEdian::isEnd()
{
    return _fileBuffer.eof();
}