#include <iostream>
#include <stdexcept>
#include <string>

#define SC_TEST_ASSERT(condition)                                              \
    {                                                                          \
        if (!(condition)) {                                                    \
            throw std::runtime_error("TEST FAILED!\n");                        \
        }                                                                      \
    }

#define SC_TEST_EQUAL(x, y)                                                    \
    {                                                                          \
        if ((x) != (y)) {                                                      \
            throw std::runtime_error("TEST FAILED!\n");                        \
        }                                                                      \
    }
