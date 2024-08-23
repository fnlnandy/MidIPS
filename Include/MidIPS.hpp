#ifndef GUARD_GLOBAL_HPP
#define GUARD_GLOBAL_HPP

#include <iostream>

#define FATAL_ERROR(msg)          \
    {                             \
        std::cerr << msg << "\n"; \
        exit(1);                  \
    }

#define INFO(msg)                             \
    {                                         \
        std::cout << "INFO: " << msg << "\n"; \
    }

#define BITS_IN(dataType) (sizeof(dataType) * 8)

#ifndef NDEBUG
#define DEBUG(msg)                \
    {                             \
        std::cout << msg << "\n"; \
    }
#else
#define DEBUG(msg)
#endif // NDEBUG

#endif // GUARD_GLOBAL_HPP