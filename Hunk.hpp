#ifndef GUARD_HUNK_HPP
#define GUARD_HUNK_HPP

#include <iostream>
#include "Types.hpp"
#include "BigEdian.hpp"

class Hunk
{
private:
    u32 _offset;
    u16 _length;
    u16 _count;
    u8 *_bytes;

public:
    Hunk(const u32 offset, const u16 length, const u16 count, u8 *bytes);
    Hunk(Hunk &hunk);
    Hunk(Hunk &&hunk);
    Hunk &operator=(const Hunk &source);

    u32 offset() const;
    u16 length() const;
    u16 count() const;
    u8 *bytes() const;

    void write(BigEdian *destination);
    static Hunk makeHunk(BigEdian *ipsParser);
};

std::ostream &operator<<(std::ostream &out, Hunk &hunk);

#endif // GUARD_HUNK_HPP