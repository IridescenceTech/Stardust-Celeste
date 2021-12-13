#include "tester_assert.hpp"

#ifdef PSP
#include <pspkernel.h>
PSP_MODULE_INFO("Test", 0, 1, 0);
#endif

void test_all() {
    SC_TEST_ASSERT(true);
    SC_TEST_EQUAL(2 + 2, 4);
}

int main(int, char**) {
    test_all();

    return 0;
}