#include "Global.hpp"
#include "BigEdian.hpp"

/**
 * @param fileName
 * @param mode
 *
 * @brief Constructor that tries
 * to open fileName in mode.
 */
BigEdian::BigEdian(const std::string &fileName, const std::ios_base::openmode &mode)
{
    _fileBuffer.open(fileName, mode);

    if (!_fileBuffer.is_open())
        FATAL_ERROR("Unable to open '" << fileName << "' for reading.");

    _fileName = fileName;
    _fileBuffer.seekg(0, std::ios::end);

    // If the seeking failed for whatever reason.
    if (_fileBuffer.fail())
        FATAL_ERROR("Errors occurred while reading '" << fileName << "'.");

    _size = _fileBuffer.tellg();
    _fileBuffer.clear(); // Clearing any flag that could've been set.
    _fileBuffer.seekg(0, std::ios::beg);
}

/**
 * @brief Destructor.
 */
BigEdian::~BigEdian()
{
    _fileBuffer.close();
}

/**
 * @brief Reads an 8-bit
 * unsigned integer.
 *
 * @returns The read u8.
 */
u8 BigEdian::readU8()
{
    if (isEnd())
        FATAL_ERROR("Reached end of file: '" << _fileName << "'.");

    return _fileBuffer.get();
}

/**
 * @brief Reads an array
 * of 8-bit unsigned integer(s).
 *
 * @returns The read u8 *.
 */
u8 *BigEdian::readBytes(const size_t &length)
{
    u8 *readArray = new u8[length];

    for (size_t i = 0; i < length; i++)
        readArray[i] = readU8();

    return readArray;
}

/**
 * @brief Reads a 16-bit
 * unsigned integer.
 *
 * @returns The read u16.
 */
u16 BigEdian::readU16()
{
    u16 retVal = readU8();
    retVal <<= BITS_IN(u8);
    retVal |= readU8();

    return retVal;
}

/**
 * @brief Reads a 24-bit
 * unsigned integer.
 *
 * @returns The read u24, but
 * promoted to u32 as that's easier.
 */
u32 BigEdian::readU24()
{
    u32 retVal = readU16();
    retVal <<= BITS_IN(u8);
    retVal |= readU8();

    return retVal;
}

/**
 * @brief Reads a 32-bit
 * unsigned integer.
 *
 * @returns The read u32.
 */
u32 BigEdian::readU32()
{
    u32 retVal = readU16();
    retVal <<= BITS_IN(u16);
    retVal |= readU16();

    return retVal;
}

/**
 * @param toWrite
 *
 * @brief Writes an 8-bit
 * unsigned integer.
 */
void BigEdian::writeU8(const u8 &toWrite)
{
    _fileBuffer.put(toWrite);
    _fileBuffer.flush();
}

/**
 * @param toWrite
 * @param length
 *
 * @brief Writes an array
 * of 8-bit unsigned integers.
 */
void BigEdian::writeBytes(const u8 *toWrite, const size_t &length)
{
    for (size_t i = 0; i < length; i++)
        writeU8(toWrite[i]);
}

/**
 * @param toWrite
 *
 * @brief Writes a 16-bit
 * unsigned integer.
 */
void BigEdian::writeU16(const u16 &toWrite)
{
    u8 hi = (toWrite >> BITS_IN(u8));
    u8 lo = (toWrite);

    writeU8(hi);
    writeU8(lo);
}

/**
 * @param toWrite
 *
 * @brief Writes a 24-bit
 * unsigned integer, but passed
 * as a 32-bit unsigned integer.
 *
 * @details It takes an u32 as that's
 * faster, but it only cares about the lower
 * 24 bits.
 */
void BigEdian::writeU24(const u32 &toWrite)
{
    u16 hi = (toWrite >> BITS_IN(u8));
    u8 lo = (toWrite);

    writeU16(hi);
    writeU8(lo);
}

/**
 * @param toWrite
 *
 * @brief Writes a 32-bit unsigned integer.
 */
void BigEdian::writeU32(const u32 &toWrite)
{
    u16 hi = (toWrite >> BITS_IN(u16));
    u16 lo = (toWrite);

    writeU16(hi);
    writeU16(lo);
}

/**
 * @brief Flushes the changes, i.e.
 * writes the buffer into the real file.
 */
void BigEdian::flush()
{
    _fileBuffer.flush();
}

/**
 * @param offset
 *
 * @brief Seeks a certain position.
 */
void BigEdian::seek(const size_t offset)
{
    _fileBuffer.seekg(offset, std::ios::beg);
    _fileBuffer.seekp(offset, std::ios::beg);
}

/**
 * @brief Tells our current position.
 *
 * @returns The get position, as we rarely
 * need this in write mode.
 *
 * @todo Make this const.
 */
size_t BigEdian::tell()
{
    return _fileBuffer.tellg();
}

/**
 * @brief Returns the size of the file.
 *
 * @todo Make this const.
 */
size_t BigEdian::size()
{
    return _size;
}

/**
 * @brief Returns whether we're at
 * the end of the file or not.
 *
 * @todo Make this const.
 */
bool BigEdian::isEnd()
{
    return _fileBuffer.eof();
}