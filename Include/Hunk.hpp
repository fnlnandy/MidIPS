#ifndef GUARD_HUNK_HPP
#define GUARD_HUNK_HPP

#include <iostream>
#include <vector>
#include "Types.hpp"
#include "BigEdian.hpp"

class Hunk
{
private:
    u32 m_offset;
    u16 m_length;
    u16 m_count;
    std::vector<u8> *m_bytes;

public:
    Hunk(const u32 offset, const u16 length, const u16 count, std::vector<u8> *bytes);
    Hunk(Hunk &hunk);
    Hunk(Hunk &&hunk);
    ~Hunk();
    Hunk &operator=(const Hunk &source);

    u32 offset() const;
    u16 length() const;
    u16 count() const;
    std::vector<u8> *bytes() const;

    void write(BigEdian *destination);
    void asIPS(BigEdian *destination);
    static Hunk fromIPS(BigEdian *ipsParser);
    static Hunk fromDiff(BigEdian *source, BigEdian *target);
};

std::ostream &operator<<(std::ostream &out, Hunk &hunk);

#endif // GUARD_HUNK_HPP