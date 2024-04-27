#ifndef GUARD_GLOBAL_HPP
#define GUARD_GLOBAL_HPP

#include <iostream>

#define FATAL_ERROR(msg)  \
    {                     \
        std::cerr << msg; \
        exit(1);          \
    }

#endif // GUARD_GLOBAL_HPP