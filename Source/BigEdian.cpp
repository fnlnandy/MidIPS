#include "MidIPS.hpp"
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
    m_fileBuffer.open(fileName, mode);

    if (!m_fileBuffer.is_open())
        FATAL_ERROR("Unable to open '" << fileName << "' for reading.");

    m_fileName = fileName;
    m_fileBuffer.seekg(0, std::ios::end);

    // If the seeking failed for whatever reason.
    if (m_fileBuffer.fail())
        FATAL_ERROR("Errors occurred while reading '" << fileName << "'.");

    m_size = m_fileBuffer.tellg();
    m_fileBuffer.clear(); // Clearing any flag that could've been set.
    m_fileBuffer.seekg(0, std::ios::beg);
}

/**
 * @brief Destructor.
 */
BigEdian::~BigEdian()
{
    m_fileBuffer.close();
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
        FATAL_ERROR("Reached end of file: '" << m_fileName << "'.");

    return m_fileBuffer.get();
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
    m_fileBuffer.put(toWrite);
    m_fileBuffer.flush();
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
    m_fileBuffer.flush();
}

/**
 * @param offset
 *
 * @brief Seeks a certain position.
 */
void BigEdian::seek(const size_t offset)
{
    m_fileBuffer.seekg(offset, std::ios::beg);
    m_fileBuffer.seekp(offset, std::ios::beg);
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
    return m_fileBuffer.tellg();
}

/**
 * @brief Returns the size of the file.
 *
 * @todo Make this const.
 */
size_t BigEdian::size()
{
    return m_size;
}

/**
 * @brief Returns whether we're at
 * the end of the file or not.
 *
 * @todo Make this const.
 */
bool BigEdian::isEnd()
{
    return m_fileBuffer.eof();
}