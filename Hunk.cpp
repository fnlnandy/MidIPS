#include "Global.hpp"
#include "Hunk.hpp"

Hunk::Hunk(const u32 offset, const u16 length, const u16 count, u8 *bytes)
{
    _offset = offset;
    _length = length;
    _count = count;
    _bytes = bytes;
}

Hunk::Hunk(Hunk &hunk)
{
    if (&hunk == this)
        return;

    *this = hunk;
    hunk._bytes = nullptr;
}

Hunk::Hunk(Hunk &&hunk)
{
    *this = hunk;
    hunk._bytes = nullptr;
}

Hunk &Hunk::operator=(const Hunk &source)
{
    _offset = source.offset();
    _length = source.length();
    _count = source.count();
    _bytes = source.bytes();
    return *this;
}

u32 Hunk::offset() const
{
    return _offset;
}

u16 Hunk::length() const
{
    return _length;
}

u16 Hunk::count() const
{
    return _count;
}

u8 *Hunk::bytes() const
{
    return _bytes;
}

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

    destination->seek(_offset);

    if (_length > 0)
    {
        destination->writeBytes(_bytes, _length);
        return;
    }

    u8 *toWrite = new u8[_count];

    for (size_t i = 0; i < _count; i++)
        toWrite[i] = _bytes[0];

    destination->writeBytes(toWrite, _count);
}

Hunk Hunk::makeHunk(BigEdian *ipsParser)
{
    u32 offset = ipsParser->readU24();
    u16 length = ipsParser->readU16();
    u16 count = 0;
    u8 *bytes = nullptr;

    if (length == 0)
    {
        count = ipsParser->readU16();
        bytes = ipsParser->readBytes(1);
    }
    else
    {
        bytes = ipsParser->readBytes(length);
    }

    return Hunk(offset, length, count, bytes);
}

std::ostream &operator<<(std::ostream &out, Hunk &hunk)
{
    std::string offsetAsString = {""};
    std::string lengthAsString = {""};
    std::string countAsString = {""};
    std::string bytesAsString = {""};
    char hexBuffer[10];

    //! Formatting to be 'readable', there's surely a better way,
    //! but this one works just fine.
    sprintf(hexBuffer, "0x%X", hunk.offset());
    offsetAsString = hexBuffer;
    sprintf(hexBuffer, "0x%X", hunk.length());
    lengthAsString = hexBuffer;
    sprintf(hexBuffer, "0x%X", hunk.count());
    countAsString = hexBuffer;

    if (hunk.length() == 0)
    {
        sprintf(hexBuffer, "0x%X", *hunk.bytes());
        bytesAsString = hexBuffer;
    }
    else
    {
        bytesAsString = "{";
        u8 *bytes = hunk.bytes();

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