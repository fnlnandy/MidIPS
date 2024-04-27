#include "Global.hpp"
#include "Diff.hpp"

Diff::Diff(const u32 &offset, const u16 &length, const u16 &count, std::vector<u8> *diffBytes)
{
    _offset = offset;
    _length = length;
    _count = count;
    _diffBytes = diffBytes;
}

Diff::Diff(Diff &diff)
{
    *this = diff;

    diff._diffBytes = nullptr;
}

Diff::Diff(Diff &&diff)
{
    *this = diff;

    diff._diffBytes = nullptr;
}

Diff &Diff::operator=(const Diff &source)
{
    if (&source == this)
        return *this;

    _offset = source.offset();
    _length = source.length();
    _count = source.count();
    _diffBytes = source.diffBytes();

    return *this;
}

u32 Diff::offset() const
{
    return _offset;
}

u16 Diff::length() const
{
    return _length;
}

u16 Diff::count() const
{
    return _count;
}

std::vector<u8> *Diff::diffBytes() const
{
    return _diffBytes;
}

void Diff::asIPS(BigEdian *ipsFile)
{
    ipsFile->writeU24(_offset);
    ipsFile->writeU16(_length);

    if (_length == 0)
    {
        ipsFile->writeU16(_count);
        ipsFile->writeU8(_diffBytes->at(0));
    }
    else
    {
        ipsFile->writeBytes(_diffBytes->data(), _length);
    }
}

Diff Diff::makeDiff(BigEdian *source, BigEdian *target)
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

    return Diff(offset, length, count, diffBytes);
}