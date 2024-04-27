#include "Global.hpp"
#include "Hunk.hpp"

/**
 * @param offset
 * @param length
 * @param count
 * @param bytes
 *
 * @brief Default constructor.
 *
 * @details Basically just copies all
 * the given arguments to its core.
 */
Hunk::Hunk(const u32 offset, const u16 length, const u16 count, std::vector<u8> *bytes)
{
    _offset = offset;
    _length = length;
    _count = count;
    _bytes = bytes;
}

/**
 * @param hunk
 *
 * @brief Copy constructor.
 */
Hunk::Hunk(Hunk &hunk)
{
    *this = hunk;

    // If it isn't a self assignment, we can
    // set the hunk's _bytes to nullptr so that
    // it doesn't delete this object's by 'accident'.
    if (&hunk != this)
        hunk._bytes = nullptr;
}

/**
 * @param hunk
 *
 * @brief Move constructor.
 */
Hunk::Hunk(Hunk &&hunk)
{
    *this = hunk;

    // Same thing as above.
    if (&hunk != this)
        hunk._bytes = nullptr;
}

/**
 * @brief Destructor.
 */
Hunk::~Hunk()
{
    delete _bytes;
}

/**
 * @param source
 *
 * @brief Assignment operator.
 */
Hunk &Hunk::operator=(const Hunk &source)
{
    // If it is a self assignment, there's no point.
    if (&source == this)
        return *this;

    _offset = source.offset();
    _length = source.length();
    _count = source.count();
    _bytes = source.bytes();
    return *this;
}

/**
 * @brief Returns the offset at
 * which the Hunk was/will be
 * located at.
 */
u32 Hunk::offset() const
{
    return _offset;
}

/**
 * @brief Returns the length of
 * the Hunk.
 */
u16 Hunk::length() const
{
    return _length;
}

/**
 * @brief Returns the count
 * of the bytes if it is an
 * 'RLE'.
 */
u16 Hunk::count() const
{
    return _count;
}

/**
 * @brief Returns the bytes
 * this Hunk is composed by.
 */
std::vector<u8> *Hunk::bytes() const
{
    return _bytes;
}

/**
 * @param destination
 *
 * @brief Writes the Hunk into
 * destination.
 *
 * @todo Maybe rename this into applyHunk ?
 *
 * @todo Delete toWrite at the end of the function.
 */
void Hunk::write(BigEdian *destination)
{
    if (_offset >= destination->size())
    {
        char offsetBuf[10];
        char destSize[10];

        sprintf(offsetBuf, "0x%X", _offset);
        sprintf(destSize, "0x%lX", destination->size());

        FATAL_ERROR("Specified offset: " << offsetBuf << " is bigger than file size: " << destSize << ".");
    }
    if ((_length == 0 && _count == 0) || _bytes == nullptr)
        return;
    // Superior to 16 MB.
    if (_offset > U24_MAX)
    {
        INFO("The patch *will not* consider data after 0xFFFFFF, skipping.");
        return;
    }

    destination->seek(_offset);

    // It isn't RLE.
    if (_length > 0)
    {
        destination->writeBytes(_bytes->data(), _length);
        return;
    }

    // It is RLE, so we iteratively
    // write the same byte over and over.
    u8 *toWrite = new u8[_count];

    for (size_t i = 0; i < _count; i++)
        toWrite[i] = _bytes->at(0);

    destination->writeBytes(toWrite, _count);
}

/**
 * @param destination
 *
 * @brief Writes the Hunk as IPS into
 * destination.
 */
void Hunk::asIPS(BigEdian *destination)
{
    if ((_length == 0 && _count == 0) || _bytes == nullptr)
        return;
    // Superior to 16 MB.
    if (_offset > U24_MAX)
    {
        INFO("The patch *will not* consider data after 0xFFFFFF, skipping.");
        return;
    }

    destination->writeU24(_offset);
    destination->writeU16(_length);

    // It is RLE.
    if (_length == 0)
    {
        destination->writeU16(_count);
        destination->writeU8(_bytes->at(0));
    }
    else
    {
        destination->writeBytes(_bytes->data(), _length);
    }
}

/**
 * @param ipsParser
 *
 * @brief Tries to parse a Hunk from
 * an IPS File.
 */
Hunk Hunk::fromIPS(BigEdian *ipsParser)
{
    u32 offset = ipsParser->readU24();
    u16 length = ipsParser->readU16();
    u16 count = 0;
    std::vector<u8> *bytes = new std::vector<u8>();

    // It is RLE.
    if (length == 0)
    {
        count = ipsParser->readU16();
        bytes->push_back(ipsParser->readU8());
    }
    else
    {
        const u8 *data = ipsParser->readBytes(length);

        bytes->reserve(length);

        for (size_t i = 0; i < length; i++)
            bytes->push_back(data[i]);
    }

    return Hunk(offset, length, count, bytes);
}

/**
 * @param source
 * @param target
 *
 * @brief Creates a Hunk from
 * the difference between source and target.
 */
Hunk Hunk::fromDiff(BigEdian *source, BigEdian *target)
{
    if (source->isEnd() || target->isEnd())
        FATAL_ERROR("Reached end of file(s).");

    u32 offset = 0;
    std::vector<u8> *diffBytes = new std::vector<u8>();
    u16 length = 0;
    u16 count = 0;
    bool isRLE = true;

    while (!source->isEnd() && !target->isEnd())
    {
        offset = source->tell();

        u8 byteSource = source->readU8();
        u8 byteTarget = target->readU8();

        if (byteSource == byteTarget)
            continue;

        for (size_t i = 0; i < U16_MAX; i++)
        {
            diffBytes->push_back(byteTarget);

            if (isRLE)
                count++;
            else
                length++;

            // It's suddenly not RLE anymore, so now
            // length has to be the one that gets udpated.
            if (isRLE && i > 0 && diffBytes->at(i - 1) != byteTarget)
            {
                isRLE = false;
                length = count;
                count = 0;
            }

            byteSource = source->readU8();
            byteTarget = target->readU8();

            // If they're the same, it's not a diff anymore.
            if (byteSource == byteTarget)
                break;
            if (source->isEnd() || target->isEnd())
                break;
        }

        break;
    }

    // There was no diff at all, so we can free
    // the diffBytes pointer.
    if (length == 0 && count == 0)
    {
        delete diffBytes;
        diffBytes = nullptr;
    }

    return Hunk(offset, length, count, diffBytes);
}

/**
 * @param out
 * @param hunk
 *
 * @brief Lets you print a Hunk
 * into std::cout, and any std::ostream.
 */
std::ostream &operator<<(std::ostream &out, Hunk &hunk)
{
    std::string offsetAsString = {""};
    std::string lengthAsString = {""};
    std::string countAsString = {""};
    std::string bytesAsString = {""};
    char hexBuffer[10];
    std::vector<u8> *bytesData = hunk.bytes();

    // Formatting to be 'readable', there's surely a better way,
    // but this one works just fine.
    sprintf(hexBuffer, "0x%X", hunk.offset());
    offsetAsString = hexBuffer;
    sprintf(hexBuffer, "0x%X", hunk.length());
    lengthAsString = hexBuffer;
    sprintf(hexBuffer, "0x%X", hunk.count());
    countAsString = hexBuffer;

    // To avoid segmentation faults trying to dereference it.
    if (bytesData == nullptr)
    {
        bytesAsString = "{}";
    }
    else if (hunk.length() == 0)
    {
        sprintf(hexBuffer, "0x%X", *(hunk.bytes()->data()));
        bytesAsString = hexBuffer;
    }
    else
    {
        bytesAsString = "{";
        u8 *bytes = hunk.bytes()->data();

        for (size_t i = 0, max = hunk.length(); i < max; i++)
        {
            sprintf(hexBuffer, "0x%X", bytes[i]);

            bytesAsString += hexBuffer;

            if (i + 1 < max)
                bytesAsString += ", ";
        }

        bytesAsString += "}";
    }

    out << "Hunk(" << offsetAsString << ", " << lengthAsString << ", " << countAsString << ", " << bytesAsString << ")";

    return out;
}