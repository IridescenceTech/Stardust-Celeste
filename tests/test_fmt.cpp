#include "../include/fmt/core.h"
#include "tester_assert.hpp"

#ifdef PSP
#include <pspkernel.h>
PSP_MODULE_INFO("Test", 0, 1, 0);
#endif

void test_all() {
    SC_TEST_ASSERT(true);
    fmt::print("Hello, {}!\n", "world");
}

int main(int, char **) {
    test_all();

    return 0;
}