#include "Global.hpp"
#include "Hunk.hpp"

Hunk::Hunk(const u32 offset, const u16 length, const u16 count, std::vector<u8> *bytes)
{
    _offset = offset;
    _length = length;
    _count = count;
    _bytes = bytes;
}

Hunk::Hunk(Hunk &hunk)
{
    *this = hunk;

    if (&hunk != this)
        hunk._bytes = nullptr;
}

Hunk::Hunk(Hunk &&hunk)
{
    *this = hunk;

    if (&hunk != this)
        hunk._bytes = nullptr;
}

Hunk::~Hunk()
{
    delete _bytes;
}

Hunk &Hunk::operator=(const Hunk &source)
{
    if (&source == this)
        return *this;

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

std::vector<u8> *Hunk::bytes() const
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
        destination->writeBytes(_bytes->data(), _length);
        return;
    }

    u8 *toWrite = new u8[_count];

    for (size_t i = 0; i < _count; i++)
        toWrite[i] = _bytes->at(0);

    destination->writeBytes(toWrite, _count);
}

void Hunk::asIPS(BigEdian *destination)
{
    destination->writeU24(_offset);
    destination->writeU16(_length);

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

Hunk Hunk::makeHunk(BigEdian *ipsParser)
{
    u32 offset = ipsParser->readU24();
    u16 length = ipsParser->readU16();
    u16 count = 0;
    std::vector<u8> *bytes = new std::vector<u8>();

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

            if (isRLE && i > 0 && diffBytes->at(i - 1) != byteTarget)
            {
                isRLE = false;
                length = count;
                count = 0;
            }

            byteSource = source->readU8();
            byteTarget = target->readU8();

            if (byteSource == byteTarget)
                break;
            if (source->isEnd() || target->isEnd())
                break;
        }

        break;
    }

    return Hunk(offset, length, count, diffBytes);
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