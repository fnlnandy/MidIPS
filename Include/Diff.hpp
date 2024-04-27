#ifndef GUARD_DIFF_HPP
#define GUARD_DIFF_HPP

#include <vector>
#include "Types.hpp"
#include "BigEdian.hpp"

class Diff
{
private:
    u32 _offset;
    u16 _length;
    u16 _count;
    std::vector<u8> *_diffBytes;

public:
    Diff(const u32 &offset, const u16 &length, const u16 &count, std::vector<u8> *diffBytes);
    Diff(Diff &diff);
    Diff(Diff &&diff);
    Diff &operator=(const Diff &source);

    u32 offset() const;
    u16 length() const;
    u16 count() const;
    std::vector<u8> *diffBytes() const;

    void asIPS(BigEdian *ipsFile);
    static Diff makeDiff(BigEdian *source, BigEdian *target);
};

#endif // GUARD_DIFF_HPP